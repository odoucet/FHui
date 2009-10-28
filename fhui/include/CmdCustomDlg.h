#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace FHUI
{

	/// <summary>
	/// Summary for CmdCustomDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdCustomDlg : public System::Windows::Forms::Form
	{
	public:
		CmdCustomDlg(CmdCustom ^cmd)
		{
			InitializeComponent();

            if( cmd )
            {
                CmdText->Text = cmd->m_Command;
                CmdCost->Value = cmd->m_EUCost;
            }
        }

        CmdCustom^  GetCommand(CommandPhase phase)
        {
            String ^cmd = CmdText->Text->Trim();
            if( !String::IsNullOrEmpty(cmd) )
            {
                return gcnew CmdCustom(phase, cmd, Decimal::ToInt32(CmdCost->Value));
            }
            return nullptr;
        }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdCustomDlg()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::TextBox^  CmdText;
    private: System::Windows::Forms::NumericUpDown^  CmdCost;
    private: System::Windows::Forms::Button^  BtnAdd;
    private: System::Windows::Forms::Button^  BtnCancel;
    protected: 

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
            this->CmdText = (gcnew System::Windows::Forms::TextBox());
            this->CmdCost = (gcnew System::Windows::Forms::NumericUpDown());
            this->BtnAdd = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CmdCost))->BeginInit();
            this->SuspendLayout();
            // 
            // CmdText
            // 
            this->CmdText->Location = System::Drawing::Point(80, 13);
            this->CmdText->Name = L"CmdText";
            this->CmdText->Size = System::Drawing::Size(411, 20);
            this->CmdText->TabIndex = 0;
            // 
            // CmdCost
            // 
            this->CmdCost->Location = System::Drawing::Point(80, 40);
            this->CmdCost->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
            this->CmdCost->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, System::Int32::MinValue});
            this->CmdCost->Name = L"CmdCost";
            this->CmdCost->Size = System::Drawing::Size(76, 20);
            this->CmdCost->TabIndex = 1;
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(13, 44);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(49, 13);
            label1->TabIndex = 2;
            label1->Text = L"EU Cost:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(13, 17);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(57, 13);
            label2->TabIndex = 2;
            label2->Text = L"Command:";
            // 
            // BtnAdd
            // 
            this->BtnAdd->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnAdd->Location = System::Drawing::Point(173, 79);
            this->BtnAdd->Name = L"BtnAdd";
            this->BtnAdd->Size = System::Drawing::Size(75, 23);
            this->BtnAdd->TabIndex = 3;
            this->BtnAdd->Text = L"Add";
            this->BtnAdd->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(254, 79);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // CmdCustomDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(503, 114);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnAdd);
            this->Controls->Add(label2);
            this->Controls->Add(label1);
            this->Controls->Add(this->CmdCost);
            this->Controls->Add(this->CmdText);
            this->Name = L"CmdCustomDlg";
            this->Text = L"Custom Command";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CmdCost))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    };
}
