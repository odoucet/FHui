#include "StdAfx.h"

#include "GridFilter.h"
#include "Enums.h"

using namespace System::Text::RegularExpressions;

namespace FHUI
{

void GridFilter::OnGridFill()
{
    m_RowsCnt = 0;
    GridFill();
    OnGridSelectionChanged();
}

void GridFilter::OnGridSelectionChanged()
{
    if( CtrlNumRows )
    {
        String ^text = "Num grid rows: " + m_RowsCnt.ToString();
        if( m_Grid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect )
            text += "; selected: " + m_Grid->SelectedRows->Count;
        CtrlNumRows->Text = text;
    }
}

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
            else if( sender == CtrlRefEdit )
                SetRefText();
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

                OnGridFill();
            }
            else if( sender == m_RefreshDummy ||
                sender == CtrlMaxMishap ||
                sender == CtrlMaxLSN ||
                sender == CtrlFiltScanK ||
                sender == CtrlFiltScanU ||
                sender == CtrlFiltColC ||
                sender == CtrlFiltColH ||
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
                    ( CtrlFiltScanK ? (CtrlFiltScanK->Checked ? (1 << 17) : 0) : 0 ) +
                    ( CtrlFiltScanU ? (CtrlFiltScanU->Checked ? (1 << 18) : 0) : 0 ) +
                    ( CtrlFiltColC ? (CtrlFiltColC->Checked ? (1 << 20) : 0) : 0 ) +
                    ( CtrlFiltColH ? (CtrlFiltColH->Checked ? (1 << 21) : 0) : 0 ) +
                    ( CtrlFiltColN ? (CtrlFiltColN->Checked ? (1 << 22) : 0) : 0 ) +
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
                    OnGridFill();
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
        if( CtrlFiltColH &&
            CtrlFiltColH->Checked == false &&
            item->GetFilterSystem()->HomeSpecies != nullptr )
        {
            return true;
        }
    }

    // Owned
    if( CtrlFiltOwnO )
    {
        Alien ^owner = item->GetFilterOwner();
        Alien ^sp    = GameData::Player;
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
            if( CtrlFiltRelP && CtrlFiltRelP->Checked == false )
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
        if( CtrlFiltScanU )
        {
            bool bShowScanK = CtrlFiltScanK->Checked;
            bool bShowScanU = CtrlFiltScanU->Checked;
            if( (!bShowScanK && system->TurnScanned != -1) ||
                (!bShowScanU && system->TurnScanned == -1) )
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
            if( maxMishap == 100.0 )    // Allow systems with mishap above 100% when filter is at max value
                maxMishap = Double::MaxValue;
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

    ++m_RowsCnt;

    return false;
}

void GridFilter::ResetControls(bool doUpdate)
{
    EnableUpdates = false;

    if( CtrlMaxLSN )
        CtrlMaxLSN->Value = DefaultLSN;
    if( CtrlMaxMishap )
        CtrlMaxMishap->Value = DefaultMishap;
    if( CtrlFiltScanK )
    {
        CtrlFiltScanK->Checked = true;
        CtrlFiltScanU->Checked = true;
    }
    if( CtrlFiltColC )
    {
        CtrlFiltColC->Checked = true;
        CtrlFiltColN->Checked = true;
        if( CtrlFiltColH )
            CtrlFiltColH->Checked = true;
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
        if( CtrlFiltRelP )
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

    if( doUpdate )
        Update();
}

void GridFilter::SetRefText()
{
    String ^ref = CtrlRefEdit->Text;
    Match ^m = Regex("\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)").Match(ref);
    if( m->Success )
    {
        CtrlRefEdit->ForeColor = System::Drawing::Color::Black;

        int x = int::Parse(m->Groups[1]->ToString());
        int y = int::Parse(m->Groups[2]->ToString());
        int z = int::Parse(m->Groups[3]->ToString());

        // Allow void systems - user may enter any coords
        StarSystem ^system = GameData::GetStarSystem(x, y, z, true);

        if( RefSystem == system )
            return;

        EnableUpdates       = false;
        CtrlRefHome->Text   = s_CaptionHome;
        CtrlRefColony->Text = s_CaptionColony;
        CtrlRefXYZ->Text    = s_CaptionXYZ;

        RefSystem = system;
        OnGridFill();

        CtrlRef->Text = String::Format("Ref. system: [{0}]",
            RefSystem->PrintLocation());
    }
    else
    {
        CtrlRefEdit->ForeColor = System::Drawing::Color::Red;
    }
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

        StarSystem ^system = GameData::GetStarSystem(x, y, z, false);

        if( RefSystem == system )
            return;

        EnableUpdates       = false;
        CtrlRefHome->Text   = s_CaptionHome;
        CtrlRefColony->Text = s_CaptionColony;

        RefSystem = system;
        OnGridFill();

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

    Alien ^sp = GameData::GetAlien(ref);

    if( RefSystem == sp->HomeSystem )
        return;

    EnableUpdates       = false;
    CtrlRefXYZ->Text    = s_CaptionXYZ;
    CtrlRefColony->Text = s_CaptionColony;
    CtrlRefShip->Text   = s_CaptionShip;

    RefSystem = sp->HomeSystem;
    OnGridFill();

    CtrlRef->Text = String::Format("Ref. system: [{0}] HOME {1}",
        RefSystem->PrintLocation(),
        sp->Name);
}

void GridFilter::SetRefColony()
{
    String ^ref = CtrlRefColony->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return;

    Alien ^sp = GameData::Player;

    Colony ^colony = sp->FindColony(ref, true);
    if( !colony )
    {
        for each( Alien ^alien in GameData::GetAliens() )
        {
            if( alien == sp )
                continue;
            for each( Colony ^iCol in alien->Colonies )
            {
                // TODO: implement more robust and safe colony search
                if( ref->Substring(0, iCol->Name->Length) == iCol->Name &&
                    iCol->Owner->Name == ref->Substring(iCol->Name->Length + 2, ref->Length - iCol->Name->Length - 3) )
                {
                    colony = iCol;
                    break;
                }
            }
            if( colony != nullptr )
                break;
        }
    }
    if( !colony )
        throw gcnew FHUIDataIntegrityException("Can't find reference colony: " + ref);

    if( RefSystem == colony->System )
        return;

    EnableUpdates       = false;
    CtrlRefXYZ->Text    = s_CaptionXYZ;
    CtrlRefHome->Text   = s_CaptionHome;
    CtrlRefShip->Text   = s_CaptionShip;

    RefSystem = colony->System;
    OnGridFill();

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
            CtrlRefShip->Text   = s_CaptionShip;

            RefSystem = system;
            OnGridFill();

            CtrlRef->Text = String::Format("Ref. system: [{0}] (space void)",
                RefSystem->PrintLocation());
        }
        else
            CtrlRefXYZ->Text = system->PrintLocation();
    }
}

void GridFilter::SetRefSystem(Colony ^colony)
{
    if( colony->System != RefSystem &&
        CtrlRefColony != nullptr )
    {
        CtrlRefColony->Text = colony->PrintRefListEntry();
    }
}

Ship^ GridFilter::GetShipFromRefList(ComboBox ^combo)
{
    String ^ref = combo->Text;
    if( String::IsNullOrEmpty(ref) || ref[0] == '[' )
        return nullptr;

    Match ^m = Regex("^[A-Z0-9s]+ (.*) \\(A\\d+\\)$").Match(ref);
    if( m->Success )
        return GameData::Player->FindShip( m->Groups[1]->ToString(), false );

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
            OnGridFill();

            CtrlRef->Text = String::Format("Ref. system: [{0}] {1} {2}",
                RefSystem->PrintLocation(),
                ship->PrintClass(),
                ship->Name );
        }
    }
}

} // end namespace FHUI
