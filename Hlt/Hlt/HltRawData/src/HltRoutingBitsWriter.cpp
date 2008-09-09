// $Id: HltRoutingBitsWriter.cpp,v 1.3 2008-09-09 13:15:49 graven Exp $
// Include files 
#include <algorithm>
// from Boost
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h" 
#include "Event/HltDecReports.h" 

// local
#include "HltRoutingBitsWriter.h"


//-----------------------------------------------------------------------------
// Implementation file for class : HltRoutingBitsWriter
//
// 2008-07-29 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRoutingBitsWriter );


void HltRoutingBitsWriter::updateEvaluators(Property&) {
    if (m_factory!=0) updateEvaluators(*m_factory);
    else { m_updateRequired = true; }
}

void HltRoutingBitsWriter::updateEvaluators(const IHltDecisionPredicateFactory& factory) {
    std::for_each(m_evaluators.begin(),m_evaluators.end(),boost::lambda::delete_ptr());
    zeroEvaluators();
    typedef std::map<unsigned int,std::string>::const_iterator iter_t;
    for (iter_t i=m_specifications.begin();i!=m_specifications.end();++i) {
        if ( i->first>3*sizeof(unsigned int) ) throw GaudiException("Out of Range","",StatusCode::FAILURE);
        combiner_t* eval = factory.create(i->second);
        if (eval==0) throw GaudiException("Invalid Evaluator","",StatusCode::FAILURE);
        m_evaluators[i->first] = eval;
    }
    m_updateRequired = false;
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRoutingBitsWriter::HltRoutingBitsWriter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_factory(0)
  , m_updateRequired(true)
{
  zeroEvaluators();
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
  declareProperty("routingBitDefinitions", m_specifications)
        ->declareUpdateHandler( &HltRoutingBitsWriter::updateEvaluators, this );

}
//=============================================================================
// Destructor
//=============================================================================
HltRoutingBitsWriter::~HltRoutingBitsWriter() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltRoutingBitsWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  m_factory = tool<IHltDecisionPredicateFactory>("HltDecisionPredicateFactory");
  updateEvaluators(*m_factory);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsWriter::execute() {
  if (m_updateRequired) {
    error() << " stale evaluators???? " << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::HltDecReports* decisions = get<LHCb::HltDecReports>( m_location );
  std::vector<unsigned int> bits(3,0); 
  for (unsigned j=0;j<3;++j) {
    unsigned int& w = bits[j];
    for (unsigned i=0;i<32;++i) {
        if (evaluate(j*32+i,*decisions)) w = w | (0x01UL << i); 
    }
  }
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  rawEvent->addBank(0,LHCb::RawBank::HltRoutingBits,0,bits);

  setFilterPassed( bits[0]!=0 || bits[1]!=0 || bits[2]!=0 );

  return StatusCode::SUCCESS;
}


StatusCode HltRoutingBitsWriter::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  
}
