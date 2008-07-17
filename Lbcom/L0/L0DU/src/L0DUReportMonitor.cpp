// $Id: L0DUReportMonitor.cpp,v 1.13 2008-07-17 16:16:07 odescham Exp $
// Include files 
#include <cmath>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"
// local
#include "L0DUReportMonitor.h"
// local
//-----------------------------------------------------------------------------
// Implementation file for class : L0DUReportMonitor
//
// 2006-02-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUReportMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUReportMonitor::L0DUReportMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_chanCntMap(), 
    m_condCntMap(),
    m_chanRateMap(),
    m_condRateMap(),
    m_trigCntMap(),
    m_trigRateMap(),
    m_trigRelRateMap(),
    m_evtCntMap(),
    m_decCntMap()
{
  declareProperty( "ReportLocation"    , m_reportLocation =  LHCb::L0DUReportLocation::Default );
  declareProperty( "DataMonitor"       , m_data = false );
  declareProperty( "ConditionsMonitor" , m_cond = false );
  declareProperty( "ChannelsMonitor"   , m_chan = false );
  declareProperty( "TriggersMonitor"   , m_trig = false );
  declareProperty( "DecisionMonitor"   , m_dec  = true  );
  declareProperty( "FullMonitoring"    , m_full  = false);
  declareProperty( "DataReBinFactor"   , m_bin = 1);
  declareProperty( "SplitConfig"       , m_split = true);

  if( context() == "Emulation" )m_reportLocation = LHCb::L0DUReportLocation::Emulated;

  setHistoDir( name );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUReportMonitor::~L0DUReportMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUReportMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  m_prevTCK   = -1;

  if(m_full){
    m_data=true;
    m_cond=true;
    m_chan=true;
    m_trig=true;
    m_dec=true;
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUReportMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  

  if( !exist<LHCb::L0DUReport>( m_reportLocation )){
    Error("L0DUReport not found at location " + m_reportLocation ).ignore();
    return StatusCode::SUCCESS;
  }

  LHCb::L0DUReport* report   = get<LHCb::L0DUReport>( m_reportLocation );
  LHCb::L0DUConfig* config   = report->configuration();
  unsigned int tck           = report->tck();
  std::stringstream ttck("");
  ttck << format("0x%04X", tck) ;


  if(config == NULL){
    Error("NULL L0DUConfig for tck = " + ttck.str() + " -> cannot monitor the report").ignore();
    return StatusCode::SUCCESS;
  }
  if( !report->valid() ){
    Error("Invalid report -> cannot monitor").ignore();
    return StatusCode::SUCCESS;
  }

  // Initialisation
  bool init = false;
  if((int) tck != m_prevTCK ){
    m_prevTCK = (int) tck;
    // is this tck known ?
    std::map<unsigned int,double>::iterator it = m_evtCntMap.find(tck);
    if( it == m_evtCntMap.end() ){
      m_configs[tck]=config;
      m_evtCnt = 0.;
      m_decCnt = 0.;
      init = true;
    }
    else{  
      m_chanCnt     = m_chanCntMap[tck];
      m_condCnt     = m_condCntMap[tck];
      m_chanRate    = m_chanRateMap[tck];
      m_chanRelRate = m_chanRelRateMap[tck];
      m_condRate    = m_condRateMap[tck];
      m_evtCnt      = m_evtCntMap[tck];
      m_decCnt      = m_decCntMap[tck];
      m_trigCnt     = m_trigCntMap[tck];
      m_trigRate    = m_trigRateMap[tck];
      m_trigRelRate = m_trigRelRateMap[tck];
    }
    if(m_prevTCK != -1){
      if( !m_split){
        Warning("New configuration tck found : reset all histos").ignore();
        debug() << "New configuration : " << tck << " (previous was : " << m_prevTCK << ")" <<endreq;
        debug() << "Reset all histograms (number of histos : " << totalNumberOfHistos() << ")" << endreq;
        // reset all 1D histos
        const Histo1DMapTitle& h1d = histo1DMapTitle();
        for(Histo1DMapTitle::const_iterator id = h1d.begin() ; id != h1d.end() ; id++){
          debug() << "Reset 1D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endreq;
          if(NULL != (*id).second)(*id).second->reset();
        }
        // reset all 2D histos
        const Histo2DMapTitle& h2d = histo2DMapTitle();
        for(Histo2DMapTitle::const_iterator id = h2d.begin() ; id != h2d.end() ; id++){
          debug() << "Reset 2D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endreq;
          if(NULL != (*id).second)(*id).second->reset(); 
        }
      } 
    } 
  }
  LHCb::L0DUChannel::Map channels = config->channels();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  LHCb::L0DUTrigger::Map triggers = config->triggers();

  
  m_evtCnt += 1.;  // event counter
  if(report->decision())m_decCnt += 1.; // decision counter
  m_chanSeq[ "Global" ] = 0;

  // Channel counters 
  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    if( init )m_chanCnt[ (*it).first ]=0;//init    
    if(report->channelDecision( ((*it).second)->id() )) m_chanCnt[ (*it).first ] += 1.;
    m_chanRate[(*it).first ] = 0.;
    m_chanRelRate[(*it).first ] = 0.;
    m_chanSeq[ (*it).first ] = 0;
  }
  // Condition counter 
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    if( init ) m_condCnt[ (*it).first ]=0;//init    
    if( report->conditionValue( ((*it).second)->id() ) ) m_condCnt[ (*it).first ] += 1.;
    m_condRate[(*it).first ] = 0.;
    m_condSeq[ (*it).first ] = 0;
  }
  // Trigger counters 
  for(LHCb::L0DUTrigger::Map::iterator it = triggers.begin();it!=triggers.end();it++){
    if( init ) m_trigCnt[ (*it).first ] = 0;
    if( report->triggerDecisionByName( (*it).first )) m_trigCnt[ (*it).first ] += 1;
    m_trigRate[ (*it).first ] = 0;
    m_trigRelRate[ (*it).first ] = 0;
    m_trigSeq[ (*it).first ] = 0;
  }

  // Ratios
  for(CounterMap::iterator it = m_trigCnt.begin(); m_trigCnt.end()!=it;it++){
    double rate = 0;
    double relRate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;
    if(m_decCnt != 0) relRate= 100. * (*it).second / m_decCnt;    
    m_trigRate[(*it).first] = rate ;
    m_trigRelRate[(*it).first] = relRate ;
  }
  for(CounterMap::iterator it = m_chanCnt.begin(); m_chanCnt.end()!=it;it++){
    double rate = 0;
    double relRate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;
    if(m_decCnt != 0) relRate= 100. * (*it).second / m_decCnt;    
    m_chanRate[(*it).first] = rate ;
    m_chanRelRate[(*it).first] = relRate ;
  }
  for(CounterMap::iterator it = m_condCnt.begin(); m_condCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt !=0 ) rate = 100.* (*it).second / m_evtCnt;
    m_condRate[ (*it).first ]= rate ;
  }


  m_trigCntMap[tck]     = m_trigCnt;
  m_trigRateMap[tck]    = m_trigRate;
  m_trigRelRateMap[tck] = m_trigRelRate;
  m_chanCntMap[tck]     = m_chanCnt;
  m_chanRateMap[tck]    = m_chanRate;
  m_chanRelRateMap[tck] = m_chanRelRate;
  m_condCntMap[tck]     = m_condCnt;
  m_condRateMap[tck]    = m_condRate;
  m_evtCntMap[tck]      = m_evtCnt;
  m_decCntMap[tck]      = m_decCnt;


  // -----------------------------------------------------------------------------------------
  // --------------------------------- Histogramming -----------------------------------------
  // -----------------------------------------------------------------------------------------
  std::stringstream base("");

  if(m_split){
    base << "Config_" << ttck.str();
  }
  else{
    base << "CurrentConfig";
  }
  

  // ------------ Data  ------------
  if(m_data){
    std::vector<std::string> moniData;
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
      LHCb::L0DUElementaryCondition* condition = (*it).second;
      LHCb::L0DUElementaryData* data = (LHCb::L0DUElementaryData*) condition->data();
      bool ok=true;
      for(std::vector<std::string>::iterator idata = moniData.begin() ; idata != moniData.end() ; idata++){
        if( data->name() == *idata){
          ok=false;
          break;
        }      
      }
      if( !ok )continue;
      if( data->type() == LHCb::L0DUElementaryData::Constant)continue;
      moniData.push_back( data->name() );
      std::stringstream dUnit("");
      dUnit << base.str() + "/L0Data/" << moniData.size();
      std::stringstream dName("");
      if(data->type() == LHCb::L0DUElementaryData::Compound)
        dName << " L0 compound  data ";
      else
        dName << " L0 predefined  data ";
      
      dName << " -" << data->name() << "- distribution (TCK = " << ttck.str() << ")";
      long dBin = data->saturation()+1;
      double value = (double) data->digit();
      plot1D( value , dUnit.str() , dName.str(), 0 , (double) dBin , dBin/m_bin);
    }
  }
  // ------------ L0DU Conditions ------------
  if(m_cond){
    std::stringstream ecName("");
    ecName << "L0DU Elementary Conditions counters (TCK = " << ttck.str() << ")";
    
    int ecBin = conditions.size();
    plot1D( -1. , base.str() + "/L0Conditions/Counters/1" , ecName.str()+ " - BX=T0", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Prev2/1" , ecName.str()+ " - BX=Prev2", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Prev1/1" , ecName.str()+ " - BX=Prev1", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Next1/1" , ecName.str()+ " - BX=Next1", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Next2/1" , ecName.str()+ " - BX=Next2", -1. ,(double) ecBin  , ecBin+1);
    if(ecBin>1)plot2D( -1. , -1. , base.str() + "/L0Conditions/Counters2D/1" 
                       , "L0DU Elementary Conditions 2D INclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
    if(ecBin>1)plot2D( -1. , -1. , base.str() + "/L0Conditions/Counters2D/2" 
                       , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
    
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;
      if( report->conditionValue( id ) ){
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/1" ,ecName.str() + " - BX=T0", -1. ,(double) ecBin  , ecBin+1);
        m_condSeq[name] |=  (1 << 2);
      }
      if( report->conditionValue( id,-2 ) ){  
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Prev2/1"
                ,ecName.str() + " - BX=Prev2", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 0);
      }
      
      if( report->conditionValue( id,-1 ) ){  
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Prev1/1" 
                ,ecName.str() + " - BX=Prev1", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 1);
      }
      if( report->conditionValue( id,+1 ) ){      
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Next1/1" 
                ,ecName.str() + " - BX=Next1", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 3);
      }    
      if( report->conditionValue( id,+2 ) ){
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Next2/1" 
                ,ecName.str() + " - BX=Next2", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 4);
      }
      std::stringstream xid("");
      xid << id+1;
      plot1D( m_condSeq[name] , base.str() + "/L0Conditions/Sequence/" + xid.str() 
              , "Decision sequence over 5 BX for -" + name + "- Elementary Condition (TCK " + ttck.str() +")  - LSB = Prev2"    
              , 0. , 32. , 32); 
      
      if( ecBin <= 1)continue;
    
      
      // correlations
      if( report->conditionValue( id ) ){
        for(LHCb::L0DUElementaryCondition::Map::iterator jt = it ;jt!=conditions.end();jt++){
          int jd = ((*jt).second)->id() ;
          if( report->conditionValue( jd ) ){
            // Inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Counters2D/1" 
                    , "L0DU Elementary Conditions 2D INclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
            // Exclusive 2D counters
            bool isX = true;
            for(LHCb::L0DUElementaryCondition::Map::iterator kt =  conditions.begin() ;kt!=conditions.end();kt++){
              int kd = ((*kt).second)->id() ;
              if( kd == id || kd == jd )continue;
              if(  report->conditionValue( kd ) ) {
                isX = false;
                break;
              }
            }
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Counters2D/2" 
                           , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
          }      
        } 
      }
    }  
  }
  // ------------ L0DU Channels --------------
  if(m_chan){
    std::stringstream tcName("");
    tcName << "L0DU  Channels Decision counters (TCK = " << ttck.str() << ")";
    std::stringstream fcName("");
    fcName << "L0DU  Channels PreDecision counters (TCK = " << ttck.str() << ")";
    int cBin = channels.size();
    plot1D( -1. , base.str() + "/L0Channels/Counters/1" , tcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/2" , fcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Prev2/1 " , tcName.str()+ " - BX=Prev2", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Prev1/1" , tcName.str()+ " - BX=Prev1", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Next1/1" , tcName.str()+ " - BX=Next1", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Next2/1" , tcName.str()+ " - BX=Next2", -1. ,(double) cBin  , cBin+1);
    if(cBin > 1)plot2D( -1. , -1. , base.str() + "/L0Channels/Counters2D/1" 
                        , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
    if(cBin > 1)plot2D( -1. , -1. , base.str() + "/L0Channels/Counters2D/2" 
                        , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
    
    
    for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;

      if( report->channelDecision( id ) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/1" , tcName.str()+ " - BX=T0"    , -1. ,(double) cBin  , cBin+1);
        m_chanSeq[name] |=  (1 << 2);
      }    


      if( report->channelDecision( id , -2) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Prev2/1" 
                , tcName.str()+ " - BX=Prev2" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq[name] |=  (1 << 0);
      }
      
      if( report->channelDecision( id , -1) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Prev1/1" 
                , tcName.str()+ " - BX=Prev1" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq[name] |=  (1 << 1);
      }
      if( report->channelDecision( id , +1) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Next1/1" 
                , tcName.str()+ " - BX=Next1" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq[name] |=  (1 << 3);
      }
    
      if( report->channelDecision( id , +2) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Next2/1"
                , tcName.str()+ " - BX=Next2" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq[name] |=  (1 << 4);
      }    
      if( report->channelPreDecision( id ) )
        plot1D( (double) id , base.str() + "/L0Channels/Counters/2" , fcName.str()+ " - BX=T0"    , -1. ,(double) cBin  , cBin+1);
      
      std::stringstream xid("");
      xid << id+1;
      plot1D( m_chanSeq[name] , base.str() + "/L0Channels/Sequence/" + xid.str() 
              , "Decision sequence over 5 BX for -" + name + "- channel (TCK " + ttck.str() +") - LSB = Prev2", 0. , 32., 32);
      
      if(cBin <= 1)continue;

      // correlations
      if( report->channelDecision( id ) ){
        for(LHCb::L0DUChannel::Map::iterator jt = it ;jt!=channels.end();jt++){
          int jd = ((*jt).second)->id() ;
          if( report->channelDecision( jd ) ){
            // inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Counters2D/1" 
                    , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);          
            //exclusive 2D counters
            bool isX = true;
            for(LHCb::L0DUChannel::Map::iterator kt = channels.begin();kt!=channels.end();kt++){
              int kd = ((*kt).second)->id() ;
              if( kd == id || kd == jd)continue;
              if( report->channelDecision( kd ) ){
                isX = false;
                break;
              }  
            }
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Counters2D/2" 
                           , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
          }
        }
      }
    }
  }
  
  // ------------ L0DU Triggers --------------
  if(m_trig && triggers.size() != 0){
    std::stringstream tName("");
    tName << "L0DU  Trigger Decision counters (TCK = " << ttck.str() << ")";
    int tBin = triggers.size();
    plot1D( -1. , base.str() + "/L0Triggers/Counters/1" , tName.str()+ " - BX=T0", -1. ,(double) tBin  , tBin+1);
    plot1D( -1. , base.str() + "/L0Triggers/Counters/Prev2/1" , tName.str()+ " - BX=Prev2", -1. ,(double) tBin  , tBin+1);
    plot1D( -1. , base.str() + "/L0Triggers/Counters/Prev1/1" , tName.str()+ " - BX=Prev1", -1. ,(double) tBin  , tBin+1);
    plot1D( -1. , base.str() + "/L0Triggers/Counters/Next1/1" , tName.str()+ " - BX=Next1", -1. ,(double) tBin  , tBin+1);
    plot1D( -1. , base.str() + "/L0Triggers/Counters/Next2/1" , tName.str()+ " - BX=Next2", -1. ,(double) tBin  , tBin+1);
    if(tBin > 1)plot2D( -1. , -1. , base.str() + "/L0Triggers/Counters2D/1" 
                        , "L0DU Triggers Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1);
    if(tBin > 1)plot2D( -1. , -1. , base.str() + "/L0Triggers/Counters2D/2" 
                        , "L0DU Triggers Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1);
    
    
    for(LHCb::L0DUTrigger::Map::iterator it = triggers.begin();it!=triggers.end();it++){
      std::string name = (*it).first;
      int id = (*it).second->id();
      if( report->triggerDecisionByName( name ) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/1" , tName.str()+ " - BX=T0"    , -1. ,(double) tBin  , tBin+1);
        m_trigSeq[name] |=  (1 << 2);
      }    


      if( report->triggerDecisionByName( name , -2) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/Prev2/1" 
                , tName.str()+ " - BX=Prev2" , -1. ,(double) tBin  , tBin+1);
        m_trigSeq[name] |=  (1 << 0);
      }
      
      if( report->triggerDecisionByName( name , -1) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/Prev1/1" 
                , tName.str()+ " - BX=Prev1" , -1. ,(double) tBin  , tBin+1);
        m_trigSeq[name] |=  (1 << 1);
      }
      if( report->triggerDecisionByName( name , +1) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/Next1/1" 
                , tName.str()+ " - BX=Next1" , -1. ,(double) tBin  , tBin+1);
        m_trigSeq[name] |=  (1 << 3);
      }
    
      if( report->triggerDecisionByName( name , +2) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/Next2/1"
                , tName.str()+ " - BX=Next2" , -1. ,(double) tBin  , tBin+1);
        m_trigSeq[name] |=  (1 << 4);
      }    

      std::stringstream xid("");
      xid << id+1;
      plot1D( m_trigSeq[name] , base.str() + "/L0Triggers/Sequence/" + xid.str() 
              , "Decision sequence over 5 BX for -" + name + "- trigger (TCK " + ttck.str() +") - LSB = Prev2", 0. , 32., 32);
      
      if(tBin <= 1)continue;

      // correlations
      if( report->triggerDecisionByName( name ) ){
        for(LHCb::L0DUTrigger::Map::iterator jt = it ;jt!=triggers.end();jt++){
          int jd = (*jt).second->id();
          if( report->triggerDecision( jd ) ){
            // inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Triggers/Counters2D/1" 
                    , "L0DU Triggers Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1);          
            //exclusive 2D counters
            bool isX = true;
            for(LHCb::L0DUTrigger::Map::iterator kt = triggers.begin();kt!=triggers.end();kt++){
              int kd = ((*kt).second)->id() ;
              if( kd == id || kd == jd)continue;
              if( report->triggerDecision( kd ) ){
                isX = false;
                break;
              }  
            }
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Triggers/Counters2D/2" 
                           , "L0DU Triggers Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1);
          }
        }
      }
    }
  }
  
  
  
  // ------------ Decision  ------------
  if(m_dec){
    std::stringstream decName("");
    decName << "L0 Decision counters (TCK = " << ttck.str() << ") - Counter/Decision/TTB/FB";
    plot1D( -1. , base.str() + "/L0Decision/Counters/1" , decName.str(), -1. ,3  , 4);
    if(report->decision())        plot1D( 0. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);
    if(report->timingTriggerBit())plot1D( 1. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);
    if(report->forceBit())        plot1D( 2. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);

    //sequence
    for(int i=-2;i<=2;i++){
      if(report->decisionFromSummary( i ))m_chanSeq["Global"] |=  (1 << (i+2) );
    }
    std::stringstream seqName("");
    seqName << "L0 Decision sequence over 5 BX (TCK = " << ttck.str() << ") - LSB = Prev2";
    plot1D( (double) m_chanSeq["Global"] , base.str() + "/L0Decision/Sequence/1" , seqName.str(), 0 ,32  , 32);
  }  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUReportMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;


  
  info() << "======================================================================== " <<endreq;
  info() << " L0DUReport Monitoring ran on " << m_evtCntMap.size() <<" Configuration(s) " << endreq;
  info() << "======================================================================== "  << endreq;
  info() << " " << endreq;
  for(std::map<unsigned int,LHCb::L0DUConfig*>::iterator it = m_configs.begin(); it != m_configs.end(); it++){
    unsigned int tck = (*it).first;
    LHCb::L0DUConfig* config =(*it).second;

    m_trigCnt     = m_trigCntMap[tck];
    m_trigRate    = m_trigRateMap[tck];
    m_trigRelRate = m_trigRelRateMap[tck];
    m_chanCnt     = m_chanCntMap[tck];
    m_condCnt     = m_condCntMap[tck];
    m_chanRate    = m_chanRateMap[tck];
    m_chanRelRate = m_chanRelRateMap[tck];
    m_condRate    = m_condRateMap[tck];
    m_evtCnt      = m_evtCntMap[tck];
    m_decCnt      = m_decCntMap[tck];
    double rate =  100.* m_decCnt / m_evtCnt ;
    double eRate = 100.* sqrt(m_decCnt)/m_evtCnt;
    
    info() << "   **************************************************** " << endreq;    
    info() << "   ***  Trigger Configuration Key : "  << format("0x%04X", tck)  << " (" << tck << ")"<<  endreq;
    info() << "   ***  Recipe name : '" << config->recipe() << "'" << endreq;
    info() << "   ***  short description : '" << config->definition() << "'" << endreq;
    info() << "   **************************************************** " << endreq;    
    debug() << "       The complete algorithm description is : " << endreq;
    debug() << config->description() << endreq;
    info() << " " << endreq;

    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "   -- L0 Performance on " << m_evtCnt << " events" << endreq;  
    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "              *  Accepted L0          : " 
           << format( " %8.0f events :  rate = ( %6.2f +- %6.2f) %% ", m_decCnt, rate, eRate ) 
           << endreq;

    if( m_trigCnt.size() != 0){
      info() << "   ------------------------ TRIGGERS --------------------------------- " <<endreq;
      for(CounterMap::iterator it =  m_trigRate.begin(); m_trigRate.end()!=it ; it++){
        std::string name = (*it).first;
        info() << "   * Trigger set : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ", 
                        name.c_str(), m_trigCnt[name], m_trigRate[name] , m_trigRelRate[name]) << endreq;
      }
    }
    
    info() << "   ------------------------ CHANNELS --------------------------------- " <<endreq;
    for(CounterMap::iterator ic =  m_chanRate.begin(); m_chanRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
      LHCb::L0DUChannel::Map channels = config->channels();
      LHCb::L0DUChannel* channel = channels[name];
      std::string status="** DISABLED **";
      if(channel->enable() ){
        status ="   ENABLED    ";
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name] , m_chanRelRate[name]) << endreq;
      }else{
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name]) << endreq;
      }
    }

    info() << "   ------------------------ CONDITIONS ------------------------------ " <<endreq;
    for(CounterMap::iterator ic =  m_condRate.begin(); m_condRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
    info() << "   *  Elementary Condition  " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_condCnt[name], m_condRate[name]) << endreq;
    }
    info() << "======================================================================== " <<endreq;
  }

   return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================




