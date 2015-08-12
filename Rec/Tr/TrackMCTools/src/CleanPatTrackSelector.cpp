
#include "GaudiKernel/SystemOfUnits.h"

#include "CleanPatTrackSelector.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"

#include "MCInterfaces/IMCParticleSelector.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( CleanPatTrackSelector )

CleanPatTrackSelector::CleanPatTrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent )
{

  // interface
  declareInterface<ITrackSelector>(this);
  declareProperty( "RejectTracksWithOutliers",
                   m_rejectTracksWithOutliers = false );

}


CleanPatTrackSelector::~CleanPatTrackSelector() { }

StatusCode CleanPatTrackSelector::initialize() {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  m_selector = tool<IMCParticleSelector>( "MCParticleSelector", "Selector", this );
  return StatusCode::SUCCESS;
}

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
 
  Asct linker = Asct( evtSvc(),idName );
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
      // use the selector
      const MCParticle* mcpart = (*range.begin()).to();
      clean = m_selector->accept(mcpart);
    }
  }
  else {
    warning() << " Failed to find linker table " << endmsg; 
  }  
  return clean;
}
