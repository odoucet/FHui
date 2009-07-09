#pragma once

using namespace System;

namespace FHUI
{

ref class BuildInfo sealed
{
public:
    initonly static String^ Version = GetVersion();

    initonly static String^ ContactWiki   = "http://www.cetnerowski.com/farhorizons/pmwiki/pmwiki.php/FHUI/FHUI";;
    initonly static String^ ContactEmails = "jdukat+fhui@gmail.com";

private:
    static String^  GetVersion();
};

} // end namespace FHUI
