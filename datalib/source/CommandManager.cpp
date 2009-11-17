#include "StdAfx.h"
#include "enums.h"
#include "RegexMatcher.h"
#include "CommandManager.h"
#include "PluginManager.h"
#include "Commands.h"
#include "GameData.h"
#include "IFHUIPlugin.h"

using namespace System::Drawing;

namespace FHUI
{

// ---------------------------------------------------------

CommandManager::CommandManager(GameData^ gd, String^ path)
    : m_RM(gcnew RegexMatcher)
    , m_bSaveEnabled(true)
{
    m_GameData = gd;
    m_Path = path;
    m_OrderList = gcnew List<String^>;
    m_CommandData = gcnew SortedList<int, TurnCommands^>;
}

// ---------------------------------------------------------

void CommandManager::SelectTurn(int turn)
{
    m_CurrentTurn = turn;
    if ( ! m_CommandData->ContainsKey(turn) )
    {
        m_CommandData[turn] = gcnew TurnCommands;
        // Disable saves until commands file is loaded
        m_bSaveEnabled = false;
    }
}

// ---------------------------------------------------------

String^ CommandManager::PrintCommandWithInfo(ICommand ^cmd, int indent)
{
    String ^indentStr = String::Format("{0}0,{1}{2}", "{", indent, "}");
    String ^ret = String::Format(indentStr + "{1}", "", cmd->Print());

    bool addSemicolon = true;
    int eu = cmd->GetEUCost();
    if( eu != 0 )
    {
        ret += String::Format("  ; {0}{1} EU", eu > 0 ? "" : "+", -eu);
        addSemicolon = false;
    }

    int pop = cmd->GetPopCost();
    if( pop != 0 )
    {
        ret += String::Format("{0} {1}{2} Pop",
            addSemicolon ? "  ;" : ",",
            pop > 0 ? "" : "+",
            -pop );
        addSemicolon = false;
    }

    for( int i = 0; i < INV_MAX; ++i )
    {
        InventoryType it = static_cast<InventoryType>(i);
        int invMod = cmd->GetInvMod(it);
        if( invMod != 0 )
        {
            ret += String::Format("{0} {1}{2} {3}",
                addSemicolon ? "  ;" : ",",
                invMod > 0 ? "+" : "",
                invMod,
                FHStrings::InvToString(it));
            addSemicolon = false;
        }
    }

    return ret + cmd->PrintOriginSuffix();
}

// ---------------------------------------------------------

void CommandManager::AddCommand(ICommand ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::Production )
        throw gcnew FHUIDataImplException("AddCommand: Wrong interface for production command!");

    if( cmd->GetCmdType() == CommandType::Transfer )
        cmd->GetRefSystem()->Transfers->Add( cmd );
    else
        m_CommandData[m_CurrentTurn]->Commands->Add(cmd);

    if( m_bSaveEnabled )
        SaveCommands();
}

void CommandManager::DelCommand(ICommand ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::Production )
        throw gcnew FHUIDataImplException("DelCommand: Wrong interface for production command!");

    if( cmd->GetCmdType() == CommandType::Transfer )
    {
        cmd->GetRefSystem()->Transfers->Remove( cmd );
        SaveCommands();
        return;
    }

    for each( ICommand ^iCmd in m_CommandData[m_CurrentTurn]->Commands )
    {
        if( iCmd == cmd ||
            iCmd->CompareTo(cmd) == 0 )
        {
            m_CommandData[m_CurrentTurn]->Commands->Remove(iCmd);
            SaveCommands();
            return;
        }
    }
}

void CommandManager::AddCommand(Colony ^colony, ICommand ^cmd)
{
    if( cmd->GetCmdType() == CommandType::Transfer )
        AddCommand(cmd);
    else
    {
        colony->Commands->Add(cmd);
        if( m_bSaveEnabled )
            SaveCommands();

        if( cmd->GetCmdType() == CommandType::BuildShip )
        {
            ProdCmdBuildShip ^shipCmd = safe_cast<ProdCmdBuildShip^>(cmd);
            Ship ^ship = GameData::CreateShip(
                GameData::Player,
                shipCmd->m_Type,
                shipCmd->m_Name,
                shipCmd->m_Sublight,
                colony->System );
            ship->Location = SHIP_LOC_LANDED;
            ship->PlanetNum = colony->PlanetNum;
            ship->CanJump = false;
            ship->BuiltThisTurn = true;
            if( ship->Type == SHIP_TR || ship->Type == SHIP_BAS )
                ship->Size = shipCmd->m_Size;
            ship->CalculateCapacity();
        }
    }
}

void CommandManager::DelCommand(Colony ^colony, ICommand ^cmd)
{
    if( cmd->GetCmdType() == CommandType::Transfer )
    {
        DelCommand(cmd);
        return;
    }

    for each( ICommand ^iCmd in colony->Commands )
    {
        if( iCmd == cmd ||
            iCmd->CompareTo(cmd) == 0 )
        {
            colony->Commands->Remove(iCmd);

            if( cmd->GetCmdType() == CommandType::BuildShip )
            {
                ProdCmdBuildShip ^shipCmd = safe_cast<ProdCmdBuildShip^>(cmd);
                Ship ^ship = nullptr;
                for each( Ship ^iShip in GameData::Player->Ships )
                {
                    if( iShip->Name == shipCmd->m_Name )
                    {
                        if( iShip->BuiltThisTurn == false )
                            throw gcnew FHUIDataIntegrityException("Deleting build ship command, but ship is not marked as BuiltThisTurn!");
                        ship = iShip;
                        break;
                    }
                }

                if( ship )
                {
                    GameData::RemoveShip( ship );
                    RemoveShipDependentCommands(ship);
                }
            }

            SaveCommands();

            return;
        }
    }
}

void CommandManager::RemoveShipDependentCommands(Ship ^ship)
{
    // Global commands
    RemoveShipDependentCommands(ship, GetCommands());
    // Systems: transfers
    for each( StarSystem ^system in GameData::GetStarSystems() )
        RemoveShipDependentCommands(ship, system->Transfers);
    // Colonies
    for each( Colony ^colony in GameData::Player->Colonies )
        RemoveShipDependentCommands(ship, colony->Commands);
}

