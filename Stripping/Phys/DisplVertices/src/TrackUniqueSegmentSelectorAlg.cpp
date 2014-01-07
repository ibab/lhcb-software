// Include files 
// -------------
// from STD

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// TrackEvent
#include "Kernel/ITrackUniqueSegmentSelector.h"
  
/** @class TrackUniqueSegmentSelectorAlg TrackUniqueSegmentSelectorAlg.cpp
 *  
 *
 *  @author Wouter Hulsbergen
 *
 */
class TrackUniqueSegmentSelectorAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackUniqueSegmentSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackUniqueSegmentSelectorAlg( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
  
private:
  ToolHandle<ITrackUniqueSegmentSelector> m_selector ;
};

//-----------------------------------------------------------------------------
// Implementation file for class : TrackUniqueSegmentSelectorAlg
//
// 2011-12-01 Wouter Hulsbergen
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackUniqueSegmentSelectorAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackUniqueSegmentSelectorAlg::TrackUniqueSegmentSelectorAlg( const std::string& name,
						    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_selector("TrackUniqueSegmentSelector",this)
{
  declareProperty("Selector",m_selector) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackUniqueSegmentSelectorAlg::~TrackUniqueSegmentSelectorAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackUniqueSegmentSelectorAlg::initialize()
{
  //StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // ------------------------------
  sc = m_selector.retrieve() ;
  if ( sc.isFailure() ) return sc; 
  
  return sc ;
}

//=============================================================================
// Main execution
//=============================================================================
namespace {
  struct UnConst {
    LHCb::Track* operator()( const LHCb::Track* trk ) {
      return const_cast<LHCb::Track*>(trk) ; 
    }
  } ;
}

StatusCode TrackUniqueSegmentSelectorAlg::execute()
{  
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(LHCb::TrackLocation::Default) ;
  // unconst them (not very neat ...)
  //std::vector<LHCb::Track*> nonconsttracks( tracks.begin(), tracks.end(),
  //[]( const LHCb::Track* trk){ return const_cast<LHCb::Track*>(trk) ;  } ) ;
  std::vector<LHCb::Track*> nonconsttracks( tracks.size()) ;
  std::transform( tracks.begin(), tracks.end(), nonconsttracks.begin(), UnConst() ) ;
  return m_selector->flag( nonconsttracks ) ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackUniqueSegmentSelectorAlg::finalize()
{
  m_selector.release().ignore() ;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
