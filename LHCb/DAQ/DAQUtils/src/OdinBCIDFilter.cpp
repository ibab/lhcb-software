// from LHCb
#include "Event/ODIN.h"
// local
#include "OdinBCIDFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OdinBCIDFilter
//
// 2008-28-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OdinBCIDFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OdinBCIDFilter::OdinBCIDFilter( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    ,m_all(0)
    ,m_acc(0)
{
  // Default : filtering odd-parity BXID
  declareProperty("Mask"        , m_mask = 0x1);
  declareProperty("Comparator"  , m_comparator = "==");
  declareProperty("Value"       , m_value=1);
  declareProperty("Revert"      , m_revert = false);
}
//=============================================================================
// Destructor
//=============================================================================
OdinBCIDFilter::~OdinBCIDFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OdinBCIDFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  //
  if( m_comparator != "=" && m_comparator != "==" && m_comparator != "!=" &&
      m_comparator != ">" && m_comparator != ">=" &&
      m_comparator != "<" && m_comparator != "<=" ){
    error() << "Comparator '"<< m_comparator << "' is not supported" << endmsg;
    error() << " Choose among : ('=' , '!=' , '>' , '>=' , '<' , '<=')" << endmsg;
    return StatusCode::FAILURE;
  }
  //
  if( m_revert )
    info() << " Filtering criteria : ![(BXID & " << m_mask <<")" << m_comparator << " " << m_value << "]" << endmsg;
  else
    info() << " Filtering criteria :  [(BXID & " << m_mask <<")" << m_comparator << " " << m_value << "]" << endmsg;

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OdinBCIDFilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_all++;

  // treat trivial requests
  setFilterPassed(false);  

  // get ODIN
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if( odin == NULL ) return Error("ODIN cannot be loaded",StatusCode::FAILURE);

  long bx = odin->bunchId();

  bool decision = (m_revert) ? !rule(bx) : rule(bx);
  if ( msgLevel(MSG::DEBUG) ) debug() << "Accept event : " << bx << " : " << decision << endmsg;
  setFilterPassed( decision );
  if(decision)m_acc++;

  return StatusCode::SUCCESS;
}


bool OdinBCIDFilter::rule(long bx){
  if(m_comparator == "="  ) return ( (bx & m_mask) == m_value  );
  else if(m_comparator == "=="  ) return ( (bx & m_mask) == m_value  );
  else if (m_comparator == "!=" ) return ( (bx & m_mask) != m_value  );
  else if (m_comparator == ">"  ) return ( (bx & m_mask) >  m_value  );
  else if (m_comparator == ">=" ) return ( (bx & m_mask) >= m_value  );
  else if (m_comparator == "<"  ) return ( (bx & m_mask) <  m_value  );
  else if (m_comparator == "<=" ) return ( (bx & m_mask) <= m_value  );
  else return false;
} 

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OdinBCIDFilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if( m_revert )
    info() << " Filtering criteria : ![(BXID & " << m_mask <<")" << m_comparator << " " << m_value << "]" << endmsg;
  else
    info() << " Filtering criteria :  [(BXID & " << m_mask <<")" << m_comparator << " " << m_value << "]" << endmsg;
  info() << "   ---> " << m_acc << " accepted events among " << m_all << endmsg;
  

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
