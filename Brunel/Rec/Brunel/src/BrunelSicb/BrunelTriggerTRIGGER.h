// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelTriggerTRIGGER.h,v 1.1.1.1 2001-03-14 16:06:47 cattaneb Exp $
#ifndef BRUNEL_TRIGGER_TRIGGER_H
#define BRUNEL_TRIGGER_TRIGGER_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   TriggerTRIGGER 
//
// Description: Steering for trigger Reconstruction in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelTriggerTRIGGER : public Algorithm    {

public:

  // Constructor.
  BrunelTriggerTRIGGER( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelTriggerTRIGGER();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_TRIGGER_TRIGGER_H
