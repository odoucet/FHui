#include "StdAfx.h"
#include "enums.h"
#include "RegexMatcher.h"
#include "CommandManager.h"
#include "PluginManager.h"

using namespace System::Drawing;
using namespace System::IO;

namespace FHUI
{

CommandManager::CommandManager(GameData^ gd, String^ path)
    : m_RM(gcnew RegexMatcher)
{
    m_GameData = gd;
    m_Path = path;
    m_OrderList = gcnew List<String^>;
    m_CommandData = gcnew SortedList<int, TurnCommands^>;
}

private ref class CommandComparer : public IComparer<ICommand^>
{
public:
    virtual int Compare(ICommand ^c1, ICommand ^c2)
    {
        return (int)c1->GetType() - (int)c2->GetType();
    }
};

void CommandManager::SortCommands()
{
    m_CommandData[m_CurrentTurn]->Commands->Sort( gcnew CommandComparer );
}

void CommandManager::AddCommand(ICommand ^cmd)
{
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
    m_CommandData[m_CurrentTurn]->Commands->Remove(cmd);
    SaveCommands();
}

void CommandManager::DeleteCommands()
{
    FileInfo ^fileInfo = gcnew FileInfo(
        m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn) );
    if( fileInfo->Exists )
        fileInfo->Delete();
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
        if( colony->OrderBuildShipyard )
        {
            commandList->Add("  build shipyard");
        }
    }

    // -- Ships
    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->Command )
        {
            commandList->Add( String::Format("SHIP {0} {1}",
                ship->PrintClassWithName(),
                ship->Command->PrintNumeric()) );
        }
    }
    // -- Commands
    for each( ICommand ^cmd in GetCommands() )
    {
        cmd->Print( commandList );
    }

    // Write to stream
    for each( String ^cmd in commandList )
    {
        sw->WriteLine(cmd);
    }
    sw->Close();
}

void CommandManager::LoadCommands()
{
    Colony^ refColony = nullptr;

    // Open file
    StreamReader ^sr;
    try 
    {
        sr = File::OpenText( m_Path + String::Format(OrdersDir::Commands, GameData::CurrentTurn) );
    }
    catch( DirectoryNotFoundException^ )
    {
        return;
    }
    catch( FileNotFoundException^ )
    {
        return;
    }

    String ^line;
    int colonyProdOrder = 1;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        line = line->Trim();
        if( String::IsNullOrEmpty(line) ||
            line[0] == ';' )
            continue;

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
        else if( m_RM->Match(line, m_RM->ExpCmdBuiShipyard) )
        {
            if( refColony )
            {
                refColony->OrderBuildShipyard = true;
            }
            else
            {
                throw gcnew FHUIParsingException("Inconsistent commands template (shipyard)!");
            }
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipJump) )
        {
            m_GameData->GetShip(m_RM->Results[1])->Command =
                gcnew Ship::Order(
                    Ship::OrderType::Jump,
                    m_GameData->GetStarSystem(
                        m_RM->GetResultInt(2),
                        m_RM->GetResultInt(3),
                        m_RM->GetResultInt(4)),
                    m_RM->GetResultInt(5) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipWormhole) )
        {
            Ship ^ship = m_GameData->GetShip(m_RM->Results[1]);
            if( !ship->System->HasWormhole )
                throw gcnew FHUIParsingException("Invalid ship wormhole command (no wormhole in system)!");
            ship->Command =
                gcnew Ship::Order(
                    Ship::OrderType::Wormhole,
                    ship->System->GetWormholeTarget(),
                    m_RM->GetResultInt(2) );
            if( ship->Command->PlanetNum != -1 &&
                ship->System->Planets->ContainsKey( ship->Command->PlanetNum ) == false )
                throw gcnew FHUIParsingException("Invalid ship wormhole command (invalid planet for orbiting)!");
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipUpg) )
        {
            m_GameData->GetShip(m_RM->Results[1])->Command =
                gcnew Ship::Order( Ship::OrderType::Upgrade );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdShipRec) )
        {
            m_GameData->GetShip(m_RM->Results[1])->Command =
                gcnew Ship::Order( Ship::OrderType::Recycle );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdPLName) )
        {
            StarSystem ^system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2));
            Planet ^planet = system->Planets[ m_RM->GetResultInt(3) ];
            String ^name = m_RM->Results[4];
            AddCommandDontSave( gcnew CmdPlanetName(system, planet->Number, name) );
            planet->AddName(name);
        }
        else if( m_RM->Match(line, m_RM->ExpCmdPLDisband) )
        {
            String ^name = m_RM->Results[0];
            for each( PlanetName ^pn in m_GameData->GetPlanetNames() )
            {
                if( pn->Name == name )
                {
                    AddCommandDontSave( gcnew CmdDisband(name) );
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
            AddCommandDontSave( gcnew CmdAlienRelation(alien, SP_NEUTRAL) );
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPAlly) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_ENEMY &&
                alien->Relation != SP_NEUTRAL )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (ally)!");

            alien->Relation = SP_ALLY;
            AddCommandDontSave(gcnew CmdAlienRelation(alien, SP_ALLY));
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPEnemy) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_NEUTRAL &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (enemy)!");

            alien->Relation = SP_ENEMY;
            AddCommandDontSave( gcnew CmdAlienRelation(alien, SP_ENEMY) );
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

            AddCommandDontSave( gcnew CmdTeach(alien, tech, level) );
            alien->TeachOrders |= 1 << tech;
        }
        else
            throw gcnew FHUIParsingException("Unrecognized line in commands template: " + line);
    }
}

