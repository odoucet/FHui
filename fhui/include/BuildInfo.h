#pragma once

private ref class BuildInfo sealed
{
public:
    initonly static String^ Version = GetVersion();

    initonly static String^ ContactWiki   = "http://www.cetnerowski.com/farhorizons/pmwiki/pmwiki.php/FHUI/FHUI";;
    initonly static String^ ContactEmails = "jdukat+fhui@gmail.com";

private:
    static String^  GetVersion();
};
