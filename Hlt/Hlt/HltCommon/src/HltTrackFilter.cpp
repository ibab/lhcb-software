#include <vector>
#include <string>
#include <memory>

#include "boost/algorithm/string/join.hpp"

#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"

#include "Event/Track.h" 

#include "LoKi/ITrHybridFactory.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"

#include "GaudiKernel/AlgFactory.h" 

namespace Hlt {

    class TrackFilter : public HltAlgorithm {
    public:
      /// Standard constructor
      TrackFilter( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~TrackFilter( ) {};         ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute();       ///< Algorithm execution
      virtual StatusCode finalize();      ///< Algorithm finalization

    private:
      StatusCode decode();
      typedef LoKi::BasicFunctors<LHCb::Track>::Predicate Predicate;

      Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selection;
      std::auto_ptr<Predicate>                          m_predicate;

      std::string                                       m_factory;
      void updateCode ( Property& );
      void updatePreambulo ( Property& );

      std::vector<std::string> m_preambulo_ ;             // the preambulo itself
      std::string m_preambulo ;                           // the preambulo itself
      std::string m_code;

      bool m_code_updated;
      bool m_preambulo_updated;
    };
}

namespace {
        template <class InputIterator, class OutputIterator, class Predicate>
        OutputIterator copy_if_(InputIterator first, InputIterator last,
                               OutputIterator result, Predicate& pred)
        { while(first!=last) {
             if(pred(**first)) result = *first;
             ++first;
          }
          return result;
        }
}

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Hlt, TrackFilter );

Hlt::TrackFilter::TrackFilter( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selection(*this)
  , m_predicate(0)
  , m_code_updated(false)
  , m_preambulo_updated(false)
{
    m_selection.declareProperties( );
    declareProperty("Factory", m_factory = "LoKi::Hybrid::TrTool/TrHybridFactory:PUBLIC");
    declareProperty("Code", m_code) ->declareUpdateHandler( &TrackFilter::updateCode, this );
    declareProperty("Preambulo", m_preambulo_)->declareUpdateHandler(&TrackFilter::updatePreambulo , this);
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt::TrackFilter::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_selection.retrieveSelections();
  m_selection.registerSelection();
  return decode();
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt::TrackFilter::execute() {
  counter("#input")  +=  m_selection.input<1>()->size();
  copy_if_( m_selection.input<1>()->begin(), m_selection.input<1>()->end(), 
            std::back_inserter(*m_selection.output()),
            *m_predicate);
  setFilterPassed( !m_selection.output()->empty() );
  return StatusCode::SUCCESS;
}
//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt::TrackFilter::finalize() {
  m_predicate.reset(0); // delete at finalize to make sure that if the predicate
                        // has picked up some tools, they get released...
  return HltAlgorithm::finalize();
}


StatusCode
Hlt::TrackFilter::decode() 
{
    LoKi::ITrHybridFactory* factory = tool<LoKi::ITrHybridFactory>( m_factory,this ) ;
    LoKi::TrackTypes::TrCut cut = LoKi::BasicFunctors<LHCb::Track>::BooleanConstant( false ) ;
    StatusCode sc = factory->get( m_code, cut, m_preambulo );
    if (sc.isFailure()) return sc;
    m_predicate.reset( cut.clone() );
    this->release(factory);
    m_code_updated      = false ;
    m_preambulo_updated = false ;
    return StatusCode::SUCCESS;
}

//=============================================================================
// update handlers
//=============================================================================
void Hlt::TrackFilter::updateCode ( Property& /* p */ )    
{
  /// mark as "to-be-updated"
  m_code_updated = true ;
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_preambulo_updated ) { return ; }
  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from Hlt::TrackFilter::decode()" , sc ) ;
}

void Hlt::TrackFilter::updatePreambulo ( Property& /* p */ )
{
  // concatenate the preambulo:
  m_preambulo = boost::algorithm::join( m_preambulo_ , "\n" );
  /// mark as "to-be-updated"
  m_preambulo_updated = true ;
  // no further action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_code_updated ) { return ; }

  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from Hlt::TrackFilter::decode()" , sc ) ;
}
