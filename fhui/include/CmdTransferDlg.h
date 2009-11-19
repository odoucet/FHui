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
	/// Summary for CmdTransferDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CmdTransferDlg : public System::Windows::Forms::Form
	{
	public:
		CmdTransferDlg(CmdTransfer ^cmd)
		{
			InitializeComponent();

            InitArray();
            InitControls(cmd);
		}

        int             GetAmount(InventoryType inv);
        CmdTransfer^    GetCommand(InventoryType inv);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CmdTransferDlg()
		{
			if (components)
			{
				delete components;
			}
		}

        void    InitArray();
        void    InitControls(CmdTransfer ^cmd);
        void    InitControlsUnits();

        void    UpdateAmounts();
        void    AddAll();

        value struct UnitControls
        {
            Label^          Info;
            NumericUpDown^  Amount;
        };
        array<UnitControls>^    m_UnitControls;

        CmdTransfer^    m_Cmd;
        array<int>^     m_Inv;
        int             m_CapacityLimit;

    private: System::Windows::Forms::ComboBox^  TransFrom;
    protected: 
    private: System::Windows::Forms::ComboBox^  TransTo;
    private: System::Windows::Forms::Label^  InfoCapacityLimit;
    private: System::Windows::Forms::Label^  InfoRM;


    private: System::Windows::Forms::NumericUpDown^  AmountRM;
    private: System::Windows::Forms::Label^  InfoCU;


    private: System::Windows::Forms::NumericUpDown^  AmountCU;
    private: System::Windows::Forms::Label^  InfoIU;


    private: System::Windows::Forms::NumericUpDown^  AmountIU;
    private: System::Windows::Forms::Label^  InfoAU;


    private: System::Windows::Forms::NumericUpDown^  AmountAU;
    private: System::Windows::Forms::Label^  InfoPD;
    private: System::Windows::Forms::Label^  InfoSU;
    private: System::Windows::Forms::Label^  InfoFD;
    private: System::Windows::Forms::Label^  InfoFS;





    private: System::Windows::Forms::NumericUpDown^  AmountPD;
    private: System::Windows::Forms::NumericUpDown^  AmountSU;
    private: System::Windows::Forms::NumericUpDown^  AmountFD;
    private: System::Windows::Forms::NumericUpDown^  AmountFS;
    private: System::Windows::Forms::Label^  InfoDR;
    private: System::Windows::Forms::Label^  InfoFM;
    private: System::Windows::Forms::Label^  InfoFJ;
    private: System::Windows::Forms::Label^  InfoGW;








    private: System::Windows::Forms::NumericUpDown^  AmountDR;
    private: System::Windows::Forms::NumericUpDown^  AmountFM;
    private: System::Windows::Forms::NumericUpDown^  AmountFJ;
    private: System::Windows::Forms::NumericUpDown^  AmountGW;
    private: System::Windows::Forms::Label^  InfoGT;
    private: System::Windows::Forms::Label^  InfoJP;






    private: System::Windows::Forms::Label^  InfoTP;

    private: System::Windows::Forms::NumericUpDown^  AmountGT;
    private: System::Windows::Forms::NumericUpDown^  AmountJP;



    private: System::Windows::Forms::NumericUpDown^  AmountTP;
    private: System::Windows::Forms::Label^  InfoGU1;
    private: System::Windows::Forms::Label^  InfoGU5;
    private: System::Windows::Forms::Label^  InfoGU9;





    private: System::Windows::Forms::Label^  InfoGU2;
    private: System::Windows::Forms::Label^  InfoGU6;



    private: System::Windows::Forms::Label^  InfoGU3;
    private: System::Windows::Forms::Label^  InfoGU7;




private: System::Windows::Forms::Label^  InfoGU4;
private: System::Windows::Forms::Label^  InfoGU8;




    private: System::Windows::Forms::NumericUpDown^  AmountGU1;
    private: System::Windows::Forms::NumericUpDown^  AmountGU5;
    private: System::Windows::Forms::NumericUpDown^  AmountGU9;





    private: System::Windows::Forms::NumericUpDown^  AmountGU2;
    private: System::Windows::Forms::NumericUpDown^  AmountGU6;



    private: System::Windows::Forms::NumericUpDown^  AmountGU3;
    private: System::Windows::Forms::NumericUpDown^  AmountGU7;




private: System::Windows::Forms::NumericUpDown^  AmountGU4;
private: System::Windows::Forms::NumericUpDown^  AmountGU8;
private: System::Windows::Forms::Label^  InfoSG1;







private: System::Windows::Forms::Label^  InfoSG5;

private: System::Windows::Forms::Label^  InfoSG9;
private: System::Windows::Forms::Label^  InfoSG2;



private: System::Windows::Forms::Label^  InfoSG6;
private: System::Windows::Forms::Label^  InfoSG3;




private: System::Windows::Forms::Label^  InfoSG7;
private: System::Windows::Forms::Label^  InfoSG4;




private: System::Windows::Forms::Label^  InfoSG8;

private: System::Windows::Forms::NumericUpDown^  AmountSG1;



private: System::Windows::Forms::NumericUpDown^  AmountSG5;

private: System::Windows::Forms::NumericUpDown^  AmountSG9;
private: System::Windows::Forms::NumericUpDown^  AmountSG2;



private: System::Windows::Forms::NumericUpDown^  AmountSG6;
private: System::Windows::Forms::NumericUpDown^  AmountSG3;




private: System::Windows::Forms::NumericUpDown^  AmountSG7;
private: System::Windows::Forms::NumericUpDown^  AmountSG4;




private: System::Windows::Forms::NumericUpDown^  AmountSG8;

private: System::Windows::Forms::Label^  InfoCapacityUsed;
private: System::Windows::Forms::Button^  BtnTransfer;
private: System::Windows::Forms::Button^  BtnCancel;
private: System::Windows::Forms::Label^  InfoInv;
private: System::Windows::Forms::Button^  BtnAll;







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
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::Label^  label10;
            System::Windows::Forms::Label^  label12;
            System::Windows::Forms::Label^  label13;
            System::Windows::Forms::Label^  label14;
            System::Windows::Forms::Label^  label15;
            System::Windows::Forms::Label^  label20;
            System::Windows::Forms::Label^  label21;
            System::Windows::Forms::Label^  label22;
            System::Windows::Forms::Label^  label23;
            System::Windows::Forms::Label^  label28;
            System::Windows::Forms::Label^  label29;
            System::Windows::Forms::Label^  label30;
            System::Windows::Forms::Label^  label36;
            System::Windows::Forms::Label^  label37;
            System::Windows::Forms::Label^  label38;
            System::Windows::Forms::Label^  label40;
            System::Windows::Forms::Label^  label41;
            System::Windows::Forms::Label^  label44;
            System::Windows::Forms::Label^  label45;
            System::Windows::Forms::Label^  label48;
            System::Windows::Forms::Label^  label49;
            System::Windows::Forms::Label^  label68;
            System::Windows::Forms::Label^  label69;
            System::Windows::Forms::Label^  label70;
            System::Windows::Forms::Label^  label72;
            System::Windows::Forms::Label^  label73;
            System::Windows::Forms::Label^  label76;
            System::Windows::Forms::Label^  label77;
            System::Windows::Forms::Label^  label80;
            System::Windows::Forms::Label^  label81;
            System::Windows::Forms::Label^  label31;
            System::Windows::Forms::Label^  label5;
            this->TransFrom = (gcnew System::Windows::Forms::ComboBox());
            this->TransTo = (gcnew System::Windows::Forms::ComboBox());
            this->InfoCapacityLimit = (gcnew System::Windows::Forms::Label());
            this->InfoRM = (gcnew System::Windows::Forms::Label());
            this->AmountRM = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoCU = (gcnew System::Windows::Forms::Label());
            this->AmountCU = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoIU = (gcnew System::Windows::Forms::Label());
            this->AmountIU = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoAU = (gcnew System::Windows::Forms::Label());
            this->AmountAU = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoPD = (gcnew System::Windows::Forms::Label());
            this->InfoSU = (gcnew System::Windows::Forms::Label());
            this->InfoFD = (gcnew System::Windows::Forms::Label());
            this->InfoFS = (gcnew System::Windows::Forms::Label());
            this->AmountPD = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSU = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountFD = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountFS = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoDR = (gcnew System::Windows::Forms::Label());
            this->InfoFM = (gcnew System::Windows::Forms::Label());
            this->InfoFJ = (gcnew System::Windows::Forms::Label());
            this->InfoGW = (gcnew System::Windows::Forms::Label());
            this->AmountDR = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountFM = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountFJ = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGW = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoGT = (gcnew System::Windows::Forms::Label());
            this->InfoJP = (gcnew System::Windows::Forms::Label());
            this->InfoTP = (gcnew System::Windows::Forms::Label());
            this->AmountGT = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountJP = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountTP = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoGU1 = (gcnew System::Windows::Forms::Label());
            this->InfoGU5 = (gcnew System::Windows::Forms::Label());
            this->InfoGU9 = (gcnew System::Windows::Forms::Label());
            this->InfoGU2 = (gcnew System::Windows::Forms::Label());
            this->InfoGU6 = (gcnew System::Windows::Forms::Label());
            this->InfoGU3 = (gcnew System::Windows::Forms::Label());
            this->InfoGU7 = (gcnew System::Windows::Forms::Label());
            this->InfoGU4 = (gcnew System::Windows::Forms::Label());
            this->InfoGU8 = (gcnew System::Windows::Forms::Label());
            this->AmountGU1 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU5 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU9 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU2 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU6 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU3 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU7 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU4 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountGU8 = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoSG1 = (gcnew System::Windows::Forms::Label());
            this->InfoSG5 = (gcnew System::Windows::Forms::Label());
            this->InfoSG9 = (gcnew System::Windows::Forms::Label());
            this->InfoSG2 = (gcnew System::Windows::Forms::Label());
            this->InfoSG6 = (gcnew System::Windows::Forms::Label());
            this->InfoSG3 = (gcnew System::Windows::Forms::Label());
            this->InfoSG7 = (gcnew System::Windows::Forms::Label());
            this->InfoSG4 = (gcnew System::Windows::Forms::Label());
            this->InfoSG8 = (gcnew System::Windows::Forms::Label());
            this->AmountSG1 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG5 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG9 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG2 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG6 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG3 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG7 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG4 = (gcnew System::Windows::Forms::NumericUpDown());
            this->AmountSG8 = (gcnew System::Windows::Forms::NumericUpDown());
            this->InfoCapacityUsed = (gcnew System::Windows::Forms::Label());
            this->BtnTransfer = (gcnew System::Windows::Forms::Button());
            this->BtnCancel = (gcnew System::Windows::Forms::Button());
            this->InfoInv = (gcnew System::Windows::Forms::Label());
            this->BtnAll = (gcnew System::Windows::Forms::Button());
            label1 = (gcnew System::Windows::Forms::Label());
            label2 = (gcnew System::Windows::Forms::Label());
            label3 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label10 = (gcnew System::Windows::Forms::Label());
            label12 = (gcnew System::Windows::Forms::Label());
            label13 = (gcnew System::Windows::Forms::Label());
            label14 = (gcnew System::Windows::Forms::Label());
            label15 = (gcnew System::Windows::Forms::Label());
            label20 = (gcnew System::Windows::Forms::Label());
            label21 = (gcnew System::Windows::Forms::Label());
            label22 = (gcnew System::Windows::Forms::Label());
            label23 = (gcnew System::Windows::Forms::Label());
            label28 = (gcnew System::Windows::Forms::Label());
            label29 = (gcnew System::Windows::Forms::Label());
            label30 = (gcnew System::Windows::Forms::Label());
            label36 = (gcnew System::Windows::Forms::Label());
            label37 = (gcnew System::Windows::Forms::Label());
            label38 = (gcnew System::Windows::Forms::Label());
            label40 = (gcnew System::Windows::Forms::Label());
            label41 = (gcnew System::Windows::Forms::Label());
            label44 = (gcnew System::Windows::Forms::Label());
            label45 = (gcnew System::Windows::Forms::Label());
            label48 = (gcnew System::Windows::Forms::Label());
            label49 = (gcnew System::Windows::Forms::Label());
            label68 = (gcnew System::Windows::Forms::Label());
            label69 = (gcnew System::Windows::Forms::Label());
            label70 = (gcnew System::Windows::Forms::Label());
            label72 = (gcnew System::Windows::Forms::Label());
            label73 = (gcnew System::Windows::Forms::Label());
            label76 = (gcnew System::Windows::Forms::Label());
            label77 = (gcnew System::Windows::Forms::Label());
            label80 = (gcnew System::Windows::Forms::Label());
            label81 = (gcnew System::Windows::Forms::Label());
            label31 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountRM))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountCU))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountIU))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountAU))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountPD))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSU))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFD))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFS))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountDR))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFM))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFJ))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGW))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGT))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountJP))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountTP))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU1))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU5))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU9))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU2))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU6))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU3))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU7))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU4))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU8))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG1))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG5))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG9))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG2))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG6))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG3))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG7))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG4))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG8))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            label1->AutoSize = true;
            label1->Location = System::Drawing::Point(12, 16);
            label1->Name = L"label1";
            label1->Size = System::Drawing::Size(72, 13);
            label1->TabIndex = 1;
            label1->Text = L"Transfer from:";
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(253, 15);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(19, 13);
            label2->TabIndex = 1;
            label2->Text = L"to:";
            // 
            // label3
            // 
            label3->AutoSize = true;
            label3->Location = System::Drawing::Point(12, 46);
            label3->Name = L"label3";
            label3->Size = System::Drawing::Size(71, 13);
            label3->TabIndex = 1;
            label3->Text = L"Capacity limit:";
            // 
            // label4
            // 
            label4->AutoSize = true;
            label4->Location = System::Drawing::Point(12, 78);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(27, 13);
            label4->TabIndex = 1;
            label4->Text = L"RM:";
            // 
            // label6
            // 
            label6->AutoSize = true;
            label6->Location = System::Drawing::Point(12, 102);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(25, 13);
            label6->TabIndex = 1;
            label6->Text = L"CU:";
            // 
            // label8
            // 
            label8->AutoSize = true;
            label8->Location = System::Drawing::Point(12, 126);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(21, 13);
            label8->TabIndex = 1;
            label8->Text = L"IU:";
            // 
            // label10
            // 
            label10->AutoSize = true;
            label10->Location = System::Drawing::Point(12, 150);
            label10->Name = L"label10";
            label10->Size = System::Drawing::Size(25, 13);
            label10->TabIndex = 1;
            label10->Text = L"AU:";
            // 
            // label12
            // 
            label12->AutoSize = true;
            label12->Location = System::Drawing::Point(12, 174);
            label12->Name = L"label12";
            label12->Size = System::Drawing::Size(25, 13);
            label12->TabIndex = 1;
            label12->Text = L"PD:";
            // 
            // label13
            // 
            label13->AutoSize = true;
            label13->Location = System::Drawing::Point(12, 198);
            label13->Name = L"label13";
            label13->Size = System::Drawing::Size(79, 13);
            label13->TabIndex = 1;
            label13->Text = L"Starbase Units:";
            // 
            // label14
            // 
            label14->AutoSize = true;
            label14->Location = System::Drawing::Point(12, 222);
            label14->Name = L"label14";
            label14->Size = System::Drawing::Size(56, 13);
            label14->TabIndex = 1;
            label14->Text = L"Field Dist.:";
            // 
            // label15
            // 
            label15->AutoSize = true;
            label15->Location = System::Drawing::Point(12, 246);
            label15->Name = L"label15";
            label15->Size = System::Drawing::Size(51, 13);
            label15->TabIndex = 1;
            label15->Text = L"Fail-Safe:";
            // 
            // label20
            // 
            label20->AutoSize = true;
            label20->Location = System::Drawing::Point(12, 270);
            label20->Name = L"label20";
            label20->Size = System::Drawing::Size(66, 13);
            label20->TabIndex = 1;
            label20->Text = L"Dam-Repair:";
            // 
            // label21
            // 
            label21->AutoSize = true;
            label21->Location = System::Drawing::Point(12, 294);
            label21->Name = L"label21";
            label21->Size = System::Drawing::Size(78, 13);
            label21->TabIndex = 1;
            label21->Text = L"Force-Misjump:";
            // 
            // label22
            // 
            label22->AutoSize = true;
            label22->Location = System::Drawing::Point(12, 318);
            label22->Name = L"label22";
            label22->Size = System::Drawing::Size(65, 13);
            label22->TabIndex = 1;
            label22->Text = L"Force-Jump:";
            // 
            // label23
            // 
            label23->AutoSize = true;
            label23->Location = System::Drawing::Point(226, 294);
            label23->Name = L"label23";
            label23->Size = System::Drawing::Size(76, 13);
            label23->TabIndex = 1;
            label23->Text = L"Germ-Warfare:";
            // 
            // label28
            // 
            label28->AutoSize = true;
            label28->Location = System::Drawing::Point(440, 294);
            label28->Name = L"label28";
            label28->Size = System::Drawing::Size(77, 13);
            label28->TabIndex = 1;
            label28->Text = L"Gv-Telescope:";
            // 
            // label29
            // 
            label29->AutoSize = true;
            label29->Location = System::Drawing::Point(440, 318);
            label29->Name = L"label29";
            label29->Size = System::Drawing::Size(65, 13);
            label29->TabIndex = 1;
            label29->Text = L"Jump-Portal:";
            // 
            // label30
            // 
            label30->AutoSize = true;
            label30->Location = System::Drawing::Point(226, 318);
            label30->Name = L"label30";
            label30->Size = System::Drawing::Size(67, 13);
            label30->TabIndex = 1;
            label30->Text = L"T-form Plant:";
            // 
            // label36
            // 
            label36->AutoSize = true;
            label36->Location = System::Drawing::Point(226, 78);
            label36->Name = L"label36";
            label36->Size = System::Drawing::Size(61, 13);
            label36->TabIndex = 1;
            label36->Text = L"Gun-Unit 1:";
            // 
            // label37
            // 
            label37->AutoSize = true;
            label37->Location = System::Drawing::Point(226, 174);
            label37->Name = L"label37";
            label37->Size = System::Drawing::Size(61, 13);
            label37->TabIndex = 1;
            label37->Text = L"Gun-Unit 5:";
            // 
            // label38
            // 
            label38->AutoSize = true;
            label38->Location = System::Drawing::Point(226, 270);
            label38->Name = L"label38";
            label38->Size = System::Drawing::Size(61, 13);
            label38->TabIndex = 1;
            label38->Text = L"Gun-Unit 9:";
            // 
            // label40
            // 
            label40->AutoSize = true;
            label40->Location = System::Drawing::Point(226, 102);
            label40->Name = L"label40";
            label40->Size = System::Drawing::Size(61, 13);
            label40->TabIndex = 1;
            label40->Text = L"Gun-Unit 2:";
            // 
            // label41
            // 
            label41->AutoSize = true;
            label41->Location = System::Drawing::Point(226, 198);
            label41->Name = L"label41";
            label41->Size = System::Drawing::Size(61, 13);
            label41->TabIndex = 1;
            label41->Text = L"Gun-Unit 6:";
            // 
            // label44
            // 
            label44->AutoSize = true;
            label44->Location = System::Drawing::Point(226, 126);
            label44->Name = L"label44";
            label44->Size = System::Drawing::Size(61, 13);
            label44->TabIndex = 1;
            label44->Text = L"Gun-Unit 3:";
            // 
            // label45
            // 
            label45->AutoSize = true;
            label45->Location = System::Drawing::Point(226, 222);
            label45->Name = L"label45";
            label45->Size = System::Drawing::Size(61, 13);
            label45->TabIndex = 1;
            label45->Text = L"Gun-Unit 7:";
            // 
            // label48
            // 
            label48->AutoSize = true;
            label48->Location = System::Drawing::Point(226, 150);
            label48->Name = L"label48";
            label48->Size = System::Drawing::Size(61, 13);
            label48->TabIndex = 1;
            label48->Text = L"Gun-Unit 4:";
            // 
            // label49
            // 
            label49->AutoSize = true;
            label49->Location = System::Drawing::Point(226, 246);
            label49->Name = L"label49";
            label49->Size = System::Drawing::Size(61, 13);
            label49->TabIndex = 1;
            label49->Text = L"Gun-Unit 8:";
            // 
            // label68
            // 
            label68->AutoSize = true;
            label68->Location = System::Drawing::Point(440, 78);
            label68->Name = L"label68";
            label68->Size = System::Drawing::Size(71, 13);
            label68->TabIndex = 1;
            label68->Text = L"Shield Gen 1:";
            // 
            // label69
            // 
            label69->AutoSize = true;
            label69->Location = System::Drawing::Point(440, 174);
            label69->Name = L"label69";
            label69->Size = System::Drawing::Size(71, 13);
            label69->TabIndex = 1;
            label69->Text = L"Shield Gen 5:";
            // 
            // label70
            // 
            label70->AutoSize = true;
            label70->Location = System::Drawing::Point(440, 270);
            label70->Name = L"label70";
            label70->Size = System::Drawing::Size(71, 13);
            label70->TabIndex = 1;
            label70->Text = L"Shield Gen 9:";
            // 
            // label72
            // 
            label72->AutoSize = true;
            label72->Location = System::Drawing::Point(440, 102);
            label72->Name = L"label72";
            label72->Size = System::Drawing::Size(71, 13);
            label72->TabIndex = 1;
            label72->Text = L"Shield Gen 2:";
            // 
            // label73
            // 
            label73->AutoSize = true;
            label73->Location = System::Drawing::Point(440, 198);
            label73->Name = L"label73";
            label73->Size = System::Drawing::Size(71, 13);
            label73->TabIndex = 1;
            label73->Text = L"Shield Gen 6:";
            // 
            // label76
            // 
            label76->AutoSize = true;
            label76->Location = System::Drawing::Point(440, 126);
            label76->Name = L"label76";
            label76->Size = System::Drawing::Size(71, 13);
            label76->TabIndex = 1;
            label76->Text = L"Shield Gen 3:";
            // 
            // label77
            // 
            label77->AutoSize = true;
            label77->Location = System::Drawing::Point(440, 222);
            label77->Name = L"label77";
            label77->Size = System::Drawing::Size(71, 13);
            label77->TabIndex = 1;
            label77->Text = L"Shield Gen 7:";
            // 
            // label80
            // 
            label80->AutoSize = true;
            label80->Location = System::Drawing::Point(440, 150);
            label80->Name = L"label80";
            label80->Size = System::Drawing::Size(71, 13);
            label80->TabIndex = 1;
            label80->Text = L"Shield Gen 4:";
            // 
            // label81
            // 
            label81->AutoSize = true;
            label81->Location = System::Drawing::Point(440, 246);
            label81->Name = L"label81";
            label81->Size = System::Drawing::Size(71, 13);
            label81->TabIndex = 1;
            label81->Text = L"Shield Gen 8:";
            // 
            // label31
            // 
            label31->AutoSize = true;
            label31->Location = System::Drawing::Point(164, 46);
            label31->Name = L"label31";
            label31->Size = System::Drawing::Size(77, 13);
            label31->TabIndex = 1;
            label31->Text = L"Capacity used:";
            // 
            // label5
            // 
            label5->AutoSize = true;
            label5->Location = System::Drawing::Point(320, 46);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(58, 13);
            label5->TabIndex = 1;
            label5->Text = L"Inv/Cargo:";
            // 
            // TransFrom
            // 
            this->TransFrom->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->TransFrom->FormattingEnabled = true;
            this->TransFrom->Location = System::Drawing::Point(91, 12);
            this->TransFrom->MaxDropDownItems = 20;
            this->TransFrom->Name = L"TransFrom";
            this->TransFrom->Size = System::Drawing::Size(152, 21);
            this->TransFrom->TabIndex = 0;
            this->TransFrom->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdTransferDlg::TransFrom_SelectedIndexChanged);
            // 
            // TransTo
            // 
            this->TransTo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->TransTo->Enabled = false;
            this->TransTo->FormattingEnabled = true;
            this->TransTo->Location = System::Drawing::Point(279, 12);
            this->TransTo->MaxDropDownItems = 20;
            this->TransTo->Name = L"TransTo";
            this->TransTo->Size = System::Drawing::Size(152, 21);
            this->TransTo->TabIndex = 0;
            this->TransTo->SelectedIndexChanged += gcnew System::EventHandler(this, &CmdTransferDlg::TransTo_SelectedIndexChanged);
            // 
            // InfoCapacityLimit
            // 
            this->InfoCapacityLimit->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCapacityLimit->Location = System::Drawing::Point(91, 44);
            this->InfoCapacityLimit->Name = L"InfoCapacityLimit";
            this->InfoCapacityLimit->Size = System::Drawing::Size(60, 17);
            this->InfoCapacityLimit->TabIndex = 1;
            this->InfoCapacityLimit->Text = L"N/A";
            // 
            // InfoRM
            // 
            this->InfoRM->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoRM->Location = System::Drawing::Point(91, 76);
            this->InfoRM->Name = L"InfoRM";
            this->InfoRM->Size = System::Drawing::Size(40, 17);
            this->InfoRM->TabIndex = 1;
            this->InfoRM->Text = L"0";
            // 
            // AmountRM
            // 
            this->AmountRM->Enabled = false;
            this->AmountRM->Location = System::Drawing::Point(138, 74);
            this->AmountRM->Name = L"AmountRM";
            this->AmountRM->Size = System::Drawing::Size(55, 20);
            this->AmountRM->TabIndex = 2;
            this->AmountRM->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountRM->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoCU
            // 
            this->InfoCU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCU->Location = System::Drawing::Point(91, 100);
            this->InfoCU->Name = L"InfoCU";
            this->InfoCU->Size = System::Drawing::Size(40, 17);
            this->InfoCU->TabIndex = 1;
            this->InfoCU->Text = L"0";
            // 
            // AmountCU
            // 
            this->AmountCU->Enabled = false;
            this->AmountCU->Location = System::Drawing::Point(138, 98);
            this->AmountCU->Name = L"AmountCU";
            this->AmountCU->Size = System::Drawing::Size(55, 20);
            this->AmountCU->TabIndex = 2;
            this->AmountCU->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountCU->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoIU
            // 
            this->InfoIU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoIU->Location = System::Drawing::Point(91, 124);
            this->InfoIU->Name = L"InfoIU";
            this->InfoIU->Size = System::Drawing::Size(40, 17);
            this->InfoIU->TabIndex = 1;
            this->InfoIU->Text = L"0";
            // 
            // AmountIU
            // 
            this->AmountIU->Enabled = false;
            this->AmountIU->Location = System::Drawing::Point(138, 122);
            this->AmountIU->Name = L"AmountIU";
            this->AmountIU->Size = System::Drawing::Size(55, 20);
            this->AmountIU->TabIndex = 2;
            this->AmountIU->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountIU->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoAU
            // 
            this->InfoAU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoAU->Location = System::Drawing::Point(91, 148);
            this->InfoAU->Name = L"InfoAU";
            this->InfoAU->Size = System::Drawing::Size(40, 17);
            this->InfoAU->TabIndex = 1;
            this->InfoAU->Text = L"0";
            // 
            // AmountAU
            // 
            this->AmountAU->Enabled = false;
            this->AmountAU->Location = System::Drawing::Point(138, 146);
            this->AmountAU->Name = L"AmountAU";
            this->AmountAU->Size = System::Drawing::Size(55, 20);
            this->AmountAU->TabIndex = 2;
            this->AmountAU->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountAU->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoPD
            // 
            this->InfoPD->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoPD->Location = System::Drawing::Point(91, 172);
            this->InfoPD->Name = L"InfoPD";
            this->InfoPD->Size = System::Drawing::Size(40, 17);
            this->InfoPD->TabIndex = 1;
            this->InfoPD->Text = L"0";
            // 
            // InfoSU
            // 
            this->InfoSU->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSU->Location = System::Drawing::Point(91, 196);
            this->InfoSU->Name = L"InfoSU";
            this->InfoSU->Size = System::Drawing::Size(40, 17);
            this->InfoSU->TabIndex = 1;
            this->InfoSU->Text = L"0";
            // 
            // InfoFD
            // 
            this->InfoFD->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoFD->Location = System::Drawing::Point(91, 220);
            this->InfoFD->Name = L"InfoFD";
            this->InfoFD->Size = System::Drawing::Size(40, 17);
            this->InfoFD->TabIndex = 1;
            this->InfoFD->Text = L"0";
            // 
            // InfoFS
            // 
            this->InfoFS->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoFS->Location = System::Drawing::Point(91, 244);
            this->InfoFS->Name = L"InfoFS";
            this->InfoFS->Size = System::Drawing::Size(40, 17);
            this->InfoFS->TabIndex = 1;
            this->InfoFS->Text = L"0";
            // 
            // AmountPD
            // 
            this->AmountPD->Enabled = false;
            this->AmountPD->Location = System::Drawing::Point(138, 170);
            this->AmountPD->Name = L"AmountPD";
            this->AmountPD->Size = System::Drawing::Size(55, 20);
            this->AmountPD->TabIndex = 2;
            this->AmountPD->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountPD->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSU
            // 
            this->AmountSU->Enabled = false;
            this->AmountSU->Location = System::Drawing::Point(138, 194);
            this->AmountSU->Name = L"AmountSU";
            this->AmountSU->Size = System::Drawing::Size(55, 20);
            this->AmountSU->TabIndex = 2;
            this->AmountSU->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSU->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountFD
            // 
            this->AmountFD->Enabled = false;
            this->AmountFD->Location = System::Drawing::Point(138, 218);
            this->AmountFD->Name = L"AmountFD";
            this->AmountFD->Size = System::Drawing::Size(55, 20);
            this->AmountFD->TabIndex = 2;
            this->AmountFD->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountFD->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountFS
            // 
            this->AmountFS->Enabled = false;
            this->AmountFS->Location = System::Drawing::Point(138, 242);
            this->AmountFS->Name = L"AmountFS";
            this->AmountFS->Size = System::Drawing::Size(55, 20);
            this->AmountFS->TabIndex = 2;
            this->AmountFS->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountFS->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoDR
            // 
            this->InfoDR->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoDR->Location = System::Drawing::Point(91, 268);
            this->InfoDR->Name = L"InfoDR";
            this->InfoDR->Size = System::Drawing::Size(40, 17);
            this->InfoDR->TabIndex = 1;
            this->InfoDR->Text = L"0";
            // 
            // InfoFM
            // 
            this->InfoFM->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoFM->Location = System::Drawing::Point(91, 292);
            this->InfoFM->Name = L"InfoFM";
            this->InfoFM->Size = System::Drawing::Size(40, 17);
            this->InfoFM->TabIndex = 1;
            this->InfoFM->Text = L"0";
            // 
            // InfoFJ
            // 
            this->InfoFJ->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoFJ->Location = System::Drawing::Point(91, 316);
            this->InfoFJ->Name = L"InfoFJ";
            this->InfoFJ->Size = System::Drawing::Size(40, 17);
            this->InfoFJ->TabIndex = 1;
            this->InfoFJ->Text = L"0";
            // 
            // InfoGW
            // 
            this->InfoGW->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGW->Location = System::Drawing::Point(305, 292);
            this->InfoGW->Name = L"InfoGW";
            this->InfoGW->Size = System::Drawing::Size(40, 17);
            this->InfoGW->TabIndex = 1;
            this->InfoGW->Text = L"0";
            // 
            // AmountDR
            // 
            this->AmountDR->Enabled = false;
            this->AmountDR->Location = System::Drawing::Point(138, 266);
            this->AmountDR->Name = L"AmountDR";
            this->AmountDR->Size = System::Drawing::Size(55, 20);
            this->AmountDR->TabIndex = 2;
            this->AmountDR->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountDR->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountFM
            // 
            this->AmountFM->Enabled = false;
            this->AmountFM->Location = System::Drawing::Point(138, 290);
            this->AmountFM->Name = L"AmountFM";
            this->AmountFM->Size = System::Drawing::Size(55, 20);
            this->AmountFM->TabIndex = 2;
            this->AmountFM->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountFM->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountFJ
            // 
            this->AmountFJ->Enabled = false;
            this->AmountFJ->Location = System::Drawing::Point(138, 314);
            this->AmountFJ->Name = L"AmountFJ";
            this->AmountFJ->Size = System::Drawing::Size(55, 20);
            this->AmountFJ->TabIndex = 2;
            this->AmountFJ->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountFJ->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGW
            // 
            this->AmountGW->Enabled = false;
            this->AmountGW->Location = System::Drawing::Point(352, 290);
            this->AmountGW->Name = L"AmountGW";
            this->AmountGW->Size = System::Drawing::Size(55, 20);
            this->AmountGW->TabIndex = 2;
            this->AmountGW->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGW->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoGT
            // 
            this->InfoGT->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGT->Location = System::Drawing::Point(519, 292);
            this->InfoGT->Name = L"InfoGT";
            this->InfoGT->Size = System::Drawing::Size(40, 17);
            this->InfoGT->TabIndex = 1;
            this->InfoGT->Text = L"0";
            // 
            // InfoJP
            // 
            this->InfoJP->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoJP->Location = System::Drawing::Point(519, 316);
            this->InfoJP->Name = L"InfoJP";
            this->InfoJP->Size = System::Drawing::Size(40, 17);
            this->InfoJP->TabIndex = 1;
            this->InfoJP->Text = L"0";
            // 
            // InfoTP
            // 
            this->InfoTP->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoTP->Location = System::Drawing::Point(305, 316);
            this->InfoTP->Name = L"InfoTP";
            this->InfoTP->Size = System::Drawing::Size(40, 17);
            this->InfoTP->TabIndex = 1;
            this->InfoTP->Text = L"0";
            // 
            // AmountGT
            // 
            this->AmountGT->Enabled = false;
            this->AmountGT->Location = System::Drawing::Point(566, 290);
            this->AmountGT->Name = L"AmountGT";
            this->AmountGT->Size = System::Drawing::Size(55, 20);
            this->AmountGT->TabIndex = 2;
            this->AmountGT->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGT->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountJP
            // 
            this->AmountJP->Enabled = false;
            this->AmountJP->Location = System::Drawing::Point(566, 314);
            this->AmountJP->Name = L"AmountJP";
            this->AmountJP->Size = System::Drawing::Size(55, 20);
            this->AmountJP->TabIndex = 2;
            this->AmountJP->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountJP->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountTP
            // 
            this->AmountTP->Enabled = false;
            this->AmountTP->Location = System::Drawing::Point(352, 314);
            this->AmountTP->Name = L"AmountTP";
            this->AmountTP->Size = System::Drawing::Size(55, 20);
            this->AmountTP->TabIndex = 2;
            this->AmountTP->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountTP->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoGU1
            // 
            this->InfoGU1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU1->Location = System::Drawing::Point(305, 76);
            this->InfoGU1->Name = L"InfoGU1";
            this->InfoGU1->Size = System::Drawing::Size(40, 17);
            this->InfoGU1->TabIndex = 1;
            this->InfoGU1->Text = L"0";
            // 
            // InfoGU5
            // 
            this->InfoGU5->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU5->Location = System::Drawing::Point(305, 172);
            this->InfoGU5->Name = L"InfoGU5";
            this->InfoGU5->Size = System::Drawing::Size(40, 17);
            this->InfoGU5->TabIndex = 1;
            this->InfoGU5->Text = L"0";
            // 
            // InfoGU9
            // 
            this->InfoGU9->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU9->Location = System::Drawing::Point(305, 268);
            this->InfoGU9->Name = L"InfoGU9";
            this->InfoGU9->Size = System::Drawing::Size(40, 17);
            this->InfoGU9->TabIndex = 1;
            this->InfoGU9->Text = L"0";
            // 
            // InfoGU2
            // 
            this->InfoGU2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU2->Location = System::Drawing::Point(305, 100);
            this->InfoGU2->Name = L"InfoGU2";
            this->InfoGU2->Size = System::Drawing::Size(40, 17);
            this->InfoGU2->TabIndex = 1;
            this->InfoGU2->Text = L"0";
            // 
            // InfoGU6
            // 
            this->InfoGU6->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU6->Location = System::Drawing::Point(305, 196);
            this->InfoGU6->Name = L"InfoGU6";
            this->InfoGU6->Size = System::Drawing::Size(40, 17);
            this->InfoGU6->TabIndex = 1;
            this->InfoGU6->Text = L"0";
            // 
            // InfoGU3
            // 
            this->InfoGU3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU3->Location = System::Drawing::Point(305, 124);
            this->InfoGU3->Name = L"InfoGU3";
            this->InfoGU3->Size = System::Drawing::Size(40, 17);
            this->InfoGU3->TabIndex = 1;
            this->InfoGU3->Text = L"0";
            // 
            // InfoGU7
            // 
            this->InfoGU7->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU7->Location = System::Drawing::Point(305, 220);
            this->InfoGU7->Name = L"InfoGU7";
            this->InfoGU7->Size = System::Drawing::Size(40, 17);
            this->InfoGU7->TabIndex = 1;
            this->InfoGU7->Text = L"0";
            // 
            // InfoGU4
            // 
            this->InfoGU4->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU4->Location = System::Drawing::Point(305, 148);
            this->InfoGU4->Name = L"InfoGU4";
            this->InfoGU4->Size = System::Drawing::Size(40, 17);
            this->InfoGU4->TabIndex = 1;
            this->InfoGU4->Text = L"0";
            // 
            // InfoGU8
            // 
            this->InfoGU8->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoGU8->Location = System::Drawing::Point(305, 244);
            this->InfoGU8->Name = L"InfoGU8";
            this->InfoGU8->Size = System::Drawing::Size(40, 17);
            this->InfoGU8->TabIndex = 1;
            this->InfoGU8->Text = L"0";
            // 
            // AmountGU1
            // 
            this->AmountGU1->Enabled = false;
            this->AmountGU1->Location = System::Drawing::Point(352, 74);
            this->AmountGU1->Name = L"AmountGU1";
            this->AmountGU1->Size = System::Drawing::Size(55, 20);
            this->AmountGU1->TabIndex = 2;
            this->AmountGU1->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU1->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU5
            // 
            this->AmountGU5->Enabled = false;
            this->AmountGU5->Location = System::Drawing::Point(352, 170);
            this->AmountGU5->Name = L"AmountGU5";
            this->AmountGU5->Size = System::Drawing::Size(55, 20);
            this->AmountGU5->TabIndex = 2;
            this->AmountGU5->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU5->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU9
            // 
            this->AmountGU9->Enabled = false;
            this->AmountGU9->Location = System::Drawing::Point(352, 266);
            this->AmountGU9->Name = L"AmountGU9";
            this->AmountGU9->Size = System::Drawing::Size(55, 20);
            this->AmountGU9->TabIndex = 2;
            this->AmountGU9->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU9->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU2
            // 
            this->AmountGU2->Enabled = false;
            this->AmountGU2->Location = System::Drawing::Point(352, 98);
            this->AmountGU2->Name = L"AmountGU2";
            this->AmountGU2->Size = System::Drawing::Size(55, 20);
            this->AmountGU2->TabIndex = 2;
            this->AmountGU2->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU2->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU6
            // 
            this->AmountGU6->Enabled = false;
            this->AmountGU6->Location = System::Drawing::Point(352, 194);
            this->AmountGU6->Name = L"AmountGU6";
            this->AmountGU6->Size = System::Drawing::Size(55, 20);
            this->AmountGU6->TabIndex = 2;
            this->AmountGU6->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU6->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU3
            // 
            this->AmountGU3->Enabled = false;
            this->AmountGU3->Location = System::Drawing::Point(352, 122);
            this->AmountGU3->Name = L"AmountGU3";
            this->AmountGU3->Size = System::Drawing::Size(55, 20);
            this->AmountGU3->TabIndex = 2;
            this->AmountGU3->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU3->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU7
            // 
            this->AmountGU7->Enabled = false;
            this->AmountGU7->Location = System::Drawing::Point(352, 218);
            this->AmountGU7->Name = L"AmountGU7";
            this->AmountGU7->Size = System::Drawing::Size(55, 20);
            this->AmountGU7->TabIndex = 2;
            this->AmountGU7->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU7->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU4
            // 
            this->AmountGU4->Enabled = false;
            this->AmountGU4->Location = System::Drawing::Point(352, 146);
            this->AmountGU4->Name = L"AmountGU4";
            this->AmountGU4->Size = System::Drawing::Size(55, 20);
            this->AmountGU4->TabIndex = 2;
            this->AmountGU4->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU4->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountGU8
            // 
            this->AmountGU8->Enabled = false;
            this->AmountGU8->Location = System::Drawing::Point(352, 242);
            this->AmountGU8->Name = L"AmountGU8";
            this->AmountGU8->Size = System::Drawing::Size(55, 20);
            this->AmountGU8->TabIndex = 2;
            this->AmountGU8->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountGU8->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoSG1
            // 
            this->InfoSG1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG1->Location = System::Drawing::Point(519, 76);
            this->InfoSG1->Name = L"InfoSG1";
            this->InfoSG1->Size = System::Drawing::Size(40, 17);
            this->InfoSG1->TabIndex = 1;
            this->InfoSG1->Text = L"0";
            // 
            // InfoSG5
            // 
            this->InfoSG5->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG5->Location = System::Drawing::Point(519, 172);
            this->InfoSG5->Name = L"InfoSG5";
            this->InfoSG5->Size = System::Drawing::Size(40, 17);
            this->InfoSG5->TabIndex = 1;
            this->InfoSG5->Text = L"0";
            // 
            // InfoSG9
            // 
            this->InfoSG9->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG9->Location = System::Drawing::Point(519, 268);
            this->InfoSG9->Name = L"InfoSG9";
            this->InfoSG9->Size = System::Drawing::Size(40, 17);
            this->InfoSG9->TabIndex = 1;
            this->InfoSG9->Text = L"0";
            // 
            // InfoSG2
            // 
            this->InfoSG2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG2->Location = System::Drawing::Point(519, 100);
            this->InfoSG2->Name = L"InfoSG2";
            this->InfoSG2->Size = System::Drawing::Size(40, 17);
            this->InfoSG2->TabIndex = 1;
            this->InfoSG2->Text = L"0";
            // 
            // InfoSG6
            // 
            this->InfoSG6->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG6->Location = System::Drawing::Point(519, 196);
            this->InfoSG6->Name = L"InfoSG6";
            this->InfoSG6->Size = System::Drawing::Size(40, 17);
            this->InfoSG6->TabIndex = 1;
            this->InfoSG6->Text = L"0";
            // 
            // InfoSG3
            // 
            this->InfoSG3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG3->Location = System::Drawing::Point(519, 124);
            this->InfoSG3->Name = L"InfoSG3";
            this->InfoSG3->Size = System::Drawing::Size(40, 17);
            this->InfoSG3->TabIndex = 1;
            this->InfoSG3->Text = L"0";
            // 
            // InfoSG7
            // 
            this->InfoSG7->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG7->Location = System::Drawing::Point(519, 220);
            this->InfoSG7->Name = L"InfoSG7";
            this->InfoSG7->Size = System::Drawing::Size(40, 17);
            this->InfoSG7->TabIndex = 1;
            this->InfoSG7->Text = L"0";
            // 
            // InfoSG4
            // 
            this->InfoSG4->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG4->Location = System::Drawing::Point(519, 148);
            this->InfoSG4->Name = L"InfoSG4";
            this->InfoSG4->Size = System::Drawing::Size(40, 17);
            this->InfoSG4->TabIndex = 1;
            this->InfoSG4->Text = L"0";
            // 
            // InfoSG8
            // 
            this->InfoSG8->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoSG8->Location = System::Drawing::Point(519, 244);
            this->InfoSG8->Name = L"InfoSG8";
            this->InfoSG8->Size = System::Drawing::Size(40, 17);
            this->InfoSG8->TabIndex = 1;
            this->InfoSG8->Text = L"0";
            // 
            // AmountSG1
            // 
            this->AmountSG1->Enabled = false;
            this->AmountSG1->Location = System::Drawing::Point(566, 74);
            this->AmountSG1->Name = L"AmountSG1";
            this->AmountSG1->Size = System::Drawing::Size(55, 20);
            this->AmountSG1->TabIndex = 2;
            this->AmountSG1->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG1->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG5
            // 
            this->AmountSG5->Enabled = false;
            this->AmountSG5->Location = System::Drawing::Point(566, 170);
            this->AmountSG5->Name = L"AmountSG5";
            this->AmountSG5->Size = System::Drawing::Size(55, 20);
            this->AmountSG5->TabIndex = 2;
            this->AmountSG5->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG5->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG9
            // 
            this->AmountSG9->Enabled = false;
            this->AmountSG9->Location = System::Drawing::Point(566, 266);
            this->AmountSG9->Name = L"AmountSG9";
            this->AmountSG9->Size = System::Drawing::Size(55, 20);
            this->AmountSG9->TabIndex = 2;
            this->AmountSG9->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG9->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG2
            // 
            this->AmountSG2->Enabled = false;
            this->AmountSG2->Location = System::Drawing::Point(566, 98);
            this->AmountSG2->Name = L"AmountSG2";
            this->AmountSG2->Size = System::Drawing::Size(55, 20);
            this->AmountSG2->TabIndex = 2;
            this->AmountSG2->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG2->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG6
            // 
            this->AmountSG6->Enabled = false;
            this->AmountSG6->Location = System::Drawing::Point(566, 194);
            this->AmountSG6->Name = L"AmountSG6";
            this->AmountSG6->Size = System::Drawing::Size(55, 20);
            this->AmountSG6->TabIndex = 2;
            this->AmountSG6->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG6->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG3
            // 
            this->AmountSG3->Enabled = false;
            this->AmountSG3->Location = System::Drawing::Point(566, 122);
            this->AmountSG3->Name = L"AmountSG3";
            this->AmountSG3->Size = System::Drawing::Size(55, 20);
            this->AmountSG3->TabIndex = 2;
            this->AmountSG3->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG3->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG7
            // 
            this->AmountSG7->Enabled = false;
            this->AmountSG7->Location = System::Drawing::Point(566, 218);
            this->AmountSG7->Name = L"AmountSG7";
            this->AmountSG7->Size = System::Drawing::Size(55, 20);
            this->AmountSG7->TabIndex = 2;
            this->AmountSG7->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG7->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG4
            // 
            this->AmountSG4->Enabled = false;
            this->AmountSG4->Location = System::Drawing::Point(566, 146);
            this->AmountSG4->Name = L"AmountSG4";
            this->AmountSG4->Size = System::Drawing::Size(55, 20);
            this->AmountSG4->TabIndex = 2;
            this->AmountSG4->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG4->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // AmountSG8
            // 
            this->AmountSG8->Enabled = false;
            this->AmountSG8->Location = System::Drawing::Point(566, 242);
            this->AmountSG8->Name = L"AmountSG8";
            this->AmountSG8->Size = System::Drawing::Size(55, 20);
            this->AmountSG8->TabIndex = 2;
            this->AmountSG8->ValueChanged += gcnew System::EventHandler(this, &CmdTransferDlg::Amounts_Update);
            this->AmountSG8->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &CmdTransferDlg::Amounts_KeyUp);
            // 
            // InfoCapacityUsed
            // 
            this->InfoCapacityUsed->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoCapacityUsed->Location = System::Drawing::Point(243, 44);
            this->InfoCapacityUsed->Name = L"InfoCapacityUsed";
            this->InfoCapacityUsed->Size = System::Drawing::Size(60, 17);
            this->InfoCapacityUsed->TabIndex = 1;
            this->InfoCapacityUsed->Text = L"N/A";
            // 
            // BtnTransfer
            // 
            this->BtnTransfer->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->BtnTransfer->Enabled = false;
            this->BtnTransfer->Location = System::Drawing::Point(240, 352);
            this->BtnTransfer->Name = L"BtnTransfer";
            this->BtnTransfer->Size = System::Drawing::Size(75, 23);
            this->BtnTransfer->TabIndex = 3;
            this->BtnTransfer->Text = L"Transfer";
            this->BtnTransfer->UseVisualStyleBackColor = true;
            // 
            // BtnCancel
            // 
            this->BtnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->BtnCancel->Location = System::Drawing::Point(321, 352);
            this->BtnCancel->Name = L"BtnCancel";
            this->BtnCancel->Size = System::Drawing::Size(75, 23);
            this->BtnCancel->TabIndex = 3;
            this->BtnCancel->Text = L"Cancel";
            this->BtnCancel->UseVisualStyleBackColor = true;
            // 
            // InfoInv
            // 
            this->InfoInv->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->InfoInv->Location = System::Drawing::Point(381, 44);
            this->InfoInv->Name = L"InfoInv";
            this->InfoInv->Size = System::Drawing::Size(240, 17);
            this->InfoInv->TabIndex = 1;
            this->InfoInv->Text = L"N/A";
            // 
            // BtnAll
            // 
            this->BtnAll->Location = System::Drawing::Point(546, 12);
            this->BtnAll->Name = L"BtnAll";
            this->BtnAll->Size = System::Drawing::Size(75, 21);
            this->BtnAll->TabIndex = 4;
            this->BtnAll->Text = L"Add All";
            this->BtnAll->UseVisualStyleBackColor = true;
            this->BtnAll->Click += gcnew System::EventHandler(this, &CmdTransferDlg::BtnAll_Click);
            // 
            // CmdTransferDlg
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(637, 385);
            this->Controls->Add(this->BtnAll);
            this->Controls->Add(this->BtnCancel);
            this->Controls->Add(this->BtnTransfer);
            this->Controls->Add(this->AmountGW);
            this->Controls->Add(this->AmountSG8);
            this->Controls->Add(this->AmountGU8);
            this->Controls->Add(this->AmountFS);
            this->Controls->Add(this->AmountSG4);
            this->Controls->Add(this->AmountGU4);
            this->Controls->Add(this->AmountAU);
            this->Controls->Add(this->AmountTP);
            this->Controls->Add(this->AmountFJ);
            this->Controls->Add(this->AmountSG7);
            this->Controls->Add(this->AmountGU7);
            this->Controls->Add(this->AmountFD);
            this->Controls->Add(this->AmountSG3);
            this->Controls->Add(this->AmountGU3);
            this->Controls->Add(this->AmountIU);
            this->Controls->Add(this->AmountJP);
            this->Controls->Add(this->AmountFM);
            this->Controls->Add(this->AmountSG6);
            this->Controls->Add(this->AmountGU6);
            this->Controls->Add(this->AmountSU);
            this->Controls->Add(this->AmountSG2);
            this->Controls->Add(this->AmountGU2);
            this->Controls->Add(this->AmountCU);
            this->Controls->Add(this->AmountGT);
            this->Controls->Add(this->AmountSG9);
            this->Controls->Add(this->AmountGU9);
            this->Controls->Add(this->AmountDR);
            this->Controls->Add(this->AmountSG5);
            this->Controls->Add(this->AmountGU5);
            this->Controls->Add(this->AmountPD);
            this->Controls->Add(this->AmountSG1);
            this->Controls->Add(this->AmountGU1);
            this->Controls->Add(this->AmountRM);
            this->Controls->Add(label2);
            this->Controls->Add(this->InfoGW);
            this->Controls->Add(this->InfoSG8);
            this->Controls->Add(this->InfoGU8);
            this->Controls->Add(this->InfoFS);
            this->Controls->Add(this->InfoSG4);
            this->Controls->Add(this->InfoGU4);
            this->Controls->Add(this->InfoAU);
            this->Controls->Add(this->InfoTP);
            this->Controls->Add(this->InfoFJ);
            this->Controls->Add(this->InfoSG7);
            this->Controls->Add(this->InfoGU7);
            this->Controls->Add(this->InfoFD);
            this->Controls->Add(this->InfoSG3);
            this->Controls->Add(this->InfoGU3);
            this->Controls->Add(this->InfoIU);
            this->Controls->Add(this->InfoJP);
            this->Controls->Add(this->InfoFM);
            this->Controls->Add(this->InfoSG6);
            this->Controls->Add(this->InfoGU6);
            this->Controls->Add(this->InfoSU);
            this->Controls->Add(this->InfoSG2);
            this->Controls->Add(this->InfoGU2);
            this->Controls->Add(this->InfoCU);
            this->Controls->Add(this->InfoGT);
            this->Controls->Add(this->InfoSG9);
            this->Controls->Add(this->InfoGU9);
            this->Controls->Add(this->InfoDR);
            this->Controls->Add(this->InfoSG5);
            this->Controls->Add(this->InfoGU5);
            this->Controls->Add(this->InfoPD);
            this->Controls->Add(this->InfoSG1);
            this->Controls->Add(this->InfoGU1);
            this->Controls->Add(this->InfoRM);
            this->Controls->Add(label23);
            this->Controls->Add(label81);
            this->Controls->Add(label49);
            this->Controls->Add(label15);
            this->Controls->Add(label80);
            this->Controls->Add(label48);
            this->Controls->Add(label10);
            this->Controls->Add(label30);
            this->Controls->Add(label22);
            this->Controls->Add(label77);
            this->Controls->Add(label45);
            this->Controls->Add(label14);
            this->Controls->Add(label76);
            this->Controls->Add(label44);
            this->Controls->Add(label8);
            this->Controls->Add(label29);
            this->Controls->Add(label21);
            this->Controls->Add(label73);
            this->Controls->Add(label41);
            this->Controls->Add(label13);
            this->Controls->Add(label72);
            this->Controls->Add(label40);
            this->Controls->Add(label6);
            this->Controls->Add(label28);
            this->Controls->Add(label70);
            this->Controls->Add(label38);
            this->Controls->Add(label20);
            this->Controls->Add(label69);
            this->Controls->Add(label37);
            this->Controls->Add(label12);
            this->Controls->Add(label68);
            this->Controls->Add(label36);
            this->Controls->Add(label4);
            this->Controls->Add(this->InfoInv);
            this->Controls->Add(label5);
            this->Controls->Add(this->InfoCapacityUsed);
            this->Controls->Add(label31);
            this->Controls->Add(this->InfoCapacityLimit);
            this->Controls->Add(label3);
            this->Controls->Add(label1);
            this->Controls->Add(this->TransTo);
            this->Controls->Add(this->TransFrom);
            this->Name = L"CmdTransferDlg";
            this->Text = L"Transfer";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountRM))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountCU))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountIU))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountAU))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountPD))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSU))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFD))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFS))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountDR))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFM))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountFJ))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGW))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGT))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountJP))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountTP))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU1))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU5))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU9))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU2))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU6))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU3))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU7))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU4))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountGU8))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG1))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG5))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG9))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG2))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG6))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG3))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG7))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG4))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AmountSG8))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion

private: System::Void TransFrom_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             if( TransFrom->Enabled )
                 InitControlsUnits();
         }
private: System::Void TransTo_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
             if( TransTo->Enabled )
                 InitControlsUnits();
         }
private: System::Void Amounts_Update(System::Object^  sender, System::EventArgs^  e) {
             UpdateAmounts();
         }
private: System::Void Amounts_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
             Amounts_Update(sender, nullptr);
         }
private: System::Void BtnAll_Click(System::Object^  sender, System::EventArgs^  e) {
             AddAll();
         }
};
}
