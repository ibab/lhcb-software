// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFinalFitTRAC.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_FINALFIT_TRAC_H
#define BRUNEL_FINALFIT_TRAC_H


// Include Files
#include "GaudiKernel/Algorithm.h"


//------------------------------------------------------------------------------
// 
// ClassName:   BrunelFinalFitTRAC  
//
// Description: Steering for Final TRAC Fit in Brunel
//
// Author:      Marco Cattaneo
//
//------------------------------------------------------------------------------


class BrunelFinalFitTRAC : public Algorithm    {

public:

  // Constructor.
  BrunelFinalFitTRAC( const std::string& name, ISvcLocator *svcloc );

  // Destructor.
  virtual ~BrunelFinalFitTRAC();

  // Initialise the algorithm
  virtual StatusCode initialize();

  // Execute the algorithm
  virtual StatusCode execute();

  // Finalise the algorithm
  virtual StatusCode finalize();

  // Create an instance of this class.
  IAlgorithm* create( const std::string& name, ISvcLocator* svcloc );

};

#endif // BRUNEL_FINALFIT_TRAC_H
