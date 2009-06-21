#pragma once

ref class GameData;
ref class Alien;
ref class PlanetView;

namespace fhui {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

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
            : m_Reports(gcnew SortedList)
		{
            InitializeComponent();
            LoadGameData();
        }

	protected:
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

        void        LoadGameData();
        void        LoadGalaxy();
        void        LoadReports();
        void        LoadReport(String ^fileName);
        void        LoadCommands();
        void        InitData();
        void        SetupSystems();
        void        SetupPlanets();
        void        SetupColonies();
        void        SetupShips();
        void        SetupAliens();

        void        FillAboutBox();
        void        DisplayReport();

        String^     SystemsGetRowTooltip(DataGridViewRow ^row);
        Color       ColoniesGetRowColor(DataGridViewRow ^row);
        Color       ShipsGetRowColor(DataGridViewRow ^row);
        Color       AliensGetRowColor(DataGridViewRow ^row);

        Color       GetAlienColor(Alien ^sp);

        GameData   ^m_GameData;
        SortedList ^m_PlanetsView;
        SortedList ^m_Reports;

        // --------------------------------------------------

    private: System::Windows::Forms::TextBox^  Summary;
    private: System::Windows::Forms::TabControl^  MenuTabs;
    private: System::Windows::Forms::TabPage^  TabReports;


    private: System::Windows::Forms::TabPage^  TabMap;
    private: System::Windows::Forms::TabPage^  TabSystems;
    private: System::Windows::Forms::TabPage^  TabPlanets;
    private: System::Windows::Forms::TabPage^  TabColonies;
    private: System::Windows::Forms::TabPage^  TabShips;
    private: System::Windows::Forms::TabPage^  TabAliens;
    private: System::Windows::Forms::TabPage^  TabCommands;
    private: System::Windows::Forms::Label^  RepStatus;

    private: System::Windows::Forms::ComboBox^  RepTurnNr;



    private: System::Windows::Forms::ComboBox^  RepMode;
    private: System::Windows::Forms::TextBox^  RepText;
    private: System::Windows::Forms::TextBox^  SystemsGVVal;


    private: System::Windows::Forms::ComboBox^  SystemsRef;

    private: System::Windows::Forms::SplitContainer^  splitContainer2;
    private: System::Windows::Forms::ComboBox^  SystemsShipAge;

    private: System::Windows::Forms::CheckBox^  SystemsGV;
    private: System::Windows::Forms::DataGridView^  SystemsGrid;
    private: System::Windows::Forms::SplitContainer^  splitContainer3;
    private: System::Windows::Forms::ComboBox^  PlanetsShipAge;

    private: System::Windows::Forms::CheckBox^  PlanetsGV;
    private: System::Windows::Forms::TextBox^  PlanetsGVVal;
    private: System::Windows::Forms::ComboBox^  PlanetsRef;
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


















    private: System::ComponentModel::IContainer^  components;





    protected: 

