// $Id: HltVertexReportsMaker.cpp,v 1.13 2010-06-01 14:29:21 graven Exp $
// Include files 
#include <vector>
#include <algorithm>
#include <utility>
#include "boost/foreach.hpp"

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"

namespace Gaudi { 
    class StringKey;
    namespace Utils {
        std::ostream& toStream ( const std::vector<Gaudi::StringKey>& key , 
                                 std::ostream&             s   ) 
        {
              s << "[ " ;
              for (std::vector<Gaudi::StringKey>::const_iterator i=key.begin();i!=key.end();++i)  {
                  if (i!=key.begin()) s << ", ";
                  toStream(*i,s); 
              }
              s << " ]";
              return s;
        }
}; };



#include "GaudiKernel/AlgFactory.h" 

#include "Event/RecVertex.h"

#include "HltBase/HltSelection.h"

// local
#include "HltVertexReportsMaker.h"

using namespace LHCb;

namespace {
    class matchByName : public std::unary_function<bool,const Hlt::Selection*> {

    public:
        matchByName(const std::string& name) : m_name(name) { }
        bool operator()(const Hlt::Selection* x) const { return x->id().str() == m_name; }
    private:
        std::string m_name;
    };
}

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
  , m_hltSvc(0)
  , m_regSvc(0)
  , m_inspectionSvc(0)
  , m_PVLocation(LHCb::RecVertexLocation::Primary)
{

  declareProperty("OutputHltVertexReportsLocation",
    m_outputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);  

  declareProperty("VertexSelections", m_vertexSelections);
  declareProperty("PVLocation", m_PVLocation);

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
  m_hltSvc = svc<Hlt::IData> ( "Hlt::Service" , true ) ;
  m_regSvc = svc<Hlt::IRegister> ( "Hlt::Service" , true ) ;
  m_inspectionSvc = svc<Hlt::IInspector> ( "Hlt::Service" , true ) ;

  m_selections.clear();  

 // get string-to-int selection ID map
  typedef std::map<IANNSvc::minor_key_type,IANNSvc::minor_mapped_type> map_t;
  map_t selectionNameToIntMap;
  BOOST_FOREACH( const IANNSvc::minor_value_type& p, m_hltANNSvc->items("Hlt1SelectionID") ) {
      selectionNameToIntMap.insert( p );
  }
  BOOST_FOREACH( const IANNSvc::minor_value_type& p, m_hltANNSvc->items("Hlt2SelectionID") ) {
      selectionNameToIntMap.insert( p );
  }

  // loop over selections given in the input list
  Hlt::IRegister::Lock lock(m_regSvc,this);
  for( std::vector<std::string>::const_iterator is=m_vertexSelections.begin();
       is!=m_vertexSelections.end();++is){
     Gaudi::StringKey key(*is);
     // find int selection id (to make sure it is saveable)
     map_t::const_iterator im = selectionNameToIntMap.find( key );
     if (im == selectionNameToIntMap.end() ) {
       Warning( " selectionName="+key.str()+ " not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
       continue;
     } 
     lock->registerInput(key,this);
     const Hlt::Selection *s = m_hltSvc->selection(key,this);
     if (s==0) {
       Warning( " selectionName="+key.str()+ " not present in Hlt::IDataSvc. Skipped. ",StatusCode::SUCCESS, 20 );
       continue;
     }
     m_selections.push_back(s);
  }
  
  //replaced OnOfflineTool with simple property
  std::size_t found = m_PVLocation.find_last_of("/");
  std::string pvSelectionName = ( found != std::string::npos ) 
                              ? m_PVLocation.substr(found+1)
                              : m_PVLocation;
  // int selection id
  map_t::const_iterator im = selectionNameToIntMap.find( pvSelectionName );
  if (im == selectionNameToIntMap.end() ) {
     Warning( " selectionName="+pvSelectionName+ " not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 10 );
  } else {
    // check we don't already have this one from the datasvc...
    if (std::find_if( m_selections.begin(), m_selections.end(), matchByName(pvSelectionName) ) == m_selections.end() ) {
        m_tesSelections.push_back(std::make_pair(pvSelectionName,m_PVLocation));
    } else {
        debug() << " got " << pvSelectionName << " also from datasvc ... using the one from the datasvc" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

template <typename ITER>
StatusCode HltVertexReportsMaker::saveCandidates(const std::string& selName, 
                                                 ITER ic, ITER end,
                                                 LHCb::HltVertexReports* outputSummary) const 
{
     // save selection ---------------------------
     SmartRefVector<VertexBase> pVtxs;
    
     // create output container for vertices and put it on TES
     VertexBase::Container* verticesOutput = new VertexBase::Container();
     put( verticesOutput, m_outputHltVertexReportsLocation + "/" + selName  );

     while ( ic!=end ) {
       const VertexBase* vbase = dynamic_cast<const VertexBase*>(*ic);
       if( vbase == 0 ) { 
           Warning( " dynamic cast failed for ="+selName+". Skipped. ",StatusCode::SUCCESS, 10 );
           continue;
       }
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
       ++ic;
     }

     // insert selection into the container
     if( outputSummary->insert(selName,pVtxs).isFailure() ) {
      return Warning(" Failed to add Hlt vertex selection name " + selName
                     + " to its container ",StatusCode::SUCCESS, 10 );
    } else {
        return StatusCode::SUCCESS;
    }
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // loop over selections
  BOOST_FOREACH( const Hlt::Selection* is, m_selections) {
     // prevent duplicate selections
     if( outputSummary->hasSelectionName( is->id().str() ) )continue;

     // unsuccessful selections can't save candidates
     if( !is->decision() )continue;

     if (is->classID() != LHCb::RecVertex::classID()) {
       Error(" Selection name "+is->id().str()+" did not select vertices. ",StatusCode::SUCCESS, 5 );
       continue;
     }
     const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*is);      
     // empty selections have nothing to save
     if( tsel.empty() )continue;
     saveCandidates(is->id().str(),tsel.begin(),tsel.end(),outputSummary).ignore();
   }
 
   // now try TES location
   typedef std::pair<std::string,std::string> pair_t;
   BOOST_FOREACH(const pair_t& p, m_tesSelections ) 
   {
     LHCb::RecVertices* pvs = getIfExists<LHCb::RecVertices>(p.second);
     if (NULL==pvs) continue;
     if( pvs->empty() )continue;
     saveCandidates(p.first,pvs->begin(),pvs->end(),outputSummary).ignore();
  }

  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " ======= HltVertexReports size= " << outputSummary->size() << endmsg;
    verbose() << *outputSummary << endmsg;
  }

  return StatusCode::SUCCESS;
}

