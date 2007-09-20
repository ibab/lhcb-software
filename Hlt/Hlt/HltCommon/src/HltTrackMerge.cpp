// $Id: HltTrackMerge.cpp,v 1.4 2007-09-20 19:22:48 tskwarni Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "HltTrackMerge.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackMerge
//
// 2007-03-22 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackMerge );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackMerge::HltTrackMerge( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("AllowEmptyInputTracks1", m_consider1=false);
  declareProperty("AllowEmptyInputTracks2", m_consider2=true);

  declareProperty("RemoveDuplicates", m_removeDuplicates=false);

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackMerge::~HltTrackMerge() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackMerge::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  checkInput(m_inputTracks," input tracks ");
  checkInput(m_inputTracks2," input tracks [2]");
  checkInput(m_outputTracks," output tracks ");

  return StatusCode::SUCCESS;
}

//===========================================================================
// Main execution
//===========================================================================
StatusCode HltTrackMerge::execute() {

  debug() << "==> Execute" << endmsg;

  copy(*m_inputTracks, *m_outputTracks);

  if( m_removeDuplicates && (m_inputTracks->size()>0) ){
    for( Hlt::TrackContainer::iterator i2=m_inputTracks2->begin();
          i2 != m_inputTracks2->end(); ++i2){
      if( std::find( m_inputTracks->begin(),m_inputTracks->end(), *i2 ) == m_inputTracks->end() ){
        m_outputTracks->push_back(*i2);
      }
    }
  } else {
    copy(*m_inputTracks2,*m_outputTracks);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackMerge::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
