// Include files 

// from Gaudi
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "GaudiKernel/SystemOfUnits.h"
// from LHCb
#include "Event/L0DUReport.h"
#include "Event/ODIN.h" 
// local
#include "L0DUMultiTrends.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUMultiTrends
//
// 2008-03-26 : Olivier Deschamps
//
// Produce L0 pattern to feed L0DU board test-benches
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUMultiTrends )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUMultiTrends::L0DUMultiTrends( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) ,
    m_config(NULL),
    m_emulator(NULL),
    m_fromRaw(NULL),
    m_datas(NULL),
    m_odin(NULL),
    m_book(true),
    m_hasOrigin(false),
    m_bin(2),
    m_origin(0),
    m_oBin(299),
    m_count(0)
{
  declareProperty( "L0DUEmulatorTool"  , m_emulatorTool= "L0DUEmulatorTool");
  declareProperty( "L0DUFromRawTool"   , m_fromRawTool = "L0DUFromRawTool" );  
  declareProperty( "L0DUConfigTool"    , m_configTool  = "L0DUConfig");
  declareProperty( "TCKList"           , m_list);
  declareProperty( "NumberOfPredefinedTriggers", m_nPredTriggers = 4 ); // Must be set according to L0DUConfigProvider list
  declareProperty( "TrendingStep"      , m_trendStep=60   ); // N seconds/step
  declareProperty( "TrendingPeriod"    , m_trendPeriod=300); // N steps/display
  declareProperty( "TimedTrending"     , m_tTrend=true ); // 
  declareProperty( "DecisionMonitorMask", m_dMon=LHCb::L0DUDecision::Any ); // 
  declareProperty( "SubTriggersMonitor" , m_sMon=true   ); // 
  declareProperty( "ChannelsMonitor"    , m_cMon=false  ); // 
  declareProperty( "RatesMonitor"       , m_rate=false  ); // 
} 

