#include "StdAfx.h"
#include "Form1.h"

using namespace System::IO;

////////////////////////////////////////////////////////////////

namespace FHUI
{

void Form1::UtilTRInfoUpdate()
{
    int trSize = Decimal::ToInt32(UtilTRSize->Value);
    double maintenance = ((trSize * 100.0) / 25.0) *
        Calculators::ShipMaintenanceDiscount(m_GameData->GetSpecies()->TechLevelsAssumed[TECH_ML]);

    UtilTRInfoCap->Text = Calculators::TransportCapacity(trSize).ToString();
    UtilTrInfoMaint->Text = maintenance.ToString("F1");
    UtilTrInfoMA->Text = (2 * trSize).ToString();
    UtilTrInfoMA->ForeColor =
        ( (2 * trSize) > m_GameData->GetSpecies()->TechLevels[TECH_MA] )
            ? Color::Red : Color::Black;
}

} // end namespace FHUI
