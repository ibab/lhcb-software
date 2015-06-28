// $Id: HltGlobalMonitor.cpp,v 1.76 2010-08-26 08:45:29 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include <boost/algorithm/string/erase.hpp>
// ============================================================================
// AIDA
// ============================================================================
#include <AIDA/IHistogram1D.h>
#include <AIDA/IHistogram2D.h>
#include <AIDA/IProfile1D.h>
#include <AIDA/IAxis.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/IIncidentSvc.h>
// ============================================================================
// GaudiUtils
// ============================================================================
#include <GaudiUtils/HistoStats.h>
#include <GaudiUtils/HistoLabels.h>
// ============================================================================
// Event
// ============================================================================
#include <Event/RawBank.h>
#include <Event/RawEvent.h>
#include <Event/HltDecReports.h>
#include <Event/RecVertex.h>
// ============================================================================
// Detector
// ============================================================================
#include <DetDesc/Condition.h>
// ============================================================================
// Memory Usage
// ============================================================================
#include <GaudiKernel/Timing.h>
#include <GaudiKernel/Memory.h>
// ============================================================================
// local
// ============================================================================
#include "HltGlobalMonitor.h"
// ============================================================================

namespace {
   using namespace LHCb;
   using namespace Gaudi::Utils::Histos;
   using namespace std::chrono;
   using std::string;
   using std::vector;
   using std::array;
   using std::pair;
   using std::to_string;
}

HltGlobalMonitor::histopair::histopair( GaudiHistoAlg& parent, string loc,
                                        const Gaudi::Histo1DDef& def,
                                        const char* yaxislabel )
    : m_parent{&parent}
    , m_histo{parent.book( def )}
    , m_profile{parent.bookProfile1D( string( "CPUvs" ) + def.title(),
                                      def.lowEdge(), def.highEdge(), def.bins() )}
    , m_loc{std::move( loc )}
{
    if ( yaxislabel ) {
        setAxisLabels( m_histo, string( "# of " ) + m_loc, "Entries" );
        setAxisLabels( m_profile, string( "# of " ) + m_loc, yaxislabel );
    }
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Gerhard Raven, Eric v. Herwijnen, Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor )

