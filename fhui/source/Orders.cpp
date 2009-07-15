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
        {
            String ^output = plugin->GenerateCombat(system);
            for each ( String^ line in output->Split('\n') )
            {
                m_OrderList->Add(line);
            }
        }
        m_OrderList->Add("");
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateCombatInfo(StarSystem^ system)
{
    m_OrderList->Add(String::Format("; Comment about {0} {1} {2} (ships + aliens) goes here", system->X, system->Y, system->Z ) );
}

void Form1::GeneratePreDeparture()
{
    m_OrderList->Add("START PRE-DEPARTURE");
    m_OrderList->Add("");

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");
    m_OrderList->Add("");

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");
    m_OrderList->Add("");

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GeneratePostArrival()
{
    m_OrderList->Add("START POST-ARRIVAL");
    m_OrderList->Add("");

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