    protected: 





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
            System::Windows::Forms::SplitContainer^  splitContainer1;
            System::Windows::Forms::Label^  label1;
            System::Windows::Forms::SplitContainer^  TopSplitCont;
            System::Windows::Forms::Label^  label2;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label3;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label9;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle7 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle8 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label10;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle9 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle10 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle11 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle12 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
            this->RepStatus = (gcnew System::Windows::Forms::Label());
            this->RepMode = (gcnew System::Windows::Forms::ComboBox());
            this->RepTurnNr = (gcnew System::Windows::Forms::ComboBox());
            this->RepText = (gcnew System::Windows::Forms::TextBox());
            this->Summary = (gcnew System::Windows::Forms::TextBox());
            this->MenuTabs = (gcnew System::Windows::Forms::TabControl());
            this->TabReports = (gcnew System::Windows::Forms::TabPage());
            this->TabMap = (gcnew System::Windows::Forms::TabPage());
            this->TabSystems = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
            this->SystemsShipAge = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsGV = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsGVVal = (gcnew System::Windows::Forms::TextBox());
            this->SystemsRef = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabPlanets = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer3 = (gcnew System::Windows::Forms::SplitContainer());
            this->PlanetsShipAge = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsGV = (gcnew System::Windows::Forms::CheckBox());
            this->PlanetsGVVal = (gcnew System::Windows::Forms::TextBox());
            this->PlanetsRef = (gcnew System::Windows::Forms::ComboBox());
            this->PlanetsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabColonies = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer4 = (gcnew System::Windows::Forms::SplitContainer());
            this->ColoniesGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabShips = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer5 = (gcnew System::Windows::Forms::SplitContainer());
            this->ShipsGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabAliens = (gcnew System::Windows::Forms::TabPage());
            this->splitContainer6 = (gcnew System::Windows::Forms::SplitContainer());
            this->AliensGrid = (gcnew System::Windows::Forms::DataGridView());
            this->TabCommands = (gcnew System::Windows::Forms::TabPage());
            this->TabAbout = (gcnew System::Windows::Forms::TabPage());
            this->TextAbout = (gcnew System::Windows::Forms::TextBox());
            this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
            this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
            this->textBox1 = (gcnew System::Windows::Forms::TextBox());
            this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
            this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
            splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
            label1 = (gcnew System::Windows::Forms::Label());
            TopSplitCont = (gcnew System::Windows::Forms::SplitContainer());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            label10 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            splitContainer1->Panel1->SuspendLayout();
            splitContainer1->Panel2->SuspendLayout();
            splitContainer1->SuspendLayout();
            TopSplitCont->Panel1->SuspendLayout();
            TopSplitCont->Panel2->SuspendLayout();
            TopSplitCont->SuspendLayout();
            this->MenuTabs->SuspendLayout();
            this->TabReports->SuspendLayout();
            this->TabSystems->SuspendLayout();
            this->splitContainer2->Panel1->SuspendLayout();
            this->splitContainer2->Panel2->SuspendLayout();
            this->splitContainer2->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->BeginInit();
            this->TabPlanets->SuspendLayout();
            this->splitContainer3->Panel1->SuspendLayout();
            this->splitContainer3->Panel2->SuspendLayout();
            this->splitContainer3->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsGrid))->BeginInit();
            this->TabColonies->SuspendLayout();
            this->splitContainer4->Panel1->SuspendLayout();
            this->splitContainer4->Panel2->SuspendLayout();
            this->splitContainer4->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesGrid))->BeginInit();
            this->TabShips->SuspendLayout();
            this->splitContainer5->Panel1->SuspendLayout();
            this->splitContainer5->Panel2->SuspendLayout();
            this->splitContainer5->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsGrid))->BeginInit();
            this->TabAliens->SuspendLayout();
            this->splitContainer6->Panel1->SuspendLayout();
            this->splitContainer6->Panel2->SuspendLayout();
            this->splitContainer6->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AliensGrid))->BeginInit();
            this->TabAbout->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
            this->SuspendLayout();
            // 
            // splitContainer1
            // 
            splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
            splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            splitContainer1->IsSplitterFixed = true;
            splitContainer1->Location = System::Drawing::Point(3, 3);
            splitContainer1->Name = L"splitContainer1";
            splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            splitContainer1->Panel1->Controls->Add(this->RepStatus);
            splitContainer1->Panel1->Controls->Add(this->RepMode);
            splitContainer1->Panel1->Controls->Add(this->RepTurnNr);
            // 
            // splitContainer1.Panel2
            // 
            splitContainer1->Panel2->Controls->Add(this->RepText);
            splitContainer1->Size = System::Drawing::Size(543, 529);
            splitContainer1->SplitterDistance = 25;
            splitContainer1->SplitterWidth = 1;
            splitContainer1->TabIndex = 0;
            // 
            // RepStatus
            // 
            this->RepStatus->AutoSize = true;
            this->RepStatus->Location = System::Drawing::Point(245, 6);
            this->RepStatus->Name = L"RepStatus";
            this->RepStatus->Size = System::Drawing::Size(16, 13);
            this->RepStatus->TabIndex = 1;
            this->RepStatus->Text = L"...";
            // 
            // RepMode
            // 
            this->RepMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->RepMode->Enabled = false;
            this->RepMode->FormattingEnabled = true;
            this->RepMode->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Commands", L"Reports"});
            this->RepMode->Location = System::Drawing::Point(3, 3);
            this->RepMode->Name = L"RepMode";
            this->RepMode->Size = System::Drawing::Size(101, 21);
            this->RepMode->Sorted = true;
            this->RepMode->TabIndex = 0;
            // 
            // RepTurnNr
            // 
            this->RepTurnNr->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->RepTurnNr->FormattingEnabled = true;
            this->RepTurnNr->Location = System::Drawing::Point(110, 3);
            this->RepTurnNr->MaxDropDownItems = 20;
            this->RepTurnNr->Name = L"RepTurnNr";
            this->RepTurnNr->Size = System::Drawing::Size(129, 21);
            this->RepTurnNr->TabIndex = 0;
            this->RepTurnNr->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::RepTurnNr_SelectedIndexChanged);
            // 
            // RepText
            // 
            this->RepText->Dock = System::Windows::Forms::DockStyle::Fill;
            this->RepText->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            this->RepText->Location = System::Drawing::Point(0, 0);
            this->RepText->Multiline = true;
            this->RepText->Name = L"RepText";
            this->RepText->ReadOnly = true;
            this->RepText->ScrollBars = System::Windows::Forms::ScrollBars::Both;
            this->RepText->Size = System::Drawing::Size(543, 503);
            this->RepText->TabIndex = 0;
            this->RepText->WordWrap = false;
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(4, 7);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(95, 13);
            label1->TabIndex = 0;
            label1->Text = L"Reference system:";
            // 
            // TopSplitCont
            // 
            TopSplitCont->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            TopSplitCont->Dock = System::Windows::Forms::DockStyle::Fill;
            TopSplitCont->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
            TopSplitCont->IsSplitterFixed = true;
            TopSplitCont->Location = System::Drawing::Point(0, 0);
            TopSplitCont->Margin = System::Windows::Forms::Padding(0);
            TopSplitCont->Name = L"TopSplitCont";
            // 
            // TopSplitCont.Panel1
            // 
            TopSplitCont->Panel1->AutoScroll = true;
            TopSplitCont->Panel1->Controls->Add(this->Summary);
            // 
            // TopSplitCont.Panel2
            // 
            TopSplitCont->Panel2->Controls->Add(this->MenuTabs);
            TopSplitCont->Size = System::Drawing::Size(793, 565);
            TopSplitCont->SplitterDistance = 230;
            TopSplitCont->SplitterWidth = 2;
            TopSplitCont->TabIndex = 0;
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
            this->Summary->Size = System::Drawing::Size(226, 561);
            this->Summary->TabIndex = 0;
            this->Summary->WordWrap = false;
            // 
            // MenuTabs
            // 
            this->MenuTabs->Controls->Add(this->TabReports);
            this->MenuTabs->Controls->Add(this->TabMap);
            this->MenuTabs->Controls->Add(this->TabSystems);
            this->MenuTabs->Controls->Add(this->TabPlanets);
            this->MenuTabs->Controls->Add(this->TabColonies);
            this->MenuTabs->Controls->Add(this->TabShips);
            this->MenuTabs->Controls->Add(this->TabAliens);
            this->MenuTabs->Controls->Add(this->TabCommands);
            this->MenuTabs->Controls->Add(this->TabAbout);
            this->MenuTabs->Dock = System::Windows::Forms::DockStyle::Fill;
            this->MenuTabs->Location = System::Drawing::Point(0, 0);
            this->MenuTabs->Name = L"MenuTabs";
            this->MenuTabs->SelectedIndex = 0;
            this->MenuTabs->Size = System::Drawing::Size(557, 561);
            this->MenuTabs->TabIndex = 0;
            // 
            // TabReports
            // 
            this->TabReports->Controls->Add(splitContainer1);
            this->TabReports->Location = System::Drawing::Point(4, 22);
            this->TabReports->Margin = System::Windows::Forms::Padding(0);
            this->TabReports->Name = L"TabReports";
            this->TabReports->Padding = System::Windows::Forms::Padding(3);
            this->TabReports->Size = System::Drawing::Size(549, 535);
            this->TabReports->TabIndex = 0;
            this->TabReports->Text = L"Reports";
            this->TabReports->UseVisualStyleBackColor = true;
            // 
            // TabMap
            // 
            this->TabMap->Location = System::Drawing::Point(4, 22);
            this->TabMap->Name = L"TabMap";
            this->TabMap->Padding = System::Windows::Forms::Padding(3);
            this->TabMap->Size = System::Drawing::Size(549, 535);
            this->TabMap->TabIndex = 1;
            this->TabMap->Text = L"Map";
            this->TabMap->UseVisualStyleBackColor = true;
            // 
            // TabSystems
            // 
            this->TabSystems->Controls->Add(this->splitContainer2);
            this->TabSystems->Location = System::Drawing::Point(4, 22);
            this->TabSystems->Name = L"TabSystems";
            this->TabSystems->Size = System::Drawing::Size(549, 535);
            this->TabSystems->TabIndex = 2;
            this->TabSystems->Text = L"Systems";
            this->TabSystems->UseVisualStyleBackColor = true;
            // 
            // splitContainer2
            // 
            this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer2->Location = System::Drawing::Point(0, 0);
            this->splitContainer2->Name = L"splitContainer2";
            this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this->splitContainer2->Panel1->Controls->Add(this->SystemsShipAge);
            this->splitContainer2->Panel1->Controls->Add(label2);
            this->splitContainer2->Panel1->Controls->Add(this->SystemsGV);
            this->splitContainer2->Panel1->Controls->Add(this->SystemsGVVal);
            this->splitContainer2->Panel1->Controls->Add(this->SystemsRef);
            this->splitContainer2->Panel1->Controls->Add(label1);
            // 
            // splitContainer2.Panel2
            // 
            this->splitContainer2->Panel2->Controls->Add(this->SystemsGrid);
            this->splitContainer2->Size = System::Drawing::Size(549, 535);
            this->splitContainer2->SplitterDistance = 30;
            this->splitContainer2->SplitterWidth = 1;
            this->splitContainer2->TabIndex = 0;
            // 
            // SystemsShipAge
            // 
            this->SystemsShipAge->FormattingEnabled = true;
            this->SystemsShipAge->Location = System::Drawing::Point(415, 5);
            this->SystemsShipAge->Name = L"SystemsShipAge";
            this->SystemsShipAge->Size = System::Drawing::Size(135, 21);
            this->SystemsShipAge->TabIndex = 6;
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(356, 8);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(53, 13);
            label2->TabIndex = 5;
            label2->Text = L"Ship Age:";
            // 
            // SystemsGV
            // 
            this->SystemsGV->AutoSize = true;
            this->SystemsGV->Location = System::Drawing::Point(258, 7);
            this->SystemsGV->Name = L"SystemsGV";
            this->SystemsGV->Size = System::Drawing::Size(44, 17);
            this->SystemsGV->TabIndex = 4;
            this->SystemsGV->Text = L"GV:";
            this->SystemsGV->UseVisualStyleBackColor = true;
            // 
            // SystemsGVVal
            // 
            this->SystemsGVVal->Location = System::Drawing::Point(305, 5);
            this->SystemsGVVal->MaxLength = 4;
            this->SystemsGVVal->Name = L"SystemsGVVal";
            this->SystemsGVVal->Size = System::Drawing::Size(45, 20);
            this->SystemsGVVal->TabIndex = 2;
            this->SystemsGVVal->Text = L"0";
            // 
            // SystemsRef
            // 
            this->SystemsRef->FormattingEnabled = true;
            this->SystemsRef->Location = System::Drawing::Point(105, 4);
            this->SystemsRef->Name = L"SystemsRef";
            this->SystemsRef->Size = System::Drawing::Size(147, 21);
            this->SystemsRef->TabIndex = 1;
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
            this->SystemsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->SystemsGrid->Size = System::Drawing::Size(549, 504);
            this->SystemsGrid->TabIndex = 0;
            this->SystemsGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::SystemsGrid_DataBindingComplete);
            // 
            // TabPlanets
            // 
            this->TabPlanets->Controls->Add(this->splitContainer3);
            this->TabPlanets->Location = System::Drawing::Point(4, 22);
            this->TabPlanets->Name = L"TabPlanets";
            this->TabPlanets->Size = System::Drawing::Size(549, 535);
            this->TabPlanets->TabIndex = 3;
            this->TabPlanets->Text = L"Planets";
            this->TabPlanets->UseVisualStyleBackColor = true;
            // 
            // splitContainer3
            // 
            this->splitContainer3->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer3->Location = System::Drawing::Point(0, 0);
            this->splitContainer3->Name = L"splitContainer3";
            this->splitContainer3->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            this->splitContainer3->Panel1->Controls->Add(this->PlanetsShipAge);
            this->splitContainer3->Panel1->Controls->Add(label3);
            this->splitContainer3->Panel1->Controls->Add(this->PlanetsGV);
            this->splitContainer3->Panel1->Controls->Add(this->PlanetsGVVal);
            this->splitContainer3->Panel1->Controls->Add(this->PlanetsRef);
            this->splitContainer3->Panel1->Controls->Add(label4);
            // 
            // splitContainer3.Panel2
            // 
            this->splitContainer3->Panel2->Controls->Add(this->PlanetsGrid);
            this->splitContainer3->Size = System::Drawing::Size(549, 535);
            this->splitContainer3->SplitterDistance = 30;
            this->splitContainer3->SplitterWidth = 1;
            this->splitContainer3->TabIndex = 1;
            // 
            // PlanetsShipAge
            // 
            this->PlanetsShipAge->FormattingEnabled = true;
            this->PlanetsShipAge->Location = System::Drawing::Point(415, 5);
            this->PlanetsShipAge->Name = L"PlanetsShipAge";
            this->PlanetsShipAge->Size = System::Drawing::Size(135, 21);
            this->PlanetsShipAge->TabIndex = 6;
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(356, 8);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(53, 13);
            label3->TabIndex = 5;
            label3->Text = L"Ship Age:";
            // 
            // PlanetsGV
            // 
            this->PlanetsGV->AutoSize = true;
            this->PlanetsGV->Location = System::Drawing::Point(258, 7);
            this->PlanetsGV->Name = L"PlanetsGV";
            this->PlanetsGV->Size = System::Drawing::Size(44, 17);
            this->PlanetsGV->TabIndex = 4;
            this->PlanetsGV->Text = L"GV:";
            this->PlanetsGV->UseVisualStyleBackColor = true;
            // 
            // PlanetsGVVal
            // 
            this->PlanetsGVVal->Location = System::Drawing::Point(305, 5);
            this->PlanetsGVVal->MaxLength = 4;
            this->PlanetsGVVal->Name = L"PlanetsGVVal";
            this->PlanetsGVVal->Size = System::Drawing::Size(45, 20);
            this->PlanetsGVVal->TabIndex = 2;
            this->PlanetsGVVal->Text = L"0";
            // 
            // PlanetsRef
            // 
            this->PlanetsRef->FormattingEnabled = true;
            this->PlanetsRef->Location = System::Drawing::Point(105, 4);
            this->PlanetsRef->Name = L"PlanetsRef";
            this->PlanetsRef->Size = System::Drawing::Size(147, 21);
            this->PlanetsRef->TabIndex = 1;
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(4, 7);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(95, 13);
            label4->TabIndex = 0;
            label4->Text = L"Reference system:";
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
            this->PlanetsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->PlanetsGrid->Size = System::Drawing::Size(549, 504);
            this->PlanetsGrid->TabIndex = 0;
            // 
            // TabColonies
            // 
            this->TabColonies->Controls->Add(this->splitContainer4);
            this->TabColonies->Location = System::Drawing::Point(4, 22);
            this->TabColonies->Name = L"TabColonies";
            this->TabColonies->Size = System::Drawing::Size(549, 535);
            this->TabColonies->TabIndex = 4;
            this->TabColonies->Text = L"Colonies";
            this->TabColonies->UseVisualStyleBackColor = true;
            // 
            // splitContainer4
            // 
            this->splitContainer4->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer4->Location = System::Drawing::Point(0, 0);
            this->splitContainer4->Name = L"splitContainer4";
            this->splitContainer4->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this->splitContainer4->Panel1->Controls->Add(label8);
            // 
            // splitContainer4.Panel2
            // 
            this->splitContainer4->Panel2->Controls->Add(this->ColoniesGrid);
            this->splitContainer4->Size = System::Drawing::Size(549, 535);
            this->splitContainer4->SplitterDistance = 30;
            this->splitContainer4->SplitterWidth = 1;
            this->splitContainer4->TabIndex = 2;
            // 
            // label8
            // 
            label8->AutoSize = true;
            label8->Location = System::Drawing::Point(4, 7);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(19, 13);
            label8->TabIndex = 0;
            label8->Text = L"....";
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
            this->ColoniesGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->ColoniesGrid->Size = System::Drawing::Size(549, 504);
            this->ColoniesGrid->TabIndex = 0;
            this->ColoniesGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::ColoniesGrid_DataBindingComplete);
            // 
            // TabShips
            // 
            this->TabShips->Controls->Add(this->splitContainer5);
            this->TabShips->Location = System::Drawing::Point(4, 22);
            this->TabShips->Name = L"TabShips";
            this->TabShips->Size = System::Drawing::Size(549, 535);
            this->TabShips->TabIndex = 5;
            this->TabShips->Text = L"Ships";
            this->TabShips->UseVisualStyleBackColor = true;
            // 
            // splitContainer5
            // 
            this->splitContainer5->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer5->Location = System::Drawing::Point(0, 0);
            this->splitContainer5->Name = L"splitContainer5";
            this->splitContainer5->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer5.Panel1
            // 
            this->splitContainer5->Panel1->Controls->Add(label9);
            // 
            // splitContainer5.Panel2
            // 
            this->splitContainer5->Panel2->Controls->Add(this->ShipsGrid);
            this->splitContainer5->Size = System::Drawing::Size(549, 535);
            this->splitContainer5->SplitterDistance = 30;
            this->splitContainer5->SplitterWidth = 1;
            this->splitContainer5->TabIndex = 2;
            // 
            // label9
            // 
            label9->AutoSize = true;
            label9->Location = System::Drawing::Point(4, 7);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(31, 13);
            label9->TabIndex = 0;
            label9->Text = L"........";
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
            this->ShipsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->ShipsGrid->Size = System::Drawing::Size(549, 504);
            this->ShipsGrid->TabIndex = 0;
            this->ShipsGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::ShipsGrid_DataBindingComplete);
            // 
            // TabAliens
            // 
            this->TabAliens->Controls->Add(this->splitContainer6);
            this->TabAliens->Location = System::Drawing::Point(4, 22);
            this->TabAliens->Name = L"TabAliens";
            this->TabAliens->Size = System::Drawing::Size(549, 535);
            this->TabAliens->TabIndex = 6;
            this->TabAliens->Text = L"Aliens";
            this->TabAliens->UseVisualStyleBackColor = true;
            // 
            // splitContainer6
            // 
            this->splitContainer6->Dock = System::Windows::Forms::DockStyle::Fill;
            this->splitContainer6->Location = System::Drawing::Point(0, 0);
            this->splitContainer6->Name = L"splitContainer6";
            this->splitContainer6->Orientation = System::Windows::Forms::Orientation::Horizontal;
            // 
            // splitContainer6.Panel1
            // 
            this->splitContainer6->Panel1->Controls->Add(label10);
            // 
            // splitContainer6.Panel2
            // 
            this->splitContainer6->Panel2->Controls->Add(this->AliensGrid);
            this->splitContainer6->Size = System::Drawing::Size(549, 535);
            this->splitContainer6->SplitterDistance = 30;
            this->splitContainer6->SplitterWidth = 1;
            this->splitContainer6->TabIndex = 2;
            // 
            // label10
            // 
            label10->AutoSize = true;
            label10->Location = System::Drawing::Point(4, 7);
            label10->Name = L"label10";
            label10->Size = System::Drawing::Size(34, 13);
            label10->TabIndex = 0;
            label10->Text = L".........";
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
            this->AliensGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->AliensGrid->Size = System::Drawing::Size(549, 504);
            this->AliensGrid->TabIndex = 0;
            this->AliensGrid->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &Form1::AliensGrid_DataBindingComplete);
            // 
            // TabCommands
            // 
            this->TabCommands->Location = System::Drawing::Point(4, 22);
            this->TabCommands->Name = L"TabCommands";
            this->TabCommands->Size = System::Drawing::Size(549, 535);
            this->TabCommands->TabIndex = 7;
            this->TabCommands->Text = L"Commands";
            this->TabCommands->UseVisualStyleBackColor = true;
            // 
            // TabAbout
            // 
            this->TabAbout->Controls->Add(this->TextAbout);
            this->TabAbout->Location = System::Drawing::Point(4, 22);
            this->TabAbout->Name = L"TabAbout";
            this->TabAbout->Padding = System::Windows::Forms::Padding(3);
            this->TabAbout->Size = System::Drawing::Size(549, 535);
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
            this->TextAbout->Size = System::Drawing::Size(543, 529);
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
            this->ClientSize = System::Drawing::Size(793, 565);
            this->Controls->Add(TopSplitCont);
            this->Name = L"Form1";
            this->Text = L"FarHorizons User Interface";
            this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
            splitContainer1->Panel1->ResumeLayout(false);
            splitContainer1->Panel1->PerformLayout();
            splitContainer1->Panel2->ResumeLayout(false);
            splitContainer1->Panel2->PerformLayout();
            splitContainer1->ResumeLayout(false);
            TopSplitCont->Panel1->ResumeLayout(false);
            TopSplitCont->Panel1->PerformLayout();
            TopSplitCont->Panel2->ResumeLayout(false);
            TopSplitCont->ResumeLayout(false);
            this->MenuTabs->ResumeLayout(false);
            this->TabReports->ResumeLayout(false);
            this->TabSystems->ResumeLayout(false);
            this->splitContainer2->Panel1->ResumeLayout(false);
            this->splitContainer2->Panel1->PerformLayout();
            this->splitContainer2->Panel2->ResumeLayout(false);
            this->splitContainer2->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->EndInit();
            this->TabPlanets->ResumeLayout(false);
            this->splitContainer3->Panel1->ResumeLayout(false);
            this->splitContainer3->Panel1->PerformLayout();
            this->splitContainer3->Panel2->ResumeLayout(false);
            this->splitContainer3->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->PlanetsGrid))->EndInit();
            this->TabColonies->ResumeLayout(false);
            this->splitContainer4->Panel1->ResumeLayout(false);
            this->splitContainer4->Panel1->PerformLayout();
            this->splitContainer4->Panel2->ResumeLayout(false);
            this->splitContainer4->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ColoniesGrid))->EndInit();
            this->TabShips->ResumeLayout(false);
            this->splitContainer5->Panel1->ResumeLayout(false);
            this->splitContainer5->Panel1->PerformLayout();
            this->splitContainer5->Panel2->ResumeLayout(false);
            this->splitContainer5->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipsGrid))->EndInit();
            this->TabAliens->ResumeLayout(false);
            this->splitContainer6->Panel1->ResumeLayout(false);
            this->splitContainer6->Panel1->PerformLayout();
            this->splitContainer6->Panel2->ResumeLayout(false);
            this->splitContainer6->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AliensGrid))->EndInit();
            this->TabAbout->ResumeLayout(false);
            this->TabAbout->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion
