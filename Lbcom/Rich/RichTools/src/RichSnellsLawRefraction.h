
//-----------------------------------------------------------------------------
/** @file RichSnellsLawRefraction.h
 *
 *  Header file for tool : Rich::SnellsLawRefraction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RichSnellsLawRefraction_H
#define RICHTOOLS_RichSnellsLawRefraction_H 1

// Base class
#include "RichKernel/RichToolBase.h"

// interfaces
#include "RichKernel/IRichSnellsLawRefraction.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichRadiatorTool.h"

// Math
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// RichDet
#include "RichDet/DeRichRadiator.h"

// Kernel
#include "RichKernel/RichTrackSegment.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class SnellsLawRefraction RichSnellsLawRefraction.h
   *
   *  A tool which implements snells law refraction at Aerogel / Rich1Gas
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   03/01/2008
   */
  //-----------------------------------------------------------------------------

  class SnellsLawRefraction : public Rich::ToolBase,
                              virtual public Rich::ISnellsLawRefraction
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    SnellsLawRefraction( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

    virtual ~SnellsLawRefraction( ); ///< Destructor

    // Initialization of the tool after creation
    virtual StatusCode initialize();

    // Finalization of the tool
    virtual StatusCode finalize();

  public: // methods (and doxygen comments) inherited from interface

    // Correct the direction vector and start point for refraction in going from Aerogel to Rich1Gas
    void aerogelToGas( Gaudi::XYZPoint & startPoint,
                       Gaudi::XYZVector &    dir,
                       const LHCb::RichTrackSegment& trSeg ) const;

    // Correct the direction vector from refraction in going from Aerogel to Rich1Gas
    void aerogelToGas( Gaudi::XYZPoint& startPoint,
                       Gaudi::XYZVector &     dir,
                       const double photonEnergy ) const;

    // Correct the direction vector for refraction in going from Rich1Gas to aerogel
    void gasToAerogel( Gaudi::XYZVector &     dir,
                       const LHCb::RichTrackSegment& trSeg ) const;

    // Correct the direction vector from refraction in going from Rich1Gas to aerogel
    void gasToAerogel( Gaudi::XYZVector &     dir,
                       const double photonEnergy ) const;

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

    /// Do the aerogel to gas correction
    void _aerogelToGas( Gaudi::XYZPoint & startPoint,
                        Gaudi::XYZVector & dir,
                        const double photonEnergy,
                        const double refAero ) const;

    /// Do the gas to aerogel correction
    void _gasToAerogel( Gaudi::XYZVector & dir,
                        const double photonEnergy,
                        const double refAero ) const;

    /// Create information for the aerogel exit plane
    void buildAeroPlaneInfo() const;

    /// Access on demand the aerogel plane normal
    inline const Gaudi::XYZVector& aeroNormVect() const
    {
      if ( !m_planeInfoMade ) buildAeroPlaneInfo();
      return m_aeroNormVect;
    }

    /// Access on demand the aerogel exit plane
    inline const Gaudi::Plane3D& aeroExitPlane() const
    {
      if ( !m_planeInfoMade ) buildAeroPlaneInfo();
      return m_aeroExitPlane;
    }

    /// UMS update for aerogel
    StatusCode aeroUpdate();

  private:

    /// Pointer to general refractive index tool
    const IRefractiveIndex * m_refIndex = nullptr;

    /// aerogel exit plane (for refraction correction)
    mutable Gaudi::Plane3D m_aeroExitPlane;

    /// Vector normal to aerogel exit plane (for refraction correction)
    mutable Gaudi::XYZVector m_aeroNormVect;

    /// Flag to say if the aerogel plane information has been constructed or not
    mutable bool m_planeInfoMade = false;

    /// z point for plane
    mutable double m_minZaero = 0;

    /// Vector of radiators
    mutable std::vector<const DeRichRadiator*> m_radiators;

    /** JO to force HLT mode. Used to make sure Offline uses the same
     *  refractive index calibration as Online */
    bool m_forceHltMode;
    
    /// Flag to say if we are in HLT mode or not
    bool m_hltMode = true;

    /// Flag to say in Aerogel is available or not
    bool m_aerogelOK = false;

  };

}

#endif // RICHTOOLS_RichSnellsLawRefraction_H
