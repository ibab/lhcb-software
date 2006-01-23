
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : RichTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.h,v 1.9 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"

// Kernel
#include "Kernel/RichDetectorType.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/RichRecSegment.h"

// constants
#include "Kernel/PhysicalConstants.h"

// RichDet
#include "RichDet/DeRich1.h"

//-----------------------------------------------------------------------------
/** @class RichTabulatedSignalDetectionEff RichTabulatedSignalDetectionEff.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Add support for different QE curves for each HPD
 */
//-----------------------------------------------------------------------------

class RichTabulatedSignalDetectionEff : public RichRecToolBase,
                                        virtual public IRichSignalDetectionEff
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTabulatedSignalDetectionEff( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedSignalDetectionEff() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( LHCb::RichRecSegment * segment,
                              const double energy ) const;

private:  // Private data

  /// Tabulated properties for each rich detector
  typedef boost::array<Rich1DTabProperty*,Rich::NRiches> TabProps;

  /// Flat mirror reflectivities
  TabProps m_flatMirRefl;

  /// Spherical mirror reflectivities
  TabProps m_sphMirRefl;

  /// quartz window losses efficiency
  double m_quartzWinEff;

  /// Digitisation pedestal loss
  double m_pedLoss;

  /// Quantum Efficiency function.
  /// For time being assume only one reference curve for all HPDs
  Rich1DTabProperty * m_QE;

};

#endif // RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
