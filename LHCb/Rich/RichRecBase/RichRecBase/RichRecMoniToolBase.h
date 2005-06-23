
//-----------------------------------------------------------------------------
/** @file RichRecMoniToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecMoniToolBase
 *
 * CVS Log :-
 * $Id: RichRecMoniToolBase.h,v 1.3 2005-06-23 15:13:05 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHRECMONITOOLBASE_H
#define RICHRECALGS_RICHRECMONITOOLBASE_H 1

// Base class
#include "RichKernel/RichMoniToolBase.h"

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
/** @class RichRecMoniToolBase RichRecMoniToolBase.h RichRecBase/RichRecMoniToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

class RichRecMoniToolBase : public RichMoniToolBase {

public:

  /// Standard constructor
  RichRecMoniToolBase( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichRecMoniToolBase() {};

  /** Initialization of the tool after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** Finalization of the tool before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

protected:   // Protected methods

  // include inline methods from common header file
#include "RichRecBase/RichRecMethodsImp.h"

private:   // Private data

  // include data members from common header file
#include "RichRecBase/RichRecPrivateData.h"

};

#endif // RICHRECALGS_RICHRECMONITOOLBASE_H
