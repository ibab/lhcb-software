// $Id: RichRefractiveIndex.h,v 1.1 2003-08-06 11:08:13 jonrob Exp $
#ifndef RICHRECTOOLS_RICHREFRACTIVEINDEX_H
#define RICHRECTOOLS_RICHREFRACTIVEINDEX_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichUtils
#include "RichUtils/IRich1DProperty.h"
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRefractiveIndex.h"
#include "RichDetTools/IRichDetInterface.h"

/** @class RichRefractiveIndex RichRefractiveIndex.h
 *
 *  Tool to calculate the effective refractive index for
 *  a given RichRecSegment
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRefractiveIndex : public RichRecToolBase,
                            virtual public IRichRefractiveIndex {

public:

  /// Standard constructor
  RichRefractiveIndex( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichRefractiveIndex() {};

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

#endif // RICHRECTOOLS_RICHREFRACTIVEINDEX_H
