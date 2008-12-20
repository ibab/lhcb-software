// $Id: HltAlgorithm.cpp,v 1.47 2008-12-20 19:22:41 graven Exp $
// Include files 

#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/ESequences.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/foreach.hpp"

namespace bl = boost::lambda;
//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------
namespace {
    class histoGuard {
    public:
        histoGuard(bool switchOff,HltAlgorithm& parent) : m_parent(parent), m_flag( parent.produceHistos() && switchOff) {
            if (m_flag) m_parent.setProduceHistos(false);
        }
        ~histoGuard() {
            if (m_flag) m_parent.setProduceHistos(true);
        }
    private:
        HltAlgorithm& m_parent;
        bool m_flag;
    };
};

HltAlgorithm::HltAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator,
                            bool requireInputsToBeValid )
  : HltBaseAlg ( name , pSvcLocator )
  , m_requireInputsToBeValid(requireInputsToBeValid)
  , m_outputSelection(0)
  , m_outputHisto(0)
{
  declareProperty("RequirePositiveInputs", m_requireInputsToBeValid );
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 0 );
  declareProperty("SaveConfiguration", m_saveConf = true);

  //TODO: in init, declare these to mon svc...
  counter("#accept");
  counter("#candidates accepted");


  //TODO: since this is not applicable to all algorithms, remove from base...
  declareProperty("MinCandidates",m_minNCandidates = 1);
}

HltAlgorithm::~HltAlgorithm()
{
  delete m_outputSelection; m_outputSelection = 0;
} 


StatusCode HltAlgorithm::sysInitialize() {
  // set up context such that tools can grab the algorithm...
  // kind of non-local, but kind of better (much more 
  // lightweight and less invasive) than the alternative.
  // Note that while GaudiAlgorithm registers the context svc
  // in sysExectute, it doesn't do so in sysInitialize...
  Gaudi::Utils::AlgContext sentry( contextSvc(), this );
  return HltBaseAlg::sysInitialize();
}


StatusCode HltAlgorithm::restart() {
  always() << "restart of " << name() << " requested -- please implement! " << endmsg;
  return StatusCode::SUCCESS;
}



void HltAlgorithm::saveConfiguration() {
//@TODO: move _all_ of this (implicitly) to IHltDataSvc...

  
  if (!m_saveConf) return;
  if ( m_outputSelection == 0 ) {
    warning() << "m_outputSelection is NULL. Not saving config." << endmsg ;
    return ;
  }
  verbose() << "Saving Config " << m_outputSelection->id() << endmsg ;

  CLID clID = m_outputSelection->classID();
  verbose() << " classID: " << m_outputSelection->classID() << endmsg ;

  std::string type = ( clID == LHCb::Track::classID()     ?  "Track"    :
                       clID == LHCb::RecVertex::classID() ?  "Vertex"   :
                       clID == LHCb::Particle::classID()  ?  "Particle" :
                                                             "unknown" );
  verbose() << "Type : " << type << endmsg ;
  
  std::string algoType =  System::typeinfoName(typeid(*this));

  confregister("Algorithm",algoType+"/"+name(), m_outputSelection->id().str());
  confregister("SelectionType",type,            m_outputSelection->id().str());

  const std::vector<stringKey>& in = m_outputSelection->inputSelectionsIDs();
  std::vector<std::string> inames;
  transform(in.begin(),in.end(),std::back_inserter(inames),bl::bind(&stringKey::str,bl::_1));

  confregister("InputSelections",inames,m_outputSelection->id().str());
  verbose() << "Done saveConfigureation" << endmsg ;  
  info() << " HLT flow: " << m_outputSelection->inputSelectionsIDs() 
         << " --> "       << m_outputSelection->id() << endreq;
}


StatusCode HltAlgorithm::sysExecute() {

  StatusCode sc = StatusCode::SUCCESS;

  // switch of histogramming for this event only if so requested
  histoGuard guard( m_histogramUpdatePeriod>0 && (counter("#accept").nEntries() % m_histogramUpdatePeriod !=0), *this );

  
  //TODO: add try/catch around this, and set error flags...
  //      need to know the 'terminus' of the current sequence (probably
  //      need an explicit property for that, cannot discover ourselves),
  //      and execute that one explicitly so it can record the problem...
  //NOTE: GaudiKernel/Algorithm.cpp does a 'try/catch' in sysExecute
  //      so the above wouldn't catch anything unless we make 
  //      sure that the IExceptionSvc which one gets as 'ExceptionSvc' 
  //      does something reasonable -- by default it 'rethrows'... 
  try {
    sc = beginExecute();
    if (sc.isFailure()) return sc;
    sc = HltBaseAlg::sysExecute();
    if (sc.isFailure()) return sc;
    sc = endExecute();
  } catch(...) {
    sc = StatusCode::FAILURE; // for now -- maybe at some point we try to continue...
    setDecision(true); // try to keep event for debugging...
    m_outputSelection->setError(true);
  }
  

  return sc;

}

StatusCode HltAlgorithm::beginExecute() {
  if ( m_outputSelection == 0 ) {
      error() << " no output selection !!" << endmsg;
      return StatusCode::FAILURE;
  }
  Assert( m_outputSelection != 0," beginExecute() no output selection !");

  setDecision(false);

  // we always process callbacks first...
  BOOST_FOREACH( CallBack* i, m_callbacks ) i->process();
  
  m_outputSelection->clean();
  bool ok = verifyInput();
  if (produceHistos()) monitorInputs();
  
  return ok ? StatusCode::SUCCESS : StatusCode::FAILURE ;
}

