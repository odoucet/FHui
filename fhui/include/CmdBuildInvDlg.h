#pragma once

#include "enums.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace FHUI
{

	/// <summary>
	/// Summary for CmdBuildInvDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdBuildInvDlg : public System::Windows::Forms::Form
	{
	public:
		CmdBuildInvDlg(Colony ^colony, ProdCmdBuildInv ^cmd)
		{
			InitializeComponent();
            InitAvailResources(colony, cmd);
		}

        int                 GetAmount();
        ProdCmdBuildInv^    GetCommand();

    protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdBuildInvDlg()
		{
			if (components)
			{
				delete components;
			}
		}

        void    InitAvailResources(Colony ^colony, ProdCmdBuildInv ^cmd);
        void    UpdateAmounts();
        void    UpdateShipCapacity();

        String^ GetInvString(int i);
        InventoryType GetInv();

        Colony^ m_Colony;
        int     m_AvailEU;
        int     m_AvailPop;
        int     m_Capacity;


    private: System::Windows::Forms::ComboBox^  Target;
    private: System::Windows::Forms::Button^  BtnCancel;
    private: System::Windows::Forms::Button^  BtnBuild;
    private: System::Windows::Forms::NumericUpDown^  Amount;
    private: System::Windows::Forms::Label^  ShipCapacity;
    private: System::Windows::Forms::Label^  TotalCost;
    private: System::Windows::Forms::Label^  InfoColony;
    private: System::Windows::Forms::Label^  AvailEU;
    private: System::Windows::Forms::ComboBox^  Inventory;
    private: System::Windows::Forms::Label^  AvailPop;


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
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::Label^  label7;
            System::Windows::Forms::Label^  label9;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label1;
            this->Target = (gcnew System::Windows::Forms::ComboBox());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->BtnBuild = (gcnew System::Windows::Forms::Button());
            this->Amount = (gcnew System::Windows::Forms::NumericUpDown());
            this->ShipCapacity = (gcnew System::Windows::Forms::Label());
            this->TotalCost = (gcnew System::Windows::Forms::Label());
            this->InfoColony = (gcnew System::Windows::Forms::Label());
            this->AvailEU = (gcnew System::Windows::Forms::Label());
            this->Inventory = (gcnew System::Windows::Forms::ComboBox());
            this->AvailPop = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label1 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Amount))->BeginInit();
            this->SuspendLayout();
            // 
            // label8
            // 
            label8->AutoSize = true;
            label8->Location = System::Drawing::Point(12, 132);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(75, 13);
            label8->TabIndex = 20;
            label8->Text = L"Ship Capacity:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 152);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(57, 13);
            label4->TabIndex = 19;
            label4->Text = L"Total cost:";
            // 
            // label7
            // 
            label7->AutoSize = true;
            label7->Location = System::Drawing::Point(12, 106);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(41, 13);
            label7->TabIndex = 12;
            label7->Text = L"Target:";
            // 
            // label9
            // 
            label9->AutoSize = true;
            label9->Location = System::Drawing::Point(12, 9);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(82, 13);
            label9->TabIndex = 13;
            label9->Text = L"Build on colony:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(12, 78);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(33, 13);
            label6->TabIndex = 9;
            label6->Text = L"Build:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(12, 28);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(71, 13);
            label5->TabIndex = 14;
            label5->Text = L"Available EU:";
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 47);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(106, 13);
            label1->TabIndex = 27;
            label1->Text = L"Available Population:";
            // 
            // Target
            // 
            this->Target->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->Target->FormattingEnabled = true;
            this->Target->Location = System::Drawing::Point(81, 102);
            this->Target->MaxDropDownItems = 20;
            this->Target->Name = L"Target";
            this->Target->Size = System::Drawing::Size(126, 21);
            this->Target->TabIndex = 26;
            this->Target->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdBuildInvDlg::Target_SelectedIndexChanged);
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(167, 178);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 18;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // BtnBuild
            // 
            this->BtnBuild->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnBuild->Location = System::Drawing::Point(86, 178);
            this->BtnBuild->Name = L"BtnBuild";
            this->BtnBuild->Size = System::Drawing::Size(75, 23);
            this->BtnBuild->TabIndex = 15;
            this->BtnBuild->Text = L"Build";
            this->BtnBuild->UseVisualStyleBackColor = true;
            // 
            // Amount
            // 
            this->Amount->Location = System::Drawing::Point(81, 76);
            this->Amount->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
            this->Amount->Name = L"Amount";
            this->Amount->Size = System::Drawing::Size(60, 20);
            this->Amount->TabIndex = 7;
            this->Amount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildInvDlg::Amount_ValueChanged);
            // 
            // ShipCapacity
            // 
            this->ShipCapacity->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->ShipCapacity->Location = System::Drawing::Point(101, 130);
            this->ShipCapacity->Name = L"ShipCapacity";
            this->ShipCapacity->Size = System::Drawing::Size(90, 17);
            this->ShipCapacity->TabIndex = 22;
            this->ShipCapacity->Text = L"0";
            this->ShipCapacity->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // TotalCost
            // 
            this->TotalCost->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->TotalCost->Location = System::Drawing::Point(101, 150);
            this->TotalCost->Name = L"TotalCost";
            this->TotalCost->Size = System::Drawing::Size(90, 17);
            this->TotalCost->TabIndex = 21;
            this->TotalCost->Text = L"0";
            this->TotalCost->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // InfoColony
            // 
            this->InfoColony->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoColony->Location = System::Drawing::Point(120, 7);
            this->InfoColony->Name = L"InfoColony";
            this->InfoColony->Size = System::Drawing::Size(120, 17);
            this->InfoColony->TabIndex = 23;
            this->InfoColony->Text = L"0";
            this->InfoColony->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // AvailEU
            // 
            this->AvailEU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->AvailEU->Location = System::Drawing::Point(120, 26);
            this->AvailEU->Name = L"AvailEU";
            this->AvailEU->Size = System::Drawing::Size(60, 17);
            this->AvailEU->TabIndex = 25;
            this->AvailEU->Text = L"0";
            this->AvailEU->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // Inventory
            // 
            this->Inventory->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->Inventory->FormattingEnabled = true;
            this->Inventory->Location = System::Drawing::Point(146, 75);
            this->Inventory->MaxDropDownItems = 20;
            this->Inventory->Name = L"Inventory";
            this->Inventory->Size = System::Drawing::Size(170, 21);
            this->Inventory->TabIndex = 26;
            this->Inventory->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdBuildInvDlg::Inventory_SelectedIndexChanged);
            // 
            // AvailPop
            // 
            this->AvailPop->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->AvailPop->Location = System::Drawing::Point(120, 45);
            this->AvailPop->Name = L"AvailPop";
            this->AvailPop->Size = System::Drawing::Size(60, 17);
            this->AvailPop->TabIndex = 28;
            this->AvailPop->Text = L"0";
            this->AvailPop->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // CmdBuildInvDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(328, 211);
            this->Controls->Add(this->AvailPop);
            this->Controls->Add(label1);
            this->Controls->Add(this->Inventory);
            this->Controls->Add(this->Target);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnBuild);
            this->Controls->Add(this->Amount);
            this->Controls->Add(this->ShipCapacity);
            this->Controls->Add(this->TotalCost);
            this->Controls->Add(this->InfoColony);
            this->Controls->Add(this->AvailEU);
            this->Controls->Add(label8);
            this->Controls->Add(label4);
            this->Controls->Add(label7);
            this->Controls->Add(label9);
            this->Controls->Add(label6);
            this->Controls->Add(label5);
            this->Name = L"CmdBuildInvDlg";
            this->Text = L"Build Inventory";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Amount))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void Amount_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmounts();
         }
    private: System::Void Inventory_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmounts();
         }
    private: System::Void Target_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateShipCapacity();
         }
};
}
