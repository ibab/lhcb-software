
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.h
 *
 *  Header file for reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.h,v 1.20 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECALGBASE_H
#define RICHRECBASE_RICHRECALGBASE_H 1

// base class
#include "RichKernel/RichAlgBase.h"

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
/** @class RichRecAlgBase RichRecAlgBase.h RichRecBase/RichRecAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecAlgBase : public RichAlgBase {

public:

  /// Standard constructor
  RichRecAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecAlgBase() = 0;

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

#endif // RICHRECBASE_RICHRECALGBASE_H
