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

private ref class CommandComparer : public IComparer<ICommand^>
{
public:
    virtual int Compare(ICommand ^c1, ICommand ^c2)
    {
        return (int)c1->GetCmdType() - (int)c2->GetCmdType();
    }
};

// ---------------------------------------------------------

CommandManager::CommandManager(GameData^ gd, String^ path)
    : m_RM(gcnew RegexMatcher)
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

void CommandManager::SortCommands()
{
    m_CommandData[m_CurrentTurn]->Commands->Sort( gcnew CommandComparer );
}

void CommandManager::AddCommand(ICommand ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::Production )
        throw gcnew FHUIDataImplException("AddCommand: Wrong interface for production command!");

    m_CommandData[m_CurrentTurn]->Commands->Add(cmd);
    SortCommands();
    SaveCommands();
}

void CommandManager::AddCommandDontSave(ICommand ^cmd)
{
    m_CommandData[m_CurrentTurn]->Commands->Add(cmd);
    SortCommands();
}

void CommandManager::DelCommand(ICommand ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::Production )
        throw gcnew FHUIDataImplException("DelCommand: Wrong interface for production command!");

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
    colony->Commands->Add(cmd);
    SaveCommands();
}

void CommandManager::DelCommand(Colony ^colony, ICommand ^cmd)
{
    for each( ICommand ^iCmd in colony->Commands )
    {
        if( iCmd == cmd ||
            iCmd->CompareTo(cmd) == 0 )
        {
            colony->Commands->Remove(iCmd);
            SaveCommands();
            return;
        }
    }
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
    switch( cmd->Origin )
    {
    case CommandOrigin::Auto:
        return "[A] " + cmd->Print();
    case CommandOrigin::Plugin:
        return "[P] " + cmd->Print();
    default:
        return cmd->Print();
    }
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
        commandList->Add( "COLONY " + colony->Name );
        for each( ICommand ^cmd in colony->Commands )
        {
            commandList->Add( PrintCommandToFile(cmd) );
        }
        commandList->Add( "END_COLONY" );
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
    return cmd;
}

void CommandManager::LoadCommands()
{
    AddPluginCommands();

    // Open file
    StreamReader ^sr;
    try 
    {
        sr = File::OpenText( m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn) );

        RemoveGeneratedCommands(CommandOrigin::Auto, false);
        RemoveGeneratedCommands(CommandOrigin::Plugin, false);

        LoadCommandsGlobal(sr);
    }
    catch( DirectoryNotFoundException^ )
    {
        return;
    }
    catch( FileNotFoundException^ )
    {
        return;
    }
    finally
    {
        if( sr )
            sr->Close();
    }
}

