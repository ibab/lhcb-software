// $Id: VeloRHit.h,v 1.2 2007-08-25 19:49:04 krinnert Exp $
#ifndef INCLUDE_TF_VELORHIT_H
#define INCLUDE_TF_VELORHIT_H 1

#include "LoKi/Range.h"
#include "VeloDet/DeVeloRType.h"
#include "Event/VeloLiteCluster.h"

#include "TfKernel/VeloHit.h"

namespace Tf {

  /** @class VeloRHit VeloRHit.h
   *  Representation of VELO r measurement
   *
   * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   * @date   2007-06-12
   */

  class VeloRHit : public VeloHit {

    public:

      //----------------------------------------------------------------------
      // shortcut typedefs
      //----------------------------------------------------------------------

      typedef std::vector<VeloRHit* > container_type; 
      typedef LoKi::Range_<container_type> range_type;

      typedef velo_rhit_tag hit_type_tag; ///< the hit type tag

    public:

      /// Standard Constructor
      inline VeloRHit(const DeVeloRType* sensor, const LHCb::VeloLiteCluster& clus, float signal);

      //== simple accessors
      double r()                             const { return m_coord; }
      double rHalfBox()                      const { return m_coordHalfBox; }
      double rIdeal()                        const { return m_coordIdeal; }
      double z()                             const { return m_sensor->z(); }
      const DeVeloRType* sensor()            const { return m_sensor; } 

    private:

      const DeVeloRType* m_sensor;        ///< link to detector element

  };

  
  //----------------------------------------------------------------------
  // shortcut typedefs
  //----------------------------------------------------------------------

  typedef VeloRHit::container_type VeloRHits; 
  typedef VeloRHit::range_type VeloRHitRange;

  //----------------------------------------------------------------------
  // inline implementations  
  //----------------------------------------------------------------------

  // constructor
  inline VeloRHit::VeloRHit(const DeVeloRType* s, const LHCb::VeloLiteCluster& c, float signal)
    : VeloHit(c
        , RegionID(c.channelID(),s)
        , s->globalR(c.channelID().strip(),c.interStripFraction())
        , s->halfboxR(c.channelID().strip(),c.interStripFraction())
        , s->rOfStrip(c.channelID().strip(),c.interStripFraction())
        , ((s->rPitch(c.channelID().strip())*c.pseudoSize())*(s->rPitch(c.channelID().strip())*c.pseudoSize()))/
        float(12)
        , signal
        )
    , m_sensor(s)
    { ; }


  // our dynamic casts
  inline const VeloRHit* HitBase::veloRHit() const 
  { 
    return type()==RegionID::VeloR ? static_cast<const VeloRHit*>(this) : 0; 
  }

}

#endif // INCLUDE_TF_VELORHIT_H

