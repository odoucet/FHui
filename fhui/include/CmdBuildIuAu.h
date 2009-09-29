#pragma once

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
		CmdBuildIuAu(int availPop, int availEU)
		{
			InitializeComponent();
            InitAvailResources(availPop, availEU);
		}

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

        void    InitAvailResources(int availPop, int availEU);
        void    UpdateAmounts();

        int     m_AvailPop;
        int     m_AvailEU;

    private: System::Windows::Forms::Label^  AvailPop;
    private: System::Windows::Forms::NumericUpDown^  IUAmount;
    private: System::Windows::Forms::NumericUpDown^  AUAmount;
    private: System::Windows::Forms::Label^  TotalCost;
    private: System::Windows::Forms::Button^  BtnBuild;
    private: System::Windows::Forms::Button^  BtnCancel;
    private: System::Windows::Forms::Label^  AvailEU;

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
            this->AvailPop = (gcnew System::Windows::Forms::Label());
            this->IUAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->AUAmount = (gcnew System::Windows::Forms::NumericUpDown());
            this->TotalCost = (gcnew System::Windows::Forms::Label());
            this->BtnBuild = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->AvailEU = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label1 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IUAmount))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AUAmount))->BeginInit();
            this->SuspendLayout();
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(12, 82);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(51, 13);
            label3->TabIndex = 3;
            label3->Text = L"Build AU:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 106);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(57, 13);
            label4->TabIndex = 4;
            label4->Text = L"Total cost:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(12, 55);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(47, 13);
            label2->TabIndex = 1;
            label2->Text = L"Build IU:";
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 9);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(71, 13);
            label1->TabIndex = 2;
            label1->Text = L"Available CU:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(12, 27);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(71, 13);
            label5->TabIndex = 2;
            label5->Text = L"Available EU:";
            // 
            // AvailPop
            // 
            this->AvailPop->Location = System::Drawing::Point(95, 9);
            this->AvailPop->Name = L"AvailPop";
            this->AvailPop->Size = System::Drawing::Size(60, 13);
            this->AvailPop->TabIndex = 5;
            this->AvailPop->Text = L"0";
            // 
            // IUAmount
            // 
            this->IUAmount->Location = System::Drawing::Point(95, 51);
            this->IUAmount->Name = L"IUAmount";
            this->IUAmount->Size = System::Drawing::Size(60, 20);
            this->IUAmount->TabIndex = 0;
            this->IUAmount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildIuAu::Amount_ValueChanged);
            // 
            // AUAmount
            // 
            this->AUAmount->Location = System::Drawing::Point(95, 78);
            this->AUAmount->Name = L"AUAmount";
            this->AUAmount->Size = System::Drawing::Size(60, 20);
            this->AUAmount->TabIndex = 1;
            this->AUAmount->ValueChanged += gcnew System::EventHandler(this, &CmdBuildIuAu::Amount_ValueChanged);
            // 
            // TotalCost
            // 
            this->TotalCost->Location = System::Drawing::Point(95, 106);
            this->TotalCost->Name = L"TotalCost";
            this->TotalCost->Size = System::Drawing::Size(60, 13);
            this->TotalCost->TabIndex = 5;
            this->TotalCost->Text = L"0";
            // 
            // BtnBuild
            // 
            this->BtnBuild->Location = System::Drawing::Point(12, 132);
            this->BtnBuild->Name = L"BtnBuild";
            this->BtnBuild->Size = System::Drawing::Size(75, 23);
            this->BtnBuild->TabIndex = 2;
            this->BtnBuild->Text = L"Build";
            this->BtnBuild->UseVisualStyleBackColor = true;
            this->BtnBuild->Click += gcnew System::EventHandler(this, &CmdBuildIuAu::BtnBuild_Click);
            // 
            // BtnCancel
            // 
            this->BtnCancel->Location = System::Drawing::Point(93, 132);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            this->BtnCancel->Click += gcnew System::EventHandler(this, &CmdBuildIuAu::BtnCancel_Click);
            // 
            // AvailEU
            // 
            this->AvailEU->Location = System::Drawing::Point(95, 27);
            this->AvailEU->Name = L"AvailEU";
            this->AvailEU->Size = System::Drawing::Size(60, 13);
            this->AvailEU->TabIndex = 5;
            this->AvailEU->Text = L"0";
            // 
            // CmdBuildIuAu
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(197, 164);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnBuild);
            this->Controls->Add(this->AUAmount);
            this->Controls->Add(this->IUAmount);
            this->Controls->Add(this->TotalCost);
            this->Controls->Add(this->AvailEU);
            this->Controls->Add(this->AvailPop);
            this->Controls->Add(label3);
            this->Controls->Add(label4);
            this->Controls->Add(label5);
            this->Controls->Add(label2);
            this->Controls->Add(label1);
            this->Name = L"CmdBuildIuAu";
            this->Text = L"Build IU & AU";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IUAmount))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AUAmount))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void BtnCancel_Click(System::Object^  sender, System::EventArgs^  e) {
             this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
             Close();
         }
private: System::Void BtnBuild_Click(System::Object^  sender, System::EventArgs^  e) {
             this->DialogResult = System::Windows::Forms::DialogResult::OK;
             Close();
         }
private: System::Void Amount_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmounts();
         }
};
}
