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
  : TupleToolTriggerBase  ( type, name , parent )
  , m_TriggerTisTosName   ( "TriggerTisTos"     )
  , m_L0TriggerTisTosName ( "L0TriggerTisTos"   )
  , m_TriggerTisTosTool   ( NULL                )
  , m_L0TriggerTisTosTool ( NULL                )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("TriggerTisTosName",m_TriggerTisTosName);
  declareProperty("L0TriggerTisTosName",m_L0TriggerTisTosName);
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
  const StatusCode sc = TupleToolTriggerBase::initialize();
  if (!sc) return sc;

  m_TriggerTisTosTool = tool<ITriggerTisTos>( m_TriggerTisTosName,"TriggerTisTos",this );
  m_L0TriggerTisTosTool = tool<ITriggerTisTos>( m_L0TriggerTisTosName,"L0TriggerTisTos",this);

  return sc;
}

//=========================================================================
//  Fill
//=========================================================================

StatusCode TupleToolTISTOS::fillBasic( const LHCb::Particle*
                                       , const LHCb::Particle* P
                                       , const std::string& head
                                       , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);

  ITriggerTisTos::TisTosDecision classifiedDec;

  if(m_doL0)
  {

    m_L0TriggerTisTosTool->setOfflineInput(*P);
    m_L0TriggerTisTosTool->setTriggerInput("L0.*Decision");
    classifiedDec = m_L0TriggerTisTosTool->triggerTisTos();
    tuple->column( prefix+"L0Global"+"_Dec", classifiedDec.decision());
    tuple->column( prefix+"L0Global"+"_TIS", classifiedDec.tis());
    tuple->column( prefix+"L0Global"+"_TOS", classifiedDec.tos());
  }


  m_TriggerTisTosTool->setOfflineInput(*P);

  if(m_doHlt1)
  {

    //Do the Hlt1
    m_TriggerTisTosTool->setTriggerInput("Hlt1.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt1 as a whole
    classifiedDec = m_TriggerTisTosTool->triggerTisTos();
    tuple->column( prefix+"Hlt1Global"+"_Dec", classifiedDec.decision());
    tuple->column( prefix+"Hlt1Global"+"_TIS", classifiedDec.tis());
    tuple->column( prefix+"Hlt1Global"+"_TOS", classifiedDec.tos());
  }


  if(m_doHlt2)
  {

    //Do the Hlt2
    m_TriggerTisTosTool->setTriggerInput("Hlt2.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt2 as a whole
    classifiedDec = m_TriggerTisTosTool->triggerTisTos();
    tuple->column( prefix+"Hlt2Global"+"_Dec", classifiedDec.decision());
    tuple->column( prefix+"Hlt2Global"+"_TIS", classifiedDec.tis());
    tuple->column( prefix+"Hlt2Global"+"_TOS", classifiedDec.tos());
  }

  if(m_doHlt2)
  {

    //Do the Hlt2
    m_TriggerTisTosTool->setTriggerInput("Stripping.*Decision");
    //Fill the decision, tis and tos parametres for the Hlt2 as a whole
    classifiedDec = m_TriggerTisTosTool->triggerTisTos();
    tuple->column( prefix+"StrippingGlobal"+"_Dec", classifiedDec.decision());
    tuple->column( prefix+"StrippingGlobal"+"_TIS", classifiedDec.tis());
    tuple->column( prefix+"StrippingGlobal"+"_TOS", classifiedDec.tos());
  }


  return StatusCode::SUCCESS;
}

StatusCode TupleToolTISTOS::fillVerbose( const LHCb::Particle*
                                         , const LHCb::Particle* P
                                         , const std::string& head
                                         , Tuples::Tuple& tuple )
{
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

  if( m_verboseStripping )
  {
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_stripping.begin();s != m_stripping.end();++s)
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
