/**
 * @file TaggingHelpers.cpp
 *
 * various tagging helper routines
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-01-08
 */
#include <cmath>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>

#include "Kernel/LHCbID.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

#include "TaggingHelpers.h"

namespace TaggingHelpers
{

  double dphi(const double phi1, const double phi2)
  {
    const double c1 = std::cos(phi1), s1 = std::sin(phi1);
    const double c2 = std::cos(phi2), s2 = std::sin(phi2);
    return std::atan2(c1 * s2 - c2 * s1, c1 * c2 + s1 * s2);
  }

  double dphi(const double x1, const double y1,
              const double x2, const double y2)
  {
    const double r1 = std::sqrt(x1 * x1 + y1 * y1);
    const double r2 = std::sqrt(x2 * x2 + y2 * y2);
    // at least one of the vectors has zero length, return NaN
    if (0. == r1 || 0. == r2)
      return std::numeric_limits<double>::quiet_NaN();
    const double c1 = x1 / r1, s1 = y1 / r1;
    const double c2 = x2 / r2, s2 = y2 / r2;
    return std::atan2(c1 * s2 - c2 * s1, c1 * c2 + s1 * s2);
  }

  const std::string& toString(const SameTrackStatus status)
  {
    static const std::string statenames[1 + nStates] = {
      "different particles",
      "ConvertedGamma",
      "MattTxTyQp",
      "MattTxTy",
      "clone (track param.)",
      "clone (hit cont.)",
      "same track",
      "same protoparticle",
      "same particle",
      "unknown"
    };
    return statenames[ (status > nStates) ? nStates : status ];
  }

