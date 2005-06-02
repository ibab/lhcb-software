// $Id: CheckVeloOverlap.cpp,v 1.1 2005-06-02 16:39:47 pkoppenb Exp $

// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/TrStoredTrack.h"
#include "Event/TrMeasurement.h"
#include "Event/VeloClusterOnStoredTrack.h"

// local
#include "CheckVeloOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckVeloOverlap: CheckVeloOverlap.cpp
//
// 28/06/2002
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CheckVeloOverlap>          s_factory ;
const        IToolFactory& CheckVeloOverlapFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckVeloOverlap::CheckVeloOverlap( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool( type, name , parent ) {
  // Declaring implemented interfaces
  declareInterface<ICheckOverlap>(this);
  
  declareProperty("MaxCommonClusters", m_maxClusters = 0 );
  declareProperty("MaxCommonClusterFraction", m_maxClusterFraction = 0 );
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
bool CheckVeloOverlap::foundOverlap( ConstParticleVector & parts, 
                                 std::vector<const ContainedObject* > & track ) {
  verbose() << "foundOverlap(parts, tracks) " << parts.size() << " " << track.size() << endmsg ;
  StatusCode sc = addOrigins( parts, track );
  if (!sc){
    Exception("Unable to get origin vector of particle vector");
  }
  verbose() << "searching overlap" << endmsg ;
  return searchOverlap( track );
}
//===========================================================================
// Check duplicate entries
//===========================================================================
bool CheckVeloOverlap::searchOverlap( std::vector<const ContainedObject* > & track ) {
  
  verbose() << "searchOverlap(tracks)" << endmsg ;
  // It its a simple particle made from track. Check.
  for (std::vector<const ContainedObject* >::const_iterator i = track.begin(); 
       i != track.end() ; ++i){
    verbose() << "In single loop" << endmsg ;
    for (std::vector<const ContainedObject* >::const_iterator j = i ; 
         j != track.end(); ++j){
      if (j==i) continue ;
      verbose() << "In double loop" << endmsg ;
      if (shareVeloClusters(*i,*j)) return true;
    } 
  }
  return false;
}
//===========================================================================
// Check for shared velo hits
//===========================================================================
bool CheckVeloOverlap::shareVeloClusters( const ContainedObject* c1, 
                                          const ContainedObject* c2) {
  
  verbose() << "shareVeloClusters" << endmsg ;
	const TrStoredTrack* tr1 = dynamic_cast<const TrStoredTrack*>(c1); 
  if ( 0==tr1 ) Error("Cannot cast to TrStoredTrack! (1)") ;
	const TrStoredTrack* tr2 = dynamic_cast<const TrStoredTrack*>(c2); 
  if ( 0==tr2 ) Error("Cannot cast to TrStoredTrack! (2)") ; ;

  if ( tr1 == tr2 ) return true ; // same track!

  long Nvelos1=0;
  long Nvelos2=0;
  long veloclustercomun=0;
  const VeloCluster* veloClu1;
  const VeloCluster* veloClu2;
	  
  const SmartRefVector <TrStoredMeasurement>& meas1 = tr1->measurements();
  const SmartRefVector <TrStoredMeasurement>& meas2 = tr2->measurements();
  
  if (meas1.empty()) return false; // OK
  if (meas2.empty()) return false; // OK    
  SmartRefVector <TrStoredMeasurement>::const_iterator  im1;
  SmartRefVector <TrStoredMeasurement>::const_iterator  im2;
  
  for( im1 = meas1.begin(); im1 != meas1.end(); ++im1){
    veloClu1 = 0;
    const TrStoredMeasurement* tStoMeas = *im1;
	      
    if (const VeloClusterOnStoredTrack* tStoVeloClu =
        dynamic_cast<const VeloClusterOnStoredTrack*>(tStoMeas)) {
      Nvelos1++;
      veloClu1 = const_cast<VeloCluster*>(tStoVeloClu->cluster());
      if (!veloClu1) {
        Error("Failed retrieving VeloCluster ");
        return false ;
      }
    }
	      
    for( im2 = meas2.begin(); im2 != meas2.end(); ++im2){
      veloClu2 = 0;
      const TrStoredMeasurement* tStoMeas = *im2;
      if (const VeloClusterOnStoredTrack* tStoVeloClu =
          dynamic_cast<const VeloClusterOnStoredTrack*>(tStoMeas)) {
        veloClu2 = const_cast<VeloCluster*>(tStoVeloClu->cluster());
        if (!veloClu2) {
          Error("Failed retrieving VeloCluster ");
          return false ;
        }
        if(veloClu1==veloClu2) veloclustercomun++;
      }		
    }//im2
  } //im1
	    
  for( im2 = meas2.begin(); im2 != meas2.end(); ++im2){
	      
    const TrStoredMeasurement* tStoMeas = *im2;
    const VeloClusterOnStoredTrack* tStoVeloClu =
      dynamic_cast<const VeloClusterOnStoredTrack*>(tStoMeas);
    if (0!=tStoVeloClu) Nvelos2++;
	      
  }//im2
  debug() << "VELO clusters: " << Nvelos1 << ", " << Nvelos2 << ". In common: " 
          <<  veloclustercomun << endmsg ;
  
  double commfrac = 2.*veloclustercomun/(Nvelos1+Nvelos2);
  return (( veloclustercomun > m_maxClusters ) || (commfrac > m_maxClusterFraction));
}
//===========================================================================
// Other interfaces to the same thing
//===========================================================================
bool CheckVeloOverlap::foundOverlap( ParticleVector & parts ){
  
  verbose() << "foundOverlap(ParticleVector)" << endmsg ;
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}

//===========================================================================
bool CheckVeloOverlap::foundOverlap( const ParticleVector & parts ){  
  verbose() << "foundOverlap(const ParticleVector)" << endmsg ;
  ConstParticleVector cparts;
  for ( ParticleVector::const_iterator i = parts.begin() ; i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( ConstParticleVector & parts ){  
  verbose() << "foundOverlap(ConstParticleVector)" << endmsg ;
  std::vector<const ContainedObject* > m_track(0);
  return foundOverlap( parts, m_track );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const Particle* particle1 ){ 
  verbose() << "foundOverlap(1)" << endmsg ;
  if (!particle1) Exception("Null pointer");
  ConstParticleVector parts(1, particle1 );
  std::vector<const ContainedObject* > m_track(0);
  return foundOverlap( parts, m_track );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2 ){  
  verbose() << "foundOverlap(2)" << endmsg ;
  if (!particle1 || !particle2) Exception("Null pointer");
  ConstParticleVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  std::vector<const ContainedObject* > m_track(0);
  return foundOverlap( parts, m_track );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const Particle* particle1, 
                                 const Particle* particle2, 
                                 const Particle* particle3){
  verbose() << "foundOverlap(3)" << endmsg ;
  if (!particle1 || !particle2 || !particle3) Exception("Null pointer");
  ConstParticleVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  std::vector<const ContainedObject* > m_track(0);
  return foundOverlap( parts, m_track );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const Particle* particle1, 
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
  std::vector<const ContainedObject* > m_track(0);
  return foundOverlap( parts, m_track );
}
//===========================================================================
// Check for duplicate use of a protoparticle to produce decay tree of
// any particle in vector. Removes found particles from vector.
//===========================================================================
StatusCode CheckVeloOverlap::removeOverlap( ParticleVector& PV){
  verbose() << "removeOverlap( ParticleVector)" << endmsg ;
  ParticleVector Out ;
  for ( ParticleVector::const_iterator i = PV.begin() ; i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
} 
//===========================================================================
StatusCode CheckVeloOverlap::removeOverlap( ConstParticleVector& PV){
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
StatusCode  CheckVeloOverlap::addOrigins( ConstParticleVector& parts, 
                                      std::vector<const ContainedObject* >& tracks){
  verbose() << "addOrigins() " << parts.size() << endmsg ;
  for ( ConstParticleVector::iterator c = parts.begin() ; c !=  parts.end() ; ++c ){
    
    verbose() << "Particle loop " << endmsg ;  
    verbose() << "Particle loop " << (*c)->particleID() << endmsg ;  
    verbose() << "Particle " << (*c)->particleID().pid() << endmsg ;
    if ( (*c)->origin() ){
      verbose() << "has an origin " << (*c)->origin() << endmsg ;
      const ProtoParticle* p1 = dynamic_cast<const ProtoParticle*>((*c)->origin());
      if ( 0==p1 ) continue;
      const TrStoredTrack* tr1 = p1->track();
      if ( 0==tr1 ) continue ;      
      tracks.push_back(tr1);
    } else if ( (*c)->endVertex() ){
      verbose() << "has a vertex" << (*c)->endVertex() << endmsg ;
      verbose() << "has a daughters " << (*c)->endVertex()->products().size() << endmsg ;
      ConstParticleVector dau = toStdVector( (*c)->endVertex()->products() ) ;
      StatusCode sc = addOrigins(dau, tracks);
      if (!sc) return sc ;
    } else {
      err() << "Particle " << (*c)->particleID().pid() << " has no origin nor endVertex" << endmsg ;
      return StatusCode::FAILURE ;
    } 
  }
  verbose() << "addOrigins() left " << tracks.size() << endmsg ;
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
