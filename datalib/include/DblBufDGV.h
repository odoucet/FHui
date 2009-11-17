#pragma once

#include "IGridData.h"

using namespace System;
using namespace System::Windows::Forms;

namespace FHUI {

public delegate void DblBufDGVMarkedForUpdateHandler();

public ref class DblBufDGV :  public System::Windows::Forms::DataGridView
{
public:
	DblBufDGV(void)
	{
        this->DoubleBuffered = true;
        UpdatePending = false;
	}

    property IGridFilter^   Filter;
    property IGridSorter^   Sorter;

    property bool           UpdatePending;
    event DblBufDGVMarkedForUpdateHandler^  MarkedForUpdate;

    void MarkForUpdate()
    {
        UpdatePending = true;
        OnMarkedForUpdate();
    }

    void OnMarkedForUpdate()
    {
        MarkedForUpdate();
    }

    void FullUpdateBegin()
    {
        UpdatePending = false;

        // Disable header size updates
        AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;

        // Store selected rows
        StoreSelection();

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

        RestoreSelection();
    }

protected:
    void StoreSelection()
    {
        if( SelectedRows->Count > 0 )
        {
            m_StoredSelection = gcnew List<Object^>;
            for each( DataGridViewRow ^r in SelectedRows )
            {
                Object ^o = r->Cells[0]->Value;
                m_StoredSelection->Add( o );
            }
        }
        else
            m_StoredSelection = nullptr;
    }

    void RestoreSelection()
    {
        ClearSelection();
        if( m_StoredSelection == nullptr )
            return;

        for each( DataGridViewRow ^row in Rows )
        {
            Object ^o = row->Cells[0]->Value;
            for each( Object ^oSel in m_StoredSelection )
                if( oSel == o )
                {
                    row->Selected = true;
                    break;
                }
        }
    }

    List<Object^>^  m_StoredSelection;
};

}
