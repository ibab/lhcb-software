// $Id: RichSignalDetectionEffSICB.h,v 1.2 2003-10-13 16:32:35 jonrob Exp $
#ifndef RICHRECTOOLS_RICHSIGNALDETECTIONEFFSICB_H
#define RICHRECTOOLS_RICHSIGNALDETECTIONEFFSICB_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

/** @class RichSignalDetectionEffSICB RichSignalDetectionEffSICB.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichSignalDetectionEffSICB : public RichRecToolBase,
                                   virtual public IRichSignalDetectionEff {

public:

  /// Standard constructor
  RichSignalDetectionEffSICB( const std::string& type,
                              const std::string& name,
                              const IInterface* parent );

  /// Destructor
  virtual ~RichSignalDetectionEffSICB() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( RichRecSegment * segment, double energy );

private:  // Private data

  /// Location of QE in XML
  std::string m_qeTableLoc;     

  // Temporary variables
  double m_detReflectorEff;

  /// quartz window losses efficiency
  double m_quartzWinEff;

  /// Digitisation pedestal loss
  double m_pedLoss;

  /// Quantum Efficiency function. For time being assume only one reference
  /// curve for all HPDs
  Rich1DTabProperty * m_QE;

  /// temporary parameters to take into acount degraded performance for robustness tests
  double m_photonEffScale;

};

#endif // RICHRECTOOLS_RICHSIGNALDETECTIONEFFSICB_H
