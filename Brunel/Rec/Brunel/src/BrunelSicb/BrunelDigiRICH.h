// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelDigiRICH.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_DIGI_RICH_H
#define BRUNEL_DIGI_RICH_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   DigitiseRICH  
//
// Description: Steering for RICH digitisation in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelDigiRICH : public Algorithm    {

public:

  // Constructor.
  BrunelDigiRICH( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelDigiRICH();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_DIGI_RICH_H
