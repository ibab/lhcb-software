#include <limits>
#include <numeric>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiAlg/IGenericTool.h"
#include "Kernel/IReferenceRate.h"
#include "DetDesc/Condition.h"
#include "LoKi/IHltFactory.h"
#include "boost/algorithm/string/join.hpp"

class HltReferenceRateSvc : public Service,
                            virtual public IReferenceRate,
                            virtual public IIncidentListener

{
  public:
    HltReferenceRateSvc( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~HltReferenceRateSvc();

    /** Query interfaces (\see{IInterface})
        @param riid       ID of Interface to be retrieved
        @param ppvUnknown Pointer to Location for interface pointer
    */
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvUnknown );

    /// Initialize Service
    StatusCode initialize();

    /// Finalize  Service
    StatusCode finalize();

    /// Handle BeginEvent incident.
    void handle( const Incident& );

    /// Return the rate (reciproke period) of this clock
    double rate() const
    {
        return m_rate;
    }

    /// Return the current tick of this clock
    size_t tick() const
    {
        return m_tick;
    }

    StatusCode i_updateConditions();

  private:
    MsgStream& info() const
    {
        return msg( MSG::INFO );
    }
    MsgStream& error() const
    {
        return msg( MSG::ERROR );
    }
    MsgStream& msg( MSG::Level level ) const;

    void report() const;

  private:
    mutable std::unique_ptr<MsgStream> m_msg;
    IIncidentSvc* m_incidentSvc;
    IDataProviderSvc* m_evtSvc;
    IToolSvc* m_toolSvc;
    IUpdateManagerSvc* m_updMgrSvc;
    ToolHandle<IGenericTool> m_decodeOdin;
    Condition* m_lumipars;
    std::string m_location;
    double m_rate;
    mutable ulonglong m_first, m_last;
    mutable size_t m_tick;
    mutable size_t m_lastReportedTick;
    bool m_useCondDB;
};

/************************************************************************/

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/ODIN.h"

DECLARE_SERVICE_FACTORY( HltReferenceRateSvc )

//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode HltReferenceRateSvc::queryInterface( const InterfaceID& riid,
                                                void** ppvUnknown )
{
    if ( IReferenceRate::interfaceID().versionMatch( riid ) ) {
        *ppvUnknown = (IReferenceRate*)this;
        addRef();
        return StatusCode::SUCCESS;
    }
    return Service::queryInterface( riid, ppvUnknown );
}

HltReferenceRateSvc::~HltReferenceRateSvc()
{
}

HltReferenceRateSvc::HltReferenceRateSvc( const std::string& name,
                                          ISvcLocator* pSvcLocator )
    : Service( name, pSvcLocator )
    , m_incidentSvc( 0 )
    , m_evtSvc( 0 )
    , m_toolSvc( 0 )
    , m_updMgrSvc( 0 )
    , m_decodeOdin( "ODINDecodeTool", this )
    , m_lumipars( 0 )
    , m_rate( 0 )
    , m_first( ~ulonglong( 0 ) )
    , m_last( 0 )
    , m_tick( 0 )
    , m_lastReportedTick( 0 )
{
    // by default, we assume 50(beam-beam)+10(beam1)+10(beam2)+10(empty) Hz of lumi
    // triggers
    declareProperty( "ReferenceRate", m_rate = 80 /*Gaudi::Units::Hz*/ );
    declareProperty( "ODINLocation", m_location = LHCb::ODINLocation::Default );
    declareProperty( "UseCondDB", m_useCondDB = true );
}

MsgStream& HltReferenceRateSvc::msg( MSG::Level level ) const
{
    if ( m_msg.get() == 0 ) m_msg.reset( new MsgStream( msgSvc(), name() ) );
    *m_msg << level;
    return *m_msg;
}

