#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

interface class ICommand
{
    void    Print(List<String^>^);
};

typedef List<ICommand^>     CommandListT;

} // end namespace FHUI
