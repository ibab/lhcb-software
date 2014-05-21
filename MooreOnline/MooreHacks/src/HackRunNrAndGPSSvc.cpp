// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "Event/ODIN.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Time.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"

// local
#include "HackRunNrAndGPSSvc.h"
//-----------------------------------------------------------------------------
// Implementation file for class : 
//
// 2014-05-01 : Gerhard Raven
//-----------------------------------------------------------------------------



// Declaration of the Algorithm Factory
DECLARE_SERVICE_FACTORY( HackRunNrAndGPSSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HackRunNrAndGPSSvc::HackRunNrAndGPSSvc( const std::string& name, ISvcLocator* pSvcLocator )
    : extends1<Service, IIncidentListener>( name, pSvcLocator )
    , m_dim{ "RunInfo/LHCb1/RunNumber" }
    , m_incident{ "APP_RUNNING" }
    , m_runnr{ -1 }
    , m_odinTool{ "ODINDecodeTool", this }
    , m_evtSvc{ nullptr }
    , m_run{ nullptr }
{
    declareProperty("RunNumberDimService" , m_dim );
    declareProperty("FixedRunNumber" , m_runnr );
    declareProperty("Incident" , m_incident );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HackRunNrAndGPSSvc::initialize()
{
    always() << "HackRunNrAndGPSSvc::initialize; " << m_dim << " " << m_runnr << endmsg;
    if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;

    // Tool to decode ODIN object from RawEvent
    StatusCode sc = m_odinTool.retrieve();
    if ( !sc.isSuccess() ) return sc;

    IIncidentSvc* svc{ nullptr } ;
    if ( !service( "IncidentSvc", svc ).isSuccess() ) return StatusCode::FAILURE;
    // add listener to be triggered by first BeginEvent with high priority
    // so it gets called first
    bool rethrow = false;
    bool oneShot = false;
    svc->addListener(this,IncidentType::BeginEvent,
                          std::numeric_limits<long>::max(),rethrow,oneShot);

    svc->addListener(this,m_incident, 0,rethrow,oneShot);
  

    return StatusCode::SUCCESS;
}

void HackRunNrAndGPSSvc::Assert( const bool         ok  ,
                                 std::string        msg ,
                                 std::string        tag ,
                                 const StatusCode   sc  ) const
{
  if (!ok) throw GaudiException( msg , tag, sc );
}
// ============================================================================

//=============================================================================
// Finalization
//=============================================================================
StatusCode HackRunNrAndGPSSvc::finalize()
{
    m_evtSvc->release();
    m_evtSvc = nullptr;
    return StatusCode::SUCCESS;
}

//=============================================================================
// start
//=============================================================================
StatusCode HackRunNrAndGPSSvc::start()
{
        return StatusCode::SUCCESS;
}

void HackRunNrAndGPSSvc::getDim()
{
        always() << "HackRunNrAndGPSSvc::getDim; " << m_dim << " " << m_runnr << endmsg;
        if (!m_dim.empty() && m_runnr <= 0 ) {
            always() << "Requesting run number from DIM service " << m_dim << endmsg;
            m_run.reset( new RunNumber(m_dim.c_str()) );
        }
}
//=============================================================================
// stop
//=============================================================================
StatusCode HackRunNrAndGPSSvc::stop()
{
        m_run.reset();
        return StatusCode::SUCCESS;
}

void 
HackRunNrAndGPSSvc::pokeODINRawBank( LHCb::RawBank *bank, unsigned runnr, Gaudi::Time time ) 
{
  Assert(bank->type() == LHCb::RawBank::ODIN, "Wrong ODIN raw bank type",name(),StatusCode::FAILURE);
  Assert(bank->magic() == LHCb::RawBank::MagicPattern, "Magic pattern mismatch in ODIN raw bank",name(),StatusCode::FAILURE);
  Assert(bank->version() == 6, "Can only handle ODIN version 6" ,name(),StatusCode::FAILURE);

  // pointer to the data
  unsigned int* data = bank->data();


  data[LHCb::ODIN::RunNumber]=runnr;

  ulonglong t = time.ns()/1000;

  data[LHCb::ODIN::GPSTimeHi]   =   ( t >> 32) & 0xFFFFFFFFul ;
  data[LHCb::ODIN::GPSTimeLo]   =   t  & 0xFFFFFFFFul ;
}

void HackRunNrAndGPSSvc::poke( LHCb::RawEvent *event, unsigned runnr ) {

        auto& odin = event->banks(LHCb::RawBank::ODIN);
        always() << " got " << odin.size() << " ODIN raw banks " << endmsg;
        if (!odin.empty()) pokeODINRawBank( odin.front(), runnr, Gaudi::Time::current());
    
        auto& daq = event->banks(LHCb::RawBank::DAQ);

        auto i = std::find_if( std::begin(daq), std::end(daq), 
                               [](const LHCb::RawBank *b) { 
                return  b->version() == DAQ_STATUS_BANK && b->type() == LHCb::RawBank::DAQ ; 
        } );
        if (i!=std::end(daq) ) {
            auto* hdr = reinterpret_cast<LHCb::MDFHeader*>( (*i)->data());
            auto  h = hdr->subHeader();
            h.H1->setRunNumber(runnr);
            always() << " poked runnr " << runnr << " into MDFHeader.SubHeader1 " << endmsg;
        } else {
            error() << "could not get DAQ_STATUS_BANK" << endmsg;
        }
}

void HackRunNrAndGPSSvc::handle( const Incident& incident )
{
    if ( incident.type() == m_incident ) getDim();
    if ( incident.type() == IncidentType::BeginEvent ) update();
}

void HackRunNrAndGPSSvc::update()
{
    // check that  ODIN is not yet present in the TES!!!! We need to get it first!!!
    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc, LHCb::ODINLocation::Default );
    if ( !!odin ) {
        error() << "Too late:  ODIN already decoded into TES..." << endmsg;
    }
    SmartDataPtr<LHCb::RawEvent> event( m_evtSvc, LHCb::RawEventLocation::Default );
    if ( !event) {
        error() << "No raw event at " <<  LHCb::RawEventLocation::Default  << endmsg;
    } else {
        unsigned runnr = m_run ? (*m_run)() : m_runnr ;
        poke( event, runnr );
    }

    if ( !odin ) { 
        always() << "requesting decode of ODIN raw bank..." << endmsg;
        m_odinTool->execute();
    }
    if ( !odin ) {
        error() << " Could not obtain ODIN...  " << endmsg;
    } else {
        always() << " decoded ODIN gives: " <<  odin->runNumber() << " " << odin->eventNumber() << " " << odin->eventTime() << endmsg;
    }
}
