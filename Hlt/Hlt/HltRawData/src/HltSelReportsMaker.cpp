// $Id: HltSelReportsMaker.cpp,v 1.27 2010-08-17 08:45:05 graven Exp $
// #define DEBUGCODE
// Include files 
#include "boost/algorithm/string/replace.hpp"
#include "boost/format.hpp"
#include "boost/foreach.hpp"
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IRegistry.h"

#include "Kernel/Particle2Vertex.h"

#include "Event/HltSelReports.h"
#include "Event/HltObjectSummary.h"
#include "Event/HltDecReports.h"

#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Event/HltStage.h"
#include "Event/HltCandidate.h"


//#include "Kernel/CaloCellCode.h"

#include "HltBase/HltSelection.h"
// local
#include "HltSelReportsMaker.h"



using namespace LHCb;


namespace {
    template <typename E> const ContainedObject* getFirst(const Hlt::Selection* sel ) {
        if ( sel->classID() != E::classID() ) return 0;
        const Hlt::TSelection<E>& tsel = dynamic_cast<const Hlt::TSelection<E>&>(*sel);
        return *tsel.begin();
    }
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelReportsMaker
//
// 2008-07-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelReportsMaker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelReportsMaker::HltSelReportsMaker( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_hltANNSvc (0)
  , m_hltSvc(0)
  , m_regSvc(0)
  , m_inspectionSvc(0)
  , m_intPVSelID(0)
  , m_presentInfoLevelTrack(kMaxInfoLevel)
  , m_presentInfoLevelRecVertex(kMaxInfoLevel)
  , m_presentInfoLevelParticle(kMaxInfoLevel)
  , m_presentInfoLevelCaloCluster(kMaxInfoLevel)
  , m_presentInfoLevelSelection(kMaxInfoLevel)
{

  declareProperty("OutputHltSelReportsLocation",
    m_outputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("MuonIDSuffix", m_muonIDSuffix = "");
  declareProperty("InputHltDecReportsLocation",
    m_inputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);  


  declareProperty("DebugEventPeriod",m_debugPeriod = 0 );

  declareProperty("MaxCandidatesDecision", m_maxCandidatesDecision = 1000);
  declareProperty("MaxCandidatesDecisionDebug", m_maxCandidatesDecisionDebug = 5000);
  
  declareProperty("MaxCandidatesNonDecision", m_maxCandidatesNonDecision = 0);
  declareProperty("MaxCandidatesNonDecisionDebug", m_maxCandidatesNonDecisionDebug = 5000);
  
  declareProperty("SelectionMaxCandidates", m_maxCandidates );
  declareProperty("SelectionMaxCandidatesDebug", m_maxCandidatesDebug );

  // changed on 2010/7/17  declareProperty("InfoLevelDecision", m_infoLevelDecision = ((unsigned int)kStandardInfoLevel) );
  declareProperty("InfoLevelDecision", m_infoLevelDecision = ((unsigned int)kMinInfoLevel ));
  declareProperty("InfoLevelDecisionDebug", m_infoLevelDecisionDebug = ((unsigned int)kMaxInfoLevel) );
  
  declareProperty("InfoLevelNonDecision", m_infoLevelNonDecision = ((unsigned int)kMinInfoLevel) );
  declareProperty("InfoLevelNonDecisionDebug", m_infoLevelNonDecisionDebug = ((unsigned int)kMaxInfoLevel) );
  
  declareProperty("SelectionInfoLevel", m_infoLevel );
  declareProperty("SelectionInfoLevelDebug", m_infoLevelDebug );

  declareProperty("InfoLevelTrack", m_infoLevelTrack = ((unsigned int)kMaxInfoLevel) );
  declareProperty("InfoLevelTrackDebug", m_infoLevelTrackDebug = ((unsigned int)kMaxInfoLevel) );

  declareProperty("InfoLevelRecVertex", m_infoLevelRecVertex = ((unsigned int)kMaxInfoLevel) );
  declareProperty("InfoLevelRecVertexDebug", m_infoLevelRecVertexDebug = ((unsigned int)kMaxInfoLevel) );

  declareProperty("InfoLevelParticle", m_infoLevelParticle = ((unsigned int)kMaxInfoLevel) );
  declareProperty("InfoLevelParticleDebug", m_infoLevelParticleDebug = ((unsigned int)kMaxInfoLevel) );

  declareProperty("InfoLevelCaloCluster", m_infoLevelCaloCluster = ((unsigned int)kMaxInfoLevel) );
  declareProperty("InfoLevelCaloClusterDebug", m_infoLevelCaloClusterDebug = ((unsigned int)kMaxInfoLevel) );

  declareProperty("InfoLevelSelection", m_infoLevelSelection = ((unsigned int)kMaxInfoLevel) );
  declareProperty("InfoLevelSelectionDebug", m_infoLevelSelectionDebug = ((unsigned int)kMaxInfoLevel) );

  declareProperty("SuppressPostscale", m_SuppressPostscale = true  );
  declareProperty("SuppressPostscaleDebug", m_SuppressPostscaleDebug = false  );

  declareProperty("DebugIncident", m_debugIncidentName = "RequestDebugEvent" );


  m_debugMode=false;
  m_event=0;

}

// add incident listener, and set m_event 
void HltSelReportsMaker::handle ( const Incident& /*incident*/ ) {
    m_event = 0u-1; // yes, it is unsigned, but all we need is
                    // ++m_event to be zero to force a debug event;-)
}


//=============================================================================
// Destructor
//=============================================================================
HltSelReportsMaker::~HltSelReportsMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelReportsMaker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");
  m_hltSvc = svc<Hlt::IData> ( "Hlt::Service" , true ) ;
  m_regSvc = svc<Hlt::IRegister> ( "Hlt::Service" , true ) ;
  m_inspectionSvc = svc<Hlt::IInspector> ( "Hlt::Service" , true ) ;

  if (!m_debugIncidentName.empty() ) {
    svc<IIncidentSvc>("IncidentSvc")->addListener(this,m_debugIncidentName);
  }


 // get string-to-int selection ID map
  typedef std::map<IANNSvc::minor_key_type,IANNSvc::minor_mapped_type> map_t;
  map_t selectionNameToIntMap;
  BOOST_FOREACH( const IANNSvc::minor_value_type& p, m_hltANNSvc->items("Hlt1SelectionID") ) {
      selectionNameToIntMap.insert( p );
  }
  BOOST_FOREACH( const IANNSvc::minor_value_type& p, m_hltANNSvc->items("Hlt2SelectionID") ) {
      selectionNameToIntMap.insert( p );
  }
 
  // initialize maps
  m_selectionInfo.clear();

  // get trigger selection names 
  Hlt::IInspector::KeyList selectionIDs;
  m_inspectionSvc->selections( selectionIDs );
  Hlt::IRegister::Lock lock(m_regSvc,this);
  for ( Hlt::IInspector::KeyList::const_iterator is = selectionIDs.begin(); is!=selectionIDs.end();++is) {
     struct selectionInfo info;
     info.id = *is;
     if (lock->registerInput(info.id,this).isFailure()) {
       Warning( " selectionName="+*is+" could not register as input. Skipped. " , StatusCode::SUCCESS, 10 );
       continue;
     }
     map_t::const_iterator im = selectionNameToIntMap.find( *is );
     if( im==selectionNameToIntMap.end() ){
       Warning( " selectionName="+*is+" not found in HltANNSvc. Skipped. " , StatusCode::SUCCESS, 10 );
       continue;
     }
     info.intId =  im->second ;
     info.selection = m_hltSvc->selection(info.id,this);
     if (info.selection==0) {
       Warning( " selectionName="+*is+" not found in HltDataSvc. Skipped. " , StatusCode::SUCCESS, 10 );
       continue;
     }
     m_debugMode = true;  info.maxCandDebug = maximumNumberOfCandidatesToStore( *is );
     m_debugMode = false; info.maxCand      = maximumNumberOfCandidatesToStore( *is );
     m_selectionInfo.push_back(info);

  }

  m_infoIntToName.clear();
  std::vector<IANNSvc::minor_value_type> hltinfos = m_hltANNSvc->items("InfoID"); 
  for( std::vector<IANNSvc::minor_value_type>::const_iterator i= hltinfos.begin();i!=hltinfos.end();++i){
    if( ( 0<= i->second ) && ( i->second<=65535 ) ){
      m_infoIntToName.insert( i->second, i->first );
    }
  }
 
  m_lastPVSelectionName = "";  
  m_intPVSelID=0;

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // determine output mode 
  ++m_event;
  m_debugMode = ( m_event == m_debugPeriod );
  if( m_debugMode ) m_event =0;    

  m_HLTmuonTracks=0;  // will get them only if needed

  // create output container and put it on TES
  HltSelReports* outputSummary = new HltSelReports();
  put( outputSummary, m_outputHltSelReportsLocation );

  // create output container for Object Summaries and put it on TES
  m_objectSummaries = new HltObjectSummary::Container();
  put( m_objectSummaries, m_outputHltSelReportsLocation.value() + "/Candidates" );
  
  const HltDecReports* decReports(0);
  // get input
  decReports = getIfExists<HltDecReports>(m_inputHltDecReportsLocation);
  if( NULL == decReports )
  {  
    Warning( " No HltDecReports at " + m_inputHltDecReportsLocation.value(), StatusCode::SUCCESS, 10 );
  }


#ifdef DEBUGCODE
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() <<" Selection Names Found =" ;
    for( std::vector<Gaudi::StringKey>::const_iterator i=selectionIDs.begin();i!=selectionIDs.end();++i){
      verbose() << i->str() << " ";
    }
    verbose() << endmsg;
  }
#endif  
 

