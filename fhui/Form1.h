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
    private: System::Windows::Forms::SplitContainer^  TopSplitCont;
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
    private: System::Windows::Forms::DataGridView^  GridSystems;


    private: System::Windows::Forms::DataGridViewTextBoxColumn^  ColX;
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
            this->RepStatus = (gcnew System::Windows::Forms::Label());
            this->RepMode = (gcnew System::Windows::Forms::ComboBox());
            this->RepTourNr = (gcnew System::Windows::Forms::ComboBox());
            this->RepText = (gcnew System::Windows::Forms::TextBox());
            this->TopSplitCont = (gcnew System::Windows::Forms::SplitContainer());
            this->Summary = (gcnew System::Windows::Forms::TextBox());
            this->MenuTabs = (gcnew System::Windows::Forms::TabControl());
            this->TabReports = (gcnew System::Windows::Forms::TabPage());
            this->TabMap = (gcnew System::Windows::Forms::TabPage());
            this->TabSystems = (gcnew System::Windows::Forms::TabPage());
            this->GridSystems = (gcnew System::Windows::Forms::DataGridView());
            this->ColX = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
            this->TabPlanets = (gcnew System::Windows::Forms::TabPage());
            this->TabColonies = (gcnew System::Windows::Forms::TabPage());
            this->TabShips = (gcnew System::Windows::Forms::TabPage());
            this->TabAliens = (gcnew System::Windows::Forms::TabPage());
            this->TabCommands = (gcnew System::Windows::Forms::TabPage());
            splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
            splitContainer1->Panel1->SuspendLayout();
            splitContainer1->Panel2->SuspendLayout();
            splitContainer1->SuspendLayout();
            this->TopSplitCont->Panel1->SuspendLayout();
            this->TopSplitCont->Panel2->SuspendLayout();
            this->TopSplitCont->SuspendLayout();
            this->MenuTabs->SuspendLayout();
            this->TabReports->SuspendLayout();
            this->TabSystems->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->GridSystems))->BeginInit();
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
            splitContainer1->Size = System::Drawing::Size(610, 568);
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
            this->RepText->Size = System::Drawing::Size(610, 542);
            this->RepText->TabIndex = 0;
            this->RepText->WordWrap = false;
            // 
            // TopSplitCont
            // 
            this->TopSplitCont->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->TopSplitCont->Dock = System::Windows::Forms::DockStyle::Fill;
            this->TopSplitCont->Location = System::Drawing::Point(0, 0);
            this->TopSplitCont->Margin = System::Windows::Forms::Padding(0);
            this->TopSplitCont->Name = L"TopSplitCont";
            // 
            // TopSplitCont.Panel1
            // 
            this->TopSplitCont->Panel1->AutoScroll = true;
            this->TopSplitCont->Panel1->Controls->Add(this->Summary);
            // 
            // TopSplitCont.Panel2
            // 
            this->TopSplitCont->Panel2->Controls->Add(this->MenuTabs);
            this->TopSplitCont->Size = System::Drawing::Size(811, 604);
            this->TopSplitCont->SplitterDistance = 181;
            this->TopSplitCont->SplitterWidth = 2;
            this->TopSplitCont->TabIndex = 0;
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
            this->Summary->Size = System::Drawing::Size(177, 600);
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
            this->MenuTabs->Size = System::Drawing::Size(624, 600);
            this->MenuTabs->TabIndex = 0;
            // 
            // TabReports
            // 
            this->TabReports->Controls->Add(splitContainer1);
            this->TabReports->Location = System::Drawing::Point(4, 22);
            this->TabReports->Margin = System::Windows::Forms::Padding(0);
            this->TabReports->Name = L"TabReports";
            this->TabReports->Padding = System::Windows::Forms::Padding(3);
            this->TabReports->Size = System::Drawing::Size(616, 574);
            this->TabReports->TabIndex = 0;
            this->TabReports->Text = L"Reports";
            this->TabReports->UseVisualStyleBackColor = true;
            // 
            // TabMap
            // 
            this->TabMap->Location = System::Drawing::Point(4, 22);
            this->TabMap->Name = L"TabMap";
            this->TabMap->Padding = System::Windows::Forms::Padding(3);
            this->TabMap->Size = System::Drawing::Size(616, 574);
            this->TabMap->TabIndex = 1;
            this->TabMap->Text = L"Map";
            this->TabMap->UseVisualStyleBackColor = true;
            // 
            // TabSystems
            // 
            this->TabSystems->Controls->Add(this->GridSystems);
            this->TabSystems->Location = System::Drawing::Point(4, 22);
            this->TabSystems->Name = L"TabSystems";
            this->TabSystems->Size = System::Drawing::Size(616, 574);
            this->TabSystems->TabIndex = 2;
            this->TabSystems->Text = L"Systems";
            this->TabSystems->UseVisualStyleBackColor = true;
            // 
            // GridSystems
            // 
            this->GridSystems->AllowUserToAddRows = false;
            this->GridSystems->AllowUserToDeleteRows = false;
            this->GridSystems->AllowUserToOrderColumns = true;
            this->GridSystems->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->GridSystems->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) {this->ColX});
            this->GridSystems->Dock = System::Windows::Forms::DockStyle::Fill;
            this->GridSystems->Location = System::Drawing::Point(0, 0);
            this->GridSystems->Name = L"GridSystems";
            this->GridSystems->ReadOnly = true;
            this->GridSystems->Size = System::Drawing::Size(616, 574);
            this->GridSystems->TabIndex = 0;
            // 
            // ColX
            // 
            this->ColX->HeaderText = L"X";
            this->ColX->Name = L"ColX";
            this->ColX->ReadOnly = true;
            // 
            // TabPlanets
            // 
            this->TabPlanets->Location = System::Drawing::Point(4, 22);
            this->TabPlanets->Name = L"TabPlanets";
            this->TabPlanets->Size = System::Drawing::Size(616, 574);
            this->TabPlanets->TabIndex = 3;
            this->TabPlanets->Text = L"Planets";
            this->TabPlanets->UseVisualStyleBackColor = true;
            // 
            // TabColonies
            // 
            this->TabColonies->Location = System::Drawing::Point(4, 22);
            this->TabColonies->Name = L"TabColonies";
            this->TabColonies->Size = System::Drawing::Size(616, 574);
            this->TabColonies->TabIndex = 4;
            this->TabColonies->Text = L"Colonies";
            this->TabColonies->UseVisualStyleBackColor = true;
            // 
            // TabShips
            // 
            this->TabShips->Location = System::Drawing::Point(4, 22);
            this->TabShips->Name = L"TabShips";
            this->TabShips->Size = System::Drawing::Size(616, 574);
            this->TabShips->TabIndex = 5;
            this->TabShips->Text = L"Ships";
            this->TabShips->UseVisualStyleBackColor = true;
            // 
            // TabAliens
            // 
            this->TabAliens->Location = System::Drawing::Point(4, 22);
            this->TabAliens->Name = L"TabAliens";
            this->TabAliens->Size = System::Drawing::Size(616, 574);
            this->TabAliens->TabIndex = 6;
            this->TabAliens->Text = L"Aliens";
            this->TabAliens->UseVisualStyleBackColor = true;
            // 
            // TabCommands
            // 
            this->TabCommands->Location = System::Drawing::Point(4, 22);
            this->TabCommands->Name = L"TabCommands";
            this->TabCommands->Size = System::Drawing::Size(616, 574);
            this->TabCommands->TabIndex = 7;
            this->TabCommands->Text = L"Commands";
            this->TabCommands->UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(811, 604);
            this->Controls->Add(this->TopSplitCont);
            this->Name = L"Form1";
            this->Text = L"FarHorizons User Interface";
            splitContainer1->Panel1->ResumeLayout(false);
            splitContainer1->Panel1->PerformLayout();
            splitContainer1->Panel2->ResumeLayout(false);
            splitContainer1->Panel2->PerformLayout();
            splitContainer1->ResumeLayout(false);
            this->TopSplitCont->Panel1->ResumeLayout(false);
            this->TopSplitCont->Panel1->PerformLayout();
            this->TopSplitCont->Panel2->ResumeLayout(false);
            this->TopSplitCont->ResumeLayout(false);
            this->MenuTabs->ResumeLayout(false);
            this->TabReports->ResumeLayout(false);
            this->TabSystems->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->GridSystems))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion
    };
}