//=============================================================================
// Destructor
//=============================================================================
L0DUMultiTrends::~L0DUMultiTrends() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0DUMultiTrends::initialize() {
  
  StatusCode sc = GaudiTupleAlg::initialize() ;
  if( sc.isFailure() ) 
  { return Error("Could not initialize base class CaudiAlgorithm",sc);}
  info() << "==> Initialize" << endmsg;
  
  // get the tools
  m_fromRaw   = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool , this );
  m_emulator  = tool<IL0DUEmulatorTool>(m_emulatorTool, m_emulatorTool);
  m_config    = tool<IL0DUConfigProvider>("L0DUMultiConfigProvider" , m_configTool);
  m_odin      = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);


  // Check
  if ( msgLevel(MSG::DEBUG) && m_list.empty())
    debug() << "Empty list of TCKs - only data TCK will be monitored" << endmsg;
  for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
    for(std::vector<std::string>::iterator jt = it+1  ; jt != m_list.end() ; jt++){
      if( (*it) == (*jt) ){
        warning() << "The requested TCK " << *it << " appears twice in the TCKList " << endmsg;
        warning() << "Emulator may give wrong result due to  downscaling - please clean the list ..." << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  // Histogramming
  m_bin = m_list.size() + 2;  

  m_oBin = m_trendPeriod - 1; // origin trending bin  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUMultiTrends::execute() {
  m_count++;
  using namespace LHCb;
  using namespace L0DUDecision;
  
  // get processor data
  if(!m_fromRaw->decodeBank()){
    Error("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
    return StatusCode::SUCCESS;
  }
  m_datas =  m_fromRaw->L0ProcessorDatas();

  // check 
  if( NULL == m_fromRaw->report().configuration() )Warning("report->L0DUConfig point to NULL").ignore();

  // book histogram (first event)
  if(m_book){
    booking("Counters/"); 
    if( m_rate )booking("Rates/"); 
    setLabels();
    m_book =false;
  }

  // =============== Fill Comparaison histo  =====================

  // event counter bin
  /* This block scoped because to compile on Windows ??? */
  {
    for( std::map<std::string,AIDA::IHistogram1D*>::iterator ih = m_cMap.begin() ; m_cMap.end() != ih ; ++ih ){
      AIDA::IHistogram1D* h1d = ih->second;
      std::string name = ih->first;
      std::string pref = name.substr(0,5);
      if( pref == std::string("Rates") )continue;
      if( NULL != h1d )fill( h1d , -1. ,1.);
    }
  }
  

  // bin 0 = current config (from raw)
  int bin = 0;
  if( m_dMon != 0 ){
    if( (Physics & m_dMon) != 0 && m_fromRaw->report().decision( Physics) )cFill("Counters/Decisions/Physics",bin);
    if( (Beam1   & m_dMon) != 0 && m_fromRaw->report().decision( Beam1)   )cFill("Counters/Decisions/Beam1"  ,bin);
    if( (Beam2   & m_dMon) != 0 && m_fromRaw->report().decision( Beam2)   )cFill("Counters/Decisions/Beam2"  ,bin);
  }

  // bin > 1 = emulated TCKs
  for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
    int itck = tck2int( *it );
    LHCb::L0DUConfig* config   = m_config->config( itck );
    if( NULL == config )continue;
    m_emulator->process(config , m_datas);
    bin++;
    if( m_dMon != 0 ){
      if( (Physics & m_dMon) != 0 && config->emulatedDecision(Physics ))cFill("Counters/Decisions/Physics", bin );
      if( (Beam1   & m_dMon) != 0 && config->emulatedDecision(Beam1   ))cFill("Counters/Decisions/Beam1"  , bin );
      if( (Beam2   & m_dMon) != 0 && config->emulatedDecision(Beam2   ))cFill("Counters/Decisions/Beam2"  , bin );
    }
  }
  // SubTriggers
  if( m_sMon)cFillSubTriggers();
  // Channels
  if( m_cMon)cFillChannels();

  // =============== trending =====================
  if(m_trendPeriod > 0 && NULL != m_odin){
    // get ODIN time
    unsigned long time = (unsigned long) ( (double) m_odin->getTime().ns()/Gaudi::Units::second);
    const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    if( NULL != odin ){
      // fix origin
      if( !m_hasOrigin ){
        m_origin = time;
        //        if( !m_tTrend )m_origin = odin->eventNumber();
        if( !m_tTrend )m_origin = m_count;
        m_hasOrigin = true;
      }      

      long diff = (time - m_origin);
      //      if( !m_tTrend )diff = odin->eventNumber() - m_origin;
      if( !m_tTrend )diff = m_count - m_origin;

      if( diff < 0)counter("lost events in trending") +=  1;
      else if( diff >= 0){ // may loose some events at first pass
        long nStep =  diff / m_trendStep ;
        long bin2 = m_oBin+nStep;
        // slide
        if( bin2 >= m_trendPeriod){
          int shift = bin2-m_trendPeriod+1; 
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << "Sliding origin = " << m_origin 
                    << " time " << time << " s - eventNumber " << odin->eventNumber() 
                    << "  diff = " << diff << "   shift = " << shift <<endmsg;
          slideHistos(shift);
          doRates();
          m_oBin -= shift;
          bin2 -= shift;
          //if(m_oBin < XXX) // restart :  protection against to far origin ??
        }
        tFill( "Counters/Trending/Event" , bin2 );
        if( m_dMon != 0x0 ){
          if((Physics & m_dMon)!= 0 &&  m_fromRaw->report().decision( Physics) )
            tFill("Counters/Trending/Decisions/Data/Physics", bin2 );
          if((Beam1 & m_dMon) != 0  &&  m_fromRaw->report().decision( Beam1  ) )
            tFill("Counters/Trending/Decisions/Data/Beam1"  , bin2 );
          if((Beam2 & m_dMon) != 0  &&  m_fromRaw->report().decision( Beam2  ) )
            tFill("Counters/Trending/Decisions/Data/Beam2"  , bin2 );
          int ic  = 0;
          for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
            int itck = tck2int( *it );
            LHCb::L0DUConfig* config   = m_config->config( itck );
            if( NULL == config )continue;
            ic++;
            std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
            if((Physics & m_dMon)!= 0&& config->emulatedDecision(Physics ))
              tFill("Counters/Trending/Decisions/"+conf+"Physics",bin2 ); 
            if((Beam1 & m_dMon) != 0 && config->emulatedDecision(Beam1   ))
              tFill("Counters/Trending/Decisions/"+conf+"Beam1", bin2 ); 
            if((Beam2 & m_dMon) != 0 &&  config->emulatedDecision(Beam2  ))
              tFill("Counters/Trending/Decisions/"+conf+"Beam2", bin2 ); 
          }
        } 
        if( m_sMon ){
          std::vector<std::string> temp = m_list;
          temp.push_back( std::string("data") );
          int ic = 0;
          for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
            LHCb::L0DUConfig* config = NULL;
            if( *i == "data" ){
              config = m_fromRaw->report().configuration();
            }else{
              int itck = tck2int( *i );
              config = m_config->config( itck );
            }
            if( NULL == config )continue;
            ic++;
            std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
            if( *i == "data" )conf = "Data/";
            const LHCb::L0DUTrigger::Map&  triggers = config->triggers();
            for( LHCb::L0DUTrigger::Map::const_iterator it = triggers.begin(); triggers.end() != it ; ++it ){
              const LHCb::L0DUTrigger* trigger = (*it).second;
              if( NULL == trigger )continue;
              if(trigger->index() >= m_nPredTriggers)continue;
              std::string name  =  trigger->name();
              std::string unit  = "Counters/Trending/SubTriggers/" + conf + name;
              if( "data" == *i && m_fromRaw->report().triggerDecisionByName( name ) ){
                tFill( unit, bin2 );
              }else if( trigger->emulatedDecision() ){
                tFill( unit, bin2 ); 
              } 
            }
          }  
        } 
        if( m_cMon ){
          std::vector<std::string> temp = m_list;
          temp.push_back( std::string("data") );
          int ic = 0;
          for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
            LHCb::L0DUConfig* config = NULL;
            if( *i == "data" ){
              config = m_fromRaw->report().configuration();
            }else{
              int itck = tck2int( *i );
              config = m_config->config( itck );
            }
            if( NULL == config )continue;
            ic++;
            std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
            if( *i == "data" )conf = "Data/";
            const LHCb::L0DUChannel::Map&  channels = config->channels();
            for( LHCb::L0DUChannel::Map::const_iterator it = channels.begin(); channels.end() != it ; ++it ){
              LHCb::L0DUChannel* channel = (*it).second;
              if( NULL == channel )continue;
              std::string name  =  channel->name();
              std::string unit  = "Counters/Trending/Channels/" + conf + name;
              if( "data" == *i && m_fromRaw->report().channelDecisionByName( name ) ){
                tFill( unit, bin2 );
              }else if( channel->emulatedDecision() ){
                tFill( unit, bin2 ); 
              } 
            }
          }
        }
      } 
    }else{
      Warning("Trending requested but ODIN bank is absent").ignore();
      counter("Missing ODIN information") += 1;
    }
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUMultiTrends::finalize() {
  info() << "==> Finalize" << endmsg;
  doRates(); // for last bin
  return GaudiTupleAlg::finalize() ;
}

