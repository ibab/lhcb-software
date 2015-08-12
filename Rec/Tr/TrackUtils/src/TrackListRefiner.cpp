
/** @class TrackListRefiner TrackListRefiner.h
 *
 *  Make a subselection of a track list
 *
 *  @author Wouter Hulsbergen
 *  @date   05/01/2010
 */

#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include <string>
#include "Event/Track.h"


class TrackListRefiner: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackListRefiner(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackListRefiner();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode execute();

private:
  std::string m_inputLocation;
  std::string m_outputLocation;
  ToolHandle<ITrackSelector> m_selector;
};


DECLARE_ALGORITHM_FACTORY( TrackListRefiner )

TrackListRefiner::TrackListRefiner(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_selector("",this)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation ) ;
  declareProperty( "outputLocation", m_outputLocation ) ;
  declareProperty( "Selector", m_selector );
}

TrackListRefiner::~TrackListRefiner()
{
  // destructor
}

StatusCode TrackListRefiner::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }
  
  // retrieve the selector if it is set
  if ( !m_selector.empty() ) {
    sc = m_selector.retrieve() ;
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg ;
  }
  
  return sc ;
}

StatusCode TrackListRefiner::finalize()
{
  if ( !m_selector.empty() ) m_selector.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

StatusCode TrackListRefiner::execute()
{
  // Do not execute if the TES location does not exist.
  if( !exist<DataObject>(m_inputLocation) ) {
    if( UNLIKELY( msgLevel(MSG::ERROR) ) ) error()<<"Location "<<m_inputLocation<<" does not exist!"<<endmsg;
    return StatusCode::SUCCESS;
  }
  LHCb::Track::Range tracksin  = get<LHCb::Track::Range>(m_inputLocation) ;
  LHCb::Track::Selection* tracksout = new LHCb::Track::Selection() ;
  put( tracksout, m_outputLocation) ;
  
  // loop 
  counter("#seeds")+=tracksin.size();
  for (LHCb::Track::Range::const_iterator itrack = tracksin.begin(); 
       itrack != tracksin.end(); ++itrack)  {
    bool accept = true ;
    if ( !m_selector.empty() ) 
      accept = m_selector->accept( **itrack ) ;
    if( accept) tracksout->insert( *itrack ) ;
  }
  counter("#passed") += tracksout->size();
  return StatusCode::SUCCESS;
}
