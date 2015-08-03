#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"
#include "Event/ProcStatus.h"

#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"

#include "DecodeRawBankToLiteClusters.h"
#include "DecodeRawBankToClusters.h"

#include "DecodeVeloRawBuffer.h"
#include "VeloRawBankDecoder.h"
#include "VeloRawBankVersions.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloRawBuffer
//
// 2004-02-02 : Chris Parkes
// 2006-02-23 : David Jones, Kurt Rinnert
//              - complete rewrite
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DecodeVeloRawBuffer )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloRawBuffer::DecodeVeloRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : Decoder::AlgBase ( name , pSvcLocator ) 
    , m_forcedBankVersion(0) // there is no version 0, so this means bank version is not enforced
    , m_velo(NULL)
    , m_ignoreErrors(false) 
    , m_incidentSvc(0)
{
  declareProperty("DecodeToVeloLiteClusters",m_decodeToVeloLiteClusters=true);
  declareProperty("DecodeToVeloClusters",m_decodeToVeloClusters=false);
  declareProperty("DumpVeloClusters",m_dumpVeloClusters=false);
  declareProperty("VeloLiteClustersLocation",m_veloLiteClusterLocation=LHCb::VeloLiteClusterLocation::Default);
  declareProperty("VeloClusterLocation",m_veloClusterLocation=LHCb::VeloClusterLocation::Default);
  declareProperty("AssumeChipChannelsInRawBuffer",m_assumeChipChannelsInRawBuffer=false);
  declareProperty("ForceBankVersion",m_forcedBankVersion=0);
  declareProperty("ErrorCount",m_errorCount=0);
  declareProperty("IgnoreErrors",m_ignoreErrors=false,
                  "Decode clusters even if errors are present. Use with care, can cause crashes on corrupted banks.");
  declareProperty("DoLengthCheck",m_doLengthCheck=false,
                  "Check when decoding lite clusters that the bank length is correct");

  declareProperty("MaxVeloClusters", m_maxVeloClusters = 10000);
  declareProperty("HideWarnings", m_hideWarnings = true);
  //new for decoders, initialize search path, and then call the base method
  m_defaultRawEventLocations={LHCb::RawEventLocation::Velo,    // When only VELO retained in stripping
			      LHCb::RawEventLocation::Other,   // Stripping default 
			      LHCb::RawEventLocation::Default};// RAW default
			      
  m_rawEventLocations.resize(m_defaultRawEventLocations.size(),"");
  std::copy(m_defaultRawEventLocations.begin(),
	    m_defaultRawEventLocations.end(),m_rawEventLocations.begin());
  initRawEventSearch();
}


