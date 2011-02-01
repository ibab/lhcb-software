// Include files 
#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Particle.h"     
// kernel
#include "Kernel/IANNSvc.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

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
    m_triggerList(0)
{
  //declareInterface<IParticleTupleTool>(this);
  
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
  
  if(m_triggerList.size() == 0 && iv )
  {
    warning() << "You have not set a list of triggers to look for, but have asked for verbose output. " 
              << endmsg;
    m_verboseL0 = m_verboseHlt1 = m_verboseHlt2 =false;
  }

  //bug, missing this line
  if(m_triggerList.size() != 0) compileMyList(m_triggerList);
  
  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode TupleToolTriggerBase::finalize( ) 
{
  
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
                                   << endmsg;
  
  bool test=true;
  test &= fillBasic(M, P, head, tuple);
  
  
  
  //Fill details about the requested triggers
  if(m_verboseL0 || m_verboseHlt1 || m_verboseHlt2 || isVerbose()) 
    test &=fillVerbose(M, P, head, tuple);
  
  return StatusCode(test);
  
}

StatusCode TupleToolTriggerBase::fill(Tuples::Tuple& tuple )
{
  if(msgLevel(MSG::DEBUG)) debug() << "fill tuple "
                                   << endmsg;
  bool test=true;
  test &= fillBasic(tuple);
  
  //Fill details about the requested triggers
  if(m_verboseL0 || m_verboseHlt1 || m_verboseHlt2 || isVerbose()) test &=fillVerbose(tuple);
  
  return StatusCode(test);
  
}


bool TupleToolTriggerBase::compileMyList(const std::vector<std::string>& list)
{
  
  if(msgLevel(MSG::DEBUG)) debug() << "compiling List "
                                   << endmsg;
  
  //boost::regex l0("Hlt1L0.*Decision");
  //boost::regex hlt1("Hlt1[^L0].*Decision");
  boost::regex l0("L0.*Decision");
  boost::regex hlt1("Hlt1.*Decision");
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


