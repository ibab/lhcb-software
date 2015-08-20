// $Id: HltSelectionsBase.cpp,v 1.2 2010-06-01 09:44:18 pkoppenb Exp $
// Include files 
#include <algorithm>
#include <utility>
#include <iterator>

// from Gaudi
#include "Kernel/IIndexedANNSvc.h"

// local
#include "HltSelectionsBase.h" 
  
//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionsBase
//
// 2007-03-29 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionsBase::HltSelectionsBase( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_isTrigger(false)
{
  declareProperty( "Hlt2SelectionID" , m_hlt2SelectionID = "Hlt2SelectionID");
}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionsBase::~HltSelectionsBase() {} 


constexpr struct swap_first_and_second_t {
    template <typename S, typename T> 
    std::pair<T,S>  operator()(const std::pair<S,T>& p) const { return { p.second, p.first }; }
} swap_first_and_second {};
//=========================================================================
//  fill selections map
//=========================================================================
const hltPairs& HltSelectionsBase::selections ( unsigned int TCK ) 
{
  if ( m_selections.empty() || m_tck != TCK )
  {
  
    auto idmap  = svc<IIndexedANNSvc>("TCKANNSvc")->i2s(TCK,m_hlt2SelectionID);
    m_selections.clear();
    std::transform( std::begin(idmap), std::end(idmap), std::back_inserter(m_selections),
                    swap_first_and_second );
    m_tck = TCK;
    if ( m_selections.empty()) Exception("Empty selections!");
    if (msgLevel(MSG::INFO)) info() << "Selections" ;
    for ( hltPairs::const_iterator n = m_selections.begin() ; m_selections.end() != n ; ++n ) 
    {
      if (msgLevel(MSG::INFO)) info() << ", " << n->first << " : " << n->second ;
    }
    if (msgLevel(MSG::INFO)) info() << endmsg ;
  }
  
  return m_selections ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionsBase::execute() 
{
  err() << "This is a base class. Do not invoke." << endmsg ;
  return StatusCode::FAILURE ;
}