  // data compression requires that we store objects from early processing stages first
  // order selections accordingly 
  //     filtering of selections for persistency also happens in this loop

  std::vector< RankedSelection > sortedSelections;
 
  // loop over selection summaries
  for( std::vector<selectionInfo>::const_iterator is=m_selectionInfo.begin();is!=m_selectionInfo.end();++is){
     const std::string selName(is->id.str()); 

     // see if marked for persistency
     int maxCandidates = m_debugMode ? is->maxCandDebug :  is->maxCand;
     if( !maxCandidates )continue;

     // find first candidate for ranking
     const Hlt::Selection* sel = is->selection;

#ifdef DEBUGCODE
     if ( msgLevel(MSG::VERBOSE) ){
       verbose() << " Selection " << selname <<  " found in dataSvc decison=" << sel->decision() << endmsg;          
     }
#endif
     // unsuccessful selections can't save candidates
     if( !sel->decision() )continue;

     // first see if we have have
     // HltSelReportsMaker                      WARNING HltSelReportsMaker::  Unsupported data type CLID=7561 - skip selection ID=Hlt1TrackAllL0Decision
     // EventSelector                           SUCCESS Reading Event record 401. Record number within stream 1: 401
     const ContainedObject* candidate = 0;
     if ( sel->classID() == Hlt::Candidate::classID() ) {
        const Hlt::TSelection<Hlt::Candidate>& csel = dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>(*sel);
        const Hlt::Candidate* cand = *csel.begin();
        if (cand!=0) {
            const Hlt::Stage* stage = cand->currentStage();
            // let's first try track...
            candidate = stage->get<LHCb::Track>();
            if (candidate==0)  candidate = stage->get<LHCb::RecVertex>();
            if (candidate==0)  Warning("Stage is neither track nor vertex", StatusCode::SUCCESS,10);
            
        }

     } 
     if (candidate==0)      candidate = getFirst<LHCb::Track>(sel);
     if (candidate==0)      candidate = getFirst<LHCb::RecVertex>(sel);
     if (candidate==0)      candidate = getFirst<LHCb::Particle>(sel);
     if (candidate==0)      candidate = getFirst<LHCb::CaloCluster>(sel);
     if (candidate==0)      {
       
       std::ostringstream mess;
       mess << " Unsupported data type CLID=" <<  sel->classID() << " - skip selection ID=" +selName;
       Warning( mess.str(),StatusCode::SUCCESS, 10 );
       continue;
       
     }

     // empty selections have nothing to save
     if( !candidate )continue;
       
#ifdef DEBUGCODE
        if ( msgLevel(MSG::VERBOSE) ){
          verbose() << "Selection " << selName <<  " has candidates " << endmsg;
        } 
#endif

    // save selection ---------------------------
 
     setPresentInfoLevel( selName );

     // classify according to first candidate
     int rnk(0);
     // go through all cases of supported candidates
     if (  !rank<Track>       (candidate, rnk)
        && !rank<RecVertex>   (candidate, rnk)
        && !rank<Particle>    (candidate, rnk)
        && !rank<CaloCluster> (candidate, rnk) )
         Warning( " Unsupported data type among candidates, CLID= " 
                  + boost::lexical_cast<std::string>(sel->classID()) + " - skip selection ID=" 
                          +selName,StatusCode::SUCCESS, 2 );

     sortedSelections.push_back( std::make_pair(rnk, *is ));
  }
  
  if( 0==sortedSelections.size() )return StatusCode::SUCCESS;

  std::sort( sortedSelections.begin(), sortedSelections.end(), rankSelLess() );

#ifdef DEBUGCODE
  if (msgLevel(MSG::VERBOSE)) {
  for( std::vector<RankedSelection>::const_iterator is=sortedSelections.begin(); is!=sortedSelections.end(); ++is ){
      verbose() << is-sortedSelections.begin() << " Selection " << is->second.id << " rank " << is->first << endmsg;
  }
  }
#endif


