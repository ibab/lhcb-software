// $Id: CheckOverlap.cpp,v 1.4 2005-02-21 08:56:42 pkoppenb Exp $

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
  
  ConstParticleVector::const_iterator iPart;
  std::vector<const ContainedObject* >::const_iterator iProto;
  
  for (iPart=parts.begin(); iPart != parts.end(); ++iPart){
    
    verbose() << "Analysing particle " 
        << (*iPart)->particleID().pid() 
        << " with origin " << (*iPart)->origin() << endreq;
    
    if((*iPart)->origin() != 0 ){
      // It its a simple particle made from protoparticle. Check.
      for(iProto=proto.begin(); iProto != proto.end(); iProto++){
        if ( (*iPart)->origin() == (*iProto) ) {
          debug() << "Found duplicate use of "
                  << "protoparticle " << *iProto << endreq;
          
          return true;
        }
      }
      proto.push_back( (*iPart)->origin() );
    } else{
      // It is a composite particle. Analyse its products.
      ConstParticleVector m_parts = toStdVector((*iPart)->endVertex()->products());
      
      if (foundOverlap( m_parts, proto ) ) return true;
    }
    
  }
  return false;
}

//===========================================================================
// 
//===========================================================================

bool CheckOverlap::foundOverlap( ParticleVector & parts ){
  
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }

  return foundOverlap( cparts );
}

bool CheckOverlap::foundOverlap( const ParticleVector & parts ){
  
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }

  return foundOverlap( cparts );

}


bool CheckOverlap::foundOverlap( ConstParticleVector & parts ){
  
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}

bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2 ){
  
  ConstParticleVector parts;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}

bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3){
  
  ConstParticleVector parts;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}

bool CheckOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3, 
                                 const Particle* particle4){
  
  ConstParticleVector parts;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  parts.push_back( particle4 );
  
  std::vector<const ContainedObject* > m_proto(0);
  return foundOverlap( parts, m_proto );
}


// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
template <class T> 
std::vector<const T*> toStdVector( const SmartRefVector<T>& refvector ) {
  std::vector<const T*> tvector;
  for( typename SmartRefVector<T>::const_iterator ip = refvector.begin();ip !=
         refvector.end(); ++ip ) {
    tvector.push_back( *ip );
  }
  return tvector;
}
