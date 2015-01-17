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

using namespace LHCb;
using namespace Gaudi::Utils::Histos;

HltGlobalMonitor::histopair::histopair( GaudiHistoAlg& parent, std::string loc,
                                        const Gaudi::Histo1DDef& def,
                                        const char* yaxislabel )
    : m_parent{&parent}
    , m_histo{parent.book( def )}
    , m_profile{parent.bookProfile1D( std::string( "CPUvs" ) + def.title(),
                                      def.lowEdge(), def.highEdge(), def.bins() )}
    , m_loc{std::move( loc )}
{
    if ( yaxislabel ) {
        setAxisLabels( m_histo, std::string( "# of " ) + m_loc, "Entries" );
        setAxisLabels( m_profile, std::string( "# of " ) + m_loc, yaxislabel );
    }
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor )

// utilities

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator )
{
    declareProperty( "ODIN", m_ODINLocation = LHCb::ODINLocation::Default );
    declareProperty( "Hlt1DecReports", m_Hlt1DecReportsLocation = LHCb::HltDecReportsLocation::Default );
    declareProperty( "Hlt2DecReports", m_Hlt2DecReportsLocation = LHCb::HltDecReportsLocation::Default );
    declareProperty( "TimeSize", m_timeSize = 120 ); // number of minutes of history
                                                     // (half an hour)
    declareProperty( "TimeInterval", m_timeInterval = 1 ); // binwidth in minutes
    declareProperty( "DecToGroupHlt1", m_DecToGroup1 );
    declareProperty( "DecToGroupHlt2", m_DecToGroup2 );
    declareProperty( "Hlt1DecName", m_hlt1Decision = "Hlt1Global" );
    declareProperty( "Hlt2DecName", m_hlt2Decision = "Hlt2Global" );
    declareProperty( "RawEventLocation",
                     m_rawEventLocation = LHCb::RawEventLocation::Default );
    declareProperty(
        "CorrelateCPUWith",
        m_correlateCPU = {
            {"Hlt/Vertex/PV3D", Gaudi::Histo1DDef( "PV3D", -0.5, 10.5, 11 )},
            {"Hlt/Track/Velo", Gaudi::Histo1DDef( "Velo", -0.5, 599.5, 120 )},
            {"Hlt2/Track/Forward",
             Gaudi::Histo1DDef( "Forward", -0.5, 599.5, 120 )}} );

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
    m_odinHLT1 =
        book1D( "ODIN_HLT1", "ODIN trigger Type, after HLT1 ", -0.5, 7.5, 8 );
    m_odinHLT2 =
        book1D( "ODIN_HLT2", "ODIN trigger Type, after HLT2 ", -0.5, 7.5, 8 );

    std::vector<std::pair<unsigned, std::string>> odinLabels = {
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
    if ( m_odinHLT1 ) setBinLabels( m_odinHLT1, odinLabels );
    if ( m_odinHLT2 ) setBinLabels( m_odinHLT2, odinLabels );

    m_gpstimesec = 0;
    m_startClock = std::chrono::high_resolution_clock::now(); // System::currentTime( System::microSec );
    // create a histogram with one bin per Alley
    // the order and the names for the bins are
    // configured in HLTConf/Configuration.py

   
    if (!m_Hlt1DecReportsLocation.empty()  && !m_DecToGroup1.empty()) {
        std::vector<std::string> hlt1AlleyLabels;
        for ( const auto& i : m_DecToGroup1 ) hlt1AlleyLabels.push_back( i.first );
        m_hlt1Alley = book1D( "Hlt1 Alleys", "Hlt1 Alleys", -0.5,
                              hlt1AlleyLabels.size() - 0.5, hlt1AlleyLabels.size() );
        if ( !setBinLabels( m_hlt1Alley, hlt1AlleyLabels ) ) {
            error() << "failed to set binlables on Hlt1 Alley hist" << endmsg;
        }
        m_hlt1AlleysCorrelations =
            book2D( "Hlt1Alleys Correlations", -0.5, hlt1AlleyLabels.size() - 0.5,
                    hlt1AlleyLabels.size(), -0.5, hlt1AlleyLabels.size() - 0.5,
                    hlt1AlleyLabels.size() );
        if ( !setBinLabels( m_hlt1AlleysCorrelations, hlt1AlleyLabels,
                            hlt1AlleyLabels ) ) {
            error() << "failed to set binlables on Hlt1Alleys Correlation hist"
                    << endmsg;
        }

        /*One Histogram for each alley*/
        for ( const auto& i : m_DecToGroup1 ) {
            std::string alleyName = std::string( "Hlt1 " ) + i.first + " Lines";
            m_hlt1Alleys.push_back( book1D( alleyName, -0.5, i.second.size() - 0.5, i.second.size() ) );
            m_hlt1AlleyRates.push_back( &counter( alleyName ) );
            declareInfo( "COUNTER_TO_RATE[" + alleyName + "]", *m_hlt1AlleyRates.back(),
                         alleyName );
            std::vector<std::string> labels;
            for ( const std::string& s : i.second ) {
                std::string label = s;
                for ( const auto& strip : std::array<std::string,3>{
                         {  "Decision"  // and of course 'Decision'...
                         ,  "Hlt1"     // just strip 'Hlt1'
                         , i.first    } }    // finally try to remove alley prefix
                      ) {
                    if ( label != strip ) boost::algorithm::erase_all( label, strip );
                }
                labels.push_back( label );
                m_hlt1Line2AlleyBin[s] = {m_hlt1Alleys.size() - 1, labels.size() - 1};
            }
            if ( !setBinLabels( m_hlt1Alleys.back(), labels ) ) {
                error() << "failed to set binlables on Hlt1 " << i.first << " Alley hist"
                        << endmsg;
            }
        }

    }

    if (!m_Hlt2DecReportsLocation.empty()  && !m_DecToGroup2.empty()) {
        std::vector<std::string> hlt2AlleyLabels;
        for ( const auto& i : m_DecToGroup2 ) hlt2AlleyLabels.push_back( i.first );

        m_hlt2Alley = book1D( "Hlt2 Alleys", "Hlt2 Alleys", -0.5,
                              hlt2AlleyLabels.size() - 0.5, hlt2AlleyLabels.size() );
        if ( !setBinLabels( m_hlt2Alley, hlt2AlleyLabels ) ) {
            error() << "failed to set binlables on Hlt2 Alley hist" << endmsg;
        }
        m_hlt2AlleysCorrelations =
            book2D( "Hlt2Alleys Correlations", -0.5, hlt2AlleyLabels.size() - 0.5,
                    hlt2AlleyLabels.size(), -0.5, hlt2AlleyLabels.size() - 0.5,
                    hlt2AlleyLabels.size() );
        if ( !setBinLabels( m_hlt2AlleysCorrelations, hlt2AlleyLabels,
                            hlt2AlleyLabels ) ) {
            error() << "failed to set binlables on Hlt2Alleys Correlation hist"
                    << endmsg;
        }

        // for hlt2
        for ( const auto& i : m_DecToGroup2 ) {
            std::string alleyName = std::string( "Hlt2 " ) + i.first + " Lines";
            m_hlt2Alleys.push_back(
                book1D( alleyName, -0.5, i.second.size() - 0.5, i.second.size() ) );
            m_hlt2AlleyRates.push_back( &counter( alleyName ) );
            declareInfo( "COUNTER_TO_RATE[" + alleyName + "]", *m_hlt2AlleyRates.back(),
                         alleyName );
            std::vector<std::string> labels;
            for ( std::string s : i.second ) {
                std::string label = s;
                for ( const auto& strip : std::array<std::string,3>{
                         { "Decision" // always remove 'Decision'...
                         , "Hlt2"     // and Hlt2
                         , i.first }}    // finally try to remove alley prefix
                      ) {
                    if ( label != strip ) boost::algorithm::erase_all( label, strip );
                }
                labels.push_back( label );
                m_hlt2Line2AlleyBin[s] = {m_hlt2Alleys.size() - 1, labels.size() - 1};
            }
            if ( !setBinLabels( m_hlt2Alleys.back(), labels ) ) {
                error() << "failed to set binlables on Hlt2 " << i.first << " Alley hist"
                        << endmsg;
            }
        }
    }


    m_hltVirtMem = bookProfile1D( "Virtual memory", 0, m_timeSize,
                                   int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_hltVirtMem, "time since start of run [min]", "memory[MB]" );

    m_hltEventsTime = bookProfile1D( "average time per event", 0, m_timeSize,
                                     int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_hltEventsTime, "time since start of run [min]",
                   "average time/event [ms]" );

    m_tasks = book1D( "# of tasks", 0, m_timeSize,
                      int( m_timeSize / m_timeInterval + 0.5 ) );
    setAxisLabels( m_tasks, "time since start of run [min]", "# of tasks" );

    m_hltTime = book1D( "time per event ", -1, 4 );
    setAxisLabels( m_hltTime, "log10(time/event/ms)", "events" );

    declareInfo( "COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory" );
    declareInfo( "COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time" );

    m_hltTimeVsEvtSize =
        bookProfile1D( "Hlt CPU time vs raw event size", 0, 200000, 100 );
    setAxisLabels( m_hltTimeVsEvtSize, "raw event length",
                   "HLT processing time [ms]" );

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
    std::string veloCondition = "/dd/Conditions/Online/Velo/MotionSystem";
    registerCondition<HltGlobalMonitor>( veloCondition, m_veloCondition,
                                         &HltGlobalMonitor::updateCondition_velo );

    // Monitor magnet polarity
    std::string magnetCondition = "/dd/Conditions/Online/LHCb/Magnet/Set";
    registerCondition<HltGlobalMonitor>( magnetCondition, m_magnetCondition,
                                         &HltGlobalMonitor::updateCondition_magnet );

    // Monitor rich1
    std::string rich1Condition = "/dd/Conditions/Online/Rich1/R1HltGasParameters";
    registerCondition<HltGlobalMonitor>( rich1Condition, m_rich1Condition,
                                         &HltGlobalMonitor::updateCondition_rich1 );

    // Monitor rich2
    std::string rich2Condition = "/dd/Conditions/Online/Rich2/R2HltGasParameters";
    registerCondition<HltGlobalMonitor>( rich2Condition, m_rich2Condition,
                                         &HltGlobalMonitor::updateCondition_rich2 );

    // Monitor lhc
    std::string lhcfillingschemeCondition =
        "/dd/Conditions/Online/LHCb/LHCFillingScheme";
    registerCondition<HltGlobalMonitor>( lhcfillingschemeCondition,
                                         m_lhcfillingschemeCondition,
                                         &HltGlobalMonitor::updateCondition_lhcfillingscheme );

    // Monitor lumipars
    std::string lumiparsCondition = "/dd/Conditions/Online/LHCb/Lumi/LumiSettings";
    registerCondition<HltGlobalMonitor>( lumiparsCondition, m_lumiparsCondition,
                                         &HltGlobalMonitor::updateCondition_lumipars );
    /*
    // Monitor runinfo
    std::string runinfoCondition = "/dd/Conditions/Online/LHCb/Runinfo";
    registerCondition< HltGlobalMonitor >( runinfoCondition, m_runinfoCondition,
                                           &HltGlobalMonitor::updateCondition );

    // Monitor run parameters
    std::string runparsCondition = "/dd/Conditions/Online/LHCb/RunParameters";
    registerCondition< HltGlobalMonitor >( runparsCondition, m_runparsCondition,
                                           &HltGlobalMonitor::updateCondition );*/

    // updateCondition().ignore();

    // sc = runUpdate();
    // if ( sc.isFailure() ) {
    //   Error( "Could not update conditions from the CondDB" );
    //   return sc;
    //}

    // fill the histograms when the condition is updated;don't fill yet, we don't
    // have the runnumber and cant get to the online.xml
    // m_resolvxr50->fill(m_xRC);
    // m_resolvxr5->fill(m_xRC);
    // m_resolvxl50->fill(m_xLA);
    // m_resolvxl5->fill(m_xLA);
    // m_resolvy50->fill(m_Y);
    // m_resolvy5->fill(m_Y);
    // monitorResolverpositions();

    // Book the vertex monitoring histograms
    for ( const LocationMap::value_type& loc : m_vertexLocations ) {
        const std::string& name = loc.first;
        for ( double e : {1., 10.} ) {
            std::string s{  name +  "_Beamspot_" + std::to_string(e) };
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

    //TODO: what if running Hlt1+Hlt2, and we fail Hlt1 -- then there is no Hlt2 decrep...
    //      or only running Hlt1?
    LHCb::HltDecReports* hlt1 = fetch<LHCb::HltDecReports>( m_Hlt1DecReportsLocation );
    auto hlt1Global = hlt1 ? hlt1->decReport("Hlt1Global") : nullptr;
    LHCb::HltDecReports* hlt2 = fetch<LHCb::HltDecReports>( m_Hlt2DecReportsLocation, hlt1Global!=nullptr && hlt1Global->decision()!=0  );
    LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default );

    monitorODIN( odin, hlt1, hlt2 );
    monitorHLT( odin, hlt1, hlt2 );
    monitorVertices();
    monitorTrends();
    monitorResolverpositions();

    counter( "#events" )++;

    return StatusCode::SUCCESS;
}

//=============================================================================
void HltGlobalMonitor::handle( const Incident& incident )
{
    m_startEvent = std::chrono::high_resolution_clock::now(); // System::currentTime( System::microSec );
    if ( m_startClock.time_since_epoch().count()  == 0 || 
         incident.type() == IncidentType::BeginRun ||
         incident.type() == "RunChange" )
        m_startClock = m_startEvent;
    using seconds = std::chrono::duration<double>;
    m_currentTime = std::chrono::duration_cast<seconds>( std::chrono::high_resolution_clock::now() - m_startEvent ).count();
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
                                    const LHCb::HltDecReports* hlt1,
                                    const LHCb::HltDecReports* hlt2 )
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
    const auto* report = hlt1 ? hlt1->decReport( m_hlt1Decision ) : nullptr;
    if ( report && report->decision() ) fill( m_odinHLT1, odin->triggerType(), 1. );

    report = hlt2 ? hlt2->decReport( m_hlt2Decision ) : nullptr;
    if ( report && report->decision() ) fill( m_odinHLT2, odin->triggerType(), 1. );
}

//==============================================================================
void HltGlobalMonitor::monitorHLT( const LHCb::ODIN* /*odin*/,
                                   const LHCb::HltDecReports* hlt1 ,
                                   const LHCb::HltDecReports* hlt2 )
{
    // filling the histograms for the alleys instead of the lines
    if (hlt1)  {
        std::vector<unsigned> nAcc1Alley( m_hlt1Alleys.size(), 0u );
        for ( auto& i : *hlt1 ) {
            if ( !i.second.decision() ) continue;
            if ( i.first.compare( 0, 4, "Hlt1" ) != 0 ) continue;
            auto j = m_hlt1Line2AlleyBin.find( i.first );
            if ( j != m_hlt1Line2AlleyBin.end() ) {
                assert( j->second.first < nAcc1Alley.size() );
                ++nAcc1Alley[j->second.first];
                fill( m_hlt1Alleys[j->second.first], j->second.second, 1.0 );
            }
        }
        for ( unsigned i = 0; i < m_DecToGroup1.size(); ++i ) {
            *m_hlt1AlleyRates[i] += ( nAcc1Alley[i] > 0 );
            fill( m_hlt1Alley, i, ( nAcc1Alley[i] > 0 ) );
            if ( nAcc1Alley[i] == 0 ) continue;
            for ( unsigned j = 0; j < m_DecToGroup1.size(); ++j ) {
                fill( m_hlt1AlleysCorrelations, i, j, ( nAcc1Alley[j] > 0 ) );
            }
        }
    }
        
    if (hlt2) {
        std::vector<unsigned> nAcc2Alley( m_hlt2Alleys.size(), 0u );
        for ( auto& i : *hlt2 ) {
            if ( !i.second.decision() ) continue;
            if ( i.first.compare( 0, 4, "Hlt2" ) != 0 ) continue;
            auto j = m_hlt2Line2AlleyBin.find( i.first );
            if ( j != m_hlt2Line2AlleyBin.end() ) {
                assert( j->second.first < nAcc2Alley.size() );
                ++nAcc2Alley[j->second.first];
                fill( m_hlt2Alleys[j->second.first], j->second.second, 1.0 );
            }
        }
        for ( unsigned i = 0; i < m_DecToGroup2.size(); ++i ) {
            *m_hlt2AlleyRates[i] += ( nAcc2Alley[i] > 0 );
            fill( m_hlt2Alley, i, ( nAcc2Alley[i] > 0 ) );
            if ( nAcc2Alley[i] == 0 ) continue;
            for ( unsigned j = 0; j < m_DecToGroup2.size(); ++j ) {
                fill( m_hlt2AlleysCorrelations, i, j, ( nAcc2Alley[j] > 0 ) );
            }
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
    using ms = std::chrono::duration<float,std::milli>;
    auto elapsedTime = std::chrono::duration_cast<ms>( std::chrono::high_resolution_clock::now() - m_startEvent );
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
size_t HltGlobalMonitor::rawEvtLength( const std::vector<RawBank*>& banks )
{
    return std::accumulate(
        std::begin( banks ), std::end( banks ), size_t( 0 ),
        []( size_t x, const RawBank* bank ) { return x += bank->totalSize(); } );
}