private: System::Void RepTurnNr_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             DisplayReport();
         }
private: System::Void SystemsGrid_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
             for each( DataGridViewRow ^row in ((DataGridView^)sender)->Rows )
             {
                 String ^scan = SystemsGetRowTooltip( row );
                 for each( DataGridViewCell ^cell in row->Cells )
                     cell->ToolTipText = scan;
             }
         }
private: System::Void ShipsGrid_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
             for each( DataGridViewRow ^row in ((DataGridView^)sender)->Rows )
             {
                 Color bgColor = ShipsGetRowColor( row );
                 for each( DataGridViewCell ^cell in row->Cells )
                     cell->Style->BackColor = bgColor;
             }
         }
private: System::Void ColoniesGrid_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
             for each( DataGridViewRow ^row in ((DataGridView^)sender)->Rows )
             {
                 Color bgColor = ColoniesGetRowColor( row );
                 for each( DataGridViewCell ^cell in row->Cells )
                     cell->Style->BackColor = bgColor;
             }
         }
private: System::Void AliensGrid_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
             for each( DataGridViewRow ^row in ((DataGridView^)sender)->Rows )
             {
                 Color bgColor = AliensGetRowColor( row );
                 for each( DataGridViewCell ^cell in row->Cells )
                     cell->Style->BackColor = bgColor;
             }
        }
};
}

