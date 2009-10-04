#pragma once
#include "GridDataSrcBase.h"
#include "Alien.h"
#include "StarSystem.h"

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
        Age = 0;
        Size = 0;
        EUToComplete = 0;
        Location = SHIP_LOC_MAX;
        PlanetNum = -1;
        System = nullptr;
        Capacity = 0;
        IsPirate = false;
        m_Cargo = gcnew array<int>(INV_MAX){0};
        DidWormholeJump = false;
    }

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
    String^         PrintCargo();
    String^         PrintRefListEntry();

    int             GetUpgradeCost();
    int             GetRecycleValue();

    void            CalculateCapacity();

    property Alien^         Owner;
    property ShipType       Type;
    property String^        Name;
    property bool           SubLight;
    property bool           CanJump;
    property int            Age;
    property ShipLocType    Location;
    property int            PlanetNum;
    property StarSystem^    System;
    property int            Capacity;
    property bool           IsPirate;
    property int            Tonnage;
    property int            WarTonnage;
    property int            OriginalCost;
    property int            EUToComplete;
    property bool           DidWormholeJump;

    property int            Size {
        int get() { return m_Size; }
        void set(int val) { m_Size = val; SetupTonnage(); }
    }

    property int            Cargo [int] {
        int  get(int inv)           { return m_Cargo[inv]; }
        void set(int inv, int val)  { m_Cargo[inv] = val; }
    }

    // ---- Ship orders ----
    enum class OrderType
    {
        Jump, Upgrade, Recycle, Wormhole
    };

    ref class Order
    {
    public:
        Order(OrderType type)
            : Type(type)
            , JumpTarget(nullptr)
            , PlanetNum(-1)
        {}
        Order(OrderType type, StarSystem ^system)
            : Type(type)
            , JumpTarget(system)
            , PlanetNum(-1)
        {}
        Order(OrderType type, StarSystem ^system, int plNum)
            : Type(type)
            , JumpTarget(system)
            , PlanetNum(plNum)
        {}

        String^     Print();
        String^     PrintNumeric();
        String^     PrintJumpDestination();

        OrderType   Type;
        StarSystem^ JumpTarget;
        int         PlanetNum;
    };

    property Order^ Command;

protected:
    void            SetupTonnage();

    int             m_Size;
    array<int>     ^m_Cargo;
};

} // end namespace FHUI
