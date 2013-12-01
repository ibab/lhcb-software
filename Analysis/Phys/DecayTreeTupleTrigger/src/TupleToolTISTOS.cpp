// Include files 
#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/Particle.h"
#include "Event/HltDecReports.h"
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
DECLARE_TOOL_FACTORY( TupleToolTISTOS )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolTISTOS::TupleToolTISTOS( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : TupleToolTriggerBase  ( type, name , parent )
    , m_TriggerTisTosName   ( "TriggerTisTos"     )
    , m_Hlt1TriggerTisTosName   ( "Hlt1TriggerTisTos"     )
    , m_Hlt2TriggerTisTosName   ( "Hlt2TriggerTisTos"     )
    , m_L0TriggerTisTosName ( "L0TriggerTisTos"   )
    , m_TriggerTisTosTool   ( NULL                )
    , m_Hlt1TriggerTisTosTool   ( NULL                )
    , m_Hlt2TriggerTisTosTool   ( NULL                )
    , m_L0TriggerTisTosTool ( NULL                )
    , m_pidList (0)//1, 22)
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("TriggerTisTosName",m_TriggerTisTosName);
  declareProperty("Hlt1TriggerTisTosName",m_Hlt1TriggerTisTosName);
  declareProperty("Hlt2TriggerTisTosName",m_Hlt2TriggerTisTosName);
  declareProperty("L0TriggerTisTosName",m_L0TriggerTisTosName);
  declareProperty("PIDList", m_pidList);
  declareProperty("TopParticleOnly", m_onlyTop = false );
  declareProperty("Hlt1Phys", m_hlt1Phys = "Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision");
  declareProperty("Hlt2Phys", m_hlt2Phys = "Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision") ;
  declareProperty("TIS", m_TIS = true );
  declareProperty("TOS", m_TOS = true );
  declareProperty("TUS", m_TUS = false );
  declareProperty("TPS", m_TPS = false );
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
  if ( sc.isFailure() ) return sc;

  m_TriggerTisTosTool   = tool<ITriggerTisTos>( m_TriggerTisTosName,"TriggerTisTos",this );
  m_Hlt1TriggerTisTosTool   = tool<ITriggerTisTos>( m_Hlt1TriggerTisTosName,"Hlt1TriggerTisTos",this );
  m_Hlt2TriggerTisTosTool   = tool<ITriggerTisTos>( m_Hlt2TriggerTisTosName,"Hlt2TriggerTisTos",this );
  m_L0TriggerTisTosTool = tool<ITriggerTisTos>( m_L0TriggerTisTosName,"L0TriggerTisTos",this);

  return sc;
}

