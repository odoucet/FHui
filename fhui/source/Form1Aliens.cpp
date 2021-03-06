#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

#include "CmdMessage.h"
#include "CmdTeachDlg.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

void Form1::AliensInitControls()
{
    AliensGridSorter ^sorter = gcnew AliensGridSorter(AliensGrid);

    // Add columns
    AliensColumns %c = m_AliensColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object    = ADD_COLUMN(nullptr,       nullptr,                    Alien,  None,       Default);
    c.Name      = ADD_COLUMN("Name",        "Species name",             String, Ascending,  Default);
    c.Relation  = ADD_COLUMN("Relation",    "Current relation",         String, Ascending,  Relation);
    c.TurnMet   = ADD_COLUMN("Met",         "Turn number when you have met this species for the first time", int,  Ascending,  Default);
    c.Home      = ADD_COLUMN("Home",        "Home planet location",     String, Ascending,  Default);
    c.Dist      = ADD_COLUMN("Dist",        "Home distance from your home system", double, Ascending, Default);
    c.TechLev   = ADD_COLUMN("Tech Levels", "Estimated technology levels", String, Ascending, Default);
    c.TC        = ADD_COLUMN("TC",          "Temperature class of their home planet", int,  Ascending,  Default);
    c.PC        = ADD_COLUMN("PC",          "Pressure class of their home planet",    int,    Ascending,  Default);
    c.Atmosphere= ADD_COLUMN("Atm",         "Atmospheric information",  String, Ascending,  Default);
    c.Message   = ADD_COLUMN("Msg",         "Message",                  String, Ascending,  Default);
    c.Teach     = ADD_COLUMN("Teach",       "Teach orders",             String, Ascending,  Default);
    c.EMail     = ADD_COLUMN("EMail",       "Species email",            String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Aliens, sorter);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(AliensGrid, nullptr, c.Object, c.Relation, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Aliens, AliensGrid);

    AliensGrid->Columns[c.Dist]->DefaultCellStyle->Alignment    = DataGridViewContentAlignment::MiddleRight;
    AliensGrid->Columns[c.EMail]->DefaultCellStyle->Font        = m_GridFontSmall;
    AliensGrid->Columns[c.Message]->DefaultCellStyle->Font      = m_GridFontSmall;
    AliensGrid->Columns[c.Teach]->DefaultCellStyle->Font        = m_GridFontSmall;
    AliensGrid->Columns[c.TechLev]->DefaultCellStyle->Font      = m_GridFontSmall;
    AliensGrid->Columns[c.Atmosphere]->DefaultCellStyle->Font   = m_GridFontSmall;

    GridFilter ^filter = gcnew GridFilter(AliensGrid, m_bGridUpdateEnabled);
    filter->GridFill += gcnew GridFillHandler(this, &Form1::AliensFillGrid);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlFiltRelA    = AliensFiltRelA;
    filter->CtrlFiltRelE    = AliensFiltRelE;
    filter->CtrlFiltRelN    = AliensFiltRelN;
    filter->CtrlFiltRelP    = AliensFiltRelP;
    filter->CtrlNumRows     = AliensNumRows;

    // Store objects
    AliensGrid->Filter = filter;
    AliensGrid->Sorter = sorter;

    filter->ResetControls(false);
}

void Form1::AliensUpdateControls()
{
    AliensGrid->Filter->EnableUpdates = true;
}