  // loop over ranked selection summaries in HltSummary
  for( std::vector<RankedSelection>::const_iterator is=sortedSelections.begin(); is!=sortedSelections.end(); ++is ){
     //info() << " Selection " << is->second.id << " rank " << is->first << endmsg;
    
     const Gaudi::StringKey name(is->second.id);
     const std::string selName(name.str());     
     // don't bother if duplicate selection 
     if( outputSummary->hasSelectionName(selName) ){
         Warning( " duplicate selection ignored selectionName=" + selName,StatusCode::SUCCESS, 10 );
         continue;        
     }

     if( ( !m_debugMode && m_SuppressPostscale ) ||  ( m_debugMode && m_SuppressPostscaleDebug ) ){
     // must also check its decision in HltDecReports since it might have been killed by postscale
       if( decReports ){
         const HltDecReport* decReport = decReports->decReport(selName);
         if( decReport ){
           if( !(decReport->decision()) )continue;
         }
       }
     }

     
     const Hlt::Selection* sel = is->second.selection;

     // unsuccessful selections can't save candidates
     if( !sel->decision() )continue;

     std::vector<const ContainedObject*> candidates; candidates.reserve( sel->size() );
     
     //TODO: make it less spaghetti like...
     if( sel->classID() == Hlt::Candidate::classID() ) {
       const Hlt::TSelection<Hlt::Candidate>& csel = dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>(*sel);
       for (Hlt::TSelection<Hlt::Candidate>::const_iterator it = csel.begin(); it != csel.end(); ++it) {
         const Hlt::Candidate* cand = *it;
         const Hlt::Stage* stage = cand->currentStage();
         const LHCb::Track *trk = stage->get<LHCb::Track>();
         if (trk!=0) {
            candidates.push_back( trk );
         } else {
            const LHCb::RecVertex *vtx = stage->get<LHCb::RecVertex>();
            if (vtx!=0) { 
                candidates.push_back( vtx );
            } else {
                Warning( " Unsupported data type in stage - skip selection ID=" +selName,StatusCode::SUCCESS, 10 );
                continue;
            }
         }
       }

     } else if( sel->classID() == LHCb::Track::classID() ) {
       
       const Hlt::TrackSelection& tsel = dynamic_cast<const Hlt::TrackSelection&>(*sel);   
       for (Hlt::TrackSelection::const_iterator it = tsel.begin(); it != tsel.end(); ++it) {
         candidates.push_back( *it );
       }

     } else if( sel->classID() == LHCb::RecVertex::classID() ) {

       const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);   
       for (Hlt::VertexSelection::const_iterator it = tsel.begin(); it != tsel.end(); ++it) {
         candidates.push_back( *it );
       }

     } else if( sel->classID() == LHCb::Particle::classID() ) {

       const Hlt::TSelection<LHCb::Particle>& psel = dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>(*sel);   
       for (Hlt::TSelection<LHCb::Particle>::const_iterator it = psel.begin(); it != psel.end(); ++it) {
        candidates.push_back( *it );
       }

     } else {
       
       Warning( " Unsupported data type  - skip selection ID=" 
                +selName,StatusCode::SUCCESS, 10 );
       continue;
       
     }
       

     // trim number of candidates if too large
     int noc = candidates.size();
     int maxCandidates = m_debugMode ? is->second.maxCandDebug :  is->second.maxCand;
     noc = (noc>maxCandidates)?maxCandidates:noc;


     // save selection ---------------------------


     HltObjectSummary selSumOut;    
     selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
     
     // integer selection id 
     selSumOut.addToInfo("0#SelectionID",float(is->second.intId));
    
     setPresentInfoLevel( selName );
  
     // must also save candidates 
     int nocc(0);

     // save PV association info with selection if found
     std::string pvSelectionName("");
     std::vector<unsigned int> packedPVKeys;
     unsigned int nPackedPVKeys(0);
     unsigned int iWord(0);
     Particle2Vertex::Table* table(0);

     for (std::vector<const ContainedObject*>::const_iterator ic = candidates.begin();
          ic != candidates.end(); ++ic) {


       ++nocc;
       if( nocc>noc )break;

       const HltObjectSummary* hos(0);
       // go through all cases of supported candidates
       hos = store<LHCb::Track>(*ic);
       if (hos==0) hos = store<LHCb::RecVertex>(*ic);
       if (hos==0) {
           hos = store<LHCb::Particle>(*ic);
           if (hos!=0) {//TODO: move into store_<Particle> ??? Oops, this shares info amongst candidates...
                        //      that bit needs to be moved out of this loop, and 'bound' into the relevant store_...
             if( kStandardInfoLevel &  m_presentInfoLevelSelection & m_presentInfoLevel ){ 
               if( pvSelectionName != "_" ){
                 bool keyFound = false;             
                 const DataObject* container = (*ic)->parent();
                 if( container ){
                   IRegistry* registry = container->registry() ;
                   if( registry ){
                     std::string path = registry->identifier() ;
                     boost::replace_last(path,"/Particles","/Particle2VertexRelations");
                     if( !table )
                     {
                       table = getIfExists< Particle2Vertex::Table >(path);
                     }
                     if( table ){                     
                       Particle2Vertex::Table::Range rels= table->relations( dynamic_cast<const LHCb::Particle*>(*ic) );
                       if( rels.size() ){
                         const VertexBase* pv = (rels[0]).to();
                         if(  !(pvSelectionName.length()) ){
                           const DataObject* containerPV = pv->parent();
                           if( containerPV ){
                             IRegistry* registryPV = containerPV->registry() ;
                             if( registryPV ){
                               std::string pathPV = registryPV ->identifier();
                               std::size_t found = pathPV.find_last_of("/");
                               if( found != std::string::npos ){
                                 pvSelectionName = pathPV.substr(found+1);
                               } else {
                                 pvSelectionName = pathPV;
                               }
                             }
                           }
                         }
                         unsigned int key( pv->key() );
                         key = ( key < 255 ) ? key : 254;
                         ++nPackedPVKeys;
                         if( 5 == nPackedPVKeys ){
                           nPackedPVKeys = 1;
                           packedPVKeys.push_back( iWord );
                           iWord = 0;
                         }
                         iWord |= ( key << ((nPackedPVKeys-1)*8) ) ;
                         keyFound = true;                     
                       }
                     }
                   }
                 }
                 if( !keyFound && pvSelectionName.length() ){
                   Warning("Only some particles in the trigger selection " +
                           selName + " have PV association - none stored", 
                           StatusCode::SUCCESS, 10 );                 
                   pvSelectionName = "_";               
                 }
               }
             }
             
           } 
        }
        if (hos==0) hos = store<LHCb::CaloCluster>(*ic);
        if (hos==0) {
               Warning(" Unsupported data type in Hlt Trigger Summary - skip remaining candidates too, selection ID=" 
                       +selName,StatusCode::SUCCESS, 10 );
               break;       
        }
       if( !hos ){
         Warning(" Could not store supported candidate - skip remaining candidates too ",StatusCode::SUCCESS, 10 );
         break;
       } 
       selSumOut.addToSubstructure(hos);
     }

