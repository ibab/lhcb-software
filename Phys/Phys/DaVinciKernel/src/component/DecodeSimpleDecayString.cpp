// $Id$
// ============================================================================
// Include files
// ============================================================================
// from ANSI C++
//#include <math.h>
//#include <assert.h>
//#include <numeric>
//#include <memory>
// ============================================================================
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
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/Decay.h"
// ============================================================================
// Local
// ============================================================================
#include "DecodeSimpleDecayString.h"
/** @file
 *  Implementation file for class : DecodeSimpleDecayString
 *  @date 2004-06-30
 *  @author Patrick KOPPENBURG
 */
//-----------------------------------------------------------------------------
typedef std::vector<int>         ints    ;
typedef std::vector<std::string> strings ;


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DecodeSimpleDecayString )

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
}

//=============================================================================
// Destructor
//=============================================================================
DecodeSimpleDecayString::~DecodeSimpleDecayString() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode DecodeSimpleDecayString::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_ppSvc = svc<LHCb::IParticlePropertySvc>
    ( "LHCb::ParticlePropertySvc", true );
  if ( !m_ppSvc )
  {
    fatal() << "ParticlePropertySvc Not Found" << endmsg;
    return StatusCode::FAILURE;
  }
  sc = reset();

  return sc;
}