  SameTrackStatus isSameTrack( const LHCb::Particle& p1, const LHCb::Particle& p2,
                               const double dEta,   const double dPhi,
                               const double sigmaP, const double dppErr,
                               const double minSharedHitFraction,
                               const double cosConvertedGammaAngle )
  {
    // compare if two particles are (essentially) the same underlying track
    if (&p1 == &p2) return SameParticle;
    if (p1.proto() && p2.proto()) {
      if (p1.proto() == p2.proto()) return SameProtoParticle;
      if (p1.proto()->track() && p2.proto()->track()) {
        if (p1.proto()->track() == p2.proto()->track()) return SameTrack;
        if (0. <= minSharedHitFraction &&
            minSharedHitFraction <= 1.0) {
          // ok, easy comparisons fail, so compare tracks based on
          // hit contents
          typedef std::vector<LHCb::LHCbID> LHCbIDs;
#undef SLOPPYHITCOMPARISONS
#ifndef SLOPPYHITCOMPARISONS
          LHCbIDs h1(p1.proto()->track()->lhcbIDs());
          LHCbIDs h2(p2.proto()->track()->lhcbIDs());
          // be paranoid: sort hits by LHCbID (old versions of the
          // Track class did not do this); for the new version of
          // the class, this should take and negligible amount of
          // time, since it's already sorted, and the vectors do not
          // have too many elements
          //
          // the reason for this gymnastics with sorted hit lists is
          // that we can then compare hit content in O(n log(n))
          // time instead of O(n^2) time
          std::sort(h1.begin(), h1.end());
          std::sort(h2.begin(), h2.end());
#else
          // faster but not safe, see above
          const LHCbIDs& h1(p1.proto()->track()->lhcbIDs());
          const LHCbIDs& h2(p2.proto()->track()->lhcbIDs());
#endif
          // go through the containers and count (common, total)
          // hits in entire track and in Velo and T segments
          const unsigned n1 = h1.size(), n2 = h2.size();
          unsigned nVelo1 = 0, nVelo2 = 0, nT1 = 0, nT2 = 0;
          unsigned nCommon = 0, nVeloCommon = 0, nTCommon = 0;
          LHCbIDs::const_iterator it = h1.begin(), jt = h2.begin();
          // FIXME: upgrade gurus: which is the equivalent of the T
          //        stations for the upgrade clone detection?
          while (h1.end() != it && h2.end() != jt) {
            if (*it < *jt) {
              if (it->isVelo() || it->isVP()) ++nVelo1;
              else if (it->isIT() || it->isOT()) ++nT1;
              ++it;
            } else if (*jt < *it) {
              if (jt->isVelo() || jt->isVP()) ++nVelo2;
              else if (jt->isIT() || jt->isOT()) ++nT2;
              ++jt;
            } else {
              ++nCommon;
              // common LHCb ID
              if (it->isVelo() || it->isVP()) {
                ++nVelo1;
                ++nVelo2;
                ++nVeloCommon;
              } else if (it->isIT() || it->isOT()) {
                ++nT1;
                ++nT2;
                ++nTCommon;
              }
              ++it;
              ++jt;
            }
          }
          // consume any hits potentially remaining in track 1
          while (h1.end() != it) {
            if (it->isVelo() || it->isVP()) ++nVelo1;
            else if (it->isIT() || it->isOT()) ++nT1;
            ++it;
          }
          // consume any hits potentially remaining in track 2
          while (h2.end() != jt) {
            if (jt->isVelo() || jt->isVP()) ++nVelo2;
            else if (jt->isIT() || jt->isOT()) ++nT2;
            ++jt;
          }
          // calculate with respect to the shorter of the two tracks
          const unsigned n = std::min(n1, n2);
          const unsigned nVelo = std::min(nVelo1, nVelo2);
          const unsigned nT = std::min(nT1, nT2);
          // calculate common hit fraction
          const double f = double(nCommon) / double(n);
          const double fVelo = nVelo ?
            (double(nVeloCommon) / double(nVelo)) : 1.;
          const double fT = nT ? (double(nTCommon) / double(nT)) : 1.;
          // if we are above threshold, flag tracks as clones based on
          // hit content
          if (f > minSharedHitFraction &&
              fVelo > minSharedHitFraction &&
              fT > minSharedHitFraction)
            return CloneHitContent;
        }
      }
    }

    // check if the particles are clones based on (track) parameters
    const Gaudi::LorentzVector& P1 = p1.momentum();
    const Gaudi::LorentzVector& P2 = p2.momentum();

    const double deta = P1.eta() - P2.eta();
    const double dphi =
      TaggingHelpers::dphi(P1.x(), P1.y(), P2.x(), P2.y());

    double p1err, p2err;
    if (dppErr >= 0.) {
      p1err = dppErr * P1.P();
      p2err = dppErr * P2.P();
    } else {
      // figure out the error on p from the covariance matrix and the
      // components of the momentum vector
      const Gaudi::SymMatrix4x4& C1 = p1.momCovMatrix();
      const Gaudi::SymMatrix4x4& C2 = p2.momCovMatrix();
      p1err =
        std::sqrt((P1.x() * P1.x() * C1(0,0) + P1.y() * P1.y() * C1(1,1) +
                   P1.z() * P1.z() * C1(2,2)) +
                  2. * (P1.x() * P1.y() * C1(0,1) + P1.x() * P1.z() * C1(0,2) +
                        P1.y() * P1.z() * C1(1,2))) / P1.P();
      p2err =
        std::sqrt((P2.x() * P2.x() * C2(0,0) + P2.y() * P2.y() * C2(1,1) +
                   P2.z() * P2.z() * C2(2,2)) +
                  2. * (P2.x() * P2.y() * C2(0,1) + P2.x() * P2.z() * C2(0,2) +
                        P2.y() * P2.z() * C2(1,2))) / P2.P();
    }

    // finally, perform the check if the two particles are clones based on
    // track parameters
    if (p1.charge() && p2.charge()) {
      const double dqp = double(p1.charge()) / P1.P() - double(p2.charge()) / P2.P();
      const double dqperr = std::sqrt((p1err / P1.P2()) * (p1err / P1.P2()) +
                                      (p2err / P2.P2()) * (p2err / P2.P2()));
      if (std::abs(dqp/dqperr) < sigmaP && std::abs(deta) < dEta &&
          std::abs(dphi) < dPhi)
        return CloneTrackParams;
    } else {
      const double dp = P1.P() - P2.P();
      const double dperr = std::sqrt(p1err * p1err + p2err * p2err);
      if (std::abs(dp/dperr) < sigmaP && std::abs(deta) < dEta &&
          std::abs(dphi) < dPhi)
        return CloneTrackParams;
    }

    const double dtx = P1.x() / P1.z() - P2.x() / P2.z();
    const double dty = P1.y() / P1.z() - P2.y() / P2.z();
    if (std::abs(dtx) < 0.0004 && std::abs(dty) < 0.0002) return MattTxTy;
    if (p1.charge() && p2.charge()) {
      const double dqp =
        double(p1.charge()) / P1.P() - double(p2.charge()) / P2.P();
      if (std::abs(dtx) < 0.005 && std::abs(dty) < 0.005 &&
          std::abs(dqp) < 1e-6)
        return MattTxTyQp;
    }

    if (p1.charge() * p2.charge() < 0) {
      const double mmu = 105.6583715, me = 0.510998928;
      Gaudi::LorentzVector P1 = p1.momentum();
      Gaudi::LorentzVector P2 = p2.momentum();
      P1.SetE(std::sqrt(me * me + P1.P2()));
      P2.SetE(std::sqrt(me * me + P2.P2()));
      // we use 0.25 * mmu as cutoff for low mass - halfway between 0
      // and half a muon mass seems fair when you keep mass resolution
      // and numerical precision in mind...
      const bool isLowMassSq =
        (std::abs((P1 + P2).M2()) < .0625 * mmu * mmu);
      P1.SetE(std::sqrt(mmu * mmu + P1.P2()));
      P2.SetE(std::sqrt(mmu * mmu + P2.P2()));
      if (isLowMassSq ||
          (std::abs((P1 + P2).M2()) < .0625 * mmu * mmu)) {
        // the system has a small invariant mass - typically, the two
        // tracks will also have a small angle (after all, it's a
        // gamma on a fixed target... ;), so cut on the cosine of that
        // angle
        const double p1p2 =
          P1.x() * P2.x() + P1.y() * P2.y() + P1.z() * P2.z();
        if (p1p2 / P1.P() / P2.P() > cosConvertedGammaAngle)
          return ConvertedGamma;
      }
    }

    // as far as we can tell, the two particles are different
    return DifferentParticles;
  }
}

// vim: tw=78:sw=4:ft=cpp
