// $Id: RichTabulatedHPDSignalDetectionEff.h,v 1.1 2003-08-26 14:40:21 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFF_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichDetTools/IRichDetInterface.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"
#include "RichKernel/RichDetectorType.h"
#include "RichKernel/BoostArray.h"

/** @class RichTabulatedHPDSignalDetectionEff RichTabulatedHPDSignalDetectionEff.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedHPDSignalDetectionEff : public RichRecToolBase,
                                           virtual public IRichSignalDetectionEff {

public:

  /// Standard constructor
  RichTabulatedHPDSignalDetectionEff( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedHPDSignalDetectionEff() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( RichRecSegment * segment, double energy );

private:  // Private data

  /// Tool pointers
  IRichDetInterface * m_richDetInt;

  /// Location of flat mirror tabulated properies in XML
  std::vector<std::string> m_flatMirReflLoc;

  /// Location of primiary mirror tabulated properies in XML
  std::vector<std::string> m_sphMirReflLoc;

  /// Pointers to reflectivities for each mirror
  typedef boost::array<Rich1DTabProperty*,Rich::NRiches> MirrorReflectivities;
  MirrorReflectivities m_flatMirRefl;
  MirrorReflectivities m_sphMirRefl;

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

#endif // RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFF_H
