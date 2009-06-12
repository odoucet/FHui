#pragma once

ref class GameData;

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

        void LoadGameData();
        void LoadGalaxy();
        void LoadReports();
        void LoadReport(String ^fileName);
        void LoadCommands();
        void InitData();

        GameData   ^m_GameData;
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

    private: System::Windows::Forms::ComboBox^  RepTourNr;



    private: System::Windows::Forms::ComboBox^  RepMode;
    private: System::Windows::Forms::TextBox^  RepText;
    private: System::Windows::Forms::TextBox^  SystemsGVVal;


    private: System::Windows::Forms::ComboBox^  SystemsRef;

    private: System::Windows::Forms::SplitContainer^  splitContainer2;
    private: System::Windows::Forms::ComboBox^  SystemsShipAge;

    private: System::Windows::Forms::CheckBox^  SystemsGV;
    private: System::Windows::Forms::DataGridView^  SystemsGrid;
















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
            this->RepStatus = (gcnew System::Windows::Forms::Label());
            this->RepMode = (gcnew System::Windows::Forms::ComboBox());
            this->RepTourNr = (gcnew System::Windows::Forms::ComboBox());
            this->RepText = (gcnew System::Windows::Forms::TextBox());
            this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
            this->SystemsGVVal = (gcnew System::Windows::Forms::TextBox());
            this->SystemsRef = (gcnew System::Windows::Forms::ComboBox());
            this->Summary = (gcnew System::Windows::Forms::TextBox());
            this->MenuTabs = (gcnew System::Windows::Forms::TabControl());
            this->TabReports = (gcnew System::Windows::Forms::TabPage());
            this->TabMap = (gcnew System::Windows::Forms::TabPage());
            this->TabSystems = (gcnew System::Windows::Forms::TabPage());
            this->TabPlanets = (gcnew System::Windows::Forms::TabPage());
            this->TabColonies = (gcnew System::Windows::Forms::TabPage());
            this->TabShips = (gcnew System::Windows::Forms::TabPage());
            this->TabAliens = (gcnew System::Windows::Forms::TabPage());
            this->TabCommands = (gcnew System::Windows::Forms::TabPage());
            this->SystemsGV = (gcnew System::Windows::Forms::CheckBox());
            this->SystemsShipAge = (gcnew System::Windows::Forms::ComboBox());
            this->SystemsGrid = (gcnew System::Windows::Forms::DataGridView());
            splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
            label1 = (gcnew System::Windows::Forms::Label());
            TopSplitCont = (gcnew System::Windows::Forms::SplitContainer());
            label2 = (gcnew System::Windows::Forms::Label());
            splitContainer1->Panel1->SuspendLayout();
            splitContainer1->Panel2->SuspendLayout();
            splitContainer1->SuspendLayout();
            this->splitContainer2->Panel1->SuspendLayout();
            this->splitContainer2->Panel2->SuspendLayout();
            this->splitContainer2->SuspendLayout();
            TopSplitCont->Panel1->SuspendLayout();
            TopSplitCont->Panel2->SuspendLayout();
            TopSplitCont->SuspendLayout();
            this->MenuTabs->SuspendLayout();
            this->TabReports->SuspendLayout();
            this->TabSystems->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->BeginInit();
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
            splitContainer1->Panel1->Controls->Add(this->RepTourNr);
            // 
            // splitContainer1.Panel2
            // 
            splitContainer1->Panel2->Controls->Add(this->RepText);
            splitContainer1->Size = System::Drawing::Size(597, 529);
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
            this->RepMode->FormattingEnabled = true;
            this->RepMode->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Commands", L"Reports"});
            this->RepMode->Location = System::Drawing::Point(3, 3);
            this->RepMode->Name = L"RepMode";
            this->RepMode->Size = System::Drawing::Size(101, 21);
            this->RepMode->Sorted = true;
            this->RepMode->TabIndex = 0;
            // 
            // RepTourNr
            // 
            this->RepTourNr->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->RepTourNr->FormattingEnabled = true;
            this->RepTourNr->Location = System::Drawing::Point(110, 3);
            this->RepTourNr->MaxDropDownItems = 12;
            this->RepTourNr->Name = L"RepTourNr";
            this->RepTourNr->Size = System::Drawing::Size(129, 21);
            this->RepTourNr->Sorted = true;
            this->RepTourNr->TabIndex = 0;
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
            this->RepText->Size = System::Drawing::Size(597, 503);
            this->RepText->TabIndex = 0;
            this->RepText->WordWrap = false;
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
            this->splitContainer2->Size = System::Drawing::Size(603, 535);
            this->splitContainer2->SplitterDistance = 30;
            this->splitContainer2->SplitterWidth = 1;
            this->splitContainer2->TabIndex = 0;
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
            TopSplitCont->SplitterDistance = 176;
            TopSplitCont->SplitterWidth = 2;
            TopSplitCont->TabIndex = 0;
            // 
            // Summary
            // 
            this->Summary->Dock = System::Windows::Forms::DockStyle::Fill;
            this->Summary->Font = (gcnew System::Drawing::Font(L"Courier New", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(238)));
            this->Summary->Location = System::Drawing::Point(0, 0);
            this->Summary->Multiline = true;
            this->Summary->Name = L"Summary";
            this->Summary->ReadOnly = true;
            this->Summary->Size = System::Drawing::Size(172, 561);
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
            this->MenuTabs->Dock = System::Windows::Forms::DockStyle::Fill;
            this->MenuTabs->Location = System::Drawing::Point(0, 0);
            this->MenuTabs->Name = L"MenuTabs";
            this->MenuTabs->SelectedIndex = 0;
            this->MenuTabs->Size = System::Drawing::Size(611, 561);
            this->MenuTabs->TabIndex = 0;
            // 
            // TabReports
            // 
            this->TabReports->Controls->Add(splitContainer1);
            this->TabReports->Location = System::Drawing::Point(4, 22);
            this->TabReports->Margin = System::Windows::Forms::Padding(0);
            this->TabReports->Name = L"TabReports";
            this->TabReports->Padding = System::Windows::Forms::Padding(3);
            this->TabReports->Size = System::Drawing::Size(603, 535);
            this->TabReports->TabIndex = 0;
            this->TabReports->Text = L"Reports";
            this->TabReports->UseVisualStyleBackColor = true;
            // 
            // TabMap
            // 
            this->TabMap->Location = System::Drawing::Point(4, 22);
            this->TabMap->Name = L"TabMap";
            this->TabMap->Padding = System::Windows::Forms::Padding(3);
            this->TabMap->Size = System::Drawing::Size(603, 535);
            this->TabMap->TabIndex = 1;
            this->TabMap->Text = L"Map";
            this->TabMap->UseVisualStyleBackColor = true;
            // 
            // TabSystems
            // 
            this->TabSystems->Controls->Add(this->splitContainer2);
            this->TabSystems->Location = System::Drawing::Point(4, 22);
            this->TabSystems->Name = L"TabSystems";
            this->TabSystems->Size = System::Drawing::Size(603, 535);
            this->TabSystems->TabIndex = 2;
            this->TabSystems->Text = L"Systems";
            this->TabSystems->UseVisualStyleBackColor = true;
            // 
            // TabPlanets
            // 
            this->TabPlanets->Location = System::Drawing::Point(4, 22);
            this->TabPlanets->Name = L"TabPlanets";
            this->TabPlanets->Size = System::Drawing::Size(603, 535);
            this->TabPlanets->TabIndex = 3;
            this->TabPlanets->Text = L"Planets";
            this->TabPlanets->UseVisualStyleBackColor = true;
            // 
            // TabColonies
            // 
            this->TabColonies->Location = System::Drawing::Point(4, 22);
            this->TabColonies->Name = L"TabColonies";
            this->TabColonies->Size = System::Drawing::Size(603, 535);
            this->TabColonies->TabIndex = 4;
            this->TabColonies->Text = L"Colonies";
            this->TabColonies->UseVisualStyleBackColor = true;
            // 
            // TabShips
            // 
            this->TabShips->Location = System::Drawing::Point(4, 22);
            this->TabShips->Name = L"TabShips";
            this->TabShips->Size = System::Drawing::Size(603, 535);
            this->TabShips->TabIndex = 5;
            this->TabShips->Text = L"Ships";
            this->TabShips->UseVisualStyleBackColor = true;
            // 
            // TabAliens
            // 
            this->TabAliens->Location = System::Drawing::Point(4, 22);
            this->TabAliens->Name = L"TabAliens";
            this->TabAliens->Size = System::Drawing::Size(603, 535);
            this->TabAliens->TabIndex = 6;
            this->TabAliens->Text = L"Aliens";
            this->TabAliens->UseVisualStyleBackColor = true;
            // 
            // TabCommands
            // 
            this->TabCommands->Location = System::Drawing::Point(4, 22);
            this->TabCommands->Name = L"TabCommands";
            this->TabCommands->Size = System::Drawing::Size(603, 535);
            this->TabCommands->TabIndex = 7;
            this->TabCommands->Text = L"Commands";
            this->TabCommands->UseVisualStyleBackColor = true;
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
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(356, 8);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(53, 13);
            label2->TabIndex = 5;
            label2->Text = L"Ship Age:";
            // 
            // SystemsShipAge
            // 
            this->SystemsShipAge->FormattingEnabled = true;
            this->SystemsShipAge->Location = System::Drawing::Point(415, 5);
            this->SystemsShipAge->Name = L"SystemsShipAge";
            this->SystemsShipAge->Size = System::Drawing::Size(135, 21);
            this->SystemsShipAge->TabIndex = 6;
            // 
            // SystemsGrid
            // 
            this->SystemsGrid->AllowUserToAddRows = false;
            this->SystemsGrid->AllowUserToDeleteRows = false;
            this->SystemsGrid->AllowUserToOrderColumns = true;
            this->SystemsGrid->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
            this->SystemsGrid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->SystemsGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->SystemsGrid->Location = System::Drawing::Point(0, 0);
            this->SystemsGrid->Name = L"SystemsGrid";
            this->SystemsGrid->ReadOnly = true;
            this->SystemsGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->SystemsGrid->Size = System::Drawing::Size(603, 504);
            this->SystemsGrid->TabIndex = 0;
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(793, 565);
            this->Controls->Add(TopSplitCont);
            this->Name = L"Form1";
            this->Text = L"FarHorizons User Interface";
            splitContainer1->Panel1->ResumeLayout(false);
            splitContainer1->Panel1->PerformLayout();
            splitContainer1->Panel2->ResumeLayout(false);
            splitContainer1->Panel2->PerformLayout();
            splitContainer1->ResumeLayout(false);
            this->splitContainer2->Panel1->ResumeLayout(false);
            this->splitContainer2->Panel1->PerformLayout();
            this->splitContainer2->Panel2->ResumeLayout(false);
            this->splitContainer2->ResumeLayout(false);
            TopSplitCont->Panel1->ResumeLayout(false);
            TopSplitCont->Panel1->PerformLayout();
            TopSplitCont->Panel2->ResumeLayout(false);
            TopSplitCont->ResumeLayout(false);
            this->MenuTabs->ResumeLayout(false);
            this->TabReports->ResumeLayout(false);
            this->TabSystems->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SystemsGrid))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion
};
}

