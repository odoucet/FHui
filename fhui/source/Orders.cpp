#include "StdAfx.h"
#include "Form1.h"
#include "Report.h"
#include "Enums.h"

using namespace System::IO;

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

private value struct OrdersDir
{
    static initonly String^ Folder   = "orders/FHUI.Data/";
    static initonly String^ Commands = "cmd_t{0}.txt";
};

void Form1::AddCommand(ICommand ^cmd)
{
    m_GameData->AddCommand(cmd);
    SaveCommands();
}

void Form1::DelCommand(ICommand ^cmd)
{
    m_GameData->DelCommand(cmd);
    SaveCommands();
}

void Form1::DeleteCommands()
{
    FileInfo ^fileInfo = gcnew FileInfo(
        GetDataDir(OrdersDir::Folder) + String::Format(OrdersDir::Commands, m_GameData->GetLastTurn()) );
    if( fileInfo->Exists )
        fileInfo->Delete();
}

void Form1::SaveCommands()
{
    // Create directory
    String^ dir = GetDataDir(OrdersDir::Folder);
    DirectoryInfo ^dirInfo = gcnew DirectoryInfo(dir);
    if( !dirInfo->Exists )
        dirInfo->Create();

    // Create stream
    StreamWriter ^sw = File::CreateText(
        dir + String::Format(OrdersDir::Commands, m_GameData->GetLastTurn()) );

    // Header
    sw->WriteLine("; FHUI generated file. Please don't edit.");

    // Prepare commands
    m_OrderList->Clear();

    // -- Colonies
    GameData::Player->SortColoniesByProdOrder();
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        m_OrderList->Add( "COLONY " + colony->Name );
        if( colony->OrderBuildShipyard )
            m_OrderList->Add("  build shipyard");
    }

    // -- Ships
    for each( Ship ^ship in GameData::Player->Ships )
        if( ship->Command )
            m_OrderList->Add( String::Format("SHIP {0} {1}",
                ship->PrintClassWithName(),
                ship->Command->PrintNumeric()) );

    // -- Commands
    m_GameData->SortCommands();
    for each( ICommand ^cmd in m_GameData->GetCommands() )
        cmd->Print(m_OrderList);

    // Write to stream
    for each( String ^cmd in m_OrderList )
        sw->WriteLine(cmd);
    sw->Close();
}

void Form1::LoadCommands()
{
    // Open file
    StreamReader ^sr;
    try {
        sr = File::OpenText(
            GetDataDir(OrdersDir::Folder) + String::Format(OrdersDir::Commands, m_GameData->GetLastTurn()) );
    } catch( DirectoryNotFoundException^ )
    {
        return;
    } catch( FileNotFoundException^ )
    {
        return;
    }

    String ^line;
    int colonyProdOrder = 1;
    m_ColoniesMenuRef = nullptr;
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
                m_ColoniesMenuRef = colony;
            }
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (colony production order)!");
        }
        else if( m_RM->Match(line, m_RM->ExpCmdBuiShipyard) )
        {
            if( m_ColoniesMenuRef )
                m_ColoniesMenuRef->OrderBuildShipyard = true;
            else
                throw gcnew FHUIParsingException("Inconsistent commands template (shipyard)!");
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
        /*
        // WORMHOLE TODO
        else if( m_RM->Match(line, m_RM->ExpCmdShipWormhole) )
        {
            m_GameData->GetShip(m_RM->Results[1])->Command =
                gcnew Ship::Order(
                    Ship::OrderType::Wormhole,
                    m_GameData->GetStarSystem(
                        m_RM->GetResultInt(2),
                        m_RM->GetResultInt(3),
                        m_RM->GetResultInt(4)),
                    m_RM->GetResultInt(5) );
        }
        */
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
            Planet ^planet = system->GetPlanet( m_RM->GetResultInt(3) );
            String ^name = m_RM->Results[4];
            m_GameData->AddCommand(
                gcnew CmdPlanetName(system, planet->Number, name) );
            planet->AddName(name);
        }
        else if( m_RM->Match(line, m_RM->ExpCmdPLDisband) )
        {
            String ^name = m_RM->Results[0];
            for each( PlanetName ^pn in m_GameData->GetPlanetNames() )
            {
                if( pn->Name == name )
                {
                    m_GameData->AddCommand( gcnew CmdDisband(name) );
                    pn->System->GetPlanet( pn->PlanetNum )->DelName();
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
            m_GameData->AddCommand(gcnew CmdAlienRelation(alien, SP_NEUTRAL));
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPAlly) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_ENEMY &&
                alien->Relation != SP_NEUTRAL )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (ally)!");

            alien->Relation = SP_ALLY;
            m_GameData->AddCommand(gcnew CmdAlienRelation(alien, SP_ALLY));
        }
        else if( m_RM->Match(line, m_RM->ExpCmdSPEnemy) )
        {
            Alien ^alien = m_GameData->GetAlien(m_RM->Results[0]);
            if( alien->Relation != SP_NEUTRAL &&
                alien->Relation != SP_ALLY )
                throw gcnew FHUIParsingException("Inconsistent alien relation command (enemy)!");

            alien->Relation = SP_ENEMY;
            m_GameData->AddCommand(gcnew CmdAlienRelation(alien, SP_ENEMY));
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

            m_GameData->AddCommand( gcnew CmdTeach(alien, tech, level) );
            alien->TeachOrders |= 1 << tech;
        }
        else
            throw gcnew FHUIParsingException("Unrecognized line in commands template: " + line);
    }
}

