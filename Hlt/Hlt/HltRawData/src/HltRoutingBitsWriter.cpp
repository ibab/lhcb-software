// $Id: HltRoutingBitsWriter.cpp,v 1.7 2009-02-24 13:50:27 graven Exp $
// Include files 
// from Boost
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h" 
#include "Event/HltDecReports.h" 
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
    std::for_each(m_evaluators.begin(),m_evaluators.end(),boost::lambda::delete_ptr());
    zeroEvaluators();
    LoKi::Hybrid::IHltFactory* factory = tool<LoKi::Hybrid::IHltFactory>( "LoKi::Hybrid::HltFactory" ) ;

    typedef std::map<unsigned int,std::string>::const_iterator iter_t;
    for (iter_t i=m_bits.begin();i!=m_bits.end();++i) {
        if ( i->first>nBits ) return StatusCode::FAILURE;
        if (i->second.empty()) { 
            m_evaluators[i->first] = 0;
        } else {
            LoKi::Types::HLT_Cut cut( LoKi::BasicFunctors<const LHCb::HltDecReports*>::BooleanConstant( false ) );
            StatusCode sc = factory->get( i->second, cut, m_preambulo );
            if (sc.isFailure()) return sc;
            m_evaluators[i->first] = cut.clone();
        }
    }
    this->release(factory);
    m_bits_updated      = false ;
    m_preambulo_updated = false ;
    return StatusCode::SUCCESS;
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
  zeroEvaluators();
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
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

  LHCb::HltDecReports* hdr = get<LHCb::HltDecReports>( m_location );
  std::vector<unsigned int> bits(3,0); 
  for (unsigned j=0;j<3;++j) {
    unsigned int& w = bits[j];
    for (unsigned i=0;i<32;++i) {
        LoKi::Types::HLT_Cut* eval = m_evaluators[ j*32+i ];
        if ( eval == 0 ) continue;
        bool result = (*eval)(hdr);
        // always() << " " << j*32+i << " " << *eval << " = " << (result?"pass":"fail") << endmsg;
        if ( result ) w |= (0x01UL << i); 
    }
  }
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  rawEvent->addBank(0,LHCb::RawBank::HltRoutingBits,0,bits);

  return StatusCode::SUCCESS;
}
