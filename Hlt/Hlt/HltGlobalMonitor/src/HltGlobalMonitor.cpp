
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
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
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


HltGlobalMonitor::histopair::histopair( GaudiHistoAlg& parent,
                                        const std::string& loc,
                                        const Gaudi::Histo1DDef& def,
                                        const char* yaxislabel ) 
   : m_parent( &parent ), m_histo( parent.book( def ) )
   , m_profile( parent.bookProfile1D( std::string("CPUvs") + def.title(),
                                      def.lowEdge(),def.highEdge(),def.bins() ) )
   , m_loc( loc )
{ 
   if ( yaxislabel ) {
      setAxisLabels(m_histo,  std::string("# of ") + m_loc,"Entries"); 
      setAxisLabels(m_profile,std::string("# of ") + m_loc,yaxislabel); 
   }
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
   : HltBaseAlg ( name , pSvcLocator )
   , m_currentTime(0)
   , m_timeSize(0)
   , m_timeInterval(0)
   , m_startClock(0)
   , m_startEvent(0)
   , m_virtmem(0)
   , m_gpstimesec(0)
   , m_time_ref(0)
   , m_totaltime(0)
{
   declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
   declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
   declareProperty("TimeSize",          m_timeSize = 120 );   // number of minutes of history (half an hour)
   declareProperty("TimeInterval",      m_timeInterval = 1 ); // binwidth in minutes 
   declareProperty("DecToGroupHlt1",    m_DecToGroup1);
   declareProperty("DecToGroupHlt2",    m_DecToGroup2);
   declareProperty("Hlt1DecName", m_hlt1Decision = "Hlt1Global" );
   declareProperty("Hlt2DecName", m_hlt2Decision = "Hlt2Global" );
   declareProperty( "RawEventLocation"   , m_rawEventLocation = LHCb::RawEventLocation::Default );

   declareProperty("CorrelateCPUWith", m_correlateCPU = boost::assign::list_of<std::pair<std::string,Gaudi::Histo1DDef> >
                   ("Hlt/Vertex/PV3D",Gaudi::Histo1DDef("PV3D", -0.5,10.5,11    ) )
                   ("Hlt/Track/Velo",Gaudi::Histo1DDef("Velo", -0.5,599.5,120  ) )
                   ("Hlt2/Track/Forward",Gaudi::Histo1DDef("Forward", -0.5,599.5,120  ) ) );

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
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   m_odin = book1D("ODIN trigger type",  "ODIN trigger Type ",-0.5, 7.5, 8);
   m_odinHLT1 = book1D("ODIN_HLT1",  "ODIN trigger Type, after HLT1 ",-0.5, 7.5, 8);
   m_odinHLT2 = book1D("ODIN_HLT2",  "ODIN trigger Type, after HLT2 ",-0.5, 7.5, 8);

   std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
      (ODIN::PhysicsTrigger,    "Physics")
      (ODIN::BeamGasTrigger,    "BeamGas")
      (ODIN::LumiTrigger,       "Lumi")
      (ODIN::TechnicalTrigger,  "Technical")
      (ODIN::AuxiliaryTrigger,  "Auxiliary")
      (ODIN::NonZSupTrigger,    "NonZSup")
      (ODIN::TimingTrigger,     "Timing")
      (ODIN::CalibrationTrigger,"Calibration");
   if (!setBinLabels( m_odin, odinLabels )) {
      error() << "failed to set binlables on ODIN hist" << endmsg;
   }
   if(m_odinHLT1) setBinLabels( m_odinHLT1, odinLabels );
   if(m_odinHLT2) setBinLabels( m_odinHLT2, odinLabels );

   m_gpstimesec=0;
   m_startClock = System::currentTime( System::microSec );
   // create a histogram with one bin per Alley
   // the order and the names for the bins are
   // configured in HLTConf/Configuration.py  

   std::vector<std::string> hlt1AlleyLabels;
   for (DecToGroupType::const_iterator i = m_DecToGroup1.begin(); i!=m_DecToGroup1.end(); ++i ) {
      hlt1AlleyLabels.push_back(i->first);
   }

   m_hlt1Alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, hlt1AlleyLabels.size()-0.5 , hlt1AlleyLabels.size() );
   if (!setBinLabels( m_hlt1Alley, hlt1AlleyLabels )) {
      error() << "failed to set binlables on Hlt1 Alley hist" << endmsg;
   }

   std::vector<std::string> hlt2AlleyLabels;
   for (DecToGroupType::const_iterator i = m_DecToGroup2.begin(); i!=m_DecToGroup2.end(); ++i ) {
      hlt2AlleyLabels.push_back(i->first);
   }
   m_hlt2Alley       = book1D("Hlt2 Alleys", "Hlt2 Alleys", -0.5, hlt2AlleyLabels.size()-0.5 , hlt2AlleyLabels.size() );
   if (!setBinLabels( m_hlt2Alley, hlt2AlleyLabels )) {
      error() << "failed to set binlables on Hlt2 Alley hist" << endmsg;
   } 

   m_hlt1AlleysCorrelations      = book2D("Hlt1Alleys Correlations", -0.5, hlt1AlleyLabels.size()-0.5, hlt1AlleyLabels.size()
                                          , -0.5, hlt1AlleyLabels.size()-0.5, hlt1AlleyLabels.size() );
   if (!setBinLabels( m_hlt1AlleysCorrelations, hlt1AlleyLabels, hlt1AlleyLabels )) {
      error() << "failed to set binlables on Hlt1Alleys Correlation hist" << endmsg;
   }

   m_hlt2AlleysCorrelations      = book2D("Hlt2Alleys Correlations", -0.5, hlt2AlleyLabels.size()-0.5, hlt2AlleyLabels.size()
                                          , -0.5, hlt2AlleyLabels.size()-0.5, hlt2AlleyLabels.size() );
   if (!setBinLabels( m_hlt2AlleysCorrelations, hlt2AlleyLabels, hlt2AlleyLabels )) {
      error() << "failed to set binlables on Hlt2Alleys Correlation hist" << endmsg;
   }

   /*One Histogram for each alley*/
   for(DecToGroupType::const_iterator i=m_DecToGroup1.begin();i!=m_DecToGroup1.end();++i){
      std::string alleyName = std::string("Hlt1 ")+i->first+" Lines";
      m_hlt1Alleys.push_back( book1D(alleyName,   -0.5,i->second.size()-0.5,i->second.size()) );
      m_hlt1AlleyRates.push_back(&counter(alleyName));
      declareInfo("COUNTER_TO_RATE["+alleyName+"]", *m_hlt1AlleyRates.back(),alleyName);
      std::vector<std::string> strip; 
      strip.push_back("Decision"); // and of course 'Decision'...
      strip.push_back("Hlt1");   // in case the above doesn't work, just strip 'Hlt1' 
      strip.push_back(i->first); // first try to remove alley prefix
      std::vector<std::string> labels;
      for (std::vector<std::string>::const_iterator j=i->second.begin();j!=i->second.end();++j) {
         std::string s = *j;
         for (std::vector<std::string>::const_iterator k = strip.begin();k!=strip.end();++k) {
            if (s!=*k) boost::algorithm::erase_all(s,*k);
         }
         labels.push_back(s);
         m_hlt1Line2AlleyBin[ *j ] =  std::make_pair( m_hlt1Alleys.size()-1, labels.size()-1 );
      }
      if (!setBinLabels( m_hlt1Alleys.back(),labels )) {
         error() << "failed to set binlables on Hlt1 " << i->first << " Alley hist" << endmsg;
      }
   }

   // for hlt2
   for(DecToGroupType::const_iterator i=m_DecToGroup2.begin();i!=m_DecToGroup2.end();++i){
      std::string alleyName = std::string("Hlt2 ")+i->first+" Lines";
      m_hlt2Alleys.push_back( book1D(alleyName, -0.5,i->second.size()-0.5,i->second.size()) );
      m_hlt2AlleyRates.push_back(&counter(alleyName));
      declareInfo("COUNTER_TO_RATE["+alleyName+"]", *m_hlt2AlleyRates.back(),alleyName);
      std::vector<std::string> strip; 
      strip.push_back("Decision"); // always remove 'Decision'...
      strip.push_back("Hlt2");   // and Hlt2
      strip.push_back(i->first); // finally try to remove alley prefix
      std::vector<std::string> labels;
      for (std::vector<std::string>::const_iterator j=i->second.begin();j!=i->second.end();++j) {
         std::string s = *j;
         for (std::vector<std::string>::const_iterator k = strip.begin();k!=strip.end();++k) {
            if (s!=*k) boost::algorithm::erase_all(s,*k);
         }
         labels.push_back(s);
         m_hlt2Line2AlleyBin[ *j ] =  std::make_pair( m_hlt2Alleys.size()-1, labels.size()-1 );
      }
      if (!setBinLabels( m_hlt2Alleys.back(),labels )) {
         error() << "failed to set binlables on Hlt2 " << i->first << " Alley hist" << endmsg;
      }
   }


   m_hltVirtTime  = bookProfile1D("Virtual memory",   0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
   setAxisLabels( m_hltVirtTime, "time since start of run [min]", "memory[MB]");
  
   m_hltEventsTime  = bookProfile1D( "average time per event", 0,
                                     m_timeSize, int( m_timeSize / m_timeInterval + 0.5 ) );
   setAxisLabels( m_hltEventsTime, "time since start of run [min]", "average time/event [ms]");

   m_tasks  = book1D("# of tasks", 0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
   setAxisLabels( m_tasks, "time since start of run [min]", "# of tasks");

   m_hltTime  = book1D("time per event ", -1, 4 );
   setAxisLabels( m_hltTime, "log10(time/event/ms)", "events");

   declareInfo("COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory");
   declareInfo("COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time");

   m_hltTimeVsEvtSize = bookProfile1D("Hlt CPU time vs raw event size", 0,200000,100);
   setAxisLabels( m_hltTimeVsEvtSize, "raw event length", "HLT processing time [ms]");
  
   for (std::map<std::string,Gaudi::Histo1DDef>::const_iterator j = m_correlateCPU.begin();
        j != m_correlateCPU.end(); ++j ) {
      m_CPUcorrelations.push_back( histopair( *this, j->first, j->second,
                                              "Average CPU time [ms]") );
   }

   declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"GPS time of last event");

   // register for incidents...
   IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
   // insert at high priority, so we also time what happens during BeginEvent incidents...
   long priority = std::numeric_limits<long>::max();
   incidentSvc->addListener( this, IncidentType::BeginEvent, priority ) ;
   incidentSvc->addListener( this, IncidentType::BeginRun, 0 ) ;
   incidentSvc->addListener( this, "RunChange", 0 ) ;

   // start by kicking ourselves in action -- just in case we don't get one otherwise...
   this->handle(Incident(name(), IncidentType::BeginRun ));

   // Monitor vertex positions
   std::string condition = "/dd/Conditions/Online/Velo/MotionSystem";
   registerCondition< HltGlobalMonitor >( condition, m_veloCondition,
                                          &HltGlobalMonitor::updateCondition );

   sc = runUpdate();
   if ( sc.isFailure() ) {
      Error( "Could not update conditions from the CondDB" );
      return sc;
   }

   const double xRC = m_veloCondition->paramAsDouble ( "ResolPosRC" );
   const double xLA = m_veloCondition->paramAsDouble ( "ResolPosLA" );
   const double   Y = m_veloCondition->paramAsDouble ( "ResolPosY"  );

   m_beamSpotX = ( xRC + xLA ) / 2;
   m_beamSpotY = Y;

   // Book the vertex monitoring histograms
   std::vector< double > edges = boost::assign::list_of( 1 )( 10 );
   AIDA::IHistogram2D* histo = 0;
   BOOST_FOREACH( const LocationMap::value_type& loc, m_vertexLocations ) {
      const std::string& name = loc.first;
      BOOST_FOREACH( double e, edges ) {
         std::stringstream s;
         s << name << "_Beamspot_" << e;
         histo = book2D( s.str().c_str(), -e, e, e * 100, -e, e, e * 100 );
         HistoMap::iterator it = m_vertexHistos.find( name );
         if ( it == m_vertexHistos.end() ) {
            m_vertexHistos[ name ] = HistoVector( 1, histo );
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
StatusCode HltGlobalMonitor::execute() {  

   LHCb::HltDecReports* hlt = fetch<LHCb::HltDecReports>( m_HltDecReportsLocation );
   LHCb::ODIN*         odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
 
   monitorODIN( odin, hlt );
   monitorHLT(  odin, hlt );
   monitorVertices();
   monitorTrends();

   counter( "#events" )++;

   return StatusCode::SUCCESS;
}

//=============================================================================
void HltGlobalMonitor::handle ( const Incident& incident )
{
   m_startEvent = System::currentTime( System::microSec );
   if ( m_startClock == 0 || incident.type() == IncidentType::BeginRun
        || incident.type() == "RunChange" ) m_startClock = m_startEvent;
   m_currentTime = double( m_startEvent - m_startClock ) / 1000000 ; // seconds
}

//==============================================================================
StatusCode HltGlobalMonitor::updateCondition()
{
  const double xRC = m_veloCondition->paramAsDouble( "ResolPosRC" );
  const double xLA = m_veloCondition->paramAsDouble( "ResolPosLA" );
  const double   Y = m_veloCondition->paramAsDouble( "ResolPosY"  );

  m_beamSpotX = ( xRC + xLA ) / 2;
  m_beamSpotY = Y;

  return StatusCode::SUCCESS;
}

//==============================================================================
void HltGlobalMonitor::monitorODIN(const LHCb::ODIN* odin,const LHCb::HltDecReports* hlt) {
   if (odin == 0 ) return;
   unsigned long long gpstime=odin->gpsTime();
   if (msgLevel(MSG::DEBUG)) debug() << "gps time" << gpstime << endreq;
   m_gpstimesec=int(gpstime/1000000-904262401); //@TODO: is this still OK with ODIN v6?
   counter("ODIN::Lumi")    += (odin->triggerType()==ODIN::LumiTrigger);
   counter("ODIN::NotLumi") += (odin->triggerType()!=ODIN::LumiTrigger);
   fill(m_odin, odin->triggerType(), 1.);

   //now check HLT decisions for rejected events after Hlt
   if (hlt!=0){
      const LHCb::HltDecReport*  report = hlt->decReport( m_hlt1Decision );
      if (report != 0 ) fill(m_odinHLT1, odin->triggerType(), 1.);
    
      report = hlt->decReport( m_hlt2Decision );
      if (report != 0 ) fill(m_odinHLT2, odin->triggerType(), 1.);
    
   }//end if hlt

   return;
}

//==============================================================================
void HltGlobalMonitor::monitorHLT(const LHCb::ODIN* /*odin*/, const LHCb::HltDecReports* hlt) {
   if (hlt==0) return;

   std::vector<unsigned> nAcc1Alley(m_hlt1Alleys.size(),unsigned(0));
   std::vector<unsigned> nAcc2Alley(m_hlt2Alleys.size(),unsigned(0));

   for (LHCb::HltDecReports::Container::const_iterator i = hlt->begin(); i!=hlt->end();++i) {
      if (!i->second.decision() ) continue;

      if ( i->first.compare(0,4,"Hlt1" ) == 0 ) {
         std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator j = m_hlt1Line2AlleyBin.find(i->first);
         if (j!=m_hlt1Line2AlleyBin.end()) {
            assert(j->second.first<nAcc1Alley.size());
            ++nAcc1Alley[ j->second.first ];
            fill( m_hlt1Alleys[j->second.first], j->second.second, 1.0 );
         }
      } else if ( i->first.compare(0,4,"Hlt2") == 0 ) {
         std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator j = m_hlt2Line2AlleyBin.find(i->first);
         if (j!=m_hlt2Line2AlleyBin.end()) {
            assert(j->second.first<nAcc2Alley.size());
            ++nAcc2Alley[ j->second.first ];
            fill( m_hlt2Alleys[j->second.first], j->second.second, 1.0 );
         }
      } else {
         warning() << "got unexpected decision name " << i->first << endmsg;
      }
   }

   //filling the histograms for the alleys instead of the lines
   for (unsigned i=0; i<m_DecToGroup1.size();i++) {
      *m_hlt1AlleyRates[i] += ( nAcc1Alley[i] > 0 );
      fill(m_hlt1Alley,i,(nAcc1Alley[i]>0));
      if(nAcc1Alley[i]==0) continue;
      for(unsigned j=0; j<m_DecToGroup1.size();++j){
         fill(m_hlt1AlleysCorrelations,i,j,(nAcc1Alley[j]>0));
      }
   }
   for (unsigned i=0; i<m_DecToGroup2.size();++i) {
      *m_hlt2AlleyRates[i] += ( nAcc2Alley[i] > 0 );
      fill(m_hlt2Alley,i,(nAcc2Alley[i]>0));
      if(nAcc2Alley[i]==0) continue;
      for(unsigned j=0; j<m_DecToGroup2.size();++j){
         fill(m_hlt2AlleysCorrelations,i,j,(nAcc2Alley[j]>0));
      }
   }
  
}

//==============================================================================
void HltGlobalMonitor::monitorVertices()
{
   typedef LHCb::RecVertex::Container Vertices;
   BOOST_FOREACH( const LocationMap::value_type& entry, m_vertexLocations ) {
      const std::string loc = entry.second;
      if ( !exist< Vertices >( loc ) ) continue;
      Vertices* vertices = get< Vertices >( loc );
      const HistoVector& histos = m_vertexHistos[ entry.first ];
      BOOST_FOREACH( AIDA::IHistogram2D* histo, histos ) {
         BOOST_FOREACH( const LHCb::RecVertex* vx, *vertices ) {
            double dx = m_beamSpotX - vx->position().x();
            double dy = m_beamSpotY - vx->position().y();
            histo->fill( dx, dy );
         }
      }
   }
}

//==============================================================================
void HltGlobalMonitor::monitorTrends()
{

   double elapsedTime = double(System::currentTime( System::microSec ) - m_startEvent);
   fill( m_hltTime, log10(elapsedTime)-3 ,1.0);  // convert to log(time/ms)

   double t =  elapsedTime/1000; // convert to ms

   double when = m_currentTime / 60;

   m_hltEventsTime->fill(when, t);

   //monitor CPU time vs evt size
   size_t evtSize = 0;
   if( exist<LHCb::RawEvent>(m_rawEventLocation) ){
      RawEvent* evt = get<LHCb::RawEvent>(m_rawEventLocation);
      evtSize = rawEvtLength(evt);
   }
   m_hltTimeVsEvtSize->fill(evtSize, t );

   // monitor CPU time vs # of PVs, # of RZVelo tracks, ...
   for (std::vector<histopair>::iterator i = m_CPUcorrelations.begin();
        i!= m_CPUcorrelations.end(); ++i) (*i)(t);

   int i = m_hltVirtTime->axis().coordToIndex( when );
   if ( m_hltVirtTime->binEntries(i)==0 ) {
      m_virtmem  = virtualMemory(System::MByte, System::Memory);
      m_hltVirtTime->fill(when, double(m_virtmem));
   }
   // histogram the # of tasks vs. time...
   i = m_tasks->axis().coordToIndex( when );
   if ( m_tasks->binEntries(i)==0 ) m_tasks->fill( when, 1 );
 
}

//==============================================================================
size_t HltGlobalMonitor::rawEvtLength(const LHCb::RawEvent* evt)
{
   size_t len = 0;
   RawEvent* raw = const_cast<RawEvent*>(evt);
   for( size_t i = RawBank::L0Calo; i < RawBank::LastType; ++i ) {
      len += rawEvtLength( raw->banks( RawBank::BankType( i ) ) );
   }
   return len;
}
//==============================================================================
/// Determine length of the sequential buffer from RawEvent object
size_t HltGlobalMonitor::rawEvtLength( const std::vector< RawBank* >& banks )
{
   size_t len = 0;
   for(std::vector<RawBank*>::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
      len += (*j)->totalSize();
   }
   return len;
}
