
//-----------------------------------------------------------------------------
/** @file RichRecDataObjVerifier.h
 *
 *  Header file for algorithm class : RichRecDataObjVerifier
 *
 *  CVS Log :-
 *  $Id: RichRecDataObjVerifier.h,v 1.2 2006-12-01 16:34:07 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHRECDATAOBJVERIFIER_H
#define RICHRECALGS_RICHRECDATAOBJVERIFIER_H 1

// from Gaudi
#include "GaudiKernel/IProperty.h"

// Rich Rec base class
#include "RichRecBase/RichRecAlgBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecDataObjVerifier RichRecDataObjVerifier.h
 *
 *  Low level debugging algorithm to verify the contents of the RichRecEvent
 *  data objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecDataObjVerifier : public RichRecAlgBase {

public:

  /// Standard constructor
  RichRecDataObjVerifier( const std::string& name,
                          ISvcLocator* pSvcLocator );

  virtual ~RichRecDataObjVerifier(); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:

  /// Flag to debug RichRecPixels
  bool m_bdPixels;

  /// Flag to debug RichRecTracks
  bool m_bdTracks;

  /// Flag to debug RichRecPhotons
  bool m_bdPhotons;

  /// Flag to debug RichRecSegments
  bool m_bdSegments;

};

#endif // RICHRECALGS_RICHRECDATAOBJVERIFIER_H