// utilities

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const string& name,
                                    ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator )
{

   declareProperty( "Stage", m_stage = "Hlt1" );
   declareProperty( "ODIN", m_ODINLocation = LHCb::ODINLocation::Default );
   declareProperty( "HltDecReports", m_hltDecReportsLocation = LHCb::HltDecReportsLocation::Default );
   declareProperty( "TimeSize", m_timeSize = 120 ); // number of minutes of history
                                                     // (half an hour)
   declareProperty( "TimeInterval", m_timeInterval = 1 ); // binwidth in minutes
   declareProperty( "DecToGroup", m_decToGroup );
   declareProperty( "HltDecName", m_hltDecision = "Hlt1Global" );
   declareProperty( "RawEventLocation",
                    m_rawEventLocation = LHCb::RawEventLocation::Default );
   declareProperty("CorrelateCPUWith",
                   m_correlateCPU = {
                      {"Hlt/Vertex/PV3D", Gaudi::Histo1DDef( "PV3D", -0.5, 10.5, 11 )},
                      {"Hlt/Track/Velo", Gaudi::Histo1DDef( "Velo", -0.5, 599.5, 120 )},
                      {"Hlt2/Track/Forward", Gaudi::Histo1DDef( "Forward", -0.5, 599.5, 120 )}} );

   declareProperty( "VertexLocations", m_vertexLocations );
}
//=============================================================================
// Destructor
//=============================================================================
HltGlobalMonitor::~HltGlobalMonitor()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalMonitor::initialize()
{
    StatusCode sc = HltBaseAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if (m_stage != "Hlt1" && m_stage != "Hlt2") {
       error() << "Stage must be either Hlt1 or Hlt2" << endmsg;
       return StatusCode::FAILURE;
    }

    m_beamSpotX = 0.;
    m_beamSpotY = 0.;
    m_xRC = 0.;
    m_xLA = 0.;
    m_Y = 0.;

    m_magnetCurrent = 0.;
    m_magnetPolarity = 0.;
    m_magnetState = 0.;

    m_rich1Temperature = 0.;
    m_rich1Presssure = 0.;
    m_rich2Temperature = 0.;
    m_rich2Presssure = 0.;

    m_lhcNCollidingBunches = 0;

    m_odin = book1D( "ODIN trigger type", "ODIN trigger Type ", -0.5, 7.5, 8 );
    m_odinHLT = book1D( string{"ODIN_"} + m_stage, string{"ODIN trigger Type, after "} + m_stage, -0.5, 7.5, 8 );

    vector<pair<unsigned, string>> odinLabels = {
        {ODIN::PhysicsTrigger, "Physics"},
        {ODIN::BeamGasTrigger, "BeamGas"},
        {ODIN::LumiTrigger, "Lumi"},
        {ODIN::TechnicalTrigger, "Technical"},
        {ODIN::AuxiliaryTrigger, "Auxiliary"},
        {ODIN::NonZSupTrigger, "NonZSup"},
        {ODIN::TimingTrigger, "Timing"},
        {ODIN::CalibrationTrigger, "Calibration"}};
    if ( !setBinLabels( m_odin, odinLabels ) ) {
        error() << "failed to set binlables on ODIN hist" << endmsg;
    }
    if ( m_odinHLT ) setBinLabels( m_odinHLT, odinLabels );

    m_gpstimesec = 0;
    m_startClock = high_resolution_clock::now(); // System::currentTime( System::microSec );
    // create a histogram with one bin per Alley
    // the order and the names for the bins are
    // configured in HLTConf/Configuration.py


    if (!m_hltDecReportsLocation.empty() && !m_decToGroup.empty()) {
        vector<string> hltAlleyLabels;
        for ( const auto& i : m_decToGroup ) hltAlleyLabels.push_back( i.first );
        m_hltAlley = book1D( m_stage + " Alleys", m_stage + " Alleys", -0.5,
                              hltAlleyLabels.size() - 0.5, hltAlleyLabels.size() );
        if ( !setBinLabels( m_hltAlley, hltAlleyLabels ) ) {
           error() << "failed to set binlables on " << m_stage << " Alley hist" << endmsg;
        }
        m_hltCorrelations =
            book2D( m_stage + " Correlations", -0.5, hltAlleyLabels.size() - 0.5,
                    hltAlleyLabels.size(), -0.5, hltAlleyLabels.size() - 0.5,
                    hltAlleyLabels.size() );
        if ( !setBinLabels( m_hltCorrelations, hltAlleyLabels, hltAlleyLabels ) ) {
           error() << "failed to set binlables on " << m_stage << " Alleys Correlation hist"
                    << endmsg;
        }

        /*One Histogram for each alley*/
        for ( const auto& i : m_decToGroup ) {
            string alleyName = m_stage + i.first + " Lines";
            m_hltAlleys.push_back( book1D( alleyName, -0.5, i.second.size() - 0.5, i.second.size() ) );
            m_hltAlleyRates.push_back( &counter( alleyName ) );
            declareInfo( "COUNTER_TO_RATE[" + alleyName + "]", *m_hltAlleyRates.back(),
                         alleyName );
            vector<string> labels;
            for ( const string& s : i.second ) {
                string label = s;
                for ( const auto& strip : array<string,3>{
                         {  "Decision"  // and of course 'Decision'...
                         ,  m_stage     // just strip 'Hlt{1,2}'
                         , i.first    } }    // finally try to remove alley prefix
                      ) {
                    if ( label != strip ) boost::algorithm::erase_all( label, strip );
                }
                labels.push_back( label );
                m_hltLine2AlleyBin[s] = {m_hltAlleys.size() - 1, labels.size() - 1};
            }
            if ( !setBinLabels( m_hltAlleys.back(), labels ) ) {
               error() << "failed to set binlables on " << m_stage << " " << i.first
                       << " Alley hist" << endmsg;
            }
        }

    }

    m_hltVirtMem = bookProfile1D( m_stage + " Virtual memory", 0, m_timeSize,
                                   int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_hltVirtMem, "time since start of run [min]", "memory[MB]" );

    m_hltEventsTime = bookProfile1D( m_stage + " average time per event", 0, m_timeSize,
                                     int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_hltEventsTime, "time since start of run [min]",
                   "average time/event [ms]" );

    m_tasks = book1D( m_stage + " # of tasks", 0, m_timeSize,
                      int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_tasks, "time since start of run [min]", "# of tasks" );

    m_hltTime = book1D( m_stage + " time per event ", -1, 4 );
    setAxisLabels( m_hltTime, "log10(time/event/ms)", "events" );

    declareInfo( "COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory" );
    declareInfo( "COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time" );

    m_hltTimeVsEvtSize =
        bookProfile1D( m_stage + " Hlt CPU time vs raw event size", 0, 200000, 100 );
    setAxisLabels( m_hltTimeVsEvtSize, "raw event length",
                   m_stage + " processing time [ms]" );

    for ( const auto& j : m_correlateCPU ) {
        m_CPUcorrelations.emplace_back( *this, j.first, j.second,
                                        "Average CPU time [ms]" );
    }

    declareInfo( "COUNTER_TO_RATE[GpsTimeoflast]", m_gpstimesec,
                 "GPS time of last event" );

    // register for incidents...
    IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
    // insert at high priority, so we also time what happens during BeginEvent
    // incidents...
    long priority = std::numeric_limits<long>::max();
    incidentSvc->addListener( this, IncidentType::BeginEvent, priority );
    incidentSvc->addListener( this, IncidentType::BeginRun, 0 );
    incidentSvc->addListener( this, "RunChange", 0 );

    // start by kicking ourselves in action -- just in case we don't get one
    // otherwise...
    this->handle( Incident( name(), IncidentType::BeginRun ) );

    // book the resolver position histograms
    // m_resolvxr50       = book1D("Velo Resolver XR-50", "Velo Resolver XR-50",
    // -50.,50., 100 );
    // m_resolvxr5        = book1D("Velo Resolver XR-5", "Velo Resolver XR-5",
    // -1.,1., 100 );
    // m_resolvxl50       = book1D("Velo Resolver XL-50", "Velo Resolver XL-50",
    // -50.,50., 100 );
    // m_resolvxl5        = book1D("Velo Resolver XL-5", "Velo Resolver XL-5",
    // -1.,1., 100 );
    // m_xRC=0.;
    // m_xLA=0.;
    // m_Y=0.;
    m_resolvxle =
        bookProfile1D( "Velo Resolver XL Entries", "Velo Resolver XL Entries", 0,
                       m_timeSize, int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_resolvxle, "time since start of run [min]",
                   "average Velo resolver XL [cm]" );
    m_resolvxre =
        bookProfile1D( "Velo Resolver XR Entries", "Velo Resolver XR Entries", 0,
                       m_timeSize, int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_resolvxre, "time since start of run [min]",
                   "average Velo resolver XR [cm]" );

    // m_resolvy50        = book1D("Velo Resolver Y-50", "Velo Resolver Y-50",
    // -50.,50., 100 );
    // m_resolvy5         = book1D("Velo Resolver Y-5", "Velo Resolver Y-5", -1.,1.,
    // 100 );
    m_resolvye =
        bookProfile1D( "Velo Resolver Y Entries", "Velo Resolver Y Entries", 0,
                       m_timeSize, int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_resolvye, "time since start of run [min]",
                   "average Velo resolver Y [cm]" );

    m_magnetcurrent =
        bookProfile1D( "Magnet Current", "Magnet Current", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_magnetcurrent, "time since start of run [min]",
                   "Average Magnet current [A]" );
    m_magnetpolarity =
        bookProfile1D( "Magnet Polarity", "Magnet Polarity", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_magnetpolarity, "time since start of run [min]",
                   "Average Magnet polarity" );
    m_magnetstate = bookProfile1D( "Magnet State", "Magnet State", 0, m_timeSize,
                                   int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_magnetstate, "time since start of run [min]",
                   "Average Magnet state " );

    m_rich1pressure =
        bookProfile1D( "Rich1 Pressure", "Rich1 Pressure", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_rich1pressure, "time since start of run [min]",
                   "average Rich1 Pressure" );
    m_rich1temperature =
        bookProfile1D( "Rich1 Temperature", "Rich1 Temperature", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_rich1temperature, "time since start of run [min]",
                   "Rich1 Temperature [K]" );

    m_rich2pressure =
        bookProfile1D( "Rich2 Pressure", "Rich2 Pressure", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_rich2pressure, "time since start of run [min]",
                   "average Rich2 Pressure " );
    m_rich2temperature =
        bookProfile1D( "Rich2 Temperature", "Rich2 Temperature", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_rich2temperature, "time since start of run [min]",
                   "average Rich2 Temperature [K]" );

    m_lhcnbofbunches =
        bookProfile1D( "Nb of bunches", "Nb of bunches", 0, m_timeSize,
                       int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_lhcnbofbunches, "time since start of run [min]",
                   "average Nb of bunches" );

    m_lumipars = bookProfile1D( "Lumi parameters", "Lumi parameters", -0.5, 3.5, 4 );
    setBinLabels( m_lumipars, {{0, "b-b"}, {1, "b-e"}, {2, "e-b"}, {3, "e-e"}} );

    // Monitor vertex positions
    string veloCondition = "/dd/Conditions/Online/Velo/MotionSystem";
    registerCondition<HltGlobalMonitor>( veloCondition, m_veloCondition,
                                         &HltGlobalMonitor::updateCondition_velo );

    // Monitor magnet polarity
    string magnetCondition = "/dd/Conditions/Online/LHCb/Magnet/Set";
    registerCondition<HltGlobalMonitor>( magnetCondition, m_magnetCondition,
                                         &HltGlobalMonitor::updateCondition_magnet );

    // Monitor rich1
    string rich1Condition = "/dd/Conditions/Online/Rich1/R1HltGasParameters";
    registerCondition<HltGlobalMonitor>( rich1Condition, m_rich1Condition,
                                         &HltGlobalMonitor::updateCondition_rich1 );

    // Monitor rich2
    string rich2Condition = "/dd/Conditions/Online/Rich2/R2HltGasParameters";
    registerCondition<HltGlobalMonitor>( rich2Condition, m_rich2Condition,
                                         &HltGlobalMonitor::updateCondition_rich2 );

    // Monitor lhc
    string lhcfillingschemeCondition =
        "/dd/Conditions/Online/LHCb/LHCFillingScheme";
    registerCondition<HltGlobalMonitor>( lhcfillingschemeCondition,
                                         m_lhcfillingschemeCondition,
                                         &HltGlobalMonitor::updateCondition_lhcfillingscheme );

    // Monitor lumipars
    string lumiparsCondition = "/dd/Conditions/Online/LHCb/Lumi/LumiSettings";
    registerCondition<HltGlobalMonitor>( lumiparsCondition, m_lumiparsCondition,
                                         &HltGlobalMonitor::updateCondition_lumipars );

    // Book the vertex monitoring histograms
    for ( const LocationMap::value_type& loc : m_vertexLocations ) {
        const string& name = loc.first;
        for ( double e : {1., 10.} ) {
            string s{  name + "_" + m_stage + "_Beamspot_" + to_string(e) };
            auto histo = book2D( s.c_str(), -e, e, 100, -e, e, 100 );
            auto it = m_vertexHistos.find( name );
            if ( it == m_vertexHistos.end() ) {
                m_vertexHistos[name] = HistoVector( 1, histo );
            } else {
                it->second.push_back( histo );
            }
        }
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute()
{
    auto decReports = fetch<LHCb::HltDecReports>( m_hltDecReportsLocation );
    LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default );

    monitorODIN( odin, decReports );
    monitorHLT( odin, decReports );
    monitorVertices();
    monitorTrends();
    monitorResolverpositions();

    counter( "#events" )++;

    return StatusCode::SUCCESS;
}

//=============================================================================
void HltGlobalMonitor::handle( const Incident& incident )
{
    m_startEvent = high_resolution_clock::now(); // System::currentTime( System::microSec );
    if ( m_startClock.time_since_epoch().count()  == 0 ||
         incident.type() == IncidentType::BeginRun ||
         incident.type() == "RunChange" )
        m_startClock = m_startEvent;
    using seconds = duration<double>;
    m_currentTime = duration_cast<seconds>( high_resolution_clock::now() - m_startClock ).count();
}

//==============================================================================
StatusCode HltGlobalMonitor::updateCondition_velo()
{
    m_xRC = m_veloCondition->paramAsDouble( "ResolPosRC" );
    m_xLA = m_veloCondition->paramAsDouble( "ResolPosLA" );
    m_Y   = m_veloCondition->paramAsDouble( "ResolPosY" );

    m_beamSpotX = ( m_xRC + m_xLA ) / 2;
    m_beamSpotY = m_Y;
    return StatusCode::SUCCESS;
}

StatusCode HltGlobalMonitor::updateCondition_magnet()
{
    m_magnetCurrent =  m_magnetCondition->paramAsDouble( "Current" );
    m_magnetPolarity =  m_magnetCondition->paramAsInt( "Polarity" );
    auto magnetState = m_magnetCondition->paramAsString( "State" );
    m_magnetState = 0.;
    if ( magnetState.compare( "OFF" ) ) m_magnetState = 0.;
    if ( magnetState.compare( "UP" ) ) m_magnetState = 1.;
    if ( magnetState.compare( "DOWN" ) ) m_magnetState = -1.;
    return StatusCode::SUCCESS;
}

StatusCode HltGlobalMonitor::updateCondition_rich1()
{
    m_rich1Temperature =  m_rich1Condition->paramAsDouble( "Temperature" );
    m_rich1Presssure =  m_rich1Condition->paramAsDouble( "Pressure" );
    return StatusCode::SUCCESS;
}
StatusCode HltGlobalMonitor::updateCondition_rich2()
{
    m_rich2Temperature =  m_rich2Condition->paramAsDouble( "Temperature" );
    m_rich2Presssure =  m_rich2Condition->paramAsDouble( "Pressure" );
    return StatusCode::SUCCESS;
}
StatusCode HltGlobalMonitor::updateCondition_lhcfillingscheme()
{
    m_lhcNCollidingBunches = m_lhcfillingschemeCondition->paramAsInt( "NCollidingBunches" );
    return StatusCode::SUCCESS;
}
StatusCode HltGlobalMonitor::updateCondition_lumipars()
{
    m_lumiparvalues = m_lumiparsCondition->paramAsDoubleVect( "LumiPars" );
    return StatusCode::SUCCESS;
}

//==============================================================================
void HltGlobalMonitor::monitorODIN( const LHCb::ODIN* odin,
                                    const LHCb::HltDecReports* decReports )
{
    if ( !odin ) return;
    auto gpstime = odin->gpsTime();
    if ( msgLevel( MSG::DEBUG ) ) debug() << "gps time" << gpstime << endmsg;
    m_gpstimesec =
        int( gpstime / 1000000 - 904262401 ); //@TODO: is this still OK with ODIN v6?
    counter( "ODIN::Lumi" ) += ( odin->triggerType() == ODIN::LumiTrigger );
    counter( "ODIN::NotLumi" ) += ( odin->triggerType() != ODIN::LumiTrigger );
    fill( m_odin, odin->triggerType(), 1. );

    // now check HLT decisions for rejected events after Hlt
    const auto* report = decReports ? decReports->decReport( m_hltDecision ) : nullptr;
    if ( report && report->decision() ) fill( m_odinHLT, odin->triggerType(), 1. );
}

//==============================================================================
void HltGlobalMonitor::monitorHLT( const LHCb::ODIN* /*odin*/,
                                   const LHCb::HltDecReports* decReports  )
{
   // filling the histograms for the alleys instead of the lines
   vector<unsigned> nAccAlley( m_hltAlleys.size(), 0u );
   for ( auto& i : *decReports ) {
      if ( !i.second.decision() ) continue;
      if ( i.first.compare( 0, 4, m_stage ) != 0 ) continue;
      auto j = m_hltLine2AlleyBin.find( i.first );
      if ( j != m_hltLine2AlleyBin.end() ) {
         assert( j->second.first < nAccAlley.size() );
         ++nAccAlley[j->second.first];
         fill( m_hltAlleys[j->second.first], j->second.second, 1.0 );
      }
   }
   for ( unsigned i = 0; i < m_decToGroup.size(); ++i ) {
      *m_hltAlleyRates[i] += ( nAccAlley[i] > 0 );
      fill( m_hltAlley, i, ( nAccAlley[i] > 0 ) );
      if ( nAccAlley[i] == 0 ) continue;
      for ( unsigned j = 0; j < m_decToGroup.size(); ++j ) {
         fill( m_hltCorrelations, i, j, ( nAccAlley[j] > 0 ) );
      }
   }
}

//==============================================================================
void HltGlobalMonitor::monitorVertices()
{
    for ( const auto& entry : m_vertexLocations ) {
        auto* vertices = getIfExists<LHCb::RecVertex::Container>( entry.second );
        if ( !vertices ) continue;
        for ( auto&& histo : m_vertexHistos[entry.first] ) {
            for ( auto&& vx : *vertices ) {
                histo->fill( vx->position().x() - m_beamSpotX,
                             vx->position().y() - m_beamSpotY );
            }
        }
    }
}

//==============================================================================
void HltGlobalMonitor::monitorResolverpositions()
{
    double when  = m_currentTime/60;

    m_resolvxle->fill( when, m_xLA );
    m_resolvxre->fill( when, m_xRC );
    m_resolvye->fill( when, m_Y );
    m_magnetpolarity->fill( when, m_magnetPolarity );
    m_magnetcurrent->fill( when, m_magnetCurrent );
    m_magnetstate->fill( when, m_magnetState );

    m_rich1temperature->fill( when, m_rich1Temperature );
    m_rich1pressure->fill( when, m_rich1Presssure );
    m_rich2temperature->fill( when, m_rich2Temperature );
    m_rich2pressure->fill( when, m_rich2Presssure );

    m_lhcnbofbunches->fill( when, m_lhcNCollidingBunches );

    m_lumipars->reset();
    if (int(m_lumiparvalues.size())!=m_lumipars->axis().bins()) {
        error() << "have " << m_lumiparvalues.size() << " lumipars, expecting " << m_lumipars->axis().bins() << endmsg;
    } else {
        for ( int i = 0; i < m_lumipars->axis().bins(); i++ ) {
            m_lumipars->fill( i, m_lumiparvalues[i] );
        }
    }
}

//==============================================================================

void HltGlobalMonitor::monitorTrends()
{
    using ms = duration<float,std::milli>;
    auto elapsedTime = duration_cast<ms>( high_resolution_clock::now() - m_startEvent );
    fill( m_hltTime, std::log10( elapsedTime.count() ) , 1.0 ); // convert to log(time/ms)

    auto t = elapsedTime.count();
    auto  when  = m_currentTime/60;;

    m_hltEventsTime->fill( when, t );

    // monitor CPU time vs evt size
    auto* evt = getIfExists<LHCb::RawEvent>( m_rawEventLocation );
    size_t evtSize = evt ? rawEvtLength( evt ) : 0;
    m_hltTimeVsEvtSize->fill( evtSize, t );

    // monitor CPU time vs # of PVs, # of RZVelo tracks, ...
    for ( histopair& i : m_CPUcorrelations ) i( t );

    auto i = m_hltVirtMem->axis().coordToIndex( when );
    if ( m_hltVirtMem->binEntries( i ) == 0 ) {
        m_virtmem = virtualMemory( System::MByte, System::Memory );
        m_hltVirtMem->fill( when, double( m_virtmem ) );
    }
    // histogram the # of tasks vs. time...
    i = m_tasks->axis().coordToIndex( when );
    if ( m_tasks->binEntries( i ) == 0 ) m_tasks->fill( when, 1 );
}

//==============================================================================
size_t HltGlobalMonitor::rawEvtLength( const LHCb::RawEvent* evt )
{
    size_t len = 0;
    RawEvent* raw = const_cast<RawEvent*>( evt );
    for ( int i = 0; i < RawBank::LastType; ++i ) {
        len += rawEvtLength( raw->banks( RawBank::BankType( i ) ) );
    }
    return len;
}
//==============================================================================
/// Determine length of the sequential buffer from RawEvent object
size_t HltGlobalMonitor::rawEvtLength( const vector<RawBank*>& banks )
{
    return std::accumulate(
        begin( banks ), end( banks ), size_t( 0 ),
        []( size_t x, const RawBank* bank ) { return x += bank->totalSize(); } );
}
