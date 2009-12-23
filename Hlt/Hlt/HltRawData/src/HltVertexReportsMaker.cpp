// $Id: HltVertexReportsMaker.cpp,v 1.11 2009-12-23 17:59:50 graven Exp $
// Include files 

// from Gaudi
#include <vector>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RecVertex.h"

#include "HltBase/HltSelection.h"

// local
#include "HltVertexReportsMaker.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexReportsMaker
//
// 2008-08-05 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexReportsMaker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexReportsMaker::HltVertexReportsMaker( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_hltANNSvc(0) 
  , m_hltDataSvc(0)
  , m_onOfflineTool(0)
{

  declareProperty("OutputHltVertexReportsLocation",
    m_outputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);  

  declareProperty("VertexSelections", m_vertexSelections);


}
//=============================================================================
// Destructor
//=============================================================================
HltVertexReportsMaker::~HltVertexReportsMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexReportsMaker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");
  m_hltDataSvc = svc<IHltDataSvc>("HltDataSvc");

  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool", this);

  m_selectionNames.clear();  

 // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );

  // get trigger selection names 
  std::vector<Gaudi::StringKey> hltDataSvcKeys = m_hltDataSvc->selectionKeys();

  // loop over selections given in the input list
  for( std::vector<std::string>::const_iterator is=m_vertexSelections.value().begin();
       is!=m_vertexSelections.value().end();++is){
     const std::string selName(*is);  
     const Gaudi::StringKey nameKey(*is);
     if( find( hltDataSvcKeys.begin(), hltDataSvcKeys.end(), nameKey ) != hltDataSvcKeys.end() ){
       
       // find int selection id (to make sure it is saveable)
       int intSelID(0);   
       for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
            si!=selectionNameToIntMap.end();++si){
         if( si->first == selName ){
           intSelID=si->second;
           break;
         }
       }
       if( !intSelID ){
         Warning( " selectionName="+selName+ " not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
         continue;
       }

       m_selectionNames.push_back(*is);
       
     }
  }
  
  // add Hlt2 vertex location from OnOfflineTool (also get TES path from this)
  const std::string& hlt2PVLocation = m_onOfflineTool->primaryVertexLocation() ;
  std::size_t found = hlt2PVLocation.find_last_of("/");
  std::string pvSelectionName;  
  if( found != std::string::npos ){
    pvSelectionName = hlt2PVLocation.substr(found+1);
    m_tesPath=hlt2PVLocation.substr(0,found+1);
  } else {
    pvSelectionName = hlt2PVLocation;
    m_tesPath="";  
  }
  // int selection id
  int intSelID(0);   
  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
       si!=selectionNameToIntMap.end();++si){
    if( si->first == pvSelectionName ){
      intSelID=si->second;
      break;
    }
  }
  if( !intSelID ){
    Warning( " selectionName="+pvSelectionName+ " not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
  } else {
    m_selectionNames.push_back(pvSelectionName);
  }
  
  

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // loop over selection names
  for( std::vector<std::string>::const_iterator is=m_selectionNames.begin();
       is!=m_selectionNames.end();++is ){
     const std::string selName(*is);     
     const Gaudi::StringKey name(*is);

     // prevent duplicate selections
     if( outputSummary->hasSelectionName( selName ) )continue;

     std::vector<const ContainedObject*> candidates;

     // try dataSvc first
     const Hlt::Selection* sel = m_hltDataSvc->selection(name,this);
     if ( sel ) {


       // unsuccessful selections can't save candidates
       if( !sel->decision() )continue;

       if (sel->classID() != LHCb::RecVertex::classID()) {
         Error(" Selection name "+selName+" did not select vertices. ",StatusCode::SUCCESS, 5 );
         continue;
       }
       
       const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);      
       // number of candidates
       int noc = tsel.size();
       // empty selections have nothing to save
       if( !noc )continue;

       for (Hlt::VertexSelection::const_iterator it = tsel.begin(); it != tsel.end(); ++it) {
         candidates.push_back( (const ContainedObject*)(*it) );
       }
       
     } else {

       // now try TES location
       const std::string location = m_tesPath + selName;       
       if ( exist<LHCb::RecVertices>(location)){

         LHCb::RecVertices* pvs = get<LHCb::RecVertices>(location);
         for( LHCb::RecVertices::const_iterator iVtx = pvs->begin(); iVtx != pvs->end(); ++iVtx ){           
           candidates.push_back( (const ContainedObject*)(*iVtx) );           
         }
         
       }
       
     }
     
     if( ! candidates.size() )continue;

     // save selection ---------------------------

     SmartRefVector<VertexBase> pVtxs;
    
     // create output container for vertices and put it on TES
     VertexBase::Container* verticesOutput = new VertexBase::Container();
     put( verticesOutput, m_outputHltVertexReportsLocation.value() + "/" + selName  );

     for (std::vector<const ContainedObject*>::const_iterator ic = candidates.begin();
             ic != candidates.end(); ++ic) {
       const RecVertex* candi = dynamic_cast<const RecVertex*>(*ic);
       if( !candi )continue;
       const VertexBase* vbase = dynamic_cast<const VertexBase*>(candi);
       if( !vbase )continue;
       // need to clone it to put into its new container, also use precision of the storage banks
       VertexBase* pVtx = new VertexBase();
       Gaudi::XYZPoint position( double(float(vbase->position().x())),
                                 double(float(vbase->position().y())),
                                 double(float(vbase->position().z())) );       
       pVtx->setPosition( position );
       pVtx->setChi2( double(float(vbase->chi2())) );
       pVtx->setNDoF( (vbase->nDoF()>0)?vbase->nDoF():0 );       
       // now also save covariance matrix 2009/11/26
       const Gaudi::SymMatrix3x3 & ocov = vbase->covMatrix();       
       Gaudi::SymMatrix3x3 cov;
       cov[0][0] = double(float(ocov[0][0]));
       cov[1][1] = double(float(ocov[1][1]));
       cov[2][2] = double(float(ocov[2][2]));
       cov[0][1] = double(float(ocov[0][1]));
       cov[0][2] = double(float(ocov[0][2]));
       cov[1][2] = double(float(ocov[1][2]));    
       pVtx->setCovMatrix(cov);       
       verticesOutput->insert(pVtx);
       pVtxs.push_back( SmartRef<VertexBase>( pVtx ) );
     }

     // insert selection into the container
     if( outputSummary->insert(selName,pVtxs) == StatusCode::FAILURE ){
       Warning(" Failed to add Hlt vertex selection name " + selName
               + " to its container ",StatusCode::SUCCESS, 20 );
    }    

  }
  
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " ======= HltVertexReports size= " << outputSummary->size() << endmsg;
    verbose() << *outputSummary << endmsg;
  }

  return StatusCode::SUCCESS;
}

