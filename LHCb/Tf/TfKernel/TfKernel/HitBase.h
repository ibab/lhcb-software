#ifndef HITBASE_H 
#define HITBASE_H 1

// Include files
#include "Kernel/LHCbID.h"

#include "TfKernel/RegionID.h"
#include <bitset>

namespace Tf {

  class VeloRHit;
  class VeloPhiHit;
  class STHit;
  class OTHit;

  //-------------------------------------------------------------------
  // Hit type tags
  //-------------------------------------------------------------------
  struct hit_base_tag                          {};  ///< type tag for hit base
  struct line_hit_tag    : public hit_base_tag {};  ///< type tag for line hit
  struct ot_hit_tag      : public line_hit_tag {};  ///< type tag for OT hit
  struct st_hit_tag      : public line_hit_tag {};  ///< type tag for ST hit
  struct velo_hit_tag    : public hit_base_tag {};  ///< type tag for Velo hit
  struct velo_rhit_tag   : public velo_hit_tag {};  ///< type tag for Velo R hit
  struct velo_phihit_tag : public velo_hit_tag {};  ///< type tag for Velo Phi hit
  
  /** @class HitBase
   *  Base class for storage of hits in all tracking systems
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class HitBase
  {

  public:
    //== Enumerated type for bits in status flag.
    enum EStatus { UsedByTsaSeeding=0, UsedByPatForward, UsedByPatSeeding, UsedByVeloPhi, 
		   UsedByVeloRZ, UsedByUnknown, Unusable, NUMSTATUSBITS } ;
    //== Nested type that describes 'use' status of this hit
    typedef std::bitset<NUMSTATUSBITS> StatusFlag ;

    typedef hit_base_tag hit_type_tag; ///< the hit type tag

    //== Simple accessors to internal data members
    float        coord()         const { return m_coord ; }
    float        variance()      const { return m_variance ; }
    float        weight  ()      const { return 1/m_variance;  }
    LHCb::LHCbID lhcbID  ()      const { return m_lhcbID;  }
    RegionID     regionID()      const { return m_regionID; }
    StatusFlag   status  ()      const { return m_status ; }

    bool         testStatus( EStatus stat) const { return m_status.test(stat) ; }
    void         setStatus ( EStatus stat, bool val=true ) const { m_status.set(stat,val) ; }
    bool         isUsed    () const { return m_status.any() ; }
    void         setUsed   ( bool flag ) const { m_status.set(UsedByUnknown,flag) ; }

    unsigned int region  () const { return static_cast<unsigned int>(m_regionID.region()); }
    unsigned int layer   () const { return static_cast<unsigned int>(m_regionID.layer()); }
    unsigned int station () const { return static_cast<unsigned int>(m_regionID.station()); }
    unsigned int type    () const { return static_cast<unsigned int>(m_regionID.type()); }
    unsigned int veloHalf() const { return static_cast<unsigned int>(m_regionID.veloHalf()); }
    unsigned int zone    () const { return static_cast<unsigned int>(m_regionID.zone()); }

    // dynamic casts

    const VeloRHit*   veloRHit()   const;
    const VeloPhiHit* veloPhiHit() const;
    const OTHit*      othit()      const ;
    const STHit*      sthit()      const ;

  protected:
    // only inherited objects are supposed to be created
    HitBase() {}
    HitBase(const LHCb::LHCbID& id, const RegionID& regionid, float coord=0, float variance=0 )
      : m_lhcbID (id), m_regionID (regionid), m_coord(coord), m_variance (variance) {}

    void setCoord(float x) { m_coord = x ; }
    void setVariance(float v) { m_variance = v ; }

  protected:
    LHCb::LHCbID m_lhcbID;
    RegionID m_regionID ;
    float m_coord ;
    float m_variance;
    mutable StatusFlag m_status ;
  };

}

#endif // HITBASE_H
