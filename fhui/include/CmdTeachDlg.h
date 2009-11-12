#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "enums.h"

namespace FHUI
{

	/// <summary>
	/// Summary for CmdTeachDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdTeachDlg : public System::Windows::Forms::Form
	{
	public:
		CmdTeachDlg(Alien ^alien, CmdTeach ^cmd)
		{
			InitializeComponent();

            InitGroups();
            InitData(alien, cmd);
        }

        int             GetLevel(TechType tech);
        CmdTeach^       GetCommand(TechType tech);

    protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdTeachDlg()
		{
			if (components)
			{
				delete components;
			}
		}

        void    InitGroups();
        void    InitData(Alien ^alien, CmdTeach ^cmd);

        void    UpdateEnables();

        value struct Group
        {
            CheckBox^       En;
            CheckBox^       EnLimit;
            NumericUpDown^  Limit;
        };
        array<Group>^       m_Groups;
        Alien^              m_Alien;


    private: System::Windows::Forms::CheckBox^  EnMI;
    private: System::Windows::Forms::CheckBox^  EnLimitMI;
    private: System::Windows::Forms::NumericUpDown^  LimitMI;
    protected: 

    protected: 


    private: System::Windows::Forms::CheckBox^  EnMA;
    private: System::Windows::Forms::CheckBox^  EnLimitMA;
    private: System::Windows::Forms::NumericUpDown^  LimitMA;



    private: System::Windows::Forms::CheckBox^  EnML;
    private: System::Windows::Forms::CheckBox^  EnLimitML;
    private: System::Windows::Forms::NumericUpDown^  LimitML;



    private: System::Windows::Forms::CheckBox^  EnGV;
    private: System::Windows::Forms::CheckBox^  EnLimitGV;
    private: System::Windows::Forms::NumericUpDown^  LimitGV;



    private: System::Windows::Forms::CheckBox^  EnLS;
    private: System::Windows::Forms::CheckBox^  EnLimitLS;
    private: System::Windows::Forms::NumericUpDown^  LimitLS;



    private: System::Windows::Forms::CheckBox^  EnBI;
    private: System::Windows::Forms::CheckBox^  EnLimitBI;
    private: System::Windows::Forms::NumericUpDown^  LimitBI;



    private: System::Windows::Forms::Label^  InfoSP;


    private: System::Windows::Forms::Button^  BtnTeach;
    private: System::Windows::Forms::Button^  BtnCancel;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            System::Windows::Forms::Label^  label1;
            this->EnMI = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitMI = (gcnew System::Windows::Forms::CheckBox());
            this->LimitMI = (gcnew System::Windows::Forms::NumericUpDown());
            this->EnMA = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitMA = (gcnew System::Windows::Forms::CheckBox());
            this->LimitMA = (gcnew System::Windows::Forms::NumericUpDown());
            this->EnML = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitML = (gcnew System::Windows::Forms::CheckBox());
            this->LimitML = (gcnew System::Windows::Forms::NumericUpDown());
            this->EnGV = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitGV = (gcnew System::Windows::Forms::CheckBox());
            this->LimitGV = (gcnew System::Windows::Forms::NumericUpDown());
            this->EnLS = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitLS = (gcnew System::Windows::Forms::CheckBox());
            this->LimitLS = (gcnew System::Windows::Forms::NumericUpDown());
            this->EnBI = (gcnew System::Windows::Forms::CheckBox());
            this->EnLimitBI = (gcnew System::Windows::Forms::CheckBox());
            this->LimitBI = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoSP = (gcnew System::Windows::Forms::Label());
            this->BtnTeach = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            label1 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitMI))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitMA))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitML))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitGV))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitLS))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitBI))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(9, 13);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(82, 13);
            label1->TabIndex = 3;
            label1->Text = L"Teach Species:";
            // 
            // EnMI
            // 
            this->EnMI->AutoSize = true;
            this->EnMI->Location = System::Drawing::Point(12, 39);
            this->EnMI->Name = L"EnMI";
            this->EnMI->Size = System::Drawing::Size(57, 17);
            this->EnMI->TabIndex = 0;
            this->EnMI->Text = L"Mining";
            this->EnMI->UseVisualStyleBackColor = true;
            this->EnMI->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitMI
            // 
            this->EnLimitMI->AutoSize = true;
            this->EnLimitMI->Location = System::Drawing::Point(113, 39);
            this->EnLimitMI->Name = L"EnLimitMI";
            this->EnLimitMI->Size = System::Drawing::Size(75, 17);
            this->EnLimitMI->TabIndex = 1;
            this->EnLimitMI->Text = L"Level limit:";
            this->EnLimitMI->UseVisualStyleBackColor = true;
            this->EnLimitMI->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitMI
            // 
            this->LimitMI->Enabled = false;
            this->LimitMI->Location = System::Drawing::Point(189, 37);
            this->LimitMI->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitMI->Name = L"LimitMI";
            this->LimitMI->Size = System::Drawing::Size(56, 20);
            this->LimitMI->TabIndex = 2;
            this->LimitMI->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // EnMA
            // 
            this->EnMA->AutoSize = true;
            this->EnMA->Location = System::Drawing::Point(12, 62);
            this->EnMA->Name = L"EnMA";
            this->EnMA->Size = System::Drawing::Size(94, 17);
            this->EnMA->TabIndex = 0;
            this->EnMA->Text = L"Manufacturing";
            this->EnMA->UseVisualStyleBackColor = true;
            this->EnMA->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitMA
            // 
            this->EnLimitMA->AutoSize = true;
            this->EnLimitMA->Location = System::Drawing::Point(113, 62);
            this->EnLimitMA->Name = L"EnLimitMA";
            this->EnLimitMA->Size = System::Drawing::Size(75, 17);
            this->EnLimitMA->TabIndex = 1;
            this->EnLimitMA->Text = L"Level limit:";
            this->EnLimitMA->UseVisualStyleBackColor = true;
            this->EnLimitMA->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitMA
            // 
            this->LimitMA->Enabled = false;
            this->LimitMA->Location = System::Drawing::Point(189, 60);
            this->LimitMA->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitMA->Name = L"LimitMA";
            this->LimitMA->Size = System::Drawing::Size(56, 20);
            this->LimitMA->TabIndex = 2;
            this->LimitMA->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // EnML
            // 
            this->EnML->AutoSize = true;
            this->EnML->Location = System::Drawing::Point(12, 85);
            this->EnML->Name = L"EnML";
            this->EnML->Size = System::Drawing::Size(58, 17);
            this->EnML->TabIndex = 0;
            this->EnML->Text = L"Military";
            this->EnML->UseVisualStyleBackColor = true;
            this->EnML->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitML
            // 
            this->EnLimitML->AutoSize = true;
            this->EnLimitML->Location = System::Drawing::Point(113, 85);
            this->EnLimitML->Name = L"EnLimitML";
            this->EnLimitML->Size = System::Drawing::Size(75, 17);
            this->EnLimitML->TabIndex = 1;
            this->EnLimitML->Text = L"Level limit:";
            this->EnLimitML->UseVisualStyleBackColor = true;
            this->EnLimitML->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitML
            // 
            this->LimitML->Enabled = false;
            this->LimitML->Location = System::Drawing::Point(189, 83);
            this->LimitML->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitML->Name = L"LimitML";
            this->LimitML->Size = System::Drawing::Size(56, 20);
            this->LimitML->TabIndex = 2;
            this->LimitML->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // EnGV
            // 
            this->EnGV->AutoSize = true;
            this->EnGV->Location = System::Drawing::Point(12, 108);
            this->EnGV->Name = L"EnGV";
            this->EnGV->Size = System::Drawing::Size(67, 17);
            this->EnGV->TabIndex = 0;
            this->EnGV->Text = L"Gravitics";
            this->EnGV->UseVisualStyleBackColor = true;
            this->EnGV->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitGV
            // 
            this->EnLimitGV->AutoSize = true;
            this->EnLimitGV->Location = System::Drawing::Point(113, 108);
            this->EnLimitGV->Name = L"EnLimitGV";
            this->EnLimitGV->Size = System::Drawing::Size(75, 17);
            this->EnLimitGV->TabIndex = 1;
            this->EnLimitGV->Text = L"Level limit:";
            this->EnLimitGV->UseVisualStyleBackColor = true;
            this->EnLimitGV->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitGV
            // 
            this->LimitGV->Enabled = false;
            this->LimitGV->Location = System::Drawing::Point(189, 106);
            this->LimitGV->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitGV->Name = L"LimitGV";
            this->LimitGV->Size = System::Drawing::Size(56, 20);
            this->LimitGV->TabIndex = 2;
            this->LimitGV->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // EnLS
            // 
            this->EnLS->AutoSize = true;
            this->EnLS->Location = System::Drawing::Point(12, 131);
            this->EnLS->Name = L"EnLS";
            this->EnLS->Size = System::Drawing::Size(86, 17);
            this->EnLS->TabIndex = 0;
            this->EnLS->Text = L"Live Support";
            this->EnLS->UseVisualStyleBackColor = true;
            this->EnLS->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitLS
            // 
            this->EnLimitLS->AutoSize = true;
            this->EnLimitLS->Location = System::Drawing::Point(113, 131);
            this->EnLimitLS->Name = L"EnLimitLS";
            this->EnLimitLS->Size = System::Drawing::Size(75, 17);
            this->EnLimitLS->TabIndex = 1;
            this->EnLimitLS->Text = L"Level limit:";
            this->EnLimitLS->UseVisualStyleBackColor = true;
            this->EnLimitLS->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitLS
            // 
            this->LimitLS->Enabled = false;
            this->LimitLS->Location = System::Drawing::Point(189, 129);
            this->LimitLS->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitLS->Name = L"LimitLS";
            this->LimitLS->Size = System::Drawing::Size(56, 20);
            this->LimitLS->TabIndex = 2;
            this->LimitLS->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // EnBI
            // 
            this->EnBI->AutoSize = true;
            this->EnBI->Location = System::Drawing::Point(12, 154);
            this->EnBI->Name = L"EnBI";
            this->EnBI->Size = System::Drawing::Size(60, 17);
            this->EnBI->TabIndex = 0;
            this->EnBI->Text = L"Biology";
            this->EnBI->UseVisualStyleBackColor = true;
            this->EnBI->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // EnLimitBI
            // 
            this->EnLimitBI->AutoSize = true;
            this->EnLimitBI->Location = System::Drawing::Point(113, 154);
            this->EnLimitBI->Name = L"EnLimitBI";
            this->EnLimitBI->Size = System::Drawing::Size(75, 17);
            this->EnLimitBI->TabIndex = 1;
            this->EnLimitBI->Text = L"Level limit:";
            this->EnLimitBI->UseVisualStyleBackColor = true;
            this->EnLimitBI->CheckedChanged += gcnew System::EventHandler(this, &CmdTeachDlg::UpdateEnables);
            // 
            // LimitBI
            // 
            this->LimitBI->Enabled = false;
            this->LimitBI->Location = System::Drawing::Point(189, 152);
            this->LimitBI->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->LimitBI->Name = L"LimitBI";
            this->LimitBI->Size = System::Drawing::Size(56, 20);
            this->LimitBI->TabIndex = 2;
            this->LimitBI->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            // 
            // InfoSP
            // 
            this->InfoSP->AutoSize = true;
            this->InfoSP->Location = System::Drawing::Point(96, 13);
            this->InfoSP->Name = L"InfoSP";
            this->InfoSP->Size = System::Drawing::Size(13, 13);
            this->InfoSP->TabIndex = 3;
            this->InfoSP->Text = L"\?";
            // 
            // BtnTeach
            // 
            this->BtnTeach->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnTeach->Location = System::Drawing::Point(50, 190);
            this->BtnTeach->Name = L"BtnTeach";
            this->BtnTeach->Size = System::Drawing::Size(75, 23);
            this->BtnTeach->TabIndex = 4;
            this->BtnTeach->Text = L"Teach";
            this->BtnTeach->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(134, 190);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 4;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // CmdTeachDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(259, 223);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnTeach);
            this->Controls->Add(this->InfoSP);
            this->Controls->Add(label1);
            this->Controls->Add(this->LimitBI);
            this->Controls->Add(this->LimitLS);
            this->Controls->Add(this->LimitGV);
            this->Controls->Add(this->LimitML);
            this->Controls->Add(this->LimitMA);
            this->Controls->Add(this->LimitMI);
            this->Controls->Add(this->EnLimitBI);
            this->Controls->Add(this->EnLimitLS);
            this->Controls->Add(this->EnLimitGV);
            this->Controls->Add(this->EnLimitML);
            this->Controls->Add(this->EnLimitMA);
            this->Controls->Add(this->EnLimitMI);
            this->Controls->Add(this->EnBI);
            this->Controls->Add(this->EnLS);
            this->Controls->Add(this->EnGV);
            this->Controls->Add(this->EnML);
            this->Controls->Add(this->EnMA);
            this->Controls->Add(this->EnMI);
            this->Name = L"CmdTeachDlg";
            this->Text = L"Teach";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitMI))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitMA))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitML))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitGV))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitLS))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LimitBI))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void UpdateEnables(System::Object^  sender, System::EventArgs^  e) {
                 UpdateEnables();
             }
};
}
