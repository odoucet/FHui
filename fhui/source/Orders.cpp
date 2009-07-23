#include "StdAfx.h"
#include "Form1.h"
#include "Enums.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// Order Template

void Form1::GenerateTemplate()
{
    m_OrderList->Clear();

    GenerateCombat();
    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();

    OrderTemplate->Lines = m_OrderList->ToArray();
}

void Form1::GenerateCombat()
{
    m_OrderList->Add("START COMBAT");
    m_OrderList->Add("");

    // TODO: Sort locations by relevance (effective tonnage)

    for each ( StarSystem^ system in m_GameData->GetStarSystems() )
    {
        if ( system->ShipsOwned->Count > 0 )
        {
            GenerateCombatInfo( system );
            for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            {
                plugin->GenerateCombat( m_OrderList, system );
            }

            m_OrderList->Add("");
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateCombatInfo(StarSystem^ system)
{
    Alien^ player = m_GameData->GetSpecies();

    String^ sysInfo = system->PrintColonies(-1, player );
    if ( String::IsNullOrEmpty(sysInfo) )
    {
        sysInfo = "empty";
    }

    m_OrderList->Add(
        String::Format("; Location: {0} ({1})", system->PrintLocation(), sysInfo ) );

    List<String^>^ MyShipInfo = gcnew List<String^>;
    List<String^>^ AlienShipInfo = gcnew List<String^>;

    for each ( Ship^ ship in system->Ships )
    {
        String^ shipLoc;
        if (ship->Location == SHIP_LOC_DEEP_SPACE )
        {
            shipLoc = "Deep";
        }
        else
        {
            shipLoc = ship->PrintLocation();
        }

        if ( ship->Owner == player )
        {
            MyShipInfo->Add(
                String::Format(";    {0} ({1}) [{2}]", ship->PrintClassWithName(), shipLoc, ship->PrintCargo() ) );
        }
        else
        {
            AlienShipInfo->Add(
                String::Format(";    {0} ({1})    SP {2}", ship->PrintClassWithName(), shipLoc, ship->Owner->Name ) );
        }
    }
    m_OrderList->AddRange( MyShipInfo );
    m_OrderList->Add( String::Format("; Alien ships: {0}", ( AlienShipInfo->Count ? "" : "none" ) ) );
    m_OrderList->AddRange( AlienShipInfo );
}

void Form1::GeneratePreDeparture()
{
    m_OrderList->Add("START PRE-DEPARTURE");

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
            plugin->GeneratePreDeparture(m_OrderList, ship);

        m_OrderList->Add("");

        for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
            plugin->GeneratePreDeparture(m_OrderList, colony);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
        {
            plugin->GenerateJumps(m_OrderList, ship);

            if( ship->Command && ship->Command->Type == Ship::OrderType::Jump )
            {
                m_OrderList->Add( String::Format("    Jump {0}, {1}  ; [{2}] -> [{3}]; Mishap: {4:F2}%",
                    ship->PrintClassWithName(),
                    ship->Command->PrintJumpDestination(),
                    ship->System->PrintLocation(),
                    ship->Command->JumpTarget->PrintLocation(),
                    ship->System->CalcMishap(
                        ship->Command->JumpTarget,
                        ship->Owner->TechLevelsAssumed[TECH_GV],
                        ship->Age) ) );
            }
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");

    BudgetTracker ^budget = gcnew BudgetTracker(m_OrderList, m_GameData->GetCarriedEU());
    m_OrderList->Add("; +" + budget->GetTotalBudget().ToString() + " EUs carried" );

    // Mark ships for upgrade
    for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
    {
        if( ship->Command != nullptr &&
            (   ship->Command->Type == Ship::OrderType::Upgrade ||
                ship->Command->Type == Ship::OrderType::Recycle ) )
        {
            ship->Command->PlanetNum = -1;
        }
    }

    // Sort colonies for production
    m_GameData->GetSpecies()->SortColoniesByProdOrder();

    // Generate production template for each colony
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
    {
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);

        int euCarried = budget->GetTotalBudget();
        budget->SetColony(colony);

        // Production summary
        String ^prodSummary = String::Format("  ; {0} +{1} = {2}",
            colony->EUAvail, euCarried, budget->GetTotalBudget());
        if( colony->PlanetType == PLANET_COLONY )
            prodSummary += "  max=" + budget->GetAvailBudget().ToString();
        m_OrderList->Add( prodSummary );

        // First RECYCLE all ships
        GenerateProductionRecycle(colony, budget);

        // Launch plugin actions
        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget);

        // Now try to UPGRADE ships here
        GenerateProductionUpgrade(colony, budget);

        // Automatic spendings summary
        if( colony->PlanetType == PLANET_COLONY )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0} (total {1})",
                budget->GetAvailBudget(),
                budget->GetTotalBudget() ) );
        }
        else if( colony->PlanetType == PLANET_HOME )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0}", budget->GetAvailBudget() ) );
        }
    }

    m_OrderList->Add("");
    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        if( ship->Command != nullptr &&
            ship->Command->Type == Ship::OrderType::Recycle &&
            ship->Command->PlanetNum == -1 )    // not yet done
        {
            int value = Calculators::ShipRecycleValue(ship->Age, ship->OriginalCost);
            m_OrderList->Add( String::Format("    Recycle {0}  ; [A] +{1} EU",
                ship->PrintClassWithName(),
                value) );
            budget->Recycle(value);

            // Mark the recycle was done on this planet.
            ship->Command->PlanetNum = colony->PlanetNum;
        }
    }
}

void Form1::GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        if( ship->Command != nullptr &&
            ship->Command->Type == Ship::OrderType::Upgrade &&
            ship->Command->PlanetNum == -1 )    // not yet done
        {
            int value = Calculators::ShipUpgradeCost(ship->Age, ship->OriginalCost);
            bool doUpgrade = true;
            // Does this colony has budget for upgrade?
            // If budget exceeded search for another colony
            // that could handle the upgrade.
            if( value > budget->GetAvailBudget() )
            {
                for each( Colony ^otherCol in colony->System->ColoniesOwned )
                {
                    if( otherCol->CanProduce &&
                        otherCol->ProductionOrder > colony->ProductionOrder &&
                        otherCol->GetMaxProductionBudget() >= value )
                    {   // Other candidate found, don't upgrade on this colony.
                        doUpgrade = false;
                    }
                }
            }

            // Even if budget for upgrade not available, but no other
            // colony able to do the upgrade found, add the command.
            if( doUpgrade )
            {
                m_OrderList->Add( String::Format("    Upgrade {0}  ; [A] -{1} EU",
                    ship->PrintClassWithName(),
                    value) );
                budget->Spend(value);

                // Mark the upgrade was done on this planet.
                ship->Command->PlanetNum = colony->PlanetNum;
            }
        }
    }
}

void Form1::GeneratePostArrival()
{
    m_OrderList->Add("START POST-ARRIVAL");
    m_OrderList->Add("");

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateStrikes()
{
    m_OrderList->Add("START STRIKES");
    m_OrderList->Add("");

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

} // end namespace FHUI