////////////////////////////////////////////////////////////////
// Order Template

void CommandManager::GenerateTemplate(System::Windows::Forms::RichTextBox^ target)
{
    m_OrderList->Clear();

    GenerateCombat();
    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();

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
        else if( s->IndexOf("[A]") != -1 )
        {   // UI generated
            target->Select(start, s->Length + 1);
            target->SelectionColor = Color::FromArgb(0xFF800080);
        }
        else if( s->IndexOf("[A+]") != -1 )
        {   // plugin generated
            target->Select(start, s->Length + 1);
            target->SelectionColor = Color::Blue;
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
            cmd->Print(m_OrderList);
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
            cmd->Print(m_OrderList);
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

        List<String^>^ autoOrders = GetAutoOrdersPreDeparture( system );
        if ( autoOrders )
        {
            for each (String^ line in autoOrders )
            {
                m_OrderList->Add( "  " + line + " ; AUTO" );
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

        if( ship->Command )
        {
            if( ship->Command->Type == Ship::OrderType::Jump )
            {
                m_OrderList->Add( String::Format("  Jump {0}, {1}  ; [{2}] -> [{3}]; Mishap: {4:F2}%",
                    ship->PrintClassWithName(),
                    ship->Command->PrintJumpDestination(),
                    ship->System->PrintLocation(),
                    ship->Command->JumpTarget->PrintLocation(),
                    ship->System->CalcMishap(
                        ship->Command->JumpTarget,
                        ship->Owner->TechLevelsAssumed[TECH_GV],
                        ship->Age) ) );
            }
            else if( ship->Command->Type == Ship::OrderType::Wormhole )
            {
                String ^order = "  Wormhole " + ship->PrintClassWithName();
                if( ship->Command->JumpTarget &&
                    ship->Command->JumpTarget->Planets->ContainsKey( ship->Command->PlanetNum ) )
                {
                    Planet ^pl = ship->Command->JumpTarget->Planets[ ship->Command->PlanetNum ];
                    if( String::IsNullOrEmpty(pl->Name) )
                    {
                        order += ", " + ship->Command->PlanetNum.ToString();
                    }
                    else
                    {
                        order += ", PL " + pl->Name;
                    }
                }
                order += " ; Distance: ";
                if( ship->System->WormholeTargetId == -1 )
                    order += "unknown";
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
        }

        String ^prefix = "  ";
        bool commentOutAuto = false;
        // Comment out auto jumps, if UI or plugin jumps were added
        if( m_OrderList->Count > prevLines )
        {
            commentOutAuto = true;
            prefix += "; ";
        }

        List<String^>^ autoOrders = GetAutoOrdersJumps( ship );
        if ( autoOrders )
        {
            for each (String^ line in autoOrders )
            {
                // If auto is to be commented out, and auto target is "???",
                // completely skip this order
                if( commentOutAuto && line->IndexOf("???") != -1 )
                    continue;

                m_OrderList->Add( prefix + line + " ; AUTO" );
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
            ship->PrintCargo() ) );
}

void CommandManager::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");

    BudgetTracker ^budget = gcnew BudgetTracker(m_OrderList, m_GameData->GetCarriedEU());
    m_OrderList->Add("  ; +" + budget->GetTotalBudget().ToString() + " EUs carried" );

    // Mark ships for upgrade
    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->Command != nullptr &&
            (   ship->Command->Type == Ship::OrderType::Upgrade ||
                ship->Command->Type == Ship::OrderType::Recycle ) )
        {
            ship->Command->PlanetNum = -1;
        }
    }

    // Sort colonies for production
    GameData::Player->SortColoniesByProdOrder();

    // Generate production template for each colony
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        if ( (colony->EconomicBase <= 0) && (colony->HasInventory == false) )
            continue;

        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);

        m_OrderList->AddRange( PrintSystemStatus(colony->System, true) );

        int euCarried = budget->GetTotalBudget();
        budget->SetColony(colony);

        // Production summary
        String ^prodSummary = String::Format("  ; {0} +{1} = {2}",
            colony->EUAvail, euCarried, budget->GetTotalBudget());
        if( colony->PlanetType == PLANET_COLONY )
            prodSummary += "  max=" + budget->GetAvailBudget().ToString();
        m_OrderList->Add( prodSummary );

        // First RECYCLE all ships
        GenerateProductionRecycle(colony, budget);

        // Launch plugin actions
        int prePluginNum = m_OrderList->Count;
        for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget);
        bool useAuto = m_OrderList->Count == prePluginNum;

        List<Pair<String^, int>^>^ autoOrders = GetAutoOrdersProduction( colony );
        if ( autoOrders )
        {
            String ^prefix = "    ";
            if( !useAuto )
                prefix += "; ";
            for each (Pair<String^, int>^ order in autoOrders )
            {
                m_OrderList->Add( prefix + order->A + String::Format(" ; AUTO (cost {0})", order->B) );
                if (order->B > 0) 
                {
                    budget->Spend(order->B);
                }
                else
                {
                    budget->Recycle(-order->B);
                }
            }
        }

        // Build SHIPYARD
        if( colony->OrderBuildShipyard )
        {
            int cost = Calculators::ShipyardCost( GameData::Player->TechLevels[TECH_MA] );
            m_OrderList->Add( "    Shipyard  ; [A]  -" + cost.ToString() );
            budget->Spend(cost);
        }

        // Now try to CONTINUE or UPGRADE ships here
        GenerateProductionUpgrade(colony, budget);

        // Automatic spendings summary
        if( colony->PlanetType == PLANET_COLONY )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0} (total {1})",
                budget->GetAvailBudget(),
                budget->GetTotalBudget() ) );
        }
        else if( colony->PlanetType == PLANET_HOME )
        {
            m_OrderList->Add( String::Format("    ; -- EU to use: {0}", budget->GetAvailBudget() ) );
        }
    }

    m_OrderList->Add("");
    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        if( ship->Command != nullptr &&
            ship->Command->Type == Ship::OrderType::Recycle &&
            ship->Command->PlanetNum == -1 )    // not yet done
        {
            // If ship is incomplete,
            // add recycle command on planet where it is under construction.
            if( ship->EUToComplete > 0 &&
                ship->PlanetNum != colony->PlanetNum )
                continue;

            int value = ship->GetRecycleValue();
            m_OrderList->Add( String::Format("    Recycle {0}  ; [A] +{1} EU",
                ship->PrintClassWithName(),
                value) );
            budget->Recycle(value);

            // Mark the recycle was done on this planet.
            ship->Command->PlanetNum = colony->PlanetNum;
        }
    }
}

