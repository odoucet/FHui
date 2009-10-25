#pragma once

#include "enums.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace FHUI {

	/// <summary>
	/// Summary for CmdBuildIuAu
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdBuildIuAu : public System::Windows::Forms::Form
	{
	public:
		CmdBuildIuAu(Colony ^colony, ProdCmdBuildIUAU ^cmd)
		{
			InitializeComponent();
            InitAvailResources(colony, cmd);
		}

        ProdCmdBuildIUAU^   GetCUCommand();
        ProdCmdBuildIUAU^   GetIUCommand();
        ProdCmdBuildIUAU^   GetAUCommand();

        int     GetCUAmount();
        int     GetIUAmount();
        int     GetAUAmount();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdBuildIuAu()
		{
			if (components)
			{
				delete components;
			}
		}

        void    InitAvailResources(Colony ^colony, ProdCmdBuildIUAU ^cmd);
        void    UpdateAmounts();

        ProdCmdBuildIUAU^   GetCommand(int amount, InventoryType inv);

        Colony^ m_Colony;
        int     m_AvailPop;
        int     m_AvailEU;

    private: System::Windows::Forms::Label^  AvailPop;
    private: System::Windows::Forms::NumericUpDown^  IUAmount;
    private: System::Windows::Forms::NumericUpDown^  AUAmount;
    private: System::Windows::Forms::Label^  TotalCost;
    private: System::Windows::Forms::Button^  BtnBuild;
    private: System::Windows::Forms::Button^  BtnCancel;
    private: System::Windows::Forms::NumericUpDown^  CUAmount;
    private: System::Windows::Forms::Label^  AvailEU;
    private: System::Windows::Forms::ComboBox^  Target;
    private: System::Windows::Forms::Label^  InfoColony;




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
            System::Windows::Forms::Label^  label3;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::Label^  label2;
            System::Windows::Forms::Label^  label1;
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label7;
            System::Windows::Forms::Label^  label9;
            this->AvailPop = (gcnew System::Windows::Forms::Label());
            this->IUAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->AUAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->TotalCost = (gcnew System::Windows::Forms::Label());
            this->BtnBuild = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->AvailEU = (gcnew System::Windows::Forms::Label());
            this->CUAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->Target = (gcnew System::Windows::Forms::ComboBox());
            this->InfoColony = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label1 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IUAmount))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AUAmount))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CUAmount))->BeginInit();
            this->SuspendLayout();
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(144, 81);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(51, 13);
            label3->TabIndex = 3;
            label3->Text = L"Build AU:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 156);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(57, 13);
            label4->TabIndex = 4;
            label4->Text = L"Total cost:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(78, 81);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(47, 13);
            label2->TabIndex = 1;
            label2->Text = L"Build IU:";
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 49);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(106, 13);
            label1->TabIndex = 2;
            label1->Text = L"Available Population:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(12, 30);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(71, 13);
            label5->TabIndex = 2;
            label5->Text = L"Available EU:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(12, 81);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(51, 13);
            label6->TabIndex = 1;
            label6->Text = L"Build CU:";
            // 
            // label7
            // 
            label7->AutoSize = true;
            label7->Location = System::Drawing::Point(12, 128);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(41, 13);
            label7->TabIndex = 1;
            label7->Text = L"Target:";
            // 
            // label9
            // 
            label9->AutoSize = true;
            label9->Location = System::Drawing::Point(12, 11);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(82, 13);
            label9->TabIndex = 2;
            label9->Text = L"Build on colony:";
            // 
            // AvailPop
            // 
            this->AvailPop->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->AvailPop->Location = System::Drawing::Point(120, 47);
            this->AvailPop->Name = L"AvailPop";
            this->AvailPop->Size = System::Drawing::Size(60, 17);
            this->AvailPop->TabIndex = 5;
            this->AvailPop->Text = L"0";
            this->AvailPop->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // IUAmount
            // 
            this->IUAmount->Location = System::Drawing::Point(81, 97);
            this->IUAmount->Name = L"IUAmount";
            this->IUAmount->Size = System::Drawing::Size(60, 20);
            this->IUAmount->TabIndex = 0;
            this->IUAmount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildIuAu::Amount_ValueChanged);
            // 
            // AUAmount
            // 
            this->AUAmount->Location = System::Drawing::Point(147, 97);
            this->AUAmount->Name = L"AUAmount";
            this->AUAmount->Size = System::Drawing::Size(60, 20);
            this->AUAmount->TabIndex = 1;
            this->AUAmount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildIuAu::Amount_ValueChanged);
            // 
            // TotalCost
            // 
            this->TotalCost->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->TotalCost->Location = System::Drawing::Point(81, 154);
            this->TotalCost->Name = L"TotalCost";
            this->TotalCost->Size = System::Drawing::Size(60, 17);
            this->TotalCost->TabIndex = 5;
            this->TotalCost->Text = L"0";
            this->TotalCost->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // BtnBuild
            // 
            this->BtnBuild->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnBuild->Location = System::Drawing::Point(33, 187);
            this->BtnBuild->Name = L"BtnBuild";
            this->BtnBuild->Size = System::Drawing::Size(75, 23);
            this->BtnBuild->TabIndex = 2;
            this->BtnBuild->Text = L"Build";
            this->BtnBuild->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(114, 187);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // AvailEU
            // 
            this->AvailEU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->AvailEU->Location = System::Drawing::Point(120, 28);
            this->AvailEU->Name = L"AvailEU";
            this->AvailEU->Size = System::Drawing::Size(60, 17);
            this->AvailEU->TabIndex = 5;
            this->AvailEU->Text = L"0";
            this->AvailEU->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // CUAmount
            // 
            this->CUAmount->Location = System::Drawing::Point(15, 97);
            this->CUAmount->Name = L"CUAmount";
            this->CUAmount->Size = System::Drawing::Size(60, 20);
            this->CUAmount->TabIndex = 0;
            this->CUAmount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildIuAu::Amount_ValueChanged);
            // 
            // Target
            // 
            this->Target->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->Target->FormattingEnabled = true;
            this->Target->Location = System::Drawing::Point(81, 124);
            this->Target->Name = L"Target";
            this->Target->Size = System::Drawing::Size(126, 21);
            this->Target->TabIndex = 6;
            // 
            // InfoColony
            // 
            this->InfoColony->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoColony->Location = System::Drawing::Point(120, 9);
            this->InfoColony->Name = L"InfoColony";
            this->InfoColony->Size = System::Drawing::Size(90, 17);
            this->InfoColony->TabIndex = 5;
            this->InfoColony->Text = L"0";
            this->InfoColony->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // CmdBuildIuAu
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(223, 223);
            this->Controls->Add(this->Target);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnBuild);
            this->Controls->Add(this->AUAmount);
            this->Controls->Add(this->CUAmount);
            this->Controls->Add(this->IUAmount);
            this->Controls->Add(this->TotalCost);
            this->Controls->Add(this->InfoColony);
            this->Controls->Add(this->AvailEU);
            this->Controls->Add(this->AvailPop);
            this->Controls->Add(label3);
            this->Controls->Add(label4);
            this->Controls->Add(label7);
            this->Controls->Add(label9);
            this->Controls->Add(label6);
            this->Controls->Add(label5);
            this->Controls->Add(label2);
            this->Controls->Add(label1);
            this->Name = L"CmdBuildIuAu";
            this->Text = L"Build CU/IU/AU";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IUAmount))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AUAmount))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CUAmount))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void Amount_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmounts();
         }
};
}
