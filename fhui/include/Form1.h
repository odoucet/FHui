#pragma once

#include "enums.h"

namespace FHUI {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    generic <typename T>
    private delegate void EventHandler1Arg(T);

    ref class RegexMatcher;

    /// <summary>
    /// Summary for Form1
    ///
    /// WARNING: If you change the name of this class, you will need to change the
    ///          'Resource File Name' property for the managed resource compiler tool
    ///          associated with all .resx files this class depends on.  Otherwise,
    ///          the designers will not be able to interact properly with localized
    ///          resources associated with this form.
    /// </summary>
    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
        }

        void        LoadGameData();

        // ==================================================
        // -- Config properties --
        property String^    DataDir;

        // ==================================================
    private:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~Form1()
        {
            if (components)
            {
                delete components;
            }
        }

        void        ScanReports();
        int         CheckReport(String ^fileName);
        void        LoadGalaxy();
        void        LoadGameTurn(int turn);
        void        LoadReport(String ^fileName);
        void        LoadOrders();
        void        TurnReload();
        void        InitData();
        void        InitRefLists();
        void        InitControls();
        void        UpdateControls();
        void        UpdateTabs();

        void        FillAboutBox();
        void        RepModeChanged();
        void        DisplayReport();
        void        DisplayTurn();
        void        ShowException(Exception ^e);

        void        TechLevelsResetToCurrent();
        void        TechLevelsResetToTaught();
        void        TechLevelsChanged();

        String^     GetDataDir(String ^suffix);

        // -- Data grids formatting
        void        ApplyDataAndFormat(
                        DataGridView ^grid,
                        DataTable ^dataTable,
                        DataColumn ^objColumn,
                        int defaultSortColIdx );
        Color       GetAlienColor(Alien ^sp);
        void        SetGridBgAndTooltip(DataGridView ^grid);
        void        SetGridRefSystemOnMouseClick(DataGridView ^grid, int rowIndex);
        void        ShowGridContextMenu(DataGridView^ grid, DataGridViewCellMouseEventArgs ^e);

        generic<typename T>
        ToolStripMenuItem^ CreateCustomMenuItem(
            String ^text,
            T data,
            EventHandler1Arg<T> ^handler );

        // ==================================================

        ////////////////////////////////////////////////////////////////
        value struct TabIndex
        {
            initonly static int Reports     = 0;
            initonly static int Map         = 1;
            initonly static int Systems     = 2;
            initonly static int Planets     = 3;
            initonly static int Colonies    = 4;
            initonly static int Ships       = 5;
            initonly static int Aliens      = 6;
            initonly static int Orders      = 7;
            initonly static int Utils       = 8;
            initonly static int About       = 9;
        };
        ////////////////////////////////////////////////////////////////

        GameData^           m_GameData;
        RegexMatcher^       m_RM;
        SortedList<int, GameData^>     ^m_GameTurns;
        SortedList<int, String^>       ^m_Reports;
        SortedList<int, String^>       ^m_RepFiles;
        SortedList<String^, String^>   ^m_CmdFiles;

        array<String^>     ^m_RepTurnNrData;

        List<String^>      ^m_RefListSystemsXYZ;
        List<String^>      ^m_RefListHomes;
        List<String^>      ^m_RefListColonies;
        List<String^>      ^m_RefListShips;

        bool                m_HadException;
        bool               ^m_bGridUpdateEnabled;

        System::Windows::Forms::ToolTip^    m_GridToolTip;

        // ==================================================
        // --- Plugins ---
        void        LoadPlugins();
        void        UpdatePlugins();

        List<IPluginBase^>^     m_AllPlugins;
        List<IGridPlugin^>^     m_GridPlugins;
        List<IOrdersPlugin^>^   m_OrdersPlugins;

        // ==================================================
        // --- SYSTEMS ---
        void        SystemsUpdateControls();
        void        SystemsSetup();
        void        SystemsSelectPlanets( int rowIndex );
        void        SystemsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex);
        void        SystemsMenuShowPlanets(Object^, EventArgs^);
        void        SystemsMenuShowColonies(Object^, EventArgs^);
        void        SystemsMenuSelectRef(Object^, EventArgs^);

        IGridFilter        ^m_SystemsFilter;
        StarSystem         ^m_SystemsMenuRef;
        int                 m_SystemsMenuRefRow;

        // ==================================================
        // --- PLANETS ---
        void        PlanetsUpdateControls();
        void        PlanetsSetup();
        void        PlanetsSelectColonies( int rowIndex );
        void        PlanetsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex);
        void        PlanetsMenuShowColonies(Object^, EventArgs^);
        void        PlanetsMenuSelectRef(Object^, EventArgs^);
        void        PlanetsMenuAddNameStart(Object^, EventArgs^);
        void        PlanetsMenuAddName(DataGridViewCellEventArgs ^cell);
        void        PlanetsMenuRemoveName(Object^, EventArgs^);

        IGridFilter        ^m_PlanetsFilter;
        Planet             ^m_PlanetsMenuRef;
        int                 m_PlanetsMenuRefRow;

        // ==================================================
        // --- COLONIES ---
        void        ColoniesUpdateControls();
        void        ColoniesSetup();
        void        ColoniesSetRef( int rowIndex );
        void        ColoniesFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex);
        void        ColoniesMenuSelectRef(Object^, EventArgs^);
        void        ColoniesMenuProdOrderAdjust(int adjustment);

        IGridFilter        ^m_ColoniesFilter;
        Colony             ^m_ColoniesMenuRef;

        // ==================================================
        // --- SHIPS ---
        void        ShipsUpdateControls();
        void        ShipsSetup();
        void        ShipsSetRef( int rowIndex );
        void        ShipsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex);
        void        ShipsMenuSelectRef(Object^, EventArgs^);

        typedef Pair<Ship^, Ship::Order^> ShipOrderData;
        void        ShipsMenuOrderSet(ShipOrderData ^data);

        ToolStripMenuItem^ ShipsMenuAddJumpsHere(
                        StarSystem ^system, int planetNum );
        ToolStripMenuItem^ ShipsMenuCreateJumpItem(
                        Ship ^ship, StarSystem ^system, int planetNum, String ^text );

        IGridFilter        ^m_ShipsFilter;
        Ship               ^m_ShipsMenuRef;

        // ==================================================
        // --- ALIENS ---
        void        AliensSetup();
        void        AliensUpdateControls();
        void        AliensFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex);

        typedef Pair<Alien^, int> AlienRelationData;
        void        AliensMenuSetRelation(AlienRelationData ^data);
        typedef Triple<Alien^, int, int> TeachData;
        void        AliensMenuTeach(TeachData ^data);
        void        AliensMenuTeachAll(Object^, EventArgs^);
        void        AliensMenuTeachCancel(Object^, EventArgs^);

        ToolStripMenuItem^ AliensMenuCreateTeach(String ^text, TechType tech);

        IGridFilter        ^m_AliensFilter;
        Alien              ^m_AliensMenuRef;

        // ==================================================
        // --- ORDER TEMPLATE ---
        void        GenerateTemplate();
        void        SaveCommands();
        void        DeleteCommands();
        void        LoadCommands();
        void        AddCommand(ICommand ^cmd);
        void        DelCommand(ICommand ^cmd);

        void        GenerateCombat();
        void        GeneratePreDeparture();
        void        GenerateJumps();
        void        GenerateProduction();
        void        GeneratePostArrival();
        void        GenerateStrikes();

        void        GenerateCombatInfo(StarSystem^);
        void        GeneratePreDepartureInfo(StarSystem^);
        void        GenerateJumpInfo(Ship^);
        void        GenerateScanOrders();
        void        GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget);
        void        GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget);

        List<String^>      ^m_OrderList;

        // ==================================================
        // --- MAP ---
        void        MapSetup();
        void        MapDraw();
        void        MapDrawGrid(Graphics ^g);
        void        MapDrawDistances(Graphics ^g);
        void        MapDrawWormholes(Graphics ^g);
        void        MapDrawSystems(Graphics ^g);
        void        MapDrawShips(Graphics ^g);
        void        MapDrawBattles(Graphics ^g);

        void        MapDrawDistance(Graphics ^g, int sp, StarSystem ^sysFrom, StarSystem ^sysTo);

        Alien^      MapGetAlien(int sp);
        StarSystem^ MapGetRefSystem(int sp);
        int         MapGetAlienGV(int sp);
        int         MapSpNumFromPtr(Alien ^sp);
        Color       MapGetAlienColor(int sp, double dim);
        PointF      MapGetSystemXY(StarSystem ^sys);
        RectangleF  MapGetSystemRect( StarSystem ^system, float size );
        Brush^      MapGetBrushLSN(int lsn);

        Alien^      MapGetAlienFromUI(ComboBox^);


        int                 m_GalaxySize;
        float               m_MapSectorSize;

        initonly static int MapMaxSpecies = 4;

        // ==================================================
        // ==================================================
        // Auto-generated code below this point
        // --------------------------------------------------

    private: System::Windows::Forms::RichTextBox^  OrderTemplate;
    private: System::Windows::Forms::TabPage^  TabUtils;
    private: System::Windows::Forms::RadioButton^  RepModeCommands;
    private: System::Windows::Forms::RadioButton^  RepModeReports;
    private: System::Windows::Forms::ComboBox^  RepTurnNr;
    private: System::Windows::Forms::RichTextBox^  RepText;
    private: System::Windows::Forms::NumericUpDown^  TechLS;
    private: System::Windows::Forms::ComboBox^  SystemsRefColony;
    private: System::Windows::Forms::NumericUpDown^  TechBI;
    private: System::Windows::Forms::NumericUpDown^  TechMI;
    private: System::Windows::Forms::NumericUpDown^  TechMA;
    private: System::Windows::Forms::NumericUpDown^  TechML;
    private: System::Windows::Forms::NumericUpDown^  TechGV;
    private: System::Windows::Forms::Button^  TechResetTaught;
    private: System::Windows::Forms::Button^  TechResetCurrent;
    private: System::Windows::Forms::TabPage^  TabOrders;
    private: System::Windows::Forms::CheckBox^  AliensFiltRelP;
    private: System::Windows::Forms::CheckBox^  AliensFiltRelN;
    private: System::Windows::Forms::CheckBox^  AliensFiltRelE;
    private: System::Windows::Forms::CheckBox^  AliensFiltRelA;
    private: System::Windows::Forms::ComboBox^  ShipsRefHome;
    private: System::Windows::Forms::CheckBox^  ShipsFiltOwnN;
    private: System::Windows::Forms::CheckBox^  ShipsFiltOwnO;
    private: System::Windows::Forms::CheckBox^  ColoniesFiltOwnN;
    private: System::Windows::Forms::CheckBox^  ColoniesFiltOwnO;
    private: System::Windows::Forms::CheckBox^  PlanetsFiltColN;
    private: System::Windows::Forms::CheckBox^  PlanetsFiltColC;
    private: System::Windows::Forms::CheckBox^  PlanetsFiltVisN;
    private: System::Windows::Forms::CheckBox^  PlanetsFiltVisV;
    private: System::Windows::Forms::CheckBox^  SystemsFiltVisN;
    private: System::Windows::Forms::CheckBox^  SystemsFiltColN;
    private: System::Windows::Forms::CheckBox^  SystemsFiltVisV;
    private: System::Windows::Forms::CheckBox^  SystemsFiltColC;
    private: System::Windows::Forms::Button^  ShipsFiltersReset;
    private: System::Windows::Forms::TextBox^  ShipsRefText;
    private: System::Windows::Forms::NumericUpDown^  ShipsMaxMishap;



    private: System::Windows::Forms::ComboBox^  ShipsRefShip;
    private: System::Windows::Forms::ComboBox^  ShipsRefColony;
    private: System::Windows::Forms::ComboBox^  ShipsRefXYZ;
    private: System::Windows::Forms::Label^  ShipsRef;
    private: System::Windows::Forms::CheckBox^  ShipsFiltRelP;
    private: System::Windows::Forms::CheckBox^  ShipsFiltRelN;
    private: System::Windows::Forms::CheckBox^  ShipsFiltRelE;
    private: System::Windows::Forms::CheckBox^  ShipsFiltRelA;
    private: System::Windows::Forms::CheckBox^  ShipsFiltTypeML;
    private: System::Windows::Forms::CheckBox^  ShipsFiltTypeTR;
    private: System::Windows::Forms::CheckBox^  ShipsFiltTypeBA;
    private: System::Windows::Forms::Button^  AliensFiltersReset;
    private: System::Windows::Forms::Button^  ColoniesFiltersReset;
    private: System::Windows::Forms::TextBox^  ColoniesRefText;
    private: System::Windows::Forms::NumericUpDown^  ColoniesMaxLSN;
    private: System::Windows::Forms::NumericUpDown^  ColoniesMaxMishap;
    private: System::Windows::Forms::NumericUpDown^  ColoniesShipAge;

    private: System::Windows::Forms::ComboBox^  ColoniesRefShip;
    private: System::Windows::Forms::ComboBox^  ColoniesRefColony;
    private: System::Windows::Forms::ComboBox^  ColoniesRefHome;
    private: System::Windows::Forms::ComboBox^  ColoniesRefXYZ;
    private: System::Windows::Forms::Label^  ColoniesRef;



private: System::Windows::Forms::CheckBox^  ColoniesMiMaBalanced;


    private: System::Windows::Forms::Button^  SystemsFiltersReset;
    private: System::Windows::Forms::Button^  PlanetsFiltersReset;

    private: System::Windows::Forms::TextBox^  PlanetsRefEdit;
    private: System::Windows::Forms::NumericUpDown^  PlanetsMaxLSN;


    private: System::Windows::Forms::NumericUpDown^  PlanetsMaxMishap;


    private: System::Windows::Forms::NumericUpDown^  PlanetsShipAge;

    private: System::Windows::Forms::ComboBox^  PlanetsRefShip;
    private: System::Windows::Forms::ComboBox^  PlanetsRefColony;

    private: System::Windows::Forms::ComboBox^  PlanetsRefHome;

    private: System::Windows::Forms::ComboBox^  PlanetsRefXYZ;
    private: System::Windows::Forms::Label^  PlanetsRef;
    private: System::Windows::Forms::TabControl^  MenuTabs;
    private: System::Windows::Forms::TabPage^  TabReports;
    private: System::Windows::Forms::TabPage^  TabMap;
    private: System::Windows::Forms::TabPage^  TabSystems;
    private: System::Windows::Forms::TabPage^  TabPlanets;
    private: System::Windows::Forms::TabPage^  TabColonies;
    private: System::Windows::Forms::TabPage^  TabShips;
    private: System::Windows::Forms::TabPage^  TabAliens;
    private: System::Windows::Forms::ComboBox^  SystemsRefXYZ;
    private: System::Windows::Forms::ComboBox^  SystemsRefShip;
    private: System::Windows::Forms::DataGridView^  SystemsGrid;
    private: System::Windows::Forms::DataGridView^  PlanetsGrid;
    private: System::Windows::Forms::TabPage^  TabAbout;
    private: System::Windows::Forms::TextBox^  TextAbout;
    private: System::Windows::Forms::SplitContainer^  splitContainer4;
    private: System::Windows::Forms::DataGridView^  ColoniesGrid;
    private: System::Windows::Forms::SplitContainer^  splitContainer5;
    private: System::Windows::Forms::DataGridView^  ShipsGrid;
    private: System::Windows::Forms::ComboBox^  comboBox1;
    private: System::Windows::Forms::CheckBox^  checkBox1;
    private: System::Windows::Forms::TextBox^  textBox1;
    private: System::Windows::Forms::ComboBox^  comboBox2;
    private: System::Windows::Forms::DataGridView^  dataGridView1;
    private: System::Windows::Forms::SplitContainer^  splitContainer6;
    private: System::Windows::Forms::DataGridView^  AliensGrid;
private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::GroupBox^  MapSPSelf;



private: System::Windows::Forms::ComboBox^  MapSPSelfSystem;
private: System::Windows::Forms::ComboBox^  MapSP1;
private: System::Windows::Forms::ComboBox^  MapSP1System;

private: System::Windows::Forms::ComboBox^  MapSP3;
private: System::Windows::Forms::ComboBox^  MapSP3System;

