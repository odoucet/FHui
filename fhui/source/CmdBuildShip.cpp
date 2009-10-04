#include "StdAfx.h"
#include "CmdBuildShips.h"
#include "enums.h"

namespace FHUI
{

void CmdBuildShips::InitControls()
{
    InfoAvailEU->Text = m_AvailEU.ToString();
    InfoAvailMA->Text = m_AvailMA.ToString();

    DoPresetScout();
}

ICommand^ CmdBuildShips::CreateCommand()
{
    ShipType type = static_cast<ShipType>( ShipClass->SelectedIndex + 1 ); // Skip BAS
    int size = Decimal::ToInt32(ShipSize->Value);
    bool sublight = ShipSublight->Checked;
    String ^name = ShipName->Text->Trim();
    return gcnew ProdCmdBuildShip(type, size, sublight, name);
}

void CmdBuildShips::Build()
{
    String ^name = ShipName->Text->Trim();
    if( String::IsNullOrEmpty(name) )
    {
        MessageBox::Show(
            this,
            "Ship must have a name!",
            "Ship Name",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error,
            MessageBoxDefaultButton::Button1);
        return;
    }

     this->DialogResult = System::Windows::Forms::DialogResult::OK;
     Close();
}

void CmdBuildShips::DoPresetScout()
{
    ShipClass->SelectedIndex = 0;
    ShipSize->Value = 1;
    ShipSublight->Checked = false;
}

void CmdBuildShips::DoPresetTransport()
{
    ShipClass->SelectedIndex = 0;
    ShipSize->Value = m_AvailMA / 2;
    ShipSublight->Checked = false;
}

void CmdBuildShips::DoPresetWarship()
{
    int index = 1;
    for( int i = SHIP_PB; i < SHIP_MAX; ++i )
    {
        if( Calculators::ShipMARequired(static_cast<ShipType>(i), 0) <= m_AvailMA )
            index = i - 1;  // Skip BAS
        else
            break;
    }

    ShipClass->SelectedIndex = index;
    ShipSublight->Checked = false;
}

void CmdBuildShips::ShipUpdate()
{
    ShipSize->Enabled = ( ShipClass->SelectedIndex == 0 );

    ShipType type = static_cast<ShipType>( ShipClass->SelectedIndex + 1 ); // Skip BAS
    int size = Decimal::ToInt32(ShipSize->Value);
    bool sublight = ShipSublight->Checked;

    InfoCost->Text = Calculators::ShipBuildCost(type, size, sublight).ToString();
    InfoUpkeep->Text = Calculators::ShipMaintenanceCost(type, size, sublight).ToString();
    InfoTonnage->Text = Calculators::ShipTonnage(type, size).ToString();

    int ma = Calculators::ShipMARequired(type, size);
    InfoAvailMA->ForeColor = ma <= m_AvailMA ? Color::Black : Color::Red;
}

} // end namespace FHUI
