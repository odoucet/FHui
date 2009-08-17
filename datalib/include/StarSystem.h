#pragma once
#include "GridDataSrcBase.h"

namespace FHUI
{

ref class Colony;
ref class Planet;
ref class Ship;

// ---------------------------------------------------
// Star system
public ref class StarSystem : public GridDataSrcBase, public IComparable
{
public:
    StarSystem(int x, int y, int z, String ^type)
        : m_Planets(gcnew array<Planet^>(0))
    {
        X = x;
        Y = y;
        Z = z;
        Type = type;
        TurnScanned = -1;
        LastVisited = -1;
        MinLSN = 99999;
        MinLSNAvail = 99999;
        HasWormhole = false;
        Master = nullptr;
        Ships = gcnew List<Ship^>;
        ShipsOwned = gcnew List<Ship^>;
        ShipsAlien = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;
        ColoniesOwned = gcnew List<Colony^>;
        ColoniesAlien = gcnew List<Colony^>;
        IsVoid = true;
    }

    // -------- IComparable ----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }
    virtual String^     GetTooltipText() override       { return GenerateScan(); }

    virtual StarSystem^ GetFilterSystem() override      { return this; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = -1; return this; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return MinLSN; }
    virtual int         GetFilterNumColonies() override { return Colonies->Count; }
    // --------------------------------------------------

    double      CalcDistance(StarSystem ^s);
    double      CalcMishap(StarSystem ^s, int gv, int age);
    double      CalcDistance(int x, int y, int z);
    double      CalcMishap(int x, int y, int z, int gv, int age);

    int         CompareLocation(StarSystem ^sys);

    int         GetId();
    Planet^     GetPlanet(int plNum);

    bool        IsExplored() { return TurnScanned != -1; }

    String^     GenerateScan();
    void        UpdateMaster();

    String^         PrintLocation() { return String::Format("{0} {1} {2}", X, Y, Z); }
    String^         PrintWormholeTarget() { return HasWormhole ? (WormholeTarget ? WormholeTarget->PrintLocation() : "???") : nullptr; }
    String^         PrintLocationAligned() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^         PrintScanStatus();
    String^         PrintColonies(int planetNum, Alien ^player);   // -1 for all colonies in system
    List<String^>^  PrintAliens();

    property int            X;
    property int            Y;
    property int            Z;
    property String^        Type;
    property String^        Comment;
    property int            TurnScanned;
    property int            LastVisited;
    property int            MinLSN;
    property int            MinLSNAvail;
    property bool           HasWormhole;
    property StarSystem^    WormholeTarget;

    property Alien^         HomeSpecies;
    property Alien^         Master;
    property bool           IsVoid;

    property List<Ship^>^   Ships;
    property List<Ship^>^   ShipsOwned;
    property List<Ship^>^   ShipsAlien;
    property List<Colony^>^ Colonies;
    property List<Colony^>^ ColoniesOwned;
    property List<Colony^>^ ColoniesAlien;

    property int        PlanetsCount { int get() { return m_Planets->Length; } }
    property Planet^    Planets [int] {
        Planet^ get(int i) { return i < m_Planets->Length ? m_Planets[i] : nullptr; }
        void    set(int i, Planet^);
    }

    array<Planet^>^     GetPlanets()        { return m_Planets; }

protected:
    array<Planet^>     ^m_Planets;

    initonly static String^ s_ScanNone = "Not scanned";
    initonly static String^ s_ScanDipl = "Received";
    initonly static String^ s_ScanSelf = "Scanned";
};

} // end namespace FHUI
