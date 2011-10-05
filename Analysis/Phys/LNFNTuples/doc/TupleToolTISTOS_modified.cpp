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

//#include <sstream>

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
  : TupleToolTriggerBase ( type, name , parent )
    , m_TriggerTisTosTool()
    , m_L0TriggerTisTosTool()
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("TriggerTisTosName",m_TriggerTisTosName="");  
  declareProperty("L0TriggerTisTosName",m_L0TriggerTisTosName="");  

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolTISTOS::~TupleToolTISTOS() {} 

//=============================================================================

//=========================================================================
//  initialize
//=========================================================================
StatusCode TupleToolTISTOS::initialize( ) 
{
  
  StatusCode sc = TupleToolTriggerBase::initialize();
  if (!sc) return sc;
  if( m_TriggerTisTosName != "" ){
    m_TriggerTisTosTool = tool<ITriggerTisTos>( m_TriggerTisTosName,this);
  } else {
    m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
  }

  if( m_L0TriggerTisTosName != "" ){
    m_L0TriggerTisTosTool = tool<ITriggerTisTos>( m_L0TriggerTisTosName,this);
  } else {
    m_L0TriggerTisTosTool = tool<ITriggerTisTos>( "L0TriggerTisTos",this);
  }
  if( !m_TriggerTisTosTool )  return StatusCode::FAILURE;
  if( !m_L0TriggerTisTosTool )  return StatusCode::FAILURE;
  
  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode TupleToolTISTOS::finalize( ) 
{
  return TupleToolTriggerBase::finalize();
}

//=========================================================================
//  Fill
//=========================================================================
StatusCode TupleToolTISTOS::fillBasic( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple )
{
  
  
  if(  fabs( P->particleID().pid()) < 500 ) return StatusCode::SUCCESS; 
  // Uncomment the following line for MuonIDCalib
  //if(  fabs( P->particleID().pid()) != 13 ) return StatusCode::SUCCESS; 

  const std::string prefix=fullName(head);
 
  ITriggerTisTos::TisTosDecision classifiedDec;

  m_L0TriggerTisTosTool->setOfflineInput(*P);
  m_L0TriggerTisTosTool->setTriggerInput("L0.*Decision");
  classifiedDec = m_L0TriggerTisTosTool->triggerTisTos();
  tuple->column( prefix+"L0Global"+"_Dec", classifiedDec.decision());
  tuple->column( prefix+"L0Global"+"_TIS", classifiedDec.tis());
  tuple->column( prefix+"L0Global"+"_TOS", classifiedDec.tos());
  
 
  m_TriggerTisTosTool->setOfflineInput(*P);

  //Do the Hlt1
  m_TriggerTisTosTool->setTriggerInput("Hlt1.*Decision");
  //Fill the decision, tis and tos parametres for the Hlt1 as a whole   
  classifiedDec = m_TriggerTisTosTool->triggerTisTos();
  tuple->column( prefix+"Hlt1Global"+"_Dec", classifiedDec.decision());
  tuple->column( prefix+"Hlt1Global"+"_TIS", classifiedDec.tis());
  tuple->column( prefix+"Hlt1Global"+"_TOS", classifiedDec.tos());
    

  //Do the Hlt2
  m_TriggerTisTosTool->setTriggerInput("Hlt2.*Decision");
  //Fill the decision, tis and tos parametres for the Hlt2 as a whole   
  classifiedDec = m_TriggerTisTosTool->triggerTisTos();
  tuple->column( prefix+"Hlt2Global"+"_Dec", classifiedDec.decision());
  tuple->column( prefix+"Hlt2Global"+"_TIS", classifiedDec.tis());
  tuple->column( prefix+"Hlt2Global"+"_TOS", classifiedDec.tos());
    
  //Do the Hlt2Phys
  m_TriggerTisTosTool->setTriggerInput("'Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Transparent)(?!PassThrough)'");
  //Fill the decision, tis and tos parametres for the Hlt2 as a whole   
  classifiedDec = m_TriggerTisTosTool->triggerTisTos();
  tuple->column( prefix+"Hlt2Phys"+"_Dec", classifiedDec.decision());
  tuple->column( prefix+"Hlt2Phys"+"_TIS", classifiedDec.tis());
  tuple->column( prefix+"Hlt2Phys"+"_TOS", classifiedDec.tos());
    

  
  return StatusCode::SUCCESS;
}

StatusCode TupleToolTISTOS::fillVerbose( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple )
{
  
  if( fabs( P->particleID().pid()) <500 ) return StatusCode::SUCCESS; /// FSoomro

  const std::string prefix=fullName(head);
 
  bool decision = false;
  bool tis = 0;
  bool tos = 0;
  
  if( m_verboseL0 )
  {      
    m_L0TriggerTisTosTool->setOfflineInput(*P);
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_l0.begin();s!= m_l0.end();++s){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
        m_L0TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
        tuple->column( prefix+*s+"_Dec", decision);
        tuple->column( prefix+*s+"_TIS", tis);
        tuple->column( prefix+*s+"_TOS", tos);
      }
  }

  m_TriggerTisTosTool->setOfflineInput(*P);
  if( m_verboseHlt1 )
  {
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_hlt1.begin();s != m_hlt1.end();++s){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
        if( s->find("Hlt1L0") == 0 ){
          // assume that we want to look at L0 rather than Hlt1L0 decisions
          m_TriggerTisTosTool->selectionTisTos(*s,decision,tis,tos);
          decision = m_TriggerTisTosTool->hltSelectionObjectSummaries(*s).size()!=0;      
        } else {
          m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
        }        
        tuple->column( prefix+*s+"_Dec", decision);
        tuple->column( prefix+*s+"_TIS", tis);
        tuple->column( prefix+*s+"_TOS", tos);
      }
  }
  
  if( m_verboseHlt2 )
  {      
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_hlt2.begin();s != m_hlt2.end();++s)
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
      m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
      tuple->column( prefix+*s+"_Dec", decision);
      tuple->column( prefix+*s+"_TIS", tis);
      tuple->column( prefix+*s+"_TOS", tos);
    }
  }
  return StatusCode::SUCCESS;
}
