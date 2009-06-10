#include "StdAfx.h"
#include "Form1.h"

#include "Report.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

namespace fhui 
{

void Form1::LoadReports()
{
    try
    {
        if( LoadReport("C:\\Games\\FarHorizons\\Reports\\t14.txt") )
        {
            Report ^report = dynamic_cast<Report^>(m_Reports->GetByIndex(0));
            if( report )
            {
                RepText->Text = report->GetContent();
                Summary->Text = report->GetSummary();
            }
        }
    }
    catch( SystemException ^e )
    {
        Summary->Text = "Failed loading report.";
        RepText->Text = e->ToString();
    }
}

bool Form1::LoadReport(const char *fileName)
{
    Report ^report = gcnew Report(m_GameData);

    string line;
    ifstream ifs(fileName);
    while( getline(ifs, line) )
    {
        if( false == report->Parse(line) )
            break;
    }

    if( report->IsValid() )
    {
        m_Reports->Add(report->GetTour(), report);
        return true;
    }

    return false;
}

void Form1::LoadCommands()
{
}

void Form1::InitData()
{
    m_GameData = gcnew GameData;

    RepMode->SelectedIndex = 1;
}

}
