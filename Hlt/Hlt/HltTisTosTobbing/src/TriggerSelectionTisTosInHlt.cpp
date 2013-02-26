// $Id: TriggerSelectionTisTosInHlt.cpp,v 1.10 2010-08-17 08:54:52 graven Exp $
// Include files 
#include <algorithm>
#include <vector>
#include <sstream>

// from Gaudi    
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "TriggerSelectionTisTosInHlt.h"

#include "Event/HltDecReports.h"
#include "Event/HltCandidate.h"

#include "HltBase/HltSelection.h"

using namespace LHCb;

namespace {

    // TODO: use transform instead of explicit loop...
    
    template<typename T, typename I>
    std::vector<T*> convert(I i,I end) {
           std::vector<T*> v; v.reserve( end-i );
           while (i!=end)  v.push_back( const_cast<T*>( *i++ ) );
           return v;
    }

    template<typename T, typename I>
    std::vector<T*> convertCandidate(I i,I end) {
           std::vector<T*> v; v.reserve( end-i );
           while (i!=end)  v.push_back( const_cast<T*>( (*i++)->currentStage()->template get<T>() ) );
           return v;
    }
    

};

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTosInHlt
//
// 2009-10-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTosInHlt );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTosInHlt::TriggerSelectionTisTosInHlt( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : ParticleTisTos ( type, name , parent )
  , m_hltDecReports(0)
  , m_hltDataSvc(0)
  , m_hltInspectorSvc(0)
  , m_objectSummaries(0)
{
  declareInterface<ITriggerSelectionTisTos>(this);
  
   declareProperty("HltDecReportsLocation",
                   m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default); 
   
  m_cached_SelectionNames.reserve(500);
  m_cached_tisTosTob.reserve(500);

}


//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTosInHlt::~TriggerSelectionTisTosInHlt() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTosInHlt::initialize() {
  StatusCode sc = ParticleTisTos::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  IIncidentSvc*                incidentSvc(0);     
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->release();

  m_hltDataSvc = svc<Hlt::IData>("Hlt::Service",true);
  m_hltInspectorSvc = svc<Hlt::IInspector>("Hlt::Service",true);

  m_newEvent =true;
   
  setOfflineInput();
  
  return StatusCode::SUCCESS;

}

void TriggerSelectionTisTosInHlt::handle(const Incident& ) 
{
  m_hltDecReports=0;

  setOfflineInput( );

  m_newEvent=true;  

  m_objectSummaries =0;
  
}

// ------------------------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::getHltSummary()
{
  if( !m_hltDecReports ){
    m_hltDecReports = getIfExists<HltDecReports>(m_HltDecReportsLocation);
    if( NULL==m_hltDecReports )
    {
      Error( " No HltDecReports at "+m_HltDecReportsLocation.value(), StatusCode::FAILURE, 2 ).setChecked();
      m_hltDecReports =0;
    }    
  }  
}

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTosInHlt::setOfflineInput( )
{
  setSignal();  clearCache();
}
   

//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist )
{
  if( addToSignal( hitlist ) )clearCache();
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const LHCb::Track & track )
{
  if( addToSignal( track ) )clearCache();
}

//    Proto-particle input -----------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const LHCb::ProtoParticle & protoParticle )
{
  if( addToSignal( protoParticle ) )clearCache();
}

//    Particle input -----------------------------------------------------------------------
void TriggerSelectionTisTosInHlt::addToOfflineInput( const LHCb::Particle & particle )
{
  if( addToSignal( particle ) )clearCache();
}


 
//=============================================================================


//=============================================================================
// -------------- outputs:
//=============================================================================



