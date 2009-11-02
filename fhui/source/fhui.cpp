// fhui.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"
#include "BuildInfo.h"

#using <System.Xml.Dll>
using namespace System::Diagnostics;
using namespace System::IO;
using namespace FHUI;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
    Debug::WriteLine( String::Format("FHUI build {0} starting...", BuildInfo::Version ) );

    // Enabling Windows XP visual effects before any controls are created
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false); 

    RegexMatcher^ GlobalRegexMatcher = gcnew RegexMatcher;

    System::String^ dataDir = nullptr;
    bool plugins = true;
    bool profile = false;
    bool verbose = false;
    bool stats = false;

    for( int i = 0; i < args->Length; ++i )
    {
        if( args[i]->ToLower() == "-dir" && i < (args->Length - 1) )
        {
            dataDir = args[i + 1];
            if( dataDir->EndsWith("\"") )
            {
                // Prevent a crash when invoked as: fhui.exe -dir "C:\My Dir\"
                dataDir = dataDir->TrimEnd(String("\"").ToCharArray());
            }
            continue;
        }
        if( args[i]->ToLower() == "-public" )
        {
            plugins = false;
        }
        if( args[i]->ToLower() == "-profile" )
        {
            profile = true;
        }
        if( args[i]->ToLower() == "-verbose" )
        {
            verbose = true;
        }
        if( args[i]->ToLower() == "-stats" )
        {
            stats = true;
        }
        if( args[i]->ToLower() == "-regexp" )
        {
            GlobalRegexMatcher->CollectStats = true;
        }
    }

    if( profile )
    {
        DirectoryInfo ^dir = gcnew DirectoryInfo(dataDir);
        Stopwatch^ timerSingle = gcnew Stopwatch();
        Stopwatch^ timerComplete = gcnew Stopwatch();

        timerComplete->Start();

        for each( DirectoryInfo ^subDir in dir->GetDirectories() )
        {
            timerSingle->Start();

            Form1 ^fhui = gcnew Form1;
            fhui->DataDir = subDir->FullName;
            fhui->EnablePlugins = plugins;
            fhui->Verbose = verbose;
            fhui->Stats = stats;
            fhui->SetRM( GlobalRegexMatcher );
            fhui->Initialize();

            timerSingle->Stop();

            Debug::WriteLine(
                String::Format("{0}: {1:00}:{2:00}.{3:000}",
                subDir->Name,
                timerSingle->Elapsed.Minutes,
                timerSingle->Elapsed.Seconds,
                timerSingle->Elapsed.Milliseconds) );

            timerSingle->Reset();
        }

        timerComplete->Stop();

        Debug::WriteLine(
            String::Format("Total: {0:00}:{1:00}.{2:000}",
            timerComplete->Elapsed.Minutes,
            timerComplete->Elapsed.Seconds,
            timerComplete->Elapsed.Milliseconds) );

        if( GlobalRegexMatcher->CollectStats )
            GlobalRegexMatcher->PrintDebugStats();
    }
    else
    {
        Stopwatch^ timer = gcnew Stopwatch();
        timer->Start();

        Form1 ^fhui = gcnew Form1;
        fhui->DataDir = dataDir;
        fhui->EnablePlugins = plugins;
        fhui->Verbose = verbose;
        fhui->Stats = stats;

        fhui->Initialize();

        timer->Stop();
        Debug::WriteLine( String::Format("Initialization took {0}.{1:000} s",
            timer->Elapsed.Seconds,
            timer->Elapsed.Milliseconds) );
        
        // Create the main window and run it
        Application::Run(fhui);
    }

    return 0;
}
