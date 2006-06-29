
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "CleanPatTrackSelector.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"


using namespace LHCb;

DECLARE_TOOL_FACTORY( CleanPatTrackSelector );

CleanPatTrackSelector::CleanPatTrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent )
{

  // interface
  declareInterface<ITrackSelector>(this);
  declareProperty("rejectTracksWithOutliers", m_rejectTracksWithOutliers = false);
}

CleanPatTrackSelector::~CleanPatTrackSelector() { }

bool CleanPatTrackSelector::accept ( const Track& aTrack ) const
{
  return cleanTrack(aTrack);
}


bool CleanPatTrackSelector::cleanTrack(const Track& aTrack) const {

  // get the track
  bool clean = true;
  const Track* pTrack =  &aTrack;

  // which container is it ?
  const ObjectContainerBase* parentCont = pTrack->parent();
  const IRegistry* reg =  parentCont->registry();
  const std::string& idName = reg->identifier();
  std::string temp = idName.substr(idName.find("Rec"));

  Asct linker = Asct( evtSvc(), TrackLocation::Forward );
  const Table* table = linker.direct();

  if (table != 0){
    Range range = table->relations( pTrack );
    if ( range.empty() == true) {
      clean = false;
    }
    else if (m_rejectTracksWithOutliers == true){
      double weight = (*range.begin()).weight();    
      if (weight < 0.99) clean = false; 
    } 
    else {
      //
    }

  }
  
  return clean;
}
