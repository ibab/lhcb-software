// $Id$

// Include files

#include "Event/Track.h"
#include "Event/Measurement.h"
// #include "Event/VeloClusterOnStoredTrack.h"

// local
#include "CheckVeloOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckVeloOverlap: CheckVeloOverlap.cpp
//
// 28/06/2002
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CheckVeloOverlap )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CheckVeloOverlap::CheckVeloOverlap( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
    : GaudiTool( type, name , parent ) 
{
  // Declaring implemented interfaces
  declareInterface<ICheckOverlap>(this);
  
  //  declareProperty("MaxCommonClusters", m_maxClusters = 0 );
  declareProperty("MaxCommonClusterFraction", m_maxClusterFraction = 0.75 );
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
bool CheckVeloOverlap::foundOverlap( const LHCb::Particle::ConstVector& parts,
                                     std::vector<const LHCb::ProtoParticle*>& proto ) {
  verbose() << "foundOverlap(parts, tracks) " << parts.size()
            << " " << proto.size() << endmsg ;
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
bool CheckVeloOverlap::searchOverlap( std::vector<const LHCb::ProtoParticle*> & proto ) {

  verbose() << "searchOverlap(protos)" << endmsg ;
  // It its a simple particle made from track. Check.
  for (std::vector<const LHCb::ProtoParticle* >::const_iterator i = proto.begin();
       i != proto.end() ; ++i){
    verbose() << "In single loop" << endmsg ;
    for (std::vector<const LHCb::ProtoParticle* >::const_iterator j = i ;
         j != proto.end(); ++j){
      if (j==i) continue ;
      verbose() << "In double loop" << endmsg ;
      if (shareVeloClusters(*i,*j)) return true;
    }
  }
  return false;
}

//===========================================================================
// Other interfaces to the same thing
//===========================================================================
bool CheckVeloOverlap::foundOverlap(const LHCb::Particle::ConstVector & parts)
{

  verbose() << "foundOverlap(ParticleVector)" << endmsg ;
  LHCb::Particle::ConstVector cparts;
  for ( LHCb::Particle::ConstVector::const_iterator i = parts.begin() ;
        i!=parts.end();++i){
    cparts.push_back( *i );
  }
  return foundOverlap( cparts );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const LHCb::Particle* particle1 ){
  verbose() << "foundOverlap(1)" << endmsg ;
  if (!particle1) Exception("Null pointer");
  const LHCb::Particle::ConstVector parts(1, particle1 );
  std::vector<const LHCb::ProtoParticle* > protos(0);
  return foundOverlap( parts, protos );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const LHCb::Particle* particle1,
                                     const LHCb::Particle* particle2 ){
  verbose() << "foundOverlap(2)" << endmsg ;
  if (!particle1 || !particle2) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  std::vector<const LHCb::ProtoParticle* > protos(0);
  return foundOverlap( parts, protos );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const LHCb::Particle* particle1,
                                     const LHCb::Particle* particle2,
                                     const LHCb::Particle* particle3){
  verbose() << "foundOverlap(3)" << endmsg ;
  if (!particle1 || !particle2 || !particle3) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  std::vector<const LHCb::ProtoParticle* > protos(0);
  return foundOverlap( parts, protos );
}
//===========================================================================
bool CheckVeloOverlap::foundOverlap( const LHCb::Particle* particle1,
                                     const LHCb::Particle* particle2,
                                     const LHCb::Particle* particle3,
                                     const LHCb::Particle* particle4){
  verbose() << "foundOverlap(4)" << endmsg ;
  if (!particle1 || !particle2 || !particle3|| !particle4 ) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  parts.push_back( particle4 );
  std::vector<const LHCb::ProtoParticle* > protos(0);
  return foundOverlap( parts, protos );
}
//===========================================================================
// Check for duplicate use of a protoparticle to produce decay tree of
// any particle in vector. Removes found particles from vector.
//===========================================================================
StatusCode CheckVeloOverlap::removeOverlap( LHCb::Particle::ConstVector& PV){
  verbose() << "removeOverlap( ParticleVector)" << endmsg ;
  LHCb::Particle::ConstVector Out ;
  for ( LHCb::Particle::ConstVector::const_iterator i = PV.begin() ;
        i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
}
//===========================================================================
StatusCode CheckVeloOverlap::removeOverlap( LHCb::Particle::Vector& PV){
  verbose() << "removeOverlap( ParticleVector)" << endmsg ;
  LHCb::Particle::Vector Out ;
  for ( LHCb::Particle::Vector::const_iterator i = PV.begin() ;
        i!=PV.end() ; ++i){
    if (!foundOverlap( *i )) Out.push_back(*i);
  }
  PV = Out ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Replace resonance by daughters in vector, helper to the Tree methods
//=============================================================================
StatusCode  CheckVeloOverlap::addOrigins( const LHCb::Particle::ConstVector& parts,
                                          std::vector<const LHCb::ProtoParticle*>& protos){
  verbose() << "addOrigins() " << parts.size() << endmsg ;
  for ( LHCb::Particle::ConstVector::const_iterator c = parts.begin() ;
        c !=  parts.end() ; ++c ){

    verbose() << "Particle loop " << endmsg ;
    verbose() << "ParticleID " << (*c)->particleID() << endmsg ;
    verbose() << "Particle PID " << (*c)->particleID().pid() << endmsg ;
    if ( (*c)->proto() ){
      const LHCb::ProtoParticle* p1 = (*c)->proto();
      verbose() << "has an origin " << p1 << endmsg ;
      protos.push_back(p1);
    } else if ( (*c)->endVertex() ){

      verbose() << "has a vertex" << (*c)->endVertex() << endmsg ;
      verbose() << "has a daughters "
                << (*c)->endVertex()->outgoingParticles().size() << endmsg ;
      const LHCb::Particle::ConstVector dau =
        toStdVector( (*c)->endVertex()->outgoingParticles() ) ;
      StatusCode sc = addOrigins(dau, protos);
      if (!sc) return sc ;
    } else {
      err() << "Particle " << (*c)->particleID().pid()
            << " has no origin nor endVertex" << endmsg ;
      return StatusCode::FAILURE ;
    }
  }
  verbose() << "addOrigins() left " << protos.size() << endmsg ;
  return StatusCode::SUCCESS ;
}
//===========================================================================
// Check for shared velo hits
//===========================================================================
bool CheckVeloOverlap::shareVeloClusters( const LHCb::ProtoParticle* c1,
                                          const LHCb::ProtoParticle* c2) {


  verbose() << "shareVeloClusters" << endmsg ;
  const LHCb::Track* const tr1 = c1->track();
  if ( 0==tr1 ) Error("First ProtoParticle has no LHCb::Track!") ;
  const LHCb::Track* const tr2 = c2->track();
  if ( 0==tr2 ) Error("Second ProtoParticle has no LHCb::Track!") ; ;

  if ( tr1 == tr2 ) return true ; // same track!

  long nVelos1=0;
  long nVelos2=0;
  long veloclustercomun=0;

  const std::vector<LHCb::LHCbID>& meas1 = tr1->lhcbIDs();
  const std::vector<LHCb::LHCbID>& meas2 = tr2->lhcbIDs();

  if (meas1.empty() || meas2.empty() ) return false; // OK

  std::vector<LHCb::LHCbID>::const_iterator  im1;
  std::vector<LHCb::LHCbID>::const_iterator  im2;

  for( im1 = meas1.begin(); im1 != meas1.end(); ++im1){

    const LHCb::LHCbID lhcbID1 = *im1;

    if (lhcbID1.checkDetectorType(LHCb::LHCbID::Velo)) {
      nVelos1++;
      for( im2 = meas2.begin(); im2 != meas2.end(); ++im2){
        const LHCb::LHCbID lhcbID2 = *im2;
        if ( lhcbID2.checkDetectorType(LHCb::LHCbID::Velo)) {
          if( lhcbID1.channelID()==lhcbID2.channelID() ) veloclustercomun++;
        } // meas2 is from VELO
      } //im2
    } // meas 1 is from VELO


  } //im1

  for( im2 = meas2.begin(); im2 != meas2.end(); ++im2){
    if ( (*im2).checkDetectorType(LHCb::LHCbID::Velo)) nVelos2++;
  } //im2

  debug() << "VELO clusters: " << nVelos1 << ", "
          << nVelos2 << ". In common: "
          <<  veloclustercomun << endmsg ;

  const double commfrac = 2.*veloclustercomun/(nVelos1+nVelos2);
  return commfrac > m_maxClusterFraction;
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
//===========================================================================
