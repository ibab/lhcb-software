// $Id: RichSignalDetectionEff.h,v 1.2 2003-08-12 13:35:44 jonrob Exp $
#ifndef RICHRECTOOLS_RICHSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_RICHSIGNALDETECTIONEFF_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichDetTools/IRichDetInterface.h"

// RichUtils
//#include "RichUtils/IRich1DProperty.h"
#include "RichUtils/Rich1DTabProperty.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

/** @class RichSignalDetectionEff RichSignalDetectionEff.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichSignalDetectionEff : public RichRecToolBase,
                               virtual public IRichSignalDetectionEff {

public:

  /// Standard constructor
  RichSignalDetectionEff( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

  /// Destructor
  virtual ~RichSignalDetectionEff() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( double energy );

private:  // Private data

  /// Pointers to tool instances
  IRichDetInterface * m_richDetInt;

  // Temporary variables
  double m_detReflectorEff;

  /// quartz window losses efficiency
  double m_quartzWinEff;

  /// Digitisation pedestal loss
  double m_pedLoss;

  /// Quantum Efficiency function. For time being assume only one reference
  /// curve for all HPDs
  Rich1DTabProperty * m_referenceQE;

  /// temporary parameters to take into acount degraded performance for robustness tests
  double m_photonEffScale;

};

#endif // RICHRECTOOLS_RICHSIGNALDETECTIONEFF_H
