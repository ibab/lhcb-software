// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Particle.h"     
// kernel

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// local
#include "TupleToolTISTOS.h"
#include "Event/L0DUReport.h"

#include "Event/HltSummary.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTISTOS
//
// 2008-04-09 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolTISTOS );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTISTOS::TupleToolTISTOS( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
, m_TriggerTisTosTool()
, m_particleDescendants(0)
{
  declareInterface<IParticleTupleTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolTISTOS::~TupleToolTISTOS() {} 

//=============================================================================

//=========================================================================
//  initialize
//=========================================================================
StatusCode TupleToolTISTOS::initialize( ) {
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
  m_particleDescendants = tool<IParticleDescendants>("ParticleDescendants",this);
  return sc;
}

//=========================================================================
//  Fill
//=========================================================================
StatusCode TupleToolTISTOS::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  bool test = true;
  
  if( P ){
    
    if (P->isBasicParticle()) return StatusCode::SUCCESS;

    if( !exist<L0DUReport>( L0DUReportLocation::Default ) || 
        !exist<HltSummary>( LHCb::HltSummaryLocation::Default)
      ){ //Require both trigger decisions to have saved a valid summary
      setBadTriggerDecision(test,head,tuple);
      return StatusCode(test); 
    } 
    const L0DUReport* pL0DUReport = get<L0DUReport>( L0DUReportLocation::Default );
    verbose() << " L0 decision= " << pL0DUReport->decision() << endmsg;

    std::vector<std::string> l0NamesInHlt = m_TriggerTisTosTool->triggerSelectionNames("L0Trigger*", 
                                                                                     ITriggerTisTos::kAllTriggerSelections);
    verbose() << " Names of L0 triggers in Hlt="; dumpvs( l0NamesInHlt );  verbose() << endmsg;

    //  Hlt overall
    const HltSummary* pHltSummary = get<HltSummary>(LHCb::HltSummaryLocation::Default);
    verbose() << " Hlt final decision= " << pHltSummary->decision() << endmsg;

    //  Hlt Alleys
    bool hltAlleysDecision,dummyTis,dummyTos;
    m_TriggerTisTosTool->setOfflineInput();  
    m_TriggerTisTosTool->triggerTisTos("*",hltAlleysDecision,dummyTis,dummyTos);
    verbose() << " Hlt Alleys decision= " << hltAlleysDecision;
    verbose() << " from OR between "; dumpvs( m_TriggerTisTosTool->triggerSelectionNames() ); verbose() << endmsg;

    //  Hlt Selections (B and D)
    bool hltSelDecision;
    std::vector< std::string > hltSel;
    hltSel.push_back("HltSelD*");
    hltSel.push_back("HltSelB*");
    //done m_TriggerTisTosTool->setOfflineInput();   
    m_TriggerTisTosTool->triggerTisTos(hltSel,hltSelDecision,dummyTis,dummyTos, ITriggerTisTos::kAllTriggerSelections);
    verbose() << " HltSelections (B and D) decision= " << hltSelDecision;
    verbose() << " from OR between "; dumpvs( m_TriggerTisTosTool->triggerSelectionNames() ); verbose() << endmsg;

    //     see which Alley triggers were on
    //done m_TriggerTisTosTool->setOfflineInput();   
    std::vector<std::string> alleysPass = m_TriggerTisTosTool->triggerSelectionNames(
    "*",ITriggerTisTos::kAlleyExitsOnly,
    ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything, ITriggerTisTos::kAnything);
    verbose() << " Hlt Alley triggers that fired="; dumpvs( alleysPass );  verbose() << endmsg;

    //     see which Hlt Selections were on
    //done m_TriggerTisTosTool->setOfflineInput();   
    std::vector<std::string> selPass = m_TriggerTisTosTool->triggerSelectionNames(
    hltSel,ITriggerTisTos::kAllTriggerSelections,
    ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything, ITriggerTisTos::kAnything);
    verbose() << " Hlt Selections that succedded="; dumpvs( selPass );  verbose() << endmsg;

    // Particle Info ---------------------------------------------------------------------------
    const std::vector<const Particle*> finals = m_particleDescendants->finalStates(P);
    verbose() << "    Particle " << (P)->particleID().pid() << " Pt " << (P)->pt()
           << " #-of-daughters " << ((P)->daughtersVector()).size()
           << " #-of-final-state-part " << finals.size();
    for( std::vector<const Particle*>::const_iterator pf=finals.begin();pf!=finals.end();++pf){
      verbose() << " " << (*pf)->particleID().pid() << " Pt " << (*pf)->pt();
    }
    verbose() << endmsg;

    // L0 Info ---------------------------------------------------------------------------
    bool decisionL0,tisL0,tosL0;
    m_TriggerTisTosTool->triggerTisTos(*P,"L0Trigger*",decisionL0,tisL0,tosL0, ITriggerTisTos::kAllTriggerSelections);
    if( !decisionL0 ) {
      test &= tuple->column( head+"_L0TIS", -1 );
      test &= tuple->column( head+"_L0TOS", -1 );
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TIS",-1,head,tuple);
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TOS",-1,head,tuple);
      //return StatusCode(test);    
    } else {

      if( tisL0 ){
        verbose() << "          L0 TIS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                     ITriggerTisTos::kTrueRequired,
                                                     ITriggerTisTos::kAnything ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_L0TIS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
        test &= tuple->column( head+"_L0TIS", 0 );
      }
      if( tosL0 ){
        verbose() << "          L0 TOS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kTrueRequired ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_L0TOS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kTrueRequired ), test,"TOS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kFalseRequired ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
        test &= tuple->column( head+"_L0TOS", 0 );
      }
    }

    // Hlt Alley Info ----------------------------------------------------------------------
    bool decisionAlleys,tisAlleys,tosAlleys;
    m_TriggerTisTosTool->triggerTisTos("*",decisionAlleys,tisAlleys,tosAlleys);
    if( !decisionAlleys ) {
      test &= tuple->column( head+"_HLTALLEYSTIS", -1 );
      test &= tuple->column( head+"_HLTALLEYSTOS", -1 );
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TIS",-1,head,tuple);
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TOS",-1,head,tuple);
      //return StatusCode(test);    
    } else {
    
      if( tisAlleys ){
        verbose() << "          Hlt Alleys TIS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                     ITriggerTisTos::kTrueRequired,
                                                     ITriggerTisTos::kAnything ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_HLTALLEYSTIS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
        test &= tuple->column( head+"_HLTALLEYSTIS", 0 );
      }
      if( tosAlleys ){
        verbose() << "          Hlt Alleys TOS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                     ITriggerTisTos::kAnything,
                                                     ITriggerTisTos::kTrueRequired ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_HLTALLEYSTOS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kTrueRequired ), test,"TOS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kFalseRequired ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
        test &= tuple->column( head+"_HLTALLEYSTOS", 0 );
      }
    }

    // Hlt SelB Info --------------------------------------------------------------------
    bool decisionSelB,tisSelB,tosSelB;
    m_TriggerTisTosTool->triggerTisTos("HltSelB*",decisionSelB,tisSelB,tosSelB, ITriggerTisTos::kAllTriggerSelections);
    if( !decisionSelB ) {
      test &= tuple->column( head+"_HLTSELTIS", -1 );
      test &= tuple->column( head+"_HLTSELTOS", -1 );
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TIS",-1,head,tuple);
      saveTriggers(m_TriggerTisTosTool->triggerSelectionNames(), test,"TOS",-1,head,tuple);
      //return StatusCode(test);    
    } else {
    
      if( tisSelB ){
        verbose() << "          Hlt SelB TIS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                     ITriggerTisTos::kTrueRequired,
                                                     ITriggerTisTos::kAnything ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_HLTSELTIS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TIS",-1,head,tuple);
        test &= tuple->column( head+"_HLTSELTIS", 0 );
      }
      if( tosSelB ){
        verbose() << "          Hlt SelB TOS selections= ";
        dumpvs( m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kAnything, 
                                                     ITriggerTisTos::kAnything,
                                                     ITriggerTisTos::kTrueRequired ) );
        verbose() << endmsg;

        test &= tuple->column( head+"_HLTSELTOS", 1 );
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kTrueRequired ), test,"TOS",1,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kFalseRequired ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
      } else {
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kTrueRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",0,head,tuple);
        saveTriggers(m_TriggerTisTosTool->triggerSelectionNames( ITriggerTisTos::kFalseRequired,
                                                   ITriggerTisTos::kAnything,
                                                   ITriggerTisTos::kAnything ), test,"TOS",-1,head,tuple);
        test &= tuple->column( head+"_HLTSELTOS", 0 );
      }
    }
   
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