void Form1::AliensFillGrid()
{
    AliensGrid->FullUpdateBegin();

    AliensColumns %c = m_AliensColumns;

    for each( Alien ^alien in m_GameData->GetAliens() )
    {
        if( AliensGrid->Filter->Filter(alien) )
            continue;

        DataGridViewRow ^row = AliensGrid->Rows[ AliensGrid->Rows->Add() ];
        DataGridViewCellCollection ^cells = row->Cells;
        cells[c.Object]->Value      = alien;
        cells[c.Name]->Value        = alien->Name;
        cells[c.Relation]->Value    = alien->PrintRelation();
        if( alien->TurnMet > 0 )
            cells[c.TurnMet]->Value = alien->TurnMet;
        cells[c.Home]->Value        = alien->PrintHome();
        cells[c.TechLev]->Value     = alien->PrintTechLevels();
        if( alien->HomeSystem )
        {
            cells[c.Dist]->Value    = GameData::Player->HomeSystem->CalcDistance( alien->HomeSystem );
        }
        if( alien->AtmReq->TempClass != -1 &&
            alien->AtmReq->PressClass != -1 )
        {
            cells[c.TC]->Value      = alien->AtmReq->TempClass;
            cells[c.PC]->Value      = alien->AtmReq->PressClass;
        }
        cells[c.Atmosphere]->Value  = alien->PrintAtmosphere();
        cells[c.EMail]->Value       = alien->Email;

        // Teach orders
        String ^teach = "";
        for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
        {
            if( cmd->GetCmdType() == CommandType::Teach )
            {
                CmdTeach ^cmdTeach = safe_cast<CmdTeach^>(cmd);
                if( cmdTeach->m_Alien == alien )
                {
                    teach += String::Format("{0}{1}{2}",
                        String::IsNullOrEmpty(teach) ? "" : ", ",
                        FHStrings::TechToString(cmdTeach->m_Tech),
                        cmdTeach->m_Level > 0 ? "(" + cmdTeach->m_Level.ToString() + ")" : "" );
                }
            }
        }
        if( !String::IsNullOrEmpty(teach) )
            cells[c.Teach]->Value = teach;

        // Message
        if( alien != GameData::Player )
        {
            String ^msgCell = "";
            String ^msgToolTip = "";
            for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
            {
                if( cmd->GetCmdType() == CommandType::Message )
                {
                    CmdMessage ^cmdMsg = safe_cast<CmdMessage^>(cmd);
                    if( cmdMsg->m_Alien == alien )
                    {
                        msgToolTip = "Message to be sent:\r\n" + cmdMsg->m_Text;
                        msgCell = "Send NEW";
                        break;
                    }
                }
            }
            if( alien->LastMessageSentTurn )
            {
                if( !String::IsNullOrEmpty(msgToolTip) )
                {
                    msgToolTip += "\r\n--------------------------------------------------\r\n";
                    msgCell += ", ";
                }
                msgCell += "S: " + alien->LastMessageSentTurn.ToString();
                msgToolTip += "Last message sent in turn " + alien->LastMessageSentTurn.ToString() + "\r\n";
            }
            if( alien->LastMessageRecv )
            {
                if( !String::IsNullOrEmpty(msgToolTip) )
                {
                    msgToolTip += "\r\n--------------------------------------------------\r\n";
                    msgCell += ", ";
                }
                msgToolTip += "Last message received in turn "
                    + alien->LastMessageRecvTurn.ToString() + ":\r\n";
                msgToolTip += alien->LastMessageRecv;
                msgCell += "R: " + alien->LastMessageRecvTurn.ToString();
                if( alien->LastMessageRecvTurn == GameData::CurrentTurn - 1 )
                {
                    msgCell += " (New)";
                    cells[c.Message]->Style->ForeColor = Color::Red;
                }
            }
            if( !String::IsNullOrEmpty(msgCell) )
            {
                cells[c.Message]->Value = msgCell;
                cells[c.Message]->ToolTipText = msgToolTip;
            }
        }

        for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
            plugin->AddRowData(row, AliensGrid->Filter, alien);
    }

    // Setup tooltips
    SetGridBgAndTooltip(AliensGrid);

    AliensGrid->FullUpdateEnd();
}

void Form1::AliensFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = AliensGrid->Columns[0]->Index;
    Alien ^alien = safe_cast<Alien^>(AliensGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_AliensMenuRef = alien;

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::AliensFiltersReset_Click));

    if( alien->TurnMet == 0 )
        return;

    if( alien->Relation == SP_NEUTRAL ||
        alien->Relation == SP_ENEMY ||
        alien->Relation == SP_ALLY )
    {
        menu->Items->Add( gcnew ToolStripSeparator );

        AliensFillMenuRelations(menu);
        AliensFillMenuMessage(menu);
        if( alien->Relation != SP_ENEMY )
            AliensFillMenuTeach(menu);
        AliensFillMenuEstimate(menu);
    }
}

