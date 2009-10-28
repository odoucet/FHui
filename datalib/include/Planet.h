#pragma once
#include "GridDataSrcBase.h"

namespace FHUI
{

// ---------------------------------------------------
// Planet in star system
public ref class Planet : public GridDataSrcBase
{
public:
    Planet(StarSystem ^s, int nr, int dia, int gv, int tc, int pc, int md)
    {
        System = s;
        Number = nr;
        Name = nullptr;
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
    // --- copy constructor - does not perform full copy !!! ---
    Planet(StarSystem^ system, Planet^ src)
    {
        System = system;
        Number = src->Number;
        // Do NOT copy planet name. This is to enable planet name disband.
        // Planets that still have name will have it read from the report.
        //Name = src->Name;
        AlienName = src->AlienName;
        Comment = src->Comment;
        Diameter = src->Diameter;
        Grav = src->Grav;
        TempClass = src->TempClass;
        PressClass = src->PressClass;
        MiDiff = src->MiDiff;
        LSN = src->LSN;
        Atmosphere = gcnew array<int>(GAS_MAX) {0};
        src->Atmosphere->CopyTo(Atmosphere, 0);

        Master = nullptr;
        NumColonies = 0;
        SuspectedColonies = gcnew SortedList<Alien^, int>;
    }

    static Planet^ Default(StarSystem ^system, int nr)
    {
        return gcnew Planet(system, nr, 99, 99, -1, -1, 999);
    }

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = Number; return System; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return LSN; }
    virtual int         GetFilterNumColonies() override { return NumColonies; }
    virtual String^     GetTooltipText() override;
    // --------------------------------------------------

    String^     GetNameWithOrders();

    String^     PrintLocation();
    String^     PrintLocationAligned();
    String^     PrintComment();

    StarSystem^     System;
    int             Number;
    String^         Name;
    String^         AlienName;
    String^         Comment;
    int             Diameter;
    int             Grav;           // * 100
    int             TempClass;
    int             PressClass;
    int             MiDiff;         // * 100
    int             LSN;
    Alien^          Master;
    int             NumColonies;
    int             NumColoniesOwned;
    array<int>^     Atmosphere;
    SortedList<Alien^, int>^ SuspectedColonies;
};

} // end namespace FHUI
