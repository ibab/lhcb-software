// $Id$

// Include files

// from LHCb
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"

// local
#include "CheckOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckOverlap: CheckOverlap.cpp
//
// 28/06/2002
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CheckOverlap )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckOverlap::CheckOverlap( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool( type, name , parent )
  , m_ppSvc(0)
{
  // Declaring implemented interfaces
  declareInterface<ICheckOverlap>(this);
}

//===========================================================================
/// Check for duplicate use of a protoparticle to produce particles.
/// Continue a previous check using the contents of the vector of pointers
/// to protoparticles.(Most intended for internal use by the recursive calls).
/// Arguments: parts is a vector of pointer to particles.
///            proto is a vector of pointers to protoparticles.
//  Real checking method. Scans the tree headed by parts. Add each
//  protoparticle to proto if it is a new one. Return true otherwise.
//  If called directly by the user, it will continue a previous check,
//  not start a new one!
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle::ConstVector & parts,
                                 std::vector<const LHCb::ProtoParticle* > & proto )
{
  if (msgLevel(MSG::VERBOSE)) 
    verbose() << "foundOverlap(parts, protos) " << parts.size() << " "
              << proto.size() << endmsg ;
  const bool sc = addOrigins( parts, proto );
  if (!sc)
  {
    Exception("Unable to get origin vector of particle vector");
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "searching overlap" << endmsg ;
  return searchOverlap( proto );
}

//===========================================================================
// Check duplicate entries
//===========================================================================
bool CheckOverlap::searchOverlap( std::vector<const LHCb::ProtoParticle* > & proto )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap(protos)" << endmsg ;
  // It its a simple particle made from protoparticle. Check.

  for (std::vector<const LHCb::ProtoParticle* >::const_iterator i = proto.begin();
       i != proto.end() ; ++i)
  {
    for (std::vector<const LHCb::ProtoParticle* >::const_iterator j = i ;
         j != proto.end(); ++j)
    {
      if (j==i) continue ;
      if ( *i==*j )
      {
        if (msgLevel(MSG::VERBOSE)) verbose() << "Found overlap " << *i << endmsg ;
        return true ;
      }
    }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Found no overlap" << endmsg ;
  return false;
}
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle::ConstVector & parts )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(LHCb::Particle::ConstVector)" << endmsg ;
  static std::vector<const LHCb::ProtoParticle* > proto(0);
  proto.clear();
  return foundOverlap( parts, proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle* particle1 )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(1)" << endmsg ;
  if (!particle1) Exception("Null pointer");
  const LHCb::Particle::ConstVector parts(1, particle1 );
  static std::vector<const LHCb::ProtoParticle* > proto(0);
  proto.clear();
  return foundOverlap( parts, proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle* particle1,
                                 const LHCb::Particle* particle2 )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(2)" << endmsg ;
  if (!particle1 || !particle2) Exception("Null pointer");
  static LHCb::Particle::ConstVector parts(2) ;
  parts[0] = particle1;
  parts[1] = particle2;
  static std::vector<const LHCb::ProtoParticle* > proto(0);
  proto.clear();
  return foundOverlap( parts, proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle* particle1,
                                 const LHCb::Particle* particle2,
                                 const LHCb::Particle* particle3 )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(3)" << endmsg ;
  if (!particle1 || !particle2 || !particle3) Exception("Null pointer");
  static LHCb::Particle::ConstVector parts(3) ;
  parts[0] = particle1 ;
  parts[1] = particle2 ;
  parts[2] = particle3 ;
  static std::vector<const LHCb::ProtoParticle* > proto(0);
  proto.clear();
  return foundOverlap( parts, proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const LHCb::Particle* particle1,
                                 const LHCb::Particle* particle2,
                                 const LHCb::Particle* particle3,
                                 const LHCb::Particle* particle4 )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(4)" << endmsg ;
  if (!particle1 || !particle2 || !particle3|| !particle4 ) Exception("Null pointer");
  static LHCb::Particle::ConstVector parts(4) ;
  parts[0] = particle1 ;
  parts[1] = particle2 ;
  parts[2] = particle3 ;
  parts[3] = particle4 ;
  static std::vector<const LHCb::ProtoParticle* > proto(0);
  proto.clear();
  return foundOverlap( parts, proto );
}
//===========================================================================
// Check for duplicate use of a protoparticle to produce decay tree of
// any particle in vector. Removes found particles from vector.
//===========================================================================
StatusCode CheckOverlap::removeOverlap( LHCb::Particle::Vector& PV )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap( LHCb::Particle::Vector)" << endmsg ;
  LHCb::Particle::Vector Out;
  Out.reserve(PV.size());
  for ( LHCb::Particle::Vector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i )
  {
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
}
//===========================================================================
StatusCode CheckOverlap::removeOverlap( LHCb::Particle::ConstVector& PV )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap(LHCb::Particle::ConstVector)" << endmsg ;
  LHCb::Particle::ConstVector Out ;
  Out.reserve(PV.size());
  for ( LHCb::Particle::ConstVector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i )
  {
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Replace resonance by daughters in vector, helper to the Tree methods
//=============================================================================
bool CheckOverlap::addOrigins( const LHCb::Particle::ConstVector& parts,
                               std::vector<const LHCb::ProtoParticle* >& protos )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() " << parts.size() << endmsg ;

  for ( LHCb::Particle::ConstVector::const_iterator c = parts.begin() ; c !=  parts.end() ; ++c )
  {

    if (msgLevel(MSG::VERBOSE))
      verbose() << "Particle " << (*c)->particleID().pid()
                << " " << (*c)->momentum() << endmsg ;

    if ( (*c)->proto() )
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "has an origin " << (*c)->proto() << endmsg ;
      protos.push_back((*c)->proto());
    }
    else if ( !((*c)->daughters().empty()) )
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "has a daughters " << (*c)->daughters().size() << endmsg ;
      const LHCb::Particle::ConstVector& dau = (*c)->daughtersVector();
      const bool sc = addOrigins(dau,protos);
      if (!sc) return sc ;
    }
    else
    {
      if ( 0==m_ppSvc ) m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
      const LHCb::ParticleProperty *pp = m_ppSvc->find((*c)->particleID());
      if (0!=pp) { Warning(pp->particle()+" has no proto nor endVertex. Assuming it's from MC.",10,
                           StatusCode::SUCCESS).ignore() ; }
      else { err() << "Particle with unknown PID " << (*c)->particleID().pid() << " has no endVertex. "
                   <<  "Assuming it's from MC" << endmsg ; }
    }
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() left " << protos.size() << endmsg ;
  return true ;
}