private: System::Windows::Forms::ComboBox^  MapSP2;
private: System::Windows::Forms::ComboBox^  MapSP2System;
private: System::Windows::Forms::NumericUpDown^  MapSP3GV;
private: System::Windows::Forms::NumericUpDown^  MapSP2GV;
private: System::Windows::Forms::NumericUpDown^  MapSP1GV;
private: System::Windows::Forms::NumericUpDown^  MapSPSelfGV;
private: System::Windows::Forms::NumericUpDown^  MapLSNVal;




private: System::Windows::Forms::CheckBox^  MapEnJumps;


private: System::Windows::Forms::CheckBox^  MapEnLSN;

private: System::Windows::Forms::CheckBox^  MapEnDist;


private: System::Windows::Forms::TextBox^  Summary;
private: System::Windows::Forms::Button^  TurnReloadBtn;

private: System::Windows::Forms::ComboBox^  TurnSelect;





private: System::Windows::Forms::NumericUpDown^  SystemsMaxMishap;






private: System::Windows::Forms::ToolTip^  BtnTooltip;
private: System::Windows::Forms::NumericUpDown^  SystemsMaxLSN;
private: System::Windows::Forms::NumericUpDown^  SystemsShipAge;
private: System::Windows::Forms::TextBox^  SystemsRefEdit;

private: System::Windows::Forms::ComboBox^  SystemsRefHome;
private: System::Windows::Forms::Label^  SystemsRef;


    private: System::ComponentModel::IContainer^  components;

    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>