     // insert PV relations if any
     if( pvSelectionName.length()  ){
       packedPVKeys.push_back( iWord );
       int intPVSelID=0;       
       if( pvSelectionName == m_lastPVSelectionName ){
         intPVSelID = m_intPVSelID;
       } else {
         std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID");
         for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt1.begin();
              si!=hlt1.end();++si){
           if( si->first == pvSelectionName ){
             intPVSelID=si->second;
             m_lastPVSelectionName = pvSelectionName;
             m_intPVSelID = intPVSelID;
             break;
           }
         }
         if( !intPVSelID ){           
           std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
           for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt2.begin();
                si!=hlt2.end();++si){
             if( si->first == pvSelectionName ){
               intPVSelID=si->second;
               m_lastPVSelectionName = pvSelectionName;
               m_intPVSelID = intPVSelID;
               break;
             }
           }           
         }         
       }
       if( !intPVSelID ){       
         Warning("PV selection " + pvSelectionName + 
                 " found via Particle2Vertex for " + selName +
                 " has unregistered name - no associations stored ",
                 StatusCode::SUCCESS, 10 );                 
       } else {
         selSumOut.addToInfo("10#" + pvSelectionName ,float(intPVSelID));
         bool notZero = false;         
         for( std::vector<unsigned int>::iterator pWord = packedPVKeys.begin();
              pWord != packedPVKeys.end(); ++pWord ){
           if( *pWord ){
             notZero = true;
             break;
           }
         }
         // store PV keys only if at least one was not zero
         if( notZero ){           
           int j=0;         
           for( std::vector<unsigned int>::iterator pWord = packedPVKeys.begin();
                pWord != packedPVKeys.end(); ++pWord, ++j ){
             std::stringstream ss;
             ss << "11#" << boost::format("%1$=08X") % j;
             union IntFloat { unsigned int mInt; float mFloat; };
             IntFloat a; a.mInt = *pWord;
             selSumOut.addToInfo( ss.str(), a.mFloat );              
           }         
         }         
       }
     }

     // insert selection into the container
     if( outputSummary->insert(selName,selSumOut) == StatusCode::FAILURE ){
       Warning(" Failed to add Hlt selection name "+selName
               +" to its container ",StatusCode::SUCCESS, 10 );

     }    
     
  }
  
  // -------------------------------------------------------------------------------------
  // clone HltObjectSummary-s of selection and insert them into object store (needed for raw data storage
  // and global selections)
  // -------------------------------------------------------------------------------------
  for( HltSelReports::Container::const_iterator i=outputSummary->begin();i!=outputSummary->end();++i){
    const HltObjectSummary& sHos= i->second;  
    HltObjectSummary* hos= new HltObjectSummary( sHos );    
    m_objectSummaries->push_back(hos);
  }


  // -------------------------------------------------------------------------------------
  // create global selections ------------------------------------------------------------
  // -------------------------------------------------------------------------------------
  if( !outputSummary->hasSelectionName("Hlt1Global") ){

     HltObjectSummary selSumOut;    
     selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
     
     // integer selection id 
     selSumOut.addToInfo("0#SelectionID",float(kHlt1GlobalID));

     // see which decisions contributed to it
     for( HltSelReports::Container::const_iterator it=outputSummary->begin();it!=outputSummary->end();++it){
       const std::string & selName = it->first;
       if( ( selName.find("Hlt1") == 0 ) && 
           ( selName.find("Decision") != std::string::npos ) ) {         
         const LHCb::HltObjectSummary& selRep = it->second;
         // must find corresponding HltObjectSummary in the object store
         HltObjectSummary::Info::const_iterator i = selRep.numericalInfo().find("0#SelectionID");
         if( i!=selRep.numericalInfo().end() ){               
           int id = (int)(i->second+0.1);
           for( HltObjectSummary::Container::const_iterator pObj=m_objectSummaries->begin();
                pObj!=m_objectSummaries->end();++pObj){
             if( (*pObj)->summarizedObjectCLID()!=1 )continue; 
             HltObjectSummary::Info::const_iterator j=(*pObj)->numericalInfo().find("0#SelectionID");
             if( j!=(*pObj)->numericalInfo().end() ){
               if( id == (int)(j->second+0.1) ){
                 // must also check its decision in HltDecReports since it might have been killed by postscale
                 if( decReports ){
                   const HltDecReport* decReport = decReports->decReport(selName);
                   if( decReport ){
                     if( !(decReport->decision()) )break;
                   }
                 }                 
                 selSumOut.addToSubstructure( (const SmartRef<HltObjectSummary>)(*pObj) );
                 break;                     
               }
             } else {
               Error(" (2) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
             }
           }
         } else {               
           Error(" (1) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
         }
       }
     }
     if( selSumOut.substructure().size() ){
       
       // insert selection into the container
       if( outputSummary->insert("Hlt1Global",selSumOut) == StatusCode::FAILURE ){
         Error( "  Failed to add Hlt selection name Hlt1Global to its container ", StatusCode::SUCCESS, 10 );
       } 
       HltObjectSummary* hos= new HltObjectSummary(  selSumOut );    
       m_objectSummaries->push_back(hos);
       
       
     }
  }

  if( !outputSummary->hasSelectionName("Hlt2Global") ){

     HltObjectSummary selSumOut;    
     selSumOut.setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
     
     // integer selection id 
     selSumOut.addToInfo("0#SelectionID",float(kHlt2GlobalID));

     // see which decisions contributed to it
     for( HltSelReports::Container::const_iterator it=outputSummary->begin();it!=outputSummary->end();++it){
       const std::string & selName = it->first;
       if( ( selName.find("Hlt2") == 0 ) && 
           ( selName.find("Decision") != std::string::npos ) ) {         
         const LHCb::HltObjectSummary& selRep = it->second;
         // must find corresponding HltObjectSummary in the object store
         HltObjectSummary::Info::const_iterator i = selRep.numericalInfo().find("0#SelectionID");
         if( i!=selRep.numericalInfo().end() ){               
           int id = (int)(i->second+0.1);
           for( HltObjectSummary::Container::const_iterator pObj=m_objectSummaries->begin();
                pObj!=m_objectSummaries->end();++pObj){
             if( (*pObj)->summarizedObjectCLID()!=1 )continue; 
             HltObjectSummary::Info::const_iterator j=(*pObj)->numericalInfo().find("0#SelectionID");
             if( j!=(*pObj)->numericalInfo().end() ){
               if( id == (int)(j->second+0.1) ){
                 // must also check its decision in HltDecReports since it might have been killed by postscale
                 if( decReports ){
                   const HltDecReport* decReport = decReports->decReport(selName);
                   if( decReport && !(decReport->decision()) )break;
                 }                 
                 selSumOut.addToSubstructure( (const SmartRef<HltObjectSummary>)(*pObj) );
                 break;                     
               }
             } else {
               Error(" (2) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
             }
           }
         } else {               
           Error(" (1) HltObjectSummary of selection-summary-type has no SelectionID info ",StatusCode::SUCCESS,20);
         }
       }
     }
     if( selSumOut.substructure().size() ){
       
       // insert selection into the container
       if( outputSummary->insert("Hlt2Global",selSumOut) == StatusCode::FAILURE ){
         Error( "  Failed to add Hlt selection name Hlt2Global to its container ", StatusCode::SUCCESS, 10 );
       } 
       m_objectSummaries->push_back( new HltObjectSummary( selSumOut ) );    
       
       
     }
  }

  // ----------------- printout ------------------------
  if ( msgLevel(MSG::VERBOSE) ){

   verbose() << " ======= HltSelReports size= " << outputSummary->size() << endmsg;  
   verbose() << *outputSummary << endmsg;

   verbose() << " ======= HltObjectSummary container size= " << m_objectSummaries->size() << endmsg;
   for( HltObjectSummary::Container::const_iterator ppHos=m_objectSummaries->begin();
        ppHos!=m_objectSummaries->end();++ppHos){
     const HltObjectSummary* pHos=*ppHos;    
     verbose() << " key " << pHos->index();
     std::vector<std::string> selby = outputSummary->selectedAsCandidateBy(pHos);
     if( selby.size() ){
       verbose() << " selectedAsCandidateBy=";       
       for( std::vector<std::string>::const_iterator i=selby.begin();i!=selby.end();++i){
         verbose()  << *i << " ";
       }
       std::pair<std::string,int> pvInfo = outputSummary->pvSelectionNameAndKey(pHos);
       if( pvInfo.second > -1 ){
         verbose() << " pvSelectionName= " << pvInfo.first << " pvKey= " << pvInfo.second << " ";
       }
     }     
     verbose() << *pHos << endmsg;    
   }
   
  }
  
  return StatusCode::SUCCESS;
}

// -------------------------------------
// store Track in HltObjectSummary store
// -------------------------------------
const LHCb::HltObjectSummary* HltSelReportsMaker::store_(const LHCb::Track* object)
{
  if( !object )return (HltObjectSummary*)(0);  
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }
  // not yet in object store; create new one
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);
 
  // find mutations of the same object
  HltObjectSummary::Info theseInfo = infoToSave( hos );
  std::vector<LHCbID> theseHits = object->lhcbIDs() ;
  std::vector<LHCbID>::const_iterator rbegin1=theseHits.end(); --rbegin1;
  std::vector<LHCbID>::const_iterator rend1=theseHits.begin(); --rend1;        
  
  //    look for objects with the same lhcbids
  //      if many then the one which leaves least amount info on this one
  int smallestSize = theseInfo.size();
  const HltObjectSummary* pBestHos(0);

  //   couldn't get reverse operator to work
  HltObjectSummarys::const_iterator rbegin=m_objectSummaries->end();  --rbegin;
  HltObjectSummarys::const_iterator rend  =m_objectSummaries->begin(); --rend;  
  for( HltObjectSummarys::const_iterator ppHos=rbegin;ppHos!=rend;--ppHos){
    const HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      std::vector<LHCbID> otherHits = pHos->lhcbIDsFlattened();      
      bool different=false;      
      if( otherHits.size() == theseHits.size() ){
        std::vector<LHCbID>::const_iterator h2=otherHits.end(); --h2;        
        for( std::vector<LHCbID>::const_iterator h1=rbegin1;h1!=rend1;--h1,--h2){
          if( h1->lhcbID() != h2->lhcbID() ){
            different=true;
            break;            
          }
        }
        if( different )continue;
        // found object of the same type with the same lhcbid content!
        // now check if its Info content is a subset of this one
        HltObjectSummary::Info otherInfo = pHos->numericalInfoFlattened(); 
        if( otherInfo.size() <= theseInfo.size() ){
          bool notcontained=false;          
          for( HltObjectSummary::Info::const_iterator i1=otherInfo.begin();i1!=otherInfo.end();++i1 ){
            HltObjectSummary::Info::const_iterator i2 = theseInfo.find( i1->first );
            if( i2==theseInfo.end() ){
              notcontained = true;
              break;
            }
            if( i1->second != i2->second ){
              notcontained = false;
              break;
            }
          }
          if( notcontained )continue;
          if( otherInfo.size() == theseInfo.size() ){
            smallestSize=0;
            pBestHos=pHos;
            // don't need to look any more since subobject has all the info
            break;            
          } else {
            int presentSize = theseInfo.size()-otherInfo.size();
            if( presentSize < smallestSize ){
              smallestSize = presentSize;
              pBestHos=pHos;
            }
          }
        }
      }
    }
  }
  // found subobject
  if( pBestHos ){
    if( smallestSize==0 ){      
      theseInfo.clear();
    } else {
        HltObjectSummary::Info otherInfo = pBestHos->numericalInfoFlattened(); 
        for( HltObjectSummary::Info::const_iterator i1=otherInfo.begin();i1!=otherInfo.end();++i1 ){
          theseInfo.erase( i1->first );
        }
    }
    // lhcbids are stored at the botton object
    theseHits.clear();
    // set subobject link
    hos->addToSubstructure( pBestHos );          
  }
  
  hos->setLhcbIDs( theseHits );
  hos->setNumericalInfo( theseInfo );

  m_objectSummaries->push_back(hos);
  return hos;  
}


