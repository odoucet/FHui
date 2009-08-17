#pragma once
#include "GridDataSrcBase.h"

namespace FHUI
{

// ---------------------------------------------------
// Planet in star system
public ref class Planet : public GridDataSrcBase
{
public:
    Planet(StarSystem ^s, int nr, int dia, float gv, int tc, int pc, int md)
    {
        System = s;
        Number = nr;
        Name = nullptr;
        NameIsNew = false;
        NameIsDisband = false;
        Comment = nullptr;
        Diameter = dia;
        Grav = gv;
        TempClass = tc;
        PressClass = pc;
        MiDiff = md;
        LSN = -1;
        Master = nullptr;
        NumColonies = 0;
        Atmosphere = gcnew array<int>(GAS_MAX) {0};
        SuspectedColonies = gcnew SortedList<Alien^, int>;
    }

    static Planet^ Default(StarSystem ^system, int nr)
    {
        return gcnew Planet(system, nr, 99, 99.99F, 99, 99, 999);
    }

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = Number; return System; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return LSN; }
    virtual int         GetFilterNumColonies() override { return NumColonies; }
    // --------------------------------------------------

    int         CalculateLSN();

    String^     GetNameWithOrders();
    void        AddName(String ^name);
    void        DelName();

    String^     PrintLocation();
    String^     PrintLocationAligned();
    String^     PrintComment();

    property StarSystem^    System;
    property int            Number;
    property String^        Name;
    property bool           NameIsNew;
    property bool           NameIsDisband;
    property String^        Comment;
    property int            Diameter;
    property float          Grav;
    property int            TempClass;
    property int            PressClass;
    property int            MiDiff; // * 100
    property int            LSN;
    property Alien^         Master;
    property int            NumColonies;
    property int            NumColoniesOwned;
    property array<int>^    Atmosphere;
    property SortedList<Alien^, int>^ SuspectedColonies;
};

// ---------------------------------------------------
// Named planet info
public ref class PlanetName
{
public:
    PlanetName(StarSystem ^system, int num, String ^name)
    {
        System      = system;
        PlanetNum   = num;
        Name        = name;
    }

    property StarSystem^    System;
    property int            PlanetNum;
    property String^        Name;
};

} // end namespace FHUI
