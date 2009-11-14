#pragma once
#include "GridDataSrcBase.h"
#include "Alien.h"
#include "StarSystem.h"
#include "Commands.h"

namespace FHUI
{

// ---------------------------------------------------
// Ship or starbase
public ref class Ship : public GridDataSrcBase, public IComparable
{
public:
    Ship(Alien ^owner, ShipType type, String ^name, bool subLight)
    {
        Owner = owner;
        Type = type;
        Name = name;
        SubLight = subLight;
        CanJump = !subLight && type != SHIP_BAS;
        BuiltThisTurn = false;
        Age = 0;
        Size = 0;
        EUToComplete = 0;
        Location = SHIP_LOC_MAX;
        PlanetNum = -1;
        System = nullptr;
        Capacity = 0;
        IsPirate = false;
        HadMishap = false;
        Cargo = gcnew array<int>(INV_MAX){0};
        CargoOriginal = gcnew array<int>(INV_MAX){0};
        CargoPreDeparture = gcnew array<int>(INV_MAX){0};
        CargoPostArrival = gcnew array<int>(INV_MAX){0};
        DidWormholeJump = false;
        Commands = gcnew List<ICommand^>;
    }

    // --------------------------------------------------
    static Ship^        FindRefListEntry(String ^entry);

    // -------- IComparable -----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Owner; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = PlanetNum; return System; }
    virtual Alien^      GetFilterOwner() override       { return Owner; }
    virtual SPRelType   GetFilterRelType() override     { return Owner->GetFilterRelType(); }
    virtual ShipType    GetFilterShipType() override    { return Type; }
    virtual String^     GetTooltipText() override       { return System->GetTooltipText(); }
    // -------- IComparers ------------------------------
    ref class TypeTonnageLocationComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            if( s1->Type != s2->Type ) return ((int)s1->Type - (int)s2->Type);
            if( s1->Tonnage != s2->Tonnage ) return s1->Tonnage - s2->Tonnage;
            if( s1->System->X != s2->System->X ) return s1->System->X - s2->System->X;
            if( s1->System->Y != s2->System->Y ) return s1->System->Y - s2->System->Y;
            if( s1->System->Z != s2->System->Z ) return s1->System->Z - s2->System->Z;
            if( s1->Age != s2->Age) return s1->Age - s2->Age;
            return s1->Location - s2->Location;
        }
    };

    ref class LocationComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            if( s1->System->X != s2->System->X ) return s1->System->X - s2->System->X;
            if( s1->System->Y != s2->System->Y ) return s1->System->Y - s2->System->Y;
            if( s1->System->Z != s2->System->Z ) return s1->System->Z - s2->System->Z;
            return s1->Location - s2->Location;
        }
    };

    ref class WarTonnageComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            // Newer, Bigger ships first
            if( s2->WarTonnage == s1->WarTonnage )
                return s1->Age - s2->Age;
            return s2->WarTonnage - s1->WarTonnage;
        }
    };
    // --------------------------------------------------

    String^         PrintClass();
    String^         PrintClassWithName()                { return PrintClass() + " " + Name; }
    String^         PrintLocation();
    String^         PrintLocationShort();
    String^         PrintAgeLocation();
    String^         PrintCargo(bool original);
    String^         PrintRefListEntry();

    StarSystem^     GetPostArrivalSystem();

    int             GetUpgradeCost();
    int             GetRecycleValue();

    void            CalculateCapacity();
    void            StoreOriginalCargo();

    Alien^          Owner;
    ShipType        Type;
    String^         Name;
    bool            SubLight;
    bool            CanJump;
    bool            BuiltThisTurn;
    int             Age;
    ShipLocType     Location;
    int             PlanetNum;
    StarSystem^     System;
    int             Capacity;
    bool            IsPirate;
    bool            HadMishap;
    int             Tonnage;
    int             WarTonnage;
    int             OriginalCost;
    int             EUToComplete;
    bool            DidWormholeJump;
    array<int>^     Cargo;
    array<int>^     CargoOriginal;
    array<int>^     CargoPreDeparture;
    array<int>^     CargoPostArrival;

    property int            Size {
        int get() { return m_Size; }
        void set(int val) { m_Size = val; SetupTonnage(); }
    }

    // ---- Ship orders ----
    List<ICommand^>^        Commands;
    int                     CommandProdPlanet;

    void            AddCommand(ICommand ^cmd);
    void            DelCommand(ICommand ^cmd);

    String^         PrintJumpDestination();
    ICommand^       GetJumpCommand();
    ICommand^       GetProdCommand();

    String^         PrintCmdSummary();
    String^         PrintCmdDetails();
    String^         PrintCmdDetailsPhase(CommandPhase phase);

protected:
    void            SetupTonnage();

    int             m_Size;
};

} // end namespace FHUI
