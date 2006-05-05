
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.h
 *
 *  Header file for tool : RichTabulatedRefractiveIndex
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRefractiveIndex.h,v 1.13 2006-05-05 11:11:00 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// Constants
#include "Kernel/PhysicalConstants.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichRefractiveIndex.h"

// RichKernel
#include "RichKernel/IRichDetParameters.h"
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/Rich1DTabProperty.h"

//-----------------------------------------------------------------------------
/** @class RichTabulatedRefractiveIndex RichTabulatedRefractiveIndex.h
 *
 *  Tool to calculate the effective refractive index for
 *  a given radiator. An implementation that uses the tabulated
 *  information from the XML
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Fix RMS calculations
 *  @todo Update UMS dependencies to be more 'fine grained'
 *  @todo Figure out how to properly deal with multiple aerogel volumes
 */
//-----------------------------------------------------------------------------

class RichTabulatedRefractiveIndex : public RichToolBase,
                                     virtual public IRichRefractiveIndex
{

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
                           const double energy ) const;

  // Calculates the average refractive index for a given radiator type
  // for a given range of photon energies.
  double refractiveIndex ( const Rich::RadiatorType rad,
                           const double energyBot,
                           const double energyTop ) const;

  // Calculates the average refractive index for a given radiator type
  // for a all visable photon energies.
  double refractiveIndex ( const Rich::RadiatorType rad ) const;

  // Calculates the refractive index R.M.S. for a given radiator type
  // for all visable photon energies.
  double refractiveIndexRMS ( const Rich::RadiatorType rad ) const;

private: // methods

  /// UMS update method for Aerogel refractive index
  StatusCode updateAerogelRefIndex();

  /// UMS update method for Rich1Gas refractive index
  StatusCode updateRich1GasRefIndex();

  /// UMS update method for Rich2Gas refractive index
  StatusCode updateRich2GasRefIndex();

  /// Update refractive index for given radiator
  StatusCode updateRefIndex( const Rich::RadiatorType rad );

private: // Private data

  /// Quantum Efficiency function.
  //Rich1DTabProperty * m_QE;

  /// Pointers to RICHes
  std::vector<const DeRich*> m_riches;

  /// Detector parameters tool
  const IRichDetParameters * m_detParams;

  /// Pointers to RICH radiator detector elements
  std::vector<DeRichRadiator *> m_deRads;

  /// refractive index RMS values
  std::vector<double> m_refI;

  /// refractive index RMS values
  std::vector<double> m_refRMS;

};

#endif // RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
