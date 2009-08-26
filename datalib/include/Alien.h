#pragma once
#include "GridDataSrcBase.h"

namespace FHUI
{

ref class Ship;
ref class Colony;

// ---------------------------------------------------
// Alien species
public ref class Alien : public GridDataSrcBase
{
public:
    Alien(String ^name, int turn)
    {
        Name = name;
        GovName = nullptr;
        GovType = nullptr;
        Relation = SP_NEUTRAL;
        RelationOriginal = SP_NEUTRAL;
        TurnMet = turn;
        Email = nullptr;
        HomeSystem = nullptr;
        HomePlanet = -1;
        TechEstimateTurn = -1;
        Ships = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;

        TechLevels        = gcnew array<int>(TECH_MAX){0};
        TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
        TechLevelsAssumed = gcnew array<int>(TECH_MAX){0};
        TeachOrders = 0;
    }
    // --- copy constructor - does not perform full copy !!! ---
    Alien(StarSystem^ home, Alien^ src)
    {
        Name = src->Name;
        GovName = src->GovName;
        GovType = src->GovType;
        Relation = src->Relation;
        RelationOriginal = src->Relation;
        TurnMet = src->TurnMet;
        Email = src->Email;
        HomeSystem = home;
        HomePlanet = src->HomePlanet;
        TechEstimateTurn = src->TechEstimateTurn;
        Ships = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;

        TechLevels        = gcnew array<int>(TECH_MAX){0};
        TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
        TechLevelsAssumed = gcnew array<int>(TECH_MAX){0};
        src->TechLevels->CopyTo(TechLevels, 0);

        TeachOrders = 0;
    }
    // -------- IGridDataSrc ----------------------------
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

    property String^            Name;
    property String^            GovName;
    property String^            GovType;
    property SPRelType          Relation;
    property SPRelType          RelationOriginal;
    property int                TurnMet;
    property String^            Email;

    property StarSystem^        HomeSystem;
    property int                HomePlanet;

    property List<Ship^>^       Ships;
    property List<Colony^>^     Colonies;

    property int                TechEstimateTurn;
    property array<int>^        TechLevels;
    property array<int>^        TechLevelsTeach;
    property array<int>^        TechLevelsAssumed;
    property int                TeachOrders;
};

} // end namespace FHUI
