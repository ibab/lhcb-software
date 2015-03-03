// $Id: VeloHit.h,v 1.7 2010-04-07 05:17:03 wouter Exp $
#ifndef INCLUDE_TF_VELOHIT_H
#define INCLUDE_TF_VELOHIT_H 1

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
    double coordHalfBox()                  const noexcept { return m_coordHalfBox; }
    double coordIdeal()                    const noexcept { return m_coordIdeal; }
    double signal()                        const noexcept { return m_signal; }
    const LHCb::VeloLiteCluster& cluster() const noexcept { return m_clu; }
    const LHCb::VeloChannelID channelID()  const noexcept { return m_clu.channelID(); }
    bool highThreshold()                   const noexcept { return m_clu.highThreshold(); }
    unsigned int strip()                   const noexcept { return m_clu.channelID().strip(); }
    double interStripFraction()            const noexcept { return m_clu.interStripFraction(); }
    unsigned int size()                    const noexcept { return m_clu.pseudoSize(); }
    bool isPileUp()                        const noexcept { return m_clu.isPileUp(); }

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
      bool operator() (const HIT* lhs, const HIT* rhs) const noexcept
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
      bool operator() (const HIT* lhs, const HIT* rhs) const noexcept
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
      bool operator() (const HIT* lhs, const HIT* rhs) const noexcept
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
      bool operator() (const HIT* lhs, const HIT* rhs) const noexcept
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
      bool operator() (const HIT* lhs, const HIT* rhs) const noexcept
      {
        return lhs->z() > rhs->z();
      }
    };

  protected:

    /// Standard Constructor, only derived types can construct this
    inline VeloHit(const LHCb::VeloLiteCluster& c
                   , RegionID rid
                   , double coord
                   , double coordHalfBox
                   , double coordIdeal
                   , double error
                   , double signal);

  protected:

    double m_coordHalfBox;        ///< the coordinate in the VELO halfbox frame
    double m_coordIdeal;          ///< the coordinate in the ideal global frame
    double m_signal;              ///< the total ADC counts of the cluster (if constructed from a full cluster)
    LHCb::VeloLiteCluster m_clu; ///< the cluster

  };


  //----------------------------------------------------------------------
  // inline implementations
  //----------------------------------------------------------------------

  // constructor
  inline VeloHit::VeloHit(const LHCb::VeloLiteCluster& c
                          , RegionID rid
                          , double coord
                          , double coordHalfBox
                          , double coordIdeal
                          , double error
                          , double signal)
    : HitBase(LHCb::LHCbID(c.channelID())
              , rid
              , coord
              , error)
    , m_coordHalfBox(coordHalfBox)
    , m_coordIdeal(coordIdeal)
    , m_signal(signal)
    , m_clu(c)
  { ; }

}

#endif // INCLUDE_TF_VELOHIT_H

