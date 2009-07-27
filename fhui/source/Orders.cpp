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
    m_GameData->GetSpecies()->SortColoniesByProdOrder();
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
        m_OrderList->Add( "COLONY " + colony->Name );

    // -- Ships
    for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
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
    //try {
        sr = File::OpenText(
            GetDataDir(OrdersDir::Folder) + String::Format(OrdersDir::Commands, m_GameData->GetLastTurn()) );
    //}

    String ^line;
    int colonyProdOrder = 1;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        line->Trim();
        if( String::IsNullOrEmpty(line) ||
            line[0] == ';' )
            continue;

        if( m_RM->Match(line, m_RM->ExpCmdColony) )
        {
            Colony ^colony = m_GameData->GetColony(m_RM->Results[0]);
            if( colony->Owner == m_GameData->GetSpecies() )
                colony->ProductionOrder = colonyProdOrder++;
            else
                throw gcnew FHUIParsingException("Inconsistent commands template!");
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

    OrderTemplate->Lines = m_OrderList->ToArray();
}

void Form1::GenerateCombat()
{
    m_OrderList->Add("START COMBAT");
    m_OrderList->Add("");

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

void Form1::GenerateCombatInfo(StarSystem^ system)
{
    Alien^ player = m_GameData->GetSpecies();

    String^ sysInfo = system->PrintColonies(-1, player );
    if ( String::IsNullOrEmpty(sysInfo) )
    {
        sysInfo = "empty";
    }

    m_OrderList->Add(
        String::Format("; Location: {0} ({1})", system->PrintLocation(), sysInfo ) );

    List<String^>^ MyShipInfo = gcnew List<String^>;
    List<String^>^ AlienShipInfo = gcnew List<String^>;

    for each ( Ship^ ship in system->Ships )
    {
        String^ shipLoc;
        if (ship->Location == SHIP_LOC_DEEP_SPACE )
        {
            shipLoc = "Deep";
        }
        else
        {
            shipLoc = ship->PrintLocation();
        }

        if ( ship->Owner == player )
        {
            MyShipInfo->Add(
                String::Format(";    {0} ({1}) [{2}]", ship->PrintClassWithName(), shipLoc, ship->PrintCargo() ) );
        }
        else
        {
            AlienShipInfo->Add(
                String::Format(";    {0} ({1})    SP {2}", ship->PrintClassWithName(), shipLoc, ship->Owner->Name ) );
        }
    }
    m_OrderList->AddRange( MyShipInfo );
    m_OrderList->Add( String::Format("; Alien ships: {0}", ( AlienShipInfo->Count ? "" : "none" ) ) );
    m_OrderList->AddRange( AlienShipInfo );
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

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
            plugin->GeneratePreDeparture(m_OrderList, ship);

        m_OrderList->Add("");

        for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
            plugin->GeneratePreDeparture(m_OrderList, colony);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateJumps()
{
    m_OrderList->Add("START JUMPS");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Jump )
            cmd->Print(m_OrderList);
    }

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
        {
            plugin->GenerateJumps(m_OrderList, ship);

            if( ship->Command && ship->Command->Type == Ship::OrderType::Jump )
            {
                m_OrderList->Add( String::Format("    Jump {0}, {1}  ; [{2}] -> [{3}]; Mishap: {4:F2}%",
                    ship->PrintClassWithName(),
                    ship->Command->PrintJumpDestination(),
                    ship->System->PrintLocation(),
                    ship->Command->JumpTarget->PrintLocation(),
                    ship->System->CalcMishap(
                        ship->Command->JumpTarget,
                        ship->Owner->TechLevelsAssumed[TECH_GV],
                        ship->Age) ) );
            }
        }
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateProduction()
{
    m_OrderList->Add("START PRODUCTION");

    BudgetTracker ^budget = gcnew BudgetTracker(m_OrderList, m_GameData->GetCarriedEU());
    m_OrderList->Add("; +" + budget->GetTotalBudget().ToString() + " EUs carried" );

    // Mark ships for upgrade
    for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
    {
        if( ship->Command != nullptr &&
            (   ship->Command->Type == Ship::OrderType::Upgrade ||
                ship->Command->Type == Ship::OrderType::Recycle ) )
        {
            ship->Command->PlanetNum = -1;
        }
    }

    // Sort colonies for production
    m_GameData->GetSpecies()->SortColoniesByProdOrder();

    // Generate production template for each colony
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
    {
        m_OrderList->Add("");
        m_OrderList->Add("  PRODUCTION PL " + colony->Name);

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
        for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
            plugin->GenerateProduction(m_OrderList, colony, budget);

        // Now try to UPGRADE ships here
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
            int value = Calculators::ShipRecycleValue(ship->Age, ship->OriginalCost);
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
    m_OrderList->Add("");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::PostArrival )
            cmd->Print(m_OrderList);
    }

    for each( IOrdersPlugin ^plugin in m_OrdersPlugins )
    {
        for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
            plugin->GeneratePostArrival(m_OrderList, ship);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

void Form1::GenerateStrikes()
{
    m_OrderList->Add("START STRIKES");
    m_OrderList->Add("");

    // Print UI commands
    for each( ICommand ^cmd in m_GameData->GetCommands() )
    {
        if( cmd->GetPhase() == CommandPhase::Strike )
            cmd->Print(m_OrderList);
    }

    m_OrderList->Add("END");
    m_OrderList->Add("");
}

} // end namespace FHUI
