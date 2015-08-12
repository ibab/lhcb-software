
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/ToolHandle.h"


// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDAQ/IOTRawBankDecoder.h"


#include "Event/Track.h"
#include "Event/ODIN.h"
#include "Event/VeloCluster.h"
#include "Event/STLiteCluster.h"

using namespace LHCb;
using namespace Gaudi;
 
/// BOOST
#include "boost/lexical_cast.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/foreach.hpp"

/** @class TrackTimingMonitor TrackTimingMonitor.h  
 * 
 * Class for track timing monitoring
 *  @author Stephanie Hansmann-Menzemer
 *  @date   14-05-2010
 */                 
                                                           
class TrackTimingMonitor : public GaudiTupleAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackTimingMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackTimingMonitor();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

private:
  ISequencerTimerTool* m_timerTool; 
  DeOTDetector* m_tracker;
  ToolHandle<IOTRawBankDecoder> m_decoder;

  
};



DECLARE_ALGORITHM_FACTORY( TrackTimingMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackTimingMonitor::TrackTimingMonitor(const std::string& name,
				       ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_timerTool(0),
  m_tracker(0),
  m_decoder("OTRawBankDecoder")  
{
  
}

//=============================================================================
// Destructor
//=============================================================================
TrackTimingMonitor::~TrackTimingMonitor() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackTimingMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();

  if ( sc.isFailure() ) { return sc; }


    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    
    // Get OT Geometry from XML
    m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

   
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackTimingMonitor::execute()
{

  int pf  = m_timerTool->indexByName("Internal PatForward");
  int ps  = m_timerTool->indexByName("Internal PatSeeding");
  int ts  = m_timerTool->indexByName("Internal TsaSeeding");
  int pd  = m_timerTool->indexByName("Internal PatDownstream");
  int vRZ = m_timerTool->indexByName("Internal VeloRZ");
  int v3D = m_timerTool->indexByName("Internal VeloSpaceTracking"); 
  int vg  = m_timerTool->indexByName("Internal PatVeloGeneral");
  int vTT = m_timerTool->indexByName("Internal VeloTT Tracking");
  int tm  = m_timerTool->indexByName("Internal TrackMatching");

  Tuple theTuple = nTuple( "Timing" , "" , CLID_ColumnWiseTuple );

  if (pf != -1)
    theTuple->column( "TimePatForward", m_timerTool->lastTime(pf));
  if (ps !=-1)
    theTuple->column( "TimePatSeeding", m_timerTool->lastTime(ps));
  if (pd != -1)
    theTuple->column( "TimePatDownstream", m_timerTool->lastTime(pd));
  if (ts !=-1)
    theTuple->column( "TimingTsaSeeding", m_timerTool->lastTime(ts));
  if (vRZ != -1)
    theTuple->column( "TimeVeloRZ", m_timerTool->lastTime(vRZ));
  if (v3D !=-1)
    theTuple->column( "TimeVeloSpace", m_timerTool->lastTime(v3D));
  if (vg != -1)
    theTuple->column( "TimeVeloGeneral", m_timerTool->lastTime(vg));
  if (vTT !=-1)
    theTuple->column( "TimingVeloTT", m_timerTool->lastTime(vTT));
  if (tm !=-1)
    theTuple->column( "TimingTrackMatching", m_timerTool->lastTime(tm));

  LHCb::Tracks* tracks = NULL;
  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::RZVelo );
  if ( NULL != tracks ) {
    theTuple->column( "NbVeloRZTracks", tracks->size());
  }

  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Velo );
  if ( NULL != tracks ) {
    theTuple->column( "NbVelo3DTracks", tracks->size());
  }

  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Forward );
  if ( NULL != tracks ) {
    theTuple->column( "NbForwardTracks", tracks->size());
  }

  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Seed );
  if ( NULL != tracks ) {
    theTuple->column( "NbSeedTracks", tracks->size());
  }
  
  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Downstream );
  if ( NULL != tracks ) {
    theTuple->column( "NbDownstreamTracks", tracks->size());
  }

  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::VeloTT );
  if ( NULL != tracks ) {
    theTuple->column( "NbVeloTTTracks", tracks->size());
  } 

  tracks = getIfExists<LHCb::Tracks>( LHCb::TrackLocation::Match );
  if ( NULL != tracks ) {
    theTuple->column( "NbMatchTracks", tracks->size());
  }

  std::vector<double> OTOcc;
  int maxOTModules = m_tracker->modules().size();

  int allOTHits = 0;

  // loop over all modules
  BOOST_FOREACH(const DeOTModule* module, m_tracker->modules()) {
    LHCb::OTChannelID modid = module->elementID();
    LHCb::OTLiteTimeRange ottimes = m_decoder->decodeModule(modid);
    OTOcc.push_back(1.0*ottimes.size()/module->nChannels());
    allOTHits += ottimes.size();
  }

  theTuple->column("NbOTHits", allOTHits);
  theTuple->farray( "OTOcc"     , OTOcc      , "nOTmodules" , maxOTModules );  

  const LHCb::STLiteCluster::STLiteClusters* ITClusterCont = 
    get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters);
  theTuple->column("NITClusters", ITClusterCont->size());
  
  const LHCb::STLiteCluster::STLiteClusters* TTClusterCont = 
    get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::TTClusters);
  theTuple->column("NTTClusters", TTClusterCont->size());

  LHCb::VeloClusters* m_clusters = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default );
  
  theTuple->column("NVeloClusters", m_clusters->size());
  
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );  
  if( NULL != odin ) { 
    theTuple->column("RunNb", odin->runNumber());

    unsigned long hi_num =  (unsigned long) (odin->eventNumber() >> 32);
    unsigned long low_num = (unsigned long) (odin->eventNumber() & 0xffffffff);
 
    theTuple->column("EventNb_hi", hi_num);
    theTuple->column("EventNb_low", low_num);
  }

  theTuple->write();

  return StatusCode::SUCCESS;

}
