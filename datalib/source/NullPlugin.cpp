#include "StdAfx.h"
#include "IFHUIPlugin.h"
#include "Enums.h"
#include "GameData.h"
#include "Commands.h"

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

BudgetTracker::BudgetTracker(List<String^>^ orders, int euCarried)
    : m_Orders(orders)
    , m_OrdersGlobal(orders)
    , m_BudgetTotal(euCarried)
{
}

void BudgetTracker::SetColony(Colony ^colony)
{
    m_Colony = colony;

    if( m_Colony )
    {
        m_BudgetTotal += colony->EUAvail;
        m_BudgetAvail = Math::Min(m_BudgetTotal, colony->GetMaxProductionBudget());

        m_Colony->ProductionReset();
        m_Colony->Res->TotalEU = m_BudgetTotal;
        m_Colony->Res->AvailEU = m_BudgetAvail;

        m_Orders = m_Colony->OrdersText;
    }
    else
    {
        m_BudgetAvail = 0;
        m_Orders = m_OrdersGlobal;
    }
}

void BudgetTracker::EvalOrder(ICommand ^cmd)
{
    // Track budget
    UpdateEU( cmd->GetEUCost() );

    // Track CU
    UpdateCU( cmd->GetCUMod() );

    // Track Inventory
    for( int i = 0; i < INV_MAX; ++i )
    {
        InventoryType it = static_cast<InventoryType>(i);
        UpdateInventory( it, cmd->GetInvMod(it) );
    }
}

void BudgetTracker::UpdateEU(int eu)
{
    m_BudgetAvail -= eu;
    m_BudgetTotal -= eu;

    m_Colony->Res->TotalEU = m_BudgetTotal;
    m_Colony->Res->AvailEU = m_BudgetAvail;

    if( m_BudgetAvail < 0 )
        AddComment( String::Format("; !!!!!! Production limit exceeded by {0} !!!!!!", -m_BudgetAvail) );

    if( m_BudgetTotal < 0 )
        AddComment( String::Format("; !!!!!! BUDGET EXCEEDED by {0} !!!!!!", -m_BudgetTotal) );
}

void BudgetTracker::UpdateCU(int pop)
{
    m_Colony->Res->AvailCU += pop;
    if( m_Colony->Res->AvailCU < 0 )
    {
        AddComment( String::Format("; !!!!!! NOT ENOUGH CU Available ({0} left) !!!!!!", m_Colony->Res->AvailCU - pop) );
        m_Colony->Res->AvailCU = 0;
    }
}

void BudgetTracker::UpdateInventory(InventoryType it, int mod)
{
    m_Colony->Res->Inventory[it] += mod;
    if( m_Colony->Res->Inventory[it] < 0 )
    {
        AddComment( String::Format(
            "; !!!!!! NOT ENOUGH Inventory Available ({0} {1} left) !!!!!!",
            m_Colony->Res->Inventory[it] - mod,
            FHStrings::InvToString(it) ) );
        m_Colony->Res->Inventory[it] = 0;
    }
}

void BudgetTracker::AddComment(String ^comment)
{
    m_Orders->Add( "    " + comment );
}

// ---------------------------------------------------------

} // end namespace FHUIPlugin
