#include "StdAfx.h"

#include "GridFilter.h"
#include "GameData.h"

using namespace System::Text::RegularExpressions;

void GridFilter::Update(Object ^sender)
{
    try
    {
        if( *m_bGridUpdateEnabled )
        {
            if( sender == CtrlRefHome )
                SetRefHome();
            else if( sender == CtrlRefXYZ )
                SetRefXYZ();
            else if( sender == CtrlRefColony )
                SetRefColony();
            else if( sender == CtrlRefShip )
                SetRefShip();
            else if( sender == CtrlGV )
                OnGridSetup();
            else if( sender == CtrlShipAge )
            {
                // Update ship reference combo
                Ship ^ship = GetShipFromRefList(CtrlRefShip);
                if( ship && Decimal::ToInt32(CtrlShipAge->Value) != ship->Age )
                    CtrlRefShip->Text = s_CaptionShip;

                OnGridSetup();
            }
            else if( sender == CtrlMaxMishap ||
                sender == CtrlMaxLSN ||
                sender == CtrlFiltVisA ||
                sender == CtrlFiltVisV ||
                sender == CtrlFiltVisN ||
                sender == CtrlFiltColA ||
                sender == CtrlFiltColC ||
                sender == CtrlFiltColN )
            {
                int filtMask =
                    ( Decimal::ToByte(CtrlMaxLSN->Value) ) +
                    ( Decimal::ToByte(CtrlMaxMishap->Value) << 8 ) +
                    ( CtrlFiltVisA ? (CtrlFiltVisA->Checked ? 1 << 16 : 0) : 0 ) +
                    ( CtrlFiltVisV ? (CtrlFiltVisV->Checked ? 1 << 17 : 0) : 0 ) +
                    ( CtrlFiltVisN ? (CtrlFiltVisN->Checked ? 1 << 18 : 0) : 0 ) +
                    ( CtrlFiltColA ? (CtrlFiltColA->Checked ? 1 << 19 : 0) : 0 ) +
                    ( CtrlFiltColC ? (CtrlFiltColC->Checked ? 1 << 20 : 0) : 0 ) +
                    ( CtrlFiltColN ? (CtrlFiltColN->Checked ? 1 << 21 : 0) : 0 );
                if( filtMask != m_LastFiltMask )
                {
                    m_LastFiltMask = filtMask;
                    OnGridSetup();
                }
            }
        }
    }
    catch( Exception ^e )
    {
        OnGridException(e);
    }
}

bool GridFilter::Filter(IGridDataSrc ^item)
{
    bool bShowVisV = CtrlFiltVisV->Checked;
    bool bShowVisN = CtrlFiltVisN->Checked;
    bool bShowColC = CtrlFiltColC->Checked;
    bool bShowColN = CtrlFiltColN->Checked;

    int numColonies = item->GetFilterNumColonies();
    if( (bShowColC && numColonies == 0) ||
        (bShowColN && numColonies > 0) )
    {
        return true;
    }

    StarSystem ^system = item->GetFilterSystem();
    if( system )
    {
        if( (bShowVisV && system->LastVisited == -1) ||
            (bShowVisN && system->LastVisited != -1) )
        {
            return true;
        }
    }

    // Mishap
    int gv  = Decimal::ToInt32(CtrlGV->Value);
    int age = Decimal::ToInt32(CtrlShipAge->Value);
    double maxMishap = Decimal::ToDouble(CtrlMaxMishap->Value);

    if( system &&
        RefSystem &&
        maxMishap < system->CalcMishap(RefSystem, gv, age) )
    {
        return true;
    }

    // LSN
    int maxLSN = Decimal::ToInt32(CtrlMaxLSN->Value);
    int itemLSN = item->GetFilterLSN();
    if( itemLSN != 99999 && maxLSN < itemLSN )
    {
        return true;
    }

    return false;
}

