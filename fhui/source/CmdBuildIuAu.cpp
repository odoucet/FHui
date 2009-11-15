#include "StdAfx.h"
#include "CmdBuildIuAu.h"

namespace FHUI
{

int CmdBuildIuAu::GetCUAmount()
{
    return Decimal::ToInt32( CUAmount->Value );
}

int CmdBuildIuAu::GetIUAmount()
{
    return Decimal::ToInt32( IUAmount->Value );
}

int CmdBuildIuAu::GetAUAmount()
{
    return Decimal::ToInt32( AUAmount->Value );
}

ProdCmdBuildIUAU^ CmdBuildIuAu::GetCUCommand()
{
    return GetCommand(GetCUAmount(), INV_CU);
}

ProdCmdBuildIUAU^ CmdBuildIuAu::GetIUCommand()
{
    return GetCommand(GetIUAmount(), INV_IU);
}

ProdCmdBuildIUAU^ CmdBuildIuAu::GetAUCommand()
{
    return GetCommand(GetAUAmount(), INV_AU);
}

ProdCmdBuildIUAU^ CmdBuildIuAu::GetCommand(int amount, InventoryType inv)
{
    if( amount <= 0 )
        return nullptr;

    if( Target->SelectedIndex == 0 )
        return gcnew ProdCmdBuildIUAU(amount, inv, nullptr, nullptr);

    String ^target = Target->Text;
    if( target->Substring(0, 3) == "PL " )
    {
        Colony ^colony = GameData::Player->FindColony(target->Substring(3), false);
        return gcnew ProdCmdBuildIUAU(amount, inv, colony, nullptr);
    }

    Ship ^ship = Ship::FindRefListEntry( target );
    return gcnew ProdCmdBuildIUAU(amount, inv, nullptr, ship);
}

void CmdBuildIuAu::InitAvailResources(Colony ^colony, ProdCmdBuildIUAU ^cmd)
{
    m_Colony = colony;
    m_AvailPop = colony->Res->AvailPop;
    m_AvailEU  = colony->Res->AvailEU;
    if( cmd )
    {
        m_AvailEU += cmd->GetEUCost();
        m_AvailPop += cmd->GetPopCost();
    }

    InfoColony->Text    = m_Colony->Name;
    AvailPop->Text      = m_AvailPop.ToString();
    AvailEU->Text       = m_AvailEU.ToString();

    CUAmount->Maximum = Math::Max(0, Math::Min(m_AvailPop, m_AvailEU));
    IUAmount->Maximum = Math::Max(0, m_AvailEU);
    AUAmount->Maximum = Math::Max(0, m_AvailEU);

    List<String^> ^targets = gcnew List<String^>;
    targets->Add("< Local Build >");
    for each( Colony ^iCol in colony->System->ColoniesOwned )
    {
        if( iCol != colony )
            targets->Add( "PL " + iCol->Name );
    }
    for each( Ship ^ship in colony->System->GetShipTargets(CommandPhase::Production) )
    {
        if( ship->Type == SHIP_TR )
            targets->Add( ship->PrintRefListEntry() );
    }
    Target->DataSource = targets;
    UpdateShipCapacity();

    if( cmd )
    {
        switch( cmd->m_Unit )
        {
        case INV_CU:
            IUAmount->Enabled = false;
            AUAmount->Enabled = false;
            CUAmount->Value = cmd->m_Amount;
            break;

        case INV_IU:
            CUAmount->Enabled = false;
            AUAmount->Enabled = false;
            IUAmount->Value = cmd->m_Amount;
            break;

        case INV_AU:
            CUAmount->Enabled = false;
            IUAmount->Enabled = false;
            AUAmount->Value = cmd->m_Amount;
            break;

        default:
            throw gcnew FHUIDataIntegrityException("Invalid Build CU/IU/AU inventory type: " + ((int)cmd->m_Unit).ToString());
        }

        if( cmd->m_Colony )
            Target->Text = "PL " + cmd->m_Colony->Name;
        if( cmd->m_Ship )
            Target->Text = cmd->m_Ship->PrintRefListEntry();

        UpdateShipCapacity();
    }
}

void CmdBuildIuAu::UpdateAmounts()
{
    int cu = GetCUAmount();
    int iu = GetIUAmount();
    int au = GetAUAmount();

    int cuNew = Math::Min(cu, m_AvailPop);
    cuNew = Math::Min(cuNew, m_AvailEU);
    int iuNew = Math::Min(iu, m_AvailEU - cuNew);
    int auNew = Math::Min(au, m_AvailEU - cuNew - iuNew);

    if( m_Capacity != -1 )
    {
        int capAvail = Math::Max(0, m_Capacity - m_CapacityUsed);

        cuNew = Math::Min(cuNew, capAvail);
        iuNew = Math::Min(iuNew, capAvail - cuNew);
        auNew = Math::Min(auNew, capAvail - cuNew - iuNew);
    }

    cuNew = Math::Max(cuNew, 0);
    iuNew = Math::Max(iuNew, 0);
    auNew = Math::Max(auNew, 0);

    if( cu != cuNew )
        CUAmount->Value = cuNew;
    if( iu != iuNew )
        IUAmount->Value = iuNew;
    if( au != auNew )
        AUAmount->Value = auNew;

    int sum = cuNew + iuNew + auNew;
    TotalCost->Text = sum.ToString();
    TotalCost->ForeColor = sum > m_AvailEU ? Color::Red : Color::Black;

    if( m_Capacity != -1 )
    {
        int cap = sum + m_CapacityUsed;

        ShipCapacity->Text = String::Format("{0} / {1}", cap, m_Capacity);
        ShipCapacity->ForeColor = cap > m_Capacity ? Color::Red : Color::Black;
    }
}

void CmdBuildIuAu::UpdateShipCapacity()
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
