// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelRecoRICH.h,v 1.1.1.1 2001-03-14 16:06:47 cattaneb Exp $
#ifndef BRUNEL_RECO_RICH_H
#define BRUNEL_RECO_RICH_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   ReconstructRICH  
//
// Description: Steering for RICH Reconstruction in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelRecoRICH : public Algorithm    {

public:

  // Constructor.
  BrunelRecoRICH( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelRecoRICH();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_RECO_RICH_H
