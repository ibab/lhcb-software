// Include files 

// from Event
#include "Event/L0DUReport.h"

// local
#include "L0Filter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Filter
//
// L0 decision(s) Filtering
//
//  configuration properties:
//
//   * FILTERING DECISION : 
//      - TriggerBit : the decision bit to be used  L0 / TimingTrigger bit / Force bit [def = L0 ]
//
//   * IF TriggerBit = "L0"
//     - Decision mask : Physics=1 / Beam1=2 / Beam2=4 / Any=7 [ def = Physics ]
//
//     - orChannels :  select the (TCK-dependent) list of L0channels [ def = {} ]
//            syntax :   orChannels += { [TCK] : { [CHANNELS] }
//                       TCK = 'ALL'  or 'OTHER' is allowed
//                       CHANNEL = 'ANY'  (filter on global decision(mask) ) or 'PASS' (filter anyway) is allowed
//                eg : orChannels += { "0x1810" : {"CALO","MUON"},
//                                     "0x2009" : {"ANY"},
//                                     "OTHER"  : {"PASS} };
//
//                     orChannels += { "ALL" : {"PASS} }  ==> NO FILTERING
//
//                     orChannels += { "0x1810" : {"PASS} , "OTHER" : {} }  ==> FILTERING on TCK =0x1810
//
//
//
//
//     - orSubTriggers : select the list of subTriggers  [ def = {} ]
//            TCK-independent predefined subTriggers :    'L0Ecal'/'L0Hcal'/'L0Muon'/'Other'
//            syntax :  orSubTrigger += { "L0Ecal" };  
//
//     - if orChannels and orSubTriggers are EMPTIES : filter on global decision ( ==>  orChannels += {"ALL" : {"ANY"} };
//
//   
//
//
//    + Revert : revert the filtering (default: event is ACCEPTED according to the filtering criteria )
//
// 2010-05-26 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Filter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Filter::L0Filter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator )
  ,m_count(0)
  ,m_sel(0)
{
  m_l0channels.clear();
  declareProperty("L0DULocation"   , m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("OrChannels"     , m_l0channels );       // TCK-dependent
  declareProperty("OrSubTriggers"  , m_l0triggers );       // TCK-independent
  declareProperty("TriggerBit"     , m_trig = "L0" );      // decision bit (L0/TTB/FB)
  declareProperty("L0DecisionMask" , m_mask = LHCb::L0DUDecision::Physics ); // L0 decision type (Physics/Beam1/Beam2)
  declareProperty("Revert"         , m_revert = false );
}
//=============================================================================
// Destructor
//=============================================================================
L0Filter::~L0Filter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Filter::initialize() {
  StatusCode sc = L0AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) )  debug() << "==> Initialize" << endmsg;

  if( "Timing" == m_trig){
    info() << "Will require that L0DU 'Timing Trigger Bit' is fired " << endmsg;
  }else if( "Force" == m_trig) { 
    info() << "Will require that the L0DU 'Force Bit' is fired " << endmsg;
  }else if( "L0" == m_trig){
    info() << "Will require that : " << endmsg;

    if ( !m_l0channels.empty()) {
      info() << " - any of : " << endmsg;
      for (std::map<std::string, std::vector<std::string> >::const_iterator c = m_l0channels.begin() ; 
            c != m_l0channels.end() ; ++c ) {
        info() << "         -  TKC = " << (*c).first << ": Channels = " << (*c).second << "  " << endmsg;
      }
      info() << " L0 channel(s) is passed "<< endmsg;
      if ( !m_l0triggers.empty()) info() << " OR " << endmsg;
    }

    if ( !m_l0triggers.empty()) {
      info() << " - any of : ["  ;
      for ( std::vector<std::string>::const_iterator c = m_l0triggers.begin() ; 
            c != m_l0triggers.end() ; ++c ) {
        info() << *c << " " ;
      } 
      info() << "] L0 trigger(s) is passed "<< endmsg;
    } 

    if( m_l0triggers.empty() && m_l0channels.empty() )
      info() << " - trigger decision type " << LHCb::L0DUDecision::Name[ m_mask ] << " is passed " << endmsg;

  }
  else{
    error() << "The 'TriggerBit' property must be either : " << endmsg;
    error() << "   - 'L0'     : standard L0DU decision " <<endmsg;
    error() << "   - 'Timing' : L0DU timing trigger bit decision" <<endmsg;
    error() << "   - 'Force'  : L0DU force bit decision " <<endmsg;
    return StatusCode::FAILURE;
  }

  std::string def = ( m_revert ) ? "REJECT" : "ACCEPT" ;
  info() << "---- > TO " << def << " THE EVENT" << endmsg;

  return StatusCode::SUCCESS;
} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Filter::execute() {

  m_count++;

  bool accept = m_revert ? false : true;
  setFilterPassed( !accept ); // switch off by default
  std::string acc = m_revert ? "rejected " : "accepted";
  std::string rej = m_revert ?  "accepted" : "rejected " ;
  std::string loc = dataLocation( m_l0Location );


  const LHCb::L0DUReport* l0 = getIfExists<LHCb::L0DUReport>( loc );
  if( NULL == l0 ){
    counter("Report not found") += 1;
    return Warning("L0DUReport not found at location : '" + loc + "' - the event is " + rej, StatusCode::SUCCESS );
  }


  // current TCK
  int tck = l0->tck();
  std::ostringstream sTck("");
  sTck <<  format("0x%04X" , tck );

  

  // Timing Trigger decision
  if( "Timing" == m_trig  ){
    if( l0->timingTriggerBit() )setFilterPassed( accept );
  }
  
  // Force decision
  if( "Force" == m_trig  ){
    if( l0->forceBit() )setFilterPassed( accept );
  }

  if( "L0" != m_trig)return StatusCode::FAILURE;

  // standard L0DU decision

  if ( !m_l0channels.empty()) {

    for ( std::map<std::string,std::vector<std::string> >::const_iterator cc = m_l0channels.begin() ; 
          cc != m_l0channels.end() ; ++cc ) {

      std::string              sT  = (*cc).first;  // TCK
      std::vector<std::string> sCs = (*cc).second; // [Channels]

      // keep only requested tck
      bool pass = false;
      if( sT == sTck.str() )pass = true;   // channel selection applied on the requested TCK
      if( toUpper( sT ).find("ANY")   != std::string::npos  )pass = true;  // ... on any TCK
      if( toUpper( sT ).find("ALL")   !=  std::string::npos )pass = true;  // ... on any TCK
      if( toUpper( sT ).find("OTHER") !=  std::string::npos && isAnother( sTck.str() ) )pass = true;  // .. on each other TCK
      if( !pass)continue;

      for ( std::vector<std::string>::const_iterator c = sCs.begin(); c != sCs.end() ; ++c ) {
        std::string ch = *c;
        
        if( (toUpper( ch ).find("ANY") != std::string::npos || toUpper( ch ).find("ALL") !=  std::string::npos ) 
            && l0->decision(m_mask ) ){
          setFilterPassed(accept);
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Triggered event pass anyway" << endmsg ;
          break;
        }else if (toUpper( ch ).find("PASS") !=  std::string::npos){
          setFilterPassed(accept) ;
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event pass anyway" << endmsg ;
          break;
        }
        else if (l0->channelDecisionByName( ch )){
          setFilterPassed( accept );
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " << ch << " Channel" << endmsg ;
        } else {
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by " << ch << " Channel" << endmsg ; 
        }
      }
    }
  }
  
  if ( !m_l0triggers.empty()) {
    for ( std::vector<std::string>::const_iterator c = m_l0triggers.begin() ; 
          c != m_l0triggers.end() ; ++c ) {
      if (l0->triggerDecisionByName(*c)){
        setFilterPassed( accept );
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " 
                                                 << *c << " SubTrigger" << endmsg ;
      } else if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by " 
                                                    << *c << " SubTrigger" << endmsg ; 
    }
  }  

  if ( m_l0triggers.empty() && m_l0channels.empty() ){
    if( l0->decision(m_mask) ){
      if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by L0 decision " 
                                             << LHCb::L0DUDecision::Name[m_mask] <<endmsg ;
      setFilterPassed( accept );
    } else {
      setFilterPassed( !accept );
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Event is rejected by L0 decision " 
                << LHCb::L0DUDecision::Name[m_mask] <<endmsg ;
    }
  }



  if ( filterPassed()){
    counter("L0Filter accept") += 1;
    if ( msgLevel(MSG::DEBUG))debug() << "Event is " << acc << endmsg ;
  }else{
    counter("L0Filter reject") += 1;
    if ( msgLevel(MSG::DEBUG))debug() << "Event is " << rej  << endmsg ;
  }
  if(filterPassed())m_sel++; 

  return StatusCode::SUCCESS; 
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Filter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << "Filtering : " << m_sel << " events among " << m_count << " processed " << endmsg;
  return L0AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================



std::string L0Filter::toUpper(std::string str){
  std::string uStr( str );
  std::transform( str.begin() , str.end() , uStr.begin () , ::toupper ) ;
  return uStr;
}

bool L0Filter::isAnother(std::string tck){
  bool other = true;
  for ( std::map<std::string,std::vector<std::string> >::const_iterator cc = m_l0channels.begin() ; 
        cc != m_l0channels.end() ; ++cc ) {  
    if( tck == (*cc).first ){
      other = false;
      break;
    } 
  }
  return other;
}