StatusCode HltAlgorithm::endExecute() {
  //TODO: add flushbacks here...
  setDecision();
  if (produceHistos()) monitorOutput();
  
  debug() << " output candidates " << m_outputSelection->ncandidates() 
          << " decision " << m_outputSelection->decision() 
          << " filterpassed " << filterPassed() << endreq;
  return StatusCode::SUCCESS;
}



bool HltAlgorithm::verifyInput() 
{
  if (!m_requireInputsToBeValid) return true;
  bool ok = true;
  BOOST_FOREACH( Hlt::Selection* i, m_inputSelections ) {
    // propagate error status!
    if (i->error()) m_outputSelection->setError(true);
    ok = ok &&  i->decision() ;
    if (m_debug) 
      debug() << " input " << i->id()
              << " decision " << i->decision() 
              << " process status " << i->processed() 
              << " error status " << i->error() 
              << " candidates " << i->ncandidates() << endreq;
  }

  if (!ok) {
    warning() << endreq;
    warning() << endreq;
    warning() << " FIXME FIXME FIXME FIXME" << endreq;
    warning() << endreq;
    warning() << " Empty input or false input selection!" << endreq;
    warning() << " Most likely due to a misconfiguration" << endreq;
    BOOST_FOREACH( Hlt::Selection *i, m_inputSelections ) {
      warning() << " input selection " << i->id()
                << " decision " << i->decision()
                << " processed " << i->processed()
                << " error " << i->error()
                << " candidates " << i->ncandidates() << endreq;      
    }
    warning() << endreq;
    warning() << endreq;
    warning() << endreq;
    return StatusCode::FAILURE;
  }
  return ok;
}

void HltAlgorithm::monitorInputs() 
{
  if (!produceHistos()) return;
  for (std::vector<Hlt::Selection*>::iterator it = m_inputSelections.begin();
       it != m_inputSelections.end(); ++it) {
    fillHisto(*m_inputHistos[(*it)->id()],(*it)->ncandidates(),1.);
  }
  // verbose() << " end monitor inputs " <<endreq;
}

void HltAlgorithm::monitorOutput() {
  if (!produceHistos()) return;
  size_t nCandidates = m_outputSelection->ncandidates();
  Assert( 0 != m_outputHisto," monitorOutput() no output histo ");
  fillHisto(*m_outputHisto,nCandidates,1.);
}

//@TODO: 
//@FIXME: 
// in case we have candidates, the decision is taken by the algorithm from the selection,
// but in case there are no candidates, we MUST call setDecision(bool) explicitly!!
void HltAlgorithm::setDecision() {
  Assert (0 != m_outputSelection," setDecision() no output selection! ");
  size_t n = m_outputSelection->ncandidates();
  counter("#candidates accepted") += n ; 
  if (n>=m_minNCandidates) m_outputSelection->setDecision(true); // for non-counting triggers, this must be done explicity by hand!!!
  setDecision( m_outputSelection->decision() );
  counter("#accept") += m_outputSelection->decision();
}

void HltAlgorithm::setDecision(bool accept) {
  setFilterPassed(accept);
  // the next forces the 'processed' flag to be set -- even if in some 
  // cases 'accept' is obtained from m_outputSelection!!!
  // (basically, if we are of type TSelection<X>, and have no selected
  // candidates, we never explicitly set 'false', and hence would otherwise
  // not set 'processed'
  m_outputSelection->setDecision(accept);
}


class cmp_by_id {
public:
    cmp_by_id(const Hlt::Selection& sel) : m_id( sel.id() ) {}
    bool operator()(const Hlt::Selection* i) { return  i->id() == m_id; }
    bool operator()(const Hlt::Selection& i) { return  i.id() == m_id; }
private:
    stringKey m_id;
};


Hlt::Selection& HltAlgorithm::retrieveSelection(const stringKey& selname) {
    Assert(!selname.empty()," retrieveSelection() no selection name");
    debug() << " retrieveSelection " << selname << endreq;
    if (!dataSvc().hasSelection(selname)) {
      error() << " unknown selection " << selname << endreq;
      Assert(0," retrieveSelection, unknown selection!");
    }
    Hlt::Selection& sel = dataSvc().selection(selname,this);
    if (std::find_if(m_inputSelections.begin(),
                     m_inputSelections.end(), 
                     cmp_by_id(sel))==m_inputSelections.end() ) {
      m_inputSelections.push_back(&sel);
      if (produceHistos()) {
        Assert(m_inputHistos.find(sel.id()) == m_inputHistos.end(),"retrieveSelection() already input selection "+sel.id().str());
        m_inputHistos[sel.id()] = initializeHisto(sel.id().str());
      }
      debug() << " Input selection " << sel.id() << endreq;
    }
    debug() << " retrieved selection " << sel.id() << endreq;    
    return sel;
}

  
void HltAlgorithm::setOutputSelection(Hlt::Selection* sel) {
    m_outputSelection = sel;
    sel->addInputSelectionIDs( m_inputSelections.begin(), m_inputSelections.end() );
    debug() << " Output selection " << sel->id() << endreq;
    StatusCode sc = dataSvc().addSelection(sel,this,false);
    if (sc.isFailure()) {
       throw GaudiException("Failed to add Selection",sel->id().str(),StatusCode::FAILURE);
    }
    if (produceHistos()) m_outputHisto = initializeHisto(sel->id().str());
    debug() << " registered selection " << sel->id() << " type " << sel->classID() << endreq;
}
