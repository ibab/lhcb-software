
//---------------------------------------------------------------------------
/** @file RichRecBackgroundEsti.h
 *
 *  Header file for algorithm class : RichRecBackgroundEsti
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEsti.h,v 1.5 2006-12-19 09:46:30 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"

//---------------------------------------------------------------------------
/** @class RichRecBackgroundEsti RichRecBackgroundEsti.h
 *
 *  Background estimation algorithm for RICH HPDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

class RichRecBackgroundEsti : public RichRecAlgBase
{

public:

  /// Standard constructor
  RichRecBackgroundEsti( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRecBackgroundEsti();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:   // Private data members

  /// Pointers to expected track signal tool
  const IRichExpectedTrackSignal * m_tkSignal;

  /// Maximum number of iterations in background normalisation
  int m_maxBkgIterations;

  /// Number of pixels per HPD without cathode acceptance
  double m_nPixelsPerPD;

};

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
