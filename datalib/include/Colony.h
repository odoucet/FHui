#pragma once
#include "GridDataSrcBase.h"
#include "StarSystem.h"
#include "Commands.h"

namespace FHUI
{

ref class Planet;
ref class StarSystem;
ref class CommandManager;
interface class ICommand;

// ---------------------------------------------------
// Colony
public ref class Colony : public GridDataSrcBase, public IComparable
{
public:
    Colony(Alien ^owner, String ^name, StarSystem ^system, int planetNum)
    {
        Owner = owner;
        Name = name;
        PlanetType = PLANET_HOME;
        System = system;
        Planet = nullptr;
        PlanetNum = planetNum;
        AvailPop = 0;
        EconomicEff = 0;
        ProdPenalty = 0;
        EUProd = 0;
        EUFleet = 0;
        MiBase = -1;
        NeedIU = 0;
        NeedAU = 0;
        RecoveryIU = 0;
        RecoveryAU = 0;
        MiDiff = 0;
        MaBase = -1;
        LSN = 99;
        Shipyards = -1;
        LastSeen = -1;
        Inventory = gcnew array<int>(INV_MAX){0};
        InventoryPreDeparture = gcnew array<int>(INV_MAX){0};
        InventoryPostArrival = gcnew array<int>(INV_MAX){0};
        RMProduced = 0;
        RMUsed = 0;
        Hidden = false;
        UnderSiege = false;
        Shared = false;
        ProductionOrder = 0;
        Commands = gcnew List<ICommand^>;
    }
    // --- copy constructor - does not perform full copy !!! ---
    Colony(StarSystem ^system, Alien^ owner, Colony^ src)
    {
        Owner = owner;
        Name = src->Name;
        PlanetType = src->PlanetType;
        System = system;
        PlanetNum = src->PlanetNum;
        Planet = system->Planets[PlanetNum];
        AvailPop = src->AvailPop;
        EconomicEff = src->EconomicEff;
        ProdPenalty = src->ProdPenalty;
        EUProd = src->EUProd;
        EUFleet = src->EUFleet;
        MiBase = src->MiBase;
        NeedIU = src->NeedIU;
        NeedAU = src->NeedAU;
        RecoveryIU = src->RecoveryIU;
        RecoveryAU = src->RecoveryAU;
        MiDiff = src->MiDiff;
        MaBase = src->MaBase;
        LSN = src->LSN;
        Shipyards = src->Shipyards;
        LastSeen = src->LastSeen;
        Inventory = gcnew array<int>(INV_MAX){0};
        InventoryPostArrival = gcnew array<int>(INV_MAX){0};
        src->Inventory->CopyTo(Inventory, 0);
        RMProduced = 0;
        RMUsed = 0;
        Hidden = false;
        UnderSiege = false;
        Shared = false;
        ProductionOrder = src->ProductionOrder;
        Commands = gcnew List<ICommand^>;
    }
    // -------- IComparable -----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Owner; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = PlanetNum; return System; }
    virtual Alien^      GetFilterOwner() override       { return Owner; }
    virtual int         GetFilterLSN() override         { return LSN; }
    virtual SPRelType   GetFilterRelType() override     { return Owner->GetFilterRelType(); }
    virtual String^     GetTooltipText() override       { return System->GetTooltipText(); }
    // -------- IComparers ------------------------------
    ref class ProdOrderComparer : public IComparer<Colony^>
    {
    public:
        virtual int Compare(Colony ^c1, Colony ^c2)
        {
            return c1->ProductionOrder - c2->ProductionOrder;
        }
    };

    ref class JumpsComparer : public IComparer<Colony^>
    {
    public:
        JumpsComparer(StarSystem ^system)
            : m_System(system)
        {}

        virtual int Compare(Colony ^c1, Colony ^c2)
        {   
            if( c1 == c2 )
                return 0;
            // Home always first
            if( c1->PlanetType == PLANET_HOME )
                return -1;
            if( c2->PlanetType == PLANET_HOME )
                return 1;
            // In the same system, by size
            if( c1->System == c2->System )
                return c2->EconomicBase - c1->EconomicBase;
            // By distance
            double d1 = m_System->CalcDistance(c1->System);
            double d2 = m_System->CalcDistance(c2->System);
            return d1 < d2 ? -1 : (d1 == d2 ? 0 : 1);
        }

    protected:
        StarSystem^ m_System;
    };
    // --------------------------------------------------

    String^         PrintLocation() { return String::Format("{0} {1}", System->PrintLocation(), PlanetNum); }
    String^         PrintInventory(bool original);
    String^         PrintRefListEntry();
    String^         PrintBalance();
    String^         PrintSize();

    void            CalculateBalance(bool MiMaBalanced);
    int             GetMaxProductionBudget();

    Alien^          Owner;
    String^         Name;
    PlanetType      PlanetType;
    StarSystem^     System;
    Planet^         Planet;
    int             PlanetNum;
    int             AvailPop;
    int             EconomicEff;
    int             ProdPenalty;
    int             EUProd;
    int             EUFleet;
    int             MaBase; // *10
    int             MiBase; // *10
    int             NeedIU;
    int             NeedAU;
    int             RecoveryIU;
    int             RecoveryAU;
    int             MiDiff; // *100
    int             LSN;
    int             Shipyards;
    int             LastSeen;
    int             ProductionOrder;
    bool            CanProduce;
    bool            Hidden;
    bool            UnderSiege;
    bool            Shared;
    property bool   IsNew
    {
        bool get()
        {
            return IsNameCommandPending();
        }
    }
    property bool   IsDisband
    {
        bool get()
        {
            return IsDisbandCommandPending();
        }
    }

    property int EconomicBase { int get() { return Math::Max(-1, MiBase + MaBase); } }
    property int EUAvail { int get() { return EUProd - EUFleet; } }

    array<int>^             Inventory;
    array<int>^             InventoryPreDeparture;
    array<int>^             InventoryPostArrival;
    property bool           HasInventory 
    {
        bool get()
        {
            for (int i=0; i<INV_MAX; i++)
                if (Inventory[i] > 0) return true;
            return false;
        }
    }

    int             RMProduced;
    int             RMUsed;

    // ---- Production / budget tracking ----
    ref class Resources
    {
    public:
        int             TotalEU;
        int             AvailEU;
        int             UsedEU;
        int             AvailPop;
        int             AvailShipyards;
        array<int>^     Inventory;
    };

    void                ProductionReset();
    bool                IsNameCommandPending();
    bool                IsDisbandCommandPending();

    String^             PrintCmdDetails(CommandManager ^cmdMgr, int %prodSum);
    String^             PrintCmdDetailsPhase(CommandPhase phase);
    String^             PrintCmdDetailsPhaseProduction(CommandManager ^cmdMgr, int %prodSum);

    List<ICommand^>^    Commands;       // Colony commands
    Resources^          Res;            // Resource tracking for orders template
    List<String^>^      OrdersText;     // Production orders generated for orders template
};

} // end namespace FHUI