void CommandManager::RemoveShipDependentCommands(Ship ^ship, List<ICommand^> ^commands)
{
    List<ICommand^> ^rmCmds = gcnew List<ICommand^>;
    for each( ICommand ^cmd in commands )
        if( cmd->IsUsingShip(ship) )
            rmCmds->Add(cmd);

    for each( ICommand ^cmd in rmCmds )
        commands->Remove(cmd);
}

List<ICommand^>^ CommandManager::GetCommands(Colony ^colony)
{
    return colony->Commands;
}

void CommandManager::DeleteCommands()
{
    FileInfo ^fileInfo = gcnew FileInfo(
        m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn) );
    if( fileInfo->Exists )
        fileInfo->Delete();
}

String^ CommandManager::PrintCommandToFile(ICommand ^cmd)
{
    String ^prefix = "";

    switch( cmd->Origin )
    {
    case CommandOrigin::Auto:   prefix += "[A] "; break;
    case CommandOrigin::Plugin: prefix += "[P] "; break;
    default:
        break;
    }

    if( cmd->RequiresPhasePrefix )
    {
        switch( cmd->GetPhase() )
        {
        case CommandPhase::Combat:          prefix += "{C} "; break;
        case CommandPhase::PreDeparture:    prefix += "{D} "; break;
        case CommandPhase::Jump:            prefix += "{J} "; break;
        case CommandPhase::Production:      prefix += "{P} "; break;
        case CommandPhase::PostArrival:     prefix += "{A} "; break;
        case CommandPhase::Strike:          prefix += "{S} "; break;
        default:
            throw gcnew FHUIDataIntegrityException("Invalid command phase: " + ((int)cmd->GetPhase()).ToString());
        }
    }

    if( cmd->GetCmdType() == CommandType::Custom )
        prefix += "CUSTOM " + cmd->GetEUCost().ToString() + " ";

    return prefix + cmd->Print();
}

void CommandManager::SaveCommands()
{
    // Create directory
    DirectoryInfo ^dirInfo = gcnew DirectoryInfo(m_Path);
    if( !dirInfo->Exists )
    {
        dirInfo->Create();
    }

    // Create stream
    StreamWriter ^sw = File::CreateText( m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn) );

    // Header
    sw->WriteLine("; FHUI generated file. Please don't edit.");

    // Prepare commands
    List<String^>^ commandList = gcnew List<String^>;

    // -- Colonies
    GameData::Player->SortColoniesByProdOrder();
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        if( colony->IsNew )
        {
            // Fresh colony
            // Put the NAME command outside of section
            for each( ICommand ^cmd in colony->Commands )
            {
                if( cmd->GetCmdType() == CommandType::Name )
                {
                    commandList->Add( PrintCommandToFile(cmd) );
                }
            }

            if( colony->Commands->Count > 1 )
            {
                commandList->Add( "COLONY " + colony->Name );
                for each( ICommand ^cmd in colony->Commands )
                {
                    if( cmd->GetCmdType() != CommandType::Name )
                    {
                        commandList->Add( PrintCommandToFile(cmd) );
                    }
                }
                commandList->Add( "END_COLONY" );
            }
        }
        else
        {
            // Established colony
            commandList->Add( "COLONY " + colony->Name );
            for each( ICommand ^cmd in colony->Commands )
            {
                commandList->Add( PrintCommandToFile(cmd) );
            }
            commandList->Add( "END_COLONY" );
        }
    }

    // -- Ships
    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->Commands->Count == 0 )
            continue;

        commandList->Add( "SHIP " + ship->Name );
        for each( ICommand ^cmd in ship->Commands )
        {
            commandList->Add( PrintCommandToFile(cmd) );
        }
        commandList->Add( "END_SHIP" );
    }

    // -- Transfers
    for each( StarSystem ^system in GameData::GetStarSystems() )
    {
        for each( CmdTransfer ^cmd in system->Transfers )
            commandList->Add( PrintCommandToFile(cmd) );
    }

    // -- Commands
    for each( ICommand ^cmd in GetCommands() )
    {
        commandList->Add( PrintCommandToFile(cmd) );
    }

    // Auto
    commandList->Add( AutoEnabled ? "AUTO" : "NO_AUTO" );

    // Write to stream
    for each( String ^cmd in commandList )
    {
        sw->WriteLine(cmd);
    }
    sw->Close();
}

// ---------------------------------------------------------

ICommand^ CommandManager::CmdSetOrigin(ICommand ^cmd)
{
    cmd->Origin = m_CmdOrigin;

    if( m_CmdPhase != CommandPhase::Custom &&
        m_CmdPhase != cmd->GetPhase() )
    {
        cmd = gcnew CmdPhaseWrapper(m_CmdPhase, cmd);
    }

    if( cmd->GetPhase() == CommandPhase::Custom )
        throw gcnew FHUIParsingException("Command loaded with undefined phase!");

    return cmd;
}

void CommandManager::LoadCommands()
{
    AddPluginCommands();

    String^ cmdPath = m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn);
    FileInfo ^fileInfo = gcnew FileInfo(cmdPath);

    if( fileInfo->Exists )
    {
        // Open file
        StreamReader ^sr = File::OpenText( cmdPath );

        RemoveGeneratedCommands(CommandOrigin::Auto, false, false);
        RemoveGeneratedCommands(CommandOrigin::Plugin, false, false);

        try
        {
            LoadCommandsGlobal(sr);
        }
        catch( Exception ^e )
        {
            throw gcnew FHUIParsingException( String::Format(
                    "Failed loading internal commands file:\n"
                    "File: {0}\n"
                    "Line {1}: {2}\n",
                    cmdPath,
                    m_LastLineNr,
                    m_LastLine ),
                e );
        }
        finally
        {
            sr->Close();
        }
    }

    m_bSaveEnabled = true;
}

