// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelRecoHCAL.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_RECO_HCAL_H
#define BRUNEL_RECO_HCAL_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   ReconstructHCAL  
//
// Description: Steering for HCAL Reconstruction in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelRecoHCAL : public Algorithm    {

public:

  // Constructor.
  BrunelRecoHCAL( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelRecoHCAL();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_RECO_HCAL_H