//=============================================================================
// Destructor
//=============================================================================
DecodeVeloRawBuffer::~DecodeVeloRawBuffer() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DecodeVeloRawBuffer::initialize() {

  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel( MSG::DEBUG ) ) debug () << "==> Initialise" << endmsg;

  // check whether enforced bank version is supported
  if(m_forcedBankVersion) {
    switch (m_forcedBankVersion) {
      case VeloDAQ::v2: // ok, supported
      case VeloDAQ::v3:
        break;
      default: // not supported, bail out
        error() << "User enforced VELO raw buffer version " 
          << m_forcedBankVersion 
          << " is not supported."
          << endmsg;
        return StatusCode::FAILURE;
    }
  }

  info() << "Cancel processing of events with more than " << m_maxVeloClusters
    << " Velo clusters" << endmsg;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  if (m_rawEventLocations.empty()) 
  {
    return Error("I can't decode if you don't tell me where to decode to! Fill RawEventLocations!",StatusCode::FAILURE);
  }
  
  if ( m_rawEventLocations.size() == 1 ){ // if one entry and not in default list print
    if ( std::find(m_defaultRawEventLocations.begin(),
		   m_defaultRawEventLocations.end(),
		   m_rawEventLocations[0]) == m_defaultRawEventLocations.end() ) {
      info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
    }
  }else if( ( m_defaultRawEventLocations.size() != m_rawEventLocations.size() ) or
	    (std::mismatch(m_defaultRawEventLocations.begin(), // if a list but not the same list print
			   m_defaultRawEventLocations.end(),
			   m_rawEventLocations.begin()).first != 
	     m_defaultRawEventLocations.end() ) ){
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  // Pointer to IncidentSvc
  m_incidentSvc = svc<IIncidentSvc>("IncidentSvc",true);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloRawBuffer::execute() {

  // reset TELL1 event info on all sensors to 'bad'.
  // it is necessary to do this here to handle the case of unknown source
  // IDs correctly.
  for ( std::vector<DeVeloSensor*>::const_iterator si=m_velo->sensorsBegin();
      si != m_velo->sensorsEnd();
      ++si ) {
    (*si)->tell1EventInfo().reset();
  }

  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if( rawEvent == NULL ) {
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Raw Event not found in " << m_rawEventLocations << endmsg;
    createEmptyBanks();
    return StatusCode::SUCCESS;
  }

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::Velo);

  StatusCode sc;

  // decode to lite clusters, if requested, bail out if something goes wrong
  if (m_decodeToVeloLiteClusters) {
    sc = decodeToVeloLiteClusters(banks);
    if (sc.isFailure()) return sc;
  }

  // decode to clusters, if requested, bail out if something goes wrong
  if (m_decodeToVeloClusters) {
    sc = decodeToVeloClusters(banks);
    if (sc.isFailure()) return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Private helpers
//=============================================================================

StatusCode DecodeVeloRawBuffer::decodeToVeloLiteClusters(const std::vector<LHCb::RawBank*>& banks) 
{
  LHCb::VeloLiteCluster::FastContainer* fastCont = new LHCb::VeloLiteCluster::FastContainer();

  // make sure we have enough capacity in the container
  // to avoid unnecessary relocations
  // yes this is a guessed number based on <4% of signal events 
  // need more than clusters (one length doubling operation required)
  fastCont->reserve(4096); 

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
      bi != banks.end();
      ++bi) {

    const LHCb::RawBank* rb = *bi;

    // --> protect against corrupted banks
    if(LHCb::RawBank::MagicPattern!=rb->magic()) {
      failEvent(format("Bad magic pattern in bank source ID %i",
            rb->sourceID()),
          "CorruptVeloBuffer",CorruptVeloBuffer,false);
      continue;
    }

    const DeVeloSensor* sensor = m_velo->sensorByTell1Id(static_cast<unsigned int>(rb->sourceID()));
    if (!sensor) {
      failEvent(format("Could not map source ID %i to sensor number!",
            rb->sourceID()),
          "BadTELL1IDMapping",BadTELL1IDMapping,false);
      continue;
    }

    int byteCount(0);
    int nClusters;
    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>(rb->data());

    if( m_doLengthCheck ) {
      nClusters =VeloDAQ::decodeRawBankToLiteClusters(rawBank,sensor,
                                                      m_assumeChipChannelsInRawBuffer,
                                                      fastCont, byteCount, m_ignoreErrors);
    }else{
      nClusters = VeloDAQ::decodeRawBankToLiteClusters(rawBank,sensor,
                                                       fastCont, m_ignoreErrors);
    }
    
    // reset the TELL1 event info in case the flags for this sensor were cleared in
    // a another decoder branch
    sensor->tell1EventInfo().reset();
    
    if( nClusters == -1 ) {
      if ( !m_ignoreErrors ) {
        if ( msgLevel( MSG::DEBUG ) ) debug() << "Header error bit set in raw bank source ID " 
                                              << rb->sourceID() << endmsg;
        failEvent(format("Header error bit set in the VELO, bank source ID %i",
                         rb->sourceID()),
                  "HeaderErrorVeloBuffer",HeaderErrorBit,false);
        // no clusters produced so continue
        continue;
      }
    } else { // there was no error, update TELL1 event info
      sensor->tell1EventInfo().setHasError(false);
    }

    if ( m_doLengthCheck && (rb->size() != byteCount) ) {      
      if ( msgLevel( MSG::DEBUG ) ) debug() << "Byte count mismatch between RawBank size and decoded bytes." 
        << " RawBank: " << rb->size() 
          << " Decoded: " << byteCount 
          << endmsg;
      failEvent(format("Raw data corruption in the VELO, bank source ID %i",
            rb->sourceID()),
          "CorruptVeloBuffer",CorruptVeloBuffer,false);
      unsigned int badSensor = sensor->sensorNumber();
      // assume all clusters from the bad sensor at the end 
      while( !fastCont->empty() &&
          fastCont->back().channelID().sensor() == badSensor ) {
        fastCont->pop_back(); // actually the only deletion method of FastClusterContainer!
      }
      // a wrong byte count is also considered an error
      sensor->tell1EventInfo().setHasError(true);
    } else { // we got clusters decoded from this bank, update TELL1 event info
      sensor->tell1EventInfo().setWasDecoded(true); 
    }
  }
  if( fastCont->size() > m_maxVeloClusters){
    fastCont->clear();
    failEvent(format("Deleted all lite VELO clusters as more than limit %i in the event",
          m_maxVeloClusters),
        "TooManyClusters",TooManyClusters,true);
  }

  std::sort(fastCont->begin(),fastCont->end(),SiDataFunctor::Less_by_Channel< LHCb::VeloLiteCluster >());
  put(fastCont,m_veloLiteClusterLocation);

  return StatusCode::SUCCESS;
}

StatusCode DecodeVeloRawBuffer::decodeToVeloClusters(const std::vector<LHCb::RawBank*>& banks) 
{
  LHCb::VeloClusters* clusters = new LHCb::VeloClusters();

  // Number of bytes in bank, including 4 byte header but
  // *without* the padding bytes at the end. 
  // This is filled by the decoding function and should always 
  // be the same as RawBank::size().
  int byteCount;

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
      bi != banks.end();
      ++bi) {

    const LHCb::RawBank* rb = *bi;

    // a priory we assume the byte count is correct
    byteCount = rb->size();

    // --> protect against corrupted banks
    if(LHCb::RawBank::MagicPattern!=rb->magic()) {
      failEvent(format("Bad magic pattern in bank source ID %i",
            rb->sourceID()),
          "CorruptVeloBuffer",CorruptVeloBuffer,false);
      continue;
    }

    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>(rb->data());

    const DeVeloSensor* sensor = m_velo->sensorByTell1Id(static_cast<unsigned int>(rb->sourceID()));
    if (!sensor) {
      failEvent(format("Could not map source ID %i to sensor number!",
            rb->sourceID()),
          "BadTELL1IDMapping",BadTELL1IDMapping,false);
      continue;
    }

    // reset the TELL1 event info in case the flags for this sensor were cleared in
    // a another decoder branch
    sensor->tell1EventInfo().reset();

    unsigned int bankVersion = m_forcedBankVersion ? m_forcedBankVersion : rb->version();

    int nClusters = 0;
    std::string errorMsg;
    switch (bankVersion) {
      case VeloDAQ::v2:
        nClusters = 
          VeloDAQ::decodeRawBankToClustersV2(rawBank,sensor,
              m_assumeChipChannelsInRawBuffer,
              clusters,byteCount,m_ignoreErrors);
        break;
      case VeloDAQ::v3:
        nClusters = 
          VeloDAQ::decodeRawBankToClustersV3(rawBank,sensor,
              m_assumeChipChannelsInRawBuffer,
              clusters,byteCount,errorMsg,m_ignoreErrors);
        if ( !errorMsg.empty() ) {
          unsigned int msgCount = 0;
          if ( msgLevel(MSG::DEBUG) ) {
            msgCount = 10;
            Warning(errorMsg, StatusCode::SUCCESS, msgCount).ignore();
          }else{
            // currently suppressed due to presence of bug in TELL1 cluster maker
            if(!m_hideWarnings){
              Warning(errorMsg, StatusCode::SUCCESS, msgCount).ignore();            
            }
          } 
        }
        break;
      default: // bank version is not supported: kill the event
        failEvent(format("VELO raw buffer version %i is not supported.",
              rb->sourceID()),
            "UnsupportedBufferVersion",UnsupportedBufferVersion,false);
        continue;
    }

    if( nClusters == -1 ) {
      if ( !m_ignoreErrors ){
        debug() << "Header error bit set in raw bank source ID " 
          << rb->sourceID() << endmsg;
        failEvent(format("Header error bit set in the VELO, bank source ID %i",
              rb->sourceID()),
            "HeaderErrorVeloBuffer",HeaderErrorBit,false);
        // no clusters produced so continue
        continue;
      }
    } else { // there was no error, update TELL1 event info
      sensor->tell1EventInfo().setHasError(false);
    }
    
    if (rb->size() != byteCount) {      
      if ( msgLevel( MSG::DEBUG ) ) debug() << "Byte count mismatch between full RawBank size and decoded bytes." 
        << " RawBank: " << rb->size() 
          << " Decoded: " << byteCount 
          << endmsg;
      failEvent(format("Raw data corruption in the VELO, bank source ID %i",
            rb->sourceID()),
          "CorruptVeloBuffer",CorruptVeloBuffer,false);
      sensor->tell1EventInfo().setHasError(true);
      StatusCode sc = replaceFullFromLite(clusters,sensor->sensorNumber(),
                                          banks);
      if(!sc){
        delete clusters;
        return Error("Failed to convert lite to fake full VELO clusters");
      }
    } else { // we got clusters decoded from this bank, update TELL1 event info
      sensor->tell1EventInfo().setWasDecoded(true); 
    }

  }

  if( clusters->size() > m_maxVeloClusters){
    clusters->clear();
    failEvent(format("Deleted all full VELO clusters as more than limit %i in the event",m_maxVeloClusters),
        "TooManyClusters",TooManyClusters,true);
  }

  put(clusters,m_veloClusterLocation);
  if (m_dumpVeloClusters) dumpVeloClusters(clusters);

  return StatusCode::SUCCESS;
}

