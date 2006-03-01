
/** @file RichGeomPhoton.h
 *
 *  Header file for RICH utility class : RichGeomPhoton
 *
 *  $Id: RichGeomPhoton.h,v 1.2 2006-03-01 09:57:25 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-05-30
 */

#ifndef RICHEVENT_RICHGEOMPHOTON_H
#define RICHEVENT_RICHGEOMPHOTON_H 1

// std include
#include <iostream>

// geometry
#include "Kernel/Point3DTypes.h"

// from Kernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

/// General LHCb namespace
namespace LHCb
{

  /** @namespace RichGeomPhotonCode
   *
   *  Namespace for definitions related to RichGeomPhoton
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace RichGeomPhotonCode
  {

    /// Name of this class for messaging
    static const std::string myName = "RichGeomPhoton";

    /// Data type for internal storage
    typedef unsigned int ShortType;

    static const ShortType BitsFlatMirr    = 6; ///< number of bits for flat mirror
    static const ShortType BitsSphMirr     = 6; ///< number of bits for spherical mirror
    static const ShortType BitsFlatMirrOK  = 1; ///< number of bits for flat mirror OK
    static const ShortType BitsSphMirrOK   = 1; ///< number of bits for spherical mirror OK
    static const ShortType BitsMirrValid   = 1; ///< number of bits for mirror information is valid

    // Shifts
    static const ShortType ShiftFlatMirr   = 0;
    static const ShortType ShiftSphMirr    = ShiftFlatMirr    + BitsFlatMirr;
    static const ShortType ShiftFlatMirrOK = ShiftSphMirr     + BitsSphMirr;
    static const ShortType ShiftSphMirrOK  = ShiftFlatMirrOK  + BitsFlatMirrOK;
    static const ShortType ShiftMirrValid  = ShiftSphMirrOK   + BitsSphMirrOK;

    // Masks
    static const ShortType MaskFlatMirr    = ( ( 1 << BitsFlatMirr ) - 1 )   << ShiftFlatMirr;
    static const ShortType MaskSphMirr     = ( ( 1 << BitsSphMirr ) - 1 )    << ShiftSphMirr;
    static const ShortType MaskFlatMirrOK  = ( ( 1 << BitsFlatMirrOK ) - 1 ) << ShiftFlatMirrOK;
    static const ShortType MaskSphMirrOK   = ( ( 1 << BitsSphMirrOK ) - 1 )  << ShiftSphMirrOK;
    static const ShortType MaskMirrValid   = ( ( 1 << BitsMirrValid ) - 1 )  << ShiftMirrValid;

    // Create the maximum allowed value for each data field
    static const ShortType MaxFlatMirr     = ( 1 << BitsFlatMirr ) - 1; ///< Max value possible for flat mirror field
    static const ShortType MaxSphMirr      = ( 1 << BitsSphMirr )  - 1; ///< Max value possible for spherical mirror field

  }

  /** @class RichGeomPhoton RichGeomPhoton.h RichKernel/RichGeomPhoton.h
   *
   *  A geometrical representation of a reconstructed Cherenkov photon.
   *
   *  @author Antonis Papanestis
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2002-05-30
   */

  class RichGeomPhoton : public Rich::BoostMemPoolAlloc<RichGeomPhoton>
{

  public:

    /** Default constructor with optional parameters
     *
     *  @param theta Cherenkov angle theta
     *  @param phi   Cherenkov angle phi
     *  @param activeFrac The fraction of the associate segment tat this photon could have been radiated from
     */
    RichGeomPhoton( const float theta      = 0,
                    const float phi        = 0,
                    const float activeFrac = 0 )
      : m_CherenkovTheta        ( theta      ),
        m_CherenkovPhi          ( phi        ),
        m_dataWord              ( 0          ),
        m_activeSegmentFraction ( activeFrac ) { }