//=========================================================================
//  Fill
//=========================================================================
StatusCode TupleToolTISTOS::fillBasic( const LHCb::Particle* top
                                       , const LHCb::Particle* P
                                       , const std::string& head
                                       , Tuples::Tuple& tuple )
{
  if( m_onlyTop ){
    if( P != top )return StatusCode::SUCCESS;
  }

  checkPID = false;

  // check if the particle matched any of the PIDs given in list IF there is a list
  if ( !m_pidList.empty() )
  {
  
    for ( std::vector<int>::const_iterator it = m_pidList.begin(); m_pidList.end()!=it ; ++it )
    {

      if(!checkPID && abs(P->particleID().pid() )==*it ) checkPID = true;
    }

    if( !checkPID) { return StatusCode::SUCCESS; }
  }

  if ( m_pidList.empty() ) { checkPID = true; }

  if ( checkPID ) 
  {

    if (msgLevel(MSG::VERBOSE))
      verbose() << "Filling TISTOS information for particle with pid " 
                << P->particleID().pid() << endmsg;

    const std::string prefix=fullName(head);

    //    ITriggerTisTos::TisTosDecision classifiedDec;
    ITriggerTisTos::TisTosTob classifiedDec;

    m_TriggerTisTosTool->setOfflineInput(*P);

    if(m_doL0)
    {

      //       m_L0TriggerTisTosTool->setOfflineInput(*P);
      // much faster to copy hits
      m_L0TriggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
      m_L0TriggerTisTosTool->setTriggerInput("L0.*Decision");
      //classifiedDec = m_L0TriggerTisTosTool->triggerTisTos();
      classifiedDec = m_L0TriggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_L0Global_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_L0Global_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_L0Global_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_L0Global_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_L0Global_TPS", classifiedDec.tps());
    }

    // discover if split Hlt1,Hlt2 or old style 
    bool split(false);
    split = exist<LHCb::HltDecReports>("Hlt1/DecReports",false);    
    if( !split ) split= exist<LHCb::HltDecReports>("Hlt1/DecReports");

    if(m_doHlt1)
    {

      ITriggerTisTos * triggerTisTosTool = m_TriggerTisTosTool;
      if( split ){
	triggerTisTosTool = m_Hlt1TriggerTisTosTool;
        triggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
      }

      //Do the Hlt1
      triggerTisTosTool->setTriggerInput("Hlt1.*Decision");
      //Fill the decision, tis and tos parametres for the Hlt1 as a whole
      //      classifiedDec = triggerTisTosTool->triggerTisTos();
      classifiedDec = triggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_Hlt1Global_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_Hlt1Global_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_Hlt1Global_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_Hlt1Global_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_Hlt1Global_TPS", classifiedDec.tps());

      // Hlt1Phys
      triggerTisTosTool->setTriggerInput(m_hlt1Phys);
      //      classifiedDec = triggerTisTosTool->triggerTisTos();
      classifiedDec = triggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_Hlt1Phys_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_Hlt1Phys_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_Hlt1Phys_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_Hlt1Phys_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_Hlt1Phys_TPS", classifiedDec.tps());
    }


    if(m_doHlt2)
    {

      ITriggerTisTos * triggerTisTosTool = m_TriggerTisTosTool;
      if( split ){
	triggerTisTosTool = m_Hlt2TriggerTisTosTool;
        triggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
      }

      //Do the Hlt2
      triggerTisTosTool->setTriggerInput("Hlt2.*Decision");
      //Fill the decision, tis and tos parametres for the Hlt2 as a whole
      //      classifiedDec = triggerTisTosTool->triggerTisTos();
      classifiedDec = triggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_Hlt2Global_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_Hlt2Global_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_Hlt2Global_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_Hlt2Global_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_Hlt2Global_TPS", classifiedDec.tps());

      // Hlt2Phys
      triggerTisTosTool->setTriggerInput(m_hlt2Phys);
      //      classifiedDec = triggerTisTosTool->triggerTisTos();
      classifiedDec = triggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_Hlt2Phys_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_Hlt2Global_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_Hlt2Global_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_Hlt2Global_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_Hlt2Global_TPS", classifiedDec.tps());
    }
    /*
    if(m_doStripping)
    {

      //Do the Hlt2
      m_TriggerTisTosTool->setTriggerInput("Stripping.*Decision");
      //Fill the decision, tis and tos parametres for the Hlt2 as a whole
      classifiedDec = m_TriggerTisTosTool->triggerTisTos();
      tuple->column( prefix+"_StrippingGlobal_Dec", classifiedDec.decision());
      tuple->column( prefix+"_StrippingGlobal_TIS", classifiedDec.tis());
      tuple->column( prefix+"_StrippingGlobal_TOS", classifiedDec.tos());
    }
    */
  }// if the PID is in the given list

  return StatusCode::SUCCESS;
}


