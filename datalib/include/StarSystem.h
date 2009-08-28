#pragma once
#include "GridDataSrcBase.h"
#include "Planet.h"

namespace FHUI
{

ref class Colony;
ref class Ship;

// ---------------------------------------------------
// Star system
public ref class StarSystem : public GridDataSrcBase, public IComparable
{
public:
    StarSystem(int x, int y, int z, String ^type)
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
        Planets = gcnew SortedList<int, Planet^>;
        IsVoid = true;
    }
    // --- copy constructor - does not perform full copy !!! ---
    StarSystem(StarSystem^ src)
    {
        X = src->X;
        Y = src->Y;
        Z = src->Z;
        Type = src->Type;
        TurnScanned = src->TurnScanned;
        LastVisited = src->LastVisited;
        MinLSN = src->MinLSN;
        MinLSNAvail = src->MinLSNAvail;
        HasWormhole = src->HasWormhole;
        IsVoid = src->IsVoid;

        Master = nullptr;
        Ships = gcnew List<Ship^>;
        ShipsOwned = gcnew List<Ship^>;
        ShipsAlien = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;
        ColoniesOwned = gcnew List<Colony^>;
        ColoniesAlien = gcnew List<Colony^>;
        Planets = gcnew SortedList<int, Planet^>;

        for each ( Planet^ planet in src->Planets->Values )
        {
            Planets->Add(planet->Number, gcnew Planet(this, planet));
        }
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

    property SortedList<int, Planet^>^ Planets;

protected:

    initonly static String^ s_ScanNone = "Not scanned";
    initonly static String^ s_ScanDipl = "Received";
    initonly static String^ s_ScanSelf = "Scanned";
};

} // end namespace FHUI