void CommandManager::GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget)
{
    if( colony->CanProduce == false )
        return;

    for each( Ship ^ship in colony->System->ShipsOwned )
    {
        if( ship->EUToComplete > 0 &&
            ship->PlanetNum == colony->PlanetNum )
        {
            // Continue ship if no recycle order issued
            if( ship->Command == nullptr ||
                ship->Command->Type != Ship::OrderType::Recycle )
            {
                m_OrderList->Add( String::Format("    Continue {0}  ; [A] -{1} EU",
                    ship->PrintClassWithName(),
                    ship->EUToComplete) );
                budget->Spend(ship->EUToComplete);
                continue;
            }
        }

        if( ship->Command != nullptr &&
            ship->Command->Type == Ship::OrderType::Upgrade &&
            ship->Command->PlanetNum == -1 )    // not yet done
        {
            int value = Calculators::ShipUpgradeCost(ship->Age, ship->OriginalCost);
            bool doUpgrade = true;
            // Does this colony has budget for upgrade?
            // If budget exceeded search for another colony
            // that could handle the upgrade.
            if( value > budget->GetAvailBudget() )
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
                m_OrderList->Add( String::Format("    Upgrade {0}  ; [A] -{1} EU",
                    ship->PrintClassWithName(),
                    value) );
                budget->Spend(value);

                // Mark the upgrade was done on this planet.
                ship->Command->PlanetNum = colony->PlanetNum;
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
            cmd->Print(m_OrderList);
    }

    if ( AutoEnabled )
    {
        m_OrderList->Add( "  AUTO" );
        GenerateScanOrders();
    }

    for each( IOrdersPlugin ^plugin in PluginManager::OrderPlugins )
    {
        for each( Ship ^ship in GameData::Player->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::GenerateScanOrders()
{
    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->CanJump &&
            ship->Type == SHIP_TR &&
            ship->Size == 1 )
        {
            m_OrderList->Add("  Scan " + ship->PrintClassWithName());
        }
    }
}

void CommandManager::GenerateStrikes()
{
    m_OrderList->Add("START STRIKES");

    // Print UI commands
    for each( ICommand ^cmd in GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Strike )
            cmd->Print(m_OrderList);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void CommandManager::SetAutoOrderPreDeparture(StarSystem^ system, String^ line)
{   
    if( !m_CommandData[m_CurrentTurn]->AutoOrdersPreDeparture->ContainsKey( system ) )
    {
        m_CommandData[m_CurrentTurn]->AutoOrdersPreDeparture->Add(system, gcnew List<String^>);
    }
    m_CommandData[m_CurrentTurn]->AutoOrdersPreDeparture[system]->Add(line);
}

void CommandManager::SetAutoOrderJumps(Ship^ ship, String^ line)
{
    if( !m_CommandData[m_CurrentTurn]->AutoOrdersJumps->ContainsKey( ship ) )
    {
        m_CommandData[m_CurrentTurn]->AutoOrdersJumps->Add(ship, gcnew List<String^>);
    }
    m_CommandData[m_CurrentTurn]->AutoOrdersJumps[ship]->Add(line);
}

void CommandManager::SetAutoOrderProduction(Colony^ colony, String^ line, int cost)
{
    if( !m_CommandData[m_CurrentTurn]->AutoOrdersProduction->ContainsKey( colony ) )
    {
        m_CommandData[m_CurrentTurn]->AutoOrdersProduction->Add(colony, gcnew List<Pair<String^, int>^>);
    }
    m_CommandData[m_CurrentTurn]->AutoOrdersProduction[colony]->Add(gcnew Pair<String^, int>(line, cost));
}

List<String^>^ CommandManager::GetAutoOrdersPreDeparture(StarSystem^ system)
{
    if ( m_CommandData[m_CurrentTurn]->AutoOrdersPreDeparture->ContainsKey( system ) )
    {
        return m_CommandData[m_CurrentTurn]->AutoOrdersPreDeparture[system];
    }
    return nullptr;
}

List<String^>^ CommandManager::GetAutoOrdersJumps(Ship^ ship)
{
    if ( m_CommandData[m_CurrentTurn]->AutoOrdersJumps->ContainsKey( ship ) )
    {
        return m_CommandData[m_CurrentTurn]->AutoOrdersJumps[ship];
    }
    return nullptr;
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

