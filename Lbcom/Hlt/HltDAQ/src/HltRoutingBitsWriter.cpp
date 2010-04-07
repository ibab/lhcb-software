// $Id: HltRoutingBitsWriter.cpp,v 1.5 2010-04-07 14:36:51 graven Exp $
// Include files 
// from Boost
#include "boost/foreach.hpp"
#include <boost/format.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h" 
#include "Event/HltDecReports.h" 
#include "Event/L0DUReport.h" 
#include "Event/ODIN.h" 
#include "boost/algorithm/string/join.hpp"
// from LoKi:
#include "LoKi/IHltFactory.h"

// local
#include "HltRoutingBitsWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltRoutingBitsWriter
//
// 2008-07-29 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRoutingBitsWriter );

StatusCode HltRoutingBitsWriter::decode() {
    zeroEvaluators();
    LoKi::Hybrid::IHltFactory* factory = tool<LoKi::Hybrid::IHltFactory>( "LoKi::Hybrid::HltFactory" ) ;

    typedef std::map<unsigned int,std::string>::const_iterator iter_t;
    for (iter_t i=m_bits.begin();i!=m_bits.end();++i) {
        if ( i->first>nBits ) return StatusCode::FAILURE;
        if (!i->second.empty()) { 
            std::string title = boost::str(boost::format("%02d:%s") % i->first % i->second) ;
            if (i->first<8) { 
                LoKi::Types::ODIN_Cut cut( LoKi::BasicFunctors<const LHCb::ODIN*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_odin_evaluators[i->first].predicate = cut.clone();
                m_odin_evaluators[i->first].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% title ),
                        *m_odin_evaluators[i->first].counter,title);
            } else if (i->first<32) {
                LoKi::Types::L0_Cut cut( LoKi::BasicFunctors<const LHCb::L0DUReport*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_l0_evaluators[i->first-8].predicate = cut.clone();
                m_l0_evaluators[i->first-8].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% title ),
                        *m_l0_evaluators[i->first-8].counter,title);
            } else {
                LoKi::Types::HLT_Cut cut( LoKi::BasicFunctors<const LHCb::HltDecReports*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_hlt_evaluators[i->first-32].predicate = cut.clone();
                m_hlt_evaluators[i->first-32].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% title ),
                        *m_hlt_evaluators[i->first-32].counter,title);
            }

        }
    }
    this->release(factory);
    m_bits_updated      = false ;
    m_preambulo_updated = false ;
    return StatusCode::SUCCESS;
}

void HltRoutingBitsWriter::zeroEvaluators(bool skipDelete) 
{ 
    BOOST_FOREACH( odin_eval_t& eval , m_odin_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = 0;
        eval.counter = 0;
    }
    BOOST_FOREACH( l0_eval_t& eval , m_l0_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = 0;
        eval.counter = 0;
    }
    BOOST_FOREACH( hlt_eval_t& eval , m_hlt_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = 0;
        eval.counter = 0;
    }
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRoutingBitsWriter::HltRoutingBitsWriter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_bits_updated(false)
  , m_preambulo_updated(false)
{
  zeroEvaluators(true);
  declareProperty("HltDecReportsLocation", m_hlt_location = LHCb::HltDecReportsLocation::Default);
  declareProperty("L0DUReportLocation", m_l0_location = LHCb::L0DUReportLocation::Default);
  declareProperty("ODINLocation", m_odin_location = LHCb::ODINLocation::Default);
  declareProperty("RoutingBits", m_bits) ->declareUpdateHandler( &HltRoutingBitsWriter::updateBits, this );
  declareProperty("Preambulo", m_preambulo_)->declareUpdateHandler(&HltRoutingBitsWriter::updatePreambulo , this);

}
//=============================================================================
// Destructor
//=============================================================================
HltRoutingBitsWriter::~HltRoutingBitsWriter() {
} 


//=============================================================================
// update handlers
//=============================================================================
void HltRoutingBitsWriter::updateBits ( Property& /* p */ )    
{
  /// mark as "to-be-updated"
  m_bits_updated = true ;
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_preambulo_updated ) { return ; }
  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from HltRoutingBitsWriter::decode()" , sc ) ;
}

void HltRoutingBitsWriter::updatePreambulo ( Property& /* p */ )
{
  // concatenate the preambulo:
  m_preambulo = boost::algorithm::join( m_preambulo_ , "\n" );
  /// mark as "to-be-updated"
  m_preambulo_updated = true ;
  // no further action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_bits_updated ) { return ; }

  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from HltRoutingBits::decode()" , sc ) ;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltRoutingBitsWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  return decode();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsWriter::execute() {
  if (updateRequired()) {
    StatusCode sc = decode();
    if ( sc.isFailure() ) return Error(" Unable to Decode ???? ", sc);    
  }
  std::vector<unsigned int> bits(3,0); 

  unsigned int& w = bits[0]; // ODIN + L0DU
  // bits 0--7 are for ODIN
  LHCb::ODIN* odin = get<LHCb::ODIN>( m_odin_location );
  for (unsigned i=0;i<8;++i) {
        LoKi::Types::ODIN_Cut* eval = m_odin_evaluators[ i ].predicate;
        if ( eval == 0 ) continue;
        bool result = (*eval)(odin);
        *(m_odin_evaluators[ i ].counter) += result;
        always() << " " << i << " " << *eval << " = " << (result?"pass":"fail") << endmsg;
        if ( result ) w |= (0x01UL << i); 
    }

  // bits 8--32 are for L0DU
  // check if L0DU exists (may not!)
  if ( exist<LHCb::L0DUReport>( m_l0_location ) ) {
    LHCb::L0DUReport* l0du = get<LHCb::L0DUReport>( m_l0_location );
    if (l0du->valid()) {
        for (unsigned i=8;i<32;++i) {
              LoKi::Types::L0_Cut* eval = m_l0_evaluators[ i-8 ].predicate;
              if ( eval == 0 ) continue;
              bool result = (*eval)(l0du);
              *(m_l0_evaluators[ i-8 ].counter) += result;
              always() << " " << i << " " << *eval << " = " << (result?"pass":"fail") << endmsg;
              if ( result ) w |= (0x01UL << i); 
        }
    }
  }

  // bits 32--95 are for HLT
  LHCb::HltDecReports* hdr = get<LHCb::HltDecReports>( m_hlt_location );
  for (unsigned j=1;j<3;++j) {
    unsigned int& w = bits[j];
    for (unsigned i=0;i<32;++i) {
        LoKi::Types::HLT_Cut* eval = m_hlt_evaluators[ (j-1)*32+i ].predicate;
        if ( eval == 0 ) continue;
        bool result = (*eval)(hdr);
        *(m_hlt_evaluators[ (j-1)*32+i ].counter) += result;
        always() << " " << j*32+i << " " << *eval << " = " << (result?"pass":"fail") << endmsg;
        if ( result ) w |= (0x01UL << i); 
    }
  }
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  rawEvent->addBank(0,LHCb::RawBank::HltRoutingBits,0,bits);

  return StatusCode::SUCCESS;
}
