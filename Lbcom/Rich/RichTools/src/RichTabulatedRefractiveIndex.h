
/** @file RichTabulatedRefractiveIndex.h
 *
 *  Header file for tool : RichTabulatedRefractiveIndex
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRefractiveIndex.h,v 1.4 2004-10-27 14:41:03 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/07/26 18:03:05  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

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

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

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

  /// Pointers to refractive indices for each radiator type
  typedef boost::array<Rich1DTabProperty*,Rich::NRadiatorTypes> RefractiveIndices;
  RefractiveIndices m_refIndex;

};

#endif // RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
