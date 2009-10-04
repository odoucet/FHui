#pragma once

namespace FHUI
{

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

	/// <summary>
	/// Summary for CmdBuildShips
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdBuildShips : public System::Windows::Forms::Form
	{
	public:
		CmdBuildShips(int availEU, int availMA)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
            m_AvailEU = availEU;
            m_AvailMA = availMA;
            InitControls();
		}

        ICommand^   CreateCommand();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdBuildShips()
		{
			if (components)
			{
				delete components;
			}
		}

        void    InitControls();
        void    Build();
        void    DoPresetScout();
        void    DoPresetTransport();
        void    DoPresetWarship();
        void    ShipUpdate();

        int     m_AvailEU;
        int     m_AvailMA;

    private: System::Windows::Forms::CheckBox^  ShipSublight;
    protected: 


    private: System::Windows::Forms::TextBox^  ShipName;
    private: System::Windows::Forms::ComboBox^  ShipClass;
    private: System::Windows::Forms::NumericUpDown^  ShipSize;



    private: System::Windows::Forms::Button^  PresetScout;
    private: System::Windows::Forms::Button^  PresetTransport;
    private: System::Windows::Forms::Button^  PresetWarship;
    private: System::Windows::Forms::Button^  BtnBuild;
    private: System::Windows::Forms::Button^  BtnCancel;





    private: System::Windows::Forms::Label^  InfoCost;
    private: System::Windows::Forms::Label^  InfoUpkeep;
    private: System::Windows::Forms::Label^  InfoTonnage;



    private: System::Windows::Forms::Label^  InfoAvailMA;

