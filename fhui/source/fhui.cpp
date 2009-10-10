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
    bool plugins = true;
    for( int i = 0; i < args->Length; ++i )
    {
        if( args[i]->ToLower() == "-dir" && i < (args->Length - 1) )
        {
            dataDir = args[i + 1];
            if( dataDir->EndsWith("\"") )
            {
                // TODO: Prevent a crash when invoked as: fhui.exe -dir "C:\My Dir\"
                // dataDir->Remove(dataDir->Length - 1);
                // dataDir->TrimEnd(String("\"").ToCharArray());
            }
            continue;
        }
        if( args[i]->ToLower() == "-public" )
        {
            plugins = false;
        }
    }

    Form1 ^fhui = gcnew Form1;
    fhui->DataDir = dataDir;
    fhui->EnablePlugins = plugins;
    fhui->Initialize();

    // Create the main window and run it
    Application::Run(fhui);
    return 0;
}
