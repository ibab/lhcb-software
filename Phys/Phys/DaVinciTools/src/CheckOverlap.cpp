// $Id: CheckOverlap.cpp,v 1.5 2005-02-24 09:01:35 pkoppenb Exp $

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
  
  StatusCode sc = addOrigins( parts, proto );
  if (!sc){
    Exception("Unable to get origin vector of particle vector");
  }
  return foundOverlap( proto );
}
//===========================================================================
// Check duplicate entries
//===========================================================================
bool CheckOverlap::foundOverlap( std::vector<const ContainedObject* > & proto ) {
  
  // It its a simple particle made from protoparticle. Check.
  for (std::vector<const ContainedObject* >::const_iterator i = proto.begin(); 
       i != proto.end()-1 ; ++i){
    for (std::vector<const ContainedObject* >::const_iterator j = i+1 ; 
         j != proto.end(); ++j){
      if ( i==j ) return true ;
    } 
  }
  return false;
}
//===========================================================================
// Other interfaces to the same thing
//===========================================================================
bool CheckOverlap::foundOverlap( ParticleVector & parts ){
  
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}

//===========================================================================
bool CheckOverlap::foundOverlap( const ParticleVector & parts ){
  
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}
//===========================================================================
bool CheckOverlap::foundOverlap( ConstParticleVector & parts ){  
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1 ){ 
  ConstParticleVector parts(1, particle1 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2 ){  
  ConstParticleVector parts(2);
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}
//===========================================================================
bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3){
  ConstParticleVector parts(3);
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
  ConstParticleVector parts(4);
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
  ParticleVector Out ;
  for ( ParticleVector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
} 
//===========================================================================
StatusCode CheckOverlap::removeOverlap( ConstParticleVector& PV){
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
  for ( ConstParticleVector::iterator c = parts.begin() ; c !=  parts.end() ; ++c ){
    if ( (*c)->origin() ){
      protos.push_back((*c)->origin());
    } else if ( (*c)->endVertex() ){
      ConstParticleVector dau = toStdVector( (*c)->endVertex()->products() ) ;
      StatusCode sc = addOrigins(dau, protos);
      if (!sc) return sc ;
    } else {
      err() << "Particle " << (*c)->particleID().pid() << " has no origin nor endVertex" << endmsg ;
      return StatusCode::FAILURE ;
    } 
  }
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