#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            this->components = (gcnew System::ComponentModel::Container());
            System::Windows::Forms::SplitContainer^  TopSplitCont;
            System::Windows::Forms::SplitContainer^  splitContainer7;
            System::Windows::Forms::Label^  label25;
            System::Windows::Forms::Label^  label26;
            System::Windows::Forms::Label^  label23;
            System::Windows::Forms::Label^  label21;
            System::Windows::Forms::Label^  label9;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::Label^  label14;
            System::Windows::Forms::GroupBox^  groupBox3;
            System::Windows::Forms::Label^  label13;
            System::Windows::Forms::GroupBox^  groupBox2;
            System::Windows::Forms::Label^  label12;
            System::Windows::Forms::GroupBox^  groupBox1;
            System::Windows::Forms::Label^  label11;
            System::Windows::Forms::Label^  label7;
            System::Windows::Forms::SplitContainer^  splitContainer2;
            System::Windows::Forms::Label^  label16;
            System::Windows::Forms::Label^  label15;
            System::Windows::Forms::Label^  label2;
            System::Windows::Forms::Label^  label17;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::SplitContainer^  splitContainer3;
            System::Windows::Forms::Label^  label1;
            System::Windows::Forms::Label^  label3;
            System::Windows::Forms::Label^  label18;
            System::Windows::Forms::Label^  label20;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::Label^  label19;
            System::Windows::Forms::Label^  label22;
            System::Windows::Forms::Label^  label24;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label10;
            System::Windows::Forms::Label^  label27;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle7 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle8 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle9 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle10 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle11 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle12 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            this->TurnReloadBtn = (gcnew System::Windows::Forms::Button());
            this->TurnSelect = (gcnew System::Windows::Forms::ComboBox());
            this->Summary = (gcnew System::Windows::Forms::TextBox());
            this->TechBI = (gcnew System::Windows::Forms::NumericUpDown());
            this->TechMI = (gcnew System::Windows::Forms::NumericUpDown());
            this->TechResetTaught = (gcnew System::Windows::Forms::Button());
            this->TechResetCurrent = (gcnew System::Windows::Forms::Button());
            this->TechMA = (gcnew System::Windows::Forms::NumericUpDown());
            this->TechML = (gcnew System::Windows::Forms::NumericUpDown());
            this->TechGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->TechLS = (gcnew System::Windows::Forms::NumericUpDown());
            this->MenuTabs = (gcnew System::Windows::Forms::TabControl());
            this->TabReports = (gcnew System::Windows::Forms::TabPage());
            this->RepText = (gcnew System::Windows::Forms::RichTextBox());
            this->RepModeCommands = (gcnew System::Windows::Forms::RadioButton());
            this->RepTurnNr = (gcnew System::Windows::Forms::ComboBox());
            this->RepModeReports = (gcnew System::Windows::Forms::RadioButton());
            this->TabMap = (gcnew System::Windows::Forms::TabPage());
            this->panel1 = (gcnew System::Windows::Forms::Panel());
            this->MapLSNVal = (gcnew System::Windows::Forms::NumericUpDown());
            this->MapEnJumps = (gcnew System::Windows::Forms::CheckBox());
            this->MapEnLSN = (gcnew System::Windows::Forms::CheckBox());
            this->MapEnDist = (gcnew System::Windows::Forms::CheckBox());
            this->MapSP3GV = (gcnew System::Windows::Forms::NumericUpDown());
            this->MapSP3 = (gcnew System::Windows::Forms::ComboBox());
            this->MapSP3System = (gcnew System::Windows::Forms::ComboBox());
            this->MapSP2GV = (gcnew System::Windows::Forms::NumericUpDown());
            this->MapSP2 = (gcnew System::Windows::Forms::ComboBox());
            this->MapSP2System = (gcnew System::Windows::Forms::ComboBox());
            this->MapSP1GV = (gcnew System::Windows::Forms::NumericUpDown());
            this->MapSP1 = (gcnew System::Windows::Forms::ComboBox());
            this->MapSP1System = (gcnew System::Windows::Forms::ComboBox());
            this->MapSPSelf = (gcnew System::Windows::Forms::GroupBox());
            this->MapSPSelfGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->MapSPSelfSystem = (gcnew System::Windows::Forms::ComboBox());
            this->TabSystems = (gcnew System::Windows::Forms::TabPage());
            this->SystemsFiltVisN = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsFiltColN = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsFiltVisV = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsFiltColC = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsFiltersReset = (gcnew System::Windows::Forms::Button());
            this->SystemsRefEdit = (gcnew System::Windows::Forms::TextBox());
            this->SystemsMaxLSN = (gcnew System::Windows::Forms::NumericUpDown());
            this->SystemsMaxMishap = (gcnew System::Windows::Forms::NumericUpDown());
            this->SystemsShipAge = (gcnew System::Windows::Forms::NumericUpDown());
            this->SystemsRefShip = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsRefColony = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsRefHome = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsRefXYZ = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsRef = (gcnew System::Windows::Forms::Label());
            this->SystemsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabPlanets = (gcnew System::Windows::Forms::TabPage());
            this->PlanetsFiltVisN = (gcnew System::Windows::Forms::CheckBox());
            this->PlanetsFiltColN = (gcnew System::Windows::Forms::CheckBox());
            this->PlanetsFiltVisV = (gcnew System::Windows::Forms::CheckBox());
            this->PlanetsFiltColC = (gcnew System::Windows::Forms::CheckBox());
            this->PlanetsFiltersReset = (gcnew System::Windows::Forms::Button());
            this->PlanetsRefEdit = (gcnew System::Windows::Forms::TextBox());
            this->PlanetsMaxLSN = (gcnew System::Windows::Forms::NumericUpDown());
            this->PlanetsMaxMishap = (gcnew System::Windows::Forms::NumericUpDown());
            this->PlanetsShipAge = (gcnew System::Windows::Forms::NumericUpDown());
            this->PlanetsRefShip = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsRefColony = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsRefHome = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsRefXYZ = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsRef = (gcnew System::Windows::Forms::Label());
            this->PlanetsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabColonies = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer4 = (gcnew System::Windows::Forms::SplitContainer());
            this->ColoniesFiltOwnN = (gcnew System::Windows::Forms::CheckBox());
            this->ColoniesFiltOwnO = (gcnew System::Windows::Forms::CheckBox());
            this->ColoniesMiMaBalanced = (gcnew System::Windows::Forms::CheckBox());
            this->ColoniesRefHome = (gcnew System::Windows::Forms::ComboBox());
            this->ColoniesFiltersReset = (gcnew System::Windows::Forms::Button());
            this->ColoniesRefText = (gcnew System::Windows::Forms::TextBox());
            this->ColoniesMaxLSN = (gcnew System::Windows::Forms::NumericUpDown());
            this->ColoniesMaxMishap = (gcnew System::Windows::Forms::NumericUpDown());
            this->ColoniesShipAge = (gcnew System::Windows::Forms::NumericUpDown());
            this->ColoniesRefShip = (gcnew System::Windows::Forms::ComboBox());
            this->ColoniesRefColony = (gcnew System::Windows::Forms::ComboBox());
            this->ColoniesRefXYZ = (gcnew System::Windows::Forms::ComboBox());
            this->ColoniesRef = (gcnew System::Windows::Forms::Label());
            this->ColoniesGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabShips = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer5 = (gcnew System::Windows::Forms::SplitContainer());
            this->ShipsRefXYZ = (gcnew System::Windows::Forms::ComboBox());
            this->ShipsFiltTypeML = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltTypeTR = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltRelP = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltTypeBA = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltRelN = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltOwnN = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltOwnO = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltRelE = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsFiltRelA = (gcnew System::Windows::Forms::CheckBox());
            this->ShipsRefHome = (gcnew System::Windows::Forms::ComboBox());
            this->ShipsFiltersReset = (gcnew System::Windows::Forms::Button());
            this->ShipsRefText = (gcnew System::Windows::Forms::TextBox());
            this->ShipsMaxMishap = (gcnew System::Windows::Forms::NumericUpDown());
            this->ShipsRefShip = (gcnew System::Windows::Forms::ComboBox());
            this->ShipsRefColony = (gcnew System::Windows::Forms::ComboBox());
            this->ShipsRef = (gcnew System::Windows::Forms::Label());
            this->ShipsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabAliens = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer6 = (gcnew System::Windows::Forms::SplitContainer());
            this->AliensFiltersReset = (gcnew System::Windows::Forms::Button());
            this->AliensFiltRelP = (gcnew System::Windows::Forms::CheckBox());
            this->AliensFiltRelN = (gcnew System::Windows::Forms::CheckBox());
            this->AliensFiltRelE = (gcnew System::Windows::Forms::CheckBox());
            this->AliensFiltRelA = (gcnew System::Windows::Forms::CheckBox());
            this->AliensGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabOrders = (gcnew System::Windows::Forms::TabPage());
            this->OrderTemplate = (gcnew System::Windows::Forms::RichTextBox());
            this->TabUtils = (gcnew System::Windows::Forms::TabPage());
            this->TabAbout = (gcnew System::Windows::Forms::TabPage());
            this->TextAbout = (gcnew System::Windows::Forms::TextBox());
            this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
            this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
            this->textBox1 = (gcnew System::Windows::Forms::TextBox());
            this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
            this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
            this->BtnTooltip = (gcnew System::Windows::Forms::ToolTip(this->components));
            TopSplitCont = (gcnew System::Windows::Forms::SplitContainer());
            splitContainer7 = (gcnew System::Windows::Forms::SplitContainer());
            label25 = (gcnew System::Windows::Forms::Label());
            label26 = (gcnew System::Windows::Forms::Label());
            label23 = (gcnew System::Windows::Forms::Label());
            label21 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label14 = (gcnew System::Windows::Forms::Label());
            groupBox3 = (gcnew System::Windows::Forms::GroupBox());
            label13 = (gcnew System::Windows::Forms::Label());
            groupBox2 = (gcnew System::Windows::Forms::GroupBox());
            label12 = (gcnew System::Windows::Forms::Label());
            groupBox1 = (gcnew System::Windows::Forms::GroupBox());
            label11 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
            label16 = (gcnew System::Windows::Forms::Label());
            label15 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label17 = (gcnew System::Windows::Forms::Label());
            splitContainer3 = (gcnew System::Windows::Forms::SplitContainer());
            label1 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label18 = (gcnew System::Windows::Forms::Label());
            label20 = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label19 = (gcnew System::Windows::Forms::Label());
            label22 = (gcnew System::Windows::Forms::Label());
            label24 = (gcnew System::Windows::Forms::Label());
            label10 = (gcnew System::Windows::Forms::Label());
            label27 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            TopSplitCont->Panel1->SuspendLayout();
            TopSplitCont->Panel2->SuspendLayout();
            TopSplitCont->SuspendLayout();
            splitContainer7->Panel1->SuspendLayout();
            splitContainer7->Panel2->SuspendLayout();
            splitContainer7->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechBI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechMI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechMA))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechML))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechGV))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechLS))->BeginInit();
            this->MenuTabs->SuspendLayout();
            this->TabReports->SuspendLayout();
            this->TabMap->SuspendLayout();
            this->panel1->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapLSNVal))->BeginInit();
            groupBox3->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP3GV))->BeginInit();
            groupBox2->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP2GV))->BeginInit();
            groupBox1->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP1GV))->BeginInit();
            this->MapSPSelf->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSPSelfGV))->BeginInit();
            this->TabSystems->SuspendLayout();
            splitContainer2->Panel1->SuspendLayout();
            splitContainer2->Panel2->SuspendLayout();
            splitContainer2->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsMaxLSN))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsMaxMishap))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsShipAge))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->BeginInit();
            this->TabPlanets->SuspendLayout();
            splitContainer3->Panel1->SuspendLayout();
            splitContainer3->Panel2->SuspendLayout();
            splitContainer3->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsMaxLSN))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsMaxMishap))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsShipAge))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsGrid))->BeginInit();
            this->TabColonies->SuspendLayout();
            this->splitContainer4->Panel1->SuspendLayout();
            this->splitContainer4->Panel2->SuspendLayout();
            this->splitContainer4->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesMaxLSN))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesMaxMishap))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesShipAge))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesGrid))->BeginInit();
            this->TabShips->SuspendLayout();
            this->splitContainer5->Panel1->SuspendLayout();
            this->splitContainer5->Panel2->SuspendLayout();
            this->splitContainer5->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsMaxMishap))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsGrid))->BeginInit();
            this->TabAliens->SuspendLayout();
            this->splitContainer6->Panel1->SuspendLayout();
            this->splitContainer6->Panel2->SuspendLayout();
            this->splitContainer6->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AliensGrid))->BeginInit();
            this->TabOrders->SuspendLayout();
            this->TabAbout->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
            this->SuspendLayout();
            // 
            // TopSplitCont
            // 
            TopSplitCont->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            TopSplitCont->Dock = System::Windows::Forms::DockStyle::Fill;
            TopSplitCont->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            TopSplitCont->Location = System::Drawing::Point(0, 0);
            TopSplitCont->Margin = System::Windows::Forms::Padding(0);
            TopSplitCont->Name = L"TopSplitCont";
            // 
            // TopSplitCont.Panel1
            // 
            TopSplitCont->Panel1->AutoScroll = true;
            TopSplitCont->Panel1->Controls->Add(splitContainer7);
            // 
            // TopSplitCont.Panel2
            // 
            TopSplitCont->Panel2->Controls->Add(this->TechBI);
            TopSplitCont->Panel2->Controls->Add(label25);
            TopSplitCont->Panel2->Controls->Add(this->TechMI);
            TopSplitCont->Panel2->Controls->Add(label26);
            TopSplitCont->Panel2->Controls->Add(this->TechResetTaught);
            TopSplitCont->Panel2->Controls->Add(this->TechResetCurrent);
            TopSplitCont->Panel2->Controls->Add(label23);
            TopSplitCont->Panel2->Controls->Add(this->TechMA);
            TopSplitCont->Panel2->Controls->Add(label21);
            TopSplitCont->Panel2->Controls->Add(this->TechML);
            TopSplitCont->Panel2->Controls->Add(label9);
            TopSplitCont->Panel2->Controls->Add(this->TechGV);
            TopSplitCont->Panel2->Controls->Add(label4);
            TopSplitCont->Panel2->Controls->Add(this->TechLS);
            TopSplitCont->Panel2->Controls->Add(label14);
            TopSplitCont->Panel2->Controls->Add(this->MenuTabs);
            TopSplitCont->Size = System::Drawing::Size(927, 593);
            TopSplitCont->SplitterDistance = 230;
            TopSplitCont->SplitterWidth = 2;
            TopSplitCont->TabIndex = 0;
            // 
            // splitContainer7
            // 
            splitContainer7->Dock = System::Windows::Forms::DockStyle::Fill;
            splitContainer7->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            splitContainer7->IsSplitterFixed = true;
            splitContainer7->Location = System::Drawing::Point(0, 0);
            splitContainer7->Margin = System::Windows::Forms::Padding(0);
            splitContainer7->Name = L"splitContainer7";
            splitContainer7->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer7.Panel1
            // 
            splitContainer7->Panel1->Controls->Add(this->TurnReloadBtn);
            splitContainer7->Panel1->Controls->Add(this->TurnSelect);
            splitContainer7->Panel1MinSize = 21;
            // 
            // splitContainer7.Panel2
            // 
            splitContainer7->Panel2->Controls->Add(this->Summary);
            splitContainer7->Size = System::Drawing::Size(226, 589);
            splitContainer7->SplitterDistance = 21;
            splitContainer7->SplitterWidth = 1;
            splitContainer7->TabIndex = 0;
            // 
            // TurnReloadBtn
            // 
            this->TurnReloadBtn->Location = System::Drawing::Point(0, -2);
            this->TurnReloadBtn->Name = L"TurnReloadBtn";
            this->TurnReloadBtn->Size = System::Drawing::Size(75, 23);
            this->TurnReloadBtn->TabIndex = 1;
            this->TurnReloadBtn->Text = L"Reload";
            this->TurnReloadBtn->UseVisualStyleBackColor = true;
            this->TurnReloadBtn->Click += gcnew System::EventHandler(this, &Form1::TurnReload_Click);
            // 
            // TurnSelect
            // 
            this->TurnSelect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
            this->TurnSelect->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->TurnSelect->FormattingEnabled = true;
            this->TurnSelect->Location = System::Drawing::Point(78, 0);
            this->TurnSelect->Name = L"TurnSelect";
            this->TurnSelect->Size = System::Drawing::Size(145, 21);
            this->TurnSelect->TabIndex = 0;
            this->TurnSelect->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::TurnSelect_SelectedIndexChanged);
            // 
            // Summary
            // 
            this->Summary->Dock = System::Windows::Forms::DockStyle::Fill;
            this->Summary->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            this->Summary->Location = System::Drawing::Point(0, 0);
            this->Summary->Multiline = true;
            this->Summary->Name = L"Summary";
            this->Summary->ReadOnly = true;
            this->Summary->Size = System::Drawing::Size(226, 567);
            this->Summary->TabIndex = 3;
            this->Summary->WordWrap = false;
            // 
            // TechBI
            // 
            this->TechBI->Location = System::Drawing::Point(468, 4);
            this->TechBI->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechBI->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechBI->Name = L"TechBI";
            this->TechBI->Size = System::Drawing::Size(42, 20);
            this->TechBI->TabIndex = 6;
            this->TechBI->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechBI, L"Set assumed BI technology level.");
            this->TechBI->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechBI->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label25
            // 
            label25->AutoSize = true;
            label25->Location = System::Drawing::Point(450, 7);
            label25->Name = L"label25";
            label25->Size = System::Drawing::Size(17, 13);
            label25->TabIndex = 7;
            label25->Text = L"BI";
            // 
            // TechMI
            // 
            this->TechMI->Location = System::Drawing::Point(139, 4);
            this->TechMI->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechMI->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechMI->Name = L"TechMI";
            this->TechMI->Size = System::Drawing::Size(42, 20);
            this->TechMI->TabIndex = 6;
            this->TechMI->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechMI, L"Set assumed MI technology level.");
            this->TechMI->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechMI->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label26
            // 
            label26->AutoSize = true;
            label26->Location = System::Drawing::Point(7, 7);
            label26->Name = L"label26";
            label26->Size = System::Drawing::Size(107, 13);
            label26->TabIndex = 7;
            label26->Text = L"Assumed tech levels:";
            // 
            // TechResetTaught
            // 
            this->TechResetTaught->Location = System::Drawing::Point(571, 3);
            this->TechResetTaught->Name = L"TechResetTaught";
            this->TechResetTaught->Size = System::Drawing::Size(50, 23);
            this->TechResetTaught->TabIndex = 12;
            this->TechResetTaught->Text = L"Taught";
            this->TechResetTaught->UseVisualStyleBackColor = true;
            this->TechResetTaught->Click += gcnew System::EventHandler(this, &Form1::TechResetTaught_Click);
            // 
            // TechResetCurrent
            // 
            this->TechResetCurrent->Location = System::Drawing::Point(522, 3);
            this->TechResetCurrent->Name = L"TechResetCurrent";
            this->TechResetCurrent->Size = System::Drawing::Size(50, 23);
            this->TechResetCurrent->TabIndex = 12;
            this->TechResetCurrent->Text = L"Current";
            this->TechResetCurrent->UseVisualStyleBackColor = true;
            this->TechResetCurrent->Click += gcnew System::EventHandler(this, &Form1::TechResetCurrent_Click);
            // 
            // label23
            // 
            label23->AutoSize = true;
            label23->Location = System::Drawing::Point(119, 7);
            label23->Name = L"label23";
            label23->Size = System::Drawing::Size(19, 13);
            label23->TabIndex = 7;
            label23->Text = L"MI";
            // 
            // TechMA
            // 
            this->TechMA->Location = System::Drawing::Point(207, 4);
            this->TechMA->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechMA->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechMA->Name = L"TechMA";
            this->TechMA->Size = System::Drawing::Size(42, 20);
            this->TechMA->TabIndex = 6;
            this->TechMA->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechMA, L"Set assumed MA technology level.");
            this->TechMA->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechMA->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label21
            // 
            label21->AutoSize = true;
            label21->Location = System::Drawing::Point(183, 7);
            label21->Name = L"label21";
            label21->Size = System::Drawing::Size(23, 13);
            label21->TabIndex = 7;
            label21->Text = L"MA";
            // 
            // TechML
            // 
            this->TechML->Location = System::Drawing::Point(274, 4);
            this->TechML->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechML->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechML->Name = L"TechML";
            this->TechML->Size = System::Drawing::Size(42, 20);
            this->TechML->TabIndex = 6;
            this->TechML->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechML, L"Set assumed ML technology level.");
            this->TechML->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechML->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label9
            // 
            label9->AutoSize = true;
            label9->Location = System::Drawing::Point(251, 7);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(22, 13);
            label9->TabIndex = 7;
            label9->Text = L"ML";
            // 
            // TechGV
            // 
            this->TechGV->Location = System::Drawing::Point(341, 4);
            this->TechGV->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechGV->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechGV->Name = L"TechGV";
            this->TechGV->Size = System::Drawing::Size(42, 20);
            this->TechGV->TabIndex = 6;
            this->TechGV->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechGV, L"Set assumed GV technology level.");
            this->TechGV->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechGV->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(318, 7);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(22, 13);
            label4->TabIndex = 7;
            label4->Text = L"GV";
            // 
            // TechLS
            // 
            this->TechLS->Location = System::Drawing::Point(406, 4);
            this->TechLS->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->TechLS->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechLS->Name = L"TechLS";
            this->TechLS->Size = System::Drawing::Size(42, 20);
            this->TechLS->TabIndex = 6;
            this->TechLS->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->BtnTooltip->SetToolTip(this->TechLS, L"Set assumed LS technology level.");
            this->TechLS->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->TechLS->ValueChanged += gcnew System::EventHandler(this, &Form1::Tech_ValueChanged);
            // 
            // label14
            // 
            label14->AutoSize = true;
            label14->Location = System::Drawing::Point(385, 7);
            label14->Name = L"label14";
            label14->Size = System::Drawing::Size(20, 13);
            label14->TabIndex = 7;
            label14->Text = L"LS";
            // 
            // MenuTabs
            // 
            this->MenuTabs->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->MenuTabs->Controls->Add(this->TabReports);
            this->MenuTabs->Controls->Add(this->TabMap);
            this->MenuTabs->Controls->Add(this->TabSystems);
            this->MenuTabs->Controls->Add(this->TabPlanets);
            this->MenuTabs->Controls->Add(this->TabColonies);
            this->MenuTabs->Controls->Add(this->TabShips);
            this->MenuTabs->Controls->Add(this->TabAliens);
            this->MenuTabs->Controls->Add(this->TabOrders);
            this->MenuTabs->Controls->Add(this->TabUtils);
            this->MenuTabs->Controls->Add(this->TabAbout);
            this->MenuTabs->Location = System::Drawing::Point(0, 30);
            this->MenuTabs->Name = L"MenuTabs";
            this->MenuTabs->SelectedIndex = 0;
            this->MenuTabs->Size = System::Drawing::Size(691, 561);
            this->MenuTabs->TabIndex = 0;
            this->MenuTabs->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::MenuTabs_SelectedIndexChanged);
            // 
            // TabReports
            // 
            this->TabReports->BackColor = System::Drawing::SystemColors::Control;
            this->TabReports->Controls->Add(this->RepText);
            this->TabReports->Controls->Add(this->RepModeCommands);
            this->TabReports->Controls->Add(this->RepTurnNr);
            this->TabReports->Controls->Add(this->RepModeReports);
            this->TabReports->Location = System::Drawing::Point(4, 22);
            this->TabReports->Margin = System::Windows::Forms::Padding(0);
            this->TabReports->Name = L"TabReports";
            this->TabReports->Padding = System::Windows::Forms::Padding(3);
            this->TabReports->Size = System::Drawing::Size(683, 535);
            this->TabReports->TabIndex = 0;
            this->TabReports->Text = L"Reports";
            // 
            // RepText
            // 
            this->RepText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->RepText->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(0)));
            this->RepText->Location = System::Drawing::Point(0, 30);
            this->RepText->Name = L"RepText";
            this->RepText->ReadOnly = true;
            this->RepText->Size = System::Drawing::Size(677, 507);
            this->RepText->TabIndex = 0;
            this->RepText->Text = L"";
            this->RepText->WordWrap = false;
            // 
            // RepModeCommands
            // 
            this->RepModeCommands->Appearance = System::Windows::Forms::Appearance::Button;
            this->RepModeCommands->Location = System::Drawing::Point(92, 3);
            this->RepModeCommands->Name = L"RepModeCommands";
            this->RepModeCommands->Size = System::Drawing::Size(80, 23);
            this->RepModeCommands->TabIndex = 2;
            this->RepModeCommands->TabStop = true;
            this->RepModeCommands->Text = L"Commands";
            this->RepModeCommands->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->RepModeCommands->UseVisualStyleBackColor = true;
            // 
            // RepTurnNr
            // 
            this->RepTurnNr->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->RepTurnNr->FormattingEnabled = true;
            this->RepTurnNr->Location = System::Drawing::Point(178, 4);
            this->RepTurnNr->MaxDropDownItems = 20;
            this->RepTurnNr->Name = L"RepTurnNr";
            this->RepTurnNr->Size = System::Drawing::Size(129, 21);
            this->RepTurnNr->TabIndex = 0;
            this->RepTurnNr->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::RepTurnNr_SelectedIndexChanged);
            // 
            // RepModeReports
            // 
            this->RepModeReports->Appearance = System::Windows::Forms::Appearance::Button;
            this->RepModeReports->Location = System::Drawing::Point(6, 3);
            this->RepModeReports->Name = L"RepModeReports";
            this->RepModeReports->Size = System::Drawing::Size(80, 23);
            this->RepModeReports->TabIndex = 2;
            this->RepModeReports->TabStop = true;
            this->RepModeReports->Text = L"Reports";
            this->RepModeReports->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->RepModeReports->UseVisualStyleBackColor = true;
            this->RepModeReports->CheckedChanged += gcnew System::EventHandler(this, &Form1::RepMode_CheckedChanged);
            // 
            // TabMap
            // 
            this->TabMap->BackColor = System::Drawing::Color::Black;
            this->TabMap->Controls->Add(this->panel1);
            this->TabMap->Location = System::Drawing::Point(4, 22);
            this->TabMap->Margin = System::Windows::Forms::Padding(0);
            this->TabMap->Name = L"TabMap";
            this->TabMap->Size = System::Drawing::Size(683, 535);
            this->TabMap->TabIndex = 1;
            this->TabMap->Text = L"Map";
            this->TabMap->UseVisualStyleBackColor = true;
            this->TabMap->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::MapCanvas_Paint);
            // 
            // panel1
            // 
            this->panel1->BackColor = System::Drawing::SystemColors::Control;
            this->panel1->Controls->Add(this->MapLSNVal);
            this->panel1->Controls->Add(this->MapEnJumps);
            this->panel1->Controls->Add(this->MapEnLSN);
            this->panel1->Controls->Add(this->MapEnDist);
            this->panel1->Controls->Add(groupBox3);
            this->panel1->Controls->Add(groupBox2);
            this->panel1->Controls->Add(groupBox1);
            this->panel1->Controls->Add(this->MapSPSelf);
            this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
            this->panel1->Location = System::Drawing::Point(483, 0);
            this->panel1->Name = L"panel1";
            this->panel1->Size = System::Drawing::Size(200, 535);
            this->panel1->TabIndex = 0;
            // 
            // MapLSNVal
            // 
            this->MapLSNVal->Location = System::Drawing::Point(108, 324);
            this->MapLSNVal->Name = L"MapLSNVal";
            this->MapLSNVal->Size = System::Drawing::Size(52, 20);
            this->MapLSNVal->TabIndex = 2;
            this->MapLSNVal->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->MapLSNVal->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {21, 0, 0, 0});
            this->MapLSNVal->ValueChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // MapEnJumps
            // 
            this->MapEnJumps->AutoSize = true;
            this->MapEnJumps->Enabled = false;
            this->MapEnJumps->Location = System::Drawing::Point(9, 413);
            this->MapEnJumps->Name = L"MapEnJumps";
            this->MapEnJumps->Size = System::Drawing::Size(110, 17);
            this->MapEnJumps->TabIndex = 1;
            this->MapEnJumps->Text = L"Show jump orders";
            this->MapEnJumps->UseVisualStyleBackColor = true;
            this->MapEnJumps->CheckedChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // MapEnLSN
            // 
            this->MapEnLSN->AutoSize = true;
            this->MapEnLSN->Checked = true;
            this->MapEnLSN->CheckState = System::Windows::Forms::CheckState::Checked;
            this->MapEnLSN->Location = System::Drawing::Point(9, 324);
            this->MapEnLSN->Name = L"MapEnLSN";
            this->MapEnLSN->Size = System::Drawing::Size(95, 17);
            this->MapEnLSN->TabIndex = 1;
            this->MapEnLSN->Text = L"Max LSN filter:";
            this->MapEnLSN->UseVisualStyleBackColor = true;
            this->MapEnLSN->CheckedChanged += gcnew System::EventHandler(this, &Form1::MapEnLSN_CheckedChanged);
            // 
            // MapEnDist
            // 
            this->MapEnDist->AutoSize = true;
            this->MapEnDist->Checked = true;
            this->MapEnDist->CheckState = System::Windows::Forms::CheckState::Checked;
            this->MapEnDist->Location = System::Drawing::Point(9, 345);
            this->MapEnDist->Name = L"MapEnDist";
            this->MapEnDist->Size = System::Drawing::Size(73, 17);
            this->MapEnDist->TabIndex = 1;
            this->MapEnDist->Text = L"Distances";
            this->MapEnDist->UseVisualStyleBackColor = true;
            this->MapEnDist->CheckedChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // groupBox3
            // 
            groupBox3->Controls->Add(this->MapSP3GV);
            groupBox3->Controls->Add(label13);
            groupBox3->Controls->Add(this->MapSP3);
            groupBox3->Controls->Add(this->MapSP3System);
            groupBox3->ForeColor = System::Drawing::Color::DarkBlue;
            groupBox3->Location = System::Drawing::Point(3, 250);
            groupBox3->Name = L"groupBox3";
            groupBox3->Size = System::Drawing::Size(194, 68);
            groupBox3->TabIndex = 0;
            groupBox3->TabStop = false;
            groupBox3->Text = L"Alien #1";
            // 
            // MapSP3GV
            // 
            this->MapSP3GV->Enabled = false;
            this->MapSP3GV->ForeColor = System::Drawing::Color::DarkBlue;
            this->MapSP3GV->Location = System::Drawing::Point(35, 42);
            this->MapSP3GV->Name = L"MapSP3GV";
            this->MapSP3GV->Size = System::Drawing::Size(46, 20);
            this->MapSP3GV->TabIndex = 3;
            this->MapSP3GV->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->MapSP3GV->ValueChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // label13
            // 
            label13->AutoSize = true;
            label13->ForeColor = System::Drawing::Color::DarkBlue;
            label13->Location = System::Drawing::Point(6, 46);
            label13->Name = L"label13";
            label13->Size = System::Drawing::Size(25, 13);
            label13->TabIndex = 2;
            label13->Text = L"GV:";
            // 
            // MapSP3
            // 
            this->MapSP3->ForeColor = System::Drawing::Color::DarkBlue;
            this->MapSP3->FormattingEnabled = true;
            this->MapSP3->Location = System::Drawing::Point(6, 19);
            this->MapSP3->Name = L"MapSP3";
            this->MapSP3->Size = System::Drawing::Size(97, 21);
            this->MapSP3->TabIndex = 1;
            // 
            // MapSP3System
            // 
            this->MapSP3System->Enabled = false;
            this->MapSP3System->ForeColor = System::Drawing::Color::DarkBlue;
            this->MapSP3System->FormattingEnabled = true;
            this->MapSP3System->Location = System::Drawing::Point(109, 18);
            this->MapSP3System->Name = L"MapSP3System";
            this->MapSP3System->Size = System::Drawing::Size(79, 21);
            this->MapSP3System->TabIndex = 1;
            // 
            // groupBox2
            // 
            groupBox2->Controls->Add(this->MapSP2GV);
            groupBox2->Controls->Add(label12);
            groupBox2->Controls->Add(this->MapSP2);
            groupBox2->Controls->Add(this->MapSP2System);
            groupBox2->ForeColor = System::Drawing::Color::DarkGreen;
            groupBox2->Location = System::Drawing::Point(3, 176);
            groupBox2->Name = L"groupBox2";
            groupBox2->Size = System::Drawing::Size(194, 68);
            groupBox2->TabIndex = 0;
            groupBox2->TabStop = false;
            groupBox2->Text = L"Alien #1";
            // 
            // MapSP2GV
            // 
            this->MapSP2GV->Enabled = false;
            this->MapSP2GV->ForeColor = System::Drawing::Color::DarkGreen;
            this->MapSP2GV->Location = System::Drawing::Point(35, 42);
            this->MapSP2GV->Name = L"MapSP2GV";
            this->MapSP2GV->Size = System::Drawing::Size(46, 20);
            this->MapSP2GV->TabIndex = 3;
            this->MapSP2GV->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->MapSP2GV->ValueChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // label12
            // 
            label12->AutoSize = true;
            label12->ForeColor = System::Drawing::Color::DarkGreen;
            label12->Location = System::Drawing::Point(6, 46);
            label12->Name = L"label12";
            label12->Size = System::Drawing::Size(25, 13);
            label12->TabIndex = 2;
            label12->Text = L"GV:";
            // 
            // MapSP2
            // 
            this->MapSP2->ForeColor = System::Drawing::Color::DarkGreen;
            this->MapSP2->FormattingEnabled = true;
            this->MapSP2->Location = System::Drawing::Point(6, 19);
            this->MapSP2->Name = L"MapSP2";
            this->MapSP2->Size = System::Drawing::Size(97, 21);
            this->MapSP2->TabIndex = 1;
            // 
            // MapSP2System
            // 
            this->MapSP2System->Enabled = false;
            this->MapSP2System->ForeColor = System::Drawing::Color::DarkGreen;
            this->MapSP2System->FormattingEnabled = true;
            this->MapSP2System->Location = System::Drawing::Point(109, 18);
            this->MapSP2System->Name = L"MapSP2System";
            this->MapSP2System->Size = System::Drawing::Size(79, 21);
            this->MapSP2System->TabIndex = 1;
            // 
            // groupBox1
            // 
            groupBox1->Controls->Add(this->MapSP1GV);
            groupBox1->Controls->Add(label11);
            groupBox1->Controls->Add(this->MapSP1);
            groupBox1->Controls->Add(this->MapSP1System);
            groupBox1->ForeColor = System::Drawing::Color::Maroon;
            groupBox1->Location = System::Drawing::Point(3, 102);
            groupBox1->Name = L"groupBox1";
            groupBox1->Size = System::Drawing::Size(194, 68);
            groupBox1->TabIndex = 0;
            groupBox1->TabStop = false;
            groupBox1->Text = L"Alien #1";
            // 
            // MapSP1GV
            // 
            this->MapSP1GV->Enabled = false;
            this->MapSP1GV->ForeColor = System::Drawing::Color::Maroon;
            this->MapSP1GV->Location = System::Drawing::Point(35, 42);
            this->MapSP1GV->Name = L"MapSP1GV";
            this->MapSP1GV->Size = System::Drawing::Size(46, 20);
            this->MapSP1GV->TabIndex = 3;
            this->MapSP1GV->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->MapSP1GV->ValueChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // label11
            // 
            label11->AutoSize = true;
            label11->ForeColor = System::Drawing::Color::Maroon;
            label11->Location = System::Drawing::Point(6, 46);
            label11->Name = L"label11";
            label11->Size = System::Drawing::Size(25, 13);
            label11->TabIndex = 2;
            label11->Text = L"GV:";
            // 
            // MapSP1
            // 
            this->MapSP1->ForeColor = System::Drawing::Color::Maroon;
            this->MapSP1->FormattingEnabled = true;
            this->MapSP1->Location = System::Drawing::Point(6, 18);
            this->MapSP1->Name = L"MapSP1";
            this->MapSP1->Size = System::Drawing::Size(97, 21);
            this->MapSP1->TabIndex = 1;
            // 
            // MapSP1System
            // 
            this->MapSP1System->Enabled = false;
            this->MapSP1System->ForeColor = System::Drawing::Color::Maroon;
            this->MapSP1System->FormattingEnabled = true;
            this->MapSP1System->Location = System::Drawing::Point(109, 18);
            this->MapSP1System->Name = L"MapSP1System";
            this->MapSP1System->Size = System::Drawing::Size(79, 21);
            this->MapSP1System->TabIndex = 1;
            // 
            // MapSPSelf
            // 
            this->MapSPSelf->Controls->Add(this->MapSPSelfGV);
            this->MapSPSelf->Controls->Add(label7);
            this->MapSPSelf->Controls->Add(this->MapSPSelfSystem);
            this->MapSPSelf->Location = System::Drawing::Point(3, 28);
            this->MapSPSelf->Name = L"MapSPSelf";
            this->MapSPSelf->Size = System::Drawing::Size(194, 68);
            this->MapSPSelf->TabIndex = 0;
            this->MapSPSelf->TabStop = false;
            this->MapSPSelf->Text = L"groupBox1";
            // 
            // MapSPSelfGV
            // 
            this->MapSPSelfGV->Location = System::Drawing::Point(36, 42);
            this->MapSPSelfGV->Name = L"MapSPSelfGV";
            this->MapSPSelfGV->Size = System::Drawing::Size(46, 20);
            this->MapSPSelfGV->TabIndex = 3;
            this->MapSPSelfGV->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
            this->MapSPSelfGV->ValueChanged += gcnew System::EventHandler(this, &Form1::MapUpdate);
            // 
            // label7
            // 
            label7->AutoSize = true;
            label7->Location = System::Drawing::Point(6, 46);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(25, 13);
            label7->TabIndex = 2;
            label7->Text = L"GV:";
            // 
            // MapSPSelfSystem
            // 
            this->MapSPSelfSystem->FormattingEnabled = true;
            this->MapSPSelfSystem->Location = System::Drawing::Point(6, 18);
            this->MapSPSelfSystem->Name = L"MapSPSelfSystem";
            this->MapSPSelfSystem->Size = System::Drawing::Size(182, 21);
            this->MapSPSelfSystem->TabIndex = 1;
            // 
            // TabSystems
            // 
            this->TabSystems->BackColor = System::Drawing::Color::Transparent;
            this->TabSystems->Controls->Add(splitContainer2);
            this->TabSystems->Location = System::Drawing::Point(4, 22);
            this->TabSystems->Name = L"TabSystems";
            this->TabSystems->Size = System::Drawing::Size(683, 535);
            this->TabSystems->TabIndex = 2;
            this->TabSystems->Text = L"Systems";
            this->TabSystems->UseVisualStyleBackColor = true;
            // 
            // splitContainer2
            // 
            splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
            splitContainer2->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            splitContainer2->IsSplitterFixed = true;
            splitContainer2->Location = System::Drawing::Point(0, 0);
            splitContainer2->Name = L"splitContainer2";
            splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            splitContainer2->Panel1->BackColor = System::Drawing::SystemColors::Control;
            splitContainer2->Panel1->Controls->Add(this->SystemsFiltVisN);
            splitContainer2->Panel1->Controls->Add(this->SystemsFiltColN);
            splitContainer2->Panel1->Controls->Add(this->SystemsFiltVisV);
            splitContainer2->Panel1->Controls->Add(this->SystemsFiltColC);
            splitContainer2->Panel1->Controls->Add(this->SystemsFiltersReset);
            splitContainer2->Panel1->Controls->Add(this->SystemsRefEdit);
            splitContainer2->Panel1->Controls->Add(this->SystemsMaxLSN);
            splitContainer2->Panel1->Controls->Add(this->SystemsMaxMishap);
            splitContainer2->Panel1->Controls->Add(label16);
            splitContainer2->Panel1->Controls->Add(label15);
            splitContainer2->Panel1->Controls->Add(this->SystemsShipAge);
            splitContainer2->Panel1->Controls->Add(this->SystemsRefShip);
            splitContainer2->Panel1->Controls->Add(label2);
            splitContainer2->Panel1->Controls->Add(this->SystemsRefColony);
            splitContainer2->Panel1->Controls->Add(this->SystemsRefHome);
            splitContainer2->Panel1->Controls->Add(this->SystemsRefXYZ);
            splitContainer2->Panel1->Controls->Add(this->SystemsRef);
            splitContainer2->Panel1->Controls->Add(label17);
            // 
            // splitContainer2.Panel2
            // 
            splitContainer2->Panel2->Controls->Add(this->SystemsGrid);
            splitContainer2->Size = System::Drawing::Size(683, 535);
            splitContainer2->SplitterDistance = 82;
            splitContainer2->SplitterWidth = 1;
            splitContainer2->TabIndex = 0;
            // 
            // SystemsFiltVisN
            // 
            this->SystemsFiltVisN->Appearance = System::Windows::Forms::Appearance::Button;
            this->SystemsFiltVisN->Location = System::Drawing::Point(296, 51);
            this->SystemsFiltVisN->Margin = System::Windows::Forms::Padding(1);
            this->SystemsFiltVisN->Name = L"SystemsFiltVisN";
            this->SystemsFiltVisN->Size = System::Drawing::Size(23, 23);
            this->SystemsFiltVisN->TabIndex = 75;
            this->SystemsFiltVisN->Text = L"N";
            this->SystemsFiltVisN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->SystemsFiltVisN, L"Show NOT VISITED systems.");
            this->SystemsFiltVisN->UseVisualStyleBackColor = true;
            this->SystemsFiltVisN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsFiltColN
            // 
            this->SystemsFiltColN->Appearance = System::Windows::Forms::Appearance::Button;
            this->SystemsFiltColN->Location = System::Drawing::Point(368, 51);
            this->SystemsFiltColN->Margin = System::Windows::Forms::Padding(1);
            this->SystemsFiltColN->Name = L"SystemsFiltColN";
            this->SystemsFiltColN->Size = System::Drawing::Size(23, 23);
            this->SystemsFiltColN->TabIndex = 74;
            this->SystemsFiltColN->Text = L"N";
            this->SystemsFiltColN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->SystemsFiltColN, L"Show NOT COLONIZED systems.");
            this->SystemsFiltColN->UseVisualStyleBackColor = true;
            this->SystemsFiltColN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsFiltVisV
            // 
            this->SystemsFiltVisV->Appearance = System::Windows::Forms::Appearance::Button;
            this->SystemsFiltVisV->Location = System::Drawing::Point(271, 51);
            this->SystemsFiltVisV->Margin = System::Windows::Forms::Padding(1);
            this->SystemsFiltVisV->Name = L"SystemsFiltVisV";
            this->SystemsFiltVisV->Size = System::Drawing::Size(23, 23);
            this->SystemsFiltVisV->TabIndex = 77;
            this->SystemsFiltVisV->Text = L"V";
            this->SystemsFiltVisV->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->SystemsFiltVisV, L"Show VISITED systems.");
            this->SystemsFiltVisV->UseVisualStyleBackColor = true;
            this->SystemsFiltVisV->CheckedChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsFiltColC
            // 
            this->SystemsFiltColC->Appearance = System::Windows::Forms::Appearance::Button;
            this->SystemsFiltColC->Location = System::Drawing::Point(343, 51);
            this->SystemsFiltColC->Margin = System::Windows::Forms::Padding(1);
            this->SystemsFiltColC->Name = L"SystemsFiltColC";
            this->SystemsFiltColC->Size = System::Drawing::Size(23, 23);
            this->SystemsFiltColC->TabIndex = 76;
            this->SystemsFiltColC->Text = L"C";
            this->SystemsFiltColC->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->SystemsFiltColC, L"Show COLONIZED systems.");
            this->SystemsFiltColC->UseVisualStyleBackColor = true;
            this->SystemsFiltColC->CheckedChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsFiltersReset
            // 
            this->SystemsFiltersReset->Location = System::Drawing::Point(558, 51);
            this->SystemsFiltersReset->Name = L"SystemsFiltersReset";
            this->SystemsFiltersReset->Size = System::Drawing::Size(59, 23);
            this->SystemsFiltersReset->TabIndex = 12;
            this->SystemsFiltersReset->Text = L"<- Reset";
            this->BtnTooltip->SetToolTip(this->SystemsFiltersReset, L"Reset filters to default values.");
            this->SystemsFiltersReset->UseVisualStyleBackColor = true;
            this->SystemsFiltersReset->Click += gcnew System::EventHandler(this, &Form1::SystemsFiltersReset_Click);
            // 
            // SystemsRefEdit
            // 
            this->SystemsRefEdit->Location = System::Drawing::Point(396, 4);
            this->SystemsRefEdit->MaxLength = 12;
            this->SystemsRefEdit->Name = L"SystemsRefEdit";
            this->SystemsRefEdit->Size = System::Drawing::Size(82, 20);
            this->SystemsRefEdit->TabIndex = 4;
            this->BtnTooltip->SetToolTip(this->SystemsRefEdit, L"Enter reference system coordinates in form: X Y Z");
            this->SystemsRefEdit->WordWrap = false;
            this->SystemsRefEdit->TextChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsMaxLSN
            // 
            this->SystemsMaxLSN->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
            this->SystemsMaxLSN->Location = System::Drawing::Point(193, 53);
            this->SystemsMaxLSN->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->SystemsMaxLSN->Name = L"SystemsMaxLSN";
            this->SystemsMaxLSN->Size = System::Drawing::Size(48, 20);
            this->SystemsMaxLSN->TabIndex = 9;
            this->BtnTooltip->SetToolTip(this->SystemsMaxLSN, L"Maximum LSN filter.");
            this->SystemsMaxLSN->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->SystemsMaxLSN->ValueChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsMaxMishap
            // 
            this->SystemsMaxMishap->Location = System::Drawing::Point(80, 53);
            this->SystemsMaxMishap->Name = L"SystemsMaxMishap";
            this->SystemsMaxMishap->Size = System::Drawing::Size(52, 20);
            this->SystemsMaxMishap->TabIndex = 8;
            this->BtnTooltip->SetToolTip(this->SystemsMaxMishap, L"Maximum acceptable mishap chance filter.");
            this->SystemsMaxMishap->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
            this->SystemsMaxMishap->ValueChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // label16
            // 
            label16->AutoSize = true;
            label16->Location = System::Drawing::Point(138, 56);
            label16->Name = L"label16";
            label16->Size = System::Drawing::Size(54, 13);
            label16->TabIndex = 9;
            label16->Text = L"Max LSN:";
            // 
            // label15
            // 
            label15->AutoSize = true;
            label15->Location = System::Drawing::Point(4, 56);
            label15->Name = L"label15";
            label15->Size = System::Drawing::Size(75, 13);
            label15->TabIndex = 9;
            label15->Text = L"Max Mishap%:";
            // 
            // SystemsShipAge
            // 
            this->SystemsShipAge->Location = System::Drawing::Point(570, 4);
            this->SystemsShipAge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
            this->SystemsShipAge->Name = L"SystemsShipAge";
            this->SystemsShipAge->Size = System::Drawing::Size(47, 20);
            this->SystemsShipAge->TabIndex = 2;
            this->BtnTooltip->SetToolTip(this->SystemsShipAge, L"Select ship age for mishap calculation.");
            this->SystemsShipAge->ValueChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsRefShip
            // 
            this->SystemsRefShip->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->SystemsRefShip->FormattingEnabled = true;
            this->SystemsRefShip->Location = System::Drawing::Point(482, 26);
            this->SystemsRefShip->Name = L"SystemsRefShip";
            this->SystemsRefShip->Size = System::Drawing::Size(135, 21);
            this->SystemsRefShip->TabIndex = 3;
            this->BtnTooltip->SetToolTip(this->SystemsRefShip, L"Select ship reference for mishap calculation.");
            this->SystemsRefShip->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(515, 7);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(53, 13);
            label2->TabIndex = 5;
            label2->Text = L"Ship Age:";
            // 
            // SystemsRefColony
            // 
            this->SystemsRefColony->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->SystemsRefColony->FormattingEnabled = true;
            this->SystemsRefColony->Location = System::Drawing::Point(308, 26);
            this->SystemsRefColony->Name = L"SystemsRefColony";
            this->SystemsRefColony->Size = System::Drawing::Size(170, 21);
            this->SystemsRefColony->TabIndex = 7;
            this->BtnTooltip->SetToolTip(this->SystemsRefColony, L"Select reference colony for distance and mishap calculation.");
            this->SystemsRefColony->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsRefHome
            // 
            this->SystemsRefHome->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->SystemsRefHome->FormattingEnabled = true;
            this->SystemsRefHome->Location = System::Drawing::Point(134, 26);
            this->SystemsRefHome->Name = L"SystemsRefHome";
            this->SystemsRefHome->Size = System::Drawing::Size(170, 21);
            this->SystemsRefHome->TabIndex = 6;
            this->BtnTooltip->SetToolTip(this->SystemsRefHome, L"Select reference home system for distance and mishap calculation.");
            this->SystemsRefHome->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsRefXYZ
            // 
            this->SystemsRefXYZ->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->SystemsRefXYZ->FormattingEnabled = true;
            this->SystemsRefXYZ->Location = System::Drawing::Point(4, 26);
            this->SystemsRefXYZ->Name = L"SystemsRefXYZ";
            this->SystemsRefXYZ->Size = System::Drawing::Size(126, 21);
            this->SystemsRefXYZ->TabIndex = 5;
            this->BtnTooltip->SetToolTip(this->SystemsRefXYZ, L"Select reference system for distance and mishap calculation.");
            this->SystemsRefXYZ->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Systems_Update);
            // 
            // SystemsRef
            // 
            this->SystemsRef->BackColor = System::Drawing::Color::AliceBlue;
            this->SystemsRef->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->SystemsRef->Location = System::Drawing::Point(4, 3);
            this->SystemsRef->Name = L"SystemsRef";
            this->SystemsRef->Size = System::Drawing::Size(300, 17);
            this->SystemsRef->TabIndex = 0;
            this->SystemsRef->Text = L"Ref. system:";
            this->BtnTooltip->SetToolTip(this->SystemsRef, L"Reference system for distance and mishap % calculation.");
            // 
            // label17
            // 
            label17->AutoSize = true;
            label17->Location = System::Drawing::Point(350, 7);
            label17->Name = L"label17";
            label17->Size = System::Drawing::Size(43, 13);
            label17->TabIndex = 0;
            label17->Text = L"Coords:";
            // 
            // SystemsGrid
            // 
            this->SystemsGrid->AllowUserToAddRows = false;
            this->SystemsGrid->AllowUserToDeleteRows = false;
            this->SystemsGrid->AllowUserToOrderColumns = true;
            this->SystemsGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->SystemsGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->SystemsGrid->DefaultCellStyle = dataGridViewCellStyle1;
            this->SystemsGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->SystemsGrid->Location = System::Drawing::Point(0, 0);
            this->SystemsGrid->Name = L"SystemsGrid";
            this->SystemsGrid->ReadOnly = true;
            dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle2->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle2->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle2->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle2->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->SystemsGrid->RowHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this->SystemsGrid->RowHeadersWidth = 4;
            this->SystemsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            this->SystemsGrid->ShowCellToolTips = false;
            this->SystemsGrid->Size = System::Drawing::Size(683, 452);
            this->SystemsGrid->TabIndex = 0;
            this->SystemsGrid->CellMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseClick);
            this->SystemsGrid->CellMouseLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseLeave);
            this->SystemsGrid->CellMouseEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseEnter);
            this->SystemsGrid->CellMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::SystemsGrid_CellMouseDoubleClick);
            this->SystemsGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::DataGrid_DataBindingComplete);
            // 
            // TabPlanets
            // 
            this->TabPlanets->BackColor = System::Drawing::Color::Transparent;
            this->TabPlanets->Controls->Add(splitContainer3);
            this->TabPlanets->Location = System::Drawing::Point(4, 22);
            this->TabPlanets->Name = L"TabPlanets";
            this->TabPlanets->Size = System::Drawing::Size(683, 535);
            this->TabPlanets->TabIndex = 3;
            this->TabPlanets->Text = L"Planets";
            this->TabPlanets->UseVisualStyleBackColor = true;
            // 
            // splitContainer3
            // 
            splitContainer3->BackColor = System::Drawing::SystemColors::Control;
            splitContainer3->Dock = System::Windows::Forms::DockStyle::Fill;
            splitContainer3->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            splitContainer3->Location = System::Drawing::Point(0, 0);
            splitContainer3->Name = L"splitContainer3";
            splitContainer3->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            splitContainer3->Panel1->Controls->Add(this->PlanetsFiltVisN);
            splitContainer3->Panel1->Controls->Add(this->PlanetsFiltColN);
            splitContainer3->Panel1->Controls->Add(this->PlanetsFiltVisV);
            splitContainer3->Panel1->Controls->Add(this->PlanetsFiltColC);
            splitContainer3->Panel1->Controls->Add(this->PlanetsFiltersReset);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRefEdit);
            splitContainer3->Panel1->Controls->Add(this->PlanetsMaxLSN);
            splitContainer3->Panel1->Controls->Add(this->PlanetsMaxMishap);
            splitContainer3->Panel1->Controls->Add(label1);
            splitContainer3->Panel1->Controls->Add(label3);
            splitContainer3->Panel1->Controls->Add(this->PlanetsShipAge);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRefShip);
            splitContainer3->Panel1->Controls->Add(label18);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRefColony);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRefHome);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRefXYZ);
            splitContainer3->Panel1->Controls->Add(this->PlanetsRef);
            splitContainer3->Panel1->Controls->Add(label20);
            // 
            // splitContainer3.Panel2
            // 
            splitContainer3->Panel2->Controls->Add(this->PlanetsGrid);
            splitContainer3->Size = System::Drawing::Size(683, 535);
            splitContainer3->SplitterDistance = 82;
            splitContainer3->SplitterWidth = 1;
            splitContainer3->TabIndex = 1;
            // 
            // PlanetsFiltVisN
            // 
            this->PlanetsFiltVisN->Appearance = System::Windows::Forms::Appearance::Button;
            this->PlanetsFiltVisN->Location = System::Drawing::Point(296, 51);
            this->PlanetsFiltVisN->Margin = System::Windows::Forms::Padding(1);
            this->PlanetsFiltVisN->Name = L"PlanetsFiltVisN";
            this->PlanetsFiltVisN->Size = System::Drawing::Size(23, 23);
            this->PlanetsFiltVisN->TabIndex = 72;
            this->PlanetsFiltVisN->Text = L"N";
            this->PlanetsFiltVisN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->PlanetsFiltVisN, L"Show planets in NOT VISITED systems.");
            this->PlanetsFiltVisN->UseVisualStyleBackColor = true;
            this->PlanetsFiltVisN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsFiltColN
            // 
            this->PlanetsFiltColN->Appearance = System::Windows::Forms::Appearance::Button;
            this->PlanetsFiltColN->Location = System::Drawing::Point(368, 51);
            this->PlanetsFiltColN->Margin = System::Windows::Forms::Padding(1);
            this->PlanetsFiltColN->Name = L"PlanetsFiltColN";
            this->PlanetsFiltColN->Size = System::Drawing::Size(23, 23);
            this->PlanetsFiltColN->TabIndex = 72;
            this->PlanetsFiltColN->Text = L"N";
            this->PlanetsFiltColN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->PlanetsFiltColN, L"Show NOT COLONIZED planets.");
            this->PlanetsFiltColN->UseVisualStyleBackColor = true;
            this->PlanetsFiltColN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsFiltVisV
            // 
            this->PlanetsFiltVisV->Appearance = System::Windows::Forms::Appearance::Button;
            this->PlanetsFiltVisV->Location = System::Drawing::Point(271, 51);
            this->PlanetsFiltVisV->Margin = System::Windows::Forms::Padding(1);
            this->PlanetsFiltVisV->Name = L"PlanetsFiltVisV";
            this->PlanetsFiltVisV->Size = System::Drawing::Size(23, 23);
            this->PlanetsFiltVisV->TabIndex = 73;
            this->PlanetsFiltVisV->Text = L"V";
            this->PlanetsFiltVisV->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->PlanetsFiltVisV, L"Show planets in VISITED systems.");
            this->PlanetsFiltVisV->UseVisualStyleBackColor = true;
            this->PlanetsFiltVisV->CheckedChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsFiltColC
            // 
            this->PlanetsFiltColC->Appearance = System::Windows::Forms::Appearance::Button;
            this->PlanetsFiltColC->Location = System::Drawing::Point(343, 51);
            this->PlanetsFiltColC->Margin = System::Windows::Forms::Padding(1);
            this->PlanetsFiltColC->Name = L"PlanetsFiltColC";
            this->PlanetsFiltColC->Size = System::Drawing::Size(23, 23);
            this->PlanetsFiltColC->TabIndex = 73;
            this->PlanetsFiltColC->Text = L"C";
            this->PlanetsFiltColC->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->PlanetsFiltColC, L"Show COLONIZED planets.");
            this->PlanetsFiltColC->UseVisualStyleBackColor = true;
            this->PlanetsFiltColC->CheckedChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsFiltersReset
            // 
            this->PlanetsFiltersReset->Location = System::Drawing::Point(558, 51);
            this->PlanetsFiltersReset->Name = L"PlanetsFiltersReset";
            this->PlanetsFiltersReset->Size = System::Drawing::Size(59, 23);
            this->PlanetsFiltersReset->TabIndex = 30;
            this->PlanetsFiltersReset->Text = L"<- Reset";
            this->BtnTooltip->SetToolTip(this->PlanetsFiltersReset, L"Reset filters to default values.");
            this->PlanetsFiltersReset->UseVisualStyleBackColor = true;
            this->PlanetsFiltersReset->Click += gcnew System::EventHandler(this, &Form1::PlanetsFiltersReset_Click);
            // 
            // PlanetsRefEdit
            // 
            this->PlanetsRefEdit->Location = System::Drawing::Point(396, 4);
            this->PlanetsRefEdit->MaxLength = 12;
            this->PlanetsRefEdit->Name = L"PlanetsRefEdit";
            this->PlanetsRefEdit->Size = System::Drawing::Size(82, 20);
            this->PlanetsRefEdit->TabIndex = 18;
            this->BtnTooltip->SetToolTip(this->PlanetsRefEdit, L"Enter reference system coordinates in form: X Y Z");
            this->PlanetsRefEdit->WordWrap = false;
            // 
            // PlanetsMaxLSN
            // 
            this->PlanetsMaxLSN->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
            this->PlanetsMaxLSN->Location = System::Drawing::Point(193, 53);
            this->PlanetsMaxLSN->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->PlanetsMaxLSN->Name = L"PlanetsMaxLSN";
            this->PlanetsMaxLSN->Size = System::Drawing::Size(48, 20);
            this->PlanetsMaxLSN->TabIndex = 27;
            this->BtnTooltip->SetToolTip(this->PlanetsMaxLSN, L"Maximum LSN filter.");
            this->PlanetsMaxLSN->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->PlanetsMaxLSN->ValueChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsMaxMishap
            // 
            this->PlanetsMaxMishap->Location = System::Drawing::Point(80, 53);
            this->PlanetsMaxMishap->Name = L"PlanetsMaxMishap";
            this->PlanetsMaxMishap->Size = System::Drawing::Size(52, 20);
            this->PlanetsMaxMishap->TabIndex = 24;
            this->BtnTooltip->SetToolTip(this->PlanetsMaxMishap, L"Maximum acceptable mishap chance filter.");
            this->PlanetsMaxMishap->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
            this->PlanetsMaxMishap->ValueChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(138, 56);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(54, 13);
            label1->TabIndex = 25;
            label1->Text = L"Max LSN:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(4, 56);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(75, 13);
            label3->TabIndex = 26;
            label3->Text = L"Max Mishap%:";
            // 
            // PlanetsShipAge
            // 
            this->PlanetsShipAge->Location = System::Drawing::Point(570, 4);
            this->PlanetsShipAge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
            this->PlanetsShipAge->Name = L"PlanetsShipAge";
            this->PlanetsShipAge->Size = System::Drawing::Size(47, 20);
            this->PlanetsShipAge->TabIndex = 16;
            this->BtnTooltip->SetToolTip(this->PlanetsShipAge, L"Select ship age for mishap calculation.");
            this->PlanetsShipAge->ValueChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsRefShip
            // 
            this->PlanetsRefShip->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->PlanetsRefShip->FormattingEnabled = true;
            this->PlanetsRefShip->Location = System::Drawing::Point(482, 26);
            this->PlanetsRefShip->Name = L"PlanetsRefShip";
            this->PlanetsRefShip->Size = System::Drawing::Size(135, 21);
            this->PlanetsRefShip->TabIndex = 17;
            this->BtnTooltip->SetToolTip(this->PlanetsRefShip, L"Select ship reference for mishap calculation.");
            this->PlanetsRefShip->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // label18
            // 
            label18->AutoSize = true;
            label18->Location = System::Drawing::Point(515, 7);
            label18->Name = L"label18";
            label18->Size = System::Drawing::Size(53, 13);
            label18->TabIndex = 20;
            label18->Text = L"Ship Age:";
            // 
            // PlanetsRefColony
            // 
            this->PlanetsRefColony->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->PlanetsRefColony->FormattingEnabled = true;
            this->PlanetsRefColony->Location = System::Drawing::Point(308, 26);
            this->PlanetsRefColony->Name = L"PlanetsRefColony";
            this->PlanetsRefColony->Size = System::Drawing::Size(170, 21);
            this->PlanetsRefColony->TabIndex = 23;
            this->BtnTooltip->SetToolTip(this->PlanetsRefColony, L"Select reference colony for distance and mishap calculation.");
            this->PlanetsRefColony->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsRefHome
            // 
            this->PlanetsRefHome->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->PlanetsRefHome->FormattingEnabled = true;
            this->PlanetsRefHome->Location = System::Drawing::Point(134, 26);
            this->PlanetsRefHome->Name = L"PlanetsRefHome";
            this->PlanetsRefHome->Size = System::Drawing::Size(170, 21);
            this->PlanetsRefHome->TabIndex = 22;
            this->BtnTooltip->SetToolTip(this->PlanetsRefHome, L"Select reference home system for distance and mishap calculation.");
            this->PlanetsRefHome->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsRefXYZ
            // 
            this->PlanetsRefXYZ->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->PlanetsRefXYZ->FormattingEnabled = true;
            this->PlanetsRefXYZ->Location = System::Drawing::Point(4, 26);
            this->PlanetsRefXYZ->Name = L"PlanetsRefXYZ";
            this->PlanetsRefXYZ->Size = System::Drawing::Size(126, 21);
            this->PlanetsRefXYZ->TabIndex = 21;
            this->BtnTooltip->SetToolTip(this->PlanetsRefXYZ, L"Select reference system for distance and mishap calculation.");
            this->PlanetsRefXYZ->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Planets_Update);
            // 
            // PlanetsRef
            // 
            this->PlanetsRef->BackColor = System::Drawing::Color::AliceBlue;
            this->PlanetsRef->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->PlanetsRef->Location = System::Drawing::Point(4, 3);
            this->PlanetsRef->Name = L"PlanetsRef";
            this->PlanetsRef->Size = System::Drawing::Size(300, 17);
            this->PlanetsRef->TabIndex = 13;
            this->PlanetsRef->Text = L"Ref. system:";
            this->BtnTooltip->SetToolTip(this->PlanetsRef, L"Reference system for distance and mishap % calculation.");
            // 
            // label20
            // 
            label20->AutoSize = true;
            label20->Location = System::Drawing::Point(350, 7);
            label20->Name = L"label20";
            label20->Size = System::Drawing::Size(43, 13);
            label20->TabIndex = 14;
            label20->Text = L"Coords:";
            // 
            // PlanetsGrid
            // 
            this->PlanetsGrid->AllowUserToAddRows = false;
            this->PlanetsGrid->AllowUserToDeleteRows = false;
            this->PlanetsGrid->AllowUserToOrderColumns = true;
            this->PlanetsGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->PlanetsGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle3->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle3->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle3->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle3->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle3->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle3->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->PlanetsGrid->DefaultCellStyle = dataGridViewCellStyle3;
            this->PlanetsGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->PlanetsGrid->Location = System::Drawing::Point(0, 0);
            this->PlanetsGrid->Name = L"PlanetsGrid";
            this->PlanetsGrid->ReadOnly = true;
            dataGridViewCellStyle4->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle4->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle4->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle4->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle4->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle4->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle4->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->PlanetsGrid->RowHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this->PlanetsGrid->RowHeadersWidth = 4;
            this->PlanetsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            this->PlanetsGrid->ShowCellToolTips = false;
            this->PlanetsGrid->Size = System::Drawing::Size(683, 452);
            this->PlanetsGrid->TabIndex = 0;
            this->PlanetsGrid->CellMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseClick);
            this->PlanetsGrid->CellMouseLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseLeave);
            this->PlanetsGrid->CellMouseEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseEnter);
            this->PlanetsGrid->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::PlanetsGrid_CellEndEdit);
            this->PlanetsGrid->CellMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::PlanetsGrid_CellMouseDoubleClick);
            this->PlanetsGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::DataGrid_DataBindingComplete);
            // 
            // TabColonies
            // 
            this->TabColonies->BackColor = System::Drawing::Color::Transparent;
            this->TabColonies->Controls->Add(this->splitContainer4);
            this->TabColonies->Location = System::Drawing::Point(4, 22);
            this->TabColonies->Name = L"TabColonies";
            this->TabColonies->Size = System::Drawing::Size(683, 535);
            this->TabColonies->TabIndex = 4;
            this->TabColonies->Text = L"Colonies";
            this->TabColonies->UseVisualStyleBackColor = true;
            // 
            // splitContainer4
            // 
            this->splitContainer4->BackColor = System::Drawing::SystemColors::Control;
            this->splitContainer4->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer4->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            this->splitContainer4->IsSplitterFixed = true;
            this->splitContainer4->Location = System::Drawing::Point(0, 0);
            this->splitContainer4->Name = L"splitContainer4";
            this->splitContainer4->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesFiltOwnN);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesFiltOwnO);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesMiMaBalanced);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRefHome);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesFiltersReset);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRefText);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesMaxLSN);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesMaxMishap);
            this->splitContainer4->Panel1->Controls->Add(label8);
            this->splitContainer4->Panel1->Controls->Add(label19);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesShipAge);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRefShip);
            this->splitContainer4->Panel1->Controls->Add(label22);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRefColony);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRefXYZ);
            this->splitContainer4->Panel1->Controls->Add(this->ColoniesRef);
            this->splitContainer4->Panel1->Controls->Add(label24);
            // 
            // splitContainer4.Panel2
            // 
            this->splitContainer4->Panel2->Controls->Add(this->ColoniesGrid);
            this->splitContainer4->Size = System::Drawing::Size(683, 535);
            this->splitContainer4->SplitterDistance = 82;
            this->splitContainer4->SplitterWidth = 1;
            this->splitContainer4->TabIndex = 2;
            // 
            // ColoniesFiltOwnN
            // 
            this->ColoniesFiltOwnN->Appearance = System::Windows::Forms::Appearance::Button;
            this->ColoniesFiltOwnN->Location = System::Drawing::Point(296, 51);
            this->ColoniesFiltOwnN->Margin = System::Windows::Forms::Padding(1);
            this->ColoniesFiltOwnN->Name = L"ColoniesFiltOwnN";
            this->ColoniesFiltOwnN->Size = System::Drawing::Size(23, 23);
            this->ColoniesFiltOwnN->TabIndex = 70;
            this->ColoniesFiltOwnN->Text = L"N";
            this->ColoniesFiltOwnN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ColoniesFiltOwnN, L"Show NOT OWNED colonies.");
            this->ColoniesFiltOwnN->UseVisualStyleBackColor = true;
            this->ColoniesFiltOwnN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesFiltOwnO
            // 
            this->ColoniesFiltOwnO->Appearance = System::Windows::Forms::Appearance::Button;
            this->ColoniesFiltOwnO->Location = System::Drawing::Point(271, 51);
            this->ColoniesFiltOwnO->Margin = System::Windows::Forms::Padding(1);
            this->ColoniesFiltOwnO->Name = L"ColoniesFiltOwnO";
            this->ColoniesFiltOwnO->Size = System::Drawing::Size(23, 23);
            this->ColoniesFiltOwnO->TabIndex = 71;
            this->ColoniesFiltOwnO->Text = L"O";
            this->ColoniesFiltOwnO->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ColoniesFiltOwnO, L"Show OWNED colonies.");
            this->ColoniesFiltOwnO->UseVisualStyleBackColor = true;
            this->ColoniesFiltOwnO->CheckedChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesMiMaBalanced
            // 
            this->ColoniesMiMaBalanced->AutoSize = true;
            this->ColoniesMiMaBalanced->Location = System::Drawing::Point(348, 55);
            this->ColoniesMiMaBalanced->Name = L"ColoniesMiMaBalanced";
            this->ColoniesMiMaBalanced->Size = System::Drawing::Size(107, 17);
            this->ColoniesMiMaBalanced->TabIndex = 50;
            this->ColoniesMiMaBalanced->Text = L"Balanced MI/MA";
            this->BtnTooltip->SetToolTip(this->ColoniesMiMaBalanced, L"If your MI and MA levels are not equal, calculate colony balance as if they were " 
                L"equal.");
            this->ColoniesMiMaBalanced->UseVisualStyleBackColor = true;
            this->ColoniesMiMaBalanced->CheckedChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesRefHome
            // 
            this->ColoniesRefHome->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ColoniesRefHome->FormattingEnabled = true;
            this->ColoniesRefHome->Location = System::Drawing::Point(134, 26);
            this->ColoniesRefHome->Name = L"ColoniesRefHome";
            this->ColoniesRefHome->Size = System::Drawing::Size(170, 21);
            this->ColoniesRefHome->TabIndex = 40;
            this->BtnTooltip->SetToolTip(this->ColoniesRefHome, L"Select reference home system for distance and mishap calculation.");
            this->ColoniesRefHome->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesFiltersReset
            // 
            this->ColoniesFiltersReset->Location = System::Drawing::Point(558, 51);
            this->ColoniesFiltersReset->Name = L"ColoniesFiltersReset";
            this->ColoniesFiltersReset->Size = System::Drawing::Size(59, 23);
            this->ColoniesFiltersReset->TabIndex = 48;
            this->ColoniesFiltersReset->Text = L"<- Reset";
            this->BtnTooltip->SetToolTip(this->ColoniesFiltersReset, L"Reset filters to default values.");
            this->ColoniesFiltersReset->UseVisualStyleBackColor = true;
            this->ColoniesFiltersReset->Click += gcnew System::EventHandler(this, &Form1::ColoniesFiltersReset_Click);
            // 
            // ColoniesRefText
            // 
            this->ColoniesRefText->Location = System::Drawing::Point(396, 4);
            this->ColoniesRefText->MaxLength = 12;
            this->ColoniesRefText->Name = L"ColoniesRefText";
            this->ColoniesRefText->Size = System::Drawing::Size(82, 20);
            this->ColoniesRefText->TabIndex = 36;
            this->BtnTooltip->SetToolTip(this->ColoniesRefText, L"Enter reference system coordinates in form: X Y Z");
            this->ColoniesRefText->WordWrap = false;
            // 
            // ColoniesMaxLSN
            // 
            this->ColoniesMaxLSN->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
            this->ColoniesMaxLSN->Location = System::Drawing::Point(193, 53);
            this->ColoniesMaxLSN->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->ColoniesMaxLSN->Name = L"ColoniesMaxLSN";
            this->ColoniesMaxLSN->Size = System::Drawing::Size(48, 20);
            this->ColoniesMaxLSN->TabIndex = 45;
            this->BtnTooltip->SetToolTip(this->ColoniesMaxLSN, L"Maximum LSN filter.");
            this->ColoniesMaxLSN->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
            this->ColoniesMaxLSN->ValueChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesMaxMishap
            // 
            this->ColoniesMaxMishap->Location = System::Drawing::Point(80, 53);
            this->ColoniesMaxMishap->Name = L"ColoniesMaxMishap";
            this->ColoniesMaxMishap->Size = System::Drawing::Size(52, 20);
            this->ColoniesMaxMishap->TabIndex = 42;
            this->BtnTooltip->SetToolTip(this->ColoniesMaxMishap, L"Maximum acceptable mishap chance filter.");
            this->ColoniesMaxMishap->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
            this->ColoniesMaxMishap->ValueChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // label8
            // 
            label8->AutoSize = true;
            label8->Location = System::Drawing::Point(138, 56);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(54, 13);
            label8->TabIndex = 43;
            label8->Text = L"Max LSN:";
            // 
            // label19
            // 
            label19->AutoSize = true;
            label19->Location = System::Drawing::Point(4, 56);
            label19->Name = L"label19";
            label19->Size = System::Drawing::Size(75, 13);
            label19->TabIndex = 44;
            label19->Text = L"Max Mishap%:";
            // 
            // ColoniesShipAge
            // 
            this->ColoniesShipAge->Location = System::Drawing::Point(570, 4);
            this->ColoniesShipAge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
            this->ColoniesShipAge->Name = L"ColoniesShipAge";
            this->ColoniesShipAge->Size = System::Drawing::Size(47, 20);
            this->ColoniesShipAge->TabIndex = 34;
            this->BtnTooltip->SetToolTip(this->ColoniesShipAge, L"Select ship age for mishap calculation.");
            this->ColoniesShipAge->ValueChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesRefShip
            // 
            this->ColoniesRefShip->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ColoniesRefShip->FormattingEnabled = true;
            this->ColoniesRefShip->Location = System::Drawing::Point(482, 26);
            this->ColoniesRefShip->Name = L"ColoniesRefShip";
            this->ColoniesRefShip->Size = System::Drawing::Size(135, 21);
            this->ColoniesRefShip->TabIndex = 35;
            this->BtnTooltip->SetToolTip(this->ColoniesRefShip, L"Select ship reference for mishap calculation.");
            this->ColoniesRefShip->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // label22
            // 
            label22->AutoSize = true;
            label22->Location = System::Drawing::Point(515, 7);
            label22->Name = L"label22";
            label22->Size = System::Drawing::Size(53, 13);
            label22->TabIndex = 38;
            label22->Text = L"Ship Age:";
            // 
            // ColoniesRefColony
            // 
            this->ColoniesRefColony->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ColoniesRefColony->FormattingEnabled = true;
            this->ColoniesRefColony->Location = System::Drawing::Point(308, 26);
            this->ColoniesRefColony->Name = L"ColoniesRefColony";
            this->ColoniesRefColony->Size = System::Drawing::Size(170, 21);
            this->ColoniesRefColony->TabIndex = 41;
            this->BtnTooltip->SetToolTip(this->ColoniesRefColony, L"Select reference colony for distance and mishap calculation.");
            this->ColoniesRefColony->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesRefXYZ
            // 
            this->ColoniesRefXYZ->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ColoniesRefXYZ->FormattingEnabled = true;
            this->ColoniesRefXYZ->Location = System::Drawing::Point(4, 26);
            this->ColoniesRefXYZ->Name = L"ColoniesRefXYZ";
            this->ColoniesRefXYZ->Size = System::Drawing::Size(126, 21);
            this->ColoniesRefXYZ->TabIndex = 39;
            this->BtnTooltip->SetToolTip(this->ColoniesRefXYZ, L"Select reference system for distance and mishap calculation.");
            this->ColoniesRefXYZ->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Colonies_Update);
            // 
            // ColoniesRef
            // 
            this->ColoniesRef->BackColor = System::Drawing::Color::AliceBlue;
            this->ColoniesRef->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->ColoniesRef->Location = System::Drawing::Point(4, 3);
            this->ColoniesRef->Name = L"ColoniesRef";
            this->ColoniesRef->Size = System::Drawing::Size(300, 17);
            this->ColoniesRef->TabIndex = 31;
            this->ColoniesRef->Text = L"Ref. system:";
            this->BtnTooltip->SetToolTip(this->ColoniesRef, L"Reference system for distance and mishap % calculation.");
            // 
            // label24
            // 
            label24->AutoSize = true;
            label24->Location = System::Drawing::Point(350, 7);
            label24->Name = L"label24";
            label24->Size = System::Drawing::Size(43, 13);
            label24->TabIndex = 32;
            label24->Text = L"Coords:";
            // 
            // ColoniesGrid
            // 
            this->ColoniesGrid->AllowUserToAddRows = false;
            this->ColoniesGrid->AllowUserToDeleteRows = false;
            this->ColoniesGrid->AllowUserToOrderColumns = true;
            this->ColoniesGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->ColoniesGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle5->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle5->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle5->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle5->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle5->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle5->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle5->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->ColoniesGrid->DefaultCellStyle = dataGridViewCellStyle5;
            this->ColoniesGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->ColoniesGrid->Location = System::Drawing::Point(0, 0);
            this->ColoniesGrid->Name = L"ColoniesGrid";
            this->ColoniesGrid->ReadOnly = true;
            dataGridViewCellStyle6->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle6->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle6->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle6->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle6->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle6->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle6->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->ColoniesGrid->RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this->ColoniesGrid->RowHeadersWidth = 4;
            this->ColoniesGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            this->ColoniesGrid->ShowCellToolTips = false;
            this->ColoniesGrid->Size = System::Drawing::Size(683, 452);
            this->ColoniesGrid->TabIndex = 0;
            this->ColoniesGrid->CellMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseClick);
            this->ColoniesGrid->CellMouseLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseLeave);
            this->ColoniesGrid->CellMouseEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseEnter);
            this->ColoniesGrid->CellMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::ColoniesGrid_CellMouseDoubleClick);
            this->ColoniesGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::DataGrid_DataBindingComplete);
            // 
            // TabShips
            // 
            this->TabShips->BackColor = System::Drawing::Color::Transparent;
            this->TabShips->Controls->Add(this->splitContainer5);
            this->TabShips->Location = System::Drawing::Point(4, 22);
            this->TabShips->Name = L"TabShips";
            this->TabShips->Size = System::Drawing::Size(683, 535);
            this->TabShips->TabIndex = 5;
            this->TabShips->Text = L"Ships";
            this->TabShips->UseVisualStyleBackColor = true;
            // 
            // splitContainer5
            // 
            this->splitContainer5->BackColor = System::Drawing::SystemColors::Control;
            this->splitContainer5->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer5->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            this->splitContainer5->IsSplitterFixed = true;
            this->splitContainer5->Location = System::Drawing::Point(0, 0);
            this->splitContainer5->Name = L"splitContainer5";
            this->splitContainer5->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer5.Panel1
            // 
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRefXYZ);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltTypeML);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltTypeTR);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltRelP);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltTypeBA);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltRelN);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltOwnN);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltOwnO);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltRelE);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltRelA);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRefHome);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsFiltersReset);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRefText);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsMaxMishap);
            this->splitContainer5->Panel1->Controls->Add(label10);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRefShip);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRefColony);
            this->splitContainer5->Panel1->Controls->Add(this->ShipsRef);
            this->splitContainer5->Panel1->Controls->Add(label27);
            // 
            // splitContainer5.Panel2
            // 
            this->splitContainer5->Panel2->Controls->Add(this->ShipsGrid);
            this->splitContainer5->Size = System::Drawing::Size(683, 535);
            this->splitContainer5->SplitterDistance = 82;
            this->splitContainer5->SplitterWidth = 1;
            this->splitContainer5->TabIndex = 2;
            // 
            // ShipsRefXYZ
            // 
            this->ShipsRefXYZ->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ShipsRefXYZ->FormattingEnabled = true;
            this->ShipsRefXYZ->Location = System::Drawing::Point(4, 26);
            this->ShipsRefXYZ->Name = L"ShipsRefXYZ";
            this->ShipsRefXYZ->Size = System::Drawing::Size(126, 21);
            this->ShipsRefXYZ->TabIndex = 59;
            this->BtnTooltip->SetToolTip(this->ShipsRefXYZ, L"Select reference system for distance and mishap calculation.");
            this->ShipsRefXYZ->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltTypeML
            // 
            this->ShipsFiltTypeML->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltTypeML->Location = System::Drawing::Point(409, 52);
            this->ShipsFiltTypeML->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltTypeML->Name = L"ShipsFiltTypeML";
            this->ShipsFiltTypeML->Size = System::Drawing::Size(31, 23);
            this->ShipsFiltTypeML->TabIndex = 70;
            this->ShipsFiltTypeML->Text = L"ML";
            this->ShipsFiltTypeML->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltTypeML, L"Show MILITARY ships.");
            this->ShipsFiltTypeML->UseVisualStyleBackColor = true;
            this->ShipsFiltTypeML->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltTypeTR
            // 
            this->ShipsFiltTypeTR->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltTypeTR->Location = System::Drawing::Point(442, 52);
            this->ShipsFiltTypeTR->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltTypeTR->Name = L"ShipsFiltTypeTR";
            this->ShipsFiltTypeTR->Size = System::Drawing::Size(31, 23);
            this->ShipsFiltTypeTR->TabIndex = 71;
            this->ShipsFiltTypeTR->Text = L"TR";
            this->ShipsFiltTypeTR->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltTypeTR, L"Show TRANSPORTS.");
            this->ShipsFiltTypeTR->UseVisualStyleBackColor = true;
            this->ShipsFiltTypeTR->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltRelP
            // 
            this->ShipsFiltRelP->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltRelP->Location = System::Drawing::Point(331, 52);
            this->ShipsFiltRelP->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltRelP->Name = L"ShipsFiltRelP";
            this->ShipsFiltRelP->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltRelP->TabIndex = 70;
            this->ShipsFiltRelP->Text = L"P";
            this->ShipsFiltRelP->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltRelP, L"Show PIRATE ships.");
            this->ShipsFiltRelP->UseVisualStyleBackColor = true;
            this->ShipsFiltRelP->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltTypeBA
            // 
            this->ShipsFiltTypeBA->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltTypeBA->Location = System::Drawing::Point(376, 52);
            this->ShipsFiltTypeBA->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltTypeBA->Name = L"ShipsFiltTypeBA";
            this->ShipsFiltTypeBA->Size = System::Drawing::Size(31, 23);
            this->ShipsFiltTypeBA->TabIndex = 68;
            this->ShipsFiltTypeBA->Text = L"BA";
            this->ShipsFiltTypeBA->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltTypeBA, L"Show STAR BASES.");
            this->ShipsFiltTypeBA->UseVisualStyleBackColor = true;
            this->ShipsFiltTypeBA->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltRelN
            // 
            this->ShipsFiltRelN->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltRelN->Location = System::Drawing::Point(306, 52);
            this->ShipsFiltRelN->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltRelN->Name = L"ShipsFiltRelN";
            this->ShipsFiltRelN->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltRelN->TabIndex = 71;
            this->ShipsFiltRelN->Text = L"N";
            this->ShipsFiltRelN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltRelN, L"Show NEUTRAL ships.");
            this->ShipsFiltRelN->UseVisualStyleBackColor = true;
            this->ShipsFiltRelN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltOwnN
            // 
            this->ShipsFiltOwnN->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltOwnN->Location = System::Drawing::Point(196, 52);
            this->ShipsFiltOwnN->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltOwnN->Name = L"ShipsFiltOwnN";
            this->ShipsFiltOwnN->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltOwnN->TabIndex = 68;
            this->ShipsFiltOwnN->Text = L"N";
            this->ShipsFiltOwnN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltOwnN, L"Show NOT OWNED ships.");
            this->ShipsFiltOwnN->UseVisualStyleBackColor = true;
            this->ShipsFiltOwnN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltOwnO
            // 
            this->ShipsFiltOwnO->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltOwnO->Location = System::Drawing::Point(171, 52);
            this->ShipsFiltOwnO->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltOwnO->Name = L"ShipsFiltOwnO";
            this->ShipsFiltOwnO->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltOwnO->TabIndex = 69;
            this->ShipsFiltOwnO->Text = L"O";
            this->ShipsFiltOwnO->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltOwnO, L"Show OWNED ships.");
            this->ShipsFiltOwnO->UseVisualStyleBackColor = true;
            this->ShipsFiltOwnO->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltRelE
            // 
            this->ShipsFiltRelE->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltRelE->Location = System::Drawing::Point(281, 52);
            this->ShipsFiltRelE->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltRelE->Name = L"ShipsFiltRelE";
            this->ShipsFiltRelE->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltRelE->TabIndex = 68;
            this->ShipsFiltRelE->Text = L"E";
            this->ShipsFiltRelE->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltRelE, L"Show ENEMY ships.");
            this->ShipsFiltRelE->UseVisualStyleBackColor = true;
            this->ShipsFiltRelE->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltRelA
            // 
            this->ShipsFiltRelA->Appearance = System::Windows::Forms::Appearance::Button;
            this->ShipsFiltRelA->Location = System::Drawing::Point(256, 52);
            this->ShipsFiltRelA->Margin = System::Windows::Forms::Padding(1);
            this->ShipsFiltRelA->Name = L"ShipsFiltRelA";
            this->ShipsFiltRelA->Size = System::Drawing::Size(23, 23);
            this->ShipsFiltRelA->TabIndex = 69;
            this->ShipsFiltRelA->Text = L"A";
            this->ShipsFiltRelA->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->ShipsFiltRelA, L"Show ALLIED ships.");
            this->ShipsFiltRelA->UseVisualStyleBackColor = true;
            this->ShipsFiltRelA->CheckedChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsRefHome
            // 
            this->ShipsRefHome->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ShipsRefHome->FormattingEnabled = true;
            this->ShipsRefHome->Location = System::Drawing::Point(134, 26);
            this->ShipsRefHome->Name = L"ShipsRefHome";
            this->ShipsRefHome->Size = System::Drawing::Size(170, 21);
            this->ShipsRefHome->TabIndex = 60;
            this->BtnTooltip->SetToolTip(this->ShipsRefHome, L"Select reference home system for distance and mishap calculation.");
            this->ShipsRefHome->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsFiltersReset
            // 
            this->ShipsFiltersReset->Location = System::Drawing::Point(558, 51);
            this->ShipsFiltersReset->Name = L"ShipsFiltersReset";
            this->ShipsFiltersReset->Size = System::Drawing::Size(59, 23);
            this->ShipsFiltersReset->TabIndex = 66;
            this->ShipsFiltersReset->Text = L"<- Reset";
            this->BtnTooltip->SetToolTip(this->ShipsFiltersReset, L"Reset filters to default values.");
            this->ShipsFiltersReset->UseVisualStyleBackColor = true;
            this->ShipsFiltersReset->Click += gcnew System::EventHandler(this, &Form1::ShipsFiltersReset_Click);
            // 
            // ShipsRefText
            // 
            this->ShipsRefText->Location = System::Drawing::Point(396, 4);
            this->ShipsRefText->MaxLength = 12;
            this->ShipsRefText->Name = L"ShipsRefText";
            this->ShipsRefText->Size = System::Drawing::Size(82, 20);
            this->ShipsRefText->TabIndex = 56;
            this->BtnTooltip->SetToolTip(this->ShipsRefText, L"Enter reference system coordinates in form: X Y Z");
            this->ShipsRefText->WordWrap = false;
            // 
            // ShipsMaxMishap
            // 
            this->ShipsMaxMishap->Location = System::Drawing::Point(80, 53);
            this->ShipsMaxMishap->Name = L"ShipsMaxMishap";
            this->ShipsMaxMishap->Size = System::Drawing::Size(52, 20);
            this->ShipsMaxMishap->TabIndex = 62;
            this->BtnTooltip->SetToolTip(this->ShipsMaxMishap, L"Maximum acceptable mishap chance filter.");
            this->ShipsMaxMishap->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
            this->ShipsMaxMishap->ValueChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // label10
            // 
            label10->AutoSize = true;
            label10->Location = System::Drawing::Point(4, 56);
            label10->Name = L"label10";
            label10->Size = System::Drawing::Size(75, 13);
            label10->TabIndex = 64;
            label10->Text = L"Max Mishap%:";
            // 
            // ShipsRefShip
            // 
            this->ShipsRefShip->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ShipsRefShip->FormattingEnabled = true;
            this->ShipsRefShip->Location = System::Drawing::Point(482, 26);
            this->ShipsRefShip->Name = L"ShipsRefShip";
            this->ShipsRefShip->Size = System::Drawing::Size(135, 21);
            this->ShipsRefShip->TabIndex = 55;
            this->BtnTooltip->SetToolTip(this->ShipsRefShip, L"Select ship reference for mishap calculation.");
            this->ShipsRefShip->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsRefColony
            // 
            this->ShipsRefColony->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ShipsRefColony->FormattingEnabled = true;
            this->ShipsRefColony->Location = System::Drawing::Point(308, 26);
            this->ShipsRefColony->Name = L"ShipsRefColony";
            this->ShipsRefColony->Size = System::Drawing::Size(170, 21);
            this->ShipsRefColony->TabIndex = 61;
            this->BtnTooltip->SetToolTip(this->ShipsRefColony, L"Select reference colony for distance and mishap calculation.");
            this->ShipsRefColony->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Ships_Update);
            // 
            // ShipsRef
            // 
            this->ShipsRef->BackColor = System::Drawing::Color::AliceBlue;
            this->ShipsRef->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->ShipsRef->Location = System::Drawing::Point(4, 3);
            this->ShipsRef->Name = L"ShipsRef";
            this->ShipsRef->Size = System::Drawing::Size(300, 17);
            this->ShipsRef->TabIndex = 51;
            this->ShipsRef->Text = L"Ref. system:";
            this->BtnTooltip->SetToolTip(this->ShipsRef, L"Reference system for distance and mishap % calculation.");
            // 
            // label27
            // 
            label27->AutoSize = true;
            label27->Location = System::Drawing::Point(350, 7);
            label27->Name = L"label27";
            label27->Size = System::Drawing::Size(43, 13);
            label27->TabIndex = 52;
            label27->Text = L"Coords:";
            // 
            // ShipsGrid
            // 
            this->ShipsGrid->AllowUserToAddRows = false;
            this->ShipsGrid->AllowUserToDeleteRows = false;
            this->ShipsGrid->AllowUserToOrderColumns = true;
            this->ShipsGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->ShipsGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle7->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle7->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle7->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle7->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle7->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle7->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle7->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->ShipsGrid->DefaultCellStyle = dataGridViewCellStyle7;
            this->ShipsGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->ShipsGrid->Location = System::Drawing::Point(0, 0);
            this->ShipsGrid->Name = L"ShipsGrid";
            this->ShipsGrid->ReadOnly = true;
            dataGridViewCellStyle8->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle8->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle8->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle8->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle8->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle8->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle8->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->ShipsGrid->RowHeadersDefaultCellStyle = dataGridViewCellStyle8;
            this->ShipsGrid->RowHeadersWidth = 4;
            this->ShipsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            this->ShipsGrid->ShowCellToolTips = false;
            this->ShipsGrid->Size = System::Drawing::Size(683, 452);
            this->ShipsGrid->TabIndex = 0;
            this->ShipsGrid->CellMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseClick);
            this->ShipsGrid->CellMouseLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseLeave);
            this->ShipsGrid->CellMouseEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseEnter);
            this->ShipsGrid->CellMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::ShipsGrid_CellMouseDoubleClick);
            this->ShipsGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::DataGrid_DataBindingComplete);
            // 
            // TabAliens
            // 
            this->TabAliens->BackColor = System::Drawing::Color::Transparent;
            this->TabAliens->Controls->Add(this->splitContainer6);
            this->TabAliens->Location = System::Drawing::Point(4, 22);
            this->TabAliens->Name = L"TabAliens";
            this->TabAliens->Size = System::Drawing::Size(683, 535);
            this->TabAliens->TabIndex = 6;
            this->TabAliens->Text = L"Aliens";
            this->TabAliens->UseVisualStyleBackColor = true;
            // 
            // splitContainer6
            // 
            this->splitContainer6->BackColor = System::Drawing::SystemColors::Control;
            this->splitContainer6->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer6->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            this->splitContainer6->IsSplitterFixed = true;
            this->splitContainer6->Location = System::Drawing::Point(0, 0);
            this->splitContainer6->Name = L"splitContainer6";
            this->splitContainer6->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer6.Panel1
            // 
            this->splitContainer6->Panel1->Controls->Add(this->AliensFiltersReset);
            this->splitContainer6->Panel1->Controls->Add(this->AliensFiltRelP);
            this->splitContainer6->Panel1->Controls->Add(this->AliensFiltRelN);
            this->splitContainer6->Panel1->Controls->Add(this->AliensFiltRelE);
            this->splitContainer6->Panel1->Controls->Add(this->AliensFiltRelA);
            // 
            // splitContainer6.Panel2
            // 
            this->splitContainer6->Panel2->Controls->Add(this->AliensGrid);
            this->splitContainer6->Size = System::Drawing::Size(683, 535);
            this->splitContainer6->SplitterDistance = 29;
            this->splitContainer6->SplitterWidth = 1;
            this->splitContainer6->TabIndex = 2;
            // 
            // AliensFiltersReset
            // 
            this->AliensFiltersReset->Location = System::Drawing::Point(558, 3);
            this->AliensFiltersReset->Name = L"AliensFiltersReset";
            this->AliensFiltersReset->Size = System::Drawing::Size(59, 23);
            this->AliensFiltersReset->TabIndex = 49;
            this->AliensFiltersReset->Text = L"<- Reset";
            this->BtnTooltip->SetToolTip(this->AliensFiltersReset, L"Reset filters to default values.");
            this->AliensFiltersReset->UseVisualStyleBackColor = true;
            this->AliensFiltersReset->Click += gcnew System::EventHandler(this, &Form1::AliensFiltersReset_Click);
            // 
            // AliensFiltRelP
            // 
            this->AliensFiltRelP->Appearance = System::Windows::Forms::Appearance::Button;
            this->AliensFiltRelP->Location = System::Drawing::Point(180, 3);
            this->AliensFiltRelP->Margin = System::Windows::Forms::Padding(1);
            this->AliensFiltRelP->Name = L"AliensFiltRelP";
            this->AliensFiltRelP->Size = System::Drawing::Size(57, 23);
            this->AliensFiltRelP->TabIndex = 0;
            this->AliensFiltRelP->Text = L"Pirates";
            this->AliensFiltRelP->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->AliensFiltRelP, L"Show PIRATES.");
            this->AliensFiltRelP->UseVisualStyleBackColor = true;
            this->AliensFiltRelP->CheckedChanged += gcnew System::EventHandler(this, &Form1::Aliens_Update);
            // 
            // AliensFiltRelN
            // 
            this->AliensFiltRelN->Appearance = System::Windows::Forms::Appearance::Button;
            this->AliensFiltRelN->Location = System::Drawing::Point(121, 3);
            this->AliensFiltRelN->Margin = System::Windows::Forms::Padding(1);
            this->AliensFiltRelN->Name = L"AliensFiltRelN";
            this->AliensFiltRelN->Size = System::Drawing::Size(57, 23);
            this->AliensFiltRelN->TabIndex = 0;
            this->AliensFiltRelN->Text = L"Neutral";
            this->AliensFiltRelN->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->AliensFiltRelN, L"Show NEUTRAL.");
            this->AliensFiltRelN->UseVisualStyleBackColor = true;
            this->AliensFiltRelN->CheckedChanged += gcnew System::EventHandler(this, &Form1::Aliens_Update);
            // 
            // AliensFiltRelE
            // 
            this->AliensFiltRelE->Appearance = System::Windows::Forms::Appearance::Button;
            this->AliensFiltRelE->Location = System::Drawing::Point(62, 3);
            this->AliensFiltRelE->Margin = System::Windows::Forms::Padding(1);
            this->AliensFiltRelE->Name = L"AliensFiltRelE";
            this->AliensFiltRelE->Size = System::Drawing::Size(57, 23);
            this->AliensFiltRelE->TabIndex = 0;
            this->AliensFiltRelE->Text = L"Enemies";
            this->AliensFiltRelE->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->AliensFiltRelE, L"Show ENEMIES.");
            this->AliensFiltRelE->UseVisualStyleBackColor = true;
            this->AliensFiltRelE->CheckedChanged += gcnew System::EventHandler(this, &Form1::Aliens_Update);
            // 
            // AliensFiltRelA
            // 
            this->AliensFiltRelA->Appearance = System::Windows::Forms::Appearance::Button;
            this->AliensFiltRelA->Location = System::Drawing::Point(3, 3);
            this->AliensFiltRelA->Margin = System::Windows::Forms::Padding(1);
            this->AliensFiltRelA->Name = L"AliensFiltRelA";
            this->AliensFiltRelA->Size = System::Drawing::Size(57, 23);
            this->AliensFiltRelA->TabIndex = 0;
            this->AliensFiltRelA->Text = L"Allies";
            this->AliensFiltRelA->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->BtnTooltip->SetToolTip(this->AliensFiltRelA, L"Show ALLIES.");
            this->AliensFiltRelA->UseVisualStyleBackColor = true;
            this->AliensFiltRelA->CheckedChanged += gcnew System::EventHandler(this, &Form1::Aliens_Update);
            // 
            // AliensGrid
            // 
            this->AliensGrid->AllowUserToAddRows = false;
            this->AliensGrid->AllowUserToDeleteRows = false;
            this->AliensGrid->AllowUserToOrderColumns = true;
            this->AliensGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->AliensGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle9->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle9->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle9->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle9->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle9->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle9->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle9->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->AliensGrid->DefaultCellStyle = dataGridViewCellStyle9;
            this->AliensGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->AliensGrid->Location = System::Drawing::Point(0, 0);
            this->AliensGrid->Name = L"AliensGrid";
            this->AliensGrid->ReadOnly = true;
            dataGridViewCellStyle10->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle10->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle10->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle10->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle10->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle10->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle10->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->AliensGrid->RowHeadersDefaultCellStyle = dataGridViewCellStyle10;
            this->AliensGrid->RowHeadersWidth = 4;
            this->AliensGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            this->AliensGrid->ShowCellToolTips = false;
            this->AliensGrid->Size = System::Drawing::Size(683, 505);
            this->AliensGrid->TabIndex = 0;
            this->AliensGrid->CellMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseClick);
            this->AliensGrid->CellMouseLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseLeave);
            this->AliensGrid->CellMouseEnter += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::Grid_CellMouseEnter);
            this->AliensGrid->CellMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::Grid_CellMouseDoubleClick);
            this->AliensGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::DataGrid_DataBindingComplete);
            // 
            // TabOrders
            // 
            this->TabOrders->Controls->Add(this->OrderTemplate);
            this->TabOrders->Location = System::Drawing::Point(4, 22);
            this->TabOrders->Name = L"TabOrders";
            this->TabOrders->Size = System::Drawing::Size(683, 535);
            this->TabOrders->TabIndex = 9;
            this->TabOrders->Text = L"Orders";
            this->TabOrders->UseVisualStyleBackColor = true;
            // 
            // OrderTemplate
            // 
            this->OrderTemplate->Dock = System::Windows::Forms::DockStyle::Fill;
            this->OrderTemplate->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(0)));
            this->OrderTemplate->Location = System::Drawing::Point(0, 0);
            this->OrderTemplate->Name = L"OrderTemplate";
            this->OrderTemplate->ReadOnly = true;
            this->OrderTemplate->Size = System::Drawing::Size(683, 535);
            this->OrderTemplate->TabIndex = 0;
            this->OrderTemplate->Text = L"";
            // 
            // TabUtils
            // 
            this->TabUtils->BackColor = System::Drawing::SystemColors::Control;
            this->TabUtils->Location = System::Drawing::Point(4, 22);
            this->TabUtils->Name = L"TabUtils";
            this->TabUtils->Size = System::Drawing::Size(683, 535);
            this->TabUtils->TabIndex = 10;
            this->TabUtils->Text = L"Utils";
            // 
            // TabAbout
            // 
            this->TabAbout->BackColor = System::Drawing::Color::Transparent;
            this->TabAbout->Controls->Add(this->TextAbout);
            this->TabAbout->Location = System::Drawing::Point(4, 22);
            this->TabAbout->Name = L"TabAbout";
            this->TabAbout->Padding = System::Windows::Forms::Padding(3);
            this->TabAbout->Size = System::Drawing::Size(683, 535);
            this->TabAbout->TabIndex = 8;
            this->TabAbout->Text = L"About";
            this->TabAbout->UseVisualStyleBackColor = true;
            // 
            // TextAbout
            // 
            this->TextAbout->Dock = System::Windows::Forms::DockStyle::Fill;
            this->TextAbout->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            this->TextAbout->Location = System::Drawing::Point(3, 3);
            this->TextAbout->Multiline = true;
            this->TextAbout->Name = L"TextAbout";
            this->TextAbout->ReadOnly = true;
            this->TextAbout->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
            this->TextAbout->Size = System::Drawing::Size(677, 529);
            this->TextAbout->TabIndex = 0;
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(356, 8);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(53, 13);
            label5->TabIndex = 5;
            label5->Text = L"Ship Age:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(4, 7);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(95, 13);
            label6->TabIndex = 0;
            label6->Text = L"Reference system:";
            // 
            // comboBox1
            // 
            this->comboBox1->FormattingEnabled = true;
            this->comboBox1->Location = System::Drawing::Point(415, 5);
            this->comboBox1->Name = L"comboBox1";
            this->comboBox1->Size = System::Drawing::Size(135, 21);
            this->comboBox1->TabIndex = 6;
            // 
            // checkBox1
            // 
            this->checkBox1->AutoSize = true;
            this->checkBox1->Location = System::Drawing::Point(258, 7);
            this->checkBox1->Name = L"checkBox1";
            this->checkBox1->Size = System::Drawing::Size(44, 17);
            this->checkBox1->TabIndex = 4;
            this->checkBox1->Text = L"GV:";
            this->checkBox1->UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this->textBox1->Location = System::Drawing::Point(305, 5);
            this->textBox1->MaxLength = 4;
            this->textBox1->Name = L"textBox1";
            this->textBox1->Size = System::Drawing::Size(45, 20);
            this->textBox1->TabIndex = 2;
            this->textBox1->Text = L"0";
            // 
            // comboBox2
            // 
            this->comboBox2->FormattingEnabled = true;
            this->comboBox2->Location = System::Drawing::Point(105, 4);
            this->comboBox2->Name = L"comboBox2";
            this->comboBox2->Size = System::Drawing::Size(147, 21);
            this->comboBox2->TabIndex = 1;
            // 
            // dataGridView1
            // 
            this->dataGridView1->AllowUserToAddRows = false;
            this->dataGridView1->AllowUserToDeleteRows = false;
            this->dataGridView1->AllowUserToOrderColumns = true;
            this->dataGridView1->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            dataGridViewCellStyle11->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle11->BackColor = System::Drawing::SystemColors::Window;
            dataGridViewCellStyle11->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle11->ForeColor = System::Drawing::SystemColors::ControlText;
            dataGridViewCellStyle11->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle11->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle11->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
            this->dataGridView1->DefaultCellStyle = dataGridViewCellStyle11;
            this->dataGridView1->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dataGridView1->Location = System::Drawing::Point(0, 0);
            this->dataGridView1->Name = L"dataGridView1";
            this->dataGridView1->ReadOnly = true;
            dataGridViewCellStyle12->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
            dataGridViewCellStyle12->BackColor = System::Drawing::SystemColors::Control;
            dataGridViewCellStyle12->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            dataGridViewCellStyle12->ForeColor = System::Drawing::SystemColors::WindowText;
            dataGridViewCellStyle12->SelectionBackColor = System::Drawing::SystemColors::Highlight;
            dataGridViewCellStyle12->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
            dataGridViewCellStyle12->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
            this->dataGridView1->RowHeadersDefaultCellStyle = dataGridViewCellStyle12;
            this->dataGridView1->RowHeadersWidth = 4;
            this->dataGridView1->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->dataGridView1->Size = System::Drawing::Size(569, 504);
            this->dataGridView1->TabIndex = 0;
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(927, 593);
            this->Controls->Add(TopSplitCont);
            this->Name = L"Form1";
            this->Text = L"FarHorizons User Interface";
            this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
            TopSplitCont->Panel1->ResumeLayout(false);
            TopSplitCont->Panel2->ResumeLayout(false);
            TopSplitCont->Panel2->PerformLayout();
            TopSplitCont->ResumeLayout(false);
            splitContainer7->Panel1->ResumeLayout(false);
            splitContainer7->Panel2->ResumeLayout(false);
            splitContainer7->Panel2->PerformLayout();
            splitContainer7->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechBI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechMI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechMA))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechML))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechGV))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TechLS))->EndInit();
            this->MenuTabs->ResumeLayout(false);
            this->TabReports->ResumeLayout(false);
            this->TabMap->ResumeLayout(false);
            this->panel1->ResumeLayout(false);
            this->panel1->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapLSNVal))->EndInit();
            groupBox3->ResumeLayout(false);
            groupBox3->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP3GV))->EndInit();
            groupBox2->ResumeLayout(false);
            groupBox2->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP2GV))->EndInit();
            groupBox1->ResumeLayout(false);
            groupBox1->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSP1GV))->EndInit();
            this->MapSPSelf->ResumeLayout(false);
            this->MapSPSelf->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MapSPSelfGV))->EndInit();
            this->TabSystems->ResumeLayout(false);
            splitContainer2->Panel1->ResumeLayout(false);
            splitContainer2->Panel1->PerformLayout();
            splitContainer2->Panel2->ResumeLayout(false);
            splitContainer2->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsMaxLSN))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsMaxMishap))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsShipAge))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->EndInit();
            this->TabPlanets->ResumeLayout(false);
            splitContainer3->Panel1->ResumeLayout(false);
            splitContainer3->Panel1->PerformLayout();
            splitContainer3->Panel2->ResumeLayout(false);
            splitContainer3->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsMaxLSN))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsMaxMishap))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsShipAge))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsGrid))->EndInit();
            this->TabColonies->ResumeLayout(false);
            this->splitContainer4->Panel1->ResumeLayout(false);
            this->splitContainer4->Panel1->PerformLayout();
            this->splitContainer4->Panel2->ResumeLayout(false);
            this->splitContainer4->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesMaxLSN))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesMaxMishap))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesShipAge))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesGrid))->EndInit();
            this->TabShips->ResumeLayout(false);
            this->splitContainer5->Panel1->ResumeLayout(false);
            this->splitContainer5->Panel1->PerformLayout();
            this->splitContainer5->Panel2->ResumeLayout(false);
            this->splitContainer5->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsMaxMishap))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsGrid))->EndInit();
            this->TabAliens->ResumeLayout(false);
            this->splitContainer6->Panel1->ResumeLayout(false);
            this->splitContainer6->Panel2->ResumeLayout(false);
            this->splitContainer6->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AliensGrid))->EndInit();
            this->TabOrders->ResumeLayout(false);
            this->TabAbout->ResumeLayout(false);
            this->TabAbout->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion
private: System::Void RepTurnNr_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             DisplayReport();
         }
private: System::Void TurnSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             DisplayTurn();
         }
private: System::Void DataGrid_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
             SetGridBgAndTooltip((DataGridView^)sender);
         }
private: System::Void MapCanvas_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
             MapDraw();
         }
private: System::Void MapEnLSN_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
             MapLSNVal->Enabled = MapEnLSN->Checked;
             MapDraw();
         }
private: System::Void MapUpdate(System::Object^  sender, System::EventArgs^  e) {
             MapDraw();
         }
private: System::Void TurnReload_Click(System::Object^  sender, System::EventArgs^  e) {
             TurnReload();
         }
private: System::Void RepMode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
             RepModeChanged();
         }
private: System::Void Systems_Update(System::Object^  sender, System::EventArgs^  e) {
             m_SystemsFilter->Update(sender);
         }
private: System::Void SystemsFiltersReset_Click(System::Object^  sender, System::EventArgs^  e) {
             m_SystemsFilter->Reset();
         }
private: System::Void SystemsGrid_CellMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Left )
             {
                 SystemsSelectPlanets( e->RowIndex );
             }
             else
                 Grid_CellMouseDoubleClick(sender, e);
         }
private: System::Void Planets_Update(System::Object^  sender, System::EventArgs^  e) {
             m_PlanetsFilter->Update(sender);
         }
