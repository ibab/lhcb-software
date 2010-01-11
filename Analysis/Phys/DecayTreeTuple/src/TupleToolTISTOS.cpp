// Include files 
#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Particle.h"     
// kernel
#include "Kernel/IANNSvc.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// local
#include "TupleToolTISTOS.h"


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

  declareProperty( "VerboseL0",   m_verboseL0=true );
  declareProperty( "VerboseHlt1", m_verboseHlt1=true );
  declareProperty( "VerboseHlt2", m_verboseHlt2=true );

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

  bool decision = false;
  bool tis = 0;
  bool tos = 0;


  if( P ){
    
    if (P->isBasicParticle()) return StatusCode::SUCCESS;
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looking at a " << P->particleID().pid() << endmsg ;
    
    // fix the trigger name list on the first call
    if( m_verboseL0 || m_verboseHlt1 || m_verboseHlt2 ){
      if( !(m_l0.size()+m_hlt1.size()+m_hlt2.size()) ){
        boost::regex l0("Hlt1L0.*Decision");
        boost::regex hlt1("Hlt1.*Decision");
        boost::regex hlt2("Hlt2.*Decision");
        std::vector<std::string> vs = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID");
        for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end();++s){
          if( boost::regex_match( *s, l0 ) ) {
            m_l0.push_back(*s);
          }
          if( boost::regex_match( *s, hlt1 ) ) {
            m_hlt1.push_back(*s);
          }
        }
        vs = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt2SelectionID");
        for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end();++s){
          if( boost::regex_match( *s,  hlt2 ) ) {
            m_hlt2.push_back(*s);
          }
        }
        if( m_verboseL0 && !m_l0.size() ){
          Error(" Cannot find any Hlt1L0 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
        }
        if( m_verboseHlt1 && !m_hlt1.size() ){
          Error(" Cannot find any Hlt1 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
        }
        if( m_verboseHlt2 && !m_hlt2.size() ){
          Error(" Cannot find any Hlt2 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
        }
      }
    }
    

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
    if( m_verboseL0 ){      
      //Now loop over all the subtriggers
      for( std::vector< std::string >::const_iterator s=m_l0.begin();s != m_l0.end();++s){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg ;
        m_TriggerTisTosTool->selectionTisTos(*s,decision,tis,tos);
        // need to overwrite Hlt1L0XXXDecision 
        decision = m_TriggerTisTosTool->hltSelectionObjectSummaries(*s).size()!=0;      
        std::string sl0 = s->substr( s->find("L0") );      
        tuple->column( head+sl0+"_Dec", decision);
        tuple->column( head+sl0+"_TIS", tis);
        tuple->column( head+sl0+"_TOS", tos);
      }
    }
    //Do the Hlt1
    m_TriggerTisTosTool->setTriggerInput("Hlt1.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt1 as a whole   
    m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
    tuple->column( head+"Hlt1Global"+"_Dec", decision);
    tuple->column( head+"Hlt1Global"+"_TIS", tis);
    tuple->column( head+"Hlt1Global"+"_TOS", tos);
    if( m_verboseHlt1 ){      
      //Now loop over all the subtriggers
      for( std::vector< std::string >::const_iterator s=m_hlt1.begin();s != m_hlt1.end();++s){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
        m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
        tuple->column( head+*s+"_Dec", decision);
        tuple->column( head+*s+"_TIS", tis);
        tuple->column( head+*s+"_TOS", tos);
      }
    }
    

    //Do the Hl2
    m_TriggerTisTosTool->setTriggerInput("Hlt2.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt2 as a whole
    m_TriggerTisTosTool->triggerTisTos(decision,tis,tos);
    tuple->column( head+"Hlt2Global"+"_Dec", decision);
    tuple->column( head+"Hlt2Global"+"_TIS", tis);
    tuple->column( head+"Hlt2Global"+"_TOS", tos);
    if( m_verboseHlt2 ){      
      //Now loop over all the subtriggers
      for( std::vector< std::string >::const_iterator s=m_hlt2.begin();s != m_hlt2.end();++s){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
        m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
        tuple->column( head+*s+"_Dec", decision);
        tuple->column( head+*s+"_TIS", tis);
        tuple->column( head+*s+"_TOS", tos);
      }
    }

  } else {
    return Error("No particle to process",StatusCode::FAILURE);    
  }
  return StatusCode::SUCCESS;
}
