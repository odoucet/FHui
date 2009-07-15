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
    // TBD: to na pewno nie moze byc tutaj...
    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
        plugin->SetGameData(m_GameData);

    GenerateCombat();

    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();

    OrderTemplate->Lines = m_OrderList->ToArray();
}

void Form1::AddOrders(List<String^> ^orders)
{
    if( orders )
    {
        for each ( String^ line in orders )
        {
            m_OrderList->Add(line);
        }
    }
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
            AddOrders(plugin->GenerateCombat(system));

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
            AddOrders(plugin->GeneratePreDeparture(ship));

        m_OrderList->Add("");

        for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
            AddOrders(plugin->GeneratePreDeparture(colony));
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
            AddOrders(plugin->GenerateJumps(ship));
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
        AddOrders(plugin->GenerateProduction(nullptr));

    // Mining and resort colonies first
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_COLONY_MINING ||
            colony->PlanetType == PLANET_COLONY_RESORT )
        {
            m_OrderList->Add("");
            m_OrderList->Add("  PRODUCTION PL " + colony->Name);
            for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
                AddOrders( plugin->GenerateProduction(colony) );
        }

    // Colony planets
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_COLONY )
        {
            m_OrderList->Add("");
            m_OrderList->Add("  PRODUCTION PL " + colony->Name);
            for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
                AddOrders( plugin->GenerateProduction(colony) );
        }

    // Home planet
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_HOME )
        {
            m_OrderList->Add("");
            m_OrderList->Add("  PRODUCTION PL " + colony->Name);
            for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
                AddOrders( plugin->GenerateProduction(colony) );
        }

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
            AddOrders(plugin->GeneratePostArrival(ship));
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
