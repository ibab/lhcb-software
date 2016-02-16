// $Id: VeloHitExtension.h,v 1.5 2008-05-29 10:55:12 smenzeme Exp $
#ifndef INCLUDE_TF_VELOHITEXTENSION_H
#define INCLUDE_TF_VELOHITEXTENSION_H 1

#include "TfKernel/HitExtension.h"

namespace Tf {

  /** @class VeloHitExtension VeloHitExtension.h
   * Base class from which VELO algorithms can derive their specific hit classes.
   *
   * This is an extension of the HitExtension class which provides the minimal interface
   * necessary to make it a possible type parameter of the VeloSensorHits template.
   *
   * @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   * @date   2007-07-30
   * @see Tf::HitExtension
   * @see Tf::VeloStationData
   */
  template <typename HIT>
  class VeloHitExtension : public HitExtension<HIT> {

    using HitExtension<HIT>::m_hit;

  public:

    typedef typename HIT::Compare        Compare;        ///< make functor visible as dependent type
    typedef typename HIT::CompareHalfBox CompareHalfBox; ///< make functor visible as dependent type
    typedef typename HIT::CompareIdeal   CompareIdeal;   ///< make functor visible as dependent type
    typedef typename HIT::IncreasingByZ  IncreasingByZ;  ///< make functor visible as dependent type
    typedef typename HIT::DecreasingByZ  DecreasingByZ;  ///< make functor visible as dependent type


  public:

    /// Constructor from pointer
    VeloHitExtension( const HIT* hit = 0 )
      : HitExtension<HIT>(hit)
    {;}

    /// Constructor from reference
    VeloHitExtension( const HIT& hit )
      : HitExtension<HIT>(&hit)
    {;}

    /// Access to coordinate of underlying VELO hit
    double coord() const { return m_hit->coord(); }

    /// Access to coordinate in VELO half box frame of underlying VELO hit
    double coordHalfBox() const { return m_hit->coordHalfBox(); }

    /// Access to ideal coordinate of underlying VELO hit
    double coordIdeal() const { return m_hit->coordIdeal(); }

    /// Access to sort coordinate in global frame of the underlying VELO hit.
    double sortCoord() const { return m_hit->sortCoord(); }

    /// Access to sort coordinate in half box frame of the underlying VELO hit.
    double sortCoordHalfBox() const { return m_hit->sortCoordHalfBox(); }

    /// Access to sort coordinate in ideal frame of the underlying VELO hit.
    double sortCoordIdeal() const { return m_hit->sortCoordIdeal(); }


    /// Access to z position of underlying VELO hit
    double z() const { return m_hit->z(); }

    /// Access to LHCbID
    LHCb::LHCbID lhcbID() const { return m_hit->lhcbID(); }

  };
}
#endif // INCLUDE_TF_VELOHITEXTENSION_H