void CommandManager::LoadCommandsGlobal(StreamReader ^sr)
{
    String ^line;
    Colony^ refColony = nullptr;
    Ship^ refShip = nullptr;
    String ^messageText = "";
    Alien ^messageTarget = nullptr;

    m_LastLineNr = 0;

    int colonyProdOrder = 1;
    while( (m_LastLine = sr->ReadLine()) != nullptr ) 
    {
        ++m_LastLineNr;

        line = m_LastLine->Trim();
        if( String::IsNullOrEmpty(line) ||
            line[0] == ';' )
            continue;

        m_CmdOrigin = CommandOrigin::GUI;
        if( line->Length > 4 )
        {
            String ^prefix = line->Substring(0, 4);
            if( prefix == "[A] " )
                m_CmdOrigin = CommandOrigin::Auto;
            else if( prefix == "[P] " )
                m_CmdOrigin = CommandOrigin::Plugin;

            if( m_CmdOrigin != CommandOrigin::GUI )
                line = line->Substring(4);
        }

        m_CmdPhase = CommandPhase::Custom;
        if( line->Length > 4 )
        {
            String ^prefix = line->Substring(0, 4);
            if( prefix == "{C} " )
                m_CmdPhase = CommandPhase::Combat;
            else if( prefix == "{D} " )
                m_CmdPhase = CommandPhase::PreDeparture;
            else if( prefix == "{J} " )
                m_CmdPhase = CommandPhase::Jump;
            else if( prefix == "{P} " )
                m_CmdPhase = CommandPhase::Production;
            else if( prefix == "{A} " )
                m_CmdPhase = CommandPhase::PostArrival;
            else if( prefix == "{S} " )
                m_CmdPhase = CommandPhase::Strike;

            if( m_CmdPhase != CommandPhase::Custom )
                line = line->Substring(4);
        }

        if( refColony )
        {
            if( LoadCommandsColony(line, refColony) )
                continue;
        }
        if( refShip )
        {
            if( LoadCommandsShip(line, refShip) )
                continue;
        }
        if( messageTarget )
        {
            if( line == "Zzz" )
            {
                if( messageTarget )
                {
                    AddCommand( CmdSetOrigin( gcnew CmdMessage(messageTarget, messageText) ) );
                    messageTarget = nullptr;
                    messageText = "";
                }
                else
                    throw gcnew FHUIParsingException("Message termination without message body!");
            }
            else
                messageText += line + "\r\n";
            continue;
        }

        if( m_RM->Match(line, m_RM->ExpCmdColony) )
        {
            Colony ^colony = GameData::Player->FindColony(m_RM->Results[0], false);
            if( colony->Owner == GameData::Player)
            {
                refColony = colony;
                if( colony->EconomicBase > 0 )
                {
                    colony->ProductionOrder = colonyProdOrder++;
                }
            }
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (colony production order)!");
        }
        else if( line == "END_COLONY" )
        {
            if( refColony )
                refColony = nullptr;
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (END_COLONY)!");
        }
    // --- obsolete ship commands ---
        else if( m_RM->Match(line, m_RM->ExpCmdShipJump_Obsolete) )
        {
            Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
            int x = m_RM->GetResultInt(1);
            int y = m_RM->GetResultInt(2);
            int z = m_RM->GetResultInt(3);
            int p = m_RM->GetResultInt(4);
            StarSystem ^target = nullptr;
            if( x != 0 || y != 0 || z != 0 )
                target = m_GameData->GetStarSystem(x, y, z, true);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdJump(ship, target, p)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipWormhole_Obsolete) )
        {
            Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
            if( !ship->System->HasWormhole )
                throw gcnew FHUIParsingException("Invalid ship wormhole command (no wormhole in system)!");
            int p = m_RM->GetResultInt(1);
            ship->AddCommand(
                CmdSetOrigin(gcnew ShipCmdWormhole(ship, ship->System->GetWormholeTarget(), p)) );
            if( p != -1 &&
                ship->System->Planets->ContainsKey( p ) == false )
                throw gcnew FHUIParsingException("Invalid ship wormhole command (invalid planet for orbiting)!");
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipUpg_Obsolete) )
        {
            Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdUpgrade(ship)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipRec_Obsolete) )
        {
            Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdRecycle(ship)) );
        }
    // --- end obsolete ship commands ---
        else if( m_RM->Match(line, m_RM->ExpCmdShip) )
        {
            Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
            if( ship->Owner == GameData::Player )
                refShip = ship;
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (ships)!");
        }
        else if( line == "END_SHIP" )
        {
            if( refShip )
                refShip = nullptr;
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (END_SHIP)!");
        }
        else if( line == "AUTO" )
            AutoEnabled = true;
        else if( line == "NO_AUTO" )
            AutoEnabled = false;
        else if( m_RM->Match(line, m_RM->ExpCmdPLName) )
        {
            StarSystem ^system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                false);
            Planet ^planet = system->Planets[ m_RM->GetResultInt(3) ];
            String ^name = m_RM->Results[4];
            Colony ^colony = GameData::CreateColony(GameData::Player, name, system, planet->Number, false);
            colony->PlanetType = PLANET_COLONY;
            AddCommand( colony, CmdSetOrigin(gcnew CmdPlanetName(system, planet->Number, name)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdPLDisband) )
        {
            AddCommand(
                GameData::Player->FindColony( m_RM->Results[0], false ),
                CmdSetOrigin(gcnew CmdDisband( m_RM->Results[0] )) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPNeutral) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_ENEMY &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (neutral)!");

            alien->Relation = SP_NEUTRAL;
            AddCommand( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_NEUTRAL)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPAlly) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            alien->Relation = SP_ALLY;
            AddCommand( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_ALLY)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPEnemy) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            alien->Relation = SP_ENEMY;
            AddCommand( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_ENEMY)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPTeach) )
        {
            TechType tech = FHStrings::TechFromString(m_RM->Results[0]);
            int level = m_RM->GetResultInt(1);
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[2]);
            AddCommand( CmdSetOrigin(gcnew CmdTeach(alien, tech, level)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPMsg) )
        {
            messageTarget = GameData::GetAlien(m_RM->Results[0]);
        }
        else if( m_RM->Match(line, m_RM->ExpCmdVisited) )
        {
            AddCommand( CmdSetOrigin(gcnew CmdVisited(
                GameData::GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                false ) ) ) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdTransfer) )
        {
            int amount = m_RM->GetResultInt(0);
            InventoryType inv = FHStrings::InvFromString(m_RM->Results[1]);

            Colony  ^fromColony = nullptr;
            Ship    ^fromShip = nullptr;
            Colony  ^toColony = nullptr;
            Ship    ^toShip = nullptr;
            // Parse source
            if( m_RM->Match(line, m_RM->ExpCmdTargetColony) )
                fromColony = GameData::Player->FindColony(m_RM->Results[0]->Trim(), false);
            else if( m_RM->Match(line, m_RM->ExpCmdTargetShip) )
                fromShip = GameData::Player->FindShip(m_RM->Results[0]->Trim(), false);
            else
                throw gcnew FHUIParsingException("Unrecognized source for transfer command: " + line);
            // Skip ","
            if( m_RM->Match(line, "\\s*,\\s*") == false )
                throw gcnew FHUIParsingException("Missing source/target separator for transfer command: " + line);
            // Parse target
            if( m_RM->Match(line, m_RM->ExpCmdTargetColony) )
                toColony = GameData::Player->FindColony(m_RM->Results[0]->Trim(), false);
            else if( m_RM->Match(line, m_RM->ExpCmdTargetShip) )
                toShip = GameData::Player->FindShip(m_RM->Results[0]->Trim(), false);
            else
                throw gcnew FHUIParsingException("Unrecognized target for transfer command: " + line);

            AddCommand( CmdSetOrigin(
                gcnew CmdTransfer(m_CmdPhase, inv, amount, fromColony, fromShip, toColony, toShip) ) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdCustom) )
        {
            AddCommand( CmdSetOrigin(
                gcnew CmdCustom(m_CmdPhase, m_RM->Results[1], m_RM->GetResultInt(0))) );
        }
        else
            throw gcnew FHUIParsingException("Unrecognized line in commands template: " + line);
    }
}

