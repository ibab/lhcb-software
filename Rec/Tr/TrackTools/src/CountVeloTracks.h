#ifndef TRACKTOOLS_CountVeloTracks_H
#define TRACKTOOLS_CoutnVeloTracks_H

//-----------------------------------------------------------------------------
/** @class CountVeloTracks CountVeloTracks.h
 *
 *  Tool for counting the distinct VELO tracks in an event
 *
 *  Based on code written by M Needham
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *
 *  @date   21/1/2011
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ICountContainedObjects.h"
#include "Event/Track.h"

class CountVeloTracks : public GaudiTool,
  virtual public ICountContainedObjects {

public:

  /// constructer
  CountVeloTracks( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// empty destructor
  virtual ~CountVeloTracks() {};

  /** Returns number of distinct VELO tracks contributing to the container
  *
  *  @param tracks reference to LHCb::Tracks container
  *
  *  @return number of distinct VELO tracks
  */
  virtual unsigned int nObj ( const ObjectContainerBase * cont ) const;
                                                                           
 private:

  /// get a vector of LHCbID for VELO only from a track pointer
  inline void getVeloIDs(std::vector<LHCb::LHCbID>& veloHits,
			 const LHCb::Track* track) const {
    const std::vector<LHCb::LHCbID>& vids = track->lhcbIDs();
    veloHits.reserve(vids.size());
    std::copy_if(vids.begin(), vids.end(), std::back_inserter(veloHits), 
                [](const LHCb::LHCbID& id) { return id.isVelo(); } );
  }

};

#endif // TRACKTOOLS_VeloExpectation_H