// -------------------------------------
// store RecVertex in HltObjectSummary store
// -------------------------------------
const LHCb::HltObjectSummary* HltSelReportsMaker::store_(const LHCb::RecVertex* object)
{  
  if( !object )return (HltObjectSummary*)(0);  
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }  
  // not yet in object store; create new one
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);

  // find mutations of the same object
  HltObjectSummary::Info theseInfo = infoToSave( hos );

  SmartRefVector<LHCb::HltObjectSummary> thisSubstructure;  
  const SmartRefVector<LHCb::Track> & tracks = object->tracks();
  for( SmartRefVector<LHCb::Track>::const_iterator it = tracks.begin();it != tracks.end(); ++it) {
    thisSubstructure.push_back( store_( *it) );
  }
  SmartRefVector<LHCb::HltObjectSummary>::const_iterator rbegin1=thisSubstructure.end(); --rbegin1;
  SmartRefVector<LHCb::HltObjectSummary>::const_iterator rend1=thisSubstructure.begin(); --rend1;        

  //    look for objects with the same lhcbids
  //      if many then the one which leaves least amount info on this one
  int smallestSize = theseInfo.size();
  const HltObjectSummary* pBestHos(0);

  //   couldn't get reverse operator to work
  HltObjectSummarys::iterator rbegin=m_objectSummaries->end();  --rbegin;
  HltObjectSummarys::iterator rend  =m_objectSummaries->begin(); --rend;  
  for( HltObjectSummarys::iterator ppHos=rbegin;ppHos!=rend;--ppHos){
    HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      SmartRefVector<LHCb::HltObjectSummary> otherSubstructure = pHos->substructureFlattened();      
      bool different=false;      
      if( otherSubstructure.size() == thisSubstructure.size() ){
        SmartRefVector<LHCb::HltObjectSummary>::const_iterator h2=otherSubstructure.end(); --h2;        
        for( SmartRefVector<LHCb::HltObjectSummary>::const_iterator h1=rbegin1;h1!=rend1;--h1,--h2){
          if( (*h1) != (*h2) ){
            different=true;
            break;            
          }
        }
        if( different )continue;
        // found object of the same type with the same (flattened) substructure!
        // now check if its Info content is a subset of this one
        HltObjectSummary::Info otherInfo = pHos->numericalInfoFlattened(); 
        if( otherInfo.size() <= theseInfo.size() ){
          bool notcontained=false;          
          for( HltObjectSummary::Info::const_iterator i1=otherInfo.begin();i1!=otherInfo.end();++i1 ){
            HltObjectSummary::Info::const_iterator i2 = theseInfo.find( i1->first );
            if( i2==theseInfo.end() ){
              notcontained = true;
              break;
            }
            if( i1->second != i2->second ){
              notcontained = false;
              break;
            }
          }
          if( notcontained )continue;
          if( otherInfo.size() == theseInfo.size() ){
            smallestSize=0;
            pBestHos=pHos;
            // don't need to look any more since subobject has all the info
            break;            
          } else {
            int presentSize = theseInfo.size()-otherInfo.size();
            if( presentSize < smallestSize ){
              smallestSize = presentSize;
              pBestHos=pHos;
            }
          }
        }
      }
    }
  }
  // found subobject
  if( pBestHos ){
    if( smallestSize==0 ){      
      theseInfo.clear();
    } else {
        HltObjectSummary::Info otherInfo = pBestHos->numericalInfoFlattened(); 
        for( HltObjectSummary::Info::const_iterator i1=otherInfo.begin();i1!=otherInfo.end();++i1 ){
          theseInfo.erase( i1->first );
        }
    }
    // set subobject link
    thisSubstructure.clear();
    // set substructure link
    thisSubstructure.push_back( pBestHos );    
  }
   
  hos->setSubstructure( thisSubstructure );
  hos->setNumericalInfo( theseInfo );
  m_objectSummaries->push_back(hos);
  return hos;  
}

