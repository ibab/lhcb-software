
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecToolBase
 *
 * CVS Log :-
 * $Id: RichRecToolBase.h,v 1.17 2005-06-23 15:13:05 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2002-07-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECTOOLBASE_H
#define RICHRECBASE_RICHRECTOOLBASE_H 1

// Base class
#include "RichKernel/RichToolBase.h"

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
/** @class RichRecToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

class RichRecToolBase : public RichToolBase {

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase() {};

  // Initialize method
  virtual StatusCode initialize();

  // Finalize method
  virtual StatusCode finalize();

protected:   // Protected methods

  // include inline methods from common header file
#include "RichRecBase/RichRecMethodsImp.h"

private:   // Private data

  // include data members from common header file
#include "RichRecBase/RichRecPrivateData.h"

};

#endif // RICHRECBASE_RICHRECTOOLBASE_H
