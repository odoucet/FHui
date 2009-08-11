#pragma once

using namespace System;
using namespace System::Windows::Forms;

namespace FHUI
{

delegate void GridSetupHandler();
delegate void GridExceptionHandler(Exception ^e);

ref class GridFilter : public IGridFilter
{
public:
    GridFilter(DataGridView ^grid, bool ^gridUpdateEnabled)
        : m_Grid(grid)
        , m_bGridUpdateEnabled(gridUpdateEnabled)
        , m_RefreshDummy(gcnew Object)
        , m_LastFiltMask(0)
    {
        GameData = nullptr;
        DefaultLSN = 99;
        DefaultMishap = 100;
        SelectRefSystemFromRefShip = true;

        RefSystem = nullptr;
    }

    virtual property GameData^      GameData;
    virtual property StarSystem^    RefSystem
    {
        void set(StarSystem^ sys)
        {
            m_RefSystem = sys;
            if( Sorter )
                Sorter->SetRefSystem(sys);
        }
        StarSystem^ get()           { return m_RefSystem; }
    }
    virtual property IGridSorter^   Sorter;
    virtual property bool           EnableUpdates
    {
        void set(bool en) { *m_bGridUpdateEnabled = en; }
        bool get()        { return *m_bGridUpdateEnabled; }
    }

    virtual property int            DefaultLSN;
    virtual property int            DefaultMishap;
    virtual property bool           SelectRefSystemFromRefShip;

    virtual property bool           MiMaBalanced { bool get() { return CtrlMiMaBalance->Checked; } }

    virtual void    Update();
    virtual void    Update(Object ^sender);
    virtual void    Reset();
    virtual bool    Filter(IGridDataSrc ^item);

    virtual void    SetRefSystem(StarSystem ^system);

    Label           ^CtrlRef;
    ComboBox        ^CtrlRefHome;
    ComboBox        ^CtrlRefXYZ;
    ComboBox        ^CtrlRefColony;
    ComboBox        ^CtrlRefShip;
    NumericUpDown   ^CtrlGV;
    NumericUpDown   ^CtrlShipAge;
    NumericUpDown   ^CtrlMaxMishap;
    NumericUpDown   ^CtrlMaxLSN;
    CheckBox        ^CtrlFiltVisV;
    CheckBox        ^CtrlFiltVisN;
    CheckBox        ^CtrlFiltColC;
    CheckBox        ^CtrlFiltColN;
    CheckBox        ^CtrlFiltOwnO;
    CheckBox        ^CtrlFiltOwnN;
    CheckBox        ^CtrlMiMaBalance;
    CheckBox        ^CtrlFiltRelA;
    CheckBox        ^CtrlFiltRelE;
    CheckBox        ^CtrlFiltRelN;
    CheckBox        ^CtrlFiltRelP;
    CheckBox        ^CtrlFiltTypeBas;
    CheckBox        ^CtrlFiltTypeMl;
    CheckBox        ^CtrlFiltTypeTr;

    event GridSetupHandler      ^GridSetup;
    event GridExceptionHandler  ^GridException;

    initonly static String  ^s_CaptionXYZ       = "[Select ref system]";
    initonly static String  ^s_CaptionHome      = "[Select ref home]";
    initonly static String  ^s_CaptionColony    = "[Select ref colony]";
    initonly static String  ^s_CaptionShip      = "[Select ref ship]";

protected:
    void            SetRefText();
    void            SetRefHome();
    void            SetRefXYZ();
    void            SetRefColony();
    void            SetRefShip();

    Ship^           GetShipFromRefList(ComboBox ^combo);

    DataGridView    ^m_Grid;
    StarSystem      ^m_RefSystem;
    bool            ^m_bGridUpdateEnabled;
    Object          ^m_RefreshDummy;

    __int64          m_LastFiltMask;
};

} // end namespace FHUI
