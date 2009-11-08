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

            List<String^> ^items = gcnew List<String^>;
            items->Add( "< Planet Names >" );
            for each( Colony ^colony in GameData::Player->Colonies )
                items->Add( "PL " + colony->Name );
            NamesColonies->DataSource = items;

            items = gcnew List<String^>;
            items->Add( "< Ship Names >" );
            for each( Ship ^ship in GameData::Player->Ships )
                items->Add( ship->PrintClassWithName() );
            NamesShips->DataSource = items;

            items = gcnew List<String^>;
            items->Add( "< Alien Names >" );
            for each( Alien ^alien in GameData::GetAliens() )
                items->Add( "SP " + alien->Name );
            NamesAliens->DataSource = items;
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
    private: System::Windows::Forms::ComboBox^  NamesColonies;
    private: System::Windows::Forms::ComboBox^  NamesShips;
    private: System::Windows::Forms::ComboBox^  NamesAliens;


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
            System::Windows::Forms::Label^  label3;
            this->CmdText = (gcnew System::Windows::Forms::TextBox());
            this->CmdCost = (gcnew System::Windows::Forms::NumericUpDown());
            this->BtnAdd = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->NamesColonies = (gcnew System::Windows::Forms::ComboBox());
            this->NamesShips = (gcnew System::Windows::Forms::ComboBox());
            this->NamesAliens = (gcnew System::Windows::Forms::ComboBox());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CmdCost))->BeginInit();
            this->SuspendLayout();
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
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(13, 71);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(43, 13);
            label3->TabIndex = 2;
            label3->Text = L"Names:";
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
            // BtnAdd
            // 
            this->BtnAdd->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnAdd->Location = System::Drawing::Point(173, 105);
            this->BtnAdd->Name = L"BtnAdd";
            this->BtnAdd->Size = System::Drawing::Size(75, 23);
            this->BtnAdd->TabIndex = 3;
            this->BtnAdd->Text = L"Add";
            this->BtnAdd->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(254, 105);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // NamesColonies
            // 
            this->NamesColonies->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->NamesColonies->FormattingEnabled = true;
            this->NamesColonies->Location = System::Drawing::Point(80, 67);
            this->NamesColonies->MaxDropDownItems = 20;
            this->NamesColonies->Name = L"NamesColonies";
            this->NamesColonies->Size = System::Drawing::Size(131, 21);
            this->NamesColonies->TabIndex = 4;
            this->NamesColonies->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdCustomDlg::CopyName);
            // 
            // NamesShips
            // 
            this->NamesShips->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->NamesShips->FormattingEnabled = true;
            this->NamesShips->Location = System::Drawing::Point(215, 67);
            this->NamesShips->MaxDropDownItems = 20;
            this->NamesShips->Name = L"NamesShips";
            this->NamesShips->Size = System::Drawing::Size(131, 21);
            this->NamesShips->TabIndex = 4;
            this->NamesShips->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdCustomDlg::CopyName);
            // 
            // NamesAliens
            // 
            this->NamesAliens->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->NamesAliens->FormattingEnabled = true;
            this->NamesAliens->Location = System::Drawing::Point(350, 67);
            this->NamesAliens->MaxDropDownItems = 20;
            this->NamesAliens->Name = L"NamesAliens";
            this->NamesAliens->Size = System::Drawing::Size(131, 21);
            this->NamesAliens->TabIndex = 4;
            this->NamesAliens->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdCustomDlg::CopyName);
            // 
            // CmdCustomDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(503, 141);
            this->Controls->Add(this->NamesAliens);
            this->Controls->Add(this->NamesShips);
            this->Controls->Add(this->NamesColonies);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnAdd);
            this->Controls->Add(label2);
            this->Controls->Add(label3);
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
    private: System::Void CopyName(System::Object^  sender, System::EventArgs^  e) {
                 ComboBox ^combo = safe_cast<ComboBox^>(sender);
                 if( combo->SelectedIndex != 0 )
                 {
                     String ^text = combo->Text;
                     CmdText->Text += text;
                     combo->SelectedIndex = 0;
                 }
             }
};
}