// -------------------------------------------
// store CaloCluster in HltObjectSummary store
// -------------------------------------------
const LHCb::HltObjectSummary* HltSelReportsMaker::store_(const LHCb::CaloCluster* object)
{
  if( !object )return (HltObjectSummary*)(0);  
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }  
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);
  
  std::vector<LHCbID> clusterSeed; clusterSeed.push_back(object->seed());  
  hos->setLhcbIDs( clusterSeed );

  m_objectSummaries->push_back(hos);
  return hos;  
}


// ----------------------------------------
// store Particle in HltObjectSummary store
// ----------------------------------------
const LHCb::HltObjectSummary* HltSelReportsMaker::store_(const LHCb::Particle* object)
{  
  if( !object )return (HltObjectSummary*)(0);  
  for( HltObjectSummarys::const_iterator ppHos=m_objectSummaries->begin();
       ppHos!=m_objectSummaries->end();++ppHos){
    const HltObjectSummary* pHos(*ppHos);    
    if( pHos->summarizedObjectCLID() == object->clID() ){
      if( pHos->summarizedObject() == object ){
        return pHos;
      }
    }
  }  
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);  

  HltObjectSummary::Info theseInfo = infoToSave( hos );

  // no subobject code implemented since so fat Hlt2 does not save intermediate selections

  std::vector<const LHCb::Particle*> daughters = object->daughtersVector();
  if( daughters.size() >0 ){    
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); 
         p!=daughters.end(); ++p){
      hos->addToSubstructure( store_( *p ) );
    }
  } else {
    // particles with no daughters have substructure via ProtoParticle 
    // we don't save Protoparticles, only things they lead to
    const ProtoParticle* pp = object->proto();
    if( !pp ){
      std::string w = " Particle with no daughters and no protoparticle ";
      const ObjectContainerBase *p = object->parent();
      if (p) { 
          IRegistry* r = p->registry();
          if (r) {
              w+= " with id " ; w+= r->identifier();
          } else {
              w += " in " ; w+= p->name() ; 
          }
      }
      w+= ", skipped";
      Warning(w,StatusCode::SUCCESS, 10 );
    } else {
      const Track* track=pp->track();
      if( track ){
        // charged track particle
        hos->addToSubstructure( store_( track ) );
        // if muon add muon stub too        
        const LHCb::MuonPID* muid = pp->muonPID();
        if( muid!=0 && object->particleID().abspid()==13 ){
          if( muid->IsMuon() ){
            const LHCb::Track*  muStub=muid->muonTrack();
            if (!muStub ){
              if( !m_HLTmuonTracks )
              { 
                //Now we need to derive the muon segment container from
                //the track container
                m_HltMuonTracksLocation = "";
                if (track->parent()) {
                  const DataObject* obj = track->parent() ;
                  if ( 0 != obj ){
                    const IRegistry* reg = obj->registry() ;
                    if ( 0 != reg ){
                      m_HltMuonTracksLocation = reg->identifier() + m_muonIDSuffix;
                    }
                  }
                }
                m_HLTmuonTracks = getIfExists<LHCb::Tracks>(m_HltMuonTracksLocation);
                if( NULL==m_HLTmuonTracks) 
                {
                  
                  Warning(" Found Particle which is a muon but no muon tracks at " + m_HltMuonTracksLocation.value() 
                          ,StatusCode::SUCCESS,10 );
                }
              }
              if( m_HLTmuonTracks ){          
                muStub= m_HLTmuonTracks->object(track->key());     
              }
            }
            if( muStub ){
              hos->addToSubstructure( store_( muStub ) );
            } else {
              Warning(" Found Particle which is a muon but could not reach muon lhcbids "
                          ,StatusCode::SUCCESS,10 );
            }            
          }
        }
      } else {
        // neutral particle ?
        // Ecal via CaloHypo
        const SmartRefVector< LHCb::CaloHypo > &caloVec = pp->calo();
        if( caloVec.size() > 0 ){
          const LHCb::CaloHypo*   hypo  = *(caloVec.begin());
          if( LHCb::CaloHypo::Photon == hypo->hypothesis() ){
            // Photon
            const CaloCluster* cluster = *(hypo->clusters().begin());          
            hos->addToSubstructure( store_( cluster ) );
          } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
            // Split Photons
            const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
            const LHCb::CaloHypo* g1 = *(hypos.begin() );
            const CaloCluster* cluster1 = *(g1->clusters().begin());
            hos->addToSubstructure( store_( cluster1 ) );
            const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
            const CaloCluster* cluster2 = *(g2->clusters().begin());
            hos->addToSubstructure( store_( cluster2 ) );
          }        
        }
      }
    }    
  }
  
  hos->setNumericalInfo( theseInfo );
  m_objectSummaries->push_back(hos);
  return hos;  
}
 