private: System::Void PlanetsFiltersReset_Click(System::Object^  sender, System::EventArgs^  e) {
             m_PlanetsFilter->Reset();
         }
private: System::Void PlanetsGrid_CellMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Left )
             {
                 PlanetsSelectColonies( e->RowIndex );
             }
             else
                 Grid_CellMouseDoubleClick(sender, e);
         }
private: System::Void Colonies_Update(System::Object^  sender, System::EventArgs^  e) {
             m_ColoniesFilter->Update(sender);
         }
private: System::Void ColoniesFiltersReset_Click(System::Object^  sender, System::EventArgs^  e) {
             m_ColoniesFilter->Reset();
         }
private: System::Void ColoniesGrid_CellMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Left )
             {
                 ColoniesSetRef( e->RowIndex );
             }
             else
                 Grid_CellMouseDoubleClick(sender, e);
         }
private: System::Void Ships_Update(System::Object^  sender, System::EventArgs^  e) {
             m_ShipsFilter->Update(sender);
         }
private: System::Void ShipsFiltersReset_Click(System::Object^  sender, System::EventArgs^  e) {
             m_ShipsFilter->Reset();
         }
private: System::Void ShipsGrid_CellMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Left )
             {
                 ShipsSetRef( e->RowIndex );
             }
             else
                 Grid_CellMouseDoubleClick(sender, e);
         }
