#include "StdAfx.h"
#include "Form1.h"
#include "Enums.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// Order Template

void Form1::AddTemplateLine(String^ line)
{
}

void Form1::GenerateTemplate()
{
    GenerateCombat();
    GeneratePreDeparture();
    GenerateJumps();
    GenerateProduction();
    GeneratePostArrival();
    GenerateStrikes();
}

void Form1::GenerateCombat()
{
}

void Form1::GeneratePreDeparture()
{
}

void Form1::GenerateJumps()
{
}

void Form1::GenerateProduction()
{
}

void Form1::GeneratePostArrival()
{
}

void Form1::GenerateStrikes()
{
}

} // end namespace FHUI