    private: System::Windows::Forms::Label^  InfoAvailEU;


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
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::Label^  label9;
            this->ShipSublight = (gcnew System::Windows::Forms::CheckBox());
            this->ShipName = (gcnew System::Windows::Forms::TextBox());
            this->ShipClass = (gcnew System::Windows::Forms::ComboBox());
            this->ShipSize = (gcnew System::Windows::Forms::NumericUpDown());
            this->PresetScout = (gcnew System::Windows::Forms::Button());
            this->PresetTransport = (gcnew System::Windows::Forms::Button());
            this->PresetWarship = (gcnew System::Windows::Forms::Button());
            this->BtnBuild = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->InfoCost = (gcnew System::Windows::Forms::Label());
            this->InfoUpkeep = (gcnew System::Windows::Forms::Label());
            this->InfoTonnage = (gcnew System::Windows::Forms::Label());
            this->InfoAvailMA = (gcnew System::Windows::Forms::Label());
            this->InfoAvailEU = (gcnew System::Windows::Forms::Label());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipSize))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 11);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(90, 13);
            label1->TabIndex = 6;
            label1->Text = L"Available Budget:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(207, 11);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(72, 13);
            label2->TabIndex = 6;
            label2->Text = L"Available MA:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(12, 43);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(40, 13);
            label3->TabIndex = 6;
            label3->Text = L"Preset:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 70);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(38, 13);
            label4->TabIndex = 6;
            label4->Text = L"Name:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(12, 97);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(35, 13);
            label5->TabIndex = 6;
            label5->Text = L"Class:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(12, 125);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(30, 13);
            label6->TabIndex = 6;
            label6->Text = L"Size:";
            // 
            // label7
            // 
            label7->AutoSize = true;
            label7->Location = System::Drawing::Point(262, 43);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(57, 13);
            label7->TabIndex = 6;
            label7->Text = L"Build Cost:";
            // 
            // label8
            // 
            label8->AutoSize = true;
            label8->Location = System::Drawing::Point(262, 70);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(48, 13);
            label8->TabIndex = 6;
            label8->Text = L"Upkeep:";
            // 
            // label9
            // 
            label9->AutoSize = true;
            label9->Location = System::Drawing::Point(262, 97);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(53, 13);
            label9->TabIndex = 6;
            label9->Text = L"Tonnage:";
            // 
            // ShipSublight
            // 
            this->ShipSublight->AutoSize = true;
            this->ShipSublight->Location = System::Drawing::Point(138, 123);
            this->ShipSublight->Name = L"ShipSublight";
            this->ShipSublight->Size = System::Drawing::Size(64, 17);
            this->ShipSublight->TabIndex = 0;
            this->ShipSublight->Text = L"Sublight";
            this->ShipSublight->UseVisualStyleBackColor = true;
            this->ShipSublight->CheckedChanged += gcnew System::EventHandler(this, &CmdBuildShips::ShipUpdateEvent);
            // 
            // ShipName
            // 
            this->ShipName->Location = System::Drawing::Point(61, 67);
            this->ShipName->Name = L"ShipName";
            this->ShipName->Size = System::Drawing::Size(182, 20);
            this->ShipName->TabIndex = 1;
            // 
            // ShipClass
            // 
            this->ShipClass->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->ShipClass->FormattingEnabled = true;
            this->ShipClass->Items->AddRange(gcnew cli::array< System::Object^  >(17) {L"TR: Transport", L"PB: Picketboat  [2]", L"CT: Corvette  [4]", 
                L"ES: Escort  [10]", L"FF: Frigate  [20]", L"DD: Destroyer  [30]", L"CL: Light Cruiser  [40]", L"CS: Strike Cruiser  [50]", L"CA: Heavy Cruiser  [60]", 
                L"CC: Command Cruiser  [70]", L"BC: Battlecruiser  [80]", L"BS: Battleship  [90]", L"DN: Dreadnought  [100]", L"SD: Super Dreadnought  [110]", 
                L"BM: Battlemoon  [120]", L"BW: Battleworld  [130]", L"BR: Battlestar  [140]"});
            this->ShipClass->Location = System::Drawing::Point(61, 94);
            this->ShipClass->Name = L"ShipClass";
            this->ShipClass->Size = System::Drawing::Size(182, 21);
            this->ShipClass->TabIndex = 2;
            this->ShipClass->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdBuildShips::ShipUpdateEvent);
            // 
            // ShipSize
            // 
            this->ShipSize->Location = System::Drawing::Point(61, 121);
            this->ShipSize->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
            this->ShipSize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->ShipSize->Name = L"ShipSize";
            this->ShipSize->Size = System::Drawing::Size(60, 20);
            this->ShipSize->TabIndex = 3;
            this->ShipSize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
            this->ShipSize->ValueChanged += gcnew System::EventHandler(this, &CmdBuildShips::ShipUpdateEvent);
            // 
            // PresetScout
            // 
            this->PresetScout->Location = System::Drawing::Point(61, 38);
            this->PresetScout->Name = L"PresetScout";
            this->PresetScout->Size = System::Drawing::Size(60, 23);
            this->PresetScout->TabIndex = 4;
            this->PresetScout->Text = L"Scout";
            this->PresetScout->UseVisualStyleBackColor = true;
            this->PresetScout->Click += gcnew System::EventHandler(this, &CmdBuildShips::PresetScout_Click);
            // 
            // PresetTransport
            // 
            this->PresetTransport->Location = System::Drawing::Point(122, 38);
            this->PresetTransport->Name = L"PresetTransport";
            this->PresetTransport->Size = System::Drawing::Size(60, 23);
            this->PresetTransport->TabIndex = 4;
            this->PresetTransport->Text = L"Transport";
            this->PresetTransport->UseVisualStyleBackColor = true;
            this->PresetTransport->Click += gcnew System::EventHandler(this, &CmdBuildShips::PresetTransport_Click);
            // 
            // PresetWarship
            // 
            this->PresetWarship->Location = System::Drawing::Point(183, 38);
            this->PresetWarship->Name = L"PresetWarship";
            this->PresetWarship->Size = System::Drawing::Size(60, 23);
            this->PresetWarship->TabIndex = 4;
            this->PresetWarship->Text = L"Warship";
            this->PresetWarship->UseVisualStyleBackColor = true;
            this->PresetWarship->Click += gcnew System::EventHandler(this, &CmdBuildShips::PresetWarship_Click);
            // 
            // BtnBuild
            // 
            this->BtnBuild->Location = System::Drawing::Point(123, 149);
            this->BtnBuild->Name = L"BtnBuild";
            this->BtnBuild->Size = System::Drawing::Size(75, 23);
            this->BtnBuild->TabIndex = 5;
            this->BtnBuild->Text = L"Build";
            this->BtnBuild->UseVisualStyleBackColor = true;
            this->BtnBuild->Click += gcnew System::EventHandler(this, &CmdBuildShips::BtnBuild_Click);
            // 
            // BtnCancel
            // 
            this->BtnCancel->Location = System::Drawing::Point(203, 149);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 5;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            this->BtnCancel->Click += gcnew System::EventHandler(this, &CmdBuildShips::BtnCancel_Click);
            // 
            // InfoCost
            // 
            this->InfoCost->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCost->Location = System::Drawing::Point(325, 43);
            this->InfoCost->Name = L"InfoCost";
            this->InfoCost->Size = System::Drawing::Size(60, 17);
            this->InfoCost->TabIndex = 6;
            this->InfoCost->Text = L"0";
            // 
            // InfoUpkeep
            // 
            this->InfoUpkeep->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoUpkeep->Location = System::Drawing::Point(325, 70);
            this->InfoUpkeep->Name = L"InfoUpkeep";
            this->InfoUpkeep->Size = System::Drawing::Size(60, 17);
            this->InfoUpkeep->TabIndex = 6;
            this->InfoUpkeep->Text = L"0";
            // 
            // InfoTonnage
            // 
            this->InfoTonnage->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoTonnage->Location = System::Drawing::Point(325, 96);
            this->InfoTonnage->Name = L"InfoTonnage";
            this->InfoTonnage->Size = System::Drawing::Size(60, 17);
            this->InfoTonnage->TabIndex = 6;
            this->InfoTonnage->Text = L"0";
            // 
            // InfoAvailMA
            // 
            this->InfoAvailMA->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoAvailMA->Location = System::Drawing::Point(285, 9);
            this->InfoAvailMA->Name = L"InfoAvailMA";
            this->InfoAvailMA->Size = System::Drawing::Size(35, 17);
            this->InfoAvailMA->TabIndex = 6;
            this->InfoAvailMA->Text = L"0";
            // 
            // InfoAvailEU
            // 
            this->InfoAvailEU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoAvailEU->Location = System::Drawing::Point(108, 9);
            this->InfoAvailEU->Name = L"InfoAvailEU";
            this->InfoAvailEU->Size = System::Drawing::Size(65, 17);
            this->InfoAvailEU->TabIndex = 6;
            this->InfoAvailEU->Text = L"0";
            // 
            // CmdBuildShips
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(400, 184);
            this->Controls->Add(label2);
            this->Controls->Add(label6);
            this->Controls->Add(label9);
            this->Controls->Add(label5);
            this->Controls->Add(label8);
            this->Controls->Add(label4);
            this->Controls->Add(this->InfoAvailEU);
            this->Controls->Add(this->InfoAvailMA);
            this->Controls->Add(this->InfoTonnage);
            this->Controls->Add(this->InfoUpkeep);
            this->Controls->Add(this->InfoCost);
            this->Controls->Add(label7);
            this->Controls->Add(label3);
            this->Controls->Add(label1);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnBuild);
            this->Controls->Add(this->PresetWarship);
            this->Controls->Add(this->PresetTransport);
            this->Controls->Add(this->PresetScout);
            this->Controls->Add(this->ShipSize);
            this->Controls->Add(this->ShipClass);
            this->Controls->Add(this->ShipName);
            this->Controls->Add(this->ShipSublight);
            this->Name = L"CmdBuildShips";
            this->Text = L"Build Ship";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ShipSize))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void PresetScout_Click(System::Object^  sender, System::EventArgs^  e) {
             DoPresetScout();
         }
private: System::Void PresetTransport_Click(System::Object^  sender, System::EventArgs^  e) {
             DoPresetTransport();
         }
private: System::Void PresetWarship_Click(System::Object^  sender, System::EventArgs^  e) {
             DoPresetWarship();
         }
private: System::Void BtnBuild_Click(System::Object^  sender, System::EventArgs^  e) {
             Build();
         }
private: System::Void BtnCancel_Click(System::Object^  sender, System::EventArgs^  e) {
             this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
             Close();
         }
private: System::Void ShipUpdateEvent(System::Object^  sender, System::EventArgs^  e) {
             ShipUpdate();
         }
};
}