//=============================================================================
//  Finalize
//=============================================================================


void L0DUMultiTrends::setLabels(){
  // Comparaison histos labelling
  for( std::map<std::string,AIDA::IHistogram1D*>::iterator ih = m_cMap.begin() ; m_cMap.end() != ih ; ++ih ){
    std::string unit = (*ih).first;    
    AIDA::IHistogram1D* h1d = histo1D( HistoID ( unit ) );
    if( NULL == h1d )continue;
    TH1D* th1d = Gaudi::Utils::Aida2ROOT::aida2root( h1d );
    TAxis* xAxis = th1d->GetXaxis();
    xAxis->SetBinLabel( 1 , "Event counter" );
    xAxis->SetBinLabel( 2 , std::string("Data TCK=" + tck2string( m_fromRaw->report().tck())).c_str() );  
    long i = 0;
    for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
      int itck = tck2int( *it );
      LHCb::L0DUConfig* config   = m_config->config( itck );
      if( NULL == config )continue;
      i++;
      xAxis->SetBinLabel( 2 + i, std::string("Emul TCK=" + *it).c_str() );
    }
    xAxis->LabelsOption( "" );
  }  
  return;
}

void L0DUMultiTrends::cFillSubTriggers(){
  std::vector<std::string> temp = m_list;
  temp.push_back( std::string("data") ); // append data
  int bin = 0;
  for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
    LHCb::L0DUConfig* config = NULL;
    if( *i == "data" ){
      config = m_fromRaw->report().configuration();
      bin = 0;
    }else{
      int itck = tck2int( *i );
      config = m_config->config( itck );
      bin++;
    }
    if( NULL == config)return;
    const LHCb::L0DUTrigger::Map& triggers = config->triggers();
    for( LHCb::L0DUTrigger::Map::const_iterator it = triggers.begin(); triggers.end()!=it ; ++it){
      const LHCb::L0DUTrigger* trigger = (*it).second;
      if( NULL == trigger )continue;
      if(trigger->index() >= m_nPredTriggers)continue;
      std::string name = trigger->name();
      std::string unit = "Counters/SubTriggers/" + name;
      if( *i == "data" && m_fromRaw->report().triggerDecisionByName( name ) )cFill(unit , bin);
      else if( trigger->emulatedDecision( ) )cFill( unit , bin );    
    }
  }
  
  return;
}


