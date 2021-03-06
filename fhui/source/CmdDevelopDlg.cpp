#include "StdAfx.h"
#include "CmdDevelopDlg.h"
#include "enums.h"

namespace FHUI
{

void CmdDevelopDlg::InidDialog(Colony ^colony, ProdCmdDevelop ^cmd)
{
    m_Colony = colony;

    m_AvailEU = colony->Res->AvailEU;
    m_AvailPop = colony->Res->AvailPop;
    if( cmd )
    {
        m_AvailEU += cmd->GetEUCost();
        m_AvailPop += cmd->GetPopCost();
    }

    InfoColony->Text = colony->Name;
    InfoBudget->Text = m_AvailEU.ToString();
    InfoPop->Text = m_AvailPop.ToString();

    List<String^> ^colonies = gcnew List<String^>;
    List<String^> ^ships = gcnew List<String^>;

    colonies->Add("< Local Develop >");
    ships->Add("< Local Develop >");

    for each( Colony ^iCol in GameData::Player->Colonies )
    {
        if( iCol->EconomicBase < 2000 &&
            iCol != colony )
        {
            colonies->Add( iCol->Name );
        }
    }

    for each( Ship ^ship in colony->System->GetShipTargets(CommandPhase::Production) )
    {
        if( ship->Type == SHIP_TR && !ship->SubLight )
            ships->Add( ship->PrintRefListEntry() );
    }


    DevColony->DataSource = colonies;
    DevShip->DataSource = ships;

    m_Command = gcnew ProdCmdDevelop(Decimal::ToInt32(DevAmount->Value), nullptr, nullptr);
    if( cmd )
    {
        DevAmount->Value = cmd->m_Amount;
        if( cmd->m_Colony )
            DevColony->Text = cmd->m_Colony->Name;
        if( cmd->m_Ship )
            DevShip->Text = cmd->m_Ship->PrintRefListEntry();
    }

    GenerateCommand(false);
}

bool CmdDevelopDlg::GenerateCommand(bool validate)
{
    m_Command->m_Amount = Decimal::ToInt32(DevAmount->Value);
    if( DevColony->SelectedIndex != 0 )
        m_Command->m_Colony = GameData::Player->FindColony( DevColony->Text, false );
    else
        m_Command->m_Colony = nullptr;
    if( DevShip->SelectedIndex != 0 )
        m_Command->m_Ship = Ship::FindRefListEntry( DevShip->Text );
    else
        m_Command->m_Ship = nullptr;

    DevShip->Enabled =
        (   m_Command->m_Colony != nullptr &&
            m_Command->m_Colony->System != m_Colony->System );

    InfoCmd->Text = m_Command->Print();
    InfoBudget->ForeColor = m_Command->GetEUCost() > m_AvailEU ? Color::Red : Color::Black;
    InfoPop->ForeColor = m_Command->GetPopCost() > m_AvailPop ? Color::Red : Color::Black;

    if( validate )
    {
        if( DevShip->Enabled &&
            DevShip->SelectedIndex == 0 )
        {
            MessageBox::Show(
                this,
                "You must select a transport for this Develop to work.",
                "Develop",
                MessageBoxButtons::OK,
                MessageBoxIcon::Error);
            return false;
        }
    }

    return true;
}

}
