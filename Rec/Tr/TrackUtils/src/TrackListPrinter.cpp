// track interfaces
#include "Event/Track.h"

#include "TrackListPrinter.h"

DECLARE_ALGORITHM_FACTORY( TrackListPrinter )

TrackListPrinter::TrackListPrinter(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "InputLocation",  m_inputLocation  = LHCb::TrackLocation::Default );
}

TrackListPrinter::~TrackListPrinter()
{
  // destructor
}


StatusCode TrackListPrinter::execute()
{
  LHCb::Track::Range tracks  = get<LHCb::Track::Range>(m_inputLocation) ;
  for (LHCb::Track::Range::const_iterator iterT = tracks.begin(); iterT != tracks.end(); ++iterT) 
    info() << **iterT << endmsg ;
  
  return StatusCode::SUCCESS;
}

