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
	/// Summary for CmdInstallDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdInstallDlg : public System::Windows::Forms::Form
	{
	public:
		CmdInstallDlg(Colony ^colony, CmdInstall ^cmd)
		{
			InitializeComponent();

            m_Colony = colony;

            InfoColony->Text = colony->Name;
            InfoMD->Text = (colony->MiDiff / 100.0).ToString("F2");
            InfoMABase->Text = (Math::Max(0, colony->MaBase) / 10.0).ToString("F2");
            InfoMIBase->Text = (Math::Max(0, colony->MiBase) / 10.0).ToString("F2");
            InfoCU->Text = colony->Res->Inventory[INV_CU].ToString();
            InfoIU->Text = colony->Res->Inventory[INV_IU].ToString();
            InfoAU->Text = colony->Res->Inventory[INV_AU].ToString();

            if( cmd )
            {
                if( cmd->m_Unit == INV_IU )
                {
                    InstIU->Value = cmd->m_Amount;
                    InstAU->Enabled = false;
                }
                else
                {
                    InstAU->Value = cmd->m_Amount;
                    InstIU->Enabled = false;
                }
            }
        }

        int GetAmount(InventoryType inv)
        {
            if( inv == INV_IU )
                return Decimal::ToInt32(InstIU->Value);
            if( inv == INV_AU )
                return Decimal::ToInt32(InstAU->Value);
            throw gcnew FHUIDataImplException("Invalid inventory for Install command!");
        }

        CmdInstall^ GetCommand(InventoryType inv)
        {
            if( inv == INV_IU )
            {
                int iu = Decimal::ToInt32(InstIU->Value);
                if( iu > 0 )
                    return gcnew CmdInstall(iu, INV_IU, m_Colony);
                return nullptr;
            }
            if( inv == INV_AU )
            {
                int au = Decimal::ToInt32(InstAU->Value);
                if( au > 0 )
                    return gcnew CmdInstall(au, INV_AU, m_Colony);
                return nullptr;
            }
            throw gcnew FHUIDataImplException("Invalid inventory for Install command!");
        }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdInstallDlg()
		{
			if (components)
			{
				delete components;
			}
		}

        Colony ^m_Colony;

    private: System::Windows::Forms::NumericUpDown^  InstIU;
    private: System::Windows::Forms::NumericUpDown^  InstAU;
    private: System::Windows::Forms::Label^  InfoColony;
    private: System::Windows::Forms::Label^  InfoMD;
    private: System::Windows::Forms::Label^  InfoCU;
    private: System::Windows::Forms::Label^  InfoIU;
    private: System::Windows::Forms::Label^  InfoAU;
    private: System::Windows::Forms::Label^  InfoMABase;
    private: System::Windows::Forms::Label^  InfoMIBase;
    private: System::Windows::Forms::Button^  BtnInstall;
    private: System::Windows::Forms::Button^  BtnCancel;

    protected: 

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
            System::Windows::Forms::Label^  label2;
            System::Windows::Forms::Label^  label3;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label7;
            System::Windows::Forms::Label^  label15;
            this->InstIU = (gcnew System::Windows::Forms::NumericUpDown());
            this->InstAU = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoColony = (gcnew System::Windows::Forms::Label());
            this->InfoMD = (gcnew System::Windows::Forms::Label());
            this->InfoCU = (gcnew System::Windows::Forms::Label());
            this->InfoIU = (gcnew System::Windows::Forms::Label());
            this->InfoAU = (gcnew System::Windows::Forms::Label());
            this->InfoMABase = (gcnew System::Windows::Forms::Label());
            this->InfoMIBase = (gcnew System::Windows::Forms::Label());
            this->BtnInstall = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            label15 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InstIU))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InstAU))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(13, 11);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(42, 13);
            label1->TabIndex = 0;
            label1->Text = L"Colony:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(12, 33);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(27, 13);
            label2->TabIndex = 0;
            label2->Text = L"MD:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(12, 55);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(25, 13);
            label3->TabIndex = 0;
            label3->Text = L"CU:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(114, 33);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(49, 13);
            label4->TabIndex = 0;
            label4->Text = L"MI Base:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(133, 55);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(21, 13);
            label5->TabIndex = 0;
            label5->Text = L"IU:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(223, 33);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(53, 13);
            label6->TabIndex = 0;
            label6->Text = L"MA Base:";
            // 
            // label7
            // 
            label7->AutoSize = true;
            label7->Location = System::Drawing::Point(246, 55);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(25, 13);
            label7->TabIndex = 0;
            label7->Text = L"AU:";
            // 
            // label15
            // 
            label15->AutoSize = true;
            label15->Location = System::Drawing::Point(57, 85);
            label15->Name = L"label15";
            label15->Size = System::Drawing::Size(64, 13);
            label15->TabIndex = 0;
            label15->Text = L"Install Units:";
            // 
            // InstIU
            // 
            this->InstIU->Location = System::Drawing::Point(133, 81);
            this->InstIU->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
            this->InstIU->Name = L"InstIU";
            this->InstIU->Size = System::Drawing::Size(75, 20);
            this->InstIU->TabIndex = 1;
            this->InstIU->ValueChanged += gcnew System::EventHandler(this, &CmdInstallDlg::UpdateAmounts);
            // 
            // InstAU
            // 
            this->InstAU->Location = System::Drawing::Point(246, 81);
            this->InstAU->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
            this->InstAU->Name = L"InstAU";
            this->InstAU->Size = System::Drawing::Size(75, 20);
            this->InstAU->TabIndex = 1;
            this->InstAU->ValueChanged += gcnew System::EventHandler(this, &CmdInstallDlg::UpdateAmounts);
            // 
            // InfoColony
            // 
            this->InfoColony->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoColony->Location = System::Drawing::Point(58, 9);
            this->InfoColony->Name = L"InfoColony";
            this->InfoColony->Size = System::Drawing::Size(150, 17);
            this->InfoColony->TabIndex = 0;
            this->InfoColony->Text = L"0";
            // 
            // InfoMD
            // 
            this->InfoMD->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoMD->Location = System::Drawing::Point(57, 31);
            this->InfoMD->Name = L"InfoMD";
            this->InfoMD->Size = System::Drawing::Size(42, 17);
            this->InfoMD->TabIndex = 0;
            this->InfoMD->Text = L"0";
            // 
            // InfoCU
            // 
            this->InfoCU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCU->Location = System::Drawing::Point(57, 53);
            this->InfoCU->Name = L"InfoCU";
            this->InfoCU->Size = System::Drawing::Size(42, 17);
            this->InfoCU->TabIndex = 0;
            this->InfoCU->Text = L"0";
            // 
            // InfoIU
            // 
            this->InfoIU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoIU->Location = System::Drawing::Point(166, 53);
            this->InfoIU->Name = L"InfoIU";
            this->InfoIU->Size = System::Drawing::Size(42, 17);
            this->InfoIU->TabIndex = 0;
            this->InfoIU->Text = L"0";
            // 
            // InfoAU
            // 
            this->InfoAU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoAU->Location = System::Drawing::Point(279, 53);
            this->InfoAU->Name = L"InfoAU";
            this->InfoAU->Size = System::Drawing::Size(42, 17);
            this->InfoAU->TabIndex = 0;
            this->InfoAU->Text = L"0";
            // 
            // InfoMABase
            // 
            this->InfoMABase->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoMABase->Location = System::Drawing::Point(279, 31);
            this->InfoMABase->Name = L"InfoMABase";
            this->InfoMABase->Size = System::Drawing::Size(42, 17);
            this->InfoMABase->TabIndex = 0;
            this->InfoMABase->Text = L"0";
            // 
            // InfoMIBase
            // 
            this->InfoMIBase->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoMIBase->Location = System::Drawing::Point(166, 31);
            this->InfoMIBase->Name = L"InfoMIBase";
            this->InfoMIBase->Size = System::Drawing::Size(42, 17);
            this->InfoMIBase->TabIndex = 0;
            this->InfoMIBase->Text = L"0";
            // 
            // BtnInstall
            // 
            this->BtnInstall->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnInstall->Location = System::Drawing::Point(90, 114);
            this->BtnInstall->Name = L"BtnInstall";
            this->BtnInstall->Size = System::Drawing::Size(75, 23);
            this->BtnInstall->TabIndex = 2;
            this->BtnInstall->Text = L"Install";
            this->BtnInstall->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(170, 114);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 2;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // CmdInstallDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(337, 147);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnInstall);
            this->Controls->Add(this->InstAU);
            this->Controls->Add(this->InstIU);
            this->Controls->Add(label7);
            this->Controls->Add(label6);
            this->Controls->Add(label5);
            this->Controls->Add(label4);
            this->Controls->Add(label15);
            this->Controls->Add(label3);
            this->Controls->Add(label2);
            this->Controls->Add(this->InfoMIBase);
            this->Controls->Add(this->InfoMABase);
            this->Controls->Add(this->InfoAU);
            this->Controls->Add(this->InfoIU);
            this->Controls->Add(this->InfoCU);
            this->Controls->Add(this->InfoMD);
            this->Controls->Add(this->InfoColony);
            this->Controls->Add(label1);
            this->Name = L"CmdInstallDlg";
            this->Text = L"Install";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InstIU))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->InstAU))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void UpdateAmounts(System::Object^  sender, System::EventArgs^  e) {
             int iu = Decimal::ToInt32(InstIU->Value);
             int au = Decimal::ToInt32(InstAU->Value);
             int cu = m_Colony->Res->Inventory[INV_CU];
             int iuNew = Math::Min( Math::Min(iu, m_Colony->Res->Inventory[INV_IU]), cu );
             int auNew = Math::Min( Math::Min(au, m_Colony->Res->Inventory[INV_AU]), cu - iuNew );
             if( iuNew != iu )
                 InstIU->Value = iuNew;
             if( auNew != au )
                 InstAU->Value = auNew;
         }
};
}
