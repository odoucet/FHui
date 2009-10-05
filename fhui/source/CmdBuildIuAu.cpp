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

void CmdBuildIuAu::InitAvailResources(int availPop, int availEU)
{
    m_AvailPop = availPop;
    m_AvailEU  = availEU;

    AvailPop->Text = availPop.ToString();
    AvailEU->Text  = availEU.ToString();

    CUAmount->Maximum = availPop;
    IUAmount->Maximum = availPop;
    AUAmount->Maximum = availPop;
}

void CmdBuildIuAu::UpdateAmounts()
{
    int cu = GetCUAmount();
    int iu = GetIUAmount();
    int au = GetAUAmount();

    int cuNew = Math::Min(cu, m_AvailPop);
    int iuNew = Math::Min(iu, m_AvailPop - cuNew);
    int auNew = Math::Min(au, m_AvailPop - cuNew - iuNew);

    if( cu != cuNew )
        CUAmount->Value = cuNew;
    if( iu != iuNew )
        IUAmount->Value = iuNew;
    if( au != auNew )
        AUAmount->Value = auNew;

    int sum = cuNew + iuNew + auNew;
    TotalCost->Text = sum.ToString();
    TotalCost->ForeColor = sum > m_AvailEU ? Color::Red : Color::Black;
}

} // end namespace FHUI