bool CommandManager::LoadCommandsColony(String ^line, Colony ^colony)
{
    // Shipyard
    if( line == "Shipyard" )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdShipyard) );
        return true;
    }
    // Hide
    if( line == "Hide" )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdHide(colony)) );
        return true;
    }

    // Research
    if( m_RM->Match(line, m_RM->ExpCmdResearch) )
    {
        int amount = m_RM->GetResultInt(0);
        TechType tech = FHStrings::TechFromString(m_RM->Results[1]);

        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdResearch(tech, amount)) );
        return true;
    }

    // Build CU/IU/AU
    if( m_RM->Match(line, m_RM->ExpCmdBuildIUAU) )
    {
        int amount = m_RM->GetResultInt(0);
        InventoryType inv = FHStrings::InvFromString(m_RM->Results[1]);

        Colony ^targetColony = nullptr;
        Ship ^targetShip = nullptr;
        line = line->Trim();
        if( !String::IsNullOrEmpty(line) )
        {
            if( m_RM->Match(line, m_RM->ExpCmdTargetColony) )
            {
                targetColony = GameData::Player->FindColony( m_RM->Results[0], false );
            }
            else if( m_RM->Match(line, m_RM->ExpCmdTargetShip) )
            {
                targetShip = GameData::Player->FindShip( m_RM->Results[0], false );
            }
            else
            {
                throw gcnew FHUIParsingException("Invalid build target: " + line);
            }
        }
        AddCommand(colony, CmdSetOrigin(
            gcnew ProdCmdBuildIUAU(
                amount,
                inv,
                targetColony,
                targetShip) ) );
        return true;
    }

    // Build Inventory
    if( m_RM->Match(line, m_RM->ExpCmdBuildInv) )
    {
        int amount = m_RM->GetResultInt(0);
        InventoryType inv = FHStrings::InvFromString(m_RM->Results[1]);

        Colony ^targetColony = nullptr;
        Ship ^targetShip = nullptr;
        line = line->Trim();
        if( !String::IsNullOrEmpty(line) )
        {
            if( m_RM->Match(line, m_RM->ExpCmdTargetColony) )
            {
                targetColony = GameData::Player->FindColony( m_RM->Results[0], false );
            }
            else if( m_RM->Match(line, m_RM->ExpCmdTargetShip) )
            {
                targetShip = GameData::Player->FindShip( m_RM->Results[0], false );
            }
            else
            {
                throw gcnew FHUIParsingException("Invalid inventory build target: " + line);
            }
        }
        AddCommand(colony, CmdSetOrigin(
            gcnew ProdCmdBuildInv(
                amount,
                inv,
                targetColony,
                targetShip) ) );
        return true;
    }

    // Build warship
    if( m_RM->Match(line, m_RM->ExpCmdBuildShip) )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdBuildShip(
            FHStrings::ShipFromString(m_RM->Results[0]),
            0,
            String::IsNullOrEmpty(m_RM->Results[1]) == false,
            m_RM->Results[2] )) );
        return true;
    }

    // Build transport ship
    if( m_RM->Match(line, m_RM->ExpCmdBuildShipTR) )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdBuildShip(
            SHIP_TR,
            m_RM->GetResultInt(0),
            String::IsNullOrEmpty(m_RM->Results[1]) == false,
            m_RM->Results[2] )) );
        return true;
    }

    // Recycle
    if( m_RM->Match(line, m_RM->ExpCmdRecycle) )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdRecycle(
            FHStrings::InvFromString( m_RM->Results[1] ),
            m_RM->GetResultInt(0) ) ) );
        return true;
    }

    // Estimate
    if( m_RM->Match(line, m_RM->ExpCmdEstimate) )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdEstimate(
            GameData::GetAlien(m_RM->Results[0]) ) ) );
        return true;
    }

    // Install
    if( m_RM->Match(line, m_RM->ExpCmdInstall) )
    {
        Colony ^colony = GameData::Player->FindColony( m_RM->Results[2], false );

        AddCommand(colony, CmdSetOrigin(gcnew CmdInstall(
            m_RM->GetResultInt(0),
            FHStrings::InvFromString(m_RM->Results[1]),
            colony ) ) );

        return true;
    }

    // Develop
    if( m_RM->Match(line, m_RM->ExpCmdDevelopCS) )
    {
        Colony ^devColony = GameData::Player->FindColony(m_RM->Results[1], false);
        Ship^ ship = GameData::Player->FindShip(m_RM->Results[2], false);
        if( !ship )
            throw gcnew FHUIParsingException("JUMP order for unknown ship: {1}" + m_RM->Results[2]);

        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            devColony,
            ship ) ) );

        return true;
    }
    if( m_RM->Match(line, m_RM->ExpCmdDevelopC) )
    {
        Colony ^devColony = GameData::Player->FindColony(m_RM->Results[1], false);

        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            devColony,
            nullptr ) ) );

        return true;
    }
    if( m_RM->Match(line, m_RM->ExpCmdDevelop) )
    {
        AddCommand(colony, CmdSetOrigin(gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            nullptr,
            nullptr ) ) );

        return true;
    }

    // Custom
    if( m_RM->Match(line, m_RM->ExpCmdCustom) )
    {
        AddCommand(colony, CmdSetOrigin(
            gcnew CmdCustom(m_CmdPhase, m_RM->Results[1], m_RM->GetResultInt(0))) );
        return true;
    }

    return false;
}