StatusCode HltReferenceRateSvc::i_updateConditions()
{
    report();
    // and update settings
    debug() << "updating lumipars" << endmsg;
    if ( m_lumipars == 0 ) {
        warning()
            << "Could not obtain Condition for lumi parameters from conditions DB"
            << endmsg;
        return StatusCode::FAILURE;
    }
    if ( !m_lumipars->exists( "LumiPars" ) ) {
        warning() << "Condition does not contain LumiPars " << endmsg;
        return StatusCode::FAILURE;
    }
    std::vector<double> lumipars =
        m_lumipars->param<std::vector<double>>( "LumiPars" );
    debug() << "got lumipars: " << lumipars << endmsg;
    m_rate = 1000 * std::accumulate( lumipars.begin(), lumipars.end(), double( 0 ) );
    info() << "updated assumed lumi rate to : " << m_rate << " Hz" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode HltReferenceRateSvc::initialize()
{
    StatusCode sc = Service::initialize();
    if ( !sc.isSuccess() ) return sc;
    sc = service( "ToolSvc", m_toolSvc );
    if ( !sc.isSuccess() ) return sc;
    sc = service( "EventDataSvc", m_evtSvc );
    if ( !sc.isSuccess() ) return sc;
    sc = m_decodeOdin.retrieve();
    if ( !sc.isSuccess() ) return sc;

    if ( m_useCondDB ) {
        sc = service( "UpdateManagerSvc", m_updMgrSvc );
        if ( !sc.isSuccess() ) return sc;
        m_updMgrSvc->registerCondition(
            this, "Conditions/Online/LHCb/Lumi/LumiSettings",
            &HltReferenceRateSvc::i_updateConditions, m_lumipars );
        sc = m_updMgrSvc->update( this );
        if ( !sc.isSuccess() ) return sc;
    }

    sc = service( "IncidentSvc", m_incidentSvc );
    if ( !sc.isSuccess() ) return sc;
    // insert at low priority, so that ODIN is available when we get invoked..
    long prio = std::numeric_limits<long>::min();
    m_incidentSvc->addListener( this, IncidentType::BeginEvent, prio );

    m_tick = 0;
    return sc;
}

void HltReferenceRateSvc::report() const
{
    if ( m_first < m_last ) {
        Gaudi::Time first( m_first * 1000 );
        Gaudi::Time last( m_last * 1000 );
        Gaudi::TimeSpan interval( last - first );
        info() << "Got " << m_tick - m_lastReportedTick << " ticks "
               << " in " << interval.seconds() << " s."
               << " during [ " << first.format( true, "%F %T" ) << ", "
               << last.format( true, "%F %T" ) << " ]" << endmsg;
        if ( interval.seconds() > 0 ) {
            info() << "This corresponds to an average observed rate of "
                   << ( m_tick - m_lastReportedTick ) / interval.seconds() << " Hz."
                   << ", compared to an assumed 'set' total rate of " << m_rate
                   << endmsg;
        }
        // reset counters...
        m_first = ~ulonglong( 0 );
        m_last = 0;
        m_lastReportedTick = m_tick;
    }
}

StatusCode HltReferenceRateSvc::finalize()
{
    report();
    StatusCode sc = Service::finalize();
    m_toolSvc->release();
    m_evtSvc->release();
    m_incidentSvc->release();
    if ( m_updMgrSvc != 0 ) m_updMgrSvc->release();
    return sc;
}

void HltReferenceRateSvc::handle( const Incident& /*incident*/ )
{
    // get the ODIN bank
    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc, m_location );
    if ( !odin ) m_decodeOdin->execute();
    if ( !odin ) {
        error() << " Could not obtain ODIN...  requesting AbortEvent" << endmsg;
        m_incidentSvc->fireIncident( Incident( name(), IncidentType::AbortEvent ) );
        return;
    }
    if ( odin->triggerType() == LHCb::ODIN::LumiTrigger ) ++m_tick;
    ulonglong gps = odin->gpsTime();
    if ( gps < m_first ) m_first = gps;
    if ( gps > m_last ) m_last = gps;
}