//=============================================================================
// setter
//=============================================================================
StatusCode DecodeSimpleDecayString::setDescriptor(const std::string& descriptor)
{
  StatusCode sc = reset();
  if ( sc.isFailure() ) return sc ;
  m_descriptor = descriptor;
  if (msgLevel(MSG::DEBUG)) debug() << "Going to strip cc in " << descriptor << endmsg ;
  sc = strip_cc(); // strip cc part -> sets is_cc
  if ( sc.isFailure() ) return sc ;
  if (msgLevel(MSG::DEBUG)) debug() << "cc is " << m_iscc << " -> "
                                    << m_descriptor << endmsg ;
  sc = splitDescriptor(m_descriptor, m_mother, m_daughters); // split
  if ( sc.isFailure() ) return sc ;
  if ( m_daughters.size() < 2 ){
    error() << "Mother: " << m_mother << ", " << m_daughters.size()
            << " daughters: " << m_daughters << endmsg ;
    error() << "Seen DecayDescriptor : " << m_descriptor << endmsg ;
    error() << "There must be at least 2 daughters. Use FilterDesktop to filter single Particles." << endmsg ;
    return StatusCode::FAILURE ;
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Mother: " << m_mother << ", " << m_daughters.size()
                                    << " daughters: " << m_daughters << endmsg ;
  if ( m_iscc ) {
    if (msgLevel(MSG::DEBUG)) debug() << "Doing cc " << descriptor << endmsg ;
    sc = do_cc();
    if ( sc.isFailure() ) return sc ;
  }

  return sc ;
}
//=============================================================================
// Reset everything
//=============================================================================
StatusCode DecodeSimpleDecayString::reset()
{
  m_daughters.clear();
  m_daughters_cc.clear();
  m_iscc = false ;
  if (msgLevel(MSG::VERBOSE)) verbose() << "Reset of DecodeSimpleDecayString done" << endmsg ;
  return StatusCode::SUCCESS;
}
//=============================================================================
// Get strings
//=============================================================================
StatusCode 
DecodeSimpleDecayString::getStrings(std::string& mother,
                                    std::vector<std::string>& daughters) const 
{
  mother    = m_mother ;
  daughters = m_daughters ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode 
DecodeSimpleDecayString::getStrings_cc(std::string& mother,
                                       std::vector<std::string>& daughters) const 
{
  mother    = m_mother_cc ;
  daughters = m_daughters_cc ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Get PIDs
//=============================================================================
StatusCode 
DecodeSimpleDecayString::getPIDs(int& mother,
                                 std::vector<int>& daughters) const
{
  return buildPIDs( m_mother, m_daughters, mother, daughters)  ;
}

//=============================================================================

StatusCode 
DecodeSimpleDecayString::getPIDs_cc(int& mother,
                                    std::vector<int>& daughters) const 
{
  return buildPIDs( m_mother_cc, m_daughters_cc, mother, daughters)  ;
}

//=============================================================================

StatusCode 
DecodeSimpleDecayString::buildPIDs(const std::string& in_m,
                                   const strings& in_d,
                                   int& mother,
                                   std::vector<int>& daughters) const 
{

  StatusCode sc = PID(in_m, mother);
  if ( sc.isFailure() ) return sc ;

  for ( const auto& id : in_d )
  {
    int pid(0) ;
    sc = PID(id, pid);
    if ( sc.isFailure() ) return sc ;
    daughters.push_back( pid );
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================

StatusCode DecodeSimpleDecayString::PID(const std::string& ps, int& pid) const
{
  const LHCb::ParticleProperty* part = m_ppSvc->find( ps );
  if (NULL==part){
    return Error("No particle Property found for "+ps);
  }
  pid = part->particleID().pid() ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Decode descriptor. Code from Gerhard.
//=============================================================================

StatusCode
DecodeSimpleDecayString::splitDescriptor(const std::string& descriptor,
                                         std::string& mother,
                                         strings& daughters)const 
{
  static const boost::regex
    expression(
               "^\\s*" // skip initial whitespace
               "(\\S+)" // mother, i.e. everything upto "->"
               "\\s+->\\s+"  // seperator between mother and daughters
               "(.*)" // daughters
               );
  boost::smatch what;
  const std::string desc = descriptor;
  if (!boost::regex_match(desc.begin(),desc.end(),what,expression))
  {
    fatal() << "not a valid decaydescriptor `"<< desc << "'" << endmsg;
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

StatusCode DecodeSimpleDecayString::do_cc(void) 
{
  if (msgLevel(MSG::DEBUG)) debug() << "Doing cc " << m_descriptor << endmsg ;
  std::string descriptor_cc = conjugate(m_descriptor);
  return splitDescriptor(descriptor_cc, m_mother_cc, m_daughters_cc);
}

//=============================================================================
// Conjugator from Gerhard
//=============================================================================
std::string DecodeSimpleDecayString::conjugate(const std::string& descriptor)
  const
{
  std::string answer;
  std::string mother;
  strings daughters;
  const StatusCode sc = splitDescriptor(descriptor, mother ,daughters);
  if (sc.isFailure()) Exception("Cannot split descriptor "+descriptor);
  const LHCb::ParticleProperty* part = m_ppSvc->find( mother );
  if (NULL==part) Exception("Cannot find ParticleProperty for "+mother);
  const LHCb::ParticleProperty* cc = part->antiParticle();
  if (cc==0) Exception("Particle "+mother+" does not have an antiparticle!");
  answer += cc->particle();
  answer += " -> ";
  for ( const auto& i : daughters )
  {
    part = m_ppSvc->find( i );
    if (NULL==part) Exception("Cannot find ParticleProperty for "+i);
    cc = part->antiParticle();
    if (cc==0) Exception("Particle "+i+" does not have an antiparticle!");
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
  if (boost::regex_match(desc.begin(),desc.end(),what,cc))
  {
    m_descriptor = std::string(what[1].first, what[1].second);
    m_iscc = true;
  } 
  else
  {
    m_iscc = false;
  }

  return StatusCode::SUCCESS;
}
// ============================================================================

// ==========================================================================
/*  get the decay form the descriptor
 *  @param decay (output) the decay
 *  @return status code
 */
// ==========================================================================
StatusCode
DecodeSimpleDecayString::getDecay( Decays::Decay& decay ) const
{
  //
  decay.setMother     ( m_mother    ) ;
  decay.setDaughters  ( m_daughters ) ;
  //
  return decay.validate ( m_ppSvc ) ;
}

// ==========================================================================
/*  get the charge conjugate decay form the descriptor
 *  @param decay (output) the decay
 *  @return status code
 */
// ==========================================================================
StatusCode DecodeSimpleDecayString::getDecay_cc( Decays::Decay& decay ) const
{
  if ( !m_iscc ) { return StatusCode::FAILURE ; }
  //
  decay.setMother     ( m_mother_cc    ) ;
  decay.setDaughters  ( m_daughters_cc ) ;
  //
  return decay.validate ( m_ppSvc ) ;
}
// ==========================================================================
/*  get all decays form the descriptor
 *  @param decays (output) the vector of decays
 *  @return status code
 */
StatusCode 
DecodeSimpleDecayString::getDecays( std::vector<Decays::Decay>& decays ) const
{
  decays.clear() ;
  // the main decay:
  Decays::Decay decay1 ( m_mother , m_daughters ) ;
  StatusCode sc = decay1.validate ( m_ppSvc ) ;
  if ( sc.isFailure() ) { return sc ; }
  decays.push_back ( decay1 ) ;
  if ( m_iscc )
  {
    Decays::Decay decay2 ( m_mother_cc , m_daughters_cc ) ;
    sc = decay2.validate ( m_ppSvc ) ;
    if ( sc.isFailure() ) { return sc ; }
    decays.push_back ( decay2 ) ;
  }
  return StatusCode::SUCCESS ;
}
// ==========================================================================

// ============================================================================
// The END
// ============================================================================
