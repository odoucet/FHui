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
    virtual void        SetTurn(int turn) {}
};

// ---------------------------------------------------------

BudgetTracker::BudgetTracker(List<String^>^ orders, int euCarried)
    : m_Orders(orders)
    , m_OrdersGlobal(orders)
    , m_BudgetTotal(euCarried)
{
}

void BudgetTracker::SetColony(Colony ^colony, CommandPhase phase)
{
    m_Colony = colony;

    if( m_Colony )
    {
        if( phase == CommandPhase::Production )
        {
            m_BudgetTotal += colony->EUAvail;
            m_BudgetAvail = Math::Min(m_BudgetTotal, colony->GetMaxProductionBudget());

            m_Colony->Res->TotalEU = m_BudgetTotal;
            m_Colony->Res->AvailEU = m_BudgetAvail;
            m_Colony->Res->UsedEU  = 0;
        }

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

    // Track Populatiaon
    UpdatePop( cmd->GetPopCost() );

    // Track Inventory
    for( int i = 0; i < INV_MAX; ++i )
    {
        InventoryType it = static_cast<InventoryType>(i);
        UpdateInventory( it, cmd->GetInvMod(it) );
    }

    // Track shipyards
    if( cmd->RequiresShipyard() && m_Colony )
    {
        if( m_Colony->Res->AvailShipyards == 0 )
            AddComment( "; !!!!!! NOT ENOUGH SHIPYARD capacity available!!!!!!" );
        else
            --m_Colony->Res->AvailShipyards;
    }
}

void BudgetTracker::EvalOrderTransfer(ICommand ^cmd)
{
    CmdTransfer ^cmdTr = safe_cast<CmdTransfer^>(cmd);

    if( cmd->GetPhase() == CommandPhase::PostArrival )
        GameData::EvalPostArrivalInventory(cmd->GetRefSystem(), cmd);

    array<int> ^invFrom = cmdTr->GetFromInventory();
    array<int> ^invTo = cmdTr->GetToInventory();

    // Track Inventory
    InventoryType inv = cmdTr->m_Type;
    int amount = cmdTr->m_Amount;

    if( amount > invFrom[inv] )
    {
        AddComment( String::Format(
            "; !!!!!! NOT ENOUGH Inventory available ({0} {1} left) !!!!!!",
            invFrom[inv],
            FHStrings::InvToString(inv) ) );
        amount = invFrom[inv];
    }
    invFrom[inv] -= amount;

    invTo[inv] += amount;
    if( cmdTr->m_ToShip )
    {
        int sumCapacity = 0;
        for( int i = 0; i < INV_MAX; ++i )
        {
            sumCapacity += Calculators::InventoryCarryCapacity(
                static_cast<InventoryType>(i),
                invTo[i] );
        }
        if( sumCapacity > cmdTr->m_ToShip->Capacity )
        {
            AddComment( String::Format(
                "; !!!!!! NOT ENOUGH transport capacity available ({0} / {1} used) !!!!!!",
                sumCapacity,
                cmdTr->m_ToShip->Capacity ) );
        }
    }
}

void BudgetTracker::UpdateEU(int eu)
{
    if( eu < 0 && m_Colony->PlanetType != PLANET_HOME )
    {   // For recycle-like commands avoid situation when available EUs
        // exeed actual production limit for the planet
        int limit = Math::Max(0, m_Colony->GetMaxProductionBudget() - m_Colony->Res->UsedEU);
        m_BudgetAvail = Math::Min(limit, m_BudgetAvail - eu);
    }
    else
        m_BudgetAvail -= eu;
    m_BudgetTotal -= eu;

    m_Colony->Res->TotalEU = m_BudgetTotal;
    m_Colony->Res->AvailEU = m_BudgetAvail;
    if( eu > 0 )
        m_Colony->Res->UsedEU += eu;

    if( m_BudgetAvail < 0 )
        AddComment( String::Format("; !!!!!! Production limit exceeded by {0} !!!!!!", -m_BudgetAvail) );

    if( m_BudgetTotal < 0 )
        AddComment( String::Format("; !!!!!! BUDGET EXCEEDED by {0} !!!!!!", -m_BudgetTotal) );
}

void BudgetTracker::UpdatePop(int pop)
{
    m_Colony->Res->AvailPop -= pop;
    if( m_Colony->Res->AvailPop < 0 )
    {
        AddComment( String::Format("; !!!!!! NOT ENOUGH Population available ({0} left) !!!!!!", m_Colony->Res->AvailPop + pop) );
        m_Colony->Res->AvailPop = 0;
    }
}

void BudgetTracker::UpdateInventory(InventoryType it, int mod)
{
    m_Colony->Res->Inventory[it] += mod;
    if( m_Colony->Res->Inventory[it] < 0 )
    {
        AddComment( String::Format(
            "; !!!!!! NOT ENOUGH Inventory available ({0} {1} left) !!!!!!",
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