// single complete Trigger Selection TisTos  (define Offline Input before calling)
unsigned int TriggerSelectionTisTosInHlt::tisTosSelection( const std::string & selectionName )
{
  unsigned int result=0;
  if( findInCache( selectionName, result ) )return result;

  getHltSummary();

  bool decision(false);  
  // get decision from HltDecReports if can find it
  bool decFound(false);  
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep ){
      decision = rep->decision();
      decFound = true;
    }    
  }
  if( decision )result |= kDecision;  

  const Gaudi::StringKey name(selectionName);
  // find producer of the selection
  const IAlgorithm* producer = m_hltInspectorSvc->producer(name);
  if( !producer ){storeInCache(selectionName,result); return result;}
  const Hlt::Selection* sel = m_hltDataSvc->retrieve(producer,name);
  if( !sel ){ storeInCache(selectionName,result); return result;}
  // unsuccessful selections not allowed to be TisTossed
  if( !(sel->decision()) ){ storeInCache(selectionName,result); return result;}

  // it is possible that decision was not stored in HltDecReports if this was an intermediate step, thus
  //   if not found in HltDecReports, set it here
  // do not overwrite decisions from HltDecReports, since positive selection could have been killed by postscale
  if( !decFound )decision=true;

  // classify the decision
  if( sel->size() > 0 ){

    if( sel->classID() == Hlt::Candidate::classID() ) {
      const Hlt::TSelection<Hlt::Candidate>& tsel = dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>(*sel);   
      if (tsel.size() >0) {
          // grab the first one, and pray for a uniform list...
          const Hlt::Candidate *cand = tsel.front();
          const Hlt::Stage* stage = cand->currentStage();
          if ( stage->get<LHCb::Track>() != 0 ) {
            result = IParticleTisTos::tisTos<Track>( convertCandidate<Track>( tsel.begin(), tsel.end() ));
          } else if (stage->get<LHCb::RecVertex>()!=0) {
            result = IParticleTisTos::tisTos<RecVertex>( convertCandidate<RecVertex>( tsel.begin(), tsel.end() ));
          } else {
            error() << " got a candidate which is neither Track nor RecVertex... " << endmsg;
          }

      }

    } else if( sel->classID() == LHCb::Track::classID() ) {

      const Hlt::TrackSelection& tsel = dynamic_cast<const Hlt::TrackSelection&>(*sel);   
      if (tsel.size() >0) {
        result = IParticleTisTos::tisTos<Track>( convert<Track>( tsel.begin(), tsel.end() ));
      }
      
    } else if( sel->classID() == LHCb::RecVertex::classID() ) {

      const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);     
      if (tsel.size() >0) {
        result = IParticleTisTos::tisTos<RecVertex>( convert<RecVertex>(tsel.begin(),tsel.end() ) );
      }

    }  else if( sel->classID() == LHCb::Particle::classID() ) {

      const Hlt::TSelection<LHCb::Particle>& tsel = dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>(*sel);   
      if( tsel.size() > 0 ){
        result = IParticleTisTos::tisTos<Particle>(convert<Particle>( tsel.begin(), tsel.end() ));
      }

    } else {
      std::vector<std::string> dependencies( sel->inputSelectionsIDs().begin(), sel->inputSelectionsIDs().end());
      if (!dependencies.empty()) {
        result = ITriggerSelectionTisTos::tisTosSelection(dependencies);        
      } else {
        // @TODO: warning: hltDataSvc doesn't know about selInput...
      }

    }
  }

  if( decision )result |= kDecision; 
  storeInCache(selectionName,result);
  return result;  
}



