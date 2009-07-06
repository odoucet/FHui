#pragma once

#include "IGridData.h"

using namespace System::Windows::Forms;

ref class Ship;
ref class StarSystem;

public delegate void GridSetupHandler();
public delegate void GridExceptionHandler(Exception ^e);

public ref class GridFilter : public IGridFilter
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
        SelectRefSystemFromRefShip = false;

        RefSystem = nullptr;
    }

    virtual property GameData^      GameData;
    virtual property StarSystem^    RefSystem;

    virtual property int            DefaultLSN;
    virtual property int            DefaultMishap;
    virtual property bool           SelectRefSystemFromRefShip;

    virtual property bool           MiMaBalanced { bool get() { return CtrlMiMaBalance->Checked; } }

    virtual void    Update(Object ^sender);
    virtual void    Reset();
    virtual bool    Filter(IGridDataSrc ^item);

    Label           ^CtrlRef;
    ComboBox        ^CtrlRefHome;
    ComboBox        ^CtrlRefXYZ;
    ComboBox        ^CtrlRefColony;
    ComboBox        ^CtrlRefShip;
    NumericUpDown   ^CtrlGV;
    NumericUpDown   ^CtrlShipAge;
    NumericUpDown   ^CtrlMaxMishap;
    NumericUpDown   ^CtrlMaxLSN;
    RadioButton     ^CtrlFiltVisA;
    RadioButton     ^CtrlFiltVisV;
    RadioButton     ^CtrlFiltVisN;
    RadioButton     ^CtrlFiltColA;
    RadioButton     ^CtrlFiltColC;
    RadioButton     ^CtrlFiltColN;
    RadioButton     ^CtrlFiltOwnA;
    RadioButton     ^CtrlFiltOwnO;
    RadioButton     ^CtrlFiltOwnN;
    CheckBox        ^CtrlMiMaBalance;
    CheckBox        ^CtrlFiltRelA;
    CheckBox        ^CtrlFiltRelE;
    CheckBox        ^CtrlFiltRelN;
    CheckBox        ^CtrlFiltRelP;
    CheckBox        ^CtrlFiltTypeBas;
    CheckBox        ^CtrlFiltTypeMl;
    CheckBox        ^CtrlFiltTypeTr;

    event GridSetupHandler      ^OnGridSetup;
    event GridExceptionHandler  ^OnGridException;

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
    bool            ^m_bGridUpdateEnabled;
    Object          ^m_RefreshDummy;

    __int64          m_LastFiltMask;
};
