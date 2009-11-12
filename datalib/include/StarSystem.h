#pragma once
#include "GridDataSrcBase.h"
#include "Planet.h"
#include "Commands.h"

namespace FHUI
{

ref class Colony;
ref class Ship;
ref class CmdTransfer;

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
        IsMarkedVisited = false;
        LastVisited = -1;
        MinLSN = 99999;
        MinLSNAvail = 99999;
        HasWormhole = false;
        WormholeTargetId = -1;
        WormholeColor = System::Drawing::Color::White;
        Master = nullptr;
        m_Ships = gcnew List<Ship^>;
        m_ShipsOwned = gcnew List<Ship^>;
        m_ShipsAlien = gcnew List<Ship^>;
        m_Colonies = gcnew List<Colony^>;
        m_ColoniesOwned = gcnew SortedList<int, Colony^>;
        m_ColoniesAlien = gcnew List<Colony^>;
        Planets = gcnew SortedList<int, Planet^>;
        Transfers = gcnew List<ICommand^>;
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
        IsMarkedVisited = src->IsMarkedVisited;
        LastVisited = src->LastVisited;
        MinLSN = src->MinLSN;
        MinLSNAvail = src->MinLSNAvail;
        HasWormhole = src->HasWormhole;
        WormholeTargetId = src->WormholeTargetId;
        WormholeColor = src->WormholeColor;
        IsVoid = src->IsVoid;

        Master = nullptr;
        m_Ships = gcnew List<Ship^>;
        m_ShipsOwned = gcnew List<Ship^>;
        m_ShipsAlien = gcnew List<Ship^>;
        m_Colonies = gcnew List<Colony^>;
        m_ColoniesOwned = gcnew SortedList<int, Colony^>;
        m_ColoniesAlien = gcnew List<Colony^>;
        Planets = gcnew SortedList<int, Planet^>;
        Transfers = gcnew List<ICommand^>;

        for each ( Planet^ planet in src->Planets->Values )
        {
            Planets->Add(planet->Number, gcnew Planet(this, planet));
        }
    }
    // -------- IComparable ----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }
    virtual String^     GetTooltipText() override       { return m_Tooltip; }

    virtual StarSystem^ GetFilterSystem() override      { return this; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = -1; return this; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return MinLSN; }
    virtual int         GetFilterNumColonies() override { return m_Colonies->Count; }
    // --------------------------------------------------

    double          CalcDistance(StarSystem ^s);
    double          CalcMishap(StarSystem ^s, int gv, int age);
    double          CalcDistance(int x, int y, int z);
    double          CalcMishap(int x, int y, int z, int gv, int age);

    int             CompareLocation(StarSystem ^sys);

    int             GetId();

    bool            IsExplored() { return TurnScanned != -1 || IsMarkedVisited; }

    bool            IsWormholeTarget(StarSystem ^system);
    StarSystem^     GetWormholeTarget();

    void            UpdateMaster();
    void            UpdateTooltip();

    void            AddShip(Ship ^ship);
    void            RemoveShip(Ship ^ship);
    void            AddColony(Colony ^colony);
    void            DeleteAlienColonies();
    void            SortShipsByTonnage();

    String^         PrintLocation() { return String::Format("{0} {1} {2}", X, Y, Z); }
    String^         PrintWormholeTarget();
    String^         PrintLocationAligned() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^         PrintScanStatus();
    String^         PrintColoniesAll();
    String^         PrintColonies(int planetNum);
    String^         PrintComment();
    List<String^>^  PrintAliens();

    String^         GenerateScan(bool hideLSN);
    String^         GenerateColoniesInfo();
    String^         GenerateShipsInfo();

    List<Ship^>^    GetShipTargets(CommandPhase phase);

    void            SetWormhole(int targetId);

    int             X;
    int             Y;
    int             Z;
    String^         Type;
    String^         Comment;
    String^         CommentHome;    // comment about destroyed home planet
    int             TurnScanned;
    bool            IsMarkedVisited;
    int             LastVisited;
    int             MinLSN;
    int             MinLSNAvail;
    bool            HasWormhole;
    int             WormholeTargetId;
    System::Drawing::Color  WormholeColor;

    Alien^          HomeSpecies;
    Alien^          Master;
    bool            IsVoid;

    property IList<Ship^>^   Ships         { IList<Ship^>^ get()   { return m_Ships->AsReadOnly(); } }
    property IList<Ship^>^   ShipsOwned    { IList<Ship^>^ get()   { return m_ShipsOwned->AsReadOnly(); } }
    property IList<Ship^>^   ShipsAlien    { IList<Ship^>^ get()   { return m_ShipsAlien->AsReadOnly(); } }
    property IList<Colony^>^ Colonies      { IList<Colony^>^ get() { return m_Colonies->AsReadOnly(); } }
    property IList<Colony^>^ ColoniesOwned { IList<Colony^>^ get() { return m_ColoniesOwned->Values; } }
    property IList<Colony^>^ ColoniesAlien { IList<Colony^>^ get() { return m_ColoniesAlien->AsReadOnly(); } }

    SortedList<int, Planet^>^ Planets;

    List<ICommand^>^        GetTransfers(Colony ^colony);
    List<ICommand^>^        GetTransfers(Ship ^ship);
    // Returns true when transfer to or from colony/ship is possible in this system
    // Colony and Ship args should never be both set in single call
    bool                    IsTransferPossible(CommandPhase phase, Colony ^colony, Ship ^ship);
    List<ICommand^>^        Transfers;

protected:

    String^                 m_Tooltip;
    String^                 m_TooltipScan;
    String^                 m_TooltipColonies;
    String^                 m_TooltipShips;

    List<Ship^>^            m_Ships;
    List<Ship^>^            m_ShipsOwned;
    List<Ship^>^            m_ShipsAlien;

    List<Colony^>^            m_Colonies;
    SortedList<int, Colony^>^ m_ColoniesOwned;
    List<Colony^>^            m_ColoniesAlien;

    initonly static String^ s_ScanNone = "Not scanned";
    initonly static String^ s_ScanDipl = "Received";
    initonly static String^ s_ScanDiplVisited = "Recv/Vis";
    initonly static String^ s_ScanSelf = "Scanned";

    static List<System::Drawing::Color>^    s_WHColorsUsed;
};

} // end namespace FHUI