void L0DUMultiTrends::cFillChannels(){
  std::vector<std::string> temp = m_list;
  temp.push_back( std::string("data") ); // append data
  int bin = 0;
  for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
    LHCb::L0DUConfig* config = NULL;
    if( *i == "data" ){
      config = m_fromRaw->report().configuration();
      bin = 0;
    }else{
      int itck = tck2int( *i );
      config = m_config->config( itck );
      bin++;
    }
    if( NULL == config)return;
    const LHCb::L0DUChannel::Map& channels = config->channels();
    for( LHCb::L0DUChannel::Map::const_iterator it = channels.begin(); channels.end()!=it ; ++it){
      LHCb::L0DUChannel* channel = (*it).second;
      if( NULL == channel )continue;
      std::string name = channel->name();
      std::string unit = "Counters/Channels/" + name;
      if( *i == "data" && m_fromRaw->report().channelDecisionByName( name ) )cFill(unit , bin);
      else if( channel->emulatedDecision( ) )cFill( unit , bin );    
    }
  }
  
  return;
}


void L0DUMultiTrends::cBookSubTriggers( LHCb::L0DUConfig* config , std::string pref){
  if( NULL == config )return;
  const LHCb::L0DUTrigger::Map&  triggers = config->triggers();
  for( LHCb::L0DUTrigger::Map::const_iterator it = triggers.begin(); triggers.end() != it ; ++it ){
    const LHCb::L0DUTrigger* trigger = (*it).second;
    if( NULL == trigger ){
      Warning("trigger" + (*it).first + " point to Null" ).ignore();
      continue;
    }
    if(trigger->index() >= m_nPredTriggers)continue;
    std::string name  =  trigger->name();
    std::string unit  = "SubTriggers/" + name ;
    std::string title = "SubTrigger '" ;
    title += LHCb::L0DUDecision::Name[trigger->decisionType()] + "|" + name;
    title +=  "' decision ";
    if(pref == "Rates/")title += "rates";
    title += " versus TCK";
    cBook( pref + unit ,title );
  }
}