//=============================================================================
// fetch info to save; to find object follow summerizedObject pointer
//=============================================================================
HltObjectSummary::Info HltSelReportsMaker::infoToSave( const HltObjectSummary* hos ) const
{
  HltObjectSummary::Info infoPersistent;
  if( kMinInfoLevel == m_presentInfoLevel )return infoPersistent; 

  if( !hos )return infoPersistent;  
  if( !(hos->summarizedObject()) )return infoPersistent;  
  switch( hos->summarizedObjectCLID() )
  {
  case LHCb::CLID_Track:
    {      
      const Track* candi = dynamic_cast<const Track*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kExtraInfoLevel &  m_presentInfoLevelTrack & m_presentInfoLevel ){ 
	const Track::ExtraInfo & eInfo = candi->extraInfo();
	for( Track::ExtraInfo::const_iterator ei=eInfo.begin();ei!=eInfo.end();++ei){
          if( ( 0<= ei->first ) && ( ei->first<=65535 ) ){
	    GaudiUtils::VectorMap< int, std::string >::const_iterator i=m_infoIntToName.find(ei->first);
	    if( i!=m_infoIntToName.end() ){
	      infoPersistent.insert( i->second, float( ei->second ) );
	    }
	  }
	}
      }
      if( kStandardInfoLevel &  m_presentInfoLevelTrack & m_presentInfoLevel ){ 
        if( candi->nStates() ){
          const State & firstState = candi->firstState();
          infoPersistent.insert( "0#Track.firstState.z", float( firstState.z() ) );
          infoPersistent.insert( "1#Track.firstState.x", float( firstState.x() ) );
          infoPersistent.insert( "2#Track.firstState.y", float( firstState.y() ) );
          infoPersistent.insert( "3#Track.firstState.tx", float( firstState.tx() ) );
          infoPersistent.insert( "4#Track.firstState.ty", float( firstState.ty() ) );
          infoPersistent.insert( "5#Track.firstState.qOverP", float( firstState.qOverP() ) );
          infoPersistent.insert( "6#Track.chi2PerDoF", float( candi->chi2PerDoF() ) );
          infoPersistent.insert( "7#Track.nDoF", float( candi->nDoF() ) );
        }
      }    
    }    
    break;
  case LHCb::CLID_RecVertex:
    {      
      const RecVertex* candi = dynamic_cast<const RecVertex*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kExtraInfoLevel &  m_presentInfoLevelRecVertex & m_presentInfoLevel ){
	const RecVertex::ExtraInfo & eInfo = candi->extraInfo();
	for( RecVertex::ExtraInfo::const_iterator ei=eInfo.begin();ei!=eInfo.end();++ei){
          if( ( 0<= ei->first ) && ( ei->first<=65535 ) ){
	    GaudiUtils::VectorMap< int, std::string >::const_iterator i=m_infoIntToName.find(ei->first);
	    if( i!=m_infoIntToName.end() ){
	      infoPersistent.insert( i->second, float( ei->second ) );
	    }
	  }
	}
      }
      if( kStandardInfoLevel &  m_presentInfoLevelRecVertex & m_presentInfoLevel ){ 
        infoPersistent.insert( "0#RecVertex.position.x", float( candi->position().x() ) );
        infoPersistent.insert( "1#RecVertex.position.y", float( candi->position().y() ) );
        infoPersistent.insert( "2#RecVertex.position.z", float( candi->position().z() ) );
      }      
    }    
    break;
  case LHCb::CLID_Particle:
    {      
      const Particle* candi = dynamic_cast<const Particle*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kExtraInfoLevel &  m_presentInfoLevelParticle & m_presentInfoLevel ){
	const Particle::ExtraInfo & eInfo = candi->extraInfo();
	for( Particle::ExtraInfo::const_iterator ei=eInfo.begin();ei!=eInfo.end();++ei){
          if( ( 0<= ei->first ) && ( ei->first<=65535 ) ){
	    GaudiUtils::VectorMap< int, std::string >::const_iterator i=m_infoIntToName.find(ei->first);
	    if( i!=m_infoIntToName.end() ){
	      infoPersistent.insert( i->second, float( ei->second ) );
	    }
	  }
	}
      }
      if( kStandardInfoLevel &  m_presentInfoLevelParticle & m_presentInfoLevel ){ 
        infoPersistent.insert( "0#Particle.particleID.pid", float( candi->particleID().pid() ) );
        infoPersistent.insert( "1#Particle.measuredMass", float( candi->measuredMass() ) );
        infoPersistent.insert( "2#Particle.referencePoint.z", float( candi->referencePoint().z() ) );
        infoPersistent.insert( "3#Particle.referencePoint.x", float( candi->referencePoint().x() ) );
        infoPersistent.insert( "4#Particle.referencePoint.y", float( candi->referencePoint().y() ) );
        infoPersistent.insert( "5#Particle.slopes.x", float( candi->slopes().x() ) );
        infoPersistent.insert( "6#Particle.slopes.y", float( candi->slopes().y() ) );
        double p = candi->p();
        if( p < 1E-9 )p=1E-9;        // Note: p is _not_ signed for a Particle. Charge is part of pid...
        infoPersistent.insert( "7#Particle.1/p", float( 1.0/p ) );
      }      
    }
    break;
  case LHCb::CLID_CaloCluster:
    {      
      const CaloCluster* candi = dynamic_cast<const CaloCluster*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kStandardInfoLevel &  m_presentInfoLevelCaloCluster & m_presentInfoLevel ){ 
        infoPersistent.insert( "0#CaloCluster.e", float( candi->e() ) );
        infoPersistent.insert( "1#CaloCluster.position.x", float( candi->position().x() ) );
        infoPersistent.insert( "2#CaloCluster.position.y", float( candi->position().y() ) );
        infoPersistent.insert( "3#CaloCluster.position.z", float( candi->position().z() ) );
      }      
    }
    break;
  default:
    { }    
  }
  return infoPersistent;
}

