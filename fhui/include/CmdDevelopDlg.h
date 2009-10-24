#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace FHUI
{

	/// <summary>
	/// Summary for CmdDevelopDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdDevelopDlg : public System::Windows::Forms::Form
	{
	public:
		CmdDevelopDlg(Colony ^colony, ProdCmdDevelop ^cmd)
		{
			InitializeComponent();

            InidDialog(colony, cmd);
        }

        ProdCmdDevelop^     GetCommand()    { return m_Command; }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdDevelopDlg()
		{
			if (components)
			{
				delete components;
			}
		}

        void        InidDialog(Colony ^colony, ProdCmdDevelop ^cmd);

        String^     GetShipTextEntry(Ship ^ship);

        bool        GenerateCommand(bool validate);

        Colony^         m_Colony;
        ProdCmdDevelop^ m_Command;

    private: System::Windows::Forms::NumericUpDown^  DevAmount;
    private: System::Windows::Forms::ComboBox^  DevShip;
    private: System::Windows::Forms::ComboBox^  DevColony;
    private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
    private: System::Windows::Forms::ComboBox^  comboBox2;
    private: System::Windows::Forms::Label^  InfoColony;
    private: System::Windows::Forms::Label^  InfoBudget;
    private: System::Windows::Forms::Label^  InfoPop;
    private: System::Windows::Forms::Button^  BtnDevelop;
    private: System::Windows::Forms::Button^  BtnCancel;
    private: System::Windows::Forms::Label^  InfoCmd;

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
            this->DevColony = (gcnew System::Windows::Forms::ComboBox());
            this->DevAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->DevShip = (gcnew System::Windows::Forms::ComboBox());
            this->InfoColony = (gcnew System::Windows::Forms::Label());
            this->InfoBudget = (gcnew System::Windows::Forms::Label());
            this->InfoPop = (gcnew System::Windows::Forms::Label());
            this->BtnDevelop = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->InfoCmd = (gcnew System::Windows::Forms::Label());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->DevAmount))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 70);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(46, 13);
            label1->TabIndex = 2;
            label1->Text = L"Amount:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(89, 70);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(42, 13);
            label2->TabIndex = 2;
            label2->Text = L"Colony:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(210, 70);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(55, 13);
            label3->TabIndex = 2;
            label3->Text = L"Transport:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 11);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(99, 13);
            label4->TabIndex = 2;
            label4->Text = L"Develop on colony:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(141, 39);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(60, 13);
            label5->TabIndex = 2;
            label5->Text = L"Population:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(12, 39);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(44, 13);
            label6->TabIndex = 2;
            label6->Text = L"Budget:";
            // 
            // DevColony
            // 
            this->DevColony->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->DevColony->FormattingEnabled = true;
            this->DevColony->Location = System::Drawing::Point(92, 86);
            this->DevColony->Name = L"DevColony";
            this->DevColony->Size = System::Drawing::Size(115, 21);
            this->DevColony->TabIndex = 2;
            this->DevColony->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdDevelopDlg::DevColony_SelectedIndexChanged);
            // 
            // DevAmount
            // 
            this->DevAmount->Location = System::Drawing::Point(15, 86);
            this->DevAmount->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
            this->DevAmount->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->DevAmount->Name = L"DevAmount";
            this->DevAmount->Size = System::Drawing::Size(71, 20);
            this->DevAmount->TabIndex = 1;
            this->DevAmount->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->DevAmount->ValueChanged += gcnew System::EventHandler(this, &CmdDevelopDlg::DevAmount_ValueChanged);
            // 
            // DevShip
            // 
            this->DevShip->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->DevShip->FormattingEnabled = true;
            this->DevShip->Location = System::Drawing::Point(213, 86);
            this->DevShip->Name = L"DevShip";
            this->DevShip->Size = System::Drawing::Size(115, 21);
            this->DevShip->TabIndex = 3;
            this->DevShip->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdDevelopDlg::DevShip_SelectedIndexChanged);
            // 
            // InfoColony
            // 
            this->InfoColony->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoColony->Location = System::Drawing::Point(117, 9);
            this->InfoColony->Name = L"InfoColony";
            this->InfoColony->Size = System::Drawing::Size(100, 17);
            this->InfoColony->TabIndex = 2;
            this->InfoColony->Text = L"0";
            this->InfoColony->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // InfoBudget
            // 
            this->InfoBudget->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoBudget->Location = System::Drawing::Point(64, 37);
            this->InfoBudget->Name = L"InfoBudget";
            this->InfoBudget->Size = System::Drawing::Size(60, 17);
            this->InfoBudget->TabIndex = 2;
            this->InfoBudget->Text = L"0";
            this->InfoBudget->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // InfoPop
            // 
            this->InfoPop->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoPop->Location = System::Drawing::Point(213, 37);
            this->InfoPop->Name = L"InfoPop";
            this->InfoPop->Size = System::Drawing::Size(60, 17);
            this->InfoPop->TabIndex = 2;
            this->InfoPop->Text = L"0";
            this->InfoPop->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // BtnDevelop
            // 
            this->BtnDevelop->Location = System::Drawing::Point(93, 144);
            this->BtnDevelop->Name = L"BtnDevelop";
            this->BtnDevelop->Size = System::Drawing::Size(75, 23);
            this->BtnDevelop->TabIndex = 4;
            this->BtnDevelop->Text = L"Develop";
            this->BtnDevelop->UseVisualStyleBackColor = true;
            this->BtnDevelop->Click += gcnew System::EventHandler(this, &CmdDevelopDlg::BtnDevelop_Click);
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(174, 144);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 5;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // InfoCmd
            // 
            this->InfoCmd->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCmd->Location = System::Drawing::Point(15, 113);
            this->InfoCmd->Name = L"InfoCmd";
            this->InfoCmd->Size = System::Drawing::Size(312, 17);
            this->InfoCmd->TabIndex = 4;
            this->InfoCmd->Text = L"0";
            this->InfoCmd->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // CmdDevelopDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(343, 183);
            this->Controls->Add(this->InfoCmd);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnDevelop);
            this->Controls->Add(label3);
            this->Controls->Add(label2);
            this->Controls->Add(label6);
            this->Controls->Add(label5);
            this->Controls->Add(this->InfoPop);
            this->Controls->Add(this->InfoBudget);
            this->Controls->Add(this->InfoColony);
            this->Controls->Add(label4);
            this->Controls->Add(label1);
            this->Controls->Add(this->DevAmount);
            this->Controls->Add(this->DevShip);
            this->Controls->Add(this->DevColony);
            this->Name = L"CmdDevelopDlg";
            this->Text = L"Develop";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->DevAmount))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void DevColony_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             GenerateCommand(false);
         }
private: System::Void DevAmount_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             GenerateCommand(false);
         }
private: System::Void DevShip_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             GenerateCommand(false);
         }
private: System::Void BtnDevelop_Click(System::Object^  sender, System::EventArgs^  e) {
             if( GenerateCommand(true) )
             {
                 this->DialogResult = System::Windows::Forms::DialogResult::OK;
             }
         }
};
}
