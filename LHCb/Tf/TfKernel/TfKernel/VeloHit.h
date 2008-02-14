// $Id: VeloHit.h,v 1.5 2008-02-14 16:17:00 cattanem Exp $
#ifndef INCLUDE_TF_VELOHIT_H
#define INCLUDE_TF_VELOHIT_H 1

#include "LoKi/Range.h"
#include "Event/VeloLiteCluster.h"

#include "TfKernel/HitBase.h"

namespace Tf {

  /** @class VeloHit VeloHit.h
   *  Representation of VELO  measurement
   *
   * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   * @date   2007-07-17
   */

  class VeloHit : public HitBase {

  public:

    typedef velo_hit_tag hit_type_tag; ///< the hit type tag


    //== simple accessors
    double coordHalfBox()                  const { return m_coordHalfBox; }
    double coordIdeal()                    const { return m_coordIdeal; }
    double signal()                        const { return m_signal; }
    const LHCb::VeloLiteCluster& cluster() const { return m_clu; }
    const LHCb::VeloChannelID channelID()  const { return m_clu.channelID(); }
    bool highThreshold()                   const { return m_clu.highThreshold(); }
    unsigned int strip()                   const { return m_clu.channelID().strip(); }
    double interStripFraction()            const { return m_clu.interStripFraction(); }
    unsigned int size()                    const { return m_clu.pseudoSize(); }
    bool isPileUp()                        const { return m_clu.isPileUp(); }

    //----------------------------------------------------------------------
    // functors
    //----------------------------------------------------------------------

    /** @struct Compare
     *  Functor to compare hits by global r
     *
     * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     * @date   2007-07-04
     */
    struct Compare  {
      template<typename HIT>
      bool operator() (const HIT* lhs, const HIT* rhs) const
      {
        return lhs->sortCoord() < rhs->sortCoord();
      }
    };

    /** @struct CompareHalfBox
     *  Functor to compare hits by r in half box frame
     *
     * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     * @date   2007-07-18
     */
    struct CompareHalfBox  {
      template<typename HIT>
      bool operator() (const HIT* lhs, const HIT* rhs) const
      {
        return lhs->sortCoordHalfBox() < rhs->sortCoordHalfBox();
      }
    };

    /** @struct CompareIdeal
     *  Functor to compare hits by r for ideal sensor positions
     *
     * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     * @date   2007-07-18
     */
    struct CompareIdeal  {
      template<typename HIT>
      bool operator() (const HIT* lhs, const HIT* rhs) const
      {
        return lhs->sortCoordIdeal() < rhs->sortCoordIdeal();
      }
    };

    /** @struct IncreasingByZ
     *  Functor to compare hits by global z
     *
     * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     * @date   2007-07-10
     */
    struct IncreasingByZ  {
      template<typename HIT>
      bool operator() (const HIT* lhs, const HIT* rhs) const
      {
        return lhs->z() < rhs->z();
      }
    };

    /** @struct DecreasingByZ
     *  Functor to compare hits by global z
     *
     * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     * @date   2007-07-10
     */
    struct DecreasingByZ  {
      template<typename HIT>
      bool operator() (const HIT* lhs, const HIT* rhs) const
      {
        return lhs->z() > rhs->z();
      }
    };

  protected:

    /// Standard Constructor, only derived types can construct this
    inline VeloHit(const LHCb::VeloLiteCluster& c
                   , RegionID rid
                   , float coord
                   , float coordHalfBox
                   , float coordIdeal
                   , float variance
                   , float signal);

  protected:

    LHCb::VeloLiteCluster m_clu; ///< the cluster
    float m_coordHalfBox;        ///< the coordinate in the VELO halfbox frame
    float m_coordIdeal;          ///< the coordinate in the ideal global frame
    float m_signal;              ///< the total ADC counts of the cluster (if constructed from a full cluster)

  };


  //----------------------------------------------------------------------
  // inline implementations
  //----------------------------------------------------------------------

  // constructor
  inline VeloHit::VeloHit(const LHCb::VeloLiteCluster& c
                          , RegionID rid
                          , float coord
                          , float coordHalfBox
                          , float coordIdeal
                          , float variance
                          , float signal)
    : HitBase(LHCb::LHCbID(c.channelID())
              , rid
              , coord
              , variance)
    , m_clu(c)
    , m_coordHalfBox(coordHalfBox)
    , m_coordIdeal(coordIdeal)
    , m_signal(signal)
  { ; }

}

#endif // INCLUDE_TF_VELOHIT_H