bool CommandManager::LoadCommandsShip(String ^line, Ship ^ship)
{
    // Jump
    if( m_RM->Match(line, m_RM->ExpCmdShipJump) )
    {
        int x = m_RM->GetResultInt(0);
        int y = m_RM->GetResultInt(1);
        int z = m_RM->GetResultInt(2);
        int p = m_RM->GetResultInt(3);
        StarSystem ^target = nullptr;
        if( x != 0 || y != 0 || z != 0 )
            target = m_GameData->GetStarSystem(x, y, z, true);
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdJump(ship, target, p)) );
        return true;
    }
    // Move
    if( m_RM->Match(line, m_RM->ExpCmdShipMove) )
    {
        int x = m_RM->GetResultInt(0);
        int y = m_RM->GetResultInt(1);
        int z = m_RM->GetResultInt(2);
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdMove(ship, x, y, z)) );
        // Get ref system just to add it to the systems list
        // otherwise crash occurs while doing Form1::SystemsFillGrid() when
        // iterating systems list and it gets modified by call to this command's
        // GetRefSystem()
        ship->GetJumpCommand()->GetRefSystem();
        return true;
    }
    // Wormhole
    if( m_RM->Match(line, m_RM->ExpCmdShipWormhole) )
    {
        if( !ship->System->HasWormhole )
            throw gcnew FHUIParsingException("Invalid ship wormhole command (no wormhole in system)!");
        int p = m_RM->GetResultInt(0);
        ship->AddCommand(
            CmdSetOrigin(gcnew ShipCmdWormhole(ship, ship->System->GetWormholeTarget(), p)) );
        if( p != -1 &&
            ship->System->Planets->ContainsKey( p ) == false )
            throw gcnew FHUIParsingException("Invalid ship wormhole command (invalid planet for orbiting)!");
        return true;
    }
    // Upgrade
    if( m_RM->Match(line, m_RM->ExpCmdShipUpg) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdUpgrade(ship)) );
        return true;
    }
    // Recycle
    if( m_RM->Match(line, m_RM->ExpCmdShipRec) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdRecycle(ship)) );
        return true;
    }
    // Unload
    if( m_RM->Match(line, m_RM->ExpCmdShipUnload) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdUnload(ship)) );
        return true;
    }
    // Scan
    if( m_RM->Match(line, m_RM->ExpCmdShipScan) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdScan(ship)) );
        return true;
    }

    // Deep
    if( m_RM->Match(line, m_RM->ExpCmdShipDeep) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdDeep(ship)) );
        return true;
    }

    // Land
    if( m_RM->Match(line, m_RM->ExpCmdShipLand) )
    {
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdLand(ship)) );
        return true;
    }

    // Orbit
    if( m_RM->Match(line, m_RM->ExpCmdShipOrbitPLName) )
    {
        Planet ^planet = GameData::Player->FindColony(m_RM->Results[1], false)->Planet;
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdOrbit(ship, planet)) );
        return true;
    }
    if( m_RM->Match(line, m_RM->ExpCmdShipOrbitPLNum) )
    {
        // Planet may be from the jump target system
        // but I don't want to search for it here.
        // If planet of given number is not present in current system,
        // just make up a new fake planet.
        Planet ^planet;
        int plNum = m_RM->GetResultInt(1);
        if( ship->System->Planets->ContainsKey(plNum) )
            planet = ship->System->Planets[plNum];
        else
            planet = Planet::Default(ship->System, plNum);
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdOrbit(ship, planet)) );
        return true;
    }

    // Custom
    if( m_RM->Match(line, m_RM->ExpCmdCustom) )
    {
        ship->AddCommand( CmdSetOrigin(
            gcnew CmdCustom(m_CmdPhase, m_RM->Results[1], m_RM->GetResultInt(0))) );
        return true;
    }

    return false;
}

void CommandManager::RemoveGeneratedCommands(CommandOrigin origin, bool productionOnly, bool preserveScouting)
{
    if( GameData::Player == nullptr )
        return; // unlikely case commands are saved for turn 0?

    for each( Colony ^colony in GameData::Player->Colonies )
        RemoveGeneratedCommandsFromList( colony->Commands, origin, productionOnly, preserveScouting );

    for each( Ship ^ship in GameData::Player->Ships )
        RemoveGeneratedCommandsFromList( ship->Commands, origin, productionOnly, preserveScouting );

    RemoveGeneratedCommandsFromList( GetCommands(), origin, productionOnly, preserveScouting );
}

