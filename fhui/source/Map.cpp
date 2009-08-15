#include "StdAfx.h"
#include "Form1.h"
#include "Enums.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

private value struct MapColors
{
    initonly static Color Background        = Color::Black;
    initonly static Color Grid              = Color::FromArgb(12, 12, 12);
    initonly static Color SpeciesSelf       = Color::FromArgb(255, 255, 255);
    initonly static Color Species1          = Color::FromArgb(255, 0, 0);
    initonly static Color Species2          = Color::FromArgb(0, 255, 0);
    initonly static Color Species3          = Color::FromArgb(0, 0, 255);
    initonly static Color SpeciesOther      = Color::FromArgb(100, 140, 120);
    initonly static Color SystemNotExplored = Color::Brown;
    initonly static Color SystemExplored    = Color::Yellow;
};

private value struct MapConstants
{
    initonly static float   RadiusHome      = 120.0F;
    initonly static float   RadiusColony    =  80.0F;
    initonly static float   RadiusSystem    =  50.0F;
};

////////////////////////////////////////////////////////////////
// Map

void Form1::MapSetup()
{
    MapSPSelf->Text = GameData::Player->Name;
}

void Form1::MapDraw()
{
    try
    {
        Graphics ^g = Graphics::FromHwnd( TabMap->Handle );

        RectangleF mapSize = g->VisibleClipBounds;
        m_MapSectorSize = Math::Min(mapSize.Width, mapSize.Height) / GameData::GalaxyDiameter;

        MapDrawGrid(g);
        MapDrawDistances(g);
        MapDrawWormholes(g);
        MapDrawSystems(g);
        MapDrawShips(g);
        MapDrawBattles(g);
    }
    catch( Exception ^e )
    {
        ShowException(e);
    }
}

////////////////////////////////////////////////////////////////

int Form1::MapSpNumFromPtr(Alien ^sp)
{
    if( sp == GameData::Player )
        return 3;

    //TODO: 3 species
    return MapMaxSpecies;
}

Alien^ Form1::MapGetAlien(int sp)
{
    switch( sp )
    {
    case 0:     return MapGetAlienFromUI(MapSP3);
    case 1:     return MapGetAlienFromUI(MapSP2);
    case 2:     return MapGetAlienFromUI(MapSP1);
    case 3:     return GameData::Player;
    default:
        // TODO: exception
        return nullptr;
    }
}

 Alien^ Form1::MapGetAlienFromUI(ComboBox^)
 {
     return nullptr;
 }

StarSystem^ Form1::MapGetRefSystem(int sp)
{
    Alien ^alien = MapGetAlien(sp);
    return alien->HomeSystem;
}

int Form1::MapGetAlienGV(int sp)
{
    Alien ^alien = MapGetAlien(sp);
    return alien->TechLevelsAssumed[TECH_GV];
}

Color Form1::MapGetAlienColor(int sp, double dim)
{
    dim = Math::Min(100.0, dim);
    dim = Math::Max(0.0, dim);

    Color c = Color::Black;

    switch( sp )
    {
    case 0:     c = MapColors::Species3; break;
    case 1:     c = MapColors::Species2; break;
    case 2:     c = MapColors::Species1; break;
    case 3:     c = MapColors::SpeciesSelf; break;
    case 4:     c = MapColors::SpeciesOther; break;
    default:
        // TODO: exception
        break;
    }

    if( dim == 0.0 )
        return c;

    int alpha = (int)( 255 * ( (100 - dim) / 100) );
    return Color::FromArgb(alpha, c);
}

PointF Form1::MapGetSystemXY(StarSystem ^sys)
{
    float halfSectorSize = m_MapSectorSize / 2.0F;
    float x = sys->X * m_MapSectorSize + halfSectorSize;
    float y = (GameData::GalaxyDiameter - (sys->Y + 1)) * m_MapSectorSize + halfSectorSize;
    return PointF(x, y);
}

RectangleF Form1::MapGetSystemRect( StarSystem ^system, float size )
{
    PointF xy = MapGetSystemXY( system );
    float radius = (m_MapSectorSize / 2.0F) * (size / 100.0F);
    return RectangleF(xy.X - radius, xy.Y - radius, 2 * radius, 2 * radius);
}

