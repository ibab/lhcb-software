// $Id: RichPhotonSignal.h,v 1.4 2003-08-12 13:35:44 jonrob Exp $
#ifndef RICHRECTOOLS_RICHPHOTONSIGNAL_H
#define RICHRECTOOLS_RICHPHOTONSIGNAL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPhoton.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// Interfaces
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichPhotonSignal RichPhotonSignal.h
 *
 *  Tool to calculate for a given photon the probabilities of it 
 *  being a signal or scattered photon, and its predicted contribution
 *  to its associated RichRecPixel, under a certain mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPhotonSignal : public RichRecToolBase,
                         virtual public IRichPhotonSignal {

public:

  /// Standard constructor
  RichPhotonSignal( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonSignal(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
  double predictedPixelSignal( RichRecPhoton * photon,
                               const Rich::ParticleIDType id );
  /// Signal Probability for a given reconstructed photon under a given mass hypothesis
  double signalProb( RichRecPhoton * photon,
                     const Rich::ParticleIDType id );

  /// Scatter Probability for a given reconstructed photon under a given mass hypothesis
  double scatterProb( RichRecPhoton * photon,
                      const Rich::ParticleIDType id );

private: // private data

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Pointer to RichCherenkovAngle interface
  IRichCherenkovAngle * m_ckAngle;

  /// Pointer to RichCherenkovResolution interface
  IRichCherenkovResolution * m_ckRes;

  /// Temporary local value for Radii of curvature
  double m_radiusCurv[Rich::NRiches];

  /// Temporary local value pixel area
  double m_pixelArea;

};

#endif // RICHRECTOOLS_RICHPHOTONSIGNAL_H
