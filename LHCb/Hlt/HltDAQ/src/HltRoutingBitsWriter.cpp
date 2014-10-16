// Include files 
// from Boost
#include <boost/format.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"

#include "Event/RawEvent.h" 
#include "Event/RawBank.h" 
#include "Event/HltDecReports.h" 
#include "Event/L0DUReport.h" 
#include "Event/ODIN.h" 
#include "boost/algorithm/string/join.hpp"
#include "AIDA/IHistogram1D.h"
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
DECLARE_ALGORITHM_FACTORY( HltRoutingBitsWriter )

StatusCode HltRoutingBitsWriter::decode() {
    zeroEvaluators();
    LoKi::Hybrid::IHltFactory* factory = tool<LoKi::Hybrid::IHltFactory>( "LoKi::Hybrid::HltFactory" ) ;

    int nBins = int(m_timeSpan/m_binWidth+0.5);

    typedef std::map<unsigned int,std::string>::const_iterator iter_t;
    for (iter_t i=m_bits.begin();i!=m_bits.end();++i) {
        if ( i->first>nBits ) return StatusCode::FAILURE;
        if (!i->second.empty()) { 
            std::string title = boost::str(boost::format("%02d:%s") % i->first % i->second) ;
            std::string htitle = boost::str(boost::format("RoutingBit%02d")%i->first);
            if (i->first<8) { 
                LoKi::Types::ODIN_Cut cut( LoKi::BasicFunctors<const LHCb::ODIN*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_odin_evaluators[i->first].predicate = cut.clone();
                m_odin_evaluators[i->first].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% htitle ),
                        *m_odin_evaluators[i->first].counter,htitle);
                m_odin_evaluators[i->first].hist   = book1D(htitle,0,nBins*m_binWidth,nBins); //TODO: set AxisLabels
            } else if (i->first<32) {
                LoKi::Types::L0_Cut cut( LoKi::BasicFunctors<const LHCb::L0DUReport*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_l0_evaluators[i->first-8].predicate = cut.clone();
                m_l0_evaluators[i->first-8].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% htitle ),
                        *m_l0_evaluators[i->first-8].counter,htitle);
                m_l0_evaluators[i->first-8].hist   = book1D(htitle,0,nBins*m_binWidth,nBins); //TODO: set AxisLabels
            } else {
                LoKi::Types::HLT_Cut cut( LoKi::BasicFunctors<const LHCb::HltDecReports*>::BooleanConstant( false ) );
                StatusCode sc = factory->get( i->second, cut, m_preambulo );
                if (sc.isFailure()) return sc;
                m_hlt_evaluators[i->first-32].predicate = cut.clone();
                m_hlt_evaluators[i->first-32].counter   = &counter(title);
                declareInfo(boost::str( boost::format("COUNTER_TO_RATE[%s]")% htitle ),
                        *m_hlt_evaluators[i->first-32].counter,htitle);
                m_hlt_evaluators[i->first-32].hist   = book1D(htitle,0,nBins*m_binWidth,nBins); //TODO: set AxisLabels
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
    for( odin_eval_t& eval : m_odin_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = nullptr;
        eval.counter = nullptr;
        eval.hist = nullptr;
    }
    for( l0_eval_t& eval : m_l0_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = nullptr;
        eval.counter = nullptr;
        eval.hist = nullptr;
    }
    for( hlt_eval_t& eval : m_hlt_evaluators ) {
        if (!skipDelete) { delete eval.predicate; }
        eval.predicate = nullptr;
        eval.counter = nullptr;
        eval.hist = nullptr;
    }
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRoutingBitsWriter::HltRoutingBitsWriter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_runpars{nullptr}
  , m_updMgrSvc{nullptr}
  , m_startOfRun(0)
  , m_binWidth(1) // in minutes!!! 
  , m_timeSpan(125) // in minutes!!!
  , m_bits_updated(false)
  , m_preambulo_updated(false)
{
  zeroEvaluators(true);
  declareProperty("ODINLocation", m_odin_location = LHCb::ODINLocation::Default);
  declareProperty("L0DUReportLocation", m_l0_location = LHCb::L0DUReportLocation::Default);
  declareProperty("Hlt1DecReportsLocation", m_hlt_location[0] = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt2DecReportsLocation", m_hlt_location[1] = LHCb::HltDecReportsLocation::Default);
  declareProperty("RoutingBits", m_bits) ->declareUpdateHandler( &HltRoutingBitsWriter::updateBits, this );
  declareProperty("Preambulo", m_preambulo_)->declareUpdateHandler(&HltRoutingBitsWriter::updatePreambulo , this);
  declareProperty("TrendTimeSpan",m_timeSpan = 125 );
  declareProperty("TrendBinWidth",m_binWidth = 1 );
  declareProperty("GetStartOfRunFromCondDB",m_useCondDB = true);
  declareProperty("UpdateExistingRawBank",m_updateBank = false);

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
  if (m_useCondDB) {
      m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc");
      if( !sc.isSuccess()) {
          error()<< "Could not update UpdateManagerSvc" << endmsg;
          return sc;
      }
      m_updMgrSvc->registerCondition( this, "Conditions/Online/LHCb/RunParameters",
                                           &HltRoutingBitsWriter::i_updateConditions,m_runpars );
      sc = m_updMgrSvc->update(this);
      if( !sc.isSuccess()) return sc;
  } else {
      // reset m_startOfRun to zero at start of run....
      IIncidentSvc *s = svc<IIncidentSvc>( "IncidentSvc");
      s->addListener(this,IncidentType::BeginRun, 0,false,false);
      s->addListener(this,"RunChange", 0,false,false);
  }
  return decode();
}

void HltRoutingBitsWriter::handle(const Incident&) {
    m_startOfRun = 0;
}

StatusCode
HltRoutingBitsWriter::i_updateConditions()
{
    if (m_runpars==0) { 
        error() << "Could not obtain Condition for run parameters from conditions DB" << endmsg;
        return StatusCode::FAILURE;
    }
    if (!m_runpars->exists("RunStartTime")) {
        error() << "Condition does not contain RunStartTime " << endmsg;
        return StatusCode::FAILURE;
    }
    m_startOfRun = (long long unsigned int) (m_runpars->param<int>("RunStartTime")*1e6); //from seconds -> microseconds since 1/1/1970
    return StatusCode::SUCCESS;
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

  // bits 0--7 are for ODIN
  LHCb::ODIN* odin = get<LHCb::ODIN>( m_odin_location );
  
  if (m_startOfRun == 0) m_startOfRun = odin->gpsTime(); // just in case we did not have the conditions DB start-of-run time...

  // go from microseconds to minutes...
  double t = odin->gpsTime()>=m_startOfRun ? double(odin->gpsTime() - m_startOfRun) : -double(m_startOfRun - odin->gpsTime()) ;
  t /= 60e6;

  // always() << " minutes since start of run: " << t << " gpstime: " << odin->gpsTime() << " startOfRun: " << m_startOfRun << endmsg;

  double weight = double(1)/(m_binWidth*60); // m_binWidth is in minutes, need rate in Hz

  for (unsigned i=0;i<8;++i) {
        auto* eval = m_odin_evaluators[ i ].predicate;
        if ( !eval ) continue;
        bool result = (*eval)(odin);
        *(m_odin_evaluators[ i ].counter) += result;
        if ( result ) m_odin_evaluators[ i ].hist->fill( t,  weight ); 
        if ( result ) bits[0] |= (0x01UL << i); 
  }

  // bits 8--32 are for L0DU
  // check if L0DU exists (may not!)
  if ( exist<LHCb::L0DUReport>( m_l0_location ) ) {
    LHCb::L0DUReport* l0du = get<LHCb::L0DUReport>( m_l0_location );
    if (l0du->valid()) {
        for (unsigned i=8;i<32;++i) {
              auto* eval = m_l0_evaluators[ i-8 ].predicate;
              if ( !eval ) continue;
              bool result = (*eval)(l0du);
              *(m_l0_evaluators[ i-8 ].counter) += result;
              if ( result ) m_l0_evaluators[ i-8 ].hist->fill( t, weight ); 
              if ( result ) bits[0] |= (0x01UL << i); 
        }
    }
  }

  // bits 32 -- 63 are Hlt1, 64 -- 95 for Hlt2
  for (unsigned j=1;j<3;++j) {
    if ( m_hlt_location[j-1].empty() ) continue;
    LHCb::HltDecReports* hdr = get<LHCb::HltDecReports>( m_hlt_location[j-1] );
    for (unsigned i=0;i<32;++i) {
        auto* eval = m_hlt_evaluators[ (j-1)*32+i ].predicate;
        if ( !eval ) continue;
        bool result = (*eval)(hdr);
        *(m_hlt_evaluators[ (j-1)*32+i ].counter) += result;
        if ( result ) m_hlt_evaluators[ (j-1)*32+i ].hist->fill( t, weight ); 
        if ( result ) bits[j] |= (0x01UL << i); 
    }
  }
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);

  if (m_updateBank) {
        std::vector<LHCb::RawBank*> banks = rawEvent->banks( LHCb::RawBank::HltRoutingBits );
        if (banks.size()!=1) { 
                return Error( " Multiple RoutingBits RawBanks -- don't know which to update. Skipping... ",
                       StatusCode::SUCCESS, 20 );
        }
        LHCb::RawBank *bank = banks.front();
        if (bank->size()!=3*sizeof(unsigned int) ) {
                return Error( " RoutingBits RawBanks has unexpected size.. Skipping",
                             StatusCode::SUCCESS, 20 );
        }
        unsigned int *data = bank->data();
        if (data[0]!=bits[0] || data[1]!=bits[1]) {
                Warning( " RoutingBits RawBanks: requested to update bank, but first two entries not the same" ,
                         StatusCode::SUCCESS, 20 ).ignore();
        }
        if (data[2]!=0 ) {
                Warning( " RoutingBits RawBanks: requested to update bank, but non-zero third entry",
                         StatusCode::SUCCESS, 20 ).ignore();
        }
        data[2] = bits[2];
  } else {
        if ( !rawEvent->banks( LHCb::RawBank::HltRoutingBits ).empty() ) {
                Warning( " Pre-existing RoutingBits bank in the event...",
                         StatusCode::SUCCESS, 0 ).ignore();
        }
        rawEvent->addBank(0,LHCb::RawBank::HltRoutingBits,0,bits);
  }

  return StatusCode::SUCCESS;
}

// Not sure why this helps here, but it does suppress the warning!
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // BOOST_ASSERT controlling expression is constant
#endif
