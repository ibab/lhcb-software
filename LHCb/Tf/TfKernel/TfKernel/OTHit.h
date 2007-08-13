#ifndef OTHIT_H 
#define OTHIT_H 1

// Include files
#include "TfKernel/LineHit.h"
#include "Event/OTTime.h"
#include "LoKi/Range.h"

namespace Tf
{

  /** @class OTHit
   *
   *  Store an OT coordinate
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class OTHit : public LineHit
  {
  public:
    typedef DeOTModule DetectorElementType ;

    typedef ot_hit_tag hit_tag; ///< the hit type tag

    // This is the 'official' constructor. It is still too slow.
    OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit) ;

    //== Simple accessors to internal data members
    /** calibrated time is drift time + propagation time - default tof */
    float calibratedTime() const { return m_calibratedTime ; }
    const DeOTModule& module() const { return *m_module ; }
    const LHCb::OTLiteTime& rawhit() const { return m_rawhit ; }
    /** default drift distance. defined halfway the wire */
    float driftDistance()  const { return m_driftDistance; }
    
    /** y-coordinate of the readout side of the wire (defines t0) */
    float  yReadout()   const { return yEnd() ; }
    
    /** propagation time relative to the default, which is halfway the
	wire. The velocity has been corrected for sign and direction */
    float propagationTime( double globaly ) const {
      return ( yReadout() -  globaly )/module().propagationVelocityY()  ; }
    /** drift time after correction for propagation time */
    float driftTime( double globaly ) const { return calibratedTime() - propagationTime( globaly ) ; }
    /** drift distance after correction for propagation time */
    float driftDistance( double globaly ) const { return module().driftDistance( driftTime( globaly) ) ; }
    bool outOfTime( double globaly, double numsigma ) const ;
    // obsolete. please, don't use. to select valid drifttimes, cut directly on the drifttime.
    float untruncatedDriftDistance( double globaly ) const { return module().untruncatedDriftDistance( driftTime( globaly) ) ; }
    float untruncatedDriftDistance()  const { return module().untruncatedDriftDistance( driftTime(yMid()) ) ; }
    
    unsigned int straw() const { return m_rawhit.channel().straw() ; }
    unsigned int monolayer() const { return module().monoLayerB(straw()) ; }
    float wireLength()    const { return module().wireLength(m_rawhit.channel()); }
  private:
    // These are the data members
    const DeOTModule*        m_module ;
    LHCb::OTLiteTime         m_rawhit ;
    float                    m_calibratedTime ;
    float                    m_driftDistance ;
  };

  /// Type for container for OTHit
  typedef std::vector<const OTHit*> OTHits;
  /// Type for range of OTHits within a container 
  typedef LoKi::Range_<OTHits> OTHitRange ;
  /// Type for a container for OTHitRange
  typedef std::vector<OTHitRange> OTHitRanges;

  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions
  ////////////////////////////////////////////////////////////////////////////////////

  inline OTHit::OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit)
    : 
    LineHit(aModule,rawhit),
    m_module(&aModule),
    m_rawhit(rawhit),
    m_calibratedTime( m_rawhit.rawTime() - module().strawT0(m_rawhit.channel().straw() ) )
  {
    // setting things from the OTHit. the cached drift distance is the
    // one in the middle of the wire, for now.
    float time = driftTime(yMid()) ;
    m_driftDistance  = aModule.driftDistance(time) ;
    float res = aModule.resolution(time) ;
    setVariance(res*res) ;
  }

  inline bool OTHit::outOfTime( double globaly, double numsigma ) const {
    float tres = module().driftTimeResolution(m_driftDistance) ;
    float tmax = module().maxDriftTime() ;
    float t    = driftTime( globaly) ;
    return t<-numsigma*tres || t>tmax + numsigma*tres ;
  }

  // our dynamic casts
  inline const OTHit* HitBase::othit() const { return type()==RegionID::OT ? static_cast<const OTHit*>(this) : 0 ; }
}
#endif // OTHit_H
