// $Id: HltAlgorithm.cpp,v 1.60 2009-12-24 14:13:20 graven Exp $
// Include files 

#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "HltBase/HltAlgorithm.h"
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
  // Bypass the initialization if the algorithm
  // has already been initialized.
  if ( Gaudi::StateMachine::INITIALIZED <= FSMState() ) return StatusCode::SUCCESS;
  // set up context such that tools can grab the algorithm...
  // kind of non-local, but kind of better (much more 
  // lightweight and less invasive) than the alternative.
  // Note that while GaudiAlgorithm registers the context svc
  // in sysExectute, it doesn't do so in sysInitialize...
  Gaudi::Utils::AlgContext sentry( contextSvc(), this );
  Hlt::IRegister::Lock lock(regSvc(),this);
  return HltBaseAlg::sysInitialize();
}

StatusCode HltAlgorithm::sysFinalize() {
  BOOST_FOREACH( CallBack* i, m_callbacks ) delete i ; 
  m_callbacks.clear();
  return HltBaseAlg::sysFinalize();
}


StatusCode HltAlgorithm::restart() {
  always() << "restart of " << name() << " requested -- please implement! " << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HltAlgorithm::sysExecute() {

  StatusCode sc = StatusCode::SUCCESS;

  // switch of histogramming for this event only if so requested
  histoGuard guard( m_histogramUpdatePeriod>0 && (counter("#accept").nEntries() % m_histogramUpdatePeriod !=0), *this );

  sc = beginExecute();
  if (sc.isFailure()) return sc;
  sc = HltBaseAlg::sysExecute();
  if (sc.isFailure()) return sc;
  sc = endExecute();

  return sc;

}

StatusCode HltAlgorithm::beginExecute() {
  if ( m_outputSelection == 0 ) {
      error() << " no output selection !!" << endmsg;
      return StatusCode::FAILURE;
  }

  setFilterPassed(false);

  // we always process callbacks first...
  BOOST_FOREACH( CallBack* i, m_callbacks ) { 
    StatusCode status = i->execute();
    if (!status) {

    }
  }
  
  // assert if not done properly...
  Assert(m_outputSelection->decision()==false 
      && m_outputSelection->processed()==false
      && m_outputSelection->error()==false," output already touched???");

  bool ok = verifyInput();
  
  return ok ? StatusCode::SUCCESS : StatusCode::FAILURE ;
}

StatusCode HltAlgorithm::endExecute() {
  //TODO: add flushbacks here...
  size_t n = m_outputSelection->size();
  counter("#candidates accepted") += n ; 
  // for non-counting triggers, this must be done explicity by hand!!!
  if (n>=m_minNCandidates) m_outputSelection->setDecision(true); 
  setDecision( m_outputSelection->decision() );

  if (produceHistos()) {
      for (std::vector<const Hlt::Selection*>::iterator it = m_inputSelections.begin();
           it != m_inputSelections.end(); ++it) {
        fill(m_inputHistos[(*it)->id()],(*it)->size(),1.);
      }
      fill(m_outputHisto,m_outputSelection->size(),1.);
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << " output candidates " << m_outputSelection->size() 
          << " decision " << m_outputSelection->decision() 
          << " filterpassed " << filterPassed() << endreq;
  return StatusCode::SUCCESS;
}



bool HltAlgorithm::verifyInput() 
{
  if (!m_requireInputsToBeValid) return true;
  bool ok = true;
  BOOST_FOREACH( const Hlt::Selection* i, m_inputSelections ) {
    // propagate error status!
    if (i->error()) m_outputSelection->setError(true);
    ok = ok &&  i->decision() ;
    if (msgLevel(MSG::DEBUG)) 
      debug() << " input " << i->id()
              << " decision " << i->decision() 
              << " process status " << i->processed() 
              << " error status " << i->error() 
              << " candidates " << i->size() << endreq;
  }

  if (!ok) {
    warning() << endreq;
    warning() << endreq;
    warning() << " FIXME FIXME FIXME FIXME" << endreq;
    warning() << endreq;
    warning() << " Empty input or false input selection!" << endreq;
    warning() << " Most likely due to a misconfiguration" << endreq;
    BOOST_FOREACH( const Hlt::Selection *i, m_inputSelections ) {
      warning() << " input selection " << i->id()
                << " decision " << i->decision()
                << " processed " << i->processed()
                << " error " << i->error()
                << " candidates " << i->size() << endreq;      
    }
    warning() << endreq;
    warning() << endreq;
    warning() << endreq;
    return StatusCode::FAILURE;
  }
  return ok;
}


void HltAlgorithm::setDecision(bool accept) {
  setFilterPassed(accept);
  // the next forces the 'processed' flag to be set -- even if in some 
  // cases 'accept' is obtained from m_outputSelection!!!
  // (basically, if we are of type TSelection<X>, and have no selected
  // candidates, we never explicitly set 'false', and hence would otherwise
  // not set 'processed'
  m_outputSelection->setDecision(accept);
  counter("#accept") += accept;
}


class cmp_by_id {
public:
    cmp_by_id(const Hlt::Selection& sel) : m_id( sel.id() ) {}
    bool operator()(const Hlt::Selection* i) { return  i->id() == m_id; }
    bool operator()(const Hlt::Selection& i) { return  i.id() == m_id; }
private:
    Gaudi::StringKey m_id;
};


const Hlt::Selection& HltAlgorithm::retrieveSelection(const Gaudi::StringKey& selname) {
    Assert(!selname.empty()," retrieveSelection() no selection name");
    if (msgLevel(MSG::DEBUG)) debug() << " retrieveSelection " << selname << endreq;
    StatusCode sc = regSvc()->registerInput(selname,this);
    if (sc.isFailure()) {
      error() << " failed to register input " << selname << endreq;
      Assert(0," retrieveSelection, failed to register input!");
        
    }
    const Hlt::Selection* sel = hltSvc()->selection(selname,this);
    if (sel == 0 ) {
      error() << " failed to retrieve input " << selname << endreq;
      Assert(0," retrieveSelection, failed to retrieve input!");
    }
    if (std::find_if(m_inputSelections.begin(),
                     m_inputSelections.end(), 
                     cmp_by_id(*sel))==m_inputSelections.end() ) {
      m_inputSelections.push_back(sel);
      if (produceHistos()) {
        Assert(m_inputHistos.find(sel->id()) == m_inputHistos.end(),
               "retrieveSelection() already input selection "+sel->id().str());
        m_inputHistos[sel->id()] = initializeHisto(sel->id().str());
      }
      if (msgLevel(MSG::DEBUG)) debug() << " Input selection " << sel->id() << endreq;
    }
    if (msgLevel(MSG::DEBUG)) debug() << " retrieved selection " << sel->id() << endreq;    
    return *sel;
}

  
void HltAlgorithm::setOutputSelection(Hlt::Selection* sel) {
    m_outputSelection = sel;
    sel->addInputSelectionIDs( m_inputSelections.begin(), m_inputSelections.end() );
    if (msgLevel(MSG::DEBUG)) debug() << " Output selection " << sel->id() << endreq;
    if (regSvc()->registerOutput(sel,this).isFailure()) {
       throw GaudiException("Failed to add Selection",sel->id().str(),StatusCode::FAILURE);
    }
    if (produceHistos()) m_outputHisto = initializeHisto(sel->id().str());
    if (msgLevel(MSG::DEBUG)) debug() << " registered selection " << sel->id() << " type " << sel->classID() << endreq;
}
