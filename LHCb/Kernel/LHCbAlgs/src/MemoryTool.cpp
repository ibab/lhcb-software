// $Id: MemoryTool.cpp,v 1.6 2009-10-08 11:18:26 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/Memory.h"
// =============================================================================

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
  : GaudiHistoTool ( type, name , parent )
  , m_counter(0)
  , m_bins(0)
  // 
  , m_skip   ( 10     )
  , m_prev   ( -1.e-6 )
  , m_histo1 ( "Total Memory [MB]" ,   0 , 2000 ) 
  , m_histo2 ( "Delta Memory [MB]" , -25 ,   25 ) 
                      //
  , m_totMem ( 0      )
  , m_delMem ( 0      )
  , m_plot1  ( 0      )
  , m_plot2  ( 0      )
{
  declareProperty( "HistoSize"  , m_bins = 500 );
  //
  declareProperty 
    ( "SkipEvents" , 
      m_skip       , 
      "Skip the first N events from delta memory counter " );
  declareProperty 
    ( "TotalMemoryHisto" , 
      m_histo1      , 
      "The parameters of 'total memory' histogram" ) ;
  declareProperty 
    ( "DeltaMemoryHisto" , 
      m_histo2      , 
      "The parameters of 'delta memory' histogram" ) ;
  //
  setProperty ( "HistoPrint", false );
  declareInterface<IGenericTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MemoryTool::~MemoryTool() {}
// ============================================================================

//=============================================================================
// Plot the memory usage
//=============================================================================
void MemoryTool::execute() {

  double mem = (double)System::virtualMemory();
  
  static const std::string s_TotalMemory = "Total Memory/MB" ;
  static const std::string s_DeltaMemory = "Delta Memory/MB" ;
  
  if ( 0 == m_totMem                   ) { m_totMem = &counter( s_TotalMemory ) ; }
  if ( 0 == m_delMem                   ) { m_delMem = &counter( s_DeltaMemory ) ; }
  if ( 0 == m_plot1 && produceHistos() ) { m_plot1  = book    ( m_histo1      ) ; }
  if ( 0 == m_plot2 && produceHistos() ) { m_plot2  = book    ( m_histo2      ) ; }
  
  // memory in megabytes 
  const double memMB = mem / 1000 ;
  // reset the counter (if required ) 
  if      ( 0 < m_skip && (unsigned long) m_skip == m_counter && 0 != m_delMem ) 
  {
    // NB: I hate StatEntity::reset, use == instead!
    *m_delMem = StatEntity() ;
    debug()  << "Reset Delta Virtual Memory counter" << endmsg ;
  } 
  // Fill the counter for "valid" previous measurements
  if   ( 0 != m_totMem ) { *m_totMem += memMB ; }
  // Fill the plot
  if   ( 0 != m_plot1  ) { fill ( m_plot1 , memMB , 1 , m_histo1.title() ) ; }
  // Fill the counter for "valid" previous measurements
  if   ( 0 <= m_prev   )
  { 
    const double delta = memMB - m_prev ;
    // fill the counter 
    if ( 0 != m_delMem ) { *m_delMem += delta  ; }
    // fill the counter 
    if ( 0 != m_plot2  ) { fill ( m_plot2 , delta , 1 , m_histo2.title() ) ; }
  } 
  
  // set "previous" measurement 
  m_prev = memMB    ;                                        // memory in MB
  
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
