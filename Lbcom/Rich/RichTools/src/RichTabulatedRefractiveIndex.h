// $Id: RichTabulatedRefractiveIndex.h,v 1.2 2004-07-15 15:44:40 jonrob Exp $
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

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTabulatedRefractiveIndex( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedRefractiveIndex() {};

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Calculates the refractive index for a given radiator type at a
  // given energy
  double refractiveIndex ( const Rich::RadiatorType rad,
                           const double energy );

  // Calculates the average refractive index for a given radiator type
  // for a given range of photon energies.
  double refractiveIndex ( const Rich::RadiatorType rad,
                           const double energyBot,     
                           const double energyTop );

  // Calculates the average refractive index for a given radiator type
  // for a all visable photon energies.
  double refractiveIndex ( const Rich::RadiatorType rad );

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
