#include "StdAfx.h"

#include "GridFilter.h"
#include "Enums.h"

using namespace System::Text::RegularExpressions;

namespace FHUI
{

void GridFilter::Update()
{
    Update(m_RefreshDummy);
}

void GridFilter::Update(Object ^sender)
{
    try
    {
        if( EnableUpdates )
        {
            if( sender == CtrlRefHome )
                SetRefHome();
            else if( sender == CtrlRefXYZ )
                SetRefXYZ();
            else if( sender == CtrlRefColony )
                SetRefColony();
            else if( sender == CtrlRefShip )
                SetRefShip();
            else if( sender == CtrlShipAge )
            {
                // Update ship reference combo
                Ship ^ship = GetShipFromRefList(CtrlRefShip);
                if( ship && Decimal::ToInt32(CtrlShipAge->Value) != ship->Age )
                    CtrlRefShip->Text = s_CaptionShip;

                GridSetup();
            }
            else if( sender == m_RefreshDummy ||
                sender == CtrlMaxMishap ||
                sender == CtrlMaxLSN ||
                sender == CtrlFiltVisV ||
                sender == CtrlFiltVisN ||
                sender == CtrlFiltColC ||
                sender == CtrlFiltColN ||
                sender == CtrlFiltOwnO ||
                sender == CtrlFiltOwnN ||
                sender == CtrlFiltRelA ||
                sender == CtrlFiltRelE ||
                sender == CtrlFiltRelN ||
                sender == CtrlFiltRelP ||
                sender == CtrlFiltTypeBas ||
                sender == CtrlFiltTypeMl ||
                sender == CtrlFiltTypeTr ||
                sender == CtrlMiMaBalance )
            {
                __int64 filtMask =
                    ( CtrlMaxLSN ? Decimal::ToByte(CtrlMaxLSN->Value) : 0 ) +
                    ( CtrlMaxMishap ? (Decimal::ToByte(CtrlMaxMishap->Value) << 8) : 0 ) +
                    ( CtrlFiltVisV ? (CtrlFiltVisV->Checked ? (1 << 17) : 0) : 0 ) +
                    ( CtrlFiltVisN ? (CtrlFiltVisN->Checked ? (1 << 18) : 0) : 0 ) +
                    ( CtrlFiltColC ? (CtrlFiltColC->Checked ? (1 << 20) : 0) : 0 ) +
                    ( CtrlFiltColN ? (CtrlFiltColN->Checked ? (1 << 21) : 0) : 0 ) +
                    ( CtrlFiltOwnO ? (CtrlFiltOwnO->Checked ? (1 << 23) : 0) : 0 ) +
                    ( CtrlFiltOwnN ? (CtrlFiltOwnN->Checked ? (1 << 24) : 0) : 0 ) +
                    ( CtrlFiltRelA ? (CtrlFiltRelA->Checked ? (1 << 25) : 0) : 0 ) +
                    ( CtrlFiltRelE ? (CtrlFiltRelE->Checked ? (1 << 26) : 0) : 0 ) +
                    ( CtrlFiltRelN ? (CtrlFiltRelN->Checked ? (1 << 27) : 0) : 0 ) +
                    ( CtrlFiltRelP ? (CtrlFiltRelP->Checked ? (1 << 28) : 0) : 0 ) +
                    ( CtrlFiltTypeBas ? (CtrlFiltTypeBas->Checked ? (1 << 29) : 0) : 0 ) +
                    ( CtrlFiltTypeMl ? (CtrlFiltTypeMl->Checked ? (1 << 30) : 0) : 0 ) +
                    ( CtrlFiltTypeTr ? (CtrlFiltTypeTr->Checked ? (1 << 31) : 0) : 0 ) +
                    ( CtrlMiMaBalance ? (CtrlMiMaBalance->Checked ? (1LL << 32) : 0) : 0 );
                if( sender == m_RefreshDummy ||
                    filtMask != m_LastFiltMask )
                {
                    m_LastFiltMask = filtMask;
                    GridSetup();
                }
            }
        }
    }
    catch( Exception ^e )
    {
        GridException(e);
    }
}

bool GridFilter::Filter(IGridDataSrc ^item)
{
    // Colonized...
    if( CtrlFiltColC )
    {
        int numColonies = item->GetFilterNumColonies();
        if( (!CtrlFiltColN->Checked && numColonies == 0) ||
            (!CtrlFiltColC->Checked && numColonies > 0) )
        {
            return true;
        }
    }

    // Owned
    if( CtrlFiltOwnO )
    {
        Alien ^owner = item->GetFilterOwner();
        Alien ^sp    = GameData->GetSpecies();
        if( (!CtrlFiltOwnO->Checked && owner == sp) ||
            (!CtrlFiltOwnN->Checked && owner != sp) )
        {
            return true;
        }
    }

    // Relation
    if( CtrlFiltRelA )
    {
        switch( item->GetFilterRelType() )
        {
        case SP_NEUTRAL:
            if( CtrlFiltRelN->Checked == false )
                return true;
            break;
        case SP_ALLY:
            if( CtrlFiltRelA->Checked == false )
                return true;
            break;
        case SP_ENEMY:
            if( CtrlFiltRelE->Checked == false )
                return true;
            break;
        case SP_PIRATE:
            if( CtrlFiltRelP->Checked == false )
                return true;
            break;
        }
    }

    // Ship type
    if( CtrlFiltTypeBas )
    {
        switch( item->GetFilterShipType() )
        {
        case SHIP_BAS:
            if( CtrlFiltTypeBas->Checked == false )
                return true;
            break;
        case SHIP_TR:
            if( CtrlFiltTypeTr->Checked == false )
                return true;
            break;
        default:
            if( CtrlFiltTypeMl->Checked == false )
                return true;
        }
    }

    StarSystem ^system = item->GetFilterSystem();
    if( system )
    {
        // Visited
        if( CtrlFiltVisV )
        {
            bool bShowVisV = CtrlFiltVisV->Checked;
            bool bShowVisN = CtrlFiltVisN->Checked;
            if( (!CtrlFiltVisN->Checked && system->LastVisited == -1) ||
                (!CtrlFiltVisV->Checked && system->LastVisited != -1) )
            {
                return true;
            }
        }

        // Mishap
        if( CtrlGV  )
        {
            int gv  = Decimal::ToInt32(CtrlGV->Value);
            int age = 0;
            if( CtrlShipAge )
                age = Decimal::ToInt32(CtrlShipAge->Value);
            else
            {
                Ship ^ship = dynamic_cast<Ship^>(item);
                if( ship )
                    age = ship->Age;
            }
            double maxMishap = Decimal::ToDouble(CtrlMaxMishap->Value);
            if( maxMishap < system->CalcMishap(RefSystem, gv, age) )
            {
                return true;
            }
        }
    }

    // LSN
    if( CtrlMaxLSN )
    {
        int maxLSN = Decimal::ToInt32(CtrlMaxLSN->Value);
        int itemLSN = item->GetFilterLSN();
        if( itemLSN != 99999 && maxLSN < itemLSN )
        {
            return true;
        }
    }

    return false;
}

void GridFilter::Reset()
{
    EnableUpdates = false;

    if( CtrlMaxLSN )
        CtrlMaxLSN->Value = DefaultLSN;
    if( CtrlMaxMishap )
        CtrlMaxMishap->Value = DefaultMishap;
    if( CtrlFiltVisV )
    {
        CtrlFiltVisV->Checked = true;
        CtrlFiltVisN->Checked = true;
    }
    if( CtrlFiltColC )
    {
        CtrlFiltColC->Checked = true;
        CtrlFiltColN->Checked = true;
    }
    if( CtrlFiltOwnO )
    {
        CtrlFiltOwnO->Checked = true;
        CtrlFiltOwnN->Checked = true;
    }
    if( CtrlFiltRelA )
    {
        CtrlFiltRelA->Checked = true;
        CtrlFiltRelE->Checked = true;
        CtrlFiltRelN->Checked = true;
        CtrlFiltRelP->Checked = true;
    }
    if( CtrlFiltTypeBas )
    {
        CtrlFiltTypeBas->Checked = true;
        CtrlFiltTypeMl->Checked = true;
        CtrlFiltTypeTr->Checked = true;
    }
    if( CtrlMiMaBalance )
        CtrlMiMaBalance->Checked = false;

    EnableUpdates = true;
    Update();
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

        EnableUpdates       = false;
        CtrlRefHome->Text   = s_CaptionHome;
        CtrlRefColony->Text = s_CaptionColony;

        RefSystem = system;
        GridSetup();

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

    EnableUpdates       = false;
    CtrlRefXYZ->Text    = s_CaptionXYZ;
    CtrlRefColony->Text = s_CaptionColony;

    RefSystem = sp->HomeSystem;
    GridSetup();

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

    EnableUpdates       = false;
    CtrlRefXYZ->Text    = s_CaptionXYZ;
    CtrlRefHome->Text   = s_CaptionHome;

    RefSystem = colony->System;
    GridSetup();

    CtrlRef->Text = String::Format("Ref. system: [{0}] PL {1}",
        RefSystem->PrintLocation(), ref);
}

void GridFilter::SetRefSystem(StarSystem ^system)
{
    if( system != RefSystem &&
        CtrlRefXYZ != nullptr )
    {
        if( system->IsVoid )
        {
            EnableUpdates       = false;
            CtrlRefXYZ->Text    = s_CaptionXYZ;
            CtrlRefHome->Text   = s_CaptionHome;
            CtrlRefColony->Text = s_CaptionColony;

            RefSystem = system;
            GridSetup();

            CtrlRef->Text = String::Format("Ref. system: [{0}] (space void)",
                RefSystem->PrintLocation());
        }
        else
            CtrlRefXYZ->Text = system->PrintLocation();
    }
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
        if( CtrlShipAge )
        {
            int refVal = Decimal::ToInt32(CtrlShipAge->Value);
            if( refVal != ship->Age )
                CtrlShipAge->Value = ship->Age;
        }

        if( SelectRefSystemFromRefShip &&
            ship->System != RefSystem )
        {
            EnableUpdates       = false;
            CtrlRefXYZ->Text    = s_CaptionXYZ;
            CtrlRefHome->Text   = s_CaptionHome;
            CtrlRefColony->Text = s_CaptionColony;

            RefSystem = ship->System;
            GridSetup();

            CtrlRef->Text = String::Format("Ref. system: [{0}] {1} {2}",
                RefSystem->PrintLocation(),
                ship->PrintClass(),
                ship->Name );
        }
    }
}

} // end namespace FHUI
