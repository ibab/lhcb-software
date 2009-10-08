// $Id: MemoryTool.h,v 1.3 2009-10-08 11:18:26 ibelyaev Exp $
// ============================================================================
#ifndef MEMORYTOOL_H 
#define MEMORYTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
//  GaudiKernel
// ============================================================================
#include "GaudiKernel/HistoDef.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IGenericTool.h"            // Interface
// ============================================================================
/** @class MemoryTool MemoryTool.h
 *  Tool to plot memory usage of the application at each call
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-14
 */
class MemoryTool : public GaudiHistoTool, virtual public IGenericTool {
public: 
  /// Standard constructor
  MemoryTool( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~MemoryTool( ); ///< Destructor

  void execute(); ///< Plot the current memory usage
  
  StatusCode finalize () 
  {
    m_totMem = 0 ;
    m_delMem = 0 ;
    m_plot1  = 0 ;
    m_plot2  = 0 ;
    return GaudiHistoTool::finalize (); 
  }

protected:

private:
  // ==========================================================================
  unsigned long m_counter; ///< Counter of calls to the tool
  unsigned int  m_bins;    ///< Number of bins of histogram (Property HistoSize)  
  // ==========================================================================
  /// flag to skip/reset events for memory measurements  
  int         m_skip   ;   // flag to skip/reset events for memory measurements
  /// the previosu measurement of virtual memory 
  double      m_prev   ;          // the previosu measurement of virtual memory
  // ==========================================================================
  /// the histogram definition (as property) 
  Gaudi::Histo1DDef m_histo1 ;         // the histogram definition (as property) 
  /// the histogram definition (as property) 
  Gaudi::Histo1DDef m_histo2 ;         // the histogram definition (as property) 
  // ==========================================================================
private:
  // ==========================================================================
  /// the counetr for total memory 
  StatEntity* m_totMem ;                       // the counter for total memory 
  /// the counetr for delta memory 
  StatEntity* m_delMem ;                       // the counter for delta memory
  /// the histogram of total memory 
  AIDA::IHistogram1D* m_plot1 ;                // the histogram of total memory 
  /// the histogram of delta memory 
  AIDA::IHistogram1D* m_plot2 ;                // the histogram of delta memory 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // MEMORYTOOL_H
// ============================================================================