void CommandManager::RemoveGeneratedCommandsFromList(List<ICommand^> ^orders, CommandOrigin origin, bool productionOnly, bool preserveScouting)
{
    bool repeat = false;
    do
    {
        repeat = false;
        for each( ICommand ^cmd in orders )
        {
            if( cmd->Origin == origin )
            {
                if( productionOnly && cmd->GetPhase() != CommandPhase::Production )
                    continue;

                if( preserveScouting )
                {
                    if( cmd->GetCmdType() == CommandType::Scan )
                        continue;
                    if( cmd->GetCmdType() == CommandType::Jump )
                    {
                        ShipCmdJump ^jump = safe_cast<ShipCmdJump^>(cmd);
                        if( jump->m_Ship->Type == SHIP_TR &&
                            jump->m_Ship->Size == 1 )
                        {
                            continue;
                        }
                    }
                }

                orders->Remove( cmd );
                repeat = true;
                break;
            }
        }
    } while ( repeat );
}

void CommandManager::AddPluginCommands()
{
    for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
    {
        plugin->GenerateCommands( this );
    }
}

////////////////////////////////////////////////////////////////
// Order Template

void CommandManager::GenerateTemplate(System::Windows::Forms::RichTextBox^ target)
{
    m_OrderList->Clear();

    m_Budget = gcnew BudgetTracker(m_OrderList, m_GameData->GetCarriedEU());

    // Sort colonies for production
    GameData::Player->SortColoniesByProdOrder();
    for each( Colony ^colony in GameData::Player->Colonies )
        colony->ProductionReset();
    for each( Ship ^ship in GameData::Player->Ships )
        ship->StoreOriginalCargo();

    GenerateCombat();
    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();

    if( target == nullptr )
        return;

    target->Font = gcnew System::Drawing::Font("Courier New", 8, FontStyle::Regular);
    target->Lines = m_OrderList->ToArray();

    // Primitive orders coloring
    int start = 0;
    System::Drawing::Font ^sectionFont = gcnew System::Drawing::Font("Courier New", 11, FontStyle::Bold);

    for each( String ^s in target->Lines )
    {
        if( s->Length == 0 )
        {
            ++start;
            continue;
        }

        if( s[0] != ' ' &&
            (s->IndexOf("START ") == 0 || s == "END") )
        {   // section start/end
            target->Select(start, s->Length + 1);
            target->SelectionFont = sectionFont;
        }
        else
        {   // Auto
            if( s->IndexOf("; [Auto]") != -1 )
            {
                target->Select(start, s->Length + 1);
                target->SelectionColor = Color::Navy;
            }
            // Plugin
            if( s->IndexOf("; [Plugin]") != -1 )
            {
                target->Select(start, s->Length + 1);
                target->SelectionColor = Color::Purple;
            }
            // Comment warning
            if( s->IndexOf("; !!!!!! ") != -1 )
            {
                target->Select(start, s->Length + 1);
                target->SelectionColor = Color::Red;
            }
            else
            {   // Regular comments
                int commentStart = s->IndexOf(L';');
                if( commentStart != -1 )
                {
                    target->Select(start + commentStart, (s->Length - commentStart) + 1);
                    target->SelectionColor = Color::Green;
                }
            }
        }

        start += s->Length + 1;
    }
}

