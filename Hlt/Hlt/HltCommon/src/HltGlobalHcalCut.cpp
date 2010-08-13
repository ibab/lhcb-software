// $Id: HltGlobalHcalCut.cpp,v 1.3 2010-08-13 13:25:13 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

// from Event
#include "HltGlobalHcalCut.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalHcalCut 
//
// 2008-02-12 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalHcalCut );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalHcalCut::HltGlobalHcalCut( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("MaximumNumberOfClusters", m_maxNumClusters = 10); 
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltGlobalHcalCut::~HltGlobalHcalCut() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalHcalCut::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalHcalCut::execute() {

  m_selections.output()->clean();

  //Print out the input, just for debug
  if (msgLevel(MSG::DEBUG)) {

        BOOST_FOREACH( const LHCb::L0CaloCandidate* iC, *m_selections.input<1>()) {
                debug() << iC << endmsg;
        }

  }
  //Filter the tracks
  StatusCode sc = filter_Clusters();

  //Print out the output, just for debug
  if (msgLevel(MSG::DEBUG)) {
        BOOST_FOREACH( const LHCb::L0CaloCandidate *iC, *m_selections.output() ) {
                debug() << iC << endmsg;
        }
  }
  
  int ncan = m_selections.output()->size();
  
  debug() << "Calorimeter clusters retained" << ncan << endmsg;
 
  //If too many clusters return an empty vector and hence kill the event
  if (ncan > m_maxNumClusters) m_selections.output()->clean();
 
  return sc;
  
}
//=============================================================================
StatusCode HltGlobalHcalCut::filter_Clusters(){
//Remove duplicate clusters created at boundary of HCAL boards

  //Loop over the clusters 
  BOOST_FOREACH(Hlt::TSelection<LHCb::L0CaloCandidate>::candidate_type* iC, *m_selections.input<1>()) {
    //code to remove duplicates goes here
    //You put all clusters which you want to save into the output like this
    m_selections.output()->push_back(iC);
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltGlobalHcalCut::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

