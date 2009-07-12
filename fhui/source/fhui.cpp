// fhui.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace FHUI;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
    // Enabling Windows XP visual effects before any controls are created
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false); 

    System::String^ dataDir = nullptr;
    for( int i = 0; i < args->Length; ++i )
    {
        if( args[i]->ToLower() == "-dir" && i < (args->Length - 1) )
            dataDir = args[i + 1];
    }

    Form1 ^fhui = gcnew Form1;
    fhui->DataDir = dataDir;
    fhui->LoadGameData();

    // Create the main window and run it
    Application::Run(fhui);
    return 0;
}
