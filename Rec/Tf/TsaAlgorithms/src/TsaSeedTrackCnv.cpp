//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

#include "TsaSeedTrackCnv.h"

using namespace Tf::Tsa;

DECLARE_ALGORITHM_FACTORY( SeedTrackCnv )

SeedTrackCnv::SeedTrackCnv(const std::string& name,
                           ISvcLocator* pSvcLocator):
  BaseAlg(name, pSvcLocator),
  m_Tsa2TrackCnv(NULL)
{
  // constructor
  declareProperty("LikCut", m_likCut = -40.);
  declareProperty("inputLocation", m_inputLocation = SeedTrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = LHCb::TrackLocation::Tsa );
}


StatusCode SeedTrackCnv::initialize()
{
  // Initialization

  const StatusCode sc = BaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }

  m_Tsa2TrackCnv = tool<ITsaSeedTrackCnvTool>("Tf::Tsa::SeedTrackCnvTool");

  return sc;
}

SeedTrackCnv::~SeedTrackCnv() { }

StatusCode SeedTrackCnv::execute()
{

  // get the tracks
  SeedTracks* trackCont = get<SeedTracks>(m_inputLocation);
  // debug() << "Found " << trackCont->size() << " SeedTracks at " << m_inputLocation << endreq;

  // output container
  LHCb::Tracks* output = new LHCb::Tracks();
  output->reserve(trackCont->size());
  put(output,m_outputLocation);

  for (SeedTracks::const_iterator iterTrack = trackCont->begin();
       iterTrack != trackCont->end(); ++iterTrack)
  {
  
    if ( ((*iterTrack)->select() != 0) && ((*iterTrack)->lik() > m_likCut) )
    {
      //LHCb::Track* fitTrack = this->convert(*iterTrack);
      LHCb::Track* fitTrack = m_Tsa2TrackCnv->convert(*iterTrack);
      output->insert(fitTrack, (*iterTrack)->key());
    }
  }

  //  debug() << "Created " << output->size() << " converted SeedTracks at " << m_outputLocation << endreq;

  return StatusCode::SUCCESS;
}

