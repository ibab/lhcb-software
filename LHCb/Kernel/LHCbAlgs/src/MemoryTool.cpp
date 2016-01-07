// =============================================================================
// Include files 
// =============================================================================
// from Gaudi
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
DECLARE_TOOL_FACTORY( MemoryTool )

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
MemoryTool::MemoryTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
  , m_counter ( 0 )
  , m_bins    ( 0 )
                      // 
  , m_skip   ( 10     )
  , m_prev   ( -1.e+6 )
  , m_histo1 ( "Total Memory [MB]" ,   0 , 2000 ) 
  , m_histo2 ( "Delta Memory [MB]" , -25 ,   25 )
  , m_check     ( 20 )
  , m_maxPrint  (  0 )
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
  declareProperty 
    ( "Check"  , 
      m_check  , 
      "Frequency for checks for suspision memory leak" ) ;
  //
  declareProperty 
    ( "MaxPrints" , 
      m_maxPrint  , 
      "Maximal number of print-out" ) ;
  //
  setProperty ( "HistoPrint", false );
  declareInterface<IGenericTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MemoryTool::~MemoryTool() {}
// ============================================================================
// finalize the tool 
// ============================================================================
StatusCode MemoryTool::finalize () 
{
  if ( 0 != m_delMem                && 
       1 < m_delMem->nEntries    () && 
       0 < m_delMem->flagMean    () && 
       0 < m_delMem->flagMeanErr () && 
       m_delMem->flagMean() > 3 * m_delMem->flagMeanErr() ) 
  {
    Warning( "Mean 'delta-memory' exceeds 3*sigma", StatusCode::SUCCESS).ignore();
  }
  
  m_totMem = 0 ;
  m_delMem = 0 ;
  m_plot1  = 0 ;
  m_plot2  = 0 ;
  return GaudiHistoTool::finalize (); 
}
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
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()  << "Reset Delta Virtual Memory counter" << endmsg ;
  } 
  // Fill the counter for "valid" previous measurements
  if   ( 0 != m_totMem ) { *m_totMem += memMB ; }
  // Fill the plot
  if   ( 0 != m_plot1  ) { fill ( m_plot1 , memMB , 1 , m_histo1.title() ) ; }
  // Fill the counter for "valid" previous measurements
  const double deltaMem = memMB - m_prev ;
  if   ( 0 <= m_prev   )
  { 
    // fill the counter 
    if ( 0 != m_delMem ) { *m_delMem += deltaMem ; }
    // fill the counter
    if ( 0 != m_plot2  ) { fill ( m_plot2 , deltaMem , 1 , m_histo2.title() ) ; }
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
  
  /// increment event counter 
  ++m_counter;                                         // increment event counter
  
  // check Total Memory for the particular event
  if ( 0  != m_totMem                    && 
       16 <  m_totMem->nEntries       () && 
       0  <  m_totMem->flagMean       () && 
       0  <  m_totMem->flagRMS        () && 
       0  <  memMB                       && 
       memMB > m_totMem->flagMean() + 3 * m_totMem->flagRMS () ) 
  {
    Warning ( "Total Memory for the event exceeds 3*sigma" , 
              StatusCode::SUCCESS , m_maxPrint     ).ignore() ;    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug () << " Total Memory : " << memMB 
               << " Mean : ("        << m_totMem->flagMean () 
               << "+-"               << m_totMem->flagRMS() << ")" << endmsg ;
  }
  // check the particular event
  if ( 0  <= m_prev                      && 
       0  != m_delMem                    && 
       16 <  m_delMem->nEntries       () && 
       0  <  m_delMem->flagRMS        () && 
       0  <  deltaMem                    && 
       deltaMem > m_delMem->flagMean() + 3 * m_delMem->flagRMS () ) 
  {
    Warning ( "Delta Memory for the event exceeds 3*sigma" , 
              StatusCode::SUCCESS , m_maxPrint     ).ignore() ;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug () << " Delta Memory : "  << deltaMem
               << " Mean : ("         << m_delMem->flagMean () 
               << "+-"                << m_delMem->flagRMS() << ")" << endmsg ;
  }
  /// check the tendency: 
  if ( ( ( 0 < m_check && 0 == m_counter % m_check ) || 1 == m_check  ) && 
       0  != m_delMem                && 
       16 < m_delMem->nEntries    () && 
       0  < m_delMem->flagMean    () && 
       0  < m_delMem->flagMeanErr () && 
       m_delMem->flagMean() > 3 * m_delMem->flagMeanErr() ) 
  {
    Warning ( "Mean 'Delta-Memory' exceeds 3*sigma" , 
              StatusCode::SUCCESS , m_maxPrint     ).ignore() ;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug () << " Memory Leak? "
               << "("  << m_delMem->flagMean() 
               << "+-" << m_delMem->flagMeanErr() << ")" << endmsg ;
  }
  
}
// ============================================================================
// The END 
// ============================================================================
