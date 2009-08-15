#include "StdAfx.h"
#include "Form1.h"
#include "enums.h"

using namespace System::IO;

////////////////////////////////////////////////////////////////

namespace FHUI
{

void Form1::UtilTabSelected()
{
    UtilProdPenaltyLS->Value = GameData::Player->TechLevelsAssumed[TECH_LS];
}

void Form1::UtilUpdateAll()
{
    UtilUpdateTRInfo();
    UtilUpdateResearch();
    UtilUpdateProdPenalty();
}

void Form1::UtilUpdateTRInfo()
{
    int trSize = Decimal::ToInt32(UtilTRSize->Value);
    double maintenance = ((trSize * 100.0) / 25.0) *
        Calculators::ShipMaintenanceDiscount(GameData::Player->TechLevelsAssumed[TECH_ML]);

    UtilTRInfoCap->Text = Calculators::TransportCapacity(trSize).ToString();
    UtilTrInfoMaint->Text = maintenance.ToString("F1");
    UtilTrInfoMA->Text = (2 * trSize).ToString();
    UtilTrInfoMA->ForeColor =
        ( (2 * trSize) > GameData::Player->TechLevels[TECH_MA] )
            ? Color::Red : Color::Black;
}

void Form1::UtilUpdateResearch()
{
    int from = Decimal::ToInt32( UtilResFrom->Value );
    int to = Decimal::ToInt32( UtilResTo->Value );
    if( from >= to )
    {
        to = from + 1;
        UtilResTo->Value = to;
    }

    UtilResInfoGuided->Text     = Calculators::ResearchCost(from, to, true).ToString();
    UtilResInfoAverage->Text    = Calculators::ResearchCost(from, to, false).ToString();
    UtilResInfoGuaranteed->Text = Calculators::ResearchCost(from, from + (to - from) * 2, false).ToString();
}

void Form1::UtilUpdateProdPenalty()
{
    int lsn = Decimal::ToInt32( UtilProdPenaltyLSN->Value );
    int ls = Decimal::ToInt32( UtilProdPenaltyLS->Value );
    int penalty = Calculators::ProductionPenalty(lsn, ls);
    if( penalty == -1 )
        UtilProdPenalty->Text = "N/A";
    else
        UtilProdPenalty->Text = penalty.ToString() + "%";
}

} // end namespace FHUI
