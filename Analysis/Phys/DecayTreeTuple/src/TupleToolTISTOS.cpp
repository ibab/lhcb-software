// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Particle.h"     
// kernel

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// local
#include "TupleToolTISTOS.h"

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
  bool decision = false;
  bool tis = 0;
  bool tos = 0;
  
  if( P ){
    
    if (P->isBasicParticle()) return StatusCode::SUCCESS;

    m_TriggerTisTosTool->setOfflineInput(*P);

    m_TriggerTisTosTool->setTriggerInput("Hlt1L0.*Decision");
    std::vector<std::string> vs = m_TriggerTisTosTool->triggerSelectionNames();
    //Fill the decision, tis and tos parametres for the L0 as a whole
    m_TriggerTisTosTool->selectionTisTos(vs,decision,tis,tos);
    // decision here is that of Hlt1 L0 pass through lines and not of L0 itself
    // however if any candidates were saved then L0 decision must have been true
    decision = m_TriggerTisTosTool->hltObjectSummaries().size()!=0;
    tuple->column( head+"L0Global"+"_Dec", decision);
    tuple->column( head+"L0Global"+"_TIS", tis);
    tuple->column( head+"L0Global"+"_TOS", tos);
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end();++s){
      m_TriggerTisTosTool->selectionTisTos(*s,decision,tis,tos);
      // need to overwrite Hlt1L0XXXDecision 
      decision = m_TriggerTisTosTool->hltSelectionObjectSummaries(*s).size()!=0;      
      std::string sl0 = s->substr( s->find("L0") );      
      tuple->column( head+sl0+"_Dec", decision);
      tuple->column( head+sl0+"_TIS", tis);
      tuple->column( head+sl0+"_TOS", tos);
    }

    //Do the Hlt1
    m_TriggerTisTosTool->setTriggerInput("Hlt1.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt1 as a whole   
    m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
    tuple->column( head+"Hlt1Global"+"_Dec", decision);
    tuple->column( head+"Hlt1Global"+"_TIS", tis);
    tuple->column( head+"Hlt1Global"+"_TOS", tos);
    //Now loop over all the subtriggers
    vs = m_TriggerTisTosTool->triggerSelectionNames();
    for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end();++s){
      m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
      tuple->column( head+*s+"_Dec", decision);
      tuple->column( head+*s+"_TIS", tis);
      tuple->column( head+*s+"_TOS", tos);
    }

    //Do the Hl2
    m_TriggerTisTosTool->setTriggerInput("Hlt2.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt2 as a whole
    m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
    tuple->column( head+"Hlt2Global"+"_Dec", decision);
    tuple->column( head+"Hlt2Global"+"_TIS", tis);
    tuple->column( head+"Hlt2Global"+"_TOS", tos);
    //Now loop over all the subtriggers
    vs = m_TriggerTisTosTool->triggerSelectionNames();
    for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end();++s){
      m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
      tuple->column( head+*s+"_Dec", decision);
      tuple->column( head+*s+"_TIS", tis);
      tuple->column( head+*s+"_TOS", tos);
    }

  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
