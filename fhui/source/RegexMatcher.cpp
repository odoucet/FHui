#include "StdAfx.h"
#include "RegexMatcher.h"

namespace FHUI
{

RegexMatcher::RegexMatcher()
{
    m_Results = gcnew array<String^>(1);

    ExpCmdColony    = gcnew Regex("^COLONY ([^,;]+)$");
    ExpCmdShipJump  = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Jump to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipUpg   = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Upgrade$");
    ExpCmdShipRec   = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Recycle$");
    ExpCmdPLName    = gcnew Regex("^Name\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+) PL ([^,;]+)$");
    ExpCmdPLDisband = gcnew Regex("^Disband PL ([^,;]+)$");
    ExpCmdSPNeutral = gcnew Regex("^Neutral SP ([^,;]+)$");
    ExpCmdSPAlly    = gcnew Regex("^Ally SP ([^,;]+)$");
    ExpCmdSPEnemy   = gcnew Regex("^Enemy SP ([^,;]+)$");
    ExpCmdSPTeach   = gcnew Regex("^Teach ([A-Z]{2}) (\\d+) SP ([^,;]+)$");
    ExpCmdShipWormhole= gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Wormhole\\s+([0-9-]+)$");
    ExpCmdResearch  = gcnew Regex("^Research (\\d+) ([A-Z]{2})$");
}

int RegexMatcher::GetResultInt(int arg)
{
    return int::Parse(m_Results[arg]);
}

//float RegexMatcher::GetResultFloat(int arg)
//{
//    return Single::Parse(
//        m_Results[arg],
//        Globalization::CultureInfo::InvariantCulture );
//}

bool RegexMatcher::Match(String ^%s, String ^exp)
{
    return Match(s, gcnew Regex(exp));
}

bool RegexMatcher::Match(String ^%s, Regex ^exp)
{
    Array::Clear(m_Results, 0, m_Results->Length);

    System::Text::RegularExpressions::Match ^m = exp->Match(s);
    if( m->Success )
    {
        String ^g = m->Groups[0]->ToString();
        s = s->Substring( s->IndexOf(g) + g->Length );

        int cnt = m->Groups->Count - 1;
        Array::Resize(m_Results, cnt);
        for( int i = 0; i < cnt; ++i )
        {
            m_Results[i] = m->Groups[i + 1]->ToString();
        }
        return true;
    }

    return false;
}

bool RegexMatcher::MatchList(String ^s, String ^prefix, String ^exp)
{
    Array::Clear(m_Results, 0, m_Results->Length);
    int cnt = 0;

    String ^exprInit = String::Format("{0}\\s*{1}", prefix, exp);
    String ^exprCont = String::Format("^\\s*,\\s*{0}", exp);
    System::Text::RegularExpressions::Match ^m = Regex(exprInit).Match(s);
    while( m->Success )
    {
        Array::Resize(m_Results, cnt + 1);
        m_Results[cnt++] = m->Groups[1]->ToString();

        int l = m->Groups[0]->ToString()->Length;
        if( s->Length == l )
            break;
        s = s->Substring(l);

        m = Regex(exprCont).Match(s);
    }

    return cnt > 0;
}

} // end namespace FHUI
