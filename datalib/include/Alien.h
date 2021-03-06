#pragma once
#include "GridDataSrcBase.h"

namespace FHUI
{

ref class Ship;
ref class Colony;

// ---------------------------------------------------
// Atmospheric requirements
public ref class AtmosphericReq
{
public:
    AtmosphericReq()
    {
        GasRequired = GAS_MAX;
        ReqMin = 0;
        ReqMax = 0;
        TempClass = -1;
        PressClass = -1;

        m_Neutral = gcnew array<bool>(GAS_MAX){false};
        m_Poisonous = gcnew array<bool>(GAS_MAX){false};
    }

    bool IsValid() { return GasRequired != GAS_MAX && TempClass != -1 && PressClass != -1; }

    property GasType    GasRequired;
    property int        ReqMin;
    property int        ReqMax;
    property int        TempClass;
    property int        PressClass;
    property bool       Neutral [int] {
        bool get(int gas)           { return m_Neutral[gas]; }
        void set(int gas, bool val) { m_Neutral[gas] = val; }
    }
    property bool       Poisonous [int] {
        bool get(int gas)           { return m_Poisonous[gas]; }
        void set(int gas, bool val) { m_Poisonous[gas] = val; }
    }

protected:
    array<bool>        ^m_Neutral;
    array<bool>        ^m_Poisonous;
};

// ---------------------------------------------------
// Alien species
public ref class Alien : public GridDataSrcBase
{
public:
    Alien(String ^name)
    {
        Name = name;
        GovName = nullptr;
        GovType = nullptr;
        Relation = SP_NEUTRAL;
        RelationDefault = SP_NEUTRAL;
        TurnMet = 0;
        Email = nullptr;
        HomeSystem = nullptr;
        HomePlanet = -1;
        AtmReq = gcnew AtmosphericReq;
        TechEstimateTurn = -1;
        Ships = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;

        TechLevels        = gcnew array<int>(TECH_MAX){0};
        TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
        TechLevelsAssumed = gcnew array<int>(TECH_MAX){0};

        LastMessageRecvTurn = 0;
        LastMessageSentTurn = 0;
    }
    // --- copy constructor - does not perform full copy !!! ---
    Alien(StarSystem^ home, Alien^ src)
    {
        Name = src->Name;
        GovName = src->GovName;
        GovType = src->GovType;
        Relation = src->Relation;
        RelationDefault = src->RelationDefault;
        TurnMet = src->TurnMet;
        Email = src->Email;
        HomeSystem = home;
        HomePlanet = src->HomePlanet;
        AtmReq = src->AtmReq;
        TechEstimateTurn = src->TechEstimateTurn;
        Ships = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;

        TechLevels        = gcnew array<int>(TECH_MAX){0};
        TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
        TechLevelsAssumed = gcnew array<int>(TECH_MAX){0};
        src->TechLevels->CopyTo(TechLevels, 0);

        LastMessageRecv = src->LastMessageRecv;
        LastMessageRecvTurn = src->LastMessageRecvTurn;
        LastMessageSentTurn = src->LastMessageSentTurn;
    }
    // -------- IGridDataSrc ----------------------------
    virtual String^     GetTooltipText() override;
    virtual Alien^      GetAlienForBgColor() override   { return this; }
    virtual SPRelType   GetFilterRelType() override     { return Relation; }
    // -------- IComparers ------------------------------
    ref class RelationComparer : public IComparer<Alien^>
    {
    public:
        virtual int Compare(Alien ^a1, Alien ^a2)
        {
            if( a1->Relation == a2->Relation )
                return String::Compare(a1->Name, a2->Name);
            return a1->Relation - a2->Relation;
        }
    };
    // --------------------------------------------------

    void            SortColoniesByProdOrder();

    String^         PrintRelation();
    String^         PrintHome();
    String^         PrintTechLevels();
    String^         PrintAtmosphere();

    Colony^         FindColony(String ^name, bool allowMissing);
    Ship^           FindShip(String ^name, bool allowMissing);

    String^             Name;
    String^             GovName;
    String^             GovType;
    SPRelType           Relation;
    SPRelType           RelationDefault;
    SPRelType           RelationParsed;
    int                 TurnMet;
    String^             Email;

    StarSystem^         HomeSystem;
    int                 HomePlanet;
    AtmosphericReq^     AtmReq;

    List<Ship^>^        Ships;
    List<Colony^>^      Colonies;

    int                 TechEstimateTurn;
    array<int>^         TechLevels;
    array<int>^         TechLevelsTeach;
    array<int>^         TechLevelsAssumed;

    String^             LastMessageRecv;
    int                 LastMessageRecvTurn;
    int                 LastMessageSentTurn;
};

} // end namespace FHUI
