// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/SimpleAlgorithm.h,v 1.2 2002-04-29 17:05:54 sponce Exp $
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
  /**
   * Constructor: A constructor of this form must be provided.
   */
  SimpleAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 

  /**
   * The "initialization" of the algorithm.
   * It creates the environment needed for processing the events.
   * In this example, everything is done here since no events are
   * processed.
   */
  StatusCode initialize();

  /**
   * This is were the code for the event processing should reside.
   * In this example, we do nothing here since we don't process any event.
   */
  StatusCode execute();

  /**
   * We use this place to display the status of the transient data store.
   */
  StatusCode finalize();

};

#endif    // DDEXAMPLE_SIMPLEALGORITHM_H
