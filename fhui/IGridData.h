#pragma once

ref class Alien;

interface class IGridDataSrc
{
    Alien^  GetAlienForBgColor();
    String^ GetTooltipText();
};
