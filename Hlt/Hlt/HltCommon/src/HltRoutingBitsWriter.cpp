// $Id: HltRoutingBitsWriter.cpp,v 1.1 2008-07-30 13:37:32 graven Exp $
// Include files 
#include <algorithm>
// from Boost
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h" 

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
    std::cout << "updateEvaluators callback!" << std::endl;
    if (m_factory!=0) updateEvaluators(*m_factory);
    else { m_updateRequired = true; }
}

void HltRoutingBitsWriter::updateEvaluators(const HltRoutingBitEvaluatorFactory& factory) {
    std::for_each(m_evaluators.begin(),m_evaluators.end(),boost::lambda::delete_ptr());
    zeroEvaluators();
    typedef std::map<unsigned int,std::string>::const_iterator iter_t;
    for (iter_t i=m_specifications.begin();i!=m_specifications.end();++i) {
        if ( i->first>95 ) throw GaudiException("Out of Range","",StatusCode::FAILURE);
        HltRoutingBitEvaluatorFactory::evaluator* eval = factory.create(i->second,*this);
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
  : HltAlgorithm ( name , pSvcLocator, false )
  , m_selection(*this)
  , m_factory(0)
  , m_updateRequired(true)
{
  zeroEvaluators();
  m_selection.declareProperties();
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
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith


  m_factory = tool<HltRoutingBitEvaluatorFactory>("HltRoutingBitEvaluatorFactory");
  updateEvaluators(*m_factory);
  
  m_selection.registerSelection();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRoutingBitsWriter::execute() {
  if (m_updateRequired) return StatusCode::FAILURE;
  bool any = false;
  std::vector<unsigned int> bits(3,0); 
  unsigned int mask = 0x00000001; unsigned j=0;
  for (unsigned i=0;i<NBits;++i) {
        if (evaluate(i)) { 
            bits[j] = bits[j] || mask; 
            any=true;
        }
        // print(std::cout,i) << " --> " << (evaluate(i)?"true":"false") << " " << mask << " " << j << std::endl;
        mask<<=1;
        if (mask==0) { mask = 0x00000001; ++j; } //rollover..
  }
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  rawEvent->addBank(0,LHCb::RawBank::HltRoutingBits,0,bits);

  m_selection.output()->setDecision(any);

  return StatusCode::SUCCESS;
}


StatusCode HltRoutingBitsWriter::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  
}