void Form1::AliensFillMenuRelations(Windows::Forms::ContextMenuStrip ^menu)
{
    Alien ^alien = m_AliensMenuRef;

    if( alien->Relation != SP_NEUTRAL )
    {   // Declare neutrality
        ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
            "Declare Neutral",
            gcnew AlienRelationData(alien, SP_NEUTRAL),
            gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
        menu->Items->Add( menuItem );
    }
    if( alien->Relation != SP_ALLY )
    {   // Declare alliance
        ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
            "Declare Ally",
            gcnew AlienRelationData(alien, SP_ALLY),
            gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
        menu->Items->Add( menuItem );
    }
    if( alien->Relation != SP_ENEMY )
    {   // Declare enemy
        ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
            "Declare Enemy",
            gcnew AlienRelationData(alien, SP_ENEMY),
            gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
        menu->Items->Add( menuItem );
    }
}

void Form1::AliensFillMenuMessage(Windows::Forms::ContextMenuStrip ^menu)
{
    Alien ^alien = m_AliensMenuRef;

    for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
    {
        if( cmd->GetCmdType() == CommandType::Message )
        {
            CmdMessage ^cmdMsg = safe_cast<CmdMessage^>(cmd);
            if( cmdMsg->m_Alien == alien )
            {
                ToolStripMenuItem ^msgMenu = gcnew ToolStripMenuItem("Message:");

                msgMenu->DropDownItems->Add( "Edit",
                    nullptr,
                    gcnew EventHandler(this, &Form1::AliensMenuCommandMessage));

                msgMenu->DropDownItems->Add( "Cancel",
                    nullptr,
                    gcnew EventHandler(this, &Form1::AliensMenuCommandMessageCancel));

                menu->Items->Add(msgMenu);

                return;
            }
        }
    }

    menu->Items->Add( "Send Message...",
        nullptr,
        gcnew EventHandler(this, &Form1::AliensMenuCommandMessage) );
}

void Form1::AliensMenuCommandMessageCancel(Object^, EventArgs^)
{
    Alien ^alien = m_AliensMenuRef;

    for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
    {
        if( cmd->GetCmdType() == CommandType::Message )
        {
            CmdMessage ^cmdMsg = safe_cast<CmdMessage^>(cmd);
            if( cmdMsg->m_Alien == alien )
            {
                m_CommandMgr->DelCommand(cmd);
                AliensGrid->MarkForUpdate();
                return;
            }
        }
    }
}

void Form1::AliensMenuCommandMessage(Object^, EventArgs^)
{
    Alien ^alien = m_AliensMenuRef;

    CmdMessage ^cmdMsg = nullptr;

    for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
    {
        if( cmd->GetCmdType() == CommandType::Message )
        {
            cmdMsg = safe_cast<CmdMessage^>(cmd);
            if( cmdMsg->m_Alien != alien )
                cmdMsg = nullptr;
            else
                break;
        }
    }

    CmdMessageDlg ^dlg = gcnew CmdMessageDlg( cmdMsg ? cmdMsg->m_Text : nullptr );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        String ^text = dlg->GetMessage();
        if( !String::IsNullOrEmpty(text) )
        {
            if( cmdMsg )
            {
                cmdMsg->m_Text = text;
                m_CommandMgr->SaveCommands();
            }
            else
            {
                m_CommandMgr->AddCommand( gcnew CmdMessage(alien, text) );
                AliensGrid->MarkForUpdate();
            }
        }
        else
            AliensMenuCommandMessageCancel(nullptr, nullptr);
    }
}

