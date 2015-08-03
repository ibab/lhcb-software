// Include files 
#include <cmath>
// from Gaudi
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
// local
#include "L0DUReportMonitor.h"
#include <bitset>
#include <limits.h>
// local
//-----------------------------------------------------------------------------
// Implementation file for class : L0DUReportMonitor
//
// 2006-02-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUReportMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUReportMonitor::L0DUReportMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator ),
    m_chanCntMap(), 
    m_condCntMap(),
    m_chanRateMap(),
    m_condRateMap(),
    m_trigCntMap(),
    m_trigRateMap(),
    m_trigRelRateMap(),
    m_decCntMap(),
    m_evtCntMap(),
    m_evtCnt(0),
    m_prevTCK(-1),
    m_swap(true)
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
  declareProperty( "xLabelOptions"     , m_lab ="" );
  declareProperty( "genericPath"       , m_generic = true );

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

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

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
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  
  std::string loc = dataLocation( m_reportLocation );
  LHCb::L0DUReport* report = getIfExists<LHCb::L0DUReport>( loc );
  if( NULL == report ){
    return Error("L0DUReport not found at location " + loc, StatusCode::SUCCESS );
  }
  LHCb::L0DUConfig* config   = report->configuration();
  unsigned int tck           = report->tck();
  std::stringstream ttck("");
  ttck << format("0x%04X", tck) ;
  
  
  if(config == NULL){
    return Error("NULL L0DUConfig for tck = " + ttck.str() + " -> cannot monitor the report", StatusCode::SUCCESS );
  }
  if( !report->valid() ){
    return Error("Invalid report -> cannot monitor", StatusCode::SUCCESS );
  }

  // Initialisation
  bool init = false;
  if((int) tck != m_prevTCK ){
    m_swap = true;
    m_prevTCK = (int) tck;
    // is this tck known ?
    std::map<unsigned int,double>::iterator it = m_evtCntMap.find(tck);
    if( it == m_evtCntMap.end() ){
      m_configs[tck]=config;
      m_evtCnt = 0.;
      int i = 1;
      while( i < LHCb::L0DUDecision::Any){
        m_decCnt[i] = 0.;
        i = i << 1;
      }
      m_decCnt[LHCb::L0DUDecision::Any] = 0.;
      init = true;
      m_chanCnt.clear();  
      m_condCnt.clear();  
      m_chanRate.clear();  
      m_chanRelRate.clear();
      m_condRate.clear();    
      m_trigCnt.clear();     
      m_trigRate.clear();    
      m_trigRelRate.clear();
    }
    else{  
      m_swap = false;
      m_chanCnt     = m_chanCntMap[tck];
      m_condCnt     = m_condCntMap[tck];
      m_chanRate    = m_chanRateMap[tck];
      m_condRate    = m_condRateMap[tck];
      m_evtCnt      = m_evtCntMap[tck];
      int i = 1;
      while( i < LHCb::L0DUDecision::Any){
        m_decCnt[i]      = m_decCntMap[i][tck];
        m_chanRelRate[i] = m_chanRelRateMap[i][tck];
        m_trigRelRate[i] = m_trigRelRateMap[i][tck];
        i = i << 1;
      }
      m_decCnt[LHCb::L0DUDecision::Any]      = m_decCntMap[LHCb::L0DUDecision::Any][tck];
      m_trigCnt     = m_trigCntMap[tck];
      m_trigRate    = m_trigRateMap[tck];
    }
    if(m_prevTCK != -1){
      if( !m_split){
        Warning("New configuration tck found : reset all histos",StatusCode::SUCCESS).ignore();
        if ( msgLevel(MSG::DEBUG) ) {
          debug() << "New configuration : " << tck << " (previous was : " << m_prevTCK << ")" <<endmsg;
          debug() << "Reset all histograms (number of histos : " << totalNumberOfHistos() << ")" << endmsg;
        }
        
        // reset all 1D histos
        const Histo1DMapTitle& h1d = histo1DMapTitle();
        for(Histo1DMapTitle::const_iterator id = h1d.begin() ; id != h1d.end() ; id++){
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << "Reset 1D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endmsg;
          if(NULL != (*id).second)(*id).second->reset();
        }
        // reset all 2D histos
        const Histo2DMapTitle& h2d = histo2DMapTitle();
        for(Histo2DMapTitle::const_iterator id = h2d.begin() ; id != h2d.end() ; id++){
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << "Reset 2D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endmsg;
          if(NULL != (*id).second)(*id).second->reset(); 
        }
      } 
    } 
  }
  LHCb::L0DUChannel::Map channels = config->channels();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  LHCb::L0DUTrigger::Map triggers = config->triggers();

  // prepare histo labelling
  std::map<int,std::string> chanNames;
  std::map<int,std::string> condNames;
  std::map<int,std::string> trigNames;
  std::map<int,std::string> deciNames;
  deciNames[0] = "L0 Physics";
  deciNames[1] = "L0 Beam1";
  deciNames[2] = "L0 Beam2";
  deciNames[3] = "Timing Trigger bit";
  deciNames[4] = "Force bit";
  std::map<int,std::string> slotNames;
  slotNames[-2] = "Prev2";
  slotNames[-1] = "Prev1";
  slotNames[ 0] = "T0";
  slotNames[+1] = "Next1";
  slotNames[+2] = "Next2";
  std::map<int,std::string> seqNames;
  for( int i = 0; i < 32 ; ++i){
    std::ostringstream s("");
    s << std::bitset<CHAR_BIT>( i );
    std::string label = s.str().substr(3,8);
    seqNames[i]=label;
  }
  



  m_evtCnt += 1.;  // event counter
  
  int idec = 1;
  while( idec < LHCb::L0DUDecision::Any){    
    if(report->decision( idec )     )m_decCnt[idec] += 1.; // decision counter
    idec = idec << 1;
  }
  if(report->decision( LHCb::L0DUDecision::Any )     )m_decCnt[LHCb::L0DUDecision::Any] += 1.; // decision counter
  
  m_chanSeq[ "Global" ] = 0;
  
  // Channel counters 
  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    if( init )m_chanCnt[ (*it).first ]=0;//init    
    if(report->channelDecision( ((*it).second)->id() )) m_chanCnt[ (*it).first ] += 1.;
    m_chanRate[(*it).first ] = 0.;
    
    int idec = 1;
    while( idec < LHCb::L0DUDecision::Any){    
      m_chanRelRate[idec][(*it).first ] = 0.;
      idec = idec << 1;
    }
    m_chanSeq[ (*it).first ] = 0;
    chanNames[ (*it).second->id() ] =  LHCb::L0DUDecision::Name[((*it).second)->decisionType()] +"|" +(*it).first ;
  }
  // Condition counter 
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    if( init ) m_condCnt[ (*it).first ]=0;//init    
    if( report->conditionValue( ((*it).second)->id() ) ) m_condCnt[ (*it).first ] += 1.;
    m_condRate[(*it).first ] = 0.;
    m_condSeq[ (*it).first ] = 0;
    condNames[ (*it).second->id() ] =  (*it).first ;
  }
  
  // Trigger counters 
  for(LHCb::L0DUTrigger::Map::iterator it = triggers.begin();it!=triggers.end();it++){
    if( init ) m_trigCnt[ (*it).first ] = 0;
    if( report->triggerDecisionByName( (*it).first )) m_trigCnt[ (*it).first ] += 1;
    m_trigRate[ (*it).first ] = 0;
    int idec = 1;
    while( idec < LHCb::L0DUDecision::Any){
      m_trigRelRate[idec][ (*it).first ] = 0;
      idec = idec << 1;
    }
    m_trigSeq[ (*it).first ] = 0;
    trigNames[ (*it).second->id() ] =  LHCb::L0DUDecision::Name[((*it).second)->decisionType()] +"|" + (*it).first ;
  }
  
  // Ratios
  for(CounterMap::iterator it = m_trigCnt.begin(); m_trigCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;
    int idec = 1;
    while( idec < LHCb::L0DUDecision::Any){    
      double relRate = 0;
      if(m_decCnt[idec] != 0) relRate= 100. * (*it).second / m_decCnt[idec];    
      m_trigRelRate[idec][(*it).first] = relRate ;
      idec = idec << 1;
    }
    m_trigRate[(*it).first] = rate ;
  }
  for(CounterMap::iterator it = m_chanCnt.begin(); m_chanCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;

    int idec = 1;
    while( idec < LHCb::L0DUDecision::Any){    
      double relRate = 0;
      if(m_decCnt[idec] != 0) relRate= 100. * (*it).second / m_decCnt[idec];    
      m_chanRelRate[idec][(*it).first] = relRate ;
      idec = idec << 1;
    }
    m_chanRate[(*it).first] = rate ;
  }
  for(CounterMap::iterator it = m_condCnt.begin(); m_condCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt !=0 ) rate = 100.* (*it).second / m_evtCnt;
    m_condRate[ (*it).first ]= rate ;
  }


  m_trigCntMap[tck]     = m_trigCnt;
  m_trigRateMap[tck]    = m_trigRate;
  m_chanCntMap[tck]     = m_chanCnt;
  m_chanRateMap[tck]    = m_chanRate;
  m_condCntMap[tck]     = m_condCnt;
  m_condRateMap[tck]    = m_condRate;
  m_evtCntMap[tck]      = m_evtCnt;

  idec = 1;
  while( idec < LHCb::L0DUDecision::Any){
    m_decCntMap[idec][tck]  = m_decCnt[idec];
    m_chanRelRateMap[idec][tck] = m_chanRelRate[idec];
    m_trigRelRateMap[idec][tck] = m_trigRelRate[idec];
    idec = idec << 1;
  }
  m_decCntMap[LHCb::L0DUDecision::Any][tck]  = m_decCnt[LHCb::L0DUDecision::Any];
  
  
  // -----------------------------------------------------------------------------------------
  // --------------------------------- Histogramming -----------------------------------------
  // -----------------------------------------------------------------------------------------
  std::ostringstream base("");
  
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
      std::ostringstream dUnit("");
      if( !m_generic ){
        dUnit << base.str() + "/L0Data/" << data->name();
      }else{
        dUnit << base.str() + "/L0Data/" << moniData.size();
      }
      std::ostringstream dName("");
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
    std::ostringstream ecName("");
    ecName << "L0DU Elementary Conditions counters (TCK = " << ttck.str() << ")";
    int ecBin = conditions.size();
    label( plot1D( -1.,base.str()+"/L0Conditions/Counters/1" , 
                   ecName.str()+ " - BX=T0", -1. ,(double) ecBin  , ecBin+1),condNames);
    label( plot1D( -1.,base.str()+"/L0Conditions/Counters/Prev2/1" ,
                   ecName.str()+ " - BX=Prev2",-1.,(double) ecBin,ecBin+1), condNames);
    label( plot1D( -1.,base.str()+"/L0Conditions/Counters/Prev1/1" ,
                   ecName.str()+ " - BX=Prev1",-1.,(double) ecBin,ecBin+1), condNames);
    label( plot1D( -1.,base.str()+"/L0Conditions/Counters/Next1/1" ,
                   ecName.str()+ " - BX=Next1",-1.,(double) ecBin,ecBin+1), condNames);
    label( plot1D( -1.,base.str()+"/L0Conditions/Counters/Next2/1" ,
                   ecName.str()+ " - BX=Next2",-1.,(double) ecBin,ecBin+1), condNames);
    if(ecBin>1)label( plot2D( -1. , -1. , base.str() + "/L0Conditions/Correlations/1" 
                       , "L0DU Elementary Conditions 2D INclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1), condNames);
    if(ecBin>1)label( plot2D( -1. , -1. , base.str() + "/L0Conditions/Correlations/2" 
                       , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1), condNames);
    
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;

      // detail per BX slot
      if( report->conditionValue( id ) ){
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/1",ecName.str()+" - BX=T0",-1.,(double) ecBin,ecBin+1);
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

      
      std::ostringstream sid("");
      sid << id;
      std::string suffix = m_generic ? sid.str() : name;
      AIDA::IHistogram1D* histo = plot1D( m_condSeq[name] , base.str() + "/L0Conditions/Sequence/" + suffix
      , "Decision sequence over 5 BX for -" + name + "- Elementary Condition (TCK " + ttck.str() +")  - LSB = Prev2"    
                                          , 0. , 32. , 32); 
      label(histo,seqNames,false,"v");
      
      // timing summary
      for(int slot = -2 ; slot <=2 ; slot++){
        int dec =   report->conditionValue( id, slot ) ;
        AIDA::IHistogram2D* histo = plot2D( (double) slot , (double) id 
                                            ,  base.str() + "/L0Conditions/Timing/1"
        , "L0Conditions value over +-2 BX around trigger : value(BXslot , L0Conditions) - (TCK "+ttck.str() +")"  
                                            , -2.5  ,2.5 , 0. , (double) ecBin, 5, ecBin, dec);
        label(histo, slotNames, 1,false);
        if(slot == 0)label( histo , condNames , 2,false);
      }
    
      
      
      // correlations
      if( report->conditionValue( id ) ){
        for(LHCb::L0DUElementaryCondition::Map::iterator jt = conditions.begin() ;jt!=conditions.end();jt++){
          int jd = ((*jt).second)->id() ;
          std::string nameY = (*jt).first;
          if( report->conditionValue( jd ) ){
            // Inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Correlations/1" 
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
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Correlations/2" 
                           , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
          }      
        } 
      }
    }  
  }
  // ------------ L0DU Channels --------------
  if(m_chan){
    std::ostringstream tcName("");
    tcName << "L0DU  Channels Decision counters (TCK = " << ttck.str() << ")";
    std::ostringstream fcName("");
    fcName << "L0DU  Channels PreDecision counters (TCK = " << ttck.str() << ")";
    int cBin = channels.size();
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/1",
                   tcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1), chanNames);
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/2", 
                   fcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1), chanNames);
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/Prev2/1",
                   tcName.str()+ " - BX=Prev2", -1. ,(double) cBin  , cBin+1), chanNames);
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/Prev1/1",
                   tcName.str()+ " - BX=Prev1", -1. ,(double) cBin  , cBin+1), chanNames);
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/Next1/1",
                   tcName.str()+ " - BX=Next1", -1. ,(double) cBin  , cBin+1), chanNames);
    label( plot1D( -1.,base.str()+"/L0Channels/Counters/Next2/1",
                   tcName.str()+ " - BX=Next2", -1. ,(double) cBin  , cBin+1), chanNames);
    if(cBin > 1)label( plot2D( -1. , -1. , base.str() + "/L0Channels/Correlations/1" 
                        , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1), chanNames);
    if(cBin > 1)label( plot2D( -1. , -1. , base.str() + "/L0Channels/Correlations/2" 
                        , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1), chanNames);
    
    
    for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;

      // detail per slot
      if( report->channelDecision( id ) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/1" , tcName.str()+ " - BX=T0",-1.,(double) cBin  , cBin+1);
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
        plot1D( (double) id , base.str() + "/L0Channels/Counters/2" 
                 , fcName.str()+ " - BX=T0"    , -1. ,(double) cBin  , cBin+1);      


      std::ostringstream sid("");
      sid << id;
      std::string suffix = m_generic ? sid.str() : name ;
      AIDA::IHistogram1D* histo = plot1D( m_chanSeq[name] , base.str() + "/L0Channels/Sequence/" + suffix
              , "Decision sequence over 5 BX for -" + name + "- channel (TCK " + ttck.str() +") - LSB = Prev2", 0. , 32., 32);
      label(histo, seqNames,false,"v");
      
      // Timing summary
      for(int slot = -2 ; slot <=2 ; slot++){
        int dec =  report->channelDecision( id , slot);
        AIDA::IHistogram2D* histo =  plot2D( (double) slot , (double) id 
                ,  base.str() + "/L0Channels/Timing/1" 
                , "Channel decision  over +-2 BX around trigger : decision(BX slot,L0Channel) - (TCK "+ttck.str() +")"  
                , -2.5  ,2.5 , 0. , (double) cBin, 5, cBin, dec);
        label(histo, slotNames, 1,false);
        if(slot == 0)label( histo , chanNames , 2,false);
      }      
      if(cBin <= 1)continue;
      // correlations
      if( report->channelDecision( id ) ){
        for(LHCb::L0DUChannel::Map::iterator jt = channels.begin() ;jt!=channels.end();jt++){
          int jd = ((*jt).second)->id() ;
          std::string nameY = (*jt).first;
          if( report->channelDecision( jd ) ){
            // inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Correlations/1" 
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
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Correlations/2" 
                           , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
          }
        }
      }
    }
  }
  

  
  // ------------ L0DU Triggers --------------
  if(m_trig && triggers.size() != 0){
    std::ostringstream tName("");
    tName << "L0DU  Trigger Decision counters (TCK = " << ttck.str() << ")";
    int tBin = triggers.size();
    label( plot1D( -1. , base.str() + "/L0Triggers/Counters/1" , 
                   tName.str()+ " - BX=T0", -1. ,(double) tBin  , tBin+1),trigNames);
    label( plot1D( -1. , base.str() + "/L0Triggers/Counters/Prev2/1",
                   tName.str()+" - BX=Prev2", -1. ,(double) tBin , tBin+1),trigNames);
    label( plot1D( -1. , base.str() + "/L0Triggers/Counters/Prev1/1", 
                   tName.str()+" - BX=Prev1", -1. ,(double) tBin , tBin+1),trigNames);
    label( plot1D( -1. , base.str() + "/L0Triggers/Counters/Next1/1",
                   tName.str()+" - BX=Next1", -1. ,(double) tBin , tBin+1),trigNames);
    label( plot1D( -1. , base.str() + "/L0Triggers/Counters/Next2/1", 
                   tName.str()+" - BX=Next2", -1. ,(double) tBin , tBin+1),trigNames);
    if(tBin > 1)label( plot2D( -1. , -1. , base.str() + "/L0Triggers/Correlations/1" 
                        , "L0DU Triggers Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1),trigNames);
    if(tBin > 1)label( plot2D( -1. , -1. , base.str() + "/L0Triggers/Correlations/2" 
                        , "L0DU Triggers Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1),trigNames);
    
    
    for(LHCb::L0DUTrigger::Map::iterator it = triggers.begin();it!=triggers.end();it++){
      std::string name = (*it).first;
      int id = (*it).second->id();
      
      // detail per BX slot
      if( report->triggerDecisionByName( name ) ){
        plot1D( (double) id , base.str() + "/L0Triggers/Counters/1" , tName.str()+ " - BX=T0" ,-1.,(double) tBin, tBin+1);
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

      std::ostringstream sid("");
      sid << id;
      std::string suffix = m_generic ?  sid.str() : name;
      AIDA::IHistogram1D* histo = plot1D( m_trigSeq[name] , base.str() + "/L0Triggers/Sequence/" + suffix
              , "Decision sequence over 5 BX for -" + name + "- trigger (TCK " + ttck.str() +") - LSB = Prev2", 0. , 32., 32);
      label(histo, seqNames,false,"v");
      
      // timing summary
      for(int slot = -2 ; slot <=2 ; slot++){
        int dec =   report->triggerDecisionByName( name , slot);
        AIDA::IHistogram2D* histo = plot2D( (double) slot , (double) id,  base.str() + "/L0Triggers/Timing/1" 
            , "Trigger decision over +-2 BX around trigger : decision(BX slot, L0Trigger) -(TCK " + ttck.str() +")" 
                                            , -2.5  , 2.5 , 0. , (double) tBin, 5, tBin, dec);
        label(histo, slotNames, 1,false);
        if(slot == 0)label( histo , trigNames , 2,false);
      }

      
      if(tBin <= 1)continue;
      // correlations
      if( report->triggerDecisionByName( name ) ){
        for(LHCb::L0DUTrigger::Map::iterator jt = triggers.begin() ;jt!=triggers.end();jt++){
          int jd = (*jt).second->id();
          std::string nameY = (*jt).first;
          if( report->triggerDecision( jd ) ){
            // inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Triggers/Correlations/1" 
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
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Triggers/Correlations/2" 
                            , "L0DU Triggers Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) tBin, -1. ,(double) tBin , tBin+1 , tBin+1);
          }
        }
      }
    }
  }  
  // ------------ Decision  ------------
  if(m_dec){
    std::ostringstream decName("");
    decName << "L0 Decision counters (TCK = " << ttck.str() << ") - Counter/Decision/TTB/FB";
    label(plot1D( -1. , base.str() + "/L0Decision/Counters/1" , decName.str(), -1. ,5  , 6),deciNames);
    if(report->decision(LHCb::L0DUDecision::Physics))
      plot1D( 0. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,5  , 6);
    if(report->decision(LHCb::L0DUDecision::Beam1))
      plot1D( 1. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,5  , 6);
    if(report->decision(LHCb::L0DUDecision::Beam2))
      plot1D( 2. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,5  , 6);
    if(report->timingTriggerBit())plot1D( 3. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,5  , 6);
    if(report->forceBit())        plot1D( 4. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,5  , 6);

    //sequence
    for(int i=-2;i<=2;i++){
      if(report->decisionFromSummary( LHCb::L0DUDecision::Physics, i ))m_chanSeq["Global"] |=  (1 << (i+2) );
    }
    std::ostringstream seqName("");
    seqName << "L0|Physics Decision sequence over 5 BX (TCK = " << ttck.str() << ") - LSB = Prev2";
    AIDA::IHistogram1D* histo = 
      plot1D( (double) m_chanSeq["Global"] , base.str() + "/L0Decision/Sequence/1" , seqName.str(), 0. ,32.  , 32);
    label(histo,seqNames,false,"v");

    // Correlations 
    AIDA::IHistogram2D* h2d = 
      plot2D( 0 , 0,  base.str() + "/L0Decision/Correlations/1" , "Decisions INclusive correlations" , 0., 4. , 0. , 4., 4 , 4);
    AIDA::IHistogram2D* h2d2 = 
      plot2D( 0 , 0,  base.str() + "/L0Decision/Correlations/2" , "Decisions EXclusive correlations" , 0., 4. , 0. , 4., 4 , 4);
    label(h2d,deciNames,2);
    label(h2d,deciNames,1);    
    label(h2d2,deciNames,2);
    label(h2d2,deciNames,1);    

    int ic = 1;
    int jc = 1;
    int idec = 1;
    int jdec = 1;
    while( idec < LHCb::L0DUDecision::Any){
      while( jdec < LHCb::L0DUDecision::Any){
        int kdec = ~(idec|jdec);
        if(report->decision(idec) == report->decision(jdec)){
          plot2D( ic , jc,  base.str() + "/L0Decision/Correlations/1" , "Decisions INclusive correlations" , 0., 4., 0., 3.,4,3);
          if( !report->decision(kdec) )
            plot2D( ic , jc,  base.str() + "/L0Decision/Correlations/2" , "Decisions EXclusive correlations" ,0., 4., 0., 3.,4,3);
        }
        
        jc++;
        jdec = jdec << 1;
      }
      ic++;
      idec = idec << 1;
    }
    
    
    // Timing summary
    for(int slot = -2 ; slot <=2 ; slot++){
      idec = 1;
      ic = 0;
      while( idec < LHCb::L0DUDecision::Any){
        int dec =  report->decisionFromSummary( idec , slot );
        AIDA::IHistogram2D* histo = plot2D( (double) slot , ic , base.str() + "/L0Decision/Timing/1" 
                                          ,"L0 decisions over +-2 BX around trigger : decisions(BX) - (TCK " + ttck.str() +")" 
                                            , -2.5  ,2.5 , 0., 4. , 5 , 4, dec);
        if(idec == 1)label(histo, slotNames, 1,false);
        if(slot==0)label(histo,deciNames,2,false);
        idec = idec << 1;
        ic++;
      }
    }
  }
  return StatusCode::SUCCESS;
}
  



//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUReportMonitor::finalize() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;


  
  info() << "======================================================================== " <<endmsg;
  info() << " L0DUReport Monitoring ran on " << m_evtCntMap.size() <<" Configuration(s) " << endmsg;
  info() << "======================================================================== "  << endmsg;
  info() << " " << endmsg;
  for(std::map<unsigned int,LHCb::L0DUConfig*>::iterator it = m_configs.begin(); it != m_configs.end(); it++){
    unsigned int tck = (*it).first;
    LHCb::L0DUConfig* config =(*it).second;

    m_trigCnt     = m_trigCntMap[tck];
    m_trigRate    = m_trigRateMap[tck];
    m_chanCnt     = m_chanCntMap[tck];
    m_condCnt     = m_condCntMap[tck];
    m_chanRate    = m_chanRateMap[tck];
    m_condRate    = m_condRateMap[tck];
    m_evtCnt      = m_evtCntMap[tck];
    
    std::map<int,double> rate;
    std::map<int,double> eRate;
    int i = 1;
    while( i < LHCb::L0DUDecision::Any){
      m_trigRelRate[i] = m_trigRelRateMap[i][tck];
      m_chanRelRate[i] = m_chanRelRateMap[i][tck];
      m_decCnt[i]  = m_decCntMap[i][tck];
      rate[i] =  ( m_evtCnt > 0 ) ? 100.* m_decCnt[i] / m_evtCnt : 0.;
      eRate[i] = ( m_evtCnt > 0 ) ? 100.* sqrt(m_decCnt[i])/m_evtCnt : 0. ; 
      i = i << 1;
    }
    m_decCnt[LHCb::L0DUDecision::Any]  = m_decCntMap[LHCb::L0DUDecision::Any][tck];
    rate[LHCb::L0DUDecision::Any] =  ( m_evtCnt > 0 ) ? 100.* m_decCnt[LHCb::L0DUDecision::Any] / m_evtCnt : 0.;
    eRate[LHCb::L0DUDecision::Any] = ( m_evtCnt > 0 ) ? 100.* sqrt(m_decCnt[LHCb::L0DUDecision::Any])/m_evtCnt : 0. ; 
      
    info() << "   **************************************************** " << endmsg;    
    info() << "   ***  Trigger Configuration Key : "  << format("0x%04X", tck)  << " (" << tck << ")"<<  endmsg;
    info() << "   ***  Recipe name : '" << config->recipe() << "'" << endmsg;
    info() << "   ***  short description : '" << config->definition() << "'" << endmsg;
    info() << "   **************************************************** " << endmsg;    
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "       The complete algorithm description is : " << endmsg;
      debug() << config->description() << endmsg;
    }    
    info() << " " << endmsg;

    info() << "   ------------------------------------------------------------------- " <<endmsg;
    info() << "   -- L0 Performance on " << m_evtCnt << " events" << endmsg;  
    info() << "   ------------------------------------------------------------------- " <<endmsg;
    int idec = LHCb::L0DUDecision::Any;
    info() << "           *  Accepted L0 for ANY decision type ' : "
           << format( " %8.0f events :  rate = ( %6.2f +- %6.2f) %% ", m_decCnt[idec], rate[idec], eRate[idec] ) 
           << endmsg;
    idec = 1;
    while( idec < LHCb::L0DUDecision::Any){
      info() << "              *  Accepted L0 for decision type = '" << LHCb::L0DUDecision::Name[idec] << "' : "
             << format( " %8.0f events :  rate = ( %6.2f +- %6.2f) %% ", m_decCnt[idec], rate[idec], eRate[idec] ) 
             << endmsg;
      idec = idec << 1;
    }
    

    if( m_trigCnt.size() != 0){
      info() << "   ------------------------ SUBTRIGGERS --------------------------------- " <<endmsg;
      int idec = 1;
      while( idec < LHCb::L0DUDecision::Any){
        int count = config->numberOfTriggers(idec);
        if(count > 0){
          info() << "    -- " << count << " L0DU SubTrigger(s) associated to decision type  = '" 
                 << LHCb::L0DUDecision::Name[idec] 
                 << "'  [OR'ed rate = " << format( " (%6.2f +- %6.2f) %% ", rate[idec], eRate[idec] )<< "] : " 
                 << endmsg;
          
          
          for(CounterMap::iterator it =  m_trigRate.begin(); m_trigRate.end()!=it ; it++){
            std::string name = (*it).first;
            LHCb::L0DUTrigger::Map triggers = config->triggers(); 
            LHCb::L0DUTrigger* trigger = triggers[name]; 
            std::string type = LHCb::L0DUDecision::Name[ trigger->decisionType() ]; 
            if( (trigger->decisionType() & idec ) == 0 )continue;
            std::string tag = type+"|"+name;
            info() << "        --- SubTrigger : "  
                   << format( "%25s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ",
                              tag.c_str(), m_trigCnt[name], m_trigRate[name],m_trigRelRate[idec][name]) 
                   << endmsg;
          }
        } 
        idec = idec << 1;
      }
    }    

    info() << "   ------------------------ CHANNELS --------------------------------- " <<endmsg;
    idec = 1;
    while( idec < LHCb::L0DUDecision::Any){
      int count = config->numberOfChannels(idec);
      if(count > 0){
        info() << "    -- " << count << " L0DU channel(s) associated to decision type  = '" 
               << LHCb::L0DUDecision::Name[idec] 
               << "'  [OR'ed rate = " << format( " (%6.2f +- %6.2f) %% ", rate[idec], eRate[idec] )<< "] : "
               << endmsg;
        for(CounterMap::iterator ic =  m_chanRate.begin(); m_chanRate.end()!=ic ; ic++){
          std::string name = (*ic).first; 
          LHCb::L0DUChannel::Map channels = config->channels(); 
          LHCb::L0DUChannel* channel = channels[name]; 
          std::string type = LHCb::L0DUDecision::Name[ channel->decisionType() ]; 
          if( (channel->decisionType() & idec ) == 0 )continue;
          std::string tag = type+"|"+name;
          info() << "        ---  L0 Channel : " 
                 << format( "%25s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ", 
                            tag.c_str(), m_chanCnt[name], m_chanRate[name] , m_chanRelRate[idec][name]) << endmsg; 
        } 
      }
      idec = idec << 1;
    }     
    // disabled channels
    int count = config->numberOfChannels(LHCb::L0DUDecision::Disable);
    if( count != 0){
      info() << "    -- " << count << " Disabled channel(s) :"<< endmsg;
      for(CounterMap::iterator ic =  m_chanRate.begin(); m_chanRate.end()!=ic ; ic++){  
        LHCb::L0DUChannel::Map channels = config->channels(); 
        std::string name = (*ic).first; 
        LHCb::L0DUChannel* channel = channels[name]; 
        if( channel->decisionType() != LHCb::L0DUDecision::Disable )continue;
        std::string type = LHCb::L0DUDecision::Name[ channel->decisionType() ]; 
        info() << "        --- L0 channel : "  
               << format( "%25s :  %8.0f events : rate = %6.2f  %%   ", 
                          (type+"|"+name).c_str(), m_chanCnt[name], m_chanRate[name]) << endmsg;  
      } 
    }
    
    
    info() << "   ------------------------ CONDITIONS ------------------------------ " <<endmsg; 
    for(CounterMap::iterator ic =  m_condRate.begin(); m_condRate.end()!=ic ; ic++){ 
      std::string name = (*ic).first; 
    info() << "   *  Elementary Condition  " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_condCnt[name], m_condRate[name]) << endmsg;
    }
    info() << "======================================================================== " <<endmsg;
  }
   return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
void L0DUReportMonitor::label( AIDA::IHistogram1D* h1d , std::map<int,std::string> labels,bool count , std::string opt) {
  if( !m_swap)return;
  TH1D* th1d = Gaudi::Utils::Aida2ROOT::aida2root( h1d );
  TAxis* xAxis = th1d->GetXaxis();
  int offset = 0;
  if(count){
    xAxis->SetBinLabel( 1 , "Counter" );
    offset = 1;
  }
  int nx = 1;
  for ( std::map<int,std::string>::iterator i  = labels.begin() ; i != labels.end() ; i++) {
    if( nx+offset <= xAxis->GetNbins())xAxis->SetBinLabel( nx+offset , (*i).second.c_str() );
    nx++;
  }
  if(opt !="")xAxis->LabelsOption( opt.c_str() );
  else xAxis->LabelsOption( m_lab.c_str() );
}
void L0DUReportMonitor::label( AIDA::IHistogram2D* h2d , std::map<int,std::string> labels,  int mask, bool count,std::string opt){
  if( !m_swap)return;
  TH2D* th2d = Gaudi::Utils::Aida2ROOT::aida2root( h2d );
  TAxis* xAxis = th2d->GetXaxis();
  TAxis* yAxis = th2d->GetYaxis();
  int offset = 0;
  if(count){
    if(mask == 1 || mask == 3)xAxis->SetBinLabel( 1 , "Counter" );
    if(mask == 2 || mask == 3)yAxis->SetBinLabel( 1 , "Counter" );
    offset = 1;
  }
  
  int nx=1;
  int ny=1;
  for ( std::map<int,std::string>::iterator i  = labels.begin() ; i != labels.end() ; i++) {
    if(mask == 1 || mask == 3){
      if(nx+offset <= xAxis->GetNbins() )xAxis->SetBinLabel( nx+offset , (*i).second.c_str() );
      nx++;
    }
    if(mask == 2 || mask == 3){
      if( ny+offset <= yAxis->GetNbins() )yAxis->SetBinLabel( ny+offset , (*i).second.c_str() );
      ny++;
    }
  }
  if(mask == 1 || mask == 3){
    if(opt !="")xAxis->LabelsOption( opt.c_str() );
    else xAxis->LabelsOption( m_lab.c_str() );
  }
}