void DecodeVeloRawBuffer::dumpVeloClusters(const LHCb::VeloClusters* clusters) const
{
  std::vector<float> adcValues;
  
  for (LHCb::VeloClusters::const_iterator ci =  clusters->begin(); 
       ci != clusters->end(); 
       ++ci) {

    const LHCb::VeloCluster* clu = *ci;
    
    unsigned int sensorNumber = clu->channelID().sensor();
    unsigned int centroidStrip = clu->channelID().strip();

    // interstrip position in 1/8 of strip pitch (as it is encoded in raw bank)
    //     float interStripPos = static_cast<unsigned int>((clu->interStripFraction())*8.0)/8.0;
    double interStripPos = clu->interStripFraction();

    info() << "DEC::POSDUMP:"
	   << " SN=" << sensorNumber
	   << " CS=" << centroidStrip
	   << " ISP=" << interStripPos
	   << endmsg;

    info() << "DEC::ADCDUMP:"
	   << " SN=" << sensorNumber;

    for (unsigned int adci=0; adci<clu->size(); ++adci) {
      info() << " " << clu->adcValue(adci)
	     << "@" << clu->strip(adci);
    }
    info() << endmsg;
      
  }
  return;
}

void DecodeVeloRawBuffer::createEmptyBanks() {
  if ( m_decodeToVeloLiteClusters ) {
    LHCb::VeloLiteCluster::FastContainer* fastCont = new LHCb::VeloLiteCluster::FastContainer();
    put(fastCont,m_veloLiteClusterLocation);
  }
  if ( m_decodeToVeloClusters ) {
    LHCb::VeloClusters* clusters = new LHCb::VeloClusters();
    put(clusters,m_veloClusterLocation);
  }
}

