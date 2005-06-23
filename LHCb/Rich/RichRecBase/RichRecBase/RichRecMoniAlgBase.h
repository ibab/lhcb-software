
//-----------------------------------------------------------------------------
/** @file RichRecMoniAlgBase.h
 *
 *  Header file for reconstruction monitor algorithm base class : RichRecMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniAlgBase.h,v 1.4 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECMONIALGBASE_H
#define RICHRECBASE_RICHRECMONIALGBASE_H 1

// base class
#include "RichKernel/RichMoniAlgBase.h"

// Event
class ProcStatus;
class RichRecStatus;
#include "Event/RichRecTrack.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"

// Interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichStatusCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichRecGeomTool.h"

//-----------------------------------------------------------------------------
/** @class RichRecMoniAlgBase RichRecMoniAlgBase.h RichRecBase/RichRecMoniAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality (identical to RichRecAlgBase) but with additional
 *  histogram and ntuple functionality provided by GaudiTupleAlg base class
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 *
 *  @todo Review if common functionality between RichRecMoniAlgBase 
 *        and RichRecAlgBase can be merged
 */
//-----------------------------------------------------------------------------

class RichRecMoniAlgBase : public RichMoniAlgBase {

public:

  /// Standard constructor
  RichRecMoniAlgBase( const std::string& name,
                      ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecMoniAlgBase() = 0;

  /** Initialization of the algorithm after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** The main event processing method. Called once for each event
   *
   * @return The status of the event processing
   * @retval StatusCode::SUCCESS Event processing was successful
   * @retval StatusCode::FAILURE Event processing failed
   */
  virtual StatusCode execute();

  /** Finalization of the algorithm before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

protected:  // Protected methods

  // include inline methods from common header file
#include "RichRecBase/RichRecMethodsImp.h"

private:   // Private data

  // include data members from common header file
#include "RichRecBase/RichRecPrivateData.h"

};

#endif // RICHRECBASE_RICHRECMONIALGBASE_H
