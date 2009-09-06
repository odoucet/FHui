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
        WormholeTargetId = -1;
        Master = nullptr;
        m_Ships = gcnew List<Ship^>;
        m_ShipsOwned = gcnew List<Ship^>;
        m_ShipsAlien = gcnew List<Ship^>;
        m_Colonies = gcnew List<Colony^>;
        m_ColoniesOwned = gcnew SortedList<int, Colony^>;
        m_ColoniesAlien = gcnew List<Colony^>;
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
        WormholeTargetId = src->WormholeTargetId;
        IsVoid = src->IsVoid;

        Master = nullptr;
        m_Ships = gcnew List<Ship^>;
        m_ShipsOwned = gcnew List<Ship^>;
        m_ShipsAlien = gcnew List<Ship^>;
        m_Colonies = gcnew List<Colony^>;
        m_ColoniesOwned = gcnew SortedList<int, Colony^>;
        m_ColoniesAlien = gcnew List<Colony^>;
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
    virtual String^     GetTooltipText() override;

    virtual StarSystem^ GetFilterSystem() override      { return this; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = -1; return this; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return MinLSN; }
    virtual int         GetFilterNumColonies() override { return m_Colonies->Count; }
    // --------------------------------------------------

    double      CalcDistance(StarSystem ^s);
    double      CalcMishap(StarSystem ^s, int gv, int age);
    double      CalcDistance(int x, int y, int z);
    double      CalcMishap(int x, int y, int z, int gv, int age);

    int         CompareLocation(StarSystem ^sys);

    int         GetId();

    bool        IsExplored() { return TurnScanned != -1; }
    bool        IsWormholeTarget(StarSystem ^system)    { return HasWormhole ? system->GetId() == WormholeTargetId : false; }

    String^     GenerateScan();
    void        UpdateMaster();

    void        AddShip(Ship^);
    void        AddColony(Colony^);

    String^         PrintLocation() { return String::Format("{0} {1} {2}", X, Y, Z); }
    String^         PrintWormholeTarget();
    String^         PrintLocationAligned() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^         PrintScanStatus();
    String^         PrintColoniesAll();
    String^         PrintColonies(int planetNum);
    String^         PrintComment();
    List<String^>^  PrintAliens();

    property int            X;
    property int            Y;
    property int            Z;
    property String^        Type;
    property String^        Comment;
    property String^        CommentHome;    // comment about destroyed home planet
    property int            TurnScanned;
    property int            LastVisited;
    property int            MinLSN;
    property int            MinLSNAvail;
    property bool           HasWormhole;
    property int            WormholeTargetId;

    property Alien^         HomeSpecies;
    property Alien^         Master;
    property bool           IsVoid;

    property IList<Ship^>^   Ships         { IList<Ship^>^ get()   { return m_Ships->AsReadOnly(); } }
    property IList<Ship^>^   ShipsOwned    { IList<Ship^>^ get()   { return m_ShipsOwned->AsReadOnly(); } }
    property IList<Ship^>^   ShipsAlien    { IList<Ship^>^ get()   { return m_ShipsAlien->AsReadOnly(); } }
    property IList<Colony^>^ Colonies      { IList<Colony^>^ get() { return m_Colonies->AsReadOnly(); } }
    property IList<Colony^>^ ColoniesOwned { IList<Colony^>^ get() { return m_ColoniesOwned->Values; } }
    property IList<Colony^>^ ColoniesAlien { IList<Colony^>^ get() { return m_ColoniesAlien->AsReadOnly(); } }

    property SortedList<int, Planet^>^ Planets;

protected:

    List<Ship^>^            m_Ships;
    List<Ship^>^            m_ShipsOwned;
    List<Ship^>^            m_ShipsAlien;

    List<Colony^>^            m_Colonies;
    SortedList<int, Colony^>^ m_ColoniesOwned;
    List<Colony^>^            m_ColoniesAlien;

    initonly static String^ s_ScanNone = "Not scanned";
    initonly static String^ s_ScanDipl = "Received";
    initonly static String^ s_ScanSelf = "Scanned";
};

} // end namespace FHUI
