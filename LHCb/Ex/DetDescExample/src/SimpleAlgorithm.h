// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/SimpleAlgorithm.h,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#ifndef DDEXAMPLE_SIMPLEALGORITHM_H
#define DDEXAMPLE_SIMPLEALGORITHM_H 1


// Include files
#include <vector>
#include "GaudiKernel/Algorithm.h"


// Forward declarations
class DeMuonStation;


//------------------------------------------------------------------------------
//
// ClassName:   SimpleAlgorithm
//  
// Description: Example of an algorithm using the detector data service and 
//              the declaration of a especific detertor element converter
//
//------------------------------------------------------------------------------


class SimpleAlgorithm : public Algorithm {
public:
  // Constructor: A constructor of this form must be provided.
  SimpleAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  // Flag if produce histograms
  bool          m_produceHistogram;

  // These data members are used in the execution of the
  // algorithm. There values are set in the initialisation
  // phase by the job options service.
  std::vector<DeMuonStation*> m_stations;
};

#endif    // DDEXAMPLE_SIMPLEALGORITHM_H