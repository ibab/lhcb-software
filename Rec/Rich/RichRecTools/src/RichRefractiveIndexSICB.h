// $Id: RichRefractiveIndexSICB.h,v 1.1 2003-08-26 14:40:20 jonrob Exp $
#ifndef RICHRECTOOLS_RICHREFRACTIVEINDEXSICB_H
#define RICHRECTOOLS_RICHREFRACTIVEINDEXSICB_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRefractiveIndex.h"
#include "RichDetTools/IRichDetInterface.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

/** @class RichRefractiveIndexSICB RichRefractiveIndexSICB.h
 *
 *  Tool to calculate the effective refractive index for
 *  a given RichRecSegment.
 *  Temporary inplementation using hardcoded parameters from SICB reco
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRefractiveIndexSICB : public RichRecToolBase,
                                virtual public IRichRefractiveIndex {

public:

  /// Standard constructor
  RichRefractiveIndexSICB( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  /// Destructor
  virtual ~RichRefractiveIndexSICB() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Average refractive index for this segments radiator
  double refractiveIndex( RichRecSegment * segment );

  /// Refractive index for a given energy
  double refractiveIndex( const Rich::RadiatorType radiator,
                          double energy );

  /// Average refractive index for given radiator
  double refractiveIndex( const Rich::RadiatorType radiator );

private:  // Private data

  /// Pointers to tool instances
  IRichDetInterface * m_richDetInt;

  // Sellmeir parameters
  double m_selF1[Rich::NRadiatorTypes];
  double m_selF2[Rich::NRadiatorTypes];
  double m_selE1[Rich::NRadiatorTypes];
  double m_selE2[Rich::NRadiatorTypes];
  double m_molW[Rich::NRadiatorTypes];
  double m_rho[Rich::NRadiatorTypes];

  /// Quantum Efficiency function.
  /// For time being assume only one reference curve for all HPDs
  Rich1DTabProperty * m_referenceQE;

};

#endif // RICHRECTOOLS_RICHREFRACTIVEINDEXSICB_H