private: System::Void Aliens_Update(System::Object^  sender, System::EventArgs^  e) {
             m_AliensFilter->Update(sender);
         }
private: System::Void AliensFiltersReset_Click(System::Object^  sender, System::EventArgs^  e) {
             m_AliensFilter->Reset();
         }
private: System::Void Grid_CellMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Middle )
             {
                 SetGridRefSystemOnMouseClick(safe_cast<DataGridView^>(sender), e->RowIndex);
             }
         }

private: System::Void Grid_CellMouseEnter(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
             DataGridView ^grid = safe_cast<DataGridView^>(sender);
             if( e->ColumnIndex >= 0 && e->RowIndex >= 0 )
             {
                 String^ toolTipText = grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->ToolTipText;

                 // Show tooltip for a 30 sec.
                 m_GridToolTip->Show(toolTipText, grid, 30000);
             }
         }
private: System::Void Grid_CellMouseLeave(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
                // Stop displaying tooltip
                m_GridToolTip->Hide(safe_cast<DataGridView^>(sender));
         }
private: System::Void Grid_CellMouseClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
             if( e->Button == Windows::Forms::MouseButtons::Right )
             {
                 ShowGridContextMenu(safe_cast<DataGridView^>(sender), e);
             }
         }
private: System::Void TechResetCurrent_Click(System::Object^  sender, System::EventArgs^  e) {
             TechLevelsResetToCurrent();
             TechLevelsChanged();
         }
private: System::Void TechResetTaught_Click(System::Object^  sender, System::EventArgs^  e) {
             TechLevelsResetToTaught();
             TechLevelsChanged();
         }
private: System::Void Tech_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             TechLevelsChanged();
         }
private: System::Void MenuTabs_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateTabs();
         }
private: System::Void PlanetsGrid_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
             PlanetsMenuAddName(e);
         }
};

} // end namespace FHUI
