// Include files 
// local
#include "SmartVeloErrorBankDecoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SmartVeloErrorBankDecoder
//
// 2008-08-24 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SmartVeloErrorBankDecoder )

using namespace VeloTELL1;

typedef unsigned int* ErrorBankIT;
typedef std::pair<ErrorBankIT, ErrorBankIT> ITPair;
typedef std::map<unsigned int, ITPair> BANKS;
typedef std::map<unsigned int, ITPair> SECTORS;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SmartVeloErrorBankDecoder::SmartVeloErrorBankDecoder( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_isDebug ( false ),
    m_rawEvent ( 0 ),
    m_errorBank ( 0 ),
    m_bankLength ( ),
    m_bankVersion ( 0 ),
    m_bankType ( 0 ),
    m_magicPattern ( 0 )
{
  declareProperty("PrintBankHeader", m_printBankHeader=0);
  declareProperty( "RawEventLocation",  m_rawEventLocation = "", 
                   "OBSOLETE. Use RawEventLocations instead" );
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");
  declareProperty("ErrorBankLocation",
                  m_errorBankLoc=VeloErrorBankLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
SmartVeloErrorBankDecoder::~SmartVeloErrorBankDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SmartVeloErrorBankDecoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_isDebug=msgLevel(MSG::DEBUG);
  if(m_isDebug) debug() << "==> Initialize" << endmsg;

  // Initialise the RawEvent locations
  bool usingDefaultLocation = m_rawEventLocations.empty() && m_rawEventLocation.empty();
  if (! m_rawEventLocation.empty()) {
    warning() << "The RawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }

  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SmartVeloErrorBankDecoder::execute() {
  
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  //
  StatusCode rawEvtStatus=getRawEvent();
  StatusCode bankStatus;
  if(rawEvtStatus.isSuccess()) bankStatus=cacheErrorRawBanks();
  if(bankStatus.isSuccess()){
    bankStatus=storeErrorRawBanks();
  }else{
    // go to the next event
    bankStatus=StatusCode::SUCCESS;
  }

  if ( ! bankStatus.isSuccess() ) {
    warning() << "storing error banks aborted (possible raw bank corruption)." << endmsg;
    bankStatus=StatusCode::SUCCESS;
  }
  //
  return ( bankStatus );
}

//=============================================================================
StatusCode SmartVeloErrorBankDecoder::getRawEvent()
{
  if(m_isDebug) debug()<< " ==> getRawEvent() " <<endmsg;
  if(m_isDebug) debug()<< "--------------------" <<endmsg;
  //

  // Retrieve the RawEvent:
  m_rawEvent = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    m_rawEvent = getIfExists<LHCb::RawEvent>(*p);
    if ( NULL != m_rawEvent ){
      if(m_isDebug) 
        debug()<< " ==> The RawEvent has been read-in from location: "
               << (*p) <<endmsg;  
      break;
    }
  }

  if( m_rawEvent == NULL ) {
    error() << " ==> There is no RawEvent at: " 
           << m_rawEventLocations <<endmsg;
    return ( StatusCode::FAILURE );
  }

  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode SmartVeloErrorBankDecoder::cacheErrorRawBanks()
{
  if(m_isDebug) debug()<< " ==> cacheErrorRawBanks() " <<endmsg;
  // check if there is error bank present
  m_cachedBanks.clear();
  m_bankLength.clear();
  const std::vector<LHCb::RawBank*>& errorBank=
    m_rawEvent->banks(LHCb::RawBank::VeloError);
  std::vector<LHCb::RawBank*>::const_iterator bIt;
  // if so write out the banks
  if(errorBank.size()!=0){
    m_errorBank=new VeloErrorBanks();
    //
    if(m_isDebug) debug()<< " --> Error bank detected of size: " << errorBank.size() <<endmsg;
    ITPair data;
    for(bIt=errorBank.begin(); bIt!=errorBank.end(); ++bIt){
      LHCb::RawBank* aBank=(*bIt);

      // --> protect against corrupted banks
      if(LHCb::RawBank::MagicPattern!=aBank->magic()) return ( StatusCode::FAILURE );

      // get the sensor number == sourceID if there is no full data
      unsigned int tell1=aBank->sourceID();
      m_bankLength[tell1]=aBank->size();
      m_bankVersion=aBank->version();
      m_bankType=aBank->type();
      m_magicPattern=aBank->magic();
      if(m_printBankHeader){
        info()<< " --> src Id: " << tell1 <<endmsg;
        info()<< " --> type of bank: " << (aBank->typeName()) <<endmsg;
        info()<< " --> bank header size: " << (aBank->hdrSize()) <<endmsg;
        info()<< " --> body size: " << (aBank->size()) <<endmsg;
        info()<< " --> total size: " << (aBank->totalSize()) <<endmsg;
      }
      // get pointer to the bank data
      data.first=aBank->begin<unsigned int>();
      unsigned int* inIT=aBank->begin<unsigned int>();
      unsigned int* dat=aBank->data();
      if(m_isDebug) debug()<< " --> src Id: " << tell1 <<endmsg;
      for(int step=0; step<6; ++step){
        if(m_isDebug) debug()<< " data ptr: " << step << " : " << (*(inIT+step)) <<endmsg;
        if(m_isDebug) debug()<< " data tab: " << step << " : " << (*(dat+step)) <<endmsg;
      }
      data.second=aBank->end<unsigned int>();
      m_cachedBanks[tell1]=data;
      int dist=std::distance(data.first, data.second);
      if(m_isDebug) debug()<< " --> bank body size: " << (dist*sizeof(unsigned int)) <<endmsg;
    }
  }else{
    Info(" --> No error bank detected - skipping to the next event ");
    return ( StatusCode::FAILURE );
  }
  if(m_isDebug) debug()<< " --> cached error bank strucure's size:" 
         << (m_cachedBanks.size()) <<endmsg;
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode SmartVeloErrorBankDecoder::storeErrorRawBanks()
{
  if(m_isDebug) debug()<< " ==> storeErrorRawBanks() " <<endmsg;
  //
  std::map<unsigned int, ITPair>::iterator bankIT;
  bankIT=m_cachedBanks.begin();
  for( ; bankIT!=m_cachedBanks.end(); ++bankIT){
    EvtInfo anEvtInfo(bankIT->first);
    allEvt evtInfoData;
    allError errorInfoData;
    dataVec sources;
    // get information on error sources
    SECTORS sectors=errorDetector(bankIT->first);
    if(sectors.size()!=SOURCES) {
      if(m_isDebug) debug()<< " --> Error detected for " << (sectors.size()) 
             << " PPFPGA(s) "<<endmsg;
      warning() << "Impossible number of PPFPGAs: " << sectors.size() << endmsg;
      return StatusCode::FAILURE;
    }

    VeloErrorBank* err=new VeloErrorBank(bankIT->first);
    
    // store words in error bank
    SECTORS::iterator secIT=sectors.begin();
    for( ; secIT!=sectors.end(); ++secIT){
      // write fixed words
      errorInfo anErrorInfo;
      ErrorBankIT begin=secIT->second.first-INIT_SHIFT;
      ErrorBankIT end=(secIT->second.first+1);
      for( ; begin!=end; ++begin){
        anErrorInfo.push_back(*begin);
      }
      if(secIT->second.second==0){
        anErrorInfo.push_back(*end);
        anErrorInfo.push_back(*(++end));
      }
      errorInfoData.push_back(anErrorInfo);
      // write optional part
      evtInfo anInfo;
      if(secIT->second.second!=0){
        // error was sent for this fpga build evt info section
        // encode properly the evt section block
        sources.push_back(secIT->first);
        ErrorBankIT begin=secIT->second.first-INIT_SHIFT;
        anInfo.push_back(*begin);
        anInfo.push_back(*(++begin));
        unsigned int word=(*(++begin));
        word=(word>>bitShift16)&bitMask16;
        anInfo.push_back(word);
        begin=secIT->second.second;
        ErrorBankIT end=begin+EVT_WORDS;
        for( ; begin!=end; ++begin){
          anInfo.push_back(*begin);
        }
      }
      evtInfoData.push_back(anInfo);
    }
    anEvtInfo.setEvtInfo(evtInfoData);
    err->setEvtInfoSection(anEvtInfo);
    err->setErrorInfoSection(errorInfoData);
    if(m_isDebug) debug()<< " bank lenght: " << m_bankLength[bankIT->first] 
           << " for tell1: " << (bankIT->first) <<endmsg;
    err->setBankLength(m_bankLength[bankIT->first]);
    err->setBankType(m_bankType);
    err->setBankVersion(m_bankVersion);
    err->setMagicPattern(m_magicPattern);
    if(m_isDebug) debug()<< " --> Detected errors: " << (sources.size()) <<endmsg;
    err->setErrorSources(sources); 
    m_errorBank->insert(err);       
  }
  //
  put(m_errorBank, m_errorBankLoc);
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
std::map<unsigned int, ITPair> SmartVeloErrorBankDecoder::errorDetector(
                                 unsigned int tell1)
{
  if(m_isDebug) debug()<< " ==> errorDetector() " <<endmsg;
  //
  unsigned int fpga=0;
  SECTORS sectors;
  BANKS::const_iterator bankIT;
  bankIT=m_cachedBanks.find(tell1);
  if(bankIT!=m_cachedBanks.end()){
    ITPair iTPair=bankIT->second;
    ErrorBankIT iT=iTPair.first;
    // we can skip first four words - see note EDMS 694818
    iT+=INIT_SHIFT;
    // check if the file has expected format
    ITPair aPair(0, 0);
    for( ; iT!=iTPair.second; ++iT){
      if(((*iT&bitMask8)==EBMarker0)&&(((*iT>>bitShift8)&bitMask8)==EBMarker)){
        // cache this iterator
        aPair.first=iT;
        if(((*iT>>bitShift16)&bitMask16)==ERROR_BANK_SIZE){
          // cache iterator to the first word
          aPair.second=(iT+1);
        }else{
          // store nothing
          aPair.second=0;
        }
        sectors[fpga]=aPair;
        ++fpga;
      }
    }
  }
  //
  return ( sectors );
}
//--
