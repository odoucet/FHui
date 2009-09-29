#include "StdAfx.h"
#include "CmdBuildIuAu.h"

namespace FHUI
{

int CmdBuildIuAu::GetIUAmount()
{
    return Decimal::ToInt32( IUAmount->Value );
}

int CmdBuildIuAu::GetAUAmount()
{
    return Decimal::ToInt32( AUAmount->Value );
}

void CmdBuildIuAu::InitAvailResources(int availPop, int availEU)
{
    m_AvailPop = availPop;
    m_AvailEU  = availEU;

    AvailPop->Text = availPop.ToString();
    AvailEU->Text  = availEU.ToString();

    IUAmount->Maximum = availPop;
    AUAmount->Maximum = availPop;
}

void CmdBuildIuAu::UpdateAmounts()
{
    int iu = GetIUAmount();
    int au = GetAUAmount();

    if( (iu + au) > m_AvailPop )
    {
        au = m_AvailPop - iu;
        AUAmount->Value = au;
    }

    TotalCost->Text = (iu + au).ToString();
    TotalCost->ForeColor = (iu + au) > m_AvailEU ? Color::Red : Color::Black;
}

} // end namespace FHUI
