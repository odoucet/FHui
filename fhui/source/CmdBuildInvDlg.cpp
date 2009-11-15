#include "StdAfx.h"
#include "CmdBuildInvDlg.h"

namespace FHUI
{

int CmdBuildInvDlg::GetAmount()
{
    return Decimal::ToInt32( Amount->Value );
}

ProdCmdBuildInv^ CmdBuildInvDlg::GetCommand()
{
    int amount = GetAmount();
    if( amount <= 0 )
        return nullptr;

    InventoryType inv = GetInv();

    if( Target->SelectedIndex == 0 )
        return gcnew ProdCmdBuildInv(amount, inv, nullptr, nullptr);

    String ^target = Target->Text;
    if( target->Substring(0, 3) == "PL " )
    {
        Colony ^colony = GameData::Player->FindColony(target->Substring(3), false);
        return gcnew ProdCmdBuildInv(amount, inv, colony, nullptr);
    }

    Ship ^ship = Ship::FindRefListEntry( target );
    return gcnew ProdCmdBuildInv(amount, inv, nullptr, ship);
}

InventoryType CmdBuildInvDlg::GetInv()
{
    String ^invText = Inventory->Text;
    invText = invText->Substring(0, invText->IndexOf(':'));
    return FHStrings::InvFromString(invText);
}

String^ CmdBuildInvDlg::GetInvString(int i)
{
    InventoryType inv = static_cast<InventoryType>(i);
    String ^ret = FHStrings::InvToString(inv) + ": ";
    switch( inv )
    {
    case INV_PD:    return ret + "Planetary Defence";
    case INV_SU:    return ret + "Starbase Unit";
    case INV_FD:    return ret + "Field Distortion";
    case INV_FS:    return ret + "Fail-safe Jump";
    case INV_DR:    return ret + "Damage Repair";
    case INV_FM:    return ret + "Forced Misjump";
    case INV_FJ:    return ret + "Forced Jump";
    case INV_GW:    return ret + "Germ Warfare";
    case INV_GT:    return ret + "Gravitic Telescope";
    case INV_JP:    return ret + "Jump Portal";
    case INV_TP:    return ret + "Terraforming Plant";
    case INV_GU1:   return ret + "Gun Unit Mark 1";
    case INV_GU2:   return ret + "Gun Unit Mark 2";
    case INV_GU3:   return ret + "Gun Unit Mark 3";
    case INV_GU4:   return ret + "Gun Unit Mark 4";
    case INV_GU5:   return ret + "Gun Unit Mark 5";
    case INV_GU6:   return ret + "Gun Unit Mark 6";
    case INV_GU7:   return ret + "Gun Unit Mark 7";
    case INV_GU8:   return ret + "Gun Unit Mark 8";
    case INV_GU9:   return ret + "Gun Unit Mark 9";
    case INV_SG1:   return ret + "Shield Generator Mark 1";
    case INV_SG2:   return ret + "Shield Generator Mark 2";
    case INV_SG3:   return ret + "Shield Generator Mark 3";
    case INV_SG4:   return ret + "Shield Generator Mark 4";
    case INV_SG5:   return ret + "Shield Generator Mark 5";
    case INV_SG6:   return ret + "Shield Generator Mark 6";
    case INV_SG7:   return ret + "Shield Generator Mark 7";
    case INV_SG8:   return ret + "Shield Generator Mark 8";
    case INV_SG9:   return ret + "Shield Generator Mark 9";
    default:
        throw gcnew FHUIDataIntegrityException("Invalid inventory for Build Inventory dialog: " + i.ToString());
    }
}

void CmdBuildInvDlg::InitAvailResources(Colony ^colony, ProdCmdBuildInv ^cmd)
{
    m_Colony = colony;
    m_AvailEU  = colony->Res->AvailEU;
    m_AvailPop = colony->Res->AvailPop;
    if( cmd )
    {
        m_AvailEU += cmd->GetEUCost();
        m_AvailPop += cmd->GetPopCost();
    }

    InfoColony->Text    = m_Colony->Name;
    AvailEU->Text       = m_AvailEU.ToString();
    AvailPop->Text      = m_AvailPop.ToString();

    List<String^> ^invList = gcnew List<String^>;
    for( int i = INV_PD; i < INV_MAX; ++i )
    {
        if( Calculators::InventoryTechAvailable(static_cast<InventoryType>(i)) )
            invList->Add( GetInvString(i) );
    }
    Inventory->DataSource = invList;

    List<String^> ^targets = gcnew List<String^>;
    targets->Add("< Local Build >");
    for each( Colony ^iCol in colony->System->ColoniesOwned )
    {
        if( iCol != colony )
            targets->Add( "PL " + iCol->Name );
    }
    for each( Ship ^ship in colony->System->GetShipTargets(CommandPhase::Production) )
    {
        targets->Add( ship->PrintRefListEntry() );
    }
    Target->DataSource = targets;
    UpdateShipCapacity();

    if( cmd )
    {
        if( Calculators::InventoryTechAvailable(cmd->m_Type) )
        {
            Amount->Value = cmd->m_Amount;
            Inventory->Text = GetInvString(cmd->m_Type);

            if( cmd->m_Colony )
                Target->Text = "PL " + cmd->m_Colony->Name;
            if( cmd->m_Ship )
                Target->Text = cmd->m_Ship->PrintRefListEntry();

            UpdateShipCapacity();
        }
        else
        {
            Amount->Enabled = false;
            BtnBuild->Enabled = false;
        }
        Inventory->Enabled = false;
    }
}

void CmdBuildInvDlg::UpdateAmounts()
{
    int amount = GetAmount();
    InventoryType inv = GetInv();
    int cost = amount * Calculators::InventoryBuildCost(inv);
    int cap = Calculators::InventoryCarryCapacity(inv, amount);

    TotalCost->Text = cost.ToString();
    TotalCost->ForeColor = cost > m_AvailEU ? Color::Red : Color::Black;

    if( m_Capacity != -1 )
    {
        cap += m_CapacityUsed;

        ShipCapacity->Text = String::Format("{0} / {1}", cap, m_Capacity);
        ShipCapacity->ForeColor = cap > m_Capacity ? Color::Red : Color::Black;
    }

    if( inv == INV_PD )
        AvailPop->ForeColor = amount > m_AvailPop ? Color::Red : Color::Black;
}

void CmdBuildInvDlg::UpdateShipCapacity()
{
    m_Capacity = -1;
    m_CapacityUsed = 0;

    if( Target->SelectedIndex != 0 )
    {
        String ^target = Target->Text;
        if( target->Substring(0, 3) != "PL " )
        {
            Ship ^ship = Ship::FindRefListEntry( target );
            m_Capacity = ship->Capacity;

            array<int> ^inv = ship->Cargo;
            for( int i = 0; i < INV_MAX; ++i )
            {
                m_CapacityUsed += Calculators::InventoryCarryCapacity( static_cast<InventoryType>(i), inv[i] );
            }
        }
    }

    if( m_Capacity == -1 )
        ShipCapacity->Text = "N/A";
    else
        UpdateAmounts();
}

} // end namespace FHUI
