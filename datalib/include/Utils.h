#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

generic<typename PairTypeA, typename PairTypeB>
public ref class Pair
{
public:
    Pair(PairTypeA first, PairTypeB second)
    {
        this->A = first;
        this->B = second;
    }

    property PairTypeA A;
    property PairTypeB B;
};

generic<typename TripleTypeA, typename TripleTypeB, typename TripleTypeC>
public ref class Triple : public Pair<TripleTypeA, TripleTypeB>
{
public:
    Triple(TripleTypeA first, TripleTypeB second, TripleTypeC third)
        : Pair(first, second)
    {
        this->C = third;
    }

    property TripleTypeC C;
};

// ---------------------------------------------------

} // end namespace FHUI
