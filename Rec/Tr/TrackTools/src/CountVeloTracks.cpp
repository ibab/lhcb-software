//-----------------------------------------------------------------------------
/** @file CountVeloTracks.cpp
 *
 *  Tool for counting the distinct VELO tracks in an event
 *
 *  Based on code by Matt Needham
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *  @date   21/01/2011
 */
//-----------------------------------------------------------------------------

// local
#include "CountVeloTracks.h"

DECLARE_TOOL_FACTORY( CountVeloTracks )

//-----------------------------------------------------------------------------

CountVeloTracks::CountVeloTracks( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent ) {

  // interface
  declareInterface<ICountContainedObjects>(this);
}


unsigned int CountVeloTracks::nObj( const ObjectContainerBase * cont ) const{

  const LHCb::Tracks * tracks = dynamic_cast<const LHCb::Tracks*>(cont);
  if(!tracks){
    error() << "Input is not an LHCb::Tracks container" <<endmsg;
    return 0;
  }

  // to save some run on lines later define a type for a container
  // first is the first VELO LHCbID on the track, second a track pointer
  typedef std::multimap<LHCb::LHCbID,LHCb::Track*> LocalMM;
  // somewhere to put tracks with VELO parts
  LocalMM localCont;

  for (LHCb::Tracks::const_iterator iterT = tracks->begin();
       iterT != tracks->end(); ++iterT ){
    if( !(*iterT)->hasVelo() ) continue; //skip tracks without VELO components

    std::vector<LHCb::LHCbID> cvids;
    getVeloIDs(cvids,*iterT);
    // keep track if not a match to an existing track
    bool keep = true;
    std::pair<LocalMM::iterator,LocalMM::iterator> posRange = 
      localCont.equal_range(cvids.front());
    // loop over tracks with same first velo hit (if any)
    for( LocalMM::iterator localIT= posRange.first ; 
	 localIT != posRange.second; ++localIT ) {
      std::vector<LHCb::LHCbID> tvids;
      getVeloIDs(tvids,localIT->second);
      if (std::equal(cvids.begin(), cvids.end(), tvids.begin())){
        keep = false; // this is a cloned VELO track
        break;
      }
    }
    if(keep){ // add this track      
      localCont.insert(std::pair<LHCb::LHCbID,LHCb::Track*>(cvids.front(),*iterT));
    }
  }
  return localCont.size();
}