// single complete Trigger Selection TisTos  (define Offline Input before calling)
std::string TriggerSelectionTisTosInHlt::analysisReportSelection( const std::string & selectionName )
{


  std::ostringstream report;
  report << offset() << " Selection " + selectionName + " ";  

  unsigned int result=0;

  getHltSummary();

  bool decision(false);  
  // get decision from HltDecReports if can find it
  bool decFound(false);  
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep ){
      decision = rep->decision();
      decFound = true;
      report << " HltDecReport decision=" << decision;   
    }    
  }
  if( decision )result |= kDecision;  

  const Gaudi::StringKey name(selectionName);
  // find producer of the selection
  const IAlgorithm* producer = m_hltInspectorSvc->producer(name);
  if( !producer ){ report << "not known to HltInspectorSvc " << std::endl; return report.str();}
  const Hlt::Selection* sel = m_hltDataSvc->retrieve(producer,name);
  if( !sel ){ report << "not found in HltDataSvc " << std::endl; return report.str();}
  // unsuccessful selections not allowed to be TisTossed
  if( !(sel->decision()) ){ report << "decision=false " << std::endl; return report.str();}

  // it is possible that decision was not stored in HltDecReports if this was an intermediate step, thus
  //   if not found in HltDecReports, set it here
  // do not overwrite decisions from HltDecReports, since positive selection could have been killed by postscale
  if( !decFound )decision=true;

  report << " size " << sel->size() << std::endl;

  if( sel->size() > 0 ){

    if( sel->classID() == LHCb::Track::classID() ) {

      const Hlt::TrackSelection& tsel = dynamic_cast<const Hlt::TrackSelection&>(*sel);   
      if (tsel.size() >0) {
        std::vector<Track*> tracks = convert<Track>(tsel.begin(),tsel.end());
        report << analysisReport<Track>(tracks);
        result = IParticleTisTos::tisTos<LHCb::Track>(tracks);
      }
      
    } else if( sel->classID() == LHCb::RecVertex::classID() ) {

      const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);     
      if (tsel.size() >0) {
        std::vector<RecVertex*> v = convert<RecVertex>( tsel.begin(), tsel.end() );
        report<< analysisReport<RecVertex>(v);
        result = IParticleTisTos::tisTos<RecVertex>(v);
      }

    }  else if( sel->classID() == LHCb::Particle::classID() ) {

      const Hlt::TSelection<LHCb::Particle>& tsel = dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>(*sel);   
      if( tsel.size() > 0 ){
        std::vector<Particle*> p = convert<Particle>( tsel.begin(), tsel.end() );

        report<< analysisReport<Particle>(p);
        result = IParticleTisTos::tisTos<Particle>(p);
      }

    } else {
      std::vector<std::string> dependencies( sel->inputSelectionsIDs().begin(), sel->inputSelectionsIDs().end());
      if (!dependencies.empty()) {
        report << offset() << " Input Selections Vector size=" << dependencies.size() << std::endl;  
        ++m_reportDepth;
        for( std::vector<std::string>::const_iterator iSel=dependencies.begin();iSel!=dependencies.end();++iSel){
          report << analysisReportSelection( *iSel );
        }
        --m_reportDepth;
        result = ITriggerSelectionTisTos::tisTosSelection(dependencies);        
      } else {
        report << " not a Track/RecVertex/Particle selection and no Input Selections " << std::endl;        
      }

    }
  }
  if( decision )result |= kDecision;  
  TisTosTob res( result );
  report << offset() << " Selection " + selectionName + " "  
         << " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() 
         << " decision= " << res.decision() << std::endl;
  return report.str();
}

 
// fast check for TOS
#define TOSTISTPS( FUN )\
{\
  getHltSummary();\
  if( m_hltDecReports ){\
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );\
    if( rep ){\
      if( !(rep->decision()) )return false;      \
    }    \
  }\
  const Gaudi::StringKey name(selectionName);\
  const IAlgorithm* producer = m_hltInspectorSvc->producer(name);\
  if( !producer )return false;\
  const Hlt::Selection* sel = m_hltDataSvc->retrieve(producer,name);\
  if( !sel )return false;\
  if( !(sel->decision()) )return false;\
  if( !(sel->size()) )return false;\
  if( sel->classID() == Hlt::Candidate::classID() ) {\
    const Hlt::TSelection<Hlt::Candidate>& tsel = dynamic_cast<const Hlt::TSelection<Hlt::Candidate>&>(*sel);   \
    if (tsel.size() >0) {\
          const Hlt::Candidate *cand = tsel.front(); \
          const Hlt::Stage* stage = cand->currentStage(); \
          if ( stage->get<LHCb::Track>() != 0 ) { \
            std::vector<Track*> tracks = convertCandidate<Track>(tsel.begin(),tsel.end());\
            return IParticleTisTos::FUN<LHCb::Track>(tracks);\
          } else if (stage->get<LHCb::RecVertex>()!=0) {\
            std::vector<RecVertex*> vertices = convertCandidate<RecVertex>(tsel.begin(),tsel.end());\
            return IParticleTisTos::FUN<RecVertex>(vertices);\
          } else {\
            error() << " got a candidate which is neither Track nor RecVertex... " << endmsg;\
            return false;\
          }\
    }      \
  } else if( sel->classID() == LHCb::Track::classID() ) {\
    const Hlt::TrackSelection& tsel = dynamic_cast<const Hlt::TrackSelection&>(*sel);   \
    if (tsel.size() >0) {\
      std::vector<Track*> tracks = convert<Track>(tsel.begin(),tsel.end());\
      return IParticleTisTos::FUN<LHCb::Track>(tracks);\
    }      \
  } else if( sel->classID() == LHCb::RecVertex::classID() ) {\
    const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);     \
    if (tsel.size() >0) {\
      std::vector<RecVertex*> vertices = convert<RecVertex>(tsel.begin(),tsel.end());\
      return IParticleTisTos::FUN<RecVertex>(vertices);\
    }\
  }  else if( sel->classID() == LHCb::Particle::classID() ) {\
    const Hlt::TSelection<LHCb::Particle>& tsel = dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>(*sel);   \
    if( tsel.size() > 0 ){\
      std::vector<Particle*> particles = convert<Particle>(tsel.begin(),tsel.end());\
      return IParticleTisTos::FUN<Particle>(particles);\
    }\
  } else {\
    std::vector<std::string> dependencies( sel->inputSelectionsIDs().begin(), sel->inputSelectionsIDs().end());\
    if (!dependencies.empty()) {\

bool TriggerSelectionTisTosInHlt::tosSelection( const std::string & selectionName )
TOSTISTPS( tos )
      return ITriggerSelectionTisTos::tosSelection(dependencies);        
    }
  }
  return false;  
}

