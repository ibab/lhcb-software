// $Id: AnalysisAlgorithm.h,v 1.1.1.1 2001-10-19 17:06:04 gcorti Exp $
#ifndef TOOLSTUTORIAL_ANALYSISALGORITHM_H
#define TOOLSTUTORIAL_ANALYSISALGORITHM_H

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance

// Forward declarations

/** User example objects: AnalysiAlgorithm

    Description:
    The algorithm class for the Gaudi tutorial on tools.

    @author:  G.Corti
    @version: 1.0
*/
class AnalysisAlgorithm : public Algorithm {

public:
  /// Constructor: A constructor of this form must be provided.
  AnalysisAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~AnalysisAlgorithm();
  /// Override Base class implementation Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();

private:
  
  std::string               m_partName; ///< Name of particle to be analysed  
  long                      m_partID;   ///< GeantID of particle to be analysed

  std::vector<std::string>  m_daugName; ///< Name of daughters to be found
  std::vector<long>         m_daugID;   ///< GeantID of daughters to be found

};

#endif // TOOLSTUTORIAL_ANALYSISALGORITHM_H
