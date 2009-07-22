#include "StdAfx.h"
#include "IFHUIPlugin.h"
#include "Enums.h"
#include "GameData.h"

using namespace System;
using namespace System::Data;

namespace FHUI
{

// ---------------------------------------------------------

public ref class NullPluginBase : public IPluginBase
{
public:
    virtual void        SetGameData(GameData^ gd) { m_GameData = gd; }

protected:
    GameData^   m_GameData;
};

// ---------------------------------------------------------

BudgetTracker::BudgetTracker(List<String^> ^orders, int euCarried)
    : m_Orders(orders)
    , m_BudgetTotal(euCarried)
{
}

void BudgetTracker::SetColony(Colony ^colony)
{
    m_PopAvail = colony->AvailPop;
    m_BudgetTotal += colony->EUAvail;
    m_BudgetAvail = colony->GetMaxProductionBudget();
}

void BudgetTracker::Recycle(int eu)
{
    m_BudgetTotal += eu;
}

void BudgetTracker::Spend(int eu)
{
    m_BudgetAvail -= eu;
    m_BudgetTotal -= eu;

    if( m_BudgetAvail < 0 )
        m_Orders->Add( String::Format("; !!!!!! Production limit exceeded by {0} !!!!!!", -m_BudgetAvail) );

    if( m_BudgetTotal < 0 )
        m_Orders->Add( String::Format("; !!!!!! BUDGET EXCEEDED by {0} !!!!!!", -m_BudgetTotal) );
}

void BudgetTracker::UsePopulation(int pop)
{
    m_PopAvail -= pop;
    if( m_PopAvail < 0 )
    {
        m_Orders->Add( String::Format("; !!!!!! NOT ENOUGH CU Available ({0} left) !!!!!!", m_PopAvail + pop) );
        m_PopAvail = 0;
    }
}

// ---------------------------------------------------------

} // end namespace FHUIPlugin