void DecodeVeloRawBuffer::failEvent(const std::string &ErrorText,
				    const std::string &ProcText,
				    AlgStatusType status,
				    bool procAborted){ 
  // set ProcStat for this event to failed in DecodeVeloRawBuffer
  LHCb::ProcStatus *pStat = 
    getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);
  pStat->addAlgorithmStatus("DecodeVeloRawBuffer",
			    "VELO",ProcText,status,procAborted);
  if( m_errorCount > 0 || msgLevel(MSG::DEBUG) ) {
    unsigned int msgCount = m_errorCount;
    if ( msgLevel(MSG::DEBUG) ) msgCount += 10;
    Error(ErrorText,StatusCode::SUCCESS,msgCount).ignore();
  }
}

StatusCode DecodeVeloRawBuffer::
replaceFullFromLite(LHCb::VeloClusters *clusters,
                    unsigned int nSensor,
                    const std::vector<LHCb::RawBank*>& banks){

  unsigned int msgCount = 1;
  if ( msgLevel(MSG::DEBUG) ) msgCount = 10;
  Error(format("Corrupt buffer sensor %i, matching lite clusters in full bank",
               nSensor), StatusCode::SUCCESS, msgCount).ignore();

  LHCb::VeloLiteCluster::FastContainer *veloLiteClusters = 
    getIfExists<LHCb::VeloLiteCluster::FastContainer>(m_veloLiteClusterLocation);
  if(!veloLiteClusters){
    // not asked to decode in options and no DOD setup :(
    // But we are the decoder! We can make the container ourselves
    StatusCode sc = decodeToVeloLiteClusters(banks);
    if (sc.isFailure()) return sc;
    veloLiteClusters = get<LHCb::VeloLiteCluster::FastContainer>(m_veloLiteClusterLocation);
  }

  std::vector<LHCb::VeloChannelID> killChannels;
  
  for(LHCb::VeloClusters::iterator iFC = clusters->begin();
      iFC != clusters->end() ; ++iFC ){
    if( (*iFC)->channelID().sensor() == nSensor ){
      killChannels.push_back((*iFC)->channelID());
    }
  }
  // nuke all existing clusters from this sensor
  for(std::vector<LHCb::VeloChannelID>::iterator iFC = killChannels.begin();
      iFC != killChannels.end() ; ++iFC ){
    clusters->erase(*iFC);
  }

  // replace with fake cluster to match the badly decoded lite container
  LHCb::VeloLiteCluster::FastContainer::const_iterator iClus = 
    veloLiteClusters->begin();
  for( ; iClus != veloLiteClusters->end(); ++iClus ){
    if( iClus->channelID().sensor() == nSensor ){
      makeFakeCluster(*iClus,clusters);
    }
  }

  return StatusCode::SUCCESS;
}