void L0DUMultiTrends::cBookChannels( LHCb::L0DUConfig* config , std::string pref){
  if( NULL == config )return;
  const LHCb::L0DUChannel::Map&  channels = config->channels();
  for( LHCb::L0DUChannel::Map::const_iterator it = channels.begin(); channels.end() != it ; ++it ){
    const LHCb::L0DUChannel* channel = (*it).second;
    if( NULL == channel ){
      Warning("Channel" + (*it).first + " point to Null" ).ignore();
      continue;
    }
    std::string name  =  channel->name();
    std::string unit  = "Channels/" + name ;
    std::string title = "Channel '" ;
    title += LHCb::L0DUDecision::Name[channel->decisionType()] + "|" + name;
    title +=  "' decision ";
    if(pref == "Rates/")title += "rates";
    title += " versus TCK";
    cBook( pref + unit ,title );
  }
}


void L0DUMultiTrends::tBookSubTriggers( std::string pref){
  std::vector<std::string> temp = m_list;
  temp.push_back( std::string("data") );

  int ic = 0;
  for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
    LHCb::L0DUConfig* config = NULL;
    if( *i == "data" )config = m_fromRaw->report().configuration();
    else{
      int itck = tck2int( *i );
      config = m_config->config( itck );
    }
    if( NULL == config )continue;
    ic++;
    std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
    if( *i == "data" )conf = "Data/";
    const LHCb::L0DUTrigger::Map&  triggers = config->triggers();
    for( LHCb::L0DUTrigger::Map::const_iterator it = triggers.begin(); triggers.end() != it ; ++it ){
      const LHCb::L0DUTrigger* trigger = (*it).second;
      if( NULL == trigger ){
        Warning("trigger" + (*it).first + " point to Null" ).ignore();
        continue;
      }
      if(trigger->index() >= m_nPredTriggers)continue;
      std::string name  =  trigger->name();
      std::string unit  = "Trending/SubTriggers/" + conf + name;
      std::string title = "SubTrigger '" ;
      title += LHCb::L0DUDecision::Name[trigger->decisionType()] + "|" + name;
      title +=  "' trending decision for ";
      if( *i == "data" )title += " the data configuration";
      else title += " the emulated TCK="+(*i); 
      tBook( pref + unit ,title ); 
    }
  }
}



void L0DUMultiTrends::tBookChannels( std::string pref){
  std::vector<std::string> temp = m_list;
  temp.push_back( std::string("data") );

  int ic = 0;
  for(std::vector<std::string>::iterator i = temp.begin() ; i != temp.end() ; i++){
    LHCb::L0DUConfig* config = NULL;
    if( *i == "data" )config = m_fromRaw->report().configuration();
    else{
      int itck = tck2int( *i );
      config = m_config->config( itck );
    }
    if( NULL == config )continue;
    ic++;
    std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
    if( *i == "data" )conf = "Data/";
    const LHCb::L0DUChannel::Map&  channels = config->channels();
    for( LHCb::L0DUChannel::Map::const_iterator it = channels.begin(); channels.end() != it ; ++it ){
      const LHCb::L0DUChannel* channel = (*it).second;
      if( NULL == channel ){
        Warning("channel" + (*it).first + " point to Null" ).ignore();
        continue;
      }
      std::string name  =  channel->name();
      std::string unit  = "Trending/Channels/" + conf + name;
      std::string title = "Channel '" ;
      title += LHCb::L0DUDecision::Name[channel->decisionType()] + "|" + name;
      title +=  "' trending decision for ";
      if( *i == "data" )title += " the data configuration";
      else title += " the emulated TCK="+(*i); 
      tBook( pref + unit ,title ); 
    }
  }
}


void L0DUMultiTrends::slideHistos(int shift){
  if( 0 == shift)return;
  double weight = ( m_trendStep > 0) ? 1./(double) m_trendStep : 1.;
  for( std::map<std::string,AIDA::IHistogram1D*>::iterator it = m_tMap.begin() ; m_tMap.end() != it ; ++it){
    std::string name = (*it).first;
    if( name.substr(0,5) == "Rates")continue;
    AIDA::IHistogram1D* h1d = (*it).second;
    if(NULL == h1d)continue;
    TH1D* th1d = Gaudi::Utils::Aida2ROOT::aida2root( h1d );
    for( int i = 0 ; i < m_trendPeriod ; ++i){
      double c = h1d->binHeight( i );
      double cp=0;
      if(i+shift < m_trendPeriod && i+shift >= 0) cp = h1d->binHeight( i+shift );
      if( c+cp != 0){
        th1d->SetBinContent( i+1, cp);
        th1d->SetBinError( i+1, sqrt(cp*weight));
      }
    }
  }
}

