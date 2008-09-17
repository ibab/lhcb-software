// $Id: HltSelReportsMaker.cpp,v 1.2 2008-09-17 16:14:56 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltSummary.h"
#include "Event/HltSelReports.h"
#include "Event/HltObjectSummary.h"

#include "Event/RecVertex.h"
#include "Event/Particle.h"

#include "Kernel/CaloCellCode.h"

// local
#include "HltSelReportsMaker.h"

using namespace LHCb;


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
  : HltBaseAlg ( name , pSvcLocator )
{

  declareProperty("InputHltSummaryLocation",
    m_inputHltSummaryLocation= LHCb::HltSummaryLocation::Default);  
  declareProperty("OutputHltSelReportsLocation",
    m_outputHltSelReportsLocation= LHCb::HltSelReportsLocation::Default);  
  declareProperty("HltMuonTrackLocation",
                  m_HltMuonTracksLocation =  LHCb::TrackLocation::HltMuon);


  declareProperty("DebugEventPeriod",m_debugPeriod = 0 );

  declareProperty("MaxCandidatesDecision", m_maxCandidatesDecision = 100);
  declareProperty("MaxCandidatesDecisionDebug", m_maxCandidatesDecisionDebug = 500);
  
  declareProperty("MaxCandidatesNonDecision", m_maxCandidatesNonDecision = 0);
  declareProperty("MaxCandidatesNonDecisionDebug", m_maxCandidatesNonDecisionDebug = 500);
  
  declareProperty("SelectionMaxCandidates", m_maxCandidates );
  declareProperty("SelectionMaxCandidatesDebug", m_maxCandidatesDebug );

  declareProperty("InfoLevelDecision", m_infoLevelDecision = ((unsigned int)kStandardInfoLevel) );
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

  m_hltANNSvc = 0;

  m_debugMode=0;
  m_event=0;

}
//=============================================================================
// Destructor
//=============================================================================
HltSelReportsMaker::~HltSelReportsMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelReportsMaker::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // determine output mode 
  ++m_event;
  if( m_event == m_debugPeriod ){
    // debug
    m_debugMode = 1;
    m_event =0;    
  } else {
    // normal
    m_debugMode = 0;
  }

  m_HLTmuonTracks=0;  // will get them only if needed

  // create output container and put it on TES
  HltSelReports* outputSummary = new HltSelReports();
  put( outputSummary, m_outputHltSelReportsLocation );

  // create output container for Object Summaries and put it on TES
  m_objectSummaries = new HltObjectSummary::Container();
  put( m_objectSummaries, m_outputHltSelReportsLocation + "/Candidates" );
  

  // get input selection summary
  if( !exist<HltSummary>(m_inputHltSummaryLocation) ){    
    Warning( " No HltSummary at "+ m_inputHltSummaryLocation,StatusCode::SUCCESS, 50 );
    return StatusCode::SUCCESS;  
  }  
  HltSummary* inputSummary = get<HltSummary>(m_inputHltSummaryLocation);


  // selection names in HltSummary
  std::vector<std::string> selIDs = inputSummary->selectionSummaryIDs();
  if( 0==selIDs.size() )return StatusCode::SUCCESS;
  
  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  if( selIDs.size() ){
    //    std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
    //    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );
    std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
    std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );
  }

  // data compression requires that we store objects from early processing stages first
  // order selections accordingly 
  //     filtering of selections for persistency also happens in this loop

  std::vector< RankedSelection > sortedSelections;
 
  // loop over selection summaries in HltSummary
  for( std::vector<std::string>::const_iterator is=selIDs.begin();is!=selIDs.end();++is){
     const std::string selName(*is);     
     const LHCb::HltSelectionSummary& selSumIn = inputSummary->selectionSummary(selName);

     // unsuccessful selections can't save candidates
     if( !selSumIn.decision() )continue;

     // number of candidates
     int noc = selSumIn.data().size();
     if( !noc ){ noc=selSumIn.particles().size();   }      
     // empty selections have nothing to save
     if( !noc )continue;
  
    // see if marked for persistency
     int maxCandidates = maximumNumberOfCandidatesToStore( selName );      
     if( !maxCandidates )continue;

    // don't bother if duplicate selection 
     if( outputSummary->hasSelectionName(selName) ){
         Warning( " duplicate selection ignored selectionName=" + selName,StatusCode::SUCCESS, 20 );
         continue;        
     }

     // save selection ---------------------------

     // int selection id
     int intSelID(0);   
     for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
          si!=selectionNameToIntMap.end();++si){
       if( si->first == selName ){
         intSelID=si->second;
         break;
       }
     }
     if( !intSelID ){
       Warning( " selectionName="+selName+" from HltSummary not found in HltANNSvc. Skipped. " , StatusCode::SUCCESS, 20 );
       continue;
     }
 
     setPresentInfoLevel( selName );
     

     // classify according to first candidate
     int rank(0);
     const std::vector<ContainedObject*>& candidates = selSumIn.data();
     if( candidates.size() ){        
       std::vector<ContainedObject*>::const_iterator ic = candidates.begin();
       // go through all cases of supported candidates
       Track* candi = dynamic_cast<Track*>(*ic);
       if( candi ){
            rank = rankTrack( candi );
       } else {
         RecVertex* candi = dynamic_cast<RecVertex*>(*ic);
         if( candi ){
           rank = rankRecVertex( candi );
         } else {
           Particle* candi = dynamic_cast<Particle*>(*ic);
           if( candi ){
             rank = rankParticle( candi );
           } else { 
             CaloCluster* candi = dynamic_cast<CaloCluster*>(*ic);
             if( candi ){
               rank = rankCaloCluster( candi );
             } else { 
               Warning( " Unsupported data type in Hlt Trigger Summary - skip selection ID=" 
                       +selName,StatusCode::SUCCESS, 20 );
               continue;    
             }
           }
         }
       }
     } else {
       // Particles as Smart Ref
       const SmartRefVector< LHCb::Particle > & candidates = selSumIn.particles();       
       if( candidates.size() > 0 ){
         SmartRefVector<LHCb::Particle>::const_iterator  ic = candidates.begin();
         const Particle* candi = *ic;
         if( candi ){
           rank = rankParticle( candi );
         }
       }
     }     
     sortedSelections.push_back( RankedSelection(rank,selName) );     
  }

  std::sort( sortedSelections.begin(), sortedSelections.end(), rankSelLess() );
  

  if( 0==sortedSelections.size() )return StatusCode::SUCCESS;


  // loop over ranked selection summaries in HltSummary
  for( std::vector<RankedSelection>::const_iterator is=sortedSelections.begin(); is!=sortedSelections.end(); ++is ){
    //    info() << " Selection " << is->second << " rank " << is->first << endmsg;
    
     const std::string selName(is->second);     
     const LHCb::HltSelectionSummary& selSumIn = inputSummary->selectionSummary(selName);

     // unsuccessful selections can't save candidates
     if( !selSumIn.decision() )continue;


     // number of candidates
     int noc = selSumIn.data().size();
     if( !noc ){ noc=selSumIn.particles().size();   }      
     // empty selections have nothing to save
     if( !noc )continue;


     // see if marked for persistency
     int maxCandidates = maximumNumberOfCandidatesToStore( selName );      
     if( !maxCandidates )continue;
     
     // trim number of candidates if too large
     noc = (noc>maxCandidates)?maxCandidates:noc;

     // don't bother if duplicate selection 
     if( outputSummary->hasSelectionName(selName) ){
         Warning( " duplicate selection ignored selectionName=" + selName,StatusCode::SUCCESS, 20 );
         continue;        
     }

     // save selection ---------------------------

     // int selection id
     int intSelID(0);   
     for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
          si!=selectionNameToIntMap.end();++si){
       if( si->first == selName ){
         intSelID=si->second;
         break;
       }
     }
     if( !intSelID ){
       Warning(" selectionName="+selName+" from HltSummary not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
       continue;
     }
   
     HltObjectSummary* selSumOut = new HltObjectSummary();    
     selSumOut->setSummarizedObjectCLID( 1 ); // use special CLID for selection summaries (lowest number for sorting to the end)
     
     // integer selection id 
     selSumOut->addToInfo("0#SelectionID",float(intSelID));
    
     setPresentInfoLevel( selName );
  
    {
      // must also save candidates 
      const std::vector<ContainedObject*>& candidates = selSumIn.data();
      if( candidates.size() ){        
        for (std::vector<ContainedObject*>::const_iterator ic = candidates.begin();
             ic != candidates.end(); ++ic) {
          const HltObjectSummary* hos(0);
          // go through all cases of supported candidates
          Track* candi = dynamic_cast<Track*>(*ic);
          if( candi ){
            hos = storeTrack( candi );
          } else {
            RecVertex* candi = dynamic_cast<RecVertex*>(*ic);
            if( candi ){
              hos = storeRecVertex( candi );
            } else {
              Particle* candi = dynamic_cast<Particle*>(*ic);
              if( candi ){
                hos = storeParticle( candi );
              } else { 
                CaloCluster* candi = dynamic_cast<CaloCluster*>(*ic);
                if( candi ){
                  hos = storeCaloCluster( candi );
                } else { 
                  Warning(" Unsupported data type in Hlt Trigger Summary - skip remaining candidates too, selection ID=" 
                          +selName,StatusCode::SUCCESS, 20 );
                  break;       
                }
              }
            }
          }
          if( !hos ){
            Warning(" Could not store supported candidate - skip remaining candidates too ",StatusCode::SUCCESS, 20 );
            break;
          } 
          selSumOut->addToSubstructure(hos);
        }
      } else {
        // Particles as Smart Ref
        const SmartRefVector< LHCb::Particle > & candidates = selSumIn.particles();       
        if( candidates.size() > 0 ){
          for(SmartRefVector<LHCb::Particle>::const_iterator  ic = candidates.begin();
              ic != candidates.end(); ++ic) {
            const Particle* candi = *ic;
            if( candi ){
              const HltObjectSummary* hos = storeParticle( candi );
              if( !hos ){
                Warning( " Could not store Particle - skip remaining candidates too ", StatusCode::SUCCESS, 20 );
                break;                
              }
              selSumOut->addToSubstructure(hos);              
            }
          }
        }
      }
    }

    // insert selection into the container
    if( outputSummary->insert(selName,*selSumOut) == StatusCode::FAILURE ){
      Warning(" Failed to add Hlt selection name "+selName
              +" to its container ",StatusCode::SUCCESS, 20 );

    }    

  }


  // clone HltObjectSummary-s of selection and insert them into object store (needed for raw data storage)
  for( HltSelReports::Container::const_iterator i=outputSummary->begin();i!=outputSummary->end();++i){
    const HltObjectSummary& sHos= i->second;  
    HltObjectSummary* hos= new HltObjectSummary( sHos );    
    m_objectSummaries->push_back(hos);
  }
  

  if ( msgLevel(MSG::VERBOSE) ){

   verbose() << " ======= HltSelReports size= " << outputSummary->size() << endmsg;  
   verbose() << *outputSummary << endmsg;

   verbose() << " ======= HltObjectSummary container size= " << m_objectSummaries->size() << endmsg;
   for( HltObjectSummary::Container::const_iterator ppHos=m_objectSummaries->begin();
        ppHos!=m_objectSummaries->end();++ppHos){
     const HltObjectSummary* pHos=*ppHos;    
     verbose() << " key " << pHos->index() << *pHos << endmsg;    
   }
   
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelReportsMaker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return HltBaseAlg::finalize();  // must be called after all other actions
}


