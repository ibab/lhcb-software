// Include files 

#include "Event/STLiteCluster.h"
#include "Event/RawEvent.h"
#include "Kernel/STTell1Board.h"
// local
#include "FastSTDecoding.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastSTDecoding
//
// 2012-10-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastSTDecoding )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FastSTDecoding::FastSTDecoding( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
    , m_stDet( NULL )
    , m_readoutTool( NULL )
    , m_bankType( LHCb::RawBank::IT )
    , m_compareLocation("")
{
  declareProperty( "DetectorName",      m_detectorName    = "IT"   );
  declareProperty( "OutputLocation",    m_outputLocation  = LHCb::STLiteClusterLocation::ITClusters );
  declareProperty( "ErrorCount",        m_errorCount      = 0      );
  declareProperty( "IgnoreErrors",      m_ignoreErrors    = false  );
  declareProperty( "CompareResult",     m_compareResult   = false  );
}
//=============================================================================
// Destructor
//=============================================================================
FastSTDecoding::~FastSTDecoding() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastSTDecoding::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if ( "TT" == m_detectorName ) {
    m_bankType       = LHCb::RawBank::TT;
    m_outputLocation = LHCb::STLiteClusterLocation::TTClusters;
  } else if ( "IT" == m_detectorName ) {
    m_bankType       = LHCb::RawBank::IT;
    m_outputLocation = LHCb::STLiteClusterLocation::ITClusters;
  }
  if ( m_compareResult ) {
    m_compareLocation = m_outputLocation;
    m_outputLocation  = m_outputLocation + "Test";
  }
  
  m_stDet = getDet<DeSTDetector>( DeSTDetLocation::location(m_detectorName) );

  std::string readoutToolName = m_detectorName + "ReadoutTool";
  m_readoutTool = tool<ISTReadoutTool>( readoutToolName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastSTDecoding::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default );
  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks( m_bankType );

  LHCb::STLiteCluster::FastContainer* fastCont = new LHCb::STLiteCluster::FastContainer();
  put( fastCont, m_outputLocation );
  
  debug() << "Number of ST banks : " << banks.size() << endmsg;

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); bi != banks.end(); ++bi) {
    const LHCb::RawBank* rb = *bi;

    //== protect against corrupted banks
    if( LHCb::RawBank::MagicPattern != rb->magic() ) {
      failEvent(format("Bad magic pattern in bank source ID %i", rb->sourceID()),
                "CorruptSTBuffer",CorruptSTBuffer,false);
      continue;
    }
    if ( 3 != rb->version() ) {
      failEvent( format( "Unsupported bank version %2d", rb->version() ), 
                "CorruptSTBuffer",CorruptSTBuffer,false);
      continue;
    }
    STDAQ::version bankVersion = STDAQ::v4;

   // get the board and data
    STTell1Board* aBoard = m_readoutTool->findByBoardID( STTell1ID( rb->sourceID() ) );
    if (!aBoard ){
      failEvent( format( "Invalid source ID %4d ->skip bank", rb->sourceID() ), 
                 "CorruptSTBuffer", CorruptSTBuffer, false );
      continue;
    }    

    const unsigned int* data = rb->data();

    unsigned int nClus = (*data) & 0xFFFF;
    unsigned int error = ( (*data) & 0x1000000 ) >> 24;
    if ( !m_ignoreErrors && 0 != error ) continue;
    debug() << "Process source id " << rb->sourceID()
            << " data " << (*data) << " nData " << nClus << endmsg;

    unsigned short* word = (unsigned short*) (data+1);
    for (  ; 0 < nClus; nClus--, ++word ) {
      unsigned int frac    = (*word) & 0x3;
      unsigned int channel = ( (*word) & 0x3FFC ) >> 2;
      unsigned int size    = ( (*word) & 0x4000 ) >> 14;
      bool high            = ( ( (*word) & 0x8000 ) >> 15) != 0;

      const std::pair<LHCb::STChannelID,int> chan = aBoard->DAQToOffline( frac, bankVersion, 
                                                                          STDAQ::StripRepresentation( channel ));
      LHCb::STLiteCluster liteCluster( chan.second, size, high, chan.first );
      fastCont->push_back( liteCluster );
    }
  }

  std::sort( fastCont->begin(), fastCont->end(), SiDataFunctor::Less_by_Channel< LHCb::STLiteCluster >() );

  //== Test if the result is equal to another decoding in the default location

  if ( m_compareResult ) {
    LHCb::STLiteCluster::FastContainer* old;
    old = get<LHCb::STLiteCluster::FastContainer>( m_compareLocation );

    if ( old->size() != fastCont->size() ) {
      info() << "=== Wrong size: old " << old->size() << " new " << fastCont->size() << endmsg;
      return StatusCode::FAILURE;
    } else {
      LHCb::STLiteCluster::FastContainer::iterator itOld = old->begin();
      LHCb::STLiteCluster::FastContainer::iterator itNew = fastCont->begin();
      for ( ;old->end() != itOld; ++itOld, ++itNew ) {
        if ( (*itOld).channelID()          != (*itNew).channelID()          ||
             (*itOld).pseudoSize()         != (*itNew).pseudoSize()         ||
             (*itOld).interStripFraction() != (*itNew).interStripFraction() ||
             (*itOld).highThreshold()      != (*itNew).highThreshold()        ) {
          info() << "Difference at word " << itOld - old->begin()
                 << " old CH " << (*itOld).channelID() << " new CH " << (*itNew).channelID()
                 << " old size " << (*itOld).pseudoSize() << " new size " << (*itNew).pseudoSize()
                 << " old Frac " << (*itOld).interStripFraction() << " new frac " << (*itNew).interStripFraction()
                 << " old HT " << (*itOld).highThreshold() << " new HT " << (*itNew).highThreshold()
                 << endmsg;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastSTDecoding::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Print and store failure information
//=========================================================================
void FastSTDecoding::failEvent( const std::string &ErrorText,
                                  const std::string &ProcText,
                                  AlgStatusType status,
                                  bool procAborted){
  // set ProcStat for this event to failed in DecodeSTRawBuffer
  LHCb::ProcStatus *pStat = getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);
  pStat->addAlgorithmStatus("FastSTDecoding", "ST", ProcText, status, procAborted);
  if( m_errorCount > 0 || msgLevel(MSG::DEBUG) ) {
    unsigned int msgCount = m_errorCount;
    if ( msgLevel(MSG::DEBUG) ) msgCount += 10;
    Error( ErrorText, StatusCode::SUCCESS, msgCount ).ignore();
  }
}
//=============================================================================
