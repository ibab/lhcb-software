// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelDigiTRAC.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_DIGI_TRAC_H
#define BRUNEL_DIGI_TRAC_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   BrunelDigiTRAC  
//
// Description: Steering for Tracker digitisation in Brunel.
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelDigiTRAC : public Algorithm    {

public:

  // Constructor.
  BrunelDigiTRAC( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelDigiTRAC();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_DIGI_TRAC_H
