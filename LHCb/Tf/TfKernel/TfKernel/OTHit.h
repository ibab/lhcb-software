
//-----------------------------------------------------------------------------
/** @file OTHit.h
 *
 *  Header file for track finding class Tf::OTHit
 *
 *  CVS Log :-
 *  $Id: OTHit.h,v 1.7 2007-09-07 13:32:10 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_OTHIT_H 
#define TFKERNEL_OTHIT_H 1

// Include files
#include "TfKernel/LineHit.h"
#include "LoKi/Range.h"

namespace Tf
{

  /** @class OTHit
   *
   *  Representation of an OT hit
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class OTHit : public LineHit
  {

  public:

    typedef DeOTModule DetectorElementType ; ///< Detector element type for OT
    typedef ot_hit_tag             hit_tag ; ///< The hit type tag

  public:

    /** Constructor from a DeOTModule and an OTLiteTime
     *  @param[in] aModule Reference to the associated DeOTModule
     *  @param[in] rawhit  The raw OT hit (OTLiteTime)
     */
    OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit );

  public: // Simple accessors to internal data members

    /** Returns the calibrated time is drift time + propagation time - default tof */
    inline float calibratedTime() const { return m_calibratedTime; }

    /** Access the associated DeOTModule for this hit
     *  @return Reference to the associated DeOTModule */
    inline const DeOTModule& module() const { return *m_module; }

    /** Access the raw hit information (OTLiteTime)
     *  @return The OTLiteTime for this hit */
    inline const LHCb::OTLiteTime& rawhit() const { return m_rawhit ; }

    /** Access the default drift distance. 
     *  Defined at the point halfway along the length the wire.
     *  @return The drift distrance */
    inline float driftDistance()  const { return m_driftDistance; }

    /** Access the y-coordinate of the readout side of the wire (defines t0) */
    inline float  yReadout()   const { return yEnd() ; }

    /** Access the propagation time relative to the default, which is halfway along the
        wire. The velocity has been corrected for sign and direction */
    inline float propagationTime( const double globaly ) const 
    {
      return (float)((yReadout()- globaly)/module().propagationVelocityY()); 
    }

    /** The drift time after correction for propagation time */
    inline float driftTime( const double globaly ) const 
    { 
      return calibratedTime() - propagationTime( globaly ) ;
    }

    /** The drift distance after correction for propagation time */
    inline float driftDistance( const double globaly ) const
    { 
      return module().driftRadius( driftTime( globaly) ) ; 
    }

    /** Test if this hit is out-of-time */
    bool outOfTime( const double globaly, const double numsigma ) const ;

    /** obsolete. please, don't use. To select valid drifttimes, cut directly on the drifttime.
     *  XXX???XXX If obsolete, should we not remove this method entirely */
    inline float untruncatedDriftDistance( const double globaly ) const 
    { 
      return module().untruncatedDriftRadius( driftTime( globaly) ) ; 
    }

    /** obsolete. please, don't use. To select valid drifttimes, cut directly on the drifttime.
     *  XXX???XXX If obsolete, should we not remove this method entirely */
    inline float untruncatedDriftDistance() const 
    { 
      return module().untruncatedDriftRadius( driftTime(yMid()) ) ; 
    }

    /** Access the straw number for this hit */
    inline unsigned int straw() const { return m_rawhit.channel().straw() ; }

    /** Access the monolayer number for this hit (XXX???XXX what exactly is this) */
    inline unsigned int monolayer() const { return module().monoLayerB(straw()) ; }

    /** The length of the wire */
    inline double wireLength() const { return module().wireLength(m_rawhit.channel()); }

  private: // data

    const DeOTModule* m_module ;         ///< Pointer to the associated DeOTModule
    LHCb::OTLiteTime  m_rawhit ;         ///< The raw OTLiteTime for this hit
    float             m_calibratedTime ; ///< The calibrated time = drift time + propagation time - default tof
    float             m_driftDistance ;  ///< The default drift distance. Defined at the point halfway along the length the wire.

  };

  /// Type for container for OTHit
  typedef std::vector<const OTHit*> OTHits;

  /// Type for range of OTHits within a container
  typedef LoKi::Range_<OTHits> OTHitRange;

  /// Type for a container for OTHitRange
  typedef std::vector<OTHitRange> OTHitRanges;

  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions
  ////////////////////////////////////////////////////////////////////////////////////

  inline OTHit::OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit) :
    LineHit(aModule,rawhit),
    m_module(&aModule),
    m_rawhit(rawhit),
    m_calibratedTime( m_rawhit.rawTime() - module().strawT0(m_rawhit.channel().straw() ) )
  {
    // setting things from the OTHit. the cached drift distance is the
    // one in the middle of the wire, for now.
    const float time = driftTime(yMid()) ;
    OTDet::RadiusWithError r = aModule.driftRadiusWithError( time ) ;
    m_driftDistance  = r.val ;
    setVariance(r.err*r.err) ;
  }

  inline bool OTHit::outOfTime( double globaly, double numsigma ) const 
  {
    const float tres = module().driftTimeResolution(m_driftDistance) ;
    const float tmax = module().maxDriftTime() ;
    const float t    = driftTime( globaly) ;
    return t<-numsigma*tres || t>tmax + numsigma*tres ;
  }

  // our dynamic casts
  inline const OTHit* HitBase::othit() const 
  { 
    return ( type() == RegionID::OT ? static_cast<const OTHit*>(this) : NULL ); 
  }

}
#endif // TFKERNEL_OTHIT_H 