void CommandManager::LoadCommandsGlobal(StreamReader ^sr)
{
    String ^line;
    Colony^ refColony = nullptr;
    Ship^ refShip = nullptr;
    String ^messageText = "";
    Alien ^messageTarget;

    int colonyProdOrder = 1;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        line = line->Trim();
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
                    AddCommandDontSave( CmdSetOrigin( gcnew CmdMessage(messageTarget, messageText) ) );
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
            Colony ^colony = m_GameData->GetColony(m_RM->Results[0]);
            if( colony->Owner == GameData::Player )
            {
                colony->ProductionOrder = colonyProdOrder++;
                refColony = colony;
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
            Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
            int x = m_RM->GetResultInt(1);
            int y = m_RM->GetResultInt(2);
            int z = m_RM->GetResultInt(3);
            int p = m_RM->GetResultInt(4);
            StarSystem ^target = nullptr;
            if( x != 0 || y != 0 || z != 0 )
                target = m_GameData->GetStarSystem(x, y, z, false);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdJump(ship, target, p)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipWormhole_Obsolete) )
        {
            Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
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
            Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdUpgrade(ship)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipRec_Obsolete) )
        {
            Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
            ship->AddCommand( CmdSetOrigin(gcnew ShipCmdRecycle(ship)) );
        }
    // --- end obsolete ship commands ---
        else if( m_RM->Match(line, m_RM->ExpCmdShip) )
        {
            Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
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
            AddCommandDontSave( CmdSetOrigin(gcnew CmdPlanetName(system, planet->Number, name)) );
            planet->AddName(name);
        }
        else if( m_RM->Match(line, m_RM->ExpCmdPLDisband) )
        {
            String ^name = m_RM->Results[0];
            for each( PlanetName ^pn in m_GameData->GetPlanetNames() )
            {
                if( pn->Name == name )
                {
                    AddCommandDontSave( CmdSetOrigin(gcnew CmdDisband(name)) );
                    pn->System->Planets[ pn->PlanetNum ]->DelName();
                    break;
                }
            }
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPNeutral) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_ENEMY &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (neutral)!");

            alien->Relation = SP_NEUTRAL;
            AddCommandDontSave( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_NEUTRAL)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPAlly) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_ENEMY &&
                alien->Relation != SP_NEUTRAL )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (ally)!");

            alien->Relation = SP_ALLY;
            AddCommandDontSave( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_ALLY)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPEnemy) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_NEUTRAL &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (enemy)!");

            alien->Relation = SP_ENEMY;
            AddCommandDontSave( CmdSetOrigin(gcnew CmdAlienRelation(alien, SP_ENEMY)) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPTeach) )
        {
            TechType tech = FHStrings::TechFromString(m_RM->Results[0]);
            int level = m_RM->GetResultInt(1);
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[2]);
            if( alien->Relation != SP_NEUTRAL &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation for Teach command!");
            if( level != GameData::Player->TechLevels[tech] )
                throw gcnew FHUIParsingException("Inconsistent tech level for Teach command!");

            AddCommandDontSave( CmdSetOrigin(gcnew CmdTeach(alien, tech, level)) );
            alien->TeachOrders |= 1 << tech;
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPMsg) )
        {
            messageTarget = GameData::GetAlien(m_RM->Results[0]);
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
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdShipyard) );
        return true;
    }
    // Hide
    if( line == "Hide" )
    {
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdHide(colony)) );
        return true;
    }

    // Research
    if( m_RM->Match(line, m_RM->ExpCmdResearch) )
    {
        int amount = m_RM->GetResultInt(0);
        TechType tech = FHStrings::TechFromString(m_RM->Results[1]);

        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdResearch(tech, amount)) );
        return true;
    }

    // Build CU/IU/AU
    if( m_RM->Match(line, m_RM->ExpCmdBuildIUAU) )
    {
        int amount = m_RM->GetResultInt(0);
        colony->Commands->Add( CmdSetOrigin(
            gcnew ProdCmdBuildIUAU(
                amount,
                FHStrings::InvFromString(m_RM->Results[1]) ) ) );
        return true;
    }

    // Build warship
    if( m_RM->Match(line, m_RM->ExpCmdBuildShip) )
    {
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdBuildShip(
            FHStrings::ShipFromString(m_RM->Results[0]),
            0,
            String::IsNullOrEmpty(m_RM->Results[1]) == false,
            m_RM->Results[2] )) );
        return true;
    }

    // Build transport ship
    if( m_RM->Match(line, m_RM->ExpCmdBuildShipTR) )
    {
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdBuildShip(
            SHIP_TR,
            m_RM->GetResultInt(0),
            String::IsNullOrEmpty(m_RM->Results[1]) == false,
            m_RM->Results[2] )) );
        return true;
    }

    // Recycle
    if( m_RM->Match(line, m_RM->ExpCmdRecycle) )
    {
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdRecycle(
            FHStrings::InvFromString( m_RM->Results[1] ),
            m_RM->GetResultInt(0) ) ) );
        return true;
    }

    // Estimate
    if( m_RM->Match(line, m_RM->ExpCmdEstimate) )
    {
        colony->Commands->Add( CmdSetOrigin(gcnew ProdCmdEstimate(
            GameData::GetAlien(m_RM->Results[0]) ) ) );
        return true;
    }

    // Install
    if( m_RM->Match(line, m_RM->ExpCmdInstall) )
    {
        Planet ^planet = m_GameData->GetPlanetByName( m_RM->Results[2] );  
        if ( planet )
        {
            Colony ^colony = GameData::GetColonyFromPlanet(planet, true);

            colony->Commands->Add( CmdSetOrigin(gcnew CmdInstall(
                m_RM->GetResultInt(0),
                m_RM->Results[1],
                colony ) ) );

            return true;
        }
        throw gcnew FHUIParsingException(
            String::Format("INSTALL order for unknown planet: PL {0}", m_RM->Results[0]) );
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
            target = m_GameData->GetStarSystem(x, y, z, false);
        ship->AddCommand( CmdSetOrigin(gcnew ShipCmdJump(ship, target, p)) );
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

    return false;
}

void CommandManager::RemoveGeneratedCommands(CommandOrigin origin, bool preserveScouting)
{
    for each( Colony ^colony in GameData::Player->Colonies )
        RemoveGeneratedCommandsFromList( colony->Commands, origin, preserveScouting );

    for each( Ship ^ship in GameData::Player->Ships )
        RemoveGeneratedCommandsFromList( ship->Commands, origin, preserveScouting );

    RemoveGeneratedCommandsFromList( GetCommands(), origin, preserveScouting );
}