StatusCode TupleToolTISTOS::fillVerbose( const LHCb::Particle* top
                                         , const LHCb::Particle* P
                                         , const std::string& head
                                         , Tuples::Tuple& tuple )
{
  if( m_onlyTop ){
    if( P != top )return StatusCode::SUCCESS;
  }


  //  if(  fabs( P->particleID().pid()) < 500 ) return StatusCode::SUCCESS; /// Uncomment the line for Bs->mumu (signal+control channel) ntuples
  // if(  ! (P->isBasicParticle()) ) return StatusCode::SUCCESS; /// Uncomment the line for MidID ntuples

  if(!checkPID) return StatusCode::SUCCESS;

  const std::string prefix=fullName(head);

  ITriggerTisTos::TisTosTob classifiedDec;
  //bool decision = false;
  //bool tis = 0;
  //bool tos = 0;

  m_TriggerTisTosTool->setOfflineInput(*P);

  if( m_verboseL0 )
  {
    // m_L0TriggerTisTosTool->setOfflineInput(*P);
    m_L0TriggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_l0.begin();s!= m_l0.end();++s){

      const std::string name = getName(*s);
      if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
      //      m_L0TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
      m_L0TriggerTisTosTool->setTriggerInput(*s);
      classifiedDec = m_L0TriggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_"+name+"_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_"+name+"_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_"+name+"_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_"+name+"_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_"+name+"_TPS", classifiedDec.tps());
    }
  }

  // discover if split Hlt1,Hlt2 or old style 
  bool split(false);
  split = exist<LHCb::HltDecReports>("Hlt1/DecReports",false);    
  if( !split ) split= exist<LHCb::HltDecReports>("Hlt1/DecReports");


  if( m_verboseHlt1 )
  {

    ITriggerTisTos * triggerTisTosTool = m_TriggerTisTosTool;
    if( split ){
      triggerTisTosTool = m_Hlt1TriggerTisTosTool;
      triggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
    }

    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_hlt1.begin();s != m_hlt1.end();++s)
    {
      const std::string name = getName(*s);
      if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
      if( s->find("Hlt1L0") == 0 ){
        // assume that we want to look at L0 rather than Hlt1L0 decisions
	//        triggerTisTosTool->selectionTisTos(*s,decision,tis,tos);
	classifiedDec = triggerTisTosTool->tisTosTobSelection(*s);
        bool decision = triggerTisTosTool->hltSelectionObjectSummaries(*s).size()!=0;
	tuple->column( prefix+"_"+name+"_Dec", decision);
      } else {
        //triggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
	triggerTisTosTool->setTriggerInput(*s);
	classifiedDec = triggerTisTosTool->tisTosTobTrigger();
	tuple->column( prefix+"_"+name+"_Dec", classifiedDec.decision());
      }
      if(m_TIS)tuple->column( prefix+"_"+name+"_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_"+name+"_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_"+name+"_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_"+name+"_TPS", classifiedDec.tps());
    }
  }

  if( m_verboseHlt2 )
  {

    ITriggerTisTos * triggerTisTosTool = m_TriggerTisTosTool;
    if( split ){
      triggerTisTosTool = m_Hlt2TriggerTisTosTool;
      triggerTisTosTool->setOfflineInput(m_TriggerTisTosTool->offlineLHCbIDs() );
    }

    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_hlt2.begin();s != m_hlt2.end();++s)
    {
      const std::string name = getName(*s);
      if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
      //triggerTisTosTool->triggerTisTos(*s,decision,tis,tos); 
      triggerTisTosTool->setTriggerInput(*s);
      classifiedDec = triggerTisTosTool->tisTosTobTrigger();
      tuple->column( prefix+"_"+name+"_Dec", classifiedDec.decision());
      if(m_TIS)tuple->column( prefix+"_"+name+"_TIS", classifiedDec.tis());
      if(m_TOS)tuple->column( prefix+"_"+name+"_TOS", classifiedDec.tos());
      if(m_TUS)tuple->column( prefix+"_"+name+"_TUS", classifiedDec.tus());
      if(m_TPS)tuple->column( prefix+"_"+name+"_TPS", classifiedDec.tps());
    }
  }
  /*
  if( m_verboseStripping )
  {
    //Now loop over all the subtriggers
    for( std::vector< std::string >::const_iterator s=m_stripping.begin();s != m_stripping.end();++s)
    {
      std::string name = getName(*s);
      if (msgLevel(MSG::VERBOSE)) verbose() << "Selection " << *s << endmsg  ;
      m_TriggerTisTosTool->triggerTisTos(*s,decision,tis,tos);
      tuple->column( prefix+"_"+name+"_Dec", decision);
      tuple->column( prefix+"_"+name+"_TIS", tis);
      tuple->column( prefix+"_"+name+"_TOS", tos);
    }
  }
  */
  return StatusCode::SUCCESS;
}


std::string TupleToolTISTOS::getName(const std::string& mystr)
{
  std::string name = mystr;
  const int found = name.find(".*");
  if ( found >= 0 ) 
  {
    name.replace( found, 2, "__");
    if (msgLevel(MSG::VERBOSE)) 
      verbose() <<"From the function... Replaced   "<<mystr<<"  with  "<<name<<endmsg;
  }
  return name;
}

StatusCode TupleToolTISTOS::fillBasic( Tuples::Tuple& T )
{
  return TupleToolTriggerBase::fillBasic(T);
}

StatusCode TupleToolTISTOS::fillVerbose( Tuples::Tuple& T )
{
  return TupleToolTriggerBase::fillVerbose(T);
}
