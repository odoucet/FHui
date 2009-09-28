#include "StdAfx.h"
#include "CmdResearch.h"

namespace FHUI
{

void CmdResearch::InitGroups()
{
    m_Groups = gcnew array<Group>(TECH_MAX);

    m_Groups[TECH_MI].En      = EnMI;
    m_Groups[TECH_MI].Amount  = AmountMI;
    m_Groups[TECH_MI].CalcAvg = CalcAvgMI;
    m_Groups[TECH_MI].CalcGtd = CalcGtdMI;
    m_Groups[TECH_MI].CalcGui = CalcGuiMI;
    m_Groups[TECH_MI].LvlFrom = CalcLvlFromMI;
    m_Groups[TECH_MI].LvlTo   = CalcLvlToMI;

    m_Groups[TECH_MA].En      = EnMA;
    m_Groups[TECH_MA].Amount  = AmountMA;
    m_Groups[TECH_MA].CalcAvg = CalcAvgMA;
    m_Groups[TECH_MA].CalcGtd = CalcGtdMA;
    m_Groups[TECH_MA].CalcGui = CalcGuiMA;
    m_Groups[TECH_MA].LvlFrom = CalcLvlFromMA;
    m_Groups[TECH_MA].LvlTo   = CalcLvlToMA;

    m_Groups[TECH_ML].En      = EnML;
    m_Groups[TECH_ML].Amount  = AmountML;
    m_Groups[TECH_ML].CalcAvg = CalcAvgML;
    m_Groups[TECH_ML].CalcGtd = CalcGtdML;
    m_Groups[TECH_ML].CalcGui = CalcGuiML;
    m_Groups[TECH_ML].LvlFrom = CalcLvlFromML;
    m_Groups[TECH_ML].LvlTo   = CalcLvlToML;

    m_Groups[TECH_GV].En      = EnGV;
    m_Groups[TECH_GV].Amount  = AmountGV;
    m_Groups[TECH_GV].CalcAvg = CalcAvgGV;
    m_Groups[TECH_GV].CalcGtd = CalcGtdGV;
    m_Groups[TECH_GV].CalcGui = CalcGuiGV;
    m_Groups[TECH_GV].LvlFrom = CalcLvlFromGV;
    m_Groups[TECH_GV].LvlTo   = CalcLvlToGV;

    m_Groups[TECH_LS].En      = EnLS;
    m_Groups[TECH_LS].Amount  = AmountLS;
    m_Groups[TECH_LS].CalcAvg = CalcAvgLS;
    m_Groups[TECH_LS].CalcGtd = CalcGtdLS;
    m_Groups[TECH_LS].CalcGui = CalcGuiLS;
    m_Groups[TECH_LS].LvlFrom = CalcLvlFromLS;
    m_Groups[TECH_LS].LvlTo   = CalcLvlToLS;

    m_Groups[TECH_BI].En      = EnBI;
    m_Groups[TECH_BI].Amount  = AmountBI;
    m_Groups[TECH_BI].CalcAvg = CalcAvgBI;
    m_Groups[TECH_BI].CalcGtd = CalcGtdBI;
    m_Groups[TECH_BI].CalcGui = CalcGuiBI;
    m_Groups[TECH_BI].LvlFrom = CalcLvlFromBI;
    m_Groups[TECH_BI].LvlTo   = CalcLvlToBI;

    for( int i = 0; i < TECH_MAX; ++i )
    {
        m_Groups[i].LvlFrom->Minimum    = GameData::Player->TechLevels[i];
        m_Groups[i].LvlFrom->Value      = GameData::Player->TechLevels[i];
        m_Groups[i].LvlTo->Minimum      = GameData::Player->TechLevels[i] + 1;
        m_Groups[i].LvlTo->Value        = GameData::Player->TechLevels[i] + 1;
    }
}

void CmdResearch::UpdateAmount()
{
    Decimal d = 0;
    for( int i = 0; i < TECH_MAX; ++i )
        d += m_Groups[i].Amount->Value;

    AmountTotal->Text = d.ToString();
}

void CmdResearch::UpdateEnable(Object ^sender)
{
    for( int i = 0; i < TECH_MAX; ++i )
    {
        if( sender == m_Groups[i].En )
        {
            bool en = m_Groups[i].En->Checked;

            m_Groups[i].Amount->Enabled  = en;
            m_Groups[i].CalcAvg->Enabled = en;
            m_Groups[i].CalcGtd->Enabled = en;
            m_Groups[i].CalcGui->Enabled = en;
            m_Groups[i].LvlFrom->Enabled = en;
            m_Groups[i].LvlTo->Enabled   = en;

            break;
        }
    }
}

void CmdResearch::Calc(Object ^sender)
{
    for( int i = 0; i < TECH_MAX; ++i )
    {
        if( sender == m_Groups[i].CalcAvg )
        {
            m_Groups[i].Amount->Value = Calculators::ResearchCost(
                Decimal::ToInt32(m_Groups[i].LvlFrom->Value),
                Decimal::ToInt32(m_Groups[i].LvlTo->Value),
                false );
            break;
        }
        else if( sender == m_Groups[i].CalcGtd )
        {
            int from = Decimal::ToInt32(m_Groups[i].LvlFrom->Value);
            int to   = Decimal::ToInt32(m_Groups[i].LvlTo->Value);
            m_Groups[i].Amount->Value = Calculators::ResearchCost(
                from, from + (to - from) * 2, false );
            break;
        }
        if( sender == m_Groups[i].CalcGui )
        {
            m_Groups[i].Amount->Value = Calculators::ResearchCost(
                Decimal::ToInt32(m_Groups[i].LvlFrom->Value),
                Decimal::ToInt32(m_Groups[i].LvlTo->Value),
                true );
            break;
        }
    }
}

int CmdResearch::GetAmount(TechType tech)
{
    if( m_Groups[tech].En->Checked )
    {
        return Decimal::ToInt32( m_Groups[tech].Amount->Value );
    }
    return -1;
}

} // end namespace FHUI
