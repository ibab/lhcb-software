// $Id: $
#ifndef RECALGS_RECPROCESSINGTIMEMONI_H 
#define RECALGS_RECPROCESSINGTIMEMONI_H 1

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h" 

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

  double m_maxTime; ///< Job Option for maximum overall processing time for plots
  double m_minTime; ///< Job Option for minimum overall processing time for plots

  double m_logMinTime; /// Cached log of min time
  double m_logMaxTime; /// cached log of max time

};

#endif // RECALGS_RECPROCESSINGTIMEMONI_H
