
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.h
 *
 *  Header file for tool : Rich::TabulatedRefractiveIndex
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
#define RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H 1

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// Base class and interfaces
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichDetParameters.h"

// RichKernel
#include "RichKernel/IRichDetParameters.h"
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/Rich1DTabProperty.h"

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
    virtual ~TabulatedRefractiveIndex() {}

    // Initialization of the tool after creation
    StatusCode initialize();

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

    // Calculates the average refractive index for a given set of radiator intersections
    // for all visable photon energies.
    double refractiveIndex ( const Rich::RadIntersection::Vector & intersections,
                             const double energy ) const;

    // Calculates the average refractive index for a given set of radiator intersections
    // for all visable photon energies
    double refractiveIndex ( const Rich::RadIntersection::Vector & intersections ) const;

    // Calculates the refractive index R.M.S. for a given set of radiator intersections
    // for all visable photon energies.
    double refractiveIndexRMS ( const Rich::RadIntersection::Vector & intersections ) const;

    // Calculates the refractive index S.D. for a given set of radiator intersections
    // for all visable photon energies.
    double refractiveIndexSD ( const Rich::RadIntersection::Vector & intersections ) const;

  private:

    /// Access on demand the Rich radiator detector elements
    inline const DeRichRadiator * deRad( const Rich::RadiatorType rad ) const
    {
      if ( !m_radiators[rad] ) 
      {
        m_radiators[rad] = getDet<DeRichRadiator>( DeRichLocations::location(rad) );
      }
      return m_radiators[rad];
    }

  private: // Private data

    /// Pointers to RICHes
    std::vector<const DeRich*> m_riches;

    /// Detector parameters tool
    const IDetParameters * m_detParams = nullptr;

    /// Pointers to RICH radiator detector elements
    mutable std::vector<const DeRichRadiator*> m_radiators;

    /** JO to force HLT mode. Used to make sure Offline uses the same
     *  refractive index calibration as Online */
    bool m_forceHltMode;

    /// Flag to say if we are in HLT mode or not
    bool m_hltMode = false;

  };

}

#endif // RICHTOOLS_RICHTABULATEDREFRACTIVEINDEX_H
