// $Id: CheckOverlap.cpp,v 1.6 2005-02-24 12:49:17 pkoppenb Exp $

// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "CheckOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckOverlap: CheckOverlap.cpp
//
// 28/06/2002
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CheckOverlap>          s_factory ;
const        IToolFactory& CheckOverlapFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckOverlap::CheckOverlap( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool( type, name , parent ) {
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
bool CheckOverlap::foundOverlap( ConstParticleVector & parts, 
                                 std::vector<const ContainedObject* > & proto ) {
  verbose() << "foundOverlap(parts, protos) " << parts.size() << " " << proto.size() << endmsg ;
  StatusCode sc = addOrigins( parts, proto );
  if (!sc){
    Exception("Unable to get origin vector of particle vector");
  }
  verbose() << "searching overlap" << endmsg ;
  return searchOverlap( proto );
}
//===========================================================================
// Check duplicate entries
//===========================================================================
bool CheckOverlap::searchOverlap( std::vector<const ContainedObject* > & proto ) {
  
  verbose() << "searchOverlap(protos)" << endmsg ;
  // It its a simple particle made from protoparticle. Check.
  for (std::vector<const ContainedObject* >::const_iterator i = proto.begin(); 
       i != proto.end() ; ++i){
    verbose() << "In single loop" << endmsg ;
    for (std::vector<const ContainedObject* >::const_iterator j = i ; 
         j != proto.end(); ++j){
      if (j==i) continue ;
      verbose() << "In double loop" << endmsg ;
      if ( *i==*j ) return true ;
    } 
  }
  return false;
}
//===========================================================================
// Other interfaces to the same thing
//===========================================================================
bool CheckOverlap::foundOverlap( ParticleVector & parts ){
  
  verbose() << "foundOverlap(ParticleVector)" << endmsg ;
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}

//===========================================================================
bool CheckOverlap::foundOverlap( const ParticleVector & parts ){  
  verbose() << "foundOverlap(const ParticleVector)" << endmsg ;
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}
//===========================================================================
bool CheckOverlap::foundOverlap( ConstParticleVector & parts ){  
  verbose() << "foundOverlap(ConstParticleVector)" << endmsg ;
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1 ){ 
  verbose() << "foundOverlap(1)" << endmsg ;
  if (!particle1) Exception("Null pointer");
  ConstParticleVector parts(1, particle1 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2 ){  
  verbose() << "foundOverlap(2)" << endmsg ;
  if (!particle1 || !particle2) Exception("Null pointer");
  ConstParticleVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3){
  verbose() << "foundOverlap(3)" << endmsg ;
  if (!particle1 || !particle2 || !particle3) Exception("Null pointer");
  ConstParticleVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3, 
                                 const Particle* particle4){
  verbose() << "foundOverlap(4)" << endmsg ;
  if (!particle1 || !particle2 || !particle3|| !particle4 ) Exception("Null pointer");
  ConstParticleVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  parts.push_back( particle4 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
// Check for duplicate use of a protoparticle to produce decay tree of
// any particle in vector. Removes found particles from vector.
//===========================================================================
StatusCode CheckOverlap::removeOverlap( ParticleVector& PV){
  verbose() << "removeOverlap( ParticleVector)" << endmsg ;
  ParticleVector Out ;
  for ( ParticleVector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
} 
//===========================================================================
StatusCode CheckOverlap::removeOverlap( ConstParticleVector& PV){
  verbose() << "removeOverlap(ConstParticleVector)" << endmsg ;
  ConstParticleVector Out ;
  for ( ConstParticleVector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Replace resonance by daughters in vector, helper to the Tree methods
//=============================================================================
StatusCode  CheckOverlap::addOrigins( ConstParticleVector& parts, 
                                      std::vector<const ContainedObject* > protos){
  verbose() << "addOrigins() " << parts.size() << endmsg ;
  for ( ConstParticleVector::iterator c = parts.begin() ; c !=  parts.end() ; ++c ){
    
    verbose() << "Particle loop " << endmsg ;  
    verbose() << "Particle loop " << (*c)->particleID() << endmsg ;  
    verbose() << "Particle " << (*c)->particleID().pid() << endmsg ;
    if ( (*c)->origin() ){
      verbose() << "has an origin " << (*c)->origin() << endmsg ;
      protos.push_back((*c)->origin());
    } else if ( (*c)->endVertex() ){
      verbose() << "has a vertex" << (*c)->endVertex() << endmsg ;
      verbose() << "has a daughters " << (*c)->endVertex()->products().size() << endmsg ;
      ConstParticleVector dau = toStdVector( (*c)->endVertex()->products() ) ;
      StatusCode sc = addOrigins(dau, protos);
      if (!sc) return sc ;
    } else {
      err() << "Particle " << (*c)->particleID().pid() << " has no origin nor endVertex" << endmsg ;
      return StatusCode::FAILURE ;
    } 
  }
  verbose() << "addOrigins() left " << protos.size() << endmsg ;
  return StatusCode::SUCCESS ;
}
//===========================================================================
// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
//===========================================================================
template <class T> 
std::vector<const T*> toStdVector( const SmartRefVector<T>& refvector ) {
  std::vector<const T*> tvector;
  for( typename SmartRefVector<T>::const_iterator ip = refvector.begin();ip !=
         refvector.end(); ++ip ) {
    tvector.push_back( *ip );
  }
  return tvector;
}
