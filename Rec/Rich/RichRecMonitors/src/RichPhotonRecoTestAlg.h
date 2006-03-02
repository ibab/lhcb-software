
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoTestAlg.h
 *
 *  Header file for algorithm class : RichPhotonRecoTestAlg
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoTestAlg.h,v 1.1 2006-03-02 15:26:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/02/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHPIXELMONITOR_H
#define RICHRECMONITOR_RICHPIXELMONITOR_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Interfaces
#include "RichRecBase/IRichPhotonReconstruction.h"

//-----------------------------------------------------------------------------
/** @class RichPhotonRecoTestAlg RichPhotonRecoTestAlg.h
 *
 *  Debug algorithm for photon reconstruction.
 *
 *  Performs some "fixed" reconstructions in each radiator, to allow easy 
 *  comparisions between different software versions etc.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/02/2006
 */
//-----------------------------------------------------------------------------

class RichPhotonRecoTestAlg : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPhotonRecoTestAlg( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichPhotonRecoTestAlg( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Pointer to photon reconstruction tool
  const IRichPhotonReconstruction * m_photonReco; 

  /// Photon reconstruction tool runtime name
  std::string m_photonRecoName;

};

#endif // RICHRECMONITOR_RICHPIXELMONITOR_H