    /** Constructor from full information
     *
     *  @param theta Cherenkov angle theta
     *  @param phi   Cherenkov angle phi
     *  @param emissionPoint The assumed photon emission point
     *  @param detectionPoint The photon detection point on the HPDs
     *  @param sphMirrReflPoint The reflection point on the spherical mirrors
     *  @param flatMirrReflPoint The reflection point on the flat mirrors
     *  @param id The RichSmartID channel identifier for the detection channel
     *  @param activeFrac The fraction of the associate segment tat this photon could have been radiated from
     */
    RichGeomPhoton( const float theta,
                    const float phi,
                    const Gaudi::XYZPoint & emissionPoint,
                    const Gaudi::XYZPoint & detectionPoint,
                    const Gaudi::XYZPoint & sphMirrReflPoint,
                    const Gaudi::XYZPoint & flatMirrReflPoint,
                    const LHCb::RichSmartID id = LHCb::RichSmartID(0),
                    const float activeFrac = 0 )
      : m_CherenkovTheta         ( theta             ),
        m_CherenkovPhi           ( phi               ),
        m_emissionPoint          ( emissionPoint     ),
        m_detectionPoint         ( detectionPoint    ),
        m_sphMirReflectionPoint  ( sphMirrReflPoint  ),
        m_flatMirReflectionPoint ( flatMirrReflPoint ),
        m_smartID                ( id                ),
        m_activeSegmentFraction  ( activeFrac        ) { }

    ~RichGeomPhoton( ) {} ///< Destructor

    /**
     * Set accessor for the Cherenkov theta angle
     * @param theta the new value for the Cherenkov theta angle
     */
    inline void setCherenkovTheta ( const float theta )
    {
      m_CherenkovTheta = theta;
    }

    /**
     * Get accessor for the Cherenkov theta angle
     * @return the current value of the Cherenkov theta angle
     */
    inline float CherenkovTheta () const
    {
      return m_CherenkovTheta;
    }

    /**
     * Set accessor for Cherenkov phi angle
     * @param phi the new value for the Cherenkov phi angle
     */
    inline void setCherenkovPhi (const float phi)
    {
      m_CherenkovPhi = phi;
    }

    /**
     * Get accessor for Cherenkov phi angle
     * @return the current value of the Cherenkov phi angle
     */
    inline float CherenkovPhi () const
    {
      return m_CherenkovPhi;
    }

    /**
     * Set accessor for the estimated emission point
     * @param emissionPoint the new value for the estimated emission point
     */
    inline void setEmissionPoint ( const Gaudi::XYZPoint& emissionPoint )
    {
      m_emissionPoint = emissionPoint;
    }

    /**
     * Const Get accessor for the estimated emission point
     * @return the current value of the estimated emission point
     */
    inline const Gaudi::XYZPoint& emissionPoint () const
    {
      return m_emissionPoint;
    }

    /**
     * Get accessor for the estimated emission point
     * @return the current value of the estimated emission point
     */
    inline Gaudi::XYZPoint& emissionPoint ()
    {
      return m_emissionPoint;
    }

    /**
     * Set accessor for the detection point
     * @param detectionPoint the new value for the detection point
     */
    inline void setDetectionPoint (const Gaudi::XYZPoint& detectionPoint)
    {
      m_detectionPoint = detectionPoint;
    }

    /**
     * Const Get accessor for the detection point
     * @return the current value of the detection point
     */
    inline const Gaudi::XYZPoint& detectionPoint () const
    {
      return m_detectionPoint;
    }

    /**
     * Get accessor for the detection point
     * @return the current value of the detection point
     */
    inline Gaudi::XYZPoint& detectionPoint ()
    {
      return m_detectionPoint;
    }

    /**
     * Set accessor to spherical mirror number status
     * @param OK status
     */
    inline void setMirrorNumValid( const bool OK )
    {
      const RichGeomPhotonCode::ShortType i = ( OK ? 1 : 0 );
      setDataWord( ((i << RichGeomPhotonCode::ShiftMirrValid) &
                    RichGeomPhotonCode::MaskMirrValid) |
                   (dataWord() & ~RichGeomPhotonCode::MaskMirrValid) );
    }

    /** The status of the mirror numbers
     *  @return Boolean indicating if the mirror numbers are valid
     */
    inline bool mirrorNumValid() const
    {
      return ( ((dataWord() & RichGeomPhotonCode::MaskMirrValid)
                >> RichGeomPhotonCode::ShiftMirrValid) != 0 );
    }

