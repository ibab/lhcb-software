// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelRecoTRAC.h,v 1.1.1.1 2001-03-14 16:06:47 cattaneb Exp $
#ifndef BRUNEL_RECO_TRAC_H
#define BRUNEL_RECO_TRAC_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   ReconstructTRAC  
//
// Description: Steering for TRAC Reconstruction in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelRecoTRAC : public Algorithm    {

public:

  // Constructor.
  BrunelRecoTRAC( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelRecoTRAC();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_RECO_TRAC_H
