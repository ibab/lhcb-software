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
#include "TupleToolTriggerBase.h"

//#include <sstream>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTriggerBase
//
// 2008-04-09 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolTriggerBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTriggerBase::TupleToolTriggerBase( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TupleToolBase ( type, name , parent ),
    m_l0(0),
    m_hlt1(0),
    m_hlt2(0),
    m_hlt1_init(0),
    m_hlt2_init(0),
    m_hlt1_all(0),
    m_hlt2_all(0),
    m_triggerList(0)
{
  //declareInterface<IParticleTupleTool>(this);
  
  declareProperty( "UseAutomaticTriggerList",   m_useAutomaticTriggerList=false );
  declareProperty( "CollateTriggerList",   m_collateTriggerList=false );
  //to turn all verbosity on, set Verbose=true
  declareProperty( "VerboseL0",   m_verboseL0=false );
  declareProperty( "VerboseHlt1", m_verboseHlt1=false );
  declareProperty( "VerboseHlt2", m_verboseHlt2=false );
  
  //List of triggers to look at
  declareProperty( "TriggerList",   m_triggerList=std::vector<std::string>(0) );
  

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolTriggerBase::~TupleToolTriggerBase() {} 

//=============================================================================

//=========================================================================
//  initialize
//=========================================================================
StatusCode TupleToolTriggerBase::initialize( ) 
{
  
  StatusCode sc = TupleToolBase::initialize();
  if (!sc) return sc;
  
  if(isVerbose()) m_verboseL0=m_verboseHlt1=m_verboseHlt2=true;
  bool iv=(m_verboseL0||m_verboseHlt1||m_verboseHlt2);
  
  if(m_triggerList.size() != 0 && !iv)
  {
    
    warning() << "You have set a list of triggers to look for, but not asked for verbose mode ... OK, but this is weird! "
              << endmsg;
  }
  
  if(m_triggerList.size() == 0 && !m_useAutomaticTriggerList && iv )
  {
    warning() << "You have not set a list of triggers to look for, so verbose has no meaning. " 
              << "For your convenience I will print out the full list at the end of the events." << endmsg;
    m_verboseL0 = m_verboseHlt1 = m_verboseHlt2 =iv=false;
    m_collateTriggerList=true;
  }
  if(m_triggerList.size() > 0 && m_useAutomaticTriggerList)
  {
    warning() << "You have set UseAutomaticTriggerList to true. " 
              << "You have also given a list to look at, so I will take that instead" << endmsg;
    m_useAutomaticTriggerList=false;
    
  }
  else if(m_useAutomaticTriggerList)
  {
    warning() << "You have set UseAutomaticTriggerList to true. " 
              << "This will not work if the triggers change in the data you are looking at" << endmsg;
  }
  
  //bug, missing this line
  if(m_triggerList.size() != 0) compileMyList(m_triggerList);
  
  if(iv && !m_collateTriggerList) m_collateTriggerList=(m_useAutomaticTriggerList || m_triggerList.size() == 0);
  
  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode TupleToolTriggerBase::finalize( ) 
{
  
  if(m_collateTriggerList)
  {
    std::string printTriggers="";
    printTriggers+="-----------------L0 HLT1 Triggers Seen------------------\n[\n";
    for(std::vector<std::string>::const_iterator s=m_hlt1_all.begin(); s!=m_hlt1_all.end(); s++)
    {
      printTriggers+=*s;
      printTriggers+=",\n";
    }
    printTriggers+="]\n";
    info() << printTriggers << endmsg;
    printTriggers="";
    
    info() << "------------------- HLT2 Triggers Seen------------------\n[\n";
    for(std::vector<std::string>::const_iterator s=m_hlt2_all.begin(); s!=m_hlt2_all.end(); s++)
    {
      printTriggers+=*s;
      printTriggers+=",\n";
    }
    printTriggers+="]\n";
    info() << printTriggers << endmsg;
    printTriggers="";
    
  }
  return TupleToolBase::finalize();
  
}

//=========================================================================
//  Fill
//=========================================================================

StatusCode TupleToolTriggerBase::fill( const LHCb::Particle* M
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple )
{
  if(msgLevel(MSG::DEBUG)) debug() << "fill particle " << P 
                                   << "m_collateTriggerList " << m_collateTriggerList 
                                   << "m_useAutomaticTriggerList" << m_useAutomaticTriggerList
                                   << endmsg;
  
  bool test=true;
  test &= fillBasic(M, P, head, tuple);
  
  //setup and check automatic filling mode
  if(m_useAutomaticTriggerList)  test &= autoListMode();
  
  
  //keep track of a list of all triggers for printing at the end
  if(m_collateTriggerList) if(!checkAutoList()) appendToList();
  
  //Fill details about the requested triggers
  if(m_verboseL0 || m_verboseHlt1 || m_verboseHlt2) test &=fillVerbose(M, P, head, tuple);
  
  return StatusCode(test);
  
}

StatusCode TupleToolTriggerBase::fill(Tuples::Tuple& tuple )
{
  if(msgLevel(MSG::DEBUG)) debug() << "fill tuple "
                                   << "m_collateTriggerList " << m_collateTriggerList 
                                   << " m_useAutomaticTriggerList " << m_useAutomaticTriggerList
                                   << endmsg;
  bool test=true;
  test &= fillBasic(tuple);
  
  //setup and check automatic filling mode
  if(m_useAutomaticTriggerList)  test &= autoListMode();
  
  //keep track of a list of all triggers for printing at the end
  if(m_collateTriggerList) if(!checkAutoList()) appendToList();
  
  //Fill details about the requested triggers
  if(m_verboseL0 || m_verboseHlt1 || m_verboseHlt2) test &=fillVerbose(tuple);
  
  return StatusCode(test);
  
}

StatusCode TupleToolTriggerBase::autoListMode()
{
  
  if( !(m_l0.size()+m_hlt1.size()+m_hlt2.size()) ) compileAutoList();
  else if(!checkAutoList())
  {
    warning() << "The trigger has changed, your automatic list won't work. " 
              << "Please fix the list manually to the triggers you are looking for" 
              << "Set the list property TriggerList" << endmsg;
  }
  
  return StatusCode::SUCCESS;
}


bool TupleToolTriggerBase::compileAutoList()
{
  
  //boost::regex l0("Hlt1L0.*Decision");
  //boost::regex hlt1("Hlt1[^L0]*Decision");//Not to save the L0 stuff twice!
  //boost::regex hlt2("Hlt2.*Decision");
  
  m_hlt1_init = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID");
  m_hlt1_all=m_hlt1_init;
  compileMyList(m_hlt1_init);
  
  m_hlt2_init = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt2SelectionID");
  m_hlt2_all=m_hlt2_init;
  compileMyList(m_hlt2_init);
  
  if( m_verboseL0 && !m_l0.size() )
  {
    Error(" Cannot find any Hlt1L0 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
  }
  if( m_verboseHlt1 && !m_hlt1.size() )
  {
    Error(" Cannot find any Hlt1 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
  }
  if( m_verboseHlt2 && !m_hlt2.size() )
  {
    Error(" Cannot find any Hlt2 trigger names in ANNSvc",StatusCode::SUCCESS,10).ignore();
  }
  
  return true;
  
}

bool TupleToolTriggerBase::compileMyList(const std::vector<std::string>& list)
{
  
  if(msgLevel(MSG::DEBUG)) debug() << "compiling List "
                                   << endmsg;
  
  boost::regex l0("Hlt1L0.*Decision");
  boost::regex hlt1("Hlt1[^L0]*Decision");
  boost::regex hlt2("Hlt2.*Decision");
  
  //m_hlt1_init = svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID");
  for( std::vector< std::string >::const_iterator s=list.begin();s != list.end();++s)
  {
    if( boost::regex_match( *s, l0 ) ) 
    {
      m_l0.push_back(*s);
    }
    if( boost::regex_match( *s, hlt1 ) ) 
    {
      m_hlt1.push_back(*s);
    }
    if( boost::regex_match( *s,  hlt2 ) ) 
    {
      m_hlt2.push_back(*s);
    }
  }
  
  
  if(msgLevel(MSG::DEBUG))
  { 
     debug() << " ==== L0 ==== " << endmsg;
     for (std::vector<std::string>::const_iterator s=m_l0.begin();s != m_l0.end();++s) debug() << " " << (*s);
     debug() <<endmsg;
     debug() << " ==== HLT1 ==== " << endmsg;
     for (std::vector<std::string>::const_iterator s=m_hlt1.begin();s != m_hlt1.end();++s) debug() << " " << (*s);
     debug() <<endmsg;
     debug() << " ==== HLT2 ==== " << endmsg;
     for (std::vector<std::string>::const_iterator s=m_hlt2.begin();s != m_hlt2.end();++s) debug() << " " << (*s);
     debug() <<endmsg;
     debug() << " ==== Compiled list ====" << endmsg;
  }
  
  return true;
  
}

bool TupleToolTriggerBase::checkAutoList()
{
  
  if(m_hlt1_init != svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID")) return false;
  if(m_hlt2_init != svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt2SelectionID"))return false;
  return true;
  
}

bool TupleToolTriggerBase::appendToList()
{
  std::vector<std::string> hlt1=svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt1SelectionID");
  std::vector<std::string> hlt2=svc<IANNSvc>("ANNDispatchSvc")->keys("Hlt2SelectionID");
  
  //in case it is the first time
  if(m_hlt1_all.size()==0 && m_hlt2_all.size()==0)
  {
    m_hlt1_all=hlt1;
    m_hlt2_all=hlt2;
    if(m_hlt1_init.size()!=0 || m_hlt2_init.size()!=0) return true;
    
    m_hlt1_init=hlt1;
    m_hlt2_init=hlt2;
    return true;
  }
  
  //otherwise compile the unique list
  for(std::vector<std::string>::const_iterator s=hlt1.begin(); s!=hlt1.end(); s++) m_hlt1_all.push_back(*s);
  for(std::vector<std::string>::const_iterator s=hlt2.begin(); s!=hlt2.end(); s++) m_hlt2_all.push_back(*s);
  std::sort(m_hlt1_all.begin(), m_hlt1_all.end());
  std::sort(m_hlt2_all.begin(), m_hlt2_all.end());
  std::unique(m_hlt1_all.begin(), m_hlt1_all.end());
  std::unique(m_hlt2_all.begin(), m_hlt2_all.end());
  return true;
  
  
}

