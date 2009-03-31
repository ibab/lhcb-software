// $Id: HltCopySelection.cpp,v 1.1 2009-03-31 11:37:17 graven Exp $
// Include files 
#include <algorithm>
#include <iterator>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltCopySelection.h"

#include "Event/Particle.h"
#include "Event/Track.h"
//-----------------------------------------------------------------------------
// Implementation file for class : HltCopySelection
//
// 2009-03-31 : Gerhard Raven
//-----------------------------------------------------------------------------

// explicit instantiation of class, and the corresponding
// declaration of the Algorithm Factory
template class HltCopySelection<LHCb::Particle>;
typedef HltCopySelection<LHCb::Particle> HltCopyParticleSelection;
DECLARE_ALGORITHM_FACTORY( HltCopyParticleSelection );

template class HltCopySelection<LHCb::Track>;
typedef HltCopySelection<LHCb::Track> HltCopyTrackSelection;
DECLARE_ALGORITHM_FACTORY( HltCopyTrackSelection );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template <typename T>
HltCopySelection<T>::HltCopySelection( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selection(*this)
{
  m_selection.declareProperties( );
}
//=============================================================================
// Destructor
//=============================================================================
template <typename T>
HltCopySelection<T>::~HltCopySelection() {} 

//=============================================================================
// Initialization
//=============================================================================
template <typename T>
StatusCode 
HltCopySelection<T>::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_selection.retrieveSelections();
  m_selection.registerSelection();
  // force creation of counters, and declare them to monitoring...
  declareInfo("#accept","",&counter("#accept"),0,std::string("Events accepted by ") + name());
  declareInfo("#input","",&counter("#input"),0,std::string("Candidates seen by ") + name());
  declareInfo("#candidated accepted","",&counter("#candidated accepted"),0,std::string("Candidates accepted by ") + name());
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

template <typename T>
StatusCode 
HltCopySelection<T>::execute() {
  counter("#input")  +=  m_selection.template input<1>()->size();
  std::copy( m_selection.template input<1>()->begin(), 
             m_selection.template input<1>()->end(), 
             std::back_inserter(*m_selection.output()) );
  counter("#accept") += !m_selection.output()->empty();
  counter("#candidated accepted") +=  m_selection.output()->size();
  setFilterPassed( !m_selection.output()->empty() );
  return StatusCode::SUCCESS;
}