void GridFilter::Reset()
{
    *m_bGridUpdateEnabled = false;

    CtrlMaxLSN->Value = DefaultLSN;
    CtrlMaxMishap->Value = DefaultMishap;
    CtrlFiltVisA->Checked = true;
    CtrlFiltColA->Checked = true;

    *m_bGridUpdateEnabled = true;
    Update(CtrlMaxLSN);
}

void GridFilter::SetRefText()
{
}

void GridFilter::SetRefXYZ()
{
    String ^ref = CtrlRefXYZ->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return;

    Match ^m = Regex("\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)").Match(ref);
    if( m->Success )
    {
        int x = int::Parse(m->Groups[1]->ToString());
        int y = int::Parse(m->Groups[2]->ToString());
        int z = int::Parse(m->Groups[3]->ToString());

        StarSystem ^system = GameData->GetStarSystem(x, y, z);

        if( RefSystem == system )
            return;

        *m_bGridUpdateEnabled   = false;
        CtrlRefHome->Text       = s_CaptionHome;
        CtrlRefColony->Text     = s_CaptionColony;

        RefSystem = system;
        OnGridSetup();

        CtrlRef->Text = String::Format("Ref. system: [{0}]",
            RefSystem->PrintLocation());
    }
    else
        throw gcnew FHUIDataIntegrityException("Can't find reference system: " + ref);
}

void GridFilter::SetRefHome()
{
    String ^ref = CtrlRefHome->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return;

    Alien ^sp = GameData->GetAlien(ref);

    if( RefSystem == sp->HomeSystem )
        return;

    *m_bGridUpdateEnabled   = false;
    CtrlRefXYZ->Text        = s_CaptionXYZ;
    CtrlRefColony->Text     = s_CaptionColony;

    RefSystem = sp->HomeSystem;
    OnGridSetup();

    CtrlRef->Text = String::Format("Ref. system: [{0}] HOME {1}",
        RefSystem->PrintLocation(),
        sp->Name);
}

void GridFilter::SetRefColony()
{
    String ^ref = CtrlRefColony->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return;

    Alien ^sp = GameData->GetSpecies();

    Colony ^colony = nullptr;
    bool bFound = false;
    for each( colony in sp->Colonies )
    {
        if( colony->Name == ref )
        {
            bFound = true;
            break;
        }
    }
    if( !bFound )
    {
        for each( colony in GameData->GetColonies() )
        {
            if( colony->Owner == sp )
                continue;
            if( ref->Substring(0, colony->Name->Length) == colony->Name &&
                colony->Owner->Name == ref->Substring(colony->Name->Length + 2, ref->Length - colony->Name->Length - 3) )
            {
                bFound = true;
                break;
            }
        }
    }
    if( !bFound )
        throw gcnew FHUIDataIntegrityException("Can't find reference colony: " + ref);

    if( RefSystem == colony->System )
        return;

    *m_bGridUpdateEnabled   = false;
    CtrlRefXYZ->Text        = s_CaptionXYZ;
    CtrlRefHome->Text       = s_CaptionHome;

    RefSystem = colony->System;
    OnGridSetup();

    CtrlRef->Text = String::Format("Ref. system: [{0}] PL {1}",
        RefSystem->PrintLocation(), ref);
}

Ship^ GridFilter::GetShipFromRefList(ComboBox ^combo)
{
    String ^ref = combo->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return nullptr;

    Match ^m = Regex("^[A-Z0-9s]+ (.*) \\(A\\d+\\)$").Match(ref);
    if( m->Success )
        return GameData->GetShip( m->Groups[1]->ToString() );

    throw gcnew FHUIDataIntegrityException("Can't find reference ship: " + ref);
}

void GridFilter::SetRefShip()
{
    Ship ^ship = GetShipFromRefList(CtrlRefShip);
    if( ship )
    {
        int refVal = Decimal::ToInt32(CtrlShipAge->Value);
        if( refVal != ship->Age )
            CtrlShipAge->Value = ship->Age;
    }
}
