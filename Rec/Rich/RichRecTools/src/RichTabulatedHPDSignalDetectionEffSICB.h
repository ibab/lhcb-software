// $Id: RichTabulatedHPDSignalDetectionEffSICB.h,v 1.1 2003-10-13 16:32:35 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFFSICB_H
#define RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFFSICB_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"

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

/** @class RichTabulatedHPDSignalDetectionEffSICB RichTabulatedHPDSignalDetectionEffSICB.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedHPDSignalDetectionEffSICB : public RichRecToolBase,
                                               virtual public IRichSignalDetectionEff {

public:

  /// Standard constructor
  RichTabulatedHPDSignalDetectionEffSICB( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedHPDSignalDetectionEffSICB() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( RichRecSegment * segment, double energy );

private:  // Private data

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

  /// Quantum Efficiency function.
  /// For time being assume only one reference curve for all HPDs
  Rich1DTabProperty * m_QE;

  /// Location of QE in XML
  std::string m_qeTableLoc;

  /// temporary parameters to take into acount degraded performance for robustness tests
  double m_photonEffScale;

  double m_fudge;

};

#endif // RICHRECTOOLS_RICHTABULATEDHPDSIGNALDETECTIONEFFSICB_H
