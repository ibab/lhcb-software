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
#include "TrackInterfaces/ICountVeloTracks.h"
#include "LoKi/select.h"
#include "boost/bind.hpp"

class CountVeloTracks : public GaudiTool,
  virtual public ICountVeloTracks {

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
  virtual unsigned int nVeloTracks ( const LHCb::Tracks& tracks ) const;
                                                                           
  /** Returns number of distinct VELO tracks contributing to the container
  *
  *  @param tracksLocation TES location of the Tracks container
  *
  *  @return number of distinct VELO tracks
  */
  virtual unsigned int nVeloTracks ( const std::string& tracksLocation ) const;

 private:

  /// get a vector of LHCbID for VELO only from a track pointer
  inline void getVeloIDs(std::vector<LHCb::LHCbID>& veloHits,
			 const LHCb::Track* track) const {
    const std::vector<LHCb::LHCbID>& vids = track->lhcbIDs();
    veloHits.reserve(vids.size());
    LoKi::select(vids.begin(), vids.end(), std::back_inserter(veloHits), 
		 boost::bind(&LHCb::LHCbID::isVelo,_1));
  }

};

#endif // TRACKTOOLS_VeloExpectation_H