void Form1::AliensFillMenuTeach(Windows::Forms::ContextMenuStrip ^menu)
{
    Alien ^alien = m_AliensMenuRef;

    ToolStripMenuItem ^teachMenu = nullptr;

    bool teachAny = false;
    for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
    {
        if( teachAny == false )
            teachMenu = gcnew ToolStripMenuItem("Teach:");

        if( cmd->GetCmdType() == CommandType::Teach )
        {
            CmdTeach ^cmdTeach = safe_cast<CmdTeach^>(cmd);
            if( cmdTeach->m_Alien == alien )
            {
                ToolStripMenuItem ^teachItem = AliensMenuCreateTeach(cmdTeach);
                teachMenu->DropDownItems->Add(teachItem);
                teachAny = true;
            }
        }
    }

    if( teachAny == false )
    {
        // Teach dialog
        menu->Items->Add( CreateCustomMenuItem<CmdTeach^>(
            "Teach...",
            safe_cast<CmdTeach^>(nullptr),
            gcnew EventHandler1Arg<CmdTeach^>(this, &Form1::AliensMenuCommandTeach) ) );
        return;
    }

    teachMenu->DropDownItems->Insert(0, gcnew ToolStripSeparator );
    teachMenu->DropDownItems->Insert(0, CreateCustomMenuItem<CmdTeach^>(
        "Teach...",
        safe_cast<CmdTeach^>(nullptr),
        gcnew EventHandler1Arg<CmdTeach^>(this, &Form1::AliensMenuCommandTeach) ) );

    teachMenu->DropDownItems->Add( gcnew ToolStripSeparator );
    teachMenu->DropDownItems->Add( CreateCustomMenuItem<CmdTeach^>(
        "Cancel ALL",
        safe_cast<CmdTeach^>(nullptr),
        gcnew EventHandler1Arg<CmdTeach^>(this, &Form1::AliensMenuCommandTeachCancel) ) );
    
    menu->Items->Add( teachMenu );
}

ToolStripMenuItem^ Form1::AliensMenuCreateTeach(CmdTeach ^cmd)
{
    ToolStripMenuItem ^teachItem = gcnew ToolStripMenuItem( cmd->Print() );

    teachItem->DropDownItems->Add( CreateCustomMenuItem<CmdTeach^>(
        "Edit...",
        cmd,
        gcnew EventHandler1Arg<CmdTeach^>(this, &Form1::AliensMenuCommandTeach) ) );
    teachItem->DropDownItems->Add( CreateCustomMenuItem<CmdTeach^>(
        "Cancel",
        cmd,
        gcnew EventHandler1Arg<CmdTeach^>(this, &Form1::AliensMenuCommandTeachCancel) ) );

    return teachItem;
}

void Form1::AliensMenuCommandTeach(CmdTeach ^cmd)
{
    Alien ^alien = m_AliensMenuRef;
    if( cmd && ! alien )    // called from systems menu
        alien = cmd->m_Alien;
    if( ! alien )
        throw gcnew FHUIDataIntegrityException("Missing alien for teach command!");

    CmdTeachDlg ^dlg = gcnew CmdTeachDlg( alien );

    array<CmdTeach^> ^cmds = gcnew array<CmdTeach^>(TECH_MAX) { nullptr };
    for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
    {
        if( iCmd->GetCmdType() == CommandType::Teach )
        {
            cmd = safe_cast<CmdTeach^>(iCmd);
            if( cmd->m_Alien == m_AliensMenuRef )
            {
                cmds[cmd->m_Tech] = cmd;
                dlg->AddCommand(cmd);
            }
        }
    }

    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        bool needSave = false;
        for( int i = 0; i < TECH_MAX; ++i )
        {
            TechType tech = static_cast<TechType>(i);
            cmd = cmds[i];
            if( cmd )
            {
                int level = dlg->GetLevel( tech );
                if( level == -1 )
                    m_CommandMgr->DelCommand(cmd);
                else if( level != cmd->m_Level )
                {
                    needSave = true;
                    cmd->m_Level = level;
                }
            }
            else
            {
                cmd = dlg->GetCommand( tech );
                if( cmd )
                {
                    m_CommandMgr->AddCommand( cmd );
                    needSave = false;
                }
            }
        }
        if( needSave )
            m_CommandMgr->SaveCommands();

        AliensGrid->MarkForUpdate();
    }
}

