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
    List<StarSystem^> ^locations = gcnew List<StarSystem^>;
    for each ( Ship ^ship in m_GameData->GetShips() )
    {
        if( ! locations->Contains( ship->System ) )
        {
            locations->Add( ship->System );
        }
    }

    for each ( StarSystem^ system in locations )
    {
        GenerateCombatInfo(system);
        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateCombat(m_OrderList, system);

        m_OrderList->Add("");
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateCombatInfo(StarSystem^ system)
{
    m_OrderList->Add(String::Format("; Comment about {0} (ships + aliens) goes here", system->PrintLocation() ) );
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

    int budget = m_GameData->GetCarriedEU();
    m_OrderList->Add("; +" + budget.ToString() + " EUs carried" );

    // Sort colonies for production
    m_GameData->GetSpecies()->SortColoniesByProdOrder();

    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
    {
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);

        // Production summary
        int eu = colony->EUProd - colony->EUFleet;
        String ^prodSummary = String::Format("  ; {0} +{1} = {2}", eu, budget, eu + budget);
        if( colony->PlanetType == PLANET_COLONY )
            prodSummary += "  max=" + (eu * 2).ToString();
        m_OrderList->Add( prodSummary );

        budget += eu;
        int colonyProd = 0;

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
                            budget += value;
                            m_OrderList->Add( String::Format("    Recycle {0}  ; [A] +{1} EU",
                                ship->PrintClassWithName(),
                                value) );
                        }
                        break;

                    case Ship::OrderType::Upgrade:
                        {
                            int value = Calculators::ShipUpgradeCost(ship->Age, ship->OriginalCost);
                            String ^cmd = String::Format("    Upgrade {0}  ; [A] -{1} EU",
                                ship->PrintClassWithName(),
                                value);
                            if( value > budget )
                                cmd + "; !!! NO BUDGET: " + cmd;


                            budget -= value;
                            colonyProd += value;
                            m_OrderList->Add(cmd);
                        }
                        break;
                    }
                }
            }
        }

        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget, colonyProd);

        // Automatic spendings summary
        if( colony->PlanetType == PLANET_COLONY )
        {
            if( colonyProd > (2 * eu) )
            {
                m_OrderList->Add( String::Format("    ; !!!!!! Production limit exceeded by {0} !!!!!!",
                    colonyProd - (2 * eu)) );
            }
            else
            {
                m_OrderList->Add( String::Format("    ; -- EU to use: {0} (total {1})",
                    Math::Min(budget, 2 * eu - colonyProd), budget) );
            }
        }
        else if( colony->PlanetType == PLANET_HOME )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0}", budget) );
        }

        if( budget < 0 )
            m_OrderList->Add("    ; !!!!! BUDGET EXCEEDED !!!!!!");
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
