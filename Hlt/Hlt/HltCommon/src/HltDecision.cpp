// $Id: HltDecision.cpp,v 1.1 2008-09-09 11:00:44 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltDecision.h"
#include "Event/HltDecReport.h"
#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecision
//
// 2008-08-22 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecision );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecision::HltDecision( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  declareProperty("OR", m_acceptNames)
               -> declareUpdateHandler( &HltDecision::acceptAlgsHandler, this );
  declareProperty("AND", m_requireNames)
               -> declareUpdateHandler( &HltDecision::requireAlgsHandler, this );
  declareProperty("NOT", m_vetoNames)
               -> declareUpdateHandler( &HltDecision::vetoAlgsHandler, this );

  m_requireNames.push_back(name); // put ourselves on the 'require' stack...
  declareProperty("Decision", m_decisionName = name );
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
}


//=============================================================================
// Destructor
//=============================================================================
HltDecision::~HltDecision() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecision::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  sc = decodeAlgorithms( m_acceptNames, m_acceptAlgs );
  if ( sc.isFailure() ) return sc; 
  sc = decodeAlgorithms( m_requireNames, m_requireAlgs );
  if ( sc.isFailure() ) return sc; 
  sc = decodeAlgorithms( m_vetoNames, m_vetoAlgs );
  if ( sc.isFailure() ) return sc; 

  return StatusCode::SUCCESS;
};


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecision::execute() {
  
  LHCb::HltDecReports* decisions = getOrCreate<LHCb::HltDecReports,LHCb::HltDecReports>( m_location );
  if ( decisions->hasSelectionName( m_decisionName ) ) {
     debug() << "HltDecReport already contains decision for " << m_decisionName << endmsg;
     error() << "HltDecReport already contains 'my' decision" << endmsg;
     return StatusCode::FAILURE;
  }
  boost::optional<IANNSvc::minor_value_type> key = annSvc().value("Hlt1SelectionID",m_decisionName);
  if (!key) {
     debug() << "HltANNSvc does not know about " << m_decisionName << endmsg;
     error() << "'my' decision not known by HltANNSvc " << endmsg;
     return StatusCode::FAILURE;
  }
  debug() << "got input: " << m_decisionName << " and key " << key->first << " -> " << key->second << endmsg;
  bool acc = accept();
  LHCb::HltDecReport decision(  acc,
                                0, // don't know about errors...
                                0, // don't know about candidates...
                                key->second );
  if (decision.invalidIntSelectionID()) {
    warning() << " selectionName=" << key->first << " has invalid intSelectionID=" << key->second << " Skipped. " << endmsg;
  } else {
    decisions->insert( m_decisionName, decision );
  }
  setFilterPassed( acc );
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDecision::finalize() {
  //TODO: add statistics...
  return GaudiAlgorithm::finalize();  
}

//=============================================================================
//  Utilities...
//=============================================================================
void HltDecision::acceptAlgsHandler( Property& /* theProp */ )  {
  if (decodeAlgorithms( m_acceptNames, m_acceptAlgs ).isFailure()) {
    throw GaudiException("Failure in OutputStream::decodeAlgorithms",
                         "HltDecision::acceptAlgsHandler",StatusCode::FAILURE);
  }
}
void HltDecision::requireAlgsHandler( Property& /* theProp */ )  {
  if (decodeAlgorithms( m_requireNames, m_requireAlgs ).isFailure()) {
    throw GaudiException("Failure in HltDecision::decodeAlgorithms",
                         "HltDecision::requireAlgsHandler",StatusCode::FAILURE);
  }
}
void HltDecision::vetoAlgsHandler( Property& /* theProp */ )  {
  if (decodeAlgorithms( m_vetoNames, m_vetoAlgs ).isFailure()) {
    throw GaudiException("Failure in HltDecision::decodeAlgorithms",
                         "HltDecision::vetoAlgsHandler",StatusCode::FAILURE);
  }
}
//=============================================================================
namespace {
    struct execAndPass : std::unary_function<const Algorithm*,bool> {
        bool operator()(const Algorithm * alg) const { return alg->isExecuted() && alg->filterPassed(); }
    };
    struct notExecOrNotPassed  : std::unary_function<const Algorithm*,bool> {
        bool operator()(const Algorithm * alg) const { return !alg->isExecuted() || !alg->filterPassed(); }
    };
}
//=============================================================================

bool HltDecision::accept() const {
  bool result = m_acceptAlgs.empty();
  if ( result ) {
    result = 
        std::find_if( m_acceptAlgs.begin(), m_acceptAlgs.end(), execAndPass() )!=m_acceptAlgs.end() ;
  }
  if ( result && !m_requireAlgs.empty() ) { 
    result = 
        std::find_if( m_requireAlgs.begin(), m_requireAlgs.end(), notExecOrNotPassed() )==m_requireAlgs.end() ;
  }

  if ( result && ! m_vetoAlgs.empty() ) { // can only reject 
    result = 
        std::find_if( m_vetoAlgs.begin(), m_vetoAlgs.end(), execAndPass() )==m_vetoAlgs.end() ;
  }   
  return result;
}
//=============================================================================
StatusCode HltDecision::decodeAlgorithms( std::vector<std::string>& theNames,
                                          std::vector<Algorithm*>& theAlgs )
{
  // Reset the list of Algorithms
  theAlgs.clear( );
  IAlgManager* theAlgMgr = svc<IAlgManager>("ApplicationMgr");
  // Build the list of Algorithms from the names list
  std::vector<std::string>::const_iterator it;
  for (it = theNames.begin(); it != theNames.end(); ++it) {
    // Check whether the suppied name corresponds to an existing
    // Algorithm object.
    IAlgorithm* theIAlg(0);
    Algorithm*  theAlgorithm(0);
    if ( theAlgMgr->getAlgorithm( *it, theIAlg ).isSuccess( ) ) {
        theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
    } 
    if ( theAlgorithm == 0 ) {
        warning() << *it << " doesn't exist - ignored" << endreq;
        continue;
    }

    // Check that the specified algorithm doesn't already exist in the list
    if (std::find( theAlgs.begin(),theAlgs.end(), theAlgorithm)!=theAlgs.end()) {
        warning() << *it << " already in list - ignored" << endreq;
        continue;
    }
    theAlgs.push_back( theAlgorithm );
  }
  return StatusCode::SUCCESS;
}
