#include "StdAfx.h"
#include "RegexMatcher.h"

namespace FHUI
{

RegexMatcher::RegexMatcher()
{
    m_Results = gcnew array<String^>(1);

    ExpCmdColony        = gcnew Regex("^COLONY ([^,;]+)$");
    ExpCmdShip          = gcnew Regex("^SHIP ([^,;]+)$");
    ExpCmdPLName        = gcnew Regex("^Name\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+PL\\s+([^,;]+)$");
    ExpCmdPLDisband     = gcnew Regex("^Disband\\s+PL\\s+([^,;]+)$");
    ExpCmdSPNeutral     = gcnew Regex("^Neutral\\s+SP\\s+([^,;]+)$");
    ExpCmdSPAlly        = gcnew Regex("^Ally\\s+SP\\s+([^,;]+)$");
    ExpCmdSPEnemy       = gcnew Regex("^Enemy\\s+SP\\s+([^,;]+)$");
    ExpCmdSPTeach       = gcnew Regex("^Teach\\s+([A-Z]{2})\\s+(\\d+)\\s+SP\\s+([^,;]+)$");
    ExpCmdSPMsg         = gcnew Regex("^Message\\s+SP\\s+([^,;]+)$");
    ExpCmdResearch      = gcnew Regex("^Research\\s+(\\d+)\\s+([A-Z]{2})$");
    ExpCmdRecycle       = gcnew Regex("^Recycle\\s+(\\d+)\\s+(\\w+)$");
    ExpCmdEstimate      = gcnew Regex("^Estimate\\s+SP\\s+([^,;]+)$");
    ExpCmdInstall       = gcnew Regex("^Install\\s+(\\d+)\\s+([IAia][Uu])\\s+PL\\s+([^,;]+)$");
    ExpCmdBuildIUAU     = gcnew Regex("^Build\\s+(\\d+)\\s+([IAC]U)");  // no $ here, target may follow
    ExpCmdBuildShipTR   = gcnew Regex("^Build\\s+TR(\\d+)([Ss]?)\\s+(.+)$");
    ExpCmdBuildShip     = gcnew Regex("^Build\\s+([A-Za-z]{2})([Ss]?)\\s+(.+)$");
    ExpCmdDevelopCS     = gcnew Regex("^Develop\\s+(\\d+)\\s+PL\\s+([^,;]+),\\s+TR[0-9]+\\s+([^,;]+)$");
    ExpCmdDevelopC      = gcnew Regex("^Develop\\s+(\\d+)\\s+PL\\s+([^,;]+)$");
    ExpCmdDevelop       = gcnew Regex("^Develop\\s+(\\d+)$");
    ExpCmdShipJump      = gcnew Regex("^Jump\\s+[A-Z0-9]+\\s+[^,;]+,\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipWormhole  = gcnew Regex("^Wormhole\\s+[A-Z0-9]+\\s+[^,;]+,\\s+([0-9-]+)$");
    ExpCmdShipUpg       = gcnew Regex("^Upgrade\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipRec       = gcnew Regex("^Recycle\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipUnload    = gcnew Regex("^Unload\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipScan      = gcnew Regex("^Scan\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipDeep      = gcnew Regex("^Deep\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipLand      = gcnew Regex("^Land\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdVisited       = gcnew Regex("^Visited\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$");
    ExpCmdCustom        = gcnew Regex("^CUSTOM\\s+(\\d+)\\s+(.+)$");

    ExpCmdTargetColony  = gcnew Regex("^PL\\s+([^,;]+)$");
    ExpCmdTargetShip    = gcnew Regex("^[A-Z0-9]+\\s+([^,;]+)$");

    ExpCmdShipJump_Obsolete     = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Jump to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipWormhole_Obsolete = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Wormhole\\s+([0-9-]+)$");
    ExpCmdShipUpg_Obsolete      = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Upgrade$");
    ExpCmdShipRec_Obsolete      = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Recycle$");
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