Brush^ Form1::MapGetBrushLSN(int lsn)
{
    float alpha;

    if( MapEnLSN->Checked )
    {
        int minLsn = Decimal::ToInt32(MapLSNVal->Value);
        if( lsn > minLsn )
            alpha = 0.25F;
        else
            alpha = (50.0F + (((minLsn - lsn) * 50.0F) / minLsn)) / 100.0F;
    }
    else
    {
        int minLsn = GameData::Player->TechLevelsAssumed[TECH_LS];
        alpha = (25.0F + (((minLsn - Math::Min(minLsn, lsn)) * 75.0F) / minLsn)) / 100.0F;
    }

    return gcnew SolidBrush(
        Color::FromArgb( (int)(255 * alpha), MapColors::SystemExplored ) );
}

////////////////////////////////////////////////////////////////

void Form1::MapDrawGrid(Graphics ^g)
{
    g->SmoothingMode = Drawing2D::SmoothingMode::None;

    // Fill background
    g->FillRectangle( Brushes::Black, g->VisibleClipBounds );

    Pen ^pen = gcnew Pen( MapColors::Grid );
    pen->DashStyle = Drawing2D::DashStyle::Dot;

    float maxX = GameData::GalaxyDiameter * m_MapSectorSize;
    float maxY = GameData::GalaxyDiameter * m_MapSectorSize;
    for( int i = 0; i <= GameData::GalaxyDiameter; ++i )
    {
        float f = i * m_MapSectorSize;
        g->DrawLine( pen, f, 0.0, f, maxY );
        g->DrawLine( pen, 0.0, f, maxX, f );
    }
}

void Form1::MapDrawDistances(Graphics ^g)
{
    g->SmoothingMode = Drawing2D::SmoothingMode::HighQuality;

    if( MapEnDist->Checked == false )
        return;

    for( int sp = 0; sp < MapMaxSpecies; ++sp )
    {
        Alien ^alien = MapGetAlien(sp);
        if( alien == nullptr )
            continue;

        // From reference system
        StarSystem ^system = MapGetRefSystem(sp);
        if( system )
        {
            for each( StarSystem ^s in m_GameData->GetStarSystems() )
                MapDrawDistance(g, sp, system, s);
        }
    }
}

void Form1::MapDrawDistance(Graphics ^g, int sp, StarSystem ^sysFrom, StarSystem ^sysTo)
{
    if( sysFrom == sysTo )
        return;

    if( MapEnLSN->Checked &&
        sysTo->MinLSN > MapLSNVal->Value )
        return;

    int gv = MapGetAlienGV(sp);
    double mishap = sysFrom->CalcMishap(sysTo, gv, 0);

    Color color = MapGetAlienColor(sp, mishap * 3);

    g->DrawLine(
        gcnew Pen( color ),
        MapGetSystemXY(sysFrom),
        MapGetSystemXY(sysTo) );
}

void Form1::MapDrawWormholes(Graphics ^g)
{
}

void Form1::MapDrawSystems(Graphics ^g)
{
    Alien ^sp1 = MapGetAlien(0);
    Alien ^sp2 = MapGetAlien(1);
    Alien ^sp3 = MapGetAlien(2);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        List<Colony^> ^colonies = system->Colonies;

        bool bIsHome = false;
        int homeSp = MapMaxSpecies;
        for each( Colony ^colony in colonies )
        {
            if( colony->PlanetType == PLANET_HOME )
            {
                bIsHome = true;
                homeSp = MapSpNumFromPtr(colony->Owner);
            }
        }
        if( bIsHome )
        {
            g->FillEllipse(
                gcnew SolidBrush( MapGetAlienColor(homeSp, 0.0) ),
                MapGetSystemRect( system, MapConstants::RadiusHome ) );
        }

        int cntAll = colonies->Count;
        if( cntAll )
        {   // System with colonies
            float sliceAngle = 360.0F / cntAll;
            float startAngle = -90.0F;
            RectangleF rect = MapGetSystemRect( system, MapConstants::RadiusColony );
            for each( Colony ^colony in colonies )
            {
                Brush ^brush = gcnew SolidBrush(
                    MapGetAlienColor(MapSpNumFromPtr(colony->Owner), 0.0) );
                g->FillPie( brush,
                    rect.X, rect.Y, rect.Width, rect.Height,
                    startAngle, sliceAngle );
                startAngle += sliceAngle;
            }
        }

        Brush ^brush;
        if( system->IsExplored() )
            brush = MapGetBrushLSN( system->MinLSN );
        else
            brush = gcnew SolidBrush( MapColors::SystemNotExplored );

        g->FillEllipse( brush, MapGetSystemRect( system, MapConstants::RadiusSystem ) );
    }
}

void Form1::MapDrawShips(Graphics ^g)
{
}

void Form1::MapDrawBattles(Graphics ^g)
{
}

} // end namespace FHUI
