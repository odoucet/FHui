#pragma once

#include "enums.h"

namespace FHUI
{

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

	/// <summary>
	/// Summary for CmdResearch
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdResearch : public System::Windows::Forms::Form
	{
	public:
		CmdResearch(void)
		{
			InitializeComponent();
            InitGroups();
		}

        int     GetAmount(TechType tech);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdResearch()
		{
			if (components)
			{
				delete components;
			}
		}

    protected: 
        void    InitGroups();

        void    UpdateEnable(Object ^sender);
        void    UpdateAmount();
        void    Calc(Object ^sender);


        value struct Group
        {
            CheckBox^       En;
            NumericUpDown^  Amount;
            Button^         CalcAvg;
            Button^         CalcGtd;
            Button^         CalcGui;
            NumericUpDown^  LvlFrom;
            NumericUpDown^  LvlTo;
        };
        array<Group>^       m_Groups;


    private: System::Windows::Forms::Label^  AmountTotal;
    private: System::Windows::Forms::NumericUpDown^  AmountMI;
    private: System::Windows::Forms::NumericUpDown^  AmountMA;
    private: System::Windows::Forms::NumericUpDown^  AmountML;
    private: System::Windows::Forms::NumericUpDown^  AmountGV;
    private: System::Windows::Forms::NumericUpDown^  AmountLS;
    private: System::Windows::Forms::NumericUpDown^  AmountBI;
    private: System::Windows::Forms::CheckBox^  EnMI;
    private: System::Windows::Forms::CheckBox^  EnMA;
    private: System::Windows::Forms::CheckBox^  EnML;
    private: System::Windows::Forms::CheckBox^  EnGV;
    private: System::Windows::Forms::CheckBox^  EnLS;
    private: System::Windows::Forms::CheckBox^  EnBI;
    private: System::Windows::Forms::Button^  BtnCancel;
    private: System::Windows::Forms::Button^  BtnResearch;
    private: System::Windows::Forms::ToolTip^  Tooltip;
    private: System::Windows::Forms::NumericUpDown^  CalcLvlFromMI;
    private: System::Windows::Forms::NumericUpDown^  CalcLvlToMI;
    private: System::Windows::Forms::Button^  CalcAvgMI;
    private: System::Windows::Forms::Button^  CalcGtdMI;
    private: System::Windows::Forms::Button^  CalcGuiMI;
    private: System::Windows::Forms::NumericUpDown^  CalcLvlFromMA;
    private: System::Windows::Forms::NumericUpDown^  CalcLvlToMA;
    private: System::Windows::Forms::Button^  CalcAvgMA;
    private: System::Windows::Forms::Button^  CalcGtdMA;
private: System::Windows::Forms::Button^  CalcGuiMA;
private: System::Windows::Forms::NumericUpDown^  CalcLvlFromML;
private: System::Windows::Forms::NumericUpDown^  CalcLvlToML;
private: System::Windows::Forms::Button^  CalcAvgML;
private: System::Windows::Forms::Button^  CalcGtdML;
private: System::Windows::Forms::Button^  CalcGuiML;
private: System::Windows::Forms::NumericUpDown^  CalcLvlFromGV;
private: System::Windows::Forms::NumericUpDown^  CalcLvlToGV;
private: System::Windows::Forms::Button^  CalcAvgGV;
private: System::Windows::Forms::Button^  CalcGtdGV;
private: System::Windows::Forms::Button^  CalcGuiGV;
private: System::Windows::Forms::NumericUpDown^  CalcLvlFromLS;
private: System::Windows::Forms::NumericUpDown^  CalcLvlToLS;
private: System::Windows::Forms::Button^  CalcAvgLS;
private: System::Windows::Forms::Button^  CalcGtdLS;
private: System::Windows::Forms::Button^  CalcGuiLS;
private: System::Windows::Forms::NumericUpDown^  CalcLvlFromBI;
private: System::Windows::Forms::NumericUpDown^  CalcLvlToBI;
private: System::Windows::Forms::Button^  CalcAvgBI;
private: System::Windows::Forms::Button^  CalcGtdBI;
private: System::Windows::Forms::Button^  CalcGuiBI;
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
            System::Windows::Forms::Label^  label1;
            System::Windows::Forms::Label^  label2;
            System::Windows::Forms::Label^  label3;
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label31;
            this->AmountMI = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountMA = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountML = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountLS = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountBI = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountTotal = (gcnew System::Windows::Forms::Label());
            this->EnMI = (gcnew System::Windows::Forms::CheckBox());
            this->EnMA = (gcnew System::Windows::Forms::CheckBox());
            this->EnML = (gcnew System::Windows::Forms::CheckBox());
            this->EnGV = (gcnew System::Windows::Forms::CheckBox());
            this->EnLS = (gcnew System::Windows::Forms::CheckBox());
            this->EnBI = (gcnew System::Windows::Forms::CheckBox());
            this->BtnResearch = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->CalcLvlFromMI = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToMI = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgMI = (gcnew System::Windows::Forms::Button());
            this->CalcGtdMI = (gcnew System::Windows::Forms::Button());
            this->CalcGuiMI = (gcnew System::Windows::Forms::Button());
            this->Tooltip = (gcnew System::Windows::Forms::ToolTip(this->components));
            this->CalcLvlFromMA = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToMA = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgMA = (gcnew System::Windows::Forms::Button());
            this->CalcGtdMA = (gcnew System::Windows::Forms::Button());
            this->CalcGuiMA = (gcnew System::Windows::Forms::Button());
            this->CalcLvlFromML = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToML = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgML = (gcnew System::Windows::Forms::Button());
            this->CalcGtdML = (gcnew System::Windows::Forms::Button());
            this->CalcGuiML = (gcnew System::Windows::Forms::Button());
            this->CalcLvlFromGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgGV = (gcnew System::Windows::Forms::Button());
            this->CalcGtdGV = (gcnew System::Windows::Forms::Button());
            this->CalcGuiGV = (gcnew System::Windows::Forms::Button());
            this->CalcLvlFromLS = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToLS = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgLS = (gcnew System::Windows::Forms::Button());
            this->CalcGtdLS = (gcnew System::Windows::Forms::Button());
            this->CalcGuiLS = (gcnew System::Windows::Forms::Button());
            this->CalcLvlFromBI = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcLvlToBI = (gcnew System::Windows::Forms::NumericUpDown());
            this->CalcAvgBI = (gcnew System::Windows::Forms::Button());
            this->CalcGtdBI = (gcnew System::Windows::Forms::Button());
            this->CalcGuiBI = (gcnew System::Windows::Forms::Button());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label31 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountMI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountMA))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountML))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGV))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountLS))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountBI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromMI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToMI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromMA))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToMA))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromML))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToML))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromGV))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToGV))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromLS))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToLS))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromBI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToBI))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 9);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(66, 13);
            label1->TabIndex = 0;
            label1->Text = L"Technology:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(192, 9);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(54, 13);
            label2->TabIndex = 0;
            label2->Text = L"Calculate:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(108, 9);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(46, 13);
            label3->TabIndex = 0;
            label3->Text = L"Amount:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(317, 9);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(46, 13);
            label5->TabIndex = 0;
            label5->Text = L"From lvl:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(374, 9);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(36, 13);
            label6->TabIndex = 0;
            label6->Text = L"To lvl:";
            // 
            // label31
            // 
            label31->AutoSize = true;
            label31->Location = System::Drawing::Point(71, 192);
            label31->Name = L"label31";
            label31->Size = System::Drawing::Size(34, 13);
            label31->TabIndex = 0;
            label31->Text = L"Total:";
            // 
            // AmountMI
            // 
            this->AmountMI->Enabled = false;
            this->AmountMI->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountMI->Location = System::Drawing::Point(111, 30);
            this->AmountMI->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountMI->Name = L"AmountMI";
            this->AmountMI->Size = System::Drawing::Size(66, 20);
            this->AmountMI->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountMI, L"Amount to spend");
            this->AmountMI->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountMA
            // 
            this->AmountMA->Enabled = false;
            this->AmountMA->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountMA->Location = System::Drawing::Point(111, 56);
            this->AmountMA->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountMA->Name = L"AmountMA";
            this->AmountMA->Size = System::Drawing::Size(66, 20);
            this->AmountMA->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountMA, L"Amount to spend");
            this->AmountMA->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountML
            // 
            this->AmountML->Enabled = false;
            this->AmountML->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountML->Location = System::Drawing::Point(111, 82);
            this->AmountML->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountML->Name = L"AmountML";
            this->AmountML->Size = System::Drawing::Size(66, 20);
            this->AmountML->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountML, L"Amount to spend");
            this->AmountML->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountGV
            // 
            this->AmountGV->Enabled = false;
            this->AmountGV->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountGV->Location = System::Drawing::Point(111, 108);
            this->AmountGV->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountGV->Name = L"AmountGV";
            this->AmountGV->Size = System::Drawing::Size(66, 20);
            this->AmountGV->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountGV, L"Amount to spend");
            this->AmountGV->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountLS
            // 
            this->AmountLS->Enabled = false;
            this->AmountLS->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountLS->Location = System::Drawing::Point(111, 134);
            this->AmountLS->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountLS->Name = L"AmountLS";
            this->AmountLS->Size = System::Drawing::Size(66, 20);
            this->AmountLS->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountLS, L"Amount to spend");
            this->AmountLS->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountBI
            // 
            this->AmountBI->Enabled = false;
            this->AmountBI->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
            this->AmountBI->Location = System::Drawing::Point(111, 160);
            this->AmountBI->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000000000, 0, 0, 0});
            this->AmountBI->Name = L"AmountBI";
            this->AmountBI->Size = System::Drawing::Size(66, 20);
            this->AmountBI->TabIndex = 1;
            this->Tooltip->SetToolTip(this->AmountBI, L"Amount to spend");
            this->AmountBI->ValueChanged += gcnew System::EventHandler(this, &CmdResearch::Amount_ValueChanged);
            // 
            // AmountTotal
            // 
            this->AmountTotal->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->AmountTotal->Location = System::Drawing::Point(111, 188);
            this->AmountTotal->Name = L"AmountTotal";
            this->AmountTotal->Size = System::Drawing::Size(66, 20);
            this->AmountTotal->TabIndex = 0;
            this->AmountTotal->Text = L"0";
            this->AmountTotal->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            this->Tooltip->SetToolTip(this->AmountTotal, L"Total amount spent on research");
            // 
            // EnMI
            // 
            this->EnMI->AutoSize = true;
            this->EnMI->Location = System::Drawing::Point(15, 31);
            this->EnMI->Name = L"EnMI";
            this->EnMI->Size = System::Drawing::Size(57, 17);
            this->EnMI->TabIndex = 2;
            this->EnMI->Text = L"Mining";
            this->Tooltip->SetToolTip(this->EnMI, L"Enable this research");
            this->EnMI->UseVisualStyleBackColor = true;
            this->EnMI->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // EnMA
            // 
            this->EnMA->AutoSize = true;
            this->EnMA->Location = System::Drawing::Point(15, 57);
            this->EnMA->Name = L"EnMA";
            this->EnMA->Size = System::Drawing::Size(94, 17);
            this->EnMA->TabIndex = 2;
            this->EnMA->Text = L"Manufacturing";
            this->Tooltip->SetToolTip(this->EnMA, L"Enable this research");
            this->EnMA->UseVisualStyleBackColor = true;
            this->EnMA->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // EnML
            // 
            this->EnML->AutoSize = true;
            this->EnML->Location = System::Drawing::Point(15, 83);
            this->EnML->Name = L"EnML";
            this->EnML->Size = System::Drawing::Size(58, 17);
            this->EnML->TabIndex = 2;
            this->EnML->Text = L"Military";
            this->Tooltip->SetToolTip(this->EnML, L"Enable this research");
            this->EnML->UseVisualStyleBackColor = true;
            this->EnML->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // EnGV
            // 
            this->EnGV->AutoSize = true;
            this->EnGV->Location = System::Drawing::Point(15, 109);
            this->EnGV->Name = L"EnGV";
            this->EnGV->Size = System::Drawing::Size(67, 17);
            this->EnGV->TabIndex = 2;
            this->EnGV->Text = L"Gravitics";
            this->Tooltip->SetToolTip(this->EnGV, L"Enable this research");
            this->EnGV->UseVisualStyleBackColor = true;
            this->EnGV->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // EnLS
            // 
            this->EnLS->AutoSize = true;
            this->EnLS->Location = System::Drawing::Point(15, 135);
            this->EnLS->Name = L"EnLS";
            this->EnLS->Size = System::Drawing::Size(83, 17);
            this->EnLS->TabIndex = 2;
            this->EnLS->Text = L"Life Support";
            this->Tooltip->SetToolTip(this->EnLS, L"Enable this research");
            this->EnLS->UseVisualStyleBackColor = true;
            this->EnLS->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // EnBI
            // 
            this->EnBI->AutoSize = true;
            this->EnBI->Location = System::Drawing::Point(15, 161);
            this->EnBI->Name = L"EnBI";
            this->EnBI->Size = System::Drawing::Size(60, 17);
            this->EnBI->TabIndex = 2;
            this->EnBI->Text = L"Biology";
            this->Tooltip->SetToolTip(this->EnBI, L"Enable this research");
            this->EnBI->UseVisualStyleBackColor = true;
            this->EnBI->CheckedChanged += gcnew System::EventHandler(this, &CmdResearch::En_CheckedChanged);
            // 
            // BtnResearch
            // 
            this->BtnResearch->Location = System::Drawing::Point(142, 217);
            this->BtnResearch->Name = L"BtnResearch";
            this->BtnResearch->Size = System::Drawing::Size(75, 23);
            this->BtnResearch->TabIndex = 3;
            this->BtnResearch->Text = L"Research";
            this->Tooltip->SetToolTip(this->BtnResearch, L"Apply Research command(s)");
            this->BtnResearch->UseVisualStyleBackColor = true;
            this->BtnResearch->Click += gcnew System::EventHandler(this, &CmdResearch::BtnResearch_Click);
            // 
            // BtnCancel
            // 
            this->BtnCancel->Location = System::Drawing::Point(226, 217);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            this->BtnCancel->Click += gcnew System::EventHandler(this, &CmdResearch::BtnCancel_Click);
            // 
            // CalcLvlFromMI
            // 
            this->CalcLvlFromMI->Enabled = false;
            this->CalcLvlFromMI->Location = System::Drawing::Point(320, 30);
            this->CalcLvlFromMI->Name = L"CalcLvlFromMI";
            this->CalcLvlFromMI->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromMI->TabIndex = 1;
            this->Tooltip->SetToolTip(this->CalcLvlFromMI, L"Calculate from this level");
            // 
            // CalcLvlToMI
            // 
            this->CalcLvlToMI->Enabled = false;
            this->CalcLvlToMI->Location = System::Drawing::Point(377, 30);
            this->CalcLvlToMI->Name = L"CalcLvlToMI";
            this->CalcLvlToMI->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToMI->TabIndex = 1;
            this->Tooltip->SetToolTip(this->CalcLvlToMI, L"Calculato to this level");
            // 
            // CalcAvgMI
            // 
            this->CalcAvgMI->Enabled = false;
            this->CalcAvgMI->Location = System::Drawing::Point(195, 28);
            this->CalcAvgMI->Name = L"CalcAvgMI";
            this->CalcAvgMI->Size = System::Drawing::Size(35, 22);
            this->CalcAvgMI->TabIndex = 3;
            this->CalcAvgMI->Text = L"Avg";
            this->Tooltip->SetToolTip(this->CalcAvgMI, L"Calculate average research cost");
            this->CalcAvgMI->UseVisualStyleBackColor = true;
            this->CalcAvgMI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdMI
            // 
            this->CalcGtdMI->Enabled = false;
            this->CalcGtdMI->Location = System::Drawing::Point(231, 28);
            this->CalcGtdMI->Name = L"CalcGtdMI";
            this->CalcGtdMI->Size = System::Drawing::Size(35, 22);
            this->CalcGtdMI->TabIndex = 3;
            this->CalcGtdMI->Text = L"Gtd";
            this->Tooltip->SetToolTip(this->CalcGtdMI, L"Calculate guaranteed research cost");
            this->CalcGtdMI->UseVisualStyleBackColor = true;
            this->CalcGtdMI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiMI
            // 
            this->CalcGuiMI->Enabled = false;
            this->CalcGuiMI->Location = System::Drawing::Point(267, 28);
            this->CalcGuiMI->Name = L"CalcGuiMI";
            this->CalcGuiMI->Size = System::Drawing::Size(35, 22);
            this->CalcGuiMI->TabIndex = 3;
            this->CalcGuiMI->Text = L"Gui";
            this->Tooltip->SetToolTip(this->CalcGuiMI, L"Calculate guided research cost");
            this->CalcGuiMI->UseVisualStyleBackColor = true;
            this->CalcGuiMI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcLvlFromMA
            // 
            this->CalcLvlFromMA->Enabled = false;
            this->CalcLvlFromMA->Location = System::Drawing::Point(320, 55);
            this->CalcLvlFromMA->Name = L"CalcLvlFromMA";
            this->CalcLvlFromMA->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromMA->TabIndex = 1;
            // 
            // CalcLvlToMA
            // 
            this->CalcLvlToMA->Enabled = false;
            this->CalcLvlToMA->Location = System::Drawing::Point(377, 55);
            this->CalcLvlToMA->Name = L"CalcLvlToMA";
            this->CalcLvlToMA->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToMA->TabIndex = 1;
            // 
            // CalcAvgMA
            // 
            this->CalcAvgMA->Enabled = false;
            this->CalcAvgMA->Location = System::Drawing::Point(195, 54);
            this->CalcAvgMA->Name = L"CalcAvgMA";
            this->CalcAvgMA->Size = System::Drawing::Size(35, 22);
            this->CalcAvgMA->TabIndex = 3;
            this->CalcAvgMA->Text = L"Avg";
            this->CalcAvgMA->UseVisualStyleBackColor = true;
            this->CalcAvgMA->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdMA
            // 
            this->CalcGtdMA->Enabled = false;
            this->CalcGtdMA->Location = System::Drawing::Point(231, 54);
            this->CalcGtdMA->Name = L"CalcGtdMA";
            this->CalcGtdMA->Size = System::Drawing::Size(35, 22);
            this->CalcGtdMA->TabIndex = 3;
            this->CalcGtdMA->Text = L"Gtd";
            this->CalcGtdMA->UseVisualStyleBackColor = true;
            this->CalcGtdMA->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiMA
            // 
            this->CalcGuiMA->Enabled = false;
            this->CalcGuiMA->Location = System::Drawing::Point(267, 54);
            this->CalcGuiMA->Name = L"CalcGuiMA";
            this->CalcGuiMA->Size = System::Drawing::Size(35, 22);
            this->CalcGuiMA->TabIndex = 3;
            this->CalcGuiMA->Text = L"Gui";
            this->CalcGuiMA->UseVisualStyleBackColor = true;
            this->CalcGuiMA->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcLvlFromML
            // 
            this->CalcLvlFromML->Enabled = false;
            this->CalcLvlFromML->Location = System::Drawing::Point(320, 81);
            this->CalcLvlFromML->Name = L"CalcLvlFromML";
            this->CalcLvlFromML->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromML->TabIndex = 1;
            // 
            // CalcLvlToML
            // 
            this->CalcLvlToML->Enabled = false;
            this->CalcLvlToML->Location = System::Drawing::Point(377, 81);
            this->CalcLvlToML->Name = L"CalcLvlToML";
            this->CalcLvlToML->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToML->TabIndex = 1;
            // 
            // CalcAvgML
            // 
            this->CalcAvgML->Enabled = false;
            this->CalcAvgML->Location = System::Drawing::Point(195, 80);
            this->CalcAvgML->Name = L"CalcAvgML";
            this->CalcAvgML->Size = System::Drawing::Size(35, 22);
            this->CalcAvgML->TabIndex = 3;
            this->CalcAvgML->Text = L"Avg";
            this->CalcAvgML->UseVisualStyleBackColor = true;
            this->CalcAvgML->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdML
            // 
            this->CalcGtdML->Enabled = false;
            this->CalcGtdML->Location = System::Drawing::Point(231, 80);
            this->CalcGtdML->Name = L"CalcGtdML";
            this->CalcGtdML->Size = System::Drawing::Size(35, 22);
            this->CalcGtdML->TabIndex = 3;
            this->CalcGtdML->Text = L"Gtd";
            this->CalcGtdML->UseVisualStyleBackColor = true;
            this->CalcGtdML->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiML
            // 
            this->CalcGuiML->Enabled = false;
            this->CalcGuiML->Location = System::Drawing::Point(267, 80);
            this->CalcGuiML->Name = L"CalcGuiML";
            this->CalcGuiML->Size = System::Drawing::Size(35, 22);
            this->CalcGuiML->TabIndex = 3;
            this->CalcGuiML->Text = L"Gui";
            this->CalcGuiML->UseVisualStyleBackColor = true;
            this->CalcGuiML->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcLvlFromGV
            // 
            this->CalcLvlFromGV->Enabled = false;
            this->CalcLvlFromGV->Location = System::Drawing::Point(320, 107);
            this->CalcLvlFromGV->Name = L"CalcLvlFromGV";
            this->CalcLvlFromGV->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromGV->TabIndex = 1;
            // 
            // CalcLvlToGV
            // 
            this->CalcLvlToGV->Enabled = false;
            this->CalcLvlToGV->Location = System::Drawing::Point(377, 107);
            this->CalcLvlToGV->Name = L"CalcLvlToGV";
            this->CalcLvlToGV->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToGV->TabIndex = 1;
            // 
            // CalcAvgGV
            // 
            this->CalcAvgGV->Enabled = false;
            this->CalcAvgGV->Location = System::Drawing::Point(195, 106);
            this->CalcAvgGV->Name = L"CalcAvgGV";
            this->CalcAvgGV->Size = System::Drawing::Size(35, 22);
            this->CalcAvgGV->TabIndex = 3;
            this->CalcAvgGV->Text = L"Avg";
            this->CalcAvgGV->UseVisualStyleBackColor = true;
            this->CalcAvgGV->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdGV
            // 
            this->CalcGtdGV->Enabled = false;
            this->CalcGtdGV->Location = System::Drawing::Point(231, 106);
            this->CalcGtdGV->Name = L"CalcGtdGV";
            this->CalcGtdGV->Size = System::Drawing::Size(35, 22);
            this->CalcGtdGV->TabIndex = 3;
            this->CalcGtdGV->Text = L"Gtd";
            this->CalcGtdGV->UseVisualStyleBackColor = true;
            this->CalcGtdGV->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiGV
            // 
            this->CalcGuiGV->Enabled = false;
            this->CalcGuiGV->Location = System::Drawing::Point(267, 106);
            this->CalcGuiGV->Name = L"CalcGuiGV";
            this->CalcGuiGV->Size = System::Drawing::Size(35, 22);
            this->CalcGuiGV->TabIndex = 3;
            this->CalcGuiGV->Text = L"Gui";
            this->CalcGuiGV->UseVisualStyleBackColor = true;
            this->CalcGuiGV->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcLvlFromLS
            // 
            this->CalcLvlFromLS->Enabled = false;
            this->CalcLvlFromLS->Location = System::Drawing::Point(320, 133);
            this->CalcLvlFromLS->Name = L"CalcLvlFromLS";
            this->CalcLvlFromLS->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromLS->TabIndex = 1;
            // 
            // CalcLvlToLS
            // 
            this->CalcLvlToLS->Enabled = false;
            this->CalcLvlToLS->Location = System::Drawing::Point(377, 133);
            this->CalcLvlToLS->Name = L"CalcLvlToLS";
            this->CalcLvlToLS->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToLS->TabIndex = 1;
            // 
            // CalcAvgLS
            // 
            this->CalcAvgLS->Enabled = false;
            this->CalcAvgLS->Location = System::Drawing::Point(195, 132);
            this->CalcAvgLS->Name = L"CalcAvgLS";
            this->CalcAvgLS->Size = System::Drawing::Size(35, 22);
            this->CalcAvgLS->TabIndex = 3;
            this->CalcAvgLS->Text = L"Avg";
            this->CalcAvgLS->UseVisualStyleBackColor = true;
            this->CalcAvgLS->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdLS
            // 
            this->CalcGtdLS->Enabled = false;
            this->CalcGtdLS->Location = System::Drawing::Point(231, 132);
            this->CalcGtdLS->Name = L"CalcGtdLS";
            this->CalcGtdLS->Size = System::Drawing::Size(35, 22);
            this->CalcGtdLS->TabIndex = 3;
            this->CalcGtdLS->Text = L"Gtd";
            this->CalcGtdLS->UseVisualStyleBackColor = true;
            this->CalcGtdLS->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiLS
            // 
            this->CalcGuiLS->Enabled = false;
            this->CalcGuiLS->Location = System::Drawing::Point(267, 132);
            this->CalcGuiLS->Name = L"CalcGuiLS";
            this->CalcGuiLS->Size = System::Drawing::Size(35, 22);
            this->CalcGuiLS->TabIndex = 3;
            this->CalcGuiLS->Text = L"Gui";
            this->CalcGuiLS->UseVisualStyleBackColor = true;
            this->CalcGuiLS->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcLvlFromBI
            // 
            this->CalcLvlFromBI->Enabled = false;
            this->CalcLvlFromBI->Location = System::Drawing::Point(320, 159);
            this->CalcLvlFromBI->Name = L"CalcLvlFromBI";
            this->CalcLvlFromBI->Size = System::Drawing::Size(48, 20);
            this->CalcLvlFromBI->TabIndex = 1;
            // 
            // CalcLvlToBI
            // 
            this->CalcLvlToBI->Enabled = false;
            this->CalcLvlToBI->Location = System::Drawing::Point(377, 159);
            this->CalcLvlToBI->Name = L"CalcLvlToBI";
            this->CalcLvlToBI->Size = System::Drawing::Size(48, 20);
            this->CalcLvlToBI->TabIndex = 1;
            // 
            // CalcAvgBI
            // 
            this->CalcAvgBI->Enabled = false;
            this->CalcAvgBI->Location = System::Drawing::Point(195, 158);
            this->CalcAvgBI->Name = L"CalcAvgBI";
            this->CalcAvgBI->Size = System::Drawing::Size(35, 22);
            this->CalcAvgBI->TabIndex = 3;
            this->CalcAvgBI->Text = L"Avg";
            this->CalcAvgBI->UseVisualStyleBackColor = true;
            this->CalcAvgBI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGtdBI
            // 
            this->CalcGtdBI->Enabled = false;
            this->CalcGtdBI->Location = System::Drawing::Point(231, 158);
            this->CalcGtdBI->Name = L"CalcGtdBI";
            this->CalcGtdBI->Size = System::Drawing::Size(35, 22);
            this->CalcGtdBI->TabIndex = 3;
            this->CalcGtdBI->Text = L"Gtd";
            this->CalcGtdBI->UseVisualStyleBackColor = true;
            this->CalcGtdBI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CalcGuiBI
            // 
            this->CalcGuiBI->Enabled = false;
            this->CalcGuiBI->Location = System::Drawing::Point(267, 158);
            this->CalcGuiBI->Name = L"CalcGuiBI";
            this->CalcGuiBI->Size = System::Drawing::Size(35, 22);
            this->CalcGuiBI->TabIndex = 3;
            this->CalcGuiBI->Text = L"Gui";
            this->CalcGuiBI->UseVisualStyleBackColor = true;
            this->CalcGuiBI->Click += gcnew System::EventHandler(this, &CmdResearch::Calc_Click);
            // 
            // CmdResearch
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(442, 251);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->CalcGuiBI);
            this->Controls->Add(this->CalcGuiLS);
            this->Controls->Add(this->CalcGuiGV);
            this->Controls->Add(this->CalcGuiML);
            this->Controls->Add(this->CalcGuiMA);
            this->Controls->Add(this->CalcGuiMI);
            this->Controls->Add(this->CalcGtdBI);
            this->Controls->Add(this->CalcGtdLS);
            this->Controls->Add(this->CalcGtdGV);
            this->Controls->Add(this->CalcGtdML);
            this->Controls->Add(this->CalcGtdMA);
            this->Controls->Add(this->CalcGtdMI);
            this->Controls->Add(this->CalcAvgBI);
            this->Controls->Add(this->CalcAvgLS);
            this->Controls->Add(this->CalcAvgGV);
            this->Controls->Add(this->CalcAvgML);
            this->Controls->Add(this->CalcAvgMA);
            this->Controls->Add(this->CalcAvgMI);
            this->Controls->Add(this->BtnResearch);
            this->Controls->Add(this->EnBI);
            this->Controls->Add(this->CalcLvlToBI);
            this->Controls->Add(this->EnLS);
            this->Controls->Add(this->CalcLvlToLS);
            this->Controls->Add(this->EnGV);
            this->Controls->Add(this->CalcLvlToGV);
            this->Controls->Add(this->EnML);
            this->Controls->Add(this->CalcLvlFromBI);
            this->Controls->Add(this->CalcLvlToML);
            this->Controls->Add(this->CalcLvlFromLS);
            this->Controls->Add(this->EnMA);
            this->Controls->Add(this->CalcLvlFromGV);
            this->Controls->Add(this->CalcLvlToMA);
            this->Controls->Add(this->CalcLvlFromML);
            this->Controls->Add(this->EnMI);
            this->Controls->Add(this->CalcLvlFromMA);
            this->Controls->Add(this->CalcLvlToMI);
            this->Controls->Add(this->CalcLvlFromMI);
            this->Controls->Add(this->AmountBI);
            this->Controls->Add(this->AmountLS);
            this->Controls->Add(this->AmountGV);
            this->Controls->Add(this->AmountML);
            this->Controls->Add(this->AmountMA);
            this->Controls->Add(this->AmountMI);
            this->Controls->Add(label3);
            this->Controls->Add(label6);
            this->Controls->Add(label5);
            this->Controls->Add(this->AmountTotal);
            this->Controls->Add(label2);
            this->Controls->Add(label31);
            this->Controls->Add(label1);
            this->Name = L"CmdResearch";
            this->Text = L"CmdResearch";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountMI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountMA))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountML))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGV))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountLS))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountBI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromMI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToMI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromMA))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToMA))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromML))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToML))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromGV))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToGV))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromLS))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToLS))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlFromBI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CalcLvlToBI))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void En_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateEnable(sender);
         }
private: System::Void Amount_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmount();
         }
private: System::Void Calc_Click(System::Object^  sender, System::EventArgs^  e) {
             Calc(sender);
         }
private: System::Void BtnCancel_Click(System::Object^  sender, System::EventArgs^  e) {
             this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
             Close();
         }
private: System::Void BtnResearch_Click(System::Object^  sender, System::EventArgs^  e) {
             this->DialogResult = System::Windows::Forms::DialogResult::OK;
             Close();
         }
};

} // end namespace FHUI