// -------------------------------------
// store Track in HltObjectSummary store
// -------------------------------------
const LHCb::HltObjectSummary* HltSelReportsMaker::storeTrack(const LHCb::Track* object)
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
const LHCb::HltObjectSummary* HltSelReportsMaker::storeRecVertex(const LHCb::RecVertex* object)
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
    const Track* track = *it;    
    thisSubstructure.push_back( storeTrack(track) );
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
const LHCb::HltObjectSummary* HltSelReportsMaker::storeCaloCluster(const LHCb::CaloCluster* object)
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
const LHCb::HltObjectSummary* HltSelReportsMaker::storeParticle(const LHCb::Particle* object)
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
      hos->addToSubstructure( storeParticle( *p ) );
    }
  } else {
    // particles with no daughters have substructure via ProtoParticle 
    // we don't save Protoparticles, only things they lead to
    const ProtoParticle* pp = object->proto();
    if( !pp ){
      Warning(" Particle with no daughters and no protoparticle, skipped ",StatusCode::SUCCESS, 20 );
    } else {
      const Track* track=pp->track();
      if( track ){
        // charged track particle
        hos->addToSubstructure( storeTrack( track ) );
        // if muon add muon stub too
        const LHCb::MuonPID* muid = pp->muonPID();
        if( muid!=0 ){
          if( muid->IsMuon() ){
            if( !m_HLTmuonTracks ){ 
              if( exist<LHCb::Tracks>(m_HltMuonTracksLocation) ){
                m_HLTmuonTracks = get<LHCb::Tracks>(m_HltMuonTracksLocation);
              } else {
                Warning(" Found track which is a muon but no muon tracks at " + m_HltMuonTracksLocation,StatusCode::SUCCESS,20 );
              }
            }
            if( m_HLTmuonTracks ){          
              const Track* muStub= m_HLTmuonTracks->object(track->key());     
              if( muStub ){
                hos->addToSubstructure( storeTrack( muStub ) );
              }
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
            hos->addToSubstructure( storeCaloCluster( cluster ) );
          } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
            // Split Photons
            const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
            const LHCb::CaloHypo* g1 = *(hypos.begin() );
            const CaloCluster* cluster1 = *(g1->clusters().begin());
            hos->addToSubstructure( storeCaloCluster( cluster1 ) );
            const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
            const CaloCluster* cluster2 = *(g2->clusters().begin());
            hos->addToSubstructure( storeCaloCluster( cluster2 ) );
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
  std::vector<IANNSvc::minor_value_type> hltinfos = m_hltANNSvc->items("InfoID"); 
  switch( hos->summarizedObjectCLID() )
  {
  case LHCb::CLID_Track:
    {      
      const Track* candi = dynamic_cast<const Track*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kExtraInfoLevel &  m_presentInfoLevelTrack & m_presentInfoLevel ){ 
        for( std::vector<IANNSvc::minor_value_type>::const_iterator i= hltinfos.begin();i!=hltinfos.end();++i){
          if( ( 0<= i->second ) && ( i->second<=65535 ) )
          if( candi->hasInfo( i->second ) ){
            infoPersistent.insert( i->first, float( candi->info(i->second,-987.0) ) );
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
        }
      }    
    }    
    break;
  case LHCb::CLID_RecVertex:
    {      
      const RecVertex* candi = dynamic_cast<const RecVertex*>(hos->summarizedObject());
      if( !candi )return infoPersistent; 
      if( kExtraInfoLevel &  m_presentInfoLevelRecVertex & m_presentInfoLevel ){
        for( std::vector<IANNSvc::minor_value_type>::const_iterator i= hltinfos.begin();i!=hltinfos.end();++i){
          if( ( 0<= i->second ) && ( i->second<=65535 ) )
            if( candi->hasInfo( i->second ) ){
              infoPersistent.insert( i->first, float( candi->info(i->second,-987.0) ) );
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
        for( std::vector<IANNSvc::minor_value_type>::const_iterator i= hltinfos.begin();i!=hltinfos.end();++i){
          if( ( 0<= i->second ) && ( i->second<=65535 ) )
            if( candi->hasInfo( i->second ) ){
              infoPersistent.insert( i->first, float( candi->info(i->second,-987.0) ) );
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
        if( p < 1E-9 )p=1E-9;        
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

int HltSelReportsMaker::rankTrack(const LHCb::Track* object) const
{
  if( !object )return 0;
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);  
  int rank=100*rankLHCbIDs( object->lhcbIDs() ) +  infoToSave( hos ).size();
  delete hos;  
  return rank;
}

int HltSelReportsMaker::rankCaloCluster(const LHCb::CaloCluster* object) const
{
  if( !object )return 0;
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);  
  std::vector<LHCbID> clusterSeed; clusterSeed.push_back(object->seed()); 
  int rank=100*rankLHCbIDs( clusterSeed ) +  infoToSave( hos ).size();
  delete hos;  
  return rank;
}

int HltSelReportsMaker::rankRecVertex(const LHCb::RecVertex* object) const
{
  if( !object )return 0;
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);  
  int rank=10000 +  infoToSave( hos ).size();
  delete hos;  
  return rank;
}

int HltSelReportsMaker::rankParticle(const LHCb::Particle* object) const
{
  if( !object )return 0;
  HltObjectSummary* hos = new HltObjectSummary();
  hos->setSummarizedObjectCLID( object->clID() );
  hos->setSummarizedObject(object);  
  int rank=100000 +  infoToSave( hos ).size();
  delete hos;  
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
      if( selName == i->first ){
        return i->second;
      }
    }
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return m_maxCandidatesDecisionDebug;
    }
    return m_maxCandidatesNonDecisionDebug;

  } else {
    
    // NORMAL MODE

    // set max number of candidates
    const SelectionSetting & maxCandidates =  m_maxCandidates.value();    
    for( SelectionSetting::const_iterator i=maxCandidates.begin();
         i!=maxCandidates.end(); ++i){
      if( selName == i->first ){
        return i->second;
      }
    }
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return m_maxCandidatesDecision;
    }
    return m_maxCandidatesNonDecision;
    
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
    if( elem1.first < elem2.first )return true;
    if( elem1.first > elem2.first )return false;
    // equal ranks; now use selection name; decisions come last
    if( elem1.second.find("Decision")!=std::string::npos ){
      if( elem2.second.find("Decision")!=std::string::npos ){
        return (elem1.second.length()<elem2.second.length());
      }
      return false;
    } 
    if( elem2.second.find("Decision")!=std::string::npos ){
      return true;
    } 
    return (elem1.second.length()<elem2.second.length());
}



  
 /***
inline bool HltSelReportsMaker::rankSelLess( const HltSelReportsMaker::RankedSelection & elem1, 
                                             const HltSelReportsMaker::RankedSelection & elem2) 
{ 
    if( elem1.first < elem2.first )return true;
    if( elem1.first > elem2.first )return false;
    // equal ranks; now use selection name; decisions come last
    if( elem1.second.find("Decision")!=std::string::npos ){
      if( elem2.second.find("Decision")!=std::string::npos ){
        return (elem1.second.length()<elem2.second.length());
      }
      return false;
    } 
    if( elem2.second.find("Decision")!=std::string::npos ){
      return true;
    } 
    return (elem1.second.length()<elem2.second.length());
}
 ***/
