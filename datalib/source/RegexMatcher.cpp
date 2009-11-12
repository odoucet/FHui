#include "StdAfx.h"
#include "RegexMatcher.h"

using namespace System::Diagnostics;

namespace FHUI
{

RegexMatcher::RegexMatcher()
{
    m_Results = gcnew array<String^>(1);

    m_Stats = gcnew SortedList<String^, Pair<int, int>^>;

    ExpCmdColony        = gcnew Regex("^COLONY ([^,;]+)$");
    ExpCmdShip          = gcnew Regex("^SHIP ([^,;]+)$");
    ExpCmdPLName        = gcnew Regex("^Name\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+PL\\s+([^,;]+)$");
    ExpCmdPLDisband     = gcnew Regex("^Disband\\s+PL\\s+([^,;]+)$");
    ExpCmdSPNeutral     = gcnew Regex("^Neutral\\s+SP\\s+([^,;]+)$");
    ExpCmdSPAlly        = gcnew Regex("^Ally\\s+SP\\s+([^,;]+)$");
    ExpCmdSPEnemy       = gcnew Regex("^Enemy\\s+SP\\s+([^,;]+)$");
    ExpCmdSPTeach       = gcnew Regex("^Teach\\s+([A-Z]{2})\\s+(\\d+\\s+)?SP\\s+([^,;]+)$");
    ExpCmdSPMsg         = gcnew Regex("^Message\\s+SP\\s+([^,;]+)$");
    ExpCmdResearch      = gcnew Regex("^Research\\s+(\\d+)\\s+([A-Z]{2})$");
    ExpCmdRecycle       = gcnew Regex("^Recycle\\s+(\\d+)\\s+(\\w+)$");
    ExpCmdEstimate      = gcnew Regex("^Estimate\\s+SP\\s+([^,;]+)$");
    ExpCmdInstall       = gcnew Regex("^Install\\s+(\\d+)\\s+([IAia][Uu])\\s+PL\\s+([^,;]+)$");
    ExpCmdBuildIUAU     = gcnew Regex("^Build\\s+(\\d+)\\s+([IAC]U)");  // no $ here, target may follow
    ExpCmdBuildInv      = gcnew Regex("^Build\\s+(\\d+)\\s+([A-Z0-9]+)");  // no $ here, target may follow
    ExpCmdBuildShipTR   = gcnew Regex("^Build\\s+TR(\\d+)([Ss]?)\\s+(.+)$");
    ExpCmdBuildShip     = gcnew Regex("^Build\\s+([A-Za-z]{2})([Ss]?)\\s+(.+)$");
    ExpCmdDevelopCS     = gcnew Regex("^Develop\\s+(\\d+\\s+)?PL\\s+([^,;]+),\\s+TR[0-9]+\\s+([^,;]+)$");
    ExpCmdDevelopC      = gcnew Regex("^Develop\\s+(\\d+)\\s+PL\\s+([^,;]+)$");
    ExpCmdDevelop       = gcnew Regex("^Develop\\s+(\\d+)$");
    ExpCmdShipJump      = gcnew Regex("^Jump\\s+[A-Z0-9]+\\s+[^,;]+,\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipMove      = gcnew Regex("^Move\\s+[A-Z0-9]+\\s+[^,;]+,\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$");
    ExpCmdShipWormhole  = gcnew Regex("^Wormhole\\s+[A-Z0-9]+\\s+[^,;]+,\\s+([0-9-]+)$");
    ExpCmdShipUpg       = gcnew Regex("^Upgrade\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipRec       = gcnew Regex("^Recycle\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipUnload    = gcnew Regex("^Unload\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipScan      = gcnew Regex("^Scan\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipDeep      = gcnew Regex("^Deep\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipLand      = gcnew Regex("^Land\\s+[A-Z0-9]+\\s+([^,;]+)$");
    ExpCmdShipOrbitPLName   = gcnew Regex("^Orbit\\s+[A-Z0-9]+\\s+([^,;]+)\\s*,\\s+PL\\s+([^,;]+)$");
    ExpCmdShipOrbitPLNum    = gcnew Regex("^Orbit\\s+[A-Z0-9]+\\s+([^,;]+)\\s*,\\s+(\\d+)$");
    ExpCmdVisited       = gcnew Regex("^Visited\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$");
    ExpCmdTransfer      = gcnew Regex("^Transfer\\s+(\\d+)\\s+(\\w+)\\s+");
    ExpCmdCustom        = gcnew Regex("^CUSTOM\\s+([0-9-]+)\\s+(.+)$");

    ExpCmdTargetColony  = gcnew Regex("^PL\\s+([^,;]+)");
    ExpCmdTargetShip    = gcnew Regex("^[A-Z0-9]+\\s+([^,;]+)");

    ExpCmdShipJump_Obsolete     = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Jump to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipWormhole_Obsolete = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Wormhole\\s+([0-9-]+)$");
    ExpCmdShipUpg_Obsolete      = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Upgrade$");
    ExpCmdShipRec_Obsolete      = gcnew Regex("^SHIP [A-Z0-9]+ ([^,;]+) Recycle$");
}

int RegexMatcher::GetResultInt(int arg)
{
    if( String::IsNullOrEmpty( m_Results[arg] ) )
        return 0;
    else
        return int::Parse(m_Results[arg]);
}

//float RegexMatcher::GetResultFloat(int arg)
//{
//    return Single::Parse(
//        m_Results[arg],5
//        Globalization::CultureInfo::InvariantCulture );
//}

bool RegexMatcher::Match(String ^%s, String ^exp)
{
    if( String::IsNullOrEmpty(s) )
    {
        return false;
    }
    else
    {
        return Match(s, gcnew Regex(exp));
    }
}

bool RegexMatcher::Match(String ^%s, Regex ^exp)
{
    bool retVal = false;
    Array::Clear(m_Results, 0, m_Results->Length);

    System::Text::RegularExpressions::Match ^m = exp->Match(s);
    if( m->Success )
    {
        //if( exp->ToString() == "^Auto$" )
        //{
        //    Debug::WriteLine( s );        
        //}
        String ^g = m->Groups[0]->ToString();
        s = s->Substring( s->IndexOf(g) + g->Length );

        int cnt = m->Groups->Count - 1;
        Array::Resize(m_Results, cnt);
        for( int i = 0; i < cnt; ++i )
        {
            m_Results[i] = m->Groups[i + 1]->ToString();
        }
        retVal = true;
    }

    if( CollectStats )
    {
        String^ key = exp->ToString();
        if( m_Stats->ContainsKey( key ) )
        {
            retVal ? ( m_Stats[key]->A ++ ) : ( m_Stats[key]->B ++ );
        }
        else
        {
            m_Stats->Add( exp->ToString(), gcnew Pair<int,int>(retVal ? 1 : 0, retVal ? 0 : 1) );
        }
    }

    return retVal;
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

void RegexMatcher::PrintDebugStats()
{
    int totalHits = 0;
    int totalMisses = 0;

    for( int i = 0; i < m_Stats->Count; i++)
    {
        int hits = m_Stats->Values[i]->A;
        int misses = m_Stats->Values[i]->B;
        double ratio = (double)misses / hits;
        totalHits += hits;
        totalMisses += misses;

        Debug::WriteLine( String::Format("H:{0,6} M:{1,6} \"{2}\"", hits, misses, m_Stats->Keys[i] ) );
    }

    Debug::WriteLine( 
        String::Format("Regular expressions used: {0}, Hits: {1}, Misses: {2} (ratio 1:{3:F0})",
        m_Stats->Count, totalHits, totalMisses, (double)totalMisses / totalHits ) );
}

} // end namespace FHUI
