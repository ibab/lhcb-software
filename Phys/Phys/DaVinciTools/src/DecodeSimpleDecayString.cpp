// $Id: DecodeSimpleDecayString.cpp,v 1.2 2004-08-11 09:04:25 graven Exp $
// Include files 

// from ANSI C++
//#include <math.h>
//#include <assert.h>
//#include <numeric>
//#include <memory>
// Horrible hacks to solve a Windows compilation problem
#define MSG __MSG
#include "boost/regex.hpp"  // the needed file that causes all the trouble
#undef MSG
#ifdef ERROR
  #undef ERROR
#endif
#undef interface
#undef pascal
#define IID InterfaceID__
// End of horrible hacks

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "DecodeSimpleDecayString.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DecodeSimpleDecayString
//
// 2004-06-30 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<DecodeSimpleDecayString>          s_factory ;
const        IToolFactory& DecodeSimpleDecayStringFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeSimpleDecayString::DecodeSimpleDecayString( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_descriptor(),
    m_mother(),    
    m_daughters(),    
    m_mother_cc(),
    m_daughters_cc(),    
    m_ppSvc(0),
    m_iscc(false)
{
  declareInterface<IDecodeSimpleDecayString>(this);
  
  return;
}
//=============================================================================
// Destructor
//=============================================================================
DecodeSimpleDecayString::~DecodeSimpleDecayString() {}; 

//=============================================================================
// Initialize
//=============================================================================
StatusCode DecodeSimpleDecayString::initialize(){

  StatusCode sc = service( "ParticlePropertySvc", m_ppSvc );
  if( sc.isFailure ()) {
    fatal() << "ParticlePropertySvc Not Found" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// setter
//=============================================================================
StatusCode DecodeSimpleDecayString::setDescriptor(const std::string& descriptor)
{
  MsgStream msg( msgSvc(), name() );
  
  m_descriptor = descriptor;
  msg << MSG::DEBUG << "Going to strip cc in " << descriptor << endreq ;  
  StatusCode sc = this->strip_cc(); // strip cc part -> sets is_cc
  if ( sc.isFailure() ) return sc ;
  msg << MSG::DEBUG << "cc is " << m_iscc << " -> " 
      << m_descriptor << endreq ;  
  sc = this->splitDescriptor(m_descriptor, m_mother, m_daughters); // split
  if ( sc.isFailure() ) return sc ;
  msg << MSG::DEBUG << "Mother: " << m_mother << ", " << m_daughters.size()
      << " daughters: " << m_daughters << endreq ;  
  if ( m_iscc ) {
    sc = this->do_cc();
    if ( sc.isFailure() ) return sc ;
  }
  
  return sc ;
}
//=============================================================================
// Get strings
//=============================================================================
StatusCode DecodeSimpleDecayString::getStrings(std::string& mother,
                                               strings& daughters) const {
  mother = m_mother ;
  daughters = m_daughters ;

  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode DecodeSimpleDecayString::getStrings_cc(std::string& mother,
                                               strings& daughters) const {

  mother = m_mother_cc ;
  daughters = m_daughters_cc ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Get PIDs
//=============================================================================
StatusCode DecodeSimpleDecayString::getPIDs(int& mother,
                                            ints& daughters) const {
  return this->buildPIDs( m_mother, m_daughters, mother, daughters)  ;
}
//=============================================================================
StatusCode DecodeSimpleDecayString::getPIDs_cc(int& mother,
                                            ints& daughters) const {
  return this->buildPIDs( m_mother_cc, m_daughters_cc, mother, daughters)  ;
}
//=============================================================================
StatusCode DecodeSimpleDecayString::buildPIDs(const std::string in_m,
                                              const strings in_d,
                                              int& mother,
                                              ints& daughters) const {
  MsgStream msg( msgSvc(), name() );
  
  StatusCode sc = this->PID(in_m, mother);
  if ( ! sc.isSuccess() ) return sc ;
  strings::const_iterator id;
  for ( id = in_d.begin(); id != in_d.end(); id++ ) {    
    int pid ;
    sc = this->PID(*id, pid);
    if ( ! sc.isSuccess() ) return sc ;
    daughters.push_back( pid);
  }

  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode DecodeSimpleDecayString::PID(const std::string& ps, int& pid) const
{
  ParticleProperty* part = m_ppSvc->find( ps );
  if (!part)  return StatusCode::FAILURE ;
  pid = part->jetsetID() ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Decode descriptor. Code from Gerhard.
//=============================================================================
StatusCode 
DecodeSimpleDecayString::splitDescriptor(const std::string& descriptor, 
                                         std::string& mother,
                                         strings& daughters)const {
  static const boost::regex 
    expression(
               "^\\s*" // skip initial whitespace
               "(\\S+)" // mother, i.e. everything upto "->"
               "\\s+->\\s+"  // seperator between mother and daughters
               "(.*)" // daughters
               );
  boost::smatch what;
  const std::string desc = descriptor;
  if (!boost::regex_match(desc.begin(),desc.end(),what,expression)) {
    fatal() << "not a valid decaydescriptor "<< desc << endmsg;
    return StatusCode::FAILURE;
  }
  mother = std::string(what[1].first, what[1].second);
  std::string d(what[2].first,what[2].second);
  boost::regex_split(back_inserter(daughters),d);
  return StatusCode::SUCCESS;
}
//=============================================================================
// Conjugator
//=============================================================================
StatusCode DecodeSimpleDecayString::do_cc(void) {
  
  std::string descriptor_cc = this->conjugate(m_descriptor);
  this->splitDescriptor(descriptor_cc, m_mother_cc, m_daughters_cc);
  return StatusCode::SUCCESS;  
}
//=============================================================================
// Conjugator from Gerhard
//=============================================================================
std::string DecodeSimpleDecayString::conjugate(const std::string& descriptor) 
const{
  std::string answer; 
  std::string mother;
  strings daughters;
  StatusCode sc = splitDescriptor(descriptor,mother ,daughters);
  if (sc.isFailure()) return std::string();
  ParticleProperty* part = m_ppSvc->find( mother );
  ParticleProperty* cc   = m_ppSvc->findByStdHepID( -part->jetsetID() );
  if (cc==0) cc = part;
  answer += cc->particle();
  answer += " -> ";
  for (strings::iterator i = daughters.begin(); i != daughters.end(); ++i) {
    ParticleProperty* part = m_ppSvc->find( *i );
    ParticleProperty* cc   = m_ppSvc->findByStdHepID( -part->jetsetID() );
    if (cc==0) cc = part;
    answer += " " ;
    answer += cc->particle();
  }
  return answer;
}
//=============================================================================
// Needs to be conjugated?
//=============================================================================
bool DecodeSimpleDecayString::is_cc(void) const
{
  return m_iscc;
}
//=============================================================================
// Needs to be conjugated?
//=============================================================================
StatusCode DecodeSimpleDecayString::strip_cc(void)
{
  static const boost::regex cc( "^\\s*"  // leading whitespace
                                "\\["    // starts with [
                                "([^\\]]+)"  // this is what we're after
                                "\\]cc" // ends with ]cc
                                "\\s*"  // trailing whitespace
                                );
  boost::smatch what;
  const std::string desc = m_descriptor;
  if (boost::regex_match(desc.begin(),desc.end(),what,cc)){
    m_descriptor = std::string(what[1].first, what[1].second);
    m_iscc = true;
  } else m_iscc = false;

  return StatusCode::SUCCESS;  
}
