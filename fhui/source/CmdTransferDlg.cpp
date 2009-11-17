#include "StdAfx.h"
#include "CmdTransferDlg.h"

namespace FHUI
{

int CmdTransferDlg::GetAmount(InventoryType inv)
{
    return Decimal::ToInt32(m_UnitControls[inv].Amount->Value);
}

CmdTransfer^ CmdTransferDlg::GetCommand(InventoryType inv)
{
    int amount = GetAmount(inv);
    if( amount == 0 )
        return nullptr;

    return gcnew CmdTransfer(
        m_Cmd->m_Phase,
        inv,
        amount,
        m_Cmd->m_FromColony,
        m_Cmd->m_FromShip,
        m_Cmd->m_ToColony,
        m_Cmd->m_ToShip );
}

void CmdTransferDlg::InitArray()
{
    m_UnitControls = gcnew array<UnitControls>(INV_MAX);

    m_UnitControls[INV_RM].Info   = InfoRM;
    m_UnitControls[INV_RM].Amount = AmountRM;
    m_UnitControls[INV_CU].Info   = InfoCU;
    m_UnitControls[INV_CU].Amount = AmountCU;
    m_UnitControls[INV_IU].Info   = InfoIU;
    m_UnitControls[INV_IU].Amount = AmountIU;
    m_UnitControls[INV_AU].Info   = InfoAU;
    m_UnitControls[INV_AU].Amount = AmountAU;
    m_UnitControls[INV_PD].Info   = InfoPD;
    m_UnitControls[INV_PD].Amount = AmountPD;
    m_UnitControls[INV_SU].Info   = InfoSU;
    m_UnitControls[INV_SU].Amount = AmountSU;
    m_UnitControls[INV_FD].Info   = InfoFD;
    m_UnitControls[INV_FD].Amount = AmountFD;
    m_UnitControls[INV_FS].Info   = InfoFS;
    m_UnitControls[INV_FS].Amount = AmountFS;
    m_UnitControls[INV_DR].Info   = InfoDR;
    m_UnitControls[INV_DR].Amount = AmountDR;
    m_UnitControls[INV_FM].Info   = InfoFM;
    m_UnitControls[INV_FM].Amount = AmountFM;
    m_UnitControls[INV_FJ].Info   = InfoFJ;
    m_UnitControls[INV_FJ].Amount = AmountFJ;
    m_UnitControls[INV_GW].Info   = InfoGW;
    m_UnitControls[INV_GW].Amount = AmountGW;
    m_UnitControls[INV_GT].Info   = InfoGT;
    m_UnitControls[INV_GT].Amount = AmountGT;
    m_UnitControls[INV_JP].Info   = InfoJP;
    m_UnitControls[INV_JP].Amount = AmountJP;
    m_UnitControls[INV_TP].Info   = InfoTP;
    m_UnitControls[INV_TP].Amount = AmountTP;
    m_UnitControls[INV_GU1].Info   = InfoGU1;
    m_UnitControls[INV_GU1].Amount = AmountGU1;
    m_UnitControls[INV_GU2].Info   = InfoGU2;
    m_UnitControls[INV_GU2].Amount = AmountGU2;
    m_UnitControls[INV_GU3].Info   = InfoGU3;
    m_UnitControls[INV_GU3].Amount = AmountGU3;
    m_UnitControls[INV_GU4].Info   = InfoGU4;
    m_UnitControls[INV_GU4].Amount = AmountGU4;
    m_UnitControls[INV_GU5].Info   = InfoGU5;
    m_UnitControls[INV_GU5].Amount = AmountGU5;
    m_UnitControls[INV_GU6].Info   = InfoGU6;
    m_UnitControls[INV_GU6].Amount = AmountGU6;
    m_UnitControls[INV_GU7].Info   = InfoGU7;
    m_UnitControls[INV_GU7].Amount = AmountGU7;
    m_UnitControls[INV_GU8].Info   = InfoGU8;
    m_UnitControls[INV_GU8].Amount = AmountGU8;
    m_UnitControls[INV_GU9].Info   = InfoGU9;
    m_UnitControls[INV_GU9].Amount = AmountGU9;
    m_UnitControls[INV_SG1].Info   = InfoSG1;
    m_UnitControls[INV_SG1].Amount = AmountSG1;
    m_UnitControls[INV_SG2].Info   = InfoSG2;
    m_UnitControls[INV_SG2].Amount = AmountSG2;
    m_UnitControls[INV_SG3].Info   = InfoSG3;
    m_UnitControls[INV_SG3].Amount = AmountSG3;
    m_UnitControls[INV_SG4].Info   = InfoSG4;
    m_UnitControls[INV_SG4].Amount = AmountSG4;
    m_UnitControls[INV_SG5].Info   = InfoSG5;
    m_UnitControls[INV_SG5].Amount = AmountSG5;
    m_UnitControls[INV_SG6].Info   = InfoSG6;
    m_UnitControls[INV_SG6].Amount = AmountSG6;
    m_UnitControls[INV_SG7].Info   = InfoSG7;
    m_UnitControls[INV_SG7].Amount = AmountSG7;
    m_UnitControls[INV_SG8].Info   = InfoSG8;
    m_UnitControls[INV_SG8].Amount = AmountSG8;
    m_UnitControls[INV_SG9].Info   = InfoSG9;
    m_UnitControls[INV_SG9].Amount = AmountSG9;
}

void CmdTransferDlg::InitControls(CmdTransfer ^cmd)
{
    m_Cmd = cmd;

    bool bFrom = true;
    bool bTo = true;

    if( m_Cmd->m_FromColony )
    {
        TransFrom->Enabled = false;
        TransFrom->DataSource = gcnew array<String^>(1){ "PL " + m_Cmd->m_FromColony->Name };
    }
    else if( m_Cmd->m_FromShip )
    {
        TransFrom->Enabled = false;
        TransFrom->DataSource = gcnew array<String^>(1){ m_Cmd->m_FromShip->PrintRefListEntry() };
    }
    else
    {
        TransFrom->Enabled = true;
        bFrom = false;

        List<String^> ^sources = gcnew List<String^>;
        sources->Add("< Select Source >");
        for each( Colony ^colony in m_Cmd->GetRefSystem()->ColoniesOwned )
        {
            if( m_Cmd->m_ToColony != colony )
                sources->Add("PL " + colony->Name );
        }
        for each( Ship ^ship in m_Cmd->GetRefSystem()->GetShipTargets(m_Cmd->m_Phase) )
        {
            if( m_Cmd->m_ToShip != ship )
                sources->Add( ship->PrintRefListEntry() );
        }
        TransFrom->DataSource = sources;
    }

    if( m_Cmd->m_ToColony )
    {
        TransTo->Enabled = false;
        TransTo->DataSource = gcnew array<String^>(1){ "PL " + m_Cmd->m_ToColony->Name };
    }
    else if( m_Cmd->m_ToShip )
    {
        TransTo->Enabled = false;
        TransTo->DataSource = gcnew array<String^>(1){ m_Cmd->m_ToShip->PrintRefListEntry() };
    }
    else
    {
        TransTo->Enabled = true;
        bTo = false;

        List<String^> ^targets = gcnew List<String^>;
        targets->Add("< Select Target >");
        for each( Colony ^colony in m_Cmd->GetRefSystem()->ColoniesOwned )
        {
            if( m_Cmd->m_FromColony != colony )
                targets->Add("PL " + colony->Name );
        }
        for each( Ship ^ship in m_Cmd->GetRefSystem()->GetShipTargets(m_Cmd->m_Phase) )
        {
            if( m_Cmd->m_FromShip != ship )
                targets->Add( ship->PrintRefListEntry() );
        }
        TransTo->DataSource = targets;
    }

    if( bFrom && bTo )
        InitControlsUnits();
}

void CmdTransferDlg::InitControlsUnits()
{
    ComboBox ^combo = nullptr;
    if( TransFrom->Enabled )
        combo = TransFrom;
    else if( TransTo->Enabled )
        combo = TransTo;

    if( combo )
    {
        if( combo->SelectedIndex == 0 )
            return;

        combo->Enabled = false;

        Colony ^selColony = nullptr;
        Ship ^selShip = nullptr;

        String ^sel = combo->Text;
        if( sel->Substring(0, 3) == "PL " )
            selColony = GameData::Player->FindColony(sel->Substring(3), false);
        else
            selShip = Ship::FindRefListEntry( sel );

        if( combo == TransFrom )
        {
            m_Cmd->m_FromColony = selColony;
            m_Cmd->m_FromShip = selShip;
        }
        else
        {
            m_Cmd->m_ToColony = selColony;
            m_Cmd->m_ToShip = selShip;
        }
    }

    if( m_Cmd->m_ToShip )
    {
        m_CapacityLimit = m_Cmd->m_ToShip->Capacity;
        InfoCapacityLimit->Text = m_CapacityLimit.ToString();
    }
    else
        m_CapacityLimit = 0;

    InfoInv->Text = GameData::PrintInventory( m_Cmd->GetToInventory(true) );

    m_Inv = m_Cmd->GetFromInventory(true);

    for( int i = 0; i < INV_MAX; ++i )
    {
        InventoryType inv = static_cast<InventoryType>(i);
        int cnt = m_Inv[i];

        m_UnitControls[i].Info->Text = cnt.ToString();

        if( m_Cmd->m_Type != INV_MAX && m_Cmd->m_Type != inv )
            continue;

        if( cnt > 0 )
        {
            m_UnitControls[i].Amount->Enabled = true;
            m_UnitControls[i].Amount->Maximum = cnt;

            if( m_Cmd->m_Type != INV_MAX )
                m_UnitControls[i].Amount->Value = Math::Min(cnt, m_Cmd->m_Amount);
        }
    }

    UpdateAmounts();
}

void CmdTransferDlg::UpdateAmounts()
{
    bool transferAny = false;

    if( m_CapacityLimit > 0 )
    {
        array<int> ^inv = m_Cmd->GetToInventory(true);
        int sumCapacity = 0;

        for( int i = 0; i < INV_MAX; ++i )
        {
            sumCapacity += Calculators::InventoryCarryCapacity( static_cast<InventoryType>(i), inv[i] );

            int cnt = Decimal::ToInt32(m_UnitControls[i].Amount->Value);
            int cap = Calculators::InventoryCarryCapacity( static_cast<InventoryType>(i), cnt );
            sumCapacity += cap;

            transferAny |= cnt > 0;
        }

        InfoCapacityUsed->Text = sumCapacity.ToString();
        InfoCapacityUsed->ForeColor = (m_CapacityLimit >= sumCapacity ? Color::Black : Color::Red);
    }
    else
    {
        for( int i = 0; i < INV_MAX; ++i )
        {
            if( Decimal::ToInt32(m_UnitControls[i].Amount->Value) > 0 )
            {
                transferAny = true;
                break;
            }
        }
    }

    BtnTransfer->Enabled = transferAny;
}

} // end namespace FHUI
