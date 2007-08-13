// $Id: VeloPhiHit.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
#ifndef INCLUDE_VELOPHIHIT_H
#define INCLUDE_VELOPHIHIT_H 1

#include "VeloDet/DeVeloPhiType.h"
#include "Event/VeloLiteCluster.h"

#include "TfKernel/VeloHit.h"

namespace Tf {

  /** @class VeloPhiHit VeloPhiHit.h
   *  Representation of VELO phi measurement
   *
   * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   * @date   2007-06-15
   */

  class VeloPhiHit : public VeloHit {

  public:

      //----------------------------------------------------------------------
      // typedefs
      //----------------------------------------------------------------------

      typedef std::vector<VeloPhiHit* > container_type; 
      typedef LoKi::Range_<container_type> range_type;

      typedef velo_phihit_tag hit_type_tag; ///< the hit type tag

    public:

      /// Standard Constructor
      inline VeloPhiHit(const DeVeloPhiType* sensor, const LHCb::VeloLiteCluster& clu, float signal);

      //== simple accessors
      double phi()                           const { return m_coord; }
      double phiHalfBox()                    const { return m_coordHalfBox; }
      double phiIdeal()                      const { return m_coordIdeal; }
      double z()                             const { return m_sensor->z(); }
      const DeVeloPhiType* sensor()          const { return m_sensor; } 

    private:

      const DeVeloPhiType* m_sensor;        ///< link to detector element
  };

  //----------------------------------------------------------------------
  // shortcut typedefs
  //----------------------------------------------------------------------

  typedef VeloPhiHit::container_type VeloPhiHits; 
  typedef VeloPhiHit::range_type VeloPhiHitRange;

  //----------------------------------------------------------------------
  // inline implementations
  //----------------------------------------------------------------------

  // constructor
  inline VeloPhiHit::VeloPhiHit(const DeVeloPhiType* s, const LHCb::VeloLiteCluster& c, float signal)
    : VeloHit(c
        , RegionID(c.channelID(),s)
        , s->globalPhiOfStrip(c.channelID().strip())+c.interStripFraction()*s->phiPitch(c.channelID().strip())
        , s->halfboxPhiOfStrip(c.channelID().strip())+c.interStripFraction()*s->phiPitch(c.channelID().strip())
        , s->idealPhiOfStrip(c.channelID().strip())+c.interStripFraction()*(s->phiPitch(c.channelID().strip()))
        , ((s->phiPitch(c.channelID().strip())*c.pseudoSize())*(s->phiPitch(c.channelID().strip())*c.pseudoSize()))/
        float(12)
        , signal
        , ( c.channelID().strip() < 683 ? 0 : 1 )
        )
    , m_sensor(s)
  {
    ;
  }

  // our dynamic casts
  inline const VeloPhiHit* HitBase::veloPhiHit() const
  {
    return type()==RegionID::VeloPhi ? static_cast<const VeloPhiHit*>(this) : 0;
  }
}

#endif // INCLUDE_VELOPHIHIT_H

