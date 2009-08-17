#pragma once
#include "GridDataSrcBase.h"
#include "StarSystem.h"

namespace FHUI
{

ref class Planet;
ref class StarSystem;

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
        OrderBuildShipyard = false;
        Hidden = false;
        UnderSiege = false;
        Shared = false;
    }

    // -------- IComparable -----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Owner; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = PlanetNum; return System; }
    virtual Alien^      GetFilterOwner() override       { return Owner; }
    virtual int         GetFilterLSN() override         { return LSN; }
    // -------- IComparers ------------------------------
    ref class ProdOrderComparer : public IComparer<Colony^>
    {
    public:
        virtual int Compare(Colony ^c1, Colony ^c2)
        {
            return c1->ProductionOrder - c2->ProductionOrder;
        }
    };
    // --------------------------------------------------

    String^         PrintLocation() { return String::Format("{0} {1}", System->PrintLocation(), PlanetNum); }
    String^         PrintInventory();
    String^         PrintRefListEntry();
    String^         PrintBalance();

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
    bool            OrderBuildShipyard;
    bool            Hidden;
    bool            UnderSiege;
    bool            Shared;

    property int EconomicBase { int get() { return Math::Max(-1, MiBase + MaBase); } }
    property int EUAvail { int get() { return EUProd - EUFleet; } }

    property array<int>^    Inventory;
    property bool           HasInventory 
    {
        bool get()
        {
            for (int i=0; i<INV_MAX; i++)
                if (Inventory[i] > 0) return true;
            return false;
        }
    }
};

} // end namespace FHUI