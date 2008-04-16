// $Id: MemoryTool.cpp,v 1.5 2008-04-16 08:11:25 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/Memory.h"

// local
#include "MemoryTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MemoryTool
//
// 2005-12-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MemoryTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MemoryTool::MemoryTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiHistoTool ( type, name , parent ), m_counter(0), m_bins(0)
{
  declareProperty( "HistoSize", m_bins = 500 );
  setProperty( "HistoPrint", false );
  declareInterface<IGenericTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MemoryTool::~MemoryTool() {}; 

//=============================================================================
// Plot the memory usage
//=============================================================================
void MemoryTool::execute() {

  double mem = (double)System::virtualMemory();

  if( m_bins > m_counter ) {
    plot( m_counter+1, "Virtual mem, all entries", "Virtual memory (kB), first 'HistoSize' entries",
          0.5, m_bins+0.5, m_bins, mem );
  }
  if( 0 == m_counter%m_bins ) {
    unsigned int bin = 1 + ( m_counter/m_bins );
    plot( bin, "Virtual mem, downscaled", "Virtual memory (kB), downscaled entries",
          0.5, m_bins+0.5, m_bins, mem );
  }
  ++m_counter;
}; 

//=============================================================================