    /**
     * Set accessor to spherical mirror number
     * @param num the new value for the spherical mirror number
     */
    inline bool setSphMirrorNum( const RichGeomPhotonCode::ShortType num )
    {
      return ( dataInRange(num,RichGeomPhotonCode::MaxSphMirr) ?
               set( num,
                    RichGeomPhotonCode::ShiftSphMirr,
                    RichGeomPhotonCode::MaskSphMirr,
                    RichGeomPhotonCode::MaskSphMirrOK ) : false );
    }

    /**
     * Get accessor to spherical mirror number
     * @return the spherical mirror number
     */
    inline RichGeomPhotonCode::ShortType sphMirrorNum() const
    {
      return (dataWord() & RichGeomPhotonCode::MaskSphMirr) >> RichGeomPhotonCode::ShiftSphMirr;
    }

    /**
     * Test if the spherical mirror number has been set
     * @return boolean indicating the status of the spherical mirror number
     */
    inline bool sphMirrorIsSet() const
    {
      return ( ((dataWord() & RichGeomPhotonCode::MaskSphMirrOK)
                >> RichGeomPhotonCode::ShiftSphMirrOK) != 0 );
    }

    /**
     * Set accessor to flat mirror number (1-40).
     * @param num the new value for the flat mirror number
     */
    inline bool setFlatMirrorNum( const RichGeomPhotonCode::ShortType num )
    {
      return ( dataInRange(num,RichGeomPhotonCode::MaxFlatMirr) ?
               set( num,
                    RichGeomPhotonCode::ShiftFlatMirr,
                    RichGeomPhotonCode::MaskFlatMirr,
                    RichGeomPhotonCode::MaskFlatMirrOK ) : false );
    }

    /**
     * Get accessor to flat mirror number
     * @return the flat mirror number
     */
    inline RichGeomPhotonCode::ShortType flatMirrorNum () const
    {
      return (dataWord() & RichGeomPhotonCode::MaskFlatMirr) >> RichGeomPhotonCode::ShiftFlatMirr;
    }

    /**
     * Test if the flat mirror number has been set
     * @return boolean indicating the status of the flat mirror number
     */
    inline bool flatMirrorIsSet() const
    {
      return ( ((dataWord() & RichGeomPhotonCode::MaskFlatMirrOK)
                >> RichGeomPhotonCode::ShiftFlatMirrOK) != 0 );
    }

    /**
     * Set accessor to the RichSmartID
     * @param smartID the new value for m_smartID
     */
    inline void setSmartID ( const LHCb::RichSmartID smartID )
    {
      m_smartID = smartID;
    }

    /**
     * Const Get accessor to RichSmartID
     * @return the current value of RichSmartID
     */
    inline LHCb::RichSmartID smartID() const
    {
      return m_smartID;
    }

    /**
     * Set accessor for the spherical mirror reflection point
     * @param sphMirReflectionPoint the new value for the spherical mirror reflection point
     */
    inline void
    setSphMirReflectionPoint ( const Gaudi::XYZPoint& sphMirReflectionPoint )
    {
      m_sphMirReflectionPoint = sphMirReflectionPoint;
    }

    /**
     * Const Get accessor for the spherical mirror reflection point
     * @return the current value of the spherical mirror reflection point
     */
    inline const Gaudi::XYZPoint& sphMirReflectionPoint () const
    {
      return m_sphMirReflectionPoint;
    }

    /**
     * Get accessor for the spherical mirror reflection point
     * @return the current value of the spherical mirror reflection point
     */
    inline Gaudi::XYZPoint& sphMirReflectionPoint ()
    {
      return m_sphMirReflectionPoint;
    }

    /**
     * Set accessor for the flat mirror reflection point
     * @param flatMirReflectionPoint the new value for the flat mirror reflection point
     */
    inline void
    setFlatMirReflectionPoint( const Gaudi::XYZPoint& flatMirReflectionPoint )
    {
      m_flatMirReflectionPoint = flatMirReflectionPoint;
    }

    /**
     * Const Get accessor for the flat mirror reflection point
     * @return the current value of the flat mirror reflection point
     */
    inline const Gaudi::XYZPoint& flatMirReflectionPoint () const
    {
      return m_flatMirReflectionPoint;
    }