void L0DUMultiTrends::tFill(std::string unit , int bin){
  AIDA::IHistogram1D* h1d=m_tMap[unit];
  double weight = ( m_trendStep > 0) ? 1./(double) m_trendStep : 1.;
  if( h1d != NULL && 0 <= bin && bin< m_trendPeriod)fill( h1d , (bin-m_trendPeriod) , weight );
  //if( h1d != NULL && 0 <= bin && bin< m_trendPeriod)fill( h1d , bin , weight );
  return;
}
 
void L0DUMultiTrends::tBook(std::string unit , std::string title){
  AIDA::IHistogram1D* h1d = book1D( HistoID(unit) ,title, (double) -m_trendPeriod, 0., m_trendPeriod );
  //AIDA::IHistogram1D* h1d = book1D( HistoID(unit) ,title, 0.,  (double) m_trendPeriod , m_trendPeriod );
  m_tMap[unit]= h1d ;
  return;
}

void L0DUMultiTrends::cFill(std::string unit , int bin){
  AIDA::IHistogram1D* h1d=m_cMap[unit];
  if( h1d != NULL && -1 <= bin && bin< m_bin)fill( h1d , bin , 1. );
  return;
}

void L0DUMultiTrends::cBook(std::string unit , std::string title){
  AIDA::IHistogram1D* h1d = book1D( HistoID(unit) ,title, -1 , (double) m_bin-1, m_bin );
  m_cMap[unit]= h1d ;
  return;
}

void L0DUMultiTrends::booking(std::string pref){

  using namespace LHCb;
  using namespace L0DUDecision;

  //======= Comparaison histos ========

  // Decisions
  if( m_dMon != 0 ){
    if( (Physics & m_dMon) != 0 )cBook(pref+"Decisions/Physics","L0|Physics decision "
                                       + std::string((pref == "Rates/") ? "rates" : "") + " versus TCK");
    if( (Beam1   & m_dMon) != 0 )cBook(pref+"Decisions/Beam1"  ,"L0|Beam1 decision "
                                       + std::string((pref == "Rates/") ? "rates" : "") + " versus TCK");
    if( (Beam2   & m_dMon) != 0 )cBook(pref+"Decisions/Beam2"  ,"L0|Beam2 decision "
                                       + std::string((pref == "Rates/") ? "rates" : "") + " versus TCK");
  }
  
  // SubTriggers
  if( m_sMon ){
    cBookSubTriggers( m_fromRaw->report().configuration() , pref);
    for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
      int itck = tck2int( *it );
      LHCb::L0DUConfig* config   = m_config->config( itck );
      if( NULL != config )continue;
      cBookSubTriggers( config , pref);
    } 
  }
  
  // Channels
  if( m_cMon ){
    cBookChannels( m_fromRaw->report().configuration() , pref);
    for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
      int itck = tck2int( *it );
      LHCb::L0DUConfig* config   = m_config->config( itck );
      if( NULL != config )continue;
      cBookChannels( config , pref);
    } 
  }
  
  //=========== Trending histos =========
  if ( 0 < m_trendPeriod ){
    if( "Counters/" == pref)tBook( pref + "Trending/Event" , "Trending event counter");
    if( m_dMon != 0 ){
      if( (Physics & m_dMon) != 0 )tBook(pref+"Trending/Decisions/Data/Physics", "L0|Physics trending decision "
                                         + std::string((pref == "Rates/") ? "rates" : "") + " for data TCK");
      if( (Beam1   & m_dMon) != 0 )tBook(pref+"Trending/Decisions/Data/Beam1"  , "L0|Beam1 trending decision "
                                         + std::string((pref == "Rates/") ? "rates" : "") + " for data TCK");
      if( (Beam2   & m_dMon) != 0 )tBook(pref+"Trending/Decisions/Data/Beam2"  , "L0|Beam2 trending decision "
                                         + std::string((pref == "Rates/") ? "rates" : "") + " for data TCK");
      int ic  = 0;
      for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
        int itck = tck2int( *it );
        LHCb::L0DUConfig* config   = m_config->config( itck );
        if( NULL == config )continue;
        ic++;
        std::string conf="Emul"+Gaudi::Utils::toString( ic )+"/";
        tBook(pref+"Trending/Decisions/"+conf+"Physics", "L0|Physics trending decision "
              + std::string((pref == "Rates/") ? "rates" : "") + " Emul TCK="+*it );
        tBook(pref+"Trending/Decisions/"+conf+"Beam1"  , "L0|Beam1   trending decision "
              + std::string((pref == "Rates/") ? "rates" : "") + " Emul TCK="+*it );
        tBook(pref+"Trending/Decisions/"+conf+"Beam2"  , "L0|Beam2   trending decision "
              + std::string((pref == "Rates/") ? "rates" : "") + " Emul TCK="+*it );
      }
    }    
    if( m_sMon )tBookSubTriggers(pref);
    if( m_cMon )tBookChannels(pref);
  }



  return; 
}