////////////////////////////////////////////////////////////////
// Order Template

void Form1::GenerateTemplate()
{
    m_OrderList->Clear();

    m_GameData->SortCommands();

    GenerateCombat();
    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();

    OrderTemplate->Font = gcnew System::Drawing::Font("Courier New", 8, FontStyle::Regular);

    OrderTemplate->Lines = m_OrderList->ToArray();

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
            OrderTemplate->Select(start, s->Length + 1);
            OrderTemplate->SelectionFont = sectionFont;
        }
        else if( s->Trim()[0] == ';' )
        {   // comments
            OrderTemplate->Select(start, s->Length + 1);
            OrderTemplate->SelectionColor = Color::Green;
        }
        else if( s->IndexOf("[A]") != -1 )
        {   // UI generated
            OrderTemplate->Select(start, s->Length + 1);
            OrderTemplate->SelectionColor = Color::FromArgb(0xFF800080);
        }
        else if( s->IndexOf("[A+]") != -1 )
        {   // plugin generated
            OrderTemplate->Select(start, s->Length + 1);
            OrderTemplate->SelectionColor = Color::Blue;
        }
        
        start += s->Length + 1;
    }
}

void Form1::GenerateCombat()
{
    m_OrderList->Add("START COMBAT");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Combat )
            cmd->Print(m_OrderList);
    }

    // TODO: Sort locations by relevance (effective tonnage)

    for each ( StarSystem^ system in m_GameData->GetStarSystems() )
    {
        if ( system->ShipsOwned->Count > 0 )
        {
            GenerateCombatInfo( system );
            for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            {
                plugin->GenerateCombat( m_OrderList, system );
            }

            m_OrderList->Add("");
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

List<String^>^ Form1::PrintSystemStatus(StarSystem^ system, bool listIncomplete)
{
    List<String^>^ status = gcnew List<String^>;

    // list own colonies
    for each ( Colony^ colony in system->ColoniesOwned )
    {
        String^ inv = colony->PrintInventory();
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
        status->Add( String::Format("  ;   #{0} PL {1} ({2}.{3}){4}{5}", 
            colony->PlanetNum,
            colony->Name,
            colony->EconomicBase / 10,
            colony->EconomicBase % 10,
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

void Form1::GenerateCombatInfo(StarSystem^ system)
{
    m_OrderList->Add( String::Format("  ; Battle location: {0}", system->PrintLocation() ) );
    m_OrderList->AddRange( PrintSystemStatus(system, false) );
}

void Form1::GeneratePreDeparture()
{
    m_OrderList->Add("START PRE-DEPARTURE");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
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

        List<String^>^ autoOrders = m_GameData->GetAutoOrdersPreDeparture( system );
        if ( autoOrders )
        {
            for each (String^ line in autoOrders )
            {
                m_OrderList->Add( "  " + line + " ; AUTO" );
            }
        }

        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
        {
            plugin->GeneratePreDeparture(m_OrderList, system);
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GeneratePreDepartureInfo(StarSystem^ system)
{
    m_OrderList->Add(String::Format("  ; Location: {0}", system->PrintLocation() ) );
    m_OrderList->AddRange( PrintSystemStatus(system, false) );
}

void Form1::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");
    
    List<Ship^>^ jumpList = GameData::Player->Ships;
    jumpList->Sort( gcnew Ship::TypeTonnageLocationComparer );

    for each( Ship ^ship in jumpList )
    {
        GenerateJumpInfo(ship);

        int prevLines = m_OrderList->Count;

        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
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
                if( ship->Command->PlanetNum != -1 )
                {
                    Planet ^pl = ship->Command->JumpTarget->GetPlanet( ship->Command->PlanetNum );
                    if( pl && !String::IsNullOrEmpty(pl->Name) )
                        order += ", PL " + pl->Name;
                    else
                        order += ", " + ship->Command->PlanetNum.ToString();
                }
                order += " ; Distance: " + ship->System->CalcDistance( ship->System->WormholeTarget ).ToString("F1");
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

        List<String^>^ autoOrders = m_GameData->GetAutoOrdersJumps( ship );
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

void Form1::GenerateJumpInfo(Ship^ ship)
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

void Form1::GenerateProduction()
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
        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget);
        bool useAuto = m_OrderList->Count == prePluginNum;

        List<Pair<String^, int>^>^ autoOrders = m_GameData->GetAutoOrdersProduction( colony );
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

void Form1::GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget)
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

void Form1::GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget)
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

void Form1::GeneratePostArrival()
{
    m_OrderList->Add("START POST-ARRIVAL");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PostArrival )
            cmd->Print(m_OrderList);
    }

    if ( m_GameData->AutoEnabled )
    {
        m_OrderList->Add( "  AUTO" );
        GenerateScanOrders();
    }

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in GameData::Player->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateScanOrders()
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

void Form1::GenerateStrikes()
{
    m_OrderList->Add("START STRIKES");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Strike )
            cmd->Print(m_OrderList);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::CopyOrdersTemplateToClipboard()
{
    Clipboard::SetText(OrderTemplate->Text, TextDataFormat::Text);
}

} // end namespace FHUI