bool TriggerSelectionTisTosInHlt::tisSelection( const std::string & selectionName )
TOSTISTPS( tis )
      return ITriggerSelectionTisTos::tisSelection(dependencies);        
    }
  }
  return false;  
}

bool TriggerSelectionTisTosInHlt::tusSelection( const std::string & selectionName )
TOSTISTPS( tus )
      return ITriggerSelectionTisTos::tusSelection(dependencies);        
    }
  }
  return false;  
}

// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TriggerSelectionTisTosInHlt::offlineLHCbIDs() 
{
  return signal();
}


#define TISTOSSELECTSTORE() \
      unsigned int result = tisTos(*object);\
      bool tis,tos,tps;\
      tis = result & kTIS;\
      tos = result & kTOS;\
      tps = result & kTPS;\
      if(    ((tisRequirement>=kAnything)||(tis==tisRequirement)) \
             && ((tosRequirement>=kAnything)||(tos==tosRequirement)) \
             && ((tpsRequirement>=kAnything)||(tps==tpsRequirement)) \
             ){\
        HltObjectSummary* hos(0);\
        for( HltObjectSummarys::iterator ppHos=m_objectSummaries->begin();\
             ppHos!=m_objectSummaries->end();++ppHos){\
          HltObjectSummary* pHos(*ppHos);    \
          if( pHos->summarizedObjectCLID() == object->clID() ){\
            if( pHos->summarizedObject() == object ) hos = pHos;\
          }\
        }\
        if( !hos ){          \
          hos = new HltObjectSummary();\
          m_objectSummaries->push_back(hos);\
          hos->setSummarizedObjectCLID( object->clID() );\
          hos->setSummarizedObject(object);\
        }        \
        matchedObjectSummaries.push_back( hos );\
      }\


// ------------  additional functionality:  lists of object summaries for tracks/vertices/particles from trigger selections
//               satisfying TIS, TOS requirements

std::vector<const LHCb::HltObjectSummary*> TriggerSelectionTisTosInHlt::hltSelectionObjectSummaries(
                      const std::string & selectionName,
                      unsigned int tisRequirement,
                      unsigned int tosRequirement,
                      unsigned int tpsRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> matchedObjectSummaries;
  
  getHltSummary();
  
  if( NULL==m_objectSummaries ) m_objectSummaries =  getOrCreate<HltObjectSummary::Container,HltObjectSummary::Container>("/Event/Hlt/TriggerSelectionTisTosInHltStore");
  
  if( NULL==m_objectSummaries )
  {
    error() << " Unable to create summary container object " << endmsg;
  }  


  const Gaudi::StringKey name(selectionName);
  // find producer of the selection
  const IAlgorithm* producer = m_hltInspectorSvc->producer(name);
  if( !producer )return matchedObjectSummaries;
  const Hlt::Selection* sel = m_hltDataSvc->retrieve(producer,name);
  if( !sel )return matchedObjectSummaries;
  if( !(sel->size()) )return matchedObjectSummaries;

  if( sel->classID() == LHCb::Track::classID() ) {

    const Hlt::TrackSelection& tsel = dynamic_cast<const Hlt::TrackSelection&>(*sel);   
    std::vector<Track*> objects = convert<Track>(tsel.begin(),tsel.end());
    for( std::vector<Track*>::const_iterator obj=objects.begin();obj!=objects.end();++obj){
      Track* object= *obj; 
      TISTOSSELECTSTORE()
    }

  } else if( sel->classID() == LHCb::RecVertex::classID() ) {

    const Hlt::VertexSelection& tsel = dynamic_cast<const Hlt::VertexSelection&>(*sel);     
    std::vector<RecVertex*> objects = convert<RecVertex>(tsel.begin(),tsel.end());
    for( std::vector<RecVertex*>::const_iterator obj=objects.begin();obj!=objects.end();++obj){
      RecVertex* object= *obj; 
      TISTOSSELECTSTORE()
    }


  } else if( sel->classID() == LHCb::Particle::classID() ) {

    const Hlt::TSelection<LHCb::Particle>& tsel = dynamic_cast<const Hlt::TSelection<LHCb::Particle>&>(*sel);   
    std::vector<Particle*> objects = convert<Particle>(tsel.begin(),tsel.end());
    for( std::vector<Particle*>::const_iterator obj=objects.begin();obj!=objects.end();++obj){
      Particle* object= *obj; 
      TISTOSSELECTSTORE()      
    }
    
  }
  
  return matchedObjectSummaries;
}
