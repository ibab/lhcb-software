// $Id: RichTabulatedRefractiveIndex.h,v 1.1.1.1 2004-06-17 12:04:09 cattanem Exp $
#ifndef RICHDETTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHDETTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichRefractiveIndex.h"

// RichKernel
#include "RichKernel/IRichDetParameters.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/BoostArray.h"

/** @class RichTabulatedRefractiveIndex RichTabulatedRefractiveIndex.h
 *
 *  Tool to calculate the effective refractive index for
 *  a given radiator. An implementation that uses the tabulated
 *  information from the XML
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedRefractiveIndex : public RichToolBase,
                                     virtual public IRichRefractiveIndex {

public:

  /// Standard constructor
  RichTabulatedRefractiveIndex( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedRefractiveIndex() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Refractive index for a given energy
  double refractiveIndex( const Rich::RadiatorType rad,
                          const double energy );

  /// Average refractive index for a given radiator between two photon energies
  double refractiveIndex( const Rich::RadiatorType rad,
                          const double energyBot,
                          const double energyTop );

  /// Average refractive index for given radiator
  double refractiveIndex( const Rich::RadiatorType rad );

private:  // Private data

  /// Quantum Efficiency function.
  Rich1DTabProperty * m_QE;

  /// Detector paramters tool
  IRichDetParameters * m_detParams;

  /// Location of QE in XML
  std::string m_qeTableLoc;

  /// Pointers to refractive indices for each radiator type
  typedef boost::array<Rich1DTabProperty*,Rich::NRadiatorTypes> RefractiveIndices;
  RefractiveIndices m_refIndex;

  /// vector of locations in XML for refractive indices
  std::vector<std::string> m_refLocations;

};

#endif // RICHDETTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
