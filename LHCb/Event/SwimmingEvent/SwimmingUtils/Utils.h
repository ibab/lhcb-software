// $Id: $
#ifndef SWIMMINGEVENT_UTILS_H 
#define SWIMMINGEVENT_UTILS_H 1

// STD & STL
#include <vector>
#include <algorithm>

// Gaudi
#include <GaudiKernel/Hash.h>
#include <GaudiKernel/GaudiException.h>

// Event
#include <Event/ProtoParticle.h>
#include <Event/Particle.h>
#include <Event/Track.h>

namespace {
   GaudiUtils::Hash<std::vector<unsigned int> > _hash;
}

namespace GaudiUtils {
   
/// Particle Hash function
template <> struct Hash<const LHCb::Particle&>
{ 
   size_t operator() (const LHCb::Particle& p) const
   {
      const LHCb::ProtoParticle* pp = p.proto();
      if (!pp) {
         throw GaudiException("Need access to proto particle to compute hash",
                              "Hash<const LHCb::Particle&>", StatusCode::FAILURE);
      }
      const LHCb::Track* track = pp->track();
      if (!track) {
         throw GaudiException("Need access to track to compute hash",
                              "Hash<const LHCb::Particle&>", StatusCode::FAILURE);
      }
      const std::vector<LHCb::LHCbID>& lhcbIDs = track->lhcbIDs();
      std::vector<unsigned int> ids;
      ids.reserve(lhcbIDs.size() + 1);
      // Add the pid so that different Particles from the same ProtoParticle have different hashes.
      ids.push_back(p.particleID().pid());
      // Add lhcbIDs
      std::transform(lhcbIDs.begin(), lhcbIDs.end(), std::back_inserter(ids),
                     std::mem_fun_ref(&LHCb::LHCbID::lhcbID));
      return _hash(ids);
   }
};

}

namespace {
   GaudiUtils::Hash<const LHCb::Particle&> _hashParticle;
}

namespace Swimming {
   inline size_t hashParticle(const LHCb::Particle& p) {
      return _hashParticle(p);
   }
}

#endif // SWIMMINGEVENT_UTILS_H
