
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.h
 *
 *  Header file for tool : Rich::TabulatedRefractiveIndex
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRefractiveIndex.h,v 1.15 2007-02-01 17:51:11 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

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
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class TabulatedRefractiveIndex RichTabulatedRefractiveIndex.h
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

  class TabulatedRefractiveIndex : public Rich::ToolBase,
                                   virtual public IRefractiveIndex
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    TabulatedRefractiveIndex( const std::string& type,
                              const std::string& name,
                              const IInterface* parent );

    /// Destructor
    virtual ~TabulatedRefractiveIndex() {};

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
    const IDetParameters * m_detParams;

    /// Pointers to RICH radiator detector elements
    std::vector<DeRichRadiator *> m_deRads;

    /// refractive index RMS values
    std::vector<double> m_refI;

    /// refractive index RMS values
    std::vector<double> m_refRMS;

  };

}

#endif // RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
