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
	/// Summary for CmdMessage
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdMessageDlg : public System::Windows::Forms::Form
	{
	public:
		CmdMessageDlg(String ^text)
		{
			InitializeComponent();

            if( text )
                MsgText->Text = text;
        }

        String^ GetMessage() { return MsgText->Text->TrimEnd(); }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdMessageDlg()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::TextBox^  textBox1;
    private: System::Windows::Forms::TextBox^  MsgText;
    private: System::Windows::Forms::Label^  label1;
    private: System::Windows::Forms::Button^  BtnSend;
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
            this->textBox1 = (gcnew System::Windows::Forms::TextBox());
            this->MsgText = (gcnew System::Windows::Forms::TextBox());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->BtnSend = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();
            // 
            // textBox1
            // 
            this->textBox1->Location = System::Drawing::Point(0, 0);
            this->textBox1->Name = L"textBox1";
            this->textBox1->Size = System::Drawing::Size(100, 20);
            this->textBox1->TabIndex = 0;
            // 
            // MsgText
            // 
            this->MsgText->AcceptsReturn = true;
            this->MsgText->Location = System::Drawing::Point(12, 27);
            this->MsgText->Multiline = true;
            this->MsgText->Name = L"MsgText";
            this->MsgText->ScrollBars = System::Windows::Forms::ScrollBars::Both;
            this->MsgText->Size = System::Drawing::Size(429, 176);
            this->MsgText->TabIndex = 0;
            this->MsgText->Text = L"Type your message here...";
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(13, 8);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(73, 13);
            this->label1->TabIndex = 1;
            this->label1->Text = L"Message text:";
            // 
            // BtnSend
            // 
            this->BtnSend->Location = System::Drawing::Point(148, 210);
            this->BtnSend->Name = L"BtnSend";
            this->BtnSend->Size = System::Drawing::Size(75, 23);
            this->BtnSend->TabIndex = 2;
            this->BtnSend->Text = L"Send";
            this->BtnSend->UseVisualStyleBackColor = true;
            this->BtnSend->Click += gcnew System::EventHandler(this, &CmdMessageDlg::BtnSend_Click);
            // 
            // BtnCancel
            // 
            this->BtnCancel->Location = System::Drawing::Point(229, 210);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 2;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            this->BtnCancel->Click += gcnew System::EventHandler(this, &CmdMessageDlg::BtnCancel_Click);
            // 
            // CmdMessage
            // 
            this->ClientSize = System::Drawing::Size(453, 241);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnSend);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->MsgText);
            this->Name = L"CmdMessage";
            this->Text = L"Send Message";
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void BtnSend_Click(System::Object^  sender, System::EventArgs^  e) {
                 for each( String ^line in MsgText->Lines )
                 {
                     if( line->Trim()->ToLower() == "zzz" )
                     {
                         MessageBox::Show(
                             this,
                             "Your message contains 'Zzz' line meaning an end of message.\r\n"
                             "FHUI will add this tag on its own, so please remove this line.",
                             "Message Error",
                             MessageBoxButtons::OK/*Cancel*/,
                             MessageBoxIcon::Error,
                             MessageBoxDefaultButton::Button1);
                         return;
                     }
                 }
                 this->DialogResult = System::Windows::Forms::DialogResult::OK;
                 Close();
             }
    private: System::Void BtnCancel_Click(System::Object^  sender, System::EventArgs^  e) {
                 this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
                 Close();
             }
};
}
