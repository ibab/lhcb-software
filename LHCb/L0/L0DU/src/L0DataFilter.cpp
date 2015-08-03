// Include files 

// from Event
#include "Event/L0DUReport.h"

// local
#include "L0DataFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DataFilter
//
// 2012-04-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DataFilter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DataFilter::L0DataFilter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator )
{
  declareProperty("L0DULocation"   , m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("Selection"   , m_selection );
  declareProperty("Logical"     , m_logical = "AND" ); // "AND" or "OR"
  declareProperty("Revert"         , m_revert = false );
}
//=============================================================================
// Destructor
//=============================================================================
L0DataFilter::~L0DataFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DataFilter::initialize() {
  StatusCode sc = L0AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if( m_selection.empty()) info() << "No selection is defined - filter has no effect" << endmsg;


  // check property
  info() << "L0DU data selection : " << endmsg;
  unsigned int k=1;
  if( m_logical != "AND" && m_logical != "OR")return Error("Logical must be 'AND' or 'OR'",StatusCode::FAILURE);
  for(std::map<std::string,std::vector<std::string> >::iterator i=m_selection.begin();m_selection.end() != i;++i){
    if( (i->second).size() != 2)return Error("Selection syntax is : 'DataName' : {'comparator','threshold'}",StatusCode::FAILURE);
    std::string name = i->first;
    std::string comp = (i->second)[0];
    std::string st = (i->second)[1];
    if( comp != ">" && comp != "<" && comp != ">=" && comp != "<=" && comp != "==")
      return Error("Comparator must be '>', '>=', '<', '<=' or '=='",StatusCode::FAILURE);
    info() << "  - ("<<name<<" "<<comp<<" "<<st<<") " ;
    if( k < m_selection.size()) info() << m_logical << endmsg;
    else info() << endmsg;
    k++;
  }
  return StatusCode::SUCCESS;
} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DataFilter::execute() {

  if( m_selection.empty()){
    setFilterPassed( true  ); 
    return StatusCode::SUCCESS;
  }


  std::string loc = dataLocation( m_l0Location );
  const  LHCb::L0DUReport* l0 = getIfExists<LHCb::L0DUReport>( loc );
  if( NULL == l0 ){
    counter("Report not found") += 1;
    setFilterPassed( false ); 
    return Warning("L0DUReport not found at location : '" + loc + "' - the event is rejected", StatusCode::SUCCESS );
  }

  
  //===== Processing
  bool oSel = false;
  bool aSel = true;
  for(std::map<std::string,std::vector<std::string> >::iterator i=m_selection.begin();m_selection.end() != i;++i){
    std::string name = i->first;
    std::string comp = (i->second)[0];
    std::string st = (i->second)[1];
    int threshold;
    std::istringstream is( st.c_str() );
    is >> threshold;
    int value = l0->dataDigit( name );
    bool sel = false;
    if (comp == ">")sel =  (value > threshold)       ? true : false;
    else if (comp == ">=")sel =  (value >= threshold)       ? true : false;
    else if (comp == "<")sel =  (value < threshold)  ? true : false;
    else if (comp == "<=")sel =  (value <= threshold)  ? true : false;
    else if (comp == "==")sel = (value == threshold) ? true : false;
    if(sel)oSel = true;
    aSel = aSel & sel;
    counter( name +" "+comp+" "+st)+=(int)sel;
  }

  bool accept = true;
  if( m_logical == "AND") accept = m_revert ? !aSel : aSel;
  else if( m_logical == "OR") accept = m_revert ? !oSel : oSel;
  counter(" -> ALL (AND) ") += (int) aSel;
  counter(" -> ALL (OR)"  ) += (int) oSel;
  counter(" -> Accept") += (int) accept;
  

  setFilterPassed( accept );

  return StatusCode::SUCCESS; 
}

//=============================================================================