    /**
     * Get accessor for the flat mirror reflection point
     * @return the current value of the flat mirror reflection point
     */
    inline Gaudi::XYZPoint& flatMirReflectionPoint ()
    {
      return m_flatMirReflectionPoint;
    }

    /**
     * Set accessor for the current active segment fraction.
     * The fraction of the RichTrackSegment trajectory this photon is associated
     * with for which it is geometrically possible this photon was produced
     *
     * @param fraction the new value for the active fraction
     */
    inline void setActiveSegmentFraction ( const float fraction )
    {
      m_activeSegmentFraction = fraction;
    }

    /**
     * Get accessor to the current active segment fraction.
     * The fraction of the RichTrackSegment trajectory this photon is associated
     * with for which it is geometrically possible this photon was produced
     *
     * @return the current value of the current active segment fraction.
     */
    inline float activeSegmentFraction () const
    {
      return m_activeSegmentFraction;
    }

    /**
     * Sets the raw data word
     * @param data The new raw data word
     */
    inline void setDataWord( const RichGeomPhotonCode::ShortType data )
    {
      m_dataWord = data;
    }

    /**
     * Returns the raw data word
     * @return the bit packed data field
     */
    inline RichGeomPhotonCode::ShortType dataWord() const
    {
      return m_dataWord;
    }

  private: // methods

    /// Set the data
    inline bool set( const RichGeomPhotonCode::ShortType value, ///< The data value for field being set
                     const RichGeomPhotonCode::ShortType shift, ///< The shift value for field being set
                     const RichGeomPhotonCode::ShortType mask,  ///< The mask value for field being set
                     const RichGeomPhotonCode::ShortType okMask ///< The "Data is OK" mask value for field being set
                     )
    {
      setDataWord( ((value << shift) & mask) | (dataWord() & ~mask) | okMask );
      return true;
    };

    /// Tests whether a given value is in range for a given data field
    inline bool dataInRange( const RichGeomPhotonCode::ShortType value, ///< The data value to test
                             const RichGeomPhotonCode::ShortType max    ///< The maximum value that can be stored
                             ) const
    {
      return ( value <= max );
    }

  private: // data

    float m_CherenkovTheta;                    ///< Cherenkov angle theta
    float m_CherenkovPhi;                      ///< Cherenkov angle phi
    Gaudi::XYZPoint m_emissionPoint;           ///< The photon emission point
    Gaudi::XYZPoint m_detectionPoint;          ///< The photon detection point on the HPDS
    Gaudi::XYZPoint m_sphMirReflectionPoint;   ///< The spherical mirror reflection point
    Gaudi::XYZPoint m_flatMirReflectionPoint;  ///< The flat mirror relfection point
    RichGeomPhotonCode::ShortType m_dataWord;  ///< Bit-pack data word
    LHCb::RichSmartID m_smartID;               ///< RichSmartID for detection channel

    /** The fraction of the RichTrackSegment trajectory this photon is associated
     *  with for which it is geometrically possible this photon was produced
     */
    float m_activeSegmentFraction;

  };

} // end LHCb namespace

/// Implement ostream << method for RichGeomPhoton
inline std::ostream& operator << ( std::ostream& s,
                                   const LHCb::RichGeomPhoton& photon )
{
  s << "{ "
    << " cherenkovTheta:\t" << photon.CherenkovTheta() << std::endl
    << " cherenkovPhi:\t" << photon.CherenkovPhi() << std::endl
    << " emissionPoint:\t" << photon.emissionPoint() << std::endl
    << " detectionPoint:\t" << photon.detectionPoint() << std::endl
    << " sphMirReflectionPoint:\t"<<photon.sphMirReflectionPoint()<<std::endl
    << " flatMirReflectionPoint:\t"<<photon.flatMirReflectionPoint()<<std::endl
    << " sphMirNumber:\t" << photon.sphMirrorNum()<<std::endl
    << " flatMirNumber:\t" << photon.flatMirrorNum()<<std::endl
    << " smartID:\t" << photon.smartID() << std::endl
    << " activeSegmentFraction:\t" << photon.activeSegmentFraction() << " } ";
  return s;
}


#endif // RICHEVENT_RICHGEOMPHOTON_H
