#pragma once

using namespace System;
using namespace System::Windows::Forms;

namespace FHUI
{

delegate void GridFillHandler();
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
        DefaultLSN = 99;
        DefaultMishap = 100;
        SelectRefSystemFromRefShip = true;

        RefSystem = nullptr;
    }

    virtual void OnGridFill();
    virtual void OnGridSelectionChanged();

    virtual property StarSystem^    RefSystem
    {
        void set(StarSystem^ sys)
        {
            if( sys != m_RefSystem )
            {
                RefSystemPrev = m_RefSystem ? m_RefSystem : sys;
                m_RefSystem = sys;
                if( Sorter )
                    Sorter->RefSystem = sys;
            }
        }
        StarSystem^ get()           { return m_RefSystem; }
    }
    virtual property StarSystem^    RefSystemPrev;
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
    virtual void    ResetControls(bool doUpdate);
    virtual bool    Filter(IGridDataSrc ^item);

    virtual void    SetRefSystem(StarSystem ^system);
    virtual void    SetRefSystem(Colony ^colony);

    Label           ^CtrlRef;
    ComboBox        ^CtrlRefHome;
    ComboBox        ^CtrlRefXYZ;
    TextBox         ^CtrlRefEdit;
    ComboBox        ^CtrlRefColony;
    ComboBox        ^CtrlRefShip;
    NumericUpDown   ^CtrlGV;
    NumericUpDown   ^CtrlShipAge;
    NumericUpDown   ^CtrlMaxMishap;
    NumericUpDown   ^CtrlMaxLSN;
    CheckBox        ^CtrlFiltScanK;
    CheckBox        ^CtrlFiltScanU;
    CheckBox        ^CtrlFiltColC;
    CheckBox        ^CtrlFiltColH;
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
    Label           ^CtrlNumRows;

    event GridFillHandler       ^GridFill;
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
    int              m_RowsCnt;
};

} // end namespace FHUI
