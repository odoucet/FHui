#pragma once

#include "IGridData.h"

using namespace System;
using namespace System::Windows::Forms;

namespace FHUI {

public ref class DblBufDGV :  public System::Windows::Forms::DataGridView
{
public:
	DblBufDGV(void)
	{
        this->DoubleBuffered = true;
	}

    property IGridFilter^   Filter;
    property IGridSorter^   Sorter;

    void FullUpdateBegin()
    {
        // Disable header size updates
        AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
        // Remove all data
        Rows->Clear();
    }

    void FullUpdateEnd()
    {
        // Sort data
        if( Sorter )
            Sort( Sorter );

        // Enable filters
        if( Filter )
            Filter->EnableUpdates = true;

        // Enable header size updates
        AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::AllCells;
    }
};

}