void CommandManager::GenerateCombat()
{
    m_OrderList->Add("START COMBAT");

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Combat )
        {
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }

    // TODO: Sort locations by relevance (effective tonnage)

    for each ( StarSystem^ system in m_GameData->GetStarSystems() )
    {
        if ( system->ShipsOwned->Count > 0 )
        {
            system->SortShipsByTonnage();
            GenerateCombatInfo( system );
            for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
            {
                plugin->GenerateCombat( m_OrderList, system );
            }

            m_OrderList->Add("");
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

List<String^>^ CommandManager::PrintSystemStatus(StarSystem^ system, bool listIncomplete, bool originalInventory)
{
    List<String^>^ status = gcnew List<String^>;

    // list own colonies
    for each ( Colony^ colony in system->ColoniesOwned )
    {
        String^ inv = colony->PrintInventory(originalInventory);
        // Flags -> String
        String^ flags = "";
        if (colony->Hidden) 
        {
            if (! String::IsNullOrEmpty(flags) ) flags += "/";
            flags += "hidden";
        }
        if (colony->Shared) 
        {
            if (! String::IsNullOrEmpty(flags) ) flags += "/";
            flags += "shared";
        }
        if (colony->UnderSiege) 
        {
            if (! String::IsNullOrEmpty(flags) ) flags += "/";
            flags += "siege";
        }
        if (! String::IsNullOrEmpty(flags) )
        {
            flags = String::Format(" ({0})", flags);
        }
        // Capabilities -> String
        String^ capabilities = "";
        if (colony->AvailPop) 
        {
            if (! String::IsNullOrEmpty(capabilities) ) capabilities += ";";
            capabilities += "pop:" + colony->AvailPop;
        }
        if( colony->Shipyards > 0 ) 
        {
            if (! String::IsNullOrEmpty(capabilities) ) capabilities += ";";
            capabilities += "SY:" + colony->Shipyards;
        }
        if (! String::IsNullOrEmpty(capabilities) )
        {
            capabilities = String::Format("[{0}] ", capabilities);
        }

        status->Add( String::Format("  ;   #{0} PL {1} ({2}.{3}){4}{5}{6}", 
            colony->PlanetNum,
            colony->Name,
            colony->EconomicBase / 10,
            colony->EconomicBase % 10,
            capabilities,
            (String::IsNullOrEmpty(inv) ? "" : ": " + inv),
            flags ) );
    }
    // list own ships
    for each ( Ship^ ship in system->ShipsOwned )
    {
        // Skip incomplete ships
        if( listIncomplete == false )
        {
            if( ship->EUToComplete > 0 ||
                ship->BuiltThisTurn )
                continue;
        }

        String^ inv = ship->PrintCargo(originalInventory);
        status->Add( String::Format("  ;   {0} {1}{2}{3}", 
            ship->PrintClassWithName(),
            ship->Type == SHIP_BAS ? String::Format("({0}k)", ship->Size / 1000) : "",
            ship->PrintAgeLocation(),
            String::IsNullOrEmpty(inv) ? "" : ": " + inv ) );
    }

    // list aliens
    List<String^>^ alienList = system->PrintAliens();
    status->Add( String::Format("  ; Aliens:{0}", alienList->Count ? "" : " none" ) );
    for each ( String^ line in alienList )
    {
        status->Add( String::Format("  ;   {0}", line ) );
    }
    return status;
}

void CommandManager::GenerateCombatInfo(StarSystem^ system)
{
    m_OrderList->Add( String::Format("  ; Battle location: {0}", system->PrintLocation() ) );
    m_OrderList->AddRange( PrintSystemStatus(system, false, true) );
}

void CommandManager::GeneratePreDeparture()
{
    m_OrderList->Add("START PRE-DEPARTURE");

    // Print UI commands: NAME
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetCmdType() == CommandType::Name ||
                cmd->GetCmdType() == CommandType::Disband )
            {
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
            }
        }
    }

    // Print UI commands global
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PreDeparture )
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
    }

    for each ( StarSystem^ system in m_GameData->GetStarSystems() )
    {
        if ( system->ShipsOwned->Count == 0 )
        {
            if ( system->ColoniesOwned->Count == 0 )
            {
                continue;
            }
            else if ( system->ColoniesOwned->Count == 1 )
            {
                Colony^ colony = system->ColoniesOwned[0];
                if ( (colony->EconomicBase <= 0) && (colony->HasInventory == false) )
                    continue;
            }
        }
        m_OrderList->Add("");
        GeneratePreDepartureInfo( system );

        // Print UI commands
        for each( CmdTransfer ^cmd in system->Transfers )
        {
            if( cmd->GetPhase() == CommandPhase::PreDeparture )
            {
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
                m_Budget->EvalOrderTransfer(cmd);
            }
        }
        for each( Ship ^ship in system->ShipsOwned )
        {
            for each( ICommand ^cmd in ship->Commands )
            {
                if( cmd->GetPhase() == CommandPhase::PreDeparture )
                {
                    m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
                    m_Budget->EvalOrder(cmd);
                }
            }
        }
        for each( Colony ^colony in system->ColoniesOwned )
        {
            m_Budget->SetColony(colony, CommandPhase::PreDeparture);
            for each( ICommand ^cmd in colony->Commands )
            {
                if( cmd->GetPhase() == CommandPhase::PreDeparture &&
                    cmd->GetCmdType() != CommandType::Name &&
                    cmd->GetCmdType() != CommandType::Disband )
                {
                    m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
                    m_Budget->EvalOrder(cmd);
                }
            }
        }
        m_Budget->SetColony(nullptr, CommandPhase::PreDeparture);

        for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
        {
            plugin->GeneratePreDeparture(m_OrderList, system);
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GeneratePreDepartureInfo(StarSystem^ system)
{
    m_OrderList->Add(String::Format("  ; Location: {0}", system->PrintLocation() ) );
    m_OrderList->AddRange( PrintSystemStatus(system, false, true) );
}

void CommandManager::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Jump )
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
    }

    List<Ship^>^ jumpList = GameData::Player->Ships;
    jumpList->Sort( gcnew Ship::TypeTonnageLocationComparer );

    for each( Ship ^ship in jumpList )
    {
        GenerateJumpInfo(ship);

        int prevLines = m_OrderList->Count;

        for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
        {
            plugin->GenerateJumps(m_OrderList, ship);
        }

        ICommand ^jumpCmd = ship->GetJumpCommand();
        if( jumpCmd )
        {
            StarSystem ^jumpTarget = jumpCmd->GetRefSystem();
            switch( jumpCmd->GetCmdType() )
            {
            case CommandType::Jump:
                if( jumpTarget )
                {
                    m_OrderList->Add( String::Format("  Jump {0}, {1}  ; ->[{3}]; Mishap:{4:F2}%{5}",
                        ship->PrintClassWithName(),
                        ship->PrintJumpDestination(),
                        ship->System->PrintLocation(),
                        jumpTarget->PrintLocation(),
                        ship->System->CalcMishap(
                            jumpTarget,
                            ship->Owner->TechLevelsAssumed[TECH_GV],
                            ship->Age),
                        jumpCmd->PrintOriginSuffix() ) );
                }
                else
                {
                    m_OrderList->Add( String::Format("  Jump {0}, ??? {1}",
                        ship->PrintClassWithName(),
                        jumpCmd->PrintOriginSuffix() ) );
                }
                break;

            case CommandType::Wormhole:
            {
                String ^order = "  Wormhole " + ship->PrintClassWithName();
                int planetNum = safe_cast<ShipCmdWormhole^>(jumpCmd)->m_PlanetNum;

                if( jumpTarget &&
                    jumpTarget->Planets->ContainsKey( planetNum ) )
                {
                    Planet ^pl = jumpTarget->Planets[ planetNum ];
                    if( String::IsNullOrEmpty(pl->Name) )
                    {
                        order += ", " + planetNum.ToString();
                    }
                    else
                    {
                        order += ", PL " + pl->Name;
                    }
                }
                order += " ; Distance: ";
                if( ship->System->WormholeTargetId == -1 )
                    order += "Unknown";
                else
                {
                    StarSystem ^target = ship->System->GetWormholeTarget();
                    order += Calculators::Distance(
                        ship->System->X,
                        ship->System->Y,
                        ship->System->Z,
                        target->X,
                        target->Y,
                        target->Z).ToString("F1");
                }
                m_OrderList->Add( order );
                break;
            }

            case CommandType::Move:
                m_OrderList->Add( PrintCommandWithInfo(jumpCmd, 2) );
                break;

            default:
                throw gcnew FHUIDataImplException("Not supported ship jump command: " + ((int)jumpCmd->GetCmdType()).ToString());
            }
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GenerateJumpInfo(Ship^ ship)
{
    // Skip incomplete ships
    if( ship->EUToComplete > 0 )
        return;

    m_OrderList->Add(
        String::Format("  ; {0} (age {1}) at {2} with [{3}]",
            ship->PrintClassWithName(),
            ship->Age,
            ship->PrintLocation(),
            ship->PrintCargo(false) ) );
}

void CommandManager::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");
    m_OrderList->Add("  ; +" + m_Budget->GetTotalBudget().ToString() + " EUs carried" );

    // Mark ships for upgrade
    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->GetProdCommand() != nullptr )
        {
            ship->CommandProdPlanet = -1;
        }
    }

    // Generate production template for each colony
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        if ( colony->EconomicBase <= 0 )
            continue;

        // Section header
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);
        m_OrderList->AddRange( PrintSystemStatus(colony->System, true, false) );

        // Remember EUs available before this colony contributes
        int euCarried = m_Budget->GetTotalBudget();

        // Set budget tracker for this colony
        m_Budget->SetColony(colony, CommandPhase::Production);
        List<String^> ^orders = colony->OrdersText;

        // Production summary
        String ^prodSummary = String::Format("  ; {0} +{1} = {2}",
            colony->EUAvail, euCarried, m_Budget->GetTotalBudget());
        if( colony->PlanetType == PLANET_COLONY )
            prodSummary += "  max=" + m_Budget->GetAvailBudget().ToString();
        orders->Add( prodSummary );

        // First RECYCLE all ships
        GenerateProductionRecycle(colony);

        // Launch plugin actions
        int prePluginNum = orders->Count;
        for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
            plugin->GenerateProduction(orders, colony, m_Budget);
        bool useAuto = orders->Count == prePluginNum;

        // Production orders
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::Production )
            {
                orders->Add( PrintCommandWithInfo(cmd, 4) );
                m_Budget->EvalOrder( cmd );
            }
        }

        // Now try to CONTINUE or UPGRADE ships here
        GenerateProductionUpgrade(colony);

        // Automatic spendings summary
        if( colony->PlanetType == PLANET_COLONY )
        {
            orders->Add( String::Format("    ; -- EU to use: {0} (total {1})",
                m_Budget->GetAvailBudget(),
                m_Budget->GetTotalBudget() ) );
        }
        else if( colony->PlanetType == PLANET_HOME )
        {
            orders->Add( String::Format("    ; -- EU to use: {0}", m_Budget->GetAvailBudget() ) );
        }

        m_OrderList->AddRange(orders);
    }
    m_Budget->SetColony(nullptr, CommandPhase::Production);

    m_OrderList->Add("");
    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GenerateProductionRecycle(Colony ^colony)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        ICommand ^prodCmd = ship->GetProdCommand();

        if( prodCmd != nullptr &&
            prodCmd->GetCmdType() == CommandType::RecycleShip &&
            ship->CommandProdPlanet == -1 )    // not yet done
        {
            // If ship is incomplete,
            // add recycle command on planet where it is under construction.
            if( ship->EUToComplete > 0 &&
                ship->PlanetNum != colony->PlanetNum )
                continue;

            colony->OrdersText->Add( PrintCommandWithInfo( prodCmd, 4 ) );
            m_Budget->EvalOrder( prodCmd );

            // Mark the recycle was done on this planet.
            ship->CommandProdPlanet = colony->PlanetNum;
        }
    }
}

