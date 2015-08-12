// $Id: $
#ifndef RECALGS_RECPROCESSINGTIMEMONI_H 
#define RECALGS_RECPROCESSINGTIMEMONI_H 1

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from Aida
#include "AIDA/IHistogram1D.h"

/** @class RecProcessingTimeMoni RecProcessingTimeMoni.h
 *  
 *  Simple monitor making basic processing time plots for the Reconstruction
 *  
 *  @author Chris Jones
 *  @date   2010-07-15
 */

class RecProcessingTimeMoni : public GaudiHistoAlg
{

public: 

  /// Standard constructor
  RecProcessingTimeMoni( const std::string& name, 
                         ISvcLocator* pSvcLocator );

  virtual ~RecProcessingTimeMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Definition of algorithm name list
  typedef std::vector<std::string> AlgorithmNames;

  AlgorithmNames m_algNames; ///< List of algorithm(s) to include in timing

  AIDA::IHistogram1D* m_hist; ///< Pointer to processing time histogram

  double m_logMaxTime; ///< Job Option for log10(maximum overall processing time) for plots
  double m_logMinTime; ///< Job Option for log10(minimum overall processing time) for plots

};

#endif // RECALGS_RECPROCESSINGTIMEMONI_H