int L0DUMultiTrends::tck2int(std::string otck){
  if( "0x" != otck.substr( 0, 2 ) ){
    Error("SKIP the requested TCK value " + otck + " (MUST be in hexadecimal format '0x" + otck + "')").ignore();
    return 0x0;
  }
  int itck;
  std::istringstream is( otck );
  is >> std::hex >> itck;
  return itck;
}

std::string L0DUMultiTrends::tck2string( int itck ){
  std::ostringstream otck("");
  otck << format("0x%04X" , itck );
  return otck.str();
}



void L0DUMultiTrends::doRates(){
  
  if( !m_rate)return;
  
  // comparaison histo : normalize with 1st bin
  for(std::map<std::string,AIDA::IHistogram1D*>::const_iterator ih = m_cMap.begin();ih != m_cMap.end() ; ++ih){
    std::string name = (*ih).first;
    if( name.substr(0,5) == "Rates")continue;
    AIDA::IHistogram1D* h1 = (*ih).second;
    int index = name.find_first_of("/")+1;
    AIDA::IHistogram1D* h2 = m_cMap["Rates/" + name.substr(index,std::string::npos) ];
    if( NULL == h1 || NULL == h2 ) continue;
    
    double norm = h1 -> binHeight( 0 );
    if( 0 == norm )continue;
    TH1D* th2 = Gaudi::Utils::Aida2ROOT::aida2root( h2 );
    for( int i = 0 ; i < m_bin ; ++i){
      double n =  h1->binHeight( i );
      if( n > norm )continue;
      double c = n / norm;
      th2->SetBinContent( i+1, c);
      th2->SetBinError  ( i+1, sqrt(n*(norm-n)/norm)/norm );
    }
  }
  

  // trending histos : divide
  AIDA::IHistogram1D* h0 = m_tMap[ "Counters/Trending/Event" ];
  if( NULL == h0 )return;
  TH1D* th0 = Gaudi::Utils::Aida2ROOT::aida2root( h0 );
  for(std::map<std::string,AIDA::IHistogram1D*>::iterator ih = m_tMap.begin();ih != m_tMap.end() ; ++ih){
    std::string name = (*ih).first;
    if( name.substr(0,5) == "Rates")continue;
    AIDA::IHistogram1D* h1 = (*ih).second;
    int index = name.find_first_of("/")+1;
    AIDA::IHistogram1D* h2 = m_tMap["Rates/"  + name.substr(index,std::string::npos) ];
    if( NULL == h1 || NULL == h2 ) continue;
    h2->reset();
    TH1D* th1 = Gaudi::Utils::Aida2ROOT::aida2root( h1 );
    TH1D* th2 = Gaudi::Utils::Aida2ROOT::aida2root( h2 );
    th2->Divide(th1,th0);
  }
}