int HltSelReportsMaker::rank_(const LHCb::Track* object) const
{
  if( !object )return 0;
  HltObjectSummary hos;
  hos.setSummarizedObjectCLID( object->clID() );
  hos.setSummarizedObject(object);  
  int rank=100*rankLHCbIDs( object->lhcbIDs() ) +  infoToSave( &hos ).size();
  return rank;
}

int HltSelReportsMaker::rank_(const LHCb::CaloCluster* object) const
{
  if( !object )return 0;
  HltObjectSummary hos;
  hos.setSummarizedObjectCLID( object->clID() );
  hos.setSummarizedObject(object);  
  std::vector<LHCbID> clusterSeed; clusterSeed.push_back(object->seed()); 
  int rank=100*rankLHCbIDs( clusterSeed ) +  infoToSave( &hos ).size();
  return rank;
}

int HltSelReportsMaker::rank_(const LHCb::RecVertex* object) const
{
  if( !object )return 0;
  HltObjectSummary hos;
  hos.setSummarizedObjectCLID( object->clID() );
  hos.setSummarizedObject(object);  
  int rank=10000 +  infoToSave( &hos ).size();
  return rank;
}

int HltSelReportsMaker::rank_(const LHCb::Particle* object) const
{
  if( !object )return 0;
  HltObjectSummary  hos;
  hos.setSummarizedObjectCLID( object->clID() );
  hos.setSummarizedObject(object);  
  int rank=100000 +  infoToSave( &hos ).size();
  return rank;
}

int HltSelReportsMaker::rankLHCbIDs( const std::vector<LHCbID> & lhcbIDs ) const
{
  int rank(0);
  for(  std::vector<LHCbID>::const_iterator i= lhcbIDs.begin();i!= lhcbIDs.end();++i){
    const LHCbID id= *i;    
    if( id.isVelo() ){
      rank = rank | 8;
    } else if( id.isTT() ){
      rank = rank | 16;      
    } else if ( id.isOT() || id.isIT() ){
      rank = rank | 32;      
    } else if ( id.isMuon() ) {
      rank = rank | 4;      
    } else if ( id.isCalo() ) {
      rank = rank | 2;      
    } else {
      rank = rank | 1;
    }
  }
  return rank;  
}

    

//=============================================================================
// qualify trigger selections for persistency
//=============================================================================
unsigned int HltSelReportsMaker::maximumNumberOfCandidatesToStore( const std::string & selName  ) const
{
  if( m_debugMode ){
    // DEBUG MODE

    // set max number of candidates
    const SelectionSetting & maxCandidatesDebug =  m_maxCandidatesDebug.value();    
    for( SelectionSetting::const_iterator i=maxCandidatesDebug.begin();
         i!=maxCandidatesDebug.end(); ++i){
      if( selName == i->first ) return i->second;
    }
    
    // not set specifically for this selection ; use default
    return ( selName.find("Decision")!=std::string::npos ) ?
              m_maxCandidatesDecisionDebug :
              m_maxCandidatesNonDecisionDebug;

  } else {
    
    // NORMAL MODE

    // set max number of candidates
    const SelectionSetting & maxCandidates =  m_maxCandidates.value();    
    for( SelectionSetting::const_iterator i=maxCandidates.begin();
         i!=maxCandidates.end(); ++i){
      if( selName == i->first ) return i->second;
    }
    
    // not set specifically for this selection ; use default
    return ( selName.find("Decision")!=std::string::npos ) ?
              m_maxCandidatesDecision :
              m_maxCandidatesNonDecision;
  }

}

//=============================================================================
// set output info level for this selection
//=============================================================================
void HltSelReportsMaker::setPresentInfoLevel( const std::string & selName )
{
  if( m_debugMode ){
    // DEBUG MODE

    m_presentInfoLevelTrack = m_infoLevelTrackDebug;
    m_presentInfoLevelRecVertex = m_infoLevelRecVertexDebug;
    m_presentInfoLevelParticle = m_infoLevelParticleDebug;
    m_presentInfoLevelCaloCluster = m_infoLevelCaloClusterDebug;
    m_presentInfoLevelSelection = m_infoLevelSelectionDebug;
    // set max number of candidates
    const SelectionSetting & infoLevelDebug =  m_infoLevelDebug.value();    
    for( SelectionSetting::const_iterator i=infoLevelDebug.begin();
         i!=infoLevelDebug.end(); ++i){
      if( selName == i->first ){
        m_presentInfoLevel = i->second;
        return;        
      }
    } 
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      m_presentInfoLevel = m_infoLevelDecisionDebug;
      return;      
    }
    m_presentInfoLevel = m_infoLevelNonDecisionDebug;
    return;    

  } else {
    
    // NORMAL MODE

    m_presentInfoLevelTrack = m_infoLevelTrack;
    m_presentInfoLevelRecVertex = m_infoLevelRecVertex;
    m_presentInfoLevelParticle = m_infoLevelParticle;
    m_presentInfoLevelCaloCluster = m_infoLevelCaloCluster;
    m_presentInfoLevelSelection = m_infoLevelSelection;
    // set max number of candidates
    const SelectionSetting & infoLevel =  m_infoLevel.value();    
    for( SelectionSetting::const_iterator i=infoLevel.begin();
         i!=infoLevel.end(); ++i){
      if( selName == i->first ){
        m_presentInfoLevel = i->second;
        return;        
      }
    }
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      m_presentInfoLevel = m_infoLevelDecision;
      return;      
    }
    m_presentInfoLevel = m_infoLevelNonDecision;
    return;    
  }

}

    

bool HltSelReportsMaker::rankSelLess::operator()( const HltSelReportsMaker::RankedSelection & elem1, 
                                                  const HltSelReportsMaker::RankedSelection & elem2) const
{  
    using namespace std;
    if( elem1.first < elem2.first )return true;
    if( elem1.first > elem2.first )return false;
    // equal ranks; now use selection name; decisions come last

    if( elem1.second.id.str().find("Decision")!=std::string::npos ){
      if( elem2.second.id.str().find("Decision")!=std::string::npos ){
        size_t l1 = elem1.second.id.str().length();
        size_t l2 = elem2.second.id.str().length();
        return (l1!=l2) ? (l1 < l2) : (elem1.second.id.str() < elem2.second.id.str()) ;
      }
      return false;
    } 
    if( elem2.second.id.str().find("Decision")!=std::string::npos ){
      return true;
    } 
    size_t l1 = elem1.second.id.str().length();
    size_t l2 = elem2.second.id.str().length();
    return (l1!=l2) ? (l1 < l2) : ( elem1.second.id.str() < elem2.second.id.str() );
}