void DecodeVeloRawBuffer::
makeFakeCluster(LHCb::VeloLiteCluster const &liteCluster,
                LHCb::VeloClusters* fakeClusters){

  LHCb::VeloCluster * fakeClus;
  if( liteCluster.pseudoSize() == 1){
    if( liteCluster.highThreshold() ){
      fakeClus =
        new LHCb::VeloCluster(
              liteCluster,
              LHCb::VeloCluster::ADCVector(1,
                std::pair<int,unsigned int>(liteCluster.channelID().strip(),64)));
    }else{
      fakeClus =
        new LHCb::VeloCluster(
              liteCluster,
              LHCb::VeloCluster::ADCVector(1,
                std::pair<int,unsigned int>(liteCluster.channelID().strip(),21)));
    }
  }else{
    // always fake two strip clusters for two or more strip clusters
    // from pseudosize
    if( liteCluster.highThreshold() ){
      unsigned int secondADC =
	static_cast<unsigned int>(4. + (liteCluster.interStripFraction()*64.));
      unsigned int firstADC = static_cast<unsigned int>(64 - secondADC);
      LHCb::VeloCluster::ADCVector vec;
      vec.reserve(2);
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip(),firstADC));
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip()+1,secondADC));
      fakeClus =
	new LHCb::VeloCluster(liteCluster,vec);
    }else{
      unsigned int secondADC =
	static_cast<unsigned int>((4. + (liteCluster.interStripFraction()*64.))/3.);
      unsigned int firstADC = static_cast<unsigned int>(21 - secondADC);
      LHCb::VeloCluster::ADCVector vec;
      vec.reserve(2);
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip(),firstADC));
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip()+1,secondADC));
      fakeClus =
	new LHCb::VeloCluster(liteCluster,vec);
    }
  }

  fakeClusters->insert(fakeClus,liteCluster.channelID());
  return;
}