void Form1::AliensMenuCommandTeachCancel(CmdTeach ^cmd)
{
    if( cmd )
    {   // Delete single command
        m_CommandMgr->DelCommand( cmd );
    }
    else
    {   // Delete all
        bool bRepeat;
        do
        {
            bRepeat = false;
            for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
            {
                if( cmd->GetCmdType() == CommandType::Teach &&
                    safe_cast<CmdTeach^>(cmd)->m_Alien == m_AliensMenuRef )
                {
                    m_CommandMgr->DelCommand( cmd );
                    bRepeat = true;
                    break;
                }
            }
        } while( bRepeat );
    }

    AliensGrid->MarkForUpdate();
}

void Form1::AliensMenuSetRelation(AlienRelationData ^data)
{
    Alien ^alien = data->A;
    SPRelType rel = (SPRelType)data->B;

    bool addNew = true;

    // Modify existing relation command
    for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
    {
        if( iCmd->GetCmdType() == CommandType::AlienRelation )
        {
            CmdAlienRelation ^cmd = safe_cast<CmdAlienRelation^>(iCmd);
            if( cmd->m_Alien == alien )
            {
                if( rel == alien->RelationParsed )
                {
                    m_CommandMgr->DelCommand(iCmd);
                }
                else
                {
                    cmd->m_Relation = rel;
                    m_CommandMgr->SaveCommands();
                }
                addNew = false;
                break;
            }
        }
    }

    if( addNew )
    {
        // Add relation command
        alien->RelationParsed = alien->Relation;
        m_CommandMgr->AddCommand( gcnew CmdAlienRelation(alien, rel) );
    }

    // Set relation
    alien->Relation = rel;

    // Cancel any teach commands for enemy species
    if( rel == SP_ENEMY )
        AliensMenuCommandTeachCancel(nullptr);

    // Update other grids to reflect new colors
    UpdateAllGrids(false);
}

void Form1::AliensFillMenuEstimate(Windows::Forms::ContextMenuStrip ^menu)
{
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetCmdType() == CommandType::Estimate )
            {
                ProdCmdEstimate ^estCmd = safe_cast<ProdCmdEstimate^>(cmd);
                if( estCmd->m_Alien == m_AliensMenuRef )
                {
                    ToolStripMenuItem ^estMenu = gcnew ToolStripMenuItem( "Estimate on PL " + colony->Name );
                    estMenu->DropDownItems->Add( CreateCustomMenuItem<ProdCmdEstimate^>(
                        "Cancel",
                        estCmd,
                        gcnew EventHandler1Arg<ProdCmdEstimate^>(this, &Form1::AliensMenuCommandEstimateCancel) ) );
                    menu->Items->Add( estMenu );
                    return;
                }
            }
        }
    }

    Colony ^home = GameData::Player->FindColony(
        GameData::Player->HomeSystem->Planets[ GameData::Player->HomePlanet ]->Name,
        true );
    if( home && home->EconomicBase > 0 )
    {
        menu->Items->Add(
            "Add Estimate on PL " + home->Name,
            nullptr,
            gcnew EventHandler(this, &Form1::AliensMenuCommandEstimate) );
    }
}

void Form1::AliensMenuCommandEstimate(Object^, EventArgs^)
{
    Colony ^home = GameData::Player->FindColony(
        GameData::Player->HomeSystem->Planets[ GameData::Player->HomePlanet ]->Name,
        false );

    m_CommandMgr->AddCommand(
        home,
        gcnew ProdCmdEstimate( m_AliensMenuRef ) );

    ColoniesGrid->MarkForUpdate();
}

void Form1::AliensMenuCommandEstimateCancel(ProdCmdEstimate ^cmd)
{
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        for each( ICommand ^iCmd in colony->Commands )
        {
            if( iCmd == cmd )
            {
                m_CommandMgr->DelCommand( colony, cmd );
                ColoniesGrid->MarkForUpdate();
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