void CommandManager::GenerateProductionUpgrade(Colony ^colony)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        ICommand ^prodCmd = ship->GetProdCommand();

        if( ship->EUToComplete > 0 &&
            ship->PlanetNum == colony->PlanetNum )
        {
            // Continue ship if no recycle order issued
            if( prodCmd == nullptr ||
                prodCmd->GetCmdType() != CommandType::Recycle )
            {
                colony->OrdersText->Add( String::Format("    Continue {0}  ; -{1} EU",
                    ship->PrintClassWithName(),
                    ship->EUToComplete) );
                m_Budget->UpdateEU(ship->EUToComplete);
                continue;
            }
        }

        if( prodCmd != nullptr &&
            prodCmd->GetCmdType() == CommandType::Upgrade &&
            ship->CommandProdPlanet == -1 )    // not yet done
        {
            int value = Calculators::ShipUpgradeCost(ship->Age, ship->OriginalCost);
            bool doUpgrade = true;
            // Does this colony has budget for upgrade?
            // If budget exceeded search for another colony
            // that could handle the upgrade.
            if( value > m_Budget->GetAvailBudget() )
            {
                for each( Colony ^otherCol in colony->System->ColoniesOwned )
                {
                    if( otherCol->CanProduce &&
                        otherCol->ProductionOrder > colony->ProductionOrder &&
                        otherCol->GetMaxProductionBudget() >= value )
                    {   // Other candidate found, don't upgrade on this colony.
                        doUpgrade = false;
                    }
                }
            }

            // Even if budget for upgrade not available, but no other
            // colony able to do the upgrade found, add the command.
            if( doUpgrade )
            {
                colony->OrdersText->Add( PrintCommandWithInfo( prodCmd, 4 ) );
                m_Budget->EvalOrder( prodCmd );

                // Mark the upgrade was done on this planet.
                ship->CommandProdPlanet = colony->PlanetNum;
            }
        }
    }
}

void CommandManager::GeneratePostArrival()
{
    m_OrderList->Add("START POST-ARRIVAL");

    for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
    {
        for each( Ship ^ship in GameData::Player->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
    }

    if ( AutoEnabled )
    {
        m_OrderList->Add( "  AUTO" );
    }

    // Print UI commands, transfers first
    for each( StarSystem ^system in GameData::GetStarSystems() )
    {
        for each( CmdTransfer ^cmd in system->Transfers )
        {
            if( cmd->GetPhase() == CommandPhase::PostArrival )
            {
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
                m_Budget->EvalOrderTransfer(cmd);
            }
        }
    }
    // Then ships
    for each( Ship ^ship in GameData::Player->Ships )
    {
        for each( ICommand ^cmd in ship->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::PostArrival )
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }
    // Then colonies
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        m_Budget->SetColony(colony, CommandPhase::PostArrival);
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::PostArrival )
            {
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
                m_Budget->EvalOrder(cmd);
            }
        }
    }
    m_Budget->SetColony(nullptr, CommandPhase::PostArrival);

    // Global commands last, without messages
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PostArrival &&
            cmd->GetCmdType() != CommandType::Message )
        {
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }
    // Messages at the end
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PostArrival &&
            cmd->GetCmdType() == CommandType::Message )
        {
            m_OrderList->Add( PrintCommandWithInfo(cmd, 0) );
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GenerateStrikes()
{
    m_OrderList->Add("START STRIKES");

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Strike )
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

} // end namespace FHUI

