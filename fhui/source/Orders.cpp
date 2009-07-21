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
        bool playerShipsHere = false;
        for each ( Ship^ ship in system->Ships )
        {
            if ( ship->Owner == m_GameData->GetSpecies() )
            {
                playerShipsHere = true;
                break;
            }
        }
        if ( playerShipsHere )
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
            shipLoc = ship->PrintLocation(player);
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
    m_OrderList->Add("; +" + budget->GetBudgetTotal().ToString() + " EUs carried" );

    // Sort colonies for production
    m_GameData->GetSpecies()->SortColoniesByProdOrder();

    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
    {
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);

        int euCarried = budget->GetBudgetTotal();
        budget->SetColony(colony);

        // Production summary
        String ^prodSummary = String::Format("  ; {0} +{1} = {2}",
            colony->EUAvail, euCarried, budget->GetBudgetTotal());
        if( colony->PlanetType == PLANET_COLONY )
            prodSummary += "  max=" + budget->GetBudgetAvail().ToString();
        m_OrderList->Add( prodSummary );

        if( colony->CanProduce )
        {
            // Recycle / upgrade ships
            for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
            {
                if( ship->System == colony->System &&
                    ship->Command != nullptr )
                {
                    switch( ship->Command->Type )
                    {
                    case Ship::OrderType::Recycle:
                        {
                            int value = Calculators::ShipRecycleValue(ship->Age, ship->OriginalCost);
                            m_OrderList->Add( String::Format("    Recycle {0}  ; [A] +{1} EU",
                                ship->PrintClassWithName(),
                                value) );
                            budget->Recycle(value);
                        }
                        break;

                    case Ship::OrderType::Upgrade:
                        {
                            int value = Calculators::ShipUpgradeCost(ship->Age, ship->OriginalCost);
                            m_OrderList->Add( String::Format("    Upgrade {0}  ; [A] -{1} EU",
                                ship->PrintClassWithName(),
                                value) );
                            budget->Spend(value);
                        }
                        break;
                    }
                }
            }
        }

        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget);

        // Automatic spendings summary
        if( colony->PlanetType == PLANET_COLONY )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0} (total {1})",
                budget->GetBudgetAvail(),
                budget->GetBudgetTotal() ) );
        }
        else if( colony->PlanetType == PLANET_HOME )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0}", budget->GetBudgetAvail() ) );
        }
    }

    m_OrderList->Add("");
    m_OrderList->Add("END");
    m_OrderList->Add("");
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
