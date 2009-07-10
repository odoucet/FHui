#pragma once

using namespace System;
using namespace System::Data;
using namespace System::Windows::Forms;

namespace FHUI
{

ref class StarSystem;
ref class Planet;
ref class Colony;
ref class Alien;
ref class Ship;
interface class IGridFilter;

public interface class IGridPlugin
{
public:
    void        AddColumnsSystems(DataTable^);
    void        AddRowDataSystems(DataRow^, StarSystem^, IGridFilter^);
    void        GridFormatSystems(DataGridView^);

    void        AddColumnsPlanets(DataTable^);
    void        AddRowDataPlanets(DataRow^, Planet^, IGridFilter^);
    void        GridFormatPlanets(DataGridView^);

    void        AddColumnsColonies(DataTable^);
    void        AddRowDataColonies(DataRow^, Colony^, IGridFilter^);
    void        GridFormatColonies(DataGridView^);

    void        AddColumnsShips(DataTable^);
    void        AddRowDataShips(DataRow^, Ship^, IGridFilter^);
    void        GridFormatShips(DataGridView^);

    void        AddColumnsAliens(DataTable^);
    void        AddRowDataAliens(DataRow^, Alien^, IGridFilter^);
    void        GridFormatAliens(DataGridView^);
};

} // end namespace FHUIPlugin
