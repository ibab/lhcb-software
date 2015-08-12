// Include files 

#include "Event/VeloLiteCluster.h"
#include "Event/RawEvent.h"
// local
#include "FastVeloDecoding.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloDecoding
//
// 2012-10-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastVeloDecoding )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FastVeloDecoding::FastVeloDecoding( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
    , m_velo(NULL)
{
  declareProperty( "OutputLocation",    m_outputLocation  = LHCb::VeloLiteClusterLocation::Default );
  declareProperty( "MaxVeloClusters",   m_maxVeloClusters = 10000  );
  declareProperty( "ErrorCount",        m_errorCount      = 0      );
  declareProperty( "IgnoreErrors",      m_ignoreErrors    = false  );
  declareProperty( "CompareResult",     m_compareResult   = false  );
}
//=============================================================================
// Destructor
//=============================================================================
FastVeloDecoding::~FastVeloDecoding() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastVeloDecoding::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastVeloDecoding::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default );
  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks( LHCb::RawBank::Velo );

  LHCb::VeloLiteCluster::FastContainer* fastCont = new LHCb::VeloLiteCluster::FastContainer();
  put( fastCont, m_outputLocation);

  debug() << "Number of Velo banks : " << banks.size() << endmsg;

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); bi != banks.end(); ++bi) {
    const LHCb::RawBank* rb = *bi;

    //== protect against corrupted banks
    if(LHCb::RawBank::MagicPattern!=rb->magic()) {
      failEvent(format("Bad magic pattern in bank source ID %i", rb->sourceID()),
                "CorruptVeloBuffer",CorruptVeloBuffer,false);
      continue;
    }

    //== Get the sensor, in fact the source id is the neumber...
    const DeVeloSensor* sensor = m_velo->sensorByTell1Id(static_cast<unsigned int>(rb->sourceID()));
    if (!sensor) {
      failEvent( format("Could not map source ID %i to sensor number!", rb->sourceID()),
                 "BadTELL1IDMapping",BadTELL1IDMapping,false);
      continue;
    }
    sensor->tell1EventInfo().setHasError(false);
    unsigned int sensorNb = sensor->sensorNumber();

    const unsigned int* data = rb->data();

    unsigned int nClus = (*data) & 0xFFFF;
    //unsigned int pcn   = ( (*data) & 0xFF0000 ) >> 16;
    unsigned int error = ( (*data) & 0x1000000 ) >> 24;
    if ( !m_ignoreErrors && 0 != error ) continue;
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Process source id " << rb->sourceID() << " that is sensor " << sensorNb
              << " data " << (*data) << " nData " << nClus << endmsg;
    }
    unsigned short* word = (unsigned short*) (data+1);
    for (  ; 0 < nClus; nClus--, ++word ) {
      unsigned int frac    = (*word) & 0x7;
      unsigned int channel = ( (*word) & 0x3FF8 ) >> 3;
      unsigned int size    = ( (*word) & 0x4000 ) >> 14;
      bool high            = ( ( (*word) & 0x8000 ) >> 15) != 0;
      fastCont->push_back( LHCb::VeloLiteCluster( frac, size, high,
                                                  LHCb::VeloChannelID( sensorNb, channel)));
    }
    sensor->tell1EventInfo().setWasDecoded(true);
  }

  if( fastCont->size() > m_maxVeloClusters){
    fastCont->clear();
    failEvent( format("Deleted all lite VELO clusters as more than limit %i in the event",
                      m_maxVeloClusters),
              "TooManyClusters", TooManyClusters, true );
  }


  std::sort( fastCont->begin(), fastCont->end(), SiDataFunctor::Less_by_Channel< LHCb::VeloLiteCluster >() );

  //== Test if the result is equal to another decoding in the default location

  if ( m_compareResult ) {
    LHCb::VeloLiteCluster::FastContainer* old;
    old = get<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default );

    if ( old->size() != fastCont->size() ) {
      info() << "=== Wrong size: old " << old->size() << " new " << fastCont->size() << endmsg;
    } else {
      LHCb::VeloLiteCluster::FastContainer::iterator itOld = old->begin();
      LHCb::VeloLiteCluster::FastContainer::iterator itNew = fastCont->begin();
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
StatusCode FastVeloDecoding::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Print and store failure information
//=========================================================================
void FastVeloDecoding::failEvent( const std::string &ErrorText,
                                  const std::string &ProcText,
                                  AlgStatusType status,
                                  bool procAborted){
  // set ProcStat for this event to failed in DecodeVeloRawBuffer
  LHCb::ProcStatus *pStat = getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);
  pStat->addAlgorithmStatus("DecodeVeloRawBuffer", "VELO", ProcText, status, procAborted);
  if( m_errorCount > 0 || msgLevel(MSG::DEBUG) ) {
    unsigned int msgCount = m_errorCount;
    if ( msgLevel(MSG::DEBUG) ) msgCount += 10;
    Error( ErrorText, StatusCode::SUCCESS, msgCount ).ignore();
  }
}
//=============================================================================
