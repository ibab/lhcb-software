/** @file PatSeedFits.h
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2014-10-08
 */
#ifndef PATSEEDFITS_H
#define PATSEEDFITS_H

#include "TfKernel/RegionID.h"
#include "TfKernel/SIMDChi2Fit.h"

#include "PatSeedTrack.h"

namespace PatSeedFits {
  using namespace Tf::SIMDChi2FitUtils;
  /// general measurement provider for the seeding
  template <class FITPOLICY, bool forceRLAmb, bool xOnly = false, bool stereoOnly = false>
  struct MeasurementProvider :
      public Tf::SIMDChi2FitUtils::MeasurementProvider<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    template <class PROJ>
    value_type wmeas(const hit_type& hit, const track_type& track,
	const PROJ& projector) const
    {
      const bool isIT = !hit->isOT();
      if (isIT) {
	return hit->x() * hit->hit()->errweight();
      } else {
	const value_type wx = track.cosine() * hit->x() * hit->hit()->errweight();
	const value_type wdx = hit->driftDistance() * hit->hit()->errweight();
	if (forceRLAmb && 0 != hit->rlAmb()) {
	  if (0 < hit->rlAmb())
	    return wx + wdx;
	  else
	    return wx - wdx;
	} else {
	  const value_type wproj = projector.wproj(track, hit);
	  const value_type dw = wx - wproj;
	  if (std::abs(dw + wdx) < std::abs(dw - wdx))
	    return wx + wdx;
	  else
	    return wx - wdx;
	}
      }
    }
    bool valid(const hit_type& hit) const
    { return hit->isSelected() && (!xOnly || hit->hit()->isX()) &&
      (!stereoOnly || !hit->hit()->isX()); }
  };

  /// measurement provider for tracks in xz projection
  template <class FITPOLICY>
  struct XMeasurementProjector : public MeasurementProjector<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(3 == nDim, "Fit Policy must have three fit parameters");
    value_type wproj(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      if (isIT) return track.xAtZ(hit->z()) * hit->hit()->errweight();
      else return track.xAtZ(hit->z()) * hit->hit()->errweight() * track.cosine();
    }
    vector_type wgrad(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      const value_type w = isIT ? hit->hit()->errweight() :
	(hit->hit()->errweight() * track.cosine());
      const value_type dz = (hit->z() - track.z0()) / value_type(1 << 10);
      return { { w, w * dz, w * dz * dz * (value_type(1) +
	  value_type(1 << 10) * dz * track.dRatio()) } };
    }
  };

  /// track updater for fits in xz projection
  template <class FITPOLICY, bool allowUpdateReject = true>
  struct XTrackUpdater : public TrackUpdater<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(3 == nDim, "Fit Policy must have three fit parameters");
    FitStatus updateTrack(track_type& track, const vector_type& dparam) const
    {
      if (allowUpdateReject && (std::abs(dparam[0]) > 1e4 ||
	    std::abs(dparam[1] / value_type(1 << 10)) > 5. ||
	    std::abs(dparam[2] / value_type(1 << 20)) > 1e-3))
	return UpdateRejected;
      // update track parameters
      track.updateParameters(dparam[0], dparam[1] / value_type(1 << 10),
	  dparam[2] / value_type(1 << 20));
      if (std::abs(dparam[0]) < 5e-3 &&
	  std::abs(dparam[1] / value_type(1 << 10)) < 5e-6 &&
	  std::abs(dparam[2] / value_type(1 << 20)) < 5e-9)
	return FitConverged;
      return UpdateAccepted;
    }
  };

  /// measurement provider for simultaneous track fits in xz and yz
  template <class FITPOLICY>
  struct YMeasurementProjector : public MeasurementProjector<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(2 == nDim, "Fit Policy must have two fit parameters");
    value_type wproj(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      if (isIT) return track.xAtZ(hit->z()) * hit->hit()->errweight();
      else return track.xAtZ(hit->z()) * hit->hit()->errweight() * track.cosine();
    }
    vector_type wgrad(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      const value_type w = isIT ? hit->hit()->errweight() :
	(hit->hit()->errweight() * track.cosine());
      const value_type dz = (hit->z() - track.z0()) / value_type(1 << 10);
      const value_type dxdy = hit->hit()->dxDy();
      return { { -w * dxdy, -w * dxdy * dz } };
    }
  };

  /// track updater for simultaneous fits in xz and yz
  template <class FITPOLICY, bool allowUpdateReject = true>
  struct YTrackUpdater : public TrackUpdater<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(2 == nDim, "Fit Policy must have two fit parameters");
    FitStatus updateTrack(track_type& track, const vector_type& dparam) const
    {
      if (allowUpdateReject && (std::abs(dparam[0]) > 1e4 ||
	    std::abs(dparam[1] / value_type(1 << 10)) > 1.))
	return UpdateRejected;
      // update track parameters
      track.updateYParameters(
	  dparam[0] - track.z0() * dparam[0] / value_type(1 << 10),
	  dparam[1] / value_type(1 << 10));
      if (std::abs(dparam[0]) < 5e-2 &&
	  std::abs(dparam[1] / value_type(1 << 10)) < 5e-6)
	return FitConverged;
      return UpdateAccepted;
    }
  };

  /// measurement provider for simultaneous track fits in xz and yz
  template <class FITPOLICY>
  struct XYMeasurementProjector : public MeasurementProjector<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(5 == nDim, "Fit Policy must have five fit parameters");
    value_type wproj(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      if (isIT) return track.xAtZ(hit->z()) * hit->hit()->errweight();
      else return track.xAtZ(hit->z()) * hit->hit()->errweight() * track.cosine();
    }
    vector_type wgrad(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      const value_type w = isIT ? hit->hit()->errweight() :
	(hit->hit()->errweight() * track.cosine());
      const value_type dz = (hit->z() - track.z0()) / value_type(1 << 10);
      const value_type dxdy = hit->hit()->dxDy();
      return { { w, w * dz, w * dz * dz * (value_type(1) +
	  value_type(1 << 10) * dz * track.dRatio()),
	     -w * dxdy, -w * dxdy * dz } };
    }
  };

  /// track updater for simultaneous fits in xz and yz
  template <class FITPOLICY, bool allowUpdateReject = true>
  struct XYTrackUpdater : public TrackUpdater<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(5 == nDim, "Fit Policy must have five fit parameters");
    FitStatus updateTrack(track_type& track, const vector_type& dparam) const
    {
      if (allowUpdateReject && (std::abs(dparam[0]) > 1e4 ||
	    std::abs(dparam[1] / value_type(1 << 10)) > 5. ||
	    std::abs(dparam[2] / value_type(1 << 20)) > 1e-3 ||
	    std::abs(dparam[3]) > 1e4 ||
	    std::abs(dparam[4] / value_type(1 << 10)) > 1.))
	return UpdateRejected;
      // update track parameters
      track.updateParameters(dparam[0], dparam[1] / value_type(1 << 10),
	  dparam[2] / value_type(1 << 20));
      track.updateYParameters(
	  dparam[3] - track.z0() * dparam[4] / value_type(1 << 10),
	  dparam[4] / value_type(1 << 10));
      if (std::abs(dparam[0]) < 5e-3 &&
	  std::abs(dparam[1] / value_type(1 << 10)) < 5e-6 &&
	  std::abs(dparam[2] / value_type(1 << 20)) < 5e-9 &&
	  std::abs(dparam[3]) < 5e-2 &&
	  std::abs(dparam[4] / value_type(1 << 10)) < 5e-6)
	return FitConverged;
      return UpdateAccepted;
    }
  };

  /// measurement provider for stub fits
  template <class FITPOLICY>
  struct StubMeasurementProjector : public MeasurementProjector<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(3 == nDim, "Fit Policy must have three fit parameters");
    value_type wproj(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      if (isIT) return track.xAtZ(hit->z()) * hit->hit()->errweight();
      else return track.xAtZ(hit->z()) * hit->hit()->errweight() * track.cosine();
    }
    vector_type wgrad(const track_type& track, const hit_type& hit) const
    {
      const bool isIT = !hit->isOT();
      const value_type w = isIT ? hit->hit()->errweight() :
	(hit->hit()->errweight() * track.cosine());
      const value_type dz = (hit->z() - track.z0()) / value_type(1 << 10);
      const value_type dxdy = hit->hit()->dxDy();
      return { { -w * dxdy * hit->z() / track.z0(), w, w * dz } };
    }
  };

  /// track updater for stub fits
  template <class FITPOLICY, bool allowUpdateReject = true>
  struct StubTrackUpdater : public TrackUpdater<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    static_assert(3 == nDim, "Fit Policy must have three fit parameters");
    FitStatus updateTrack(track_type& track, const vector_type& dparam) const
    {
      if (allowUpdateReject && (std::abs(dparam[0]) > 1e3 ||
	    std::abs(dparam[1]) > 1e3 ||
	    std::abs(dparam[2] / value_type(1 << 10)) > 5.0))
	return UpdateRejected;
      // update track parameters
      track.updateParameters(
	  dparam[1], dparam[2] / value_type(1 << 10),
	  track.chi2() * (dparam[1] - track.z0() * dparam[2] / value_type(1 << 10)));
      track.updateYParameters(0., dparam[0] / track.z0());
      if (std::abs(dparam[0]) < 1e-5 &&
	  std::abs(dparam[1]) < 5e-3 &&
	  std::abs(dparam[2] / value_type(1 << 10)) < 5e-6)
	return FitConverged;
      return UpdateAccepted;
    }
  };

  /// null hit updater (no-op)
  template <class FITPOLICY>
    struct NullHitUpdater : public HitUpdater<FITPOLICY>
  {
    COMMONPOLICY_INJECT_TYPES(FITPOLICY);
    void updateHit(hit_type&, const track_type&) const { }
  };

  /// fit policy for xz fits
  template <unsigned maxIter>
  using XFitPolicy = CommonFitPolicy<double, PatSeedTrack, PatFwdHit*, 3, maxIter>;

  /// fit object (xz fits)
  template <bool forceRLAmb, unsigned maxIter,
	   bool xOnly = false, bool stereoOnly = false,
	   bool allowUpdateReject = true>
  using XFit = SIMDChi2Fit<
    MeasurementProvider<XFitPolicy<maxIter>, forceRLAmb, xOnly, stereoOnly>,
    XMeasurementProjector<XFitPolicy<maxIter> >,
    XTrackUpdater<XFitPolicy<maxIter>, allowUpdateReject>,
    NullHitUpdater<XFitPolicy<maxIter> >, 16>;

  /// fit policy for yz fits
  template <unsigned maxIter>
  using YFitPolicy = CommonFitPolicy<double, PatSeedTrack, PatFwdHit*, 2, maxIter>;

  /// fit object (yz fits)
  template <bool forceRLAmb, unsigned maxIter,
	   bool xOnly = false, bool stereoOnly = false,
	   bool allowUpdateReject = true>
  using YFit = SIMDChi2Fit<
    MeasurementProvider<YFitPolicy<maxIter>, forceRLAmb, xOnly, stereoOnly>,
    YMeasurementProjector<YFitPolicy<maxIter> >,
    YTrackUpdater<YFitPolicy<maxIter>, allowUpdateReject>,
    NullHitUpdater<YFitPolicy<maxIter> >, 16>;

  /// fit policy for simultaneous xz/yz fits
  template <unsigned maxIter>
  using XYFitPolicy = CommonFitPolicy<double, PatSeedTrack, PatFwdHit*, 5, maxIter>;

  /// fit object (simultaneous xz/yz fits)
  template <bool forceRLAmb, unsigned maxIter,
	   bool allowUpdateReject = true>
  using XYFit = SIMDChi2Fit<
    MeasurementProvider<XYFitPolicy<maxIter>, forceRLAmb>,
    XYMeasurementProjector<XYFitPolicy<maxIter> >,
    XYTrackUpdater<XYFitPolicy<maxIter>, allowUpdateReject>,
    NullHitUpdater<XYFitPolicy<maxIter> >, 32>;

  /// fit policy for stub fits
  template <unsigned maxIter>
  using StubFitPolicy = CommonFitPolicy<double, PatSeedTrack, PatFwdHit*, 3, maxIter>;

  /// fit object for stub fits
  template <bool forceRLAmb, unsigned maxIter,
	   bool allowUpdateReject = true>
  using StubFit = SIMDChi2Fit<
    MeasurementProvider<StubFitPolicy<maxIter>, forceRLAmb>,
    StubMeasurementProjector<StubFitPolicy<maxIter> >,
    StubTrackUpdater<StubFitPolicy<maxIter>, allowUpdateReject>,
    NullHitUpdater<StubFitPolicy<maxIter> >, 8>;
} // namespace PatSeedFits

#endif // PATSEEDFITS_H

// vim: ft=cpp:sw=4:tw=78