void CommandManager::RemoveGeneratedCommandsFromList(List<ICommand^> ^orders, CommandOrigin origin, bool preserveScouting)
{
    bool repeat = false;
    do
    {
        repeat = false;
        for each( ICommand ^cmd in orders )
        {
            if( cmd->Origin == origin )
            {
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
    //TODO...
}

////////////////////////////////////////////////////////////////
// Order Template

void CommandManager::GenerateTemplate(System::Windows::Forms::RichTextBox^ target)
{
    m_OrderList->Clear();

    m_Budget = gcnew BudgetTracker(m_OrderList, m_GameData->GetCarriedEU());

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

    for each( String ^s in m_OrderList )
    {
        if( s->Length == 0 )
        {
            ++start;
            continue;
        }

        if( s[0] != ' ' )
        {   // section start/end
            target->Select(start, s->Length + 1);
            target->SelectionFont = sectionFont;
        }
        else if( s->Trim()[0] == ';' )
        {   // comments
            target->Select(start, s->Length + 1);
            target->SelectionColor = Color::Green;
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

List<String^>^ CommandManager::PrintSystemStatus(StarSystem^ system, bool listIncomplete)
{
    List<String^>^ status = gcnew List<String^>;

    // list own colonies
    for each ( Colony^ colony in system->ColoniesOwned )
    {
        String^ inv = colony->PrintInventory();
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
        if (colony->Shipyards) 
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
        if( (listIncomplete == false) && (ship->EUToComplete > 0) ) continue;

        String^ inv = ship->PrintCargo();
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
    m_OrderList->AddRange( PrintSystemStatus(system, false) );
}

void CommandManager::GeneratePreDeparture()
{
    m_OrderList->Add("START PRE-DEPARTURE");

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PreDeparture )
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
    }
    for each( Ship ^ship in GameData::Player->Ships )
    {
        for each( ICommand ^cmd in ship->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::PreDeparture )
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }

    for each ( StarSystem^ system in m_GameData->GetStarSystems() )
    {
        if ( system->ShipsOwned->Count == 0 )
        {
            if ( system->ColoniesOwned->Count == 0)
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
        for each( Colony ^colony in system->ColoniesOwned )
        {
            for each( ICommand ^cmd in colony->Commands )
            {
                if( cmd->GetPhase() == CommandPhase::PreDeparture )
                    m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
            }
        }

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
    m_OrderList->AddRange( PrintSystemStatus(system, false) );
}

void CommandManager::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");
    
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
            if( jumpCmd->GetCmdType() == CommandType::Jump )
            {
                StarSystem ^jumpTarget = safe_cast<ShipCmdJump^>(jumpCmd)->m_JumpTarget;
                if( jumpTarget )
                {
                    m_OrderList->Add( String::Format("  Jump {0}, {1}  ; [{2}] -> [{3}]; Mishap: {4:F2}%{5}",
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
            }
            else if( jumpCmd->GetCmdType() == CommandType::Wormhole )
            {
                String ^order = "  Wormhole " + ship->PrintClassWithName();
                StarSystem ^jumpTarget = safe_cast<ShipCmdWormhole^>(jumpCmd)->m_JumpTarget;
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
            }
            else
                throw gcnew FHUIDataImplException("Not supported ship jump command: " + ((int)jumpCmd->GetCmdType()).ToString());
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
            ship->PrintCargo() ) );
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

    // Sort colonies for production
    GameData::Player->SortColoniesByProdOrder();

    // Generate production template for each colony
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        //if ( (colony->EconomicBase <= 0) &&
        //     (colony->HasInventory == false) )
        //    continue;

        // Section header
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);
        m_OrderList->AddRange( PrintSystemStatus(colony->System, true) );

        // Set budget tracker for this colony
        m_Budget->SetColony(colony);
        List<String^> ^orders = colony->OrdersText;

        // Production summary
        int euCarried = m_Budget->GetTotalBudget();
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

        // Add auto orders
        List<Pair<String^, int>^>^ autoOrders = GetAutoOrdersProduction( colony );
        if ( autoOrders )
        {
            String ^prefix = "    ";
            if( !useAuto )
                prefix += "; ";
            for each (Pair<String^, int>^ order in autoOrders )
            {
                orders->Add( prefix + order->A + String::Format(" ; AUTO (cost {0})", order->B) );
                m_Budget->UpdateEU(order->B);
            }
        }

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
            prodCmd->GetCmdType() == CommandType::Recycle &&
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

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PostArrival )
            m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
    }
    for each( Ship ^ship in GameData::Player->Ships )
    {
        for each( ICommand ^cmd in ship->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::PostArrival )
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetPhase() == CommandPhase::PostArrival )
                m_OrderList->Add( PrintCommandWithInfo(cmd, 2) );
        }
    }

    if ( AutoEnabled )
    {
        m_OrderList->Add( "  AUTO" );
    }

    for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
    {
        for each( Ship ^ship in GameData::Player->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
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

void CommandManager::SetAutoOrderProduction(Colony^ colony, String^ line, int cost)
{
    if( !m_CommandData[m_CurrentTurn]->AutoOrdersProduction->ContainsKey( colony ) )
    {
        m_CommandData[m_CurrentTurn]->AutoOrdersProduction->Add(colony, gcnew List<Pair<String^, int>^>);
    }
    m_CommandData[m_CurrentTurn]->AutoOrdersProduction[colony]->Add(gcnew Pair<String^, int>(line, cost));
}

List<Pair<String^, int>^>^ CommandManager::GetAutoOrdersProduction(Colony^ colony)
{
    if ( m_CommandData[m_CurrentTurn]->AutoOrdersProduction->ContainsKey( colony ) )
    {
        return m_CommandData[m_CurrentTurn]->AutoOrdersProduction[colony];
    }
    return nullptr;
}

} // end namespace FHUI

