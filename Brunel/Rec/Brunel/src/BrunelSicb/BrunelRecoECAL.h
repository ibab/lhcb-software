// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelRecoECAL.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_RECO_ECAL_H
#define BRUNEL_RECO_ECAL_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   ReconstructECAL  
//
// Description: Steering for ECAL Reconstruction in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelRecoECAL : public Algorithm    {

public:

  // Constructor.
  BrunelRecoECAL( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelRecoECAL();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_RECO_ECAL_H
