// $Id$

// Include files

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// local
#include "FindCloneTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FindCloneTool: FindCloneTool.cpp
// Author: Robert Lambert
// 05/06/2008
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FindCloneTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FindCloneTool::FindCloneTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool( type, name , parent )
    , m_ppSvc(0)
{
  // Declaring implemented interfaces
  declareInterface<ICheckOverlap>(this);
}

//===========================================================================
/// FindCloneTool::foundOverlap will look for overlaps.
/* Arguments: parts is a vector of pointer to particles.
 *            proto is a vector of vectors of pointers to protoparticles.
 *  Real checking method. Scans the tree headed by parts. Add each
 *  protoparticle to proto.
 */
//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle::ConstVector & parts,
                                  std::vector< std::vector<const LHCb::ProtoParticle*> > & proto,
                                  std::vector<bool> & isclone )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(parts, protos) " << parts.size() << " "
                                        << proto.size() << endmsg ;
  if(parts.size()<2)
  {
    for(unsigned int i=0; i<parts.size();i++) isclone.push_back(false);
    return false; //can't have a clone with only 1 particle
  }


  StatusCode sc = addOrigins( parts, proto );
  if (!sc)
  {
    Exception("Unable to get origin vector of particle vector");
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "searching overlap" << endmsg ;
  return searchOverlap( proto, isclone);
}

bool FindCloneTool::foundOverlap( const LHCb::Particle::Vector & parts,
                                  std::vector< std::vector<const LHCb::ProtoParticle*> > & proto,
                                  std::vector<bool> & isclone )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(parts, protos) " << parts.size() << " "
                                        << proto.size() << endmsg ;
  if(parts.size()<2) return false; //can't have a clone with only 1 particle
  proto.clear(); //want this to be empty before filling it!

  StatusCode sc = addOrigins( parts, proto );
  if (!sc)
  {
    Exception("Unable to get origin vector of particle vector");
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "searching overlap" << endmsg ;
  return searchOverlap( proto, isclone);
}

//===========================================================================
// Check duplicate entries
//===========================================================================
bool FindCloneTool::searchOverlap( std::vector< std::vector<const LHCb::ProtoParticle*> > & proto,
                                   std::vector<bool> & isclone )
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap(protos) starting with a size " << proto.size() << endmsg ;
  // It its a simple particle made from protoparticle. Check.

  isclone.clear(); //I want this to be an empty vector at this stage.


  bool foundsomeclones=false;

  for (std::vector< std::vector<const LHCb::ProtoParticle*> >::const_iterator i = proto.begin();
       i != proto.end() ; ++i) //loop over first particle
  {
    bool i_isclone=false;

    for (std::vector< std::vector<const LHCb::ProtoParticle*> >::const_iterator j = i ;
         j != proto.end() && !i_isclone; ++j) //loop over second particle
    {

      if (j==i) continue ; //don't check the same particle :S
      if (j->size()!=i->size()) continue; //can't be a clone with a different # of protos
      unsigned int duplicates=0;

      for (std::vector<const LHCb::ProtoParticle*>::const_iterator ii = i->begin();
           ii != i->end() ; ++ii) //loop over first particle
      { // check if ii is in j
        bool iscopied=false;

        for (std::vector<const LHCb::ProtoParticle*>::const_iterator jj = j->begin();
             jj != j->end() && !iscopied ; ++jj) //loop over comparing particle
        {
          iscopied=(iscopied || (*ii==*jj) );
        }
        if (iscopied) duplicates++;

      }

      if(duplicates >= i->size()) i_isclone=true; //it's a clone :)
      //if all the same proto particles are used at least once .

    }
    isclone.push_back(i_isclone);
    foundsomeclones=(foundsomeclones || i_isclone);

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap(protos) Found an overlap? " << foundsomeclones
                                        << " finishing with a size " << isclone.size() << endmsg ;
  //the vector of bools is now filled, so return it.
  return foundsomeclones;

}

//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle::ConstVector & parts,
                                  std::vector<const LHCb::ProtoParticle*> & proto  )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(LHCb::Particle::ConstVector) dummy proto has size: "
                                        << proto.size() << endmsg ;
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}
//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle::Vector & parts,
                                  std::vector<const LHCb::ProtoParticle*> & proto )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(LHCb::Particle::Vector) dummy proto has size: "
                                        << proto.size() << endmsg ;
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}
//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle::ConstVector & parts )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(LHCb::Particle::ConstVector)" << endmsg ;
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}

//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle::Vector & parts )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(LHCb::Particle::Vector)" << endmsg ;
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}

//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle* particle1 )
{
  //can't have a clone if there is only one particle!
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "foundOverlap(1)" ;
    if(particle1)  verbose() << " on particle: " <<  particle1->particleID().pid();
    verbose() <<  endmsg ;

  }

  return false;
}

//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle* particle1,
                                  const LHCb::Particle* particle2 )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(2)" << endmsg ;
  if (!particle1 || !particle2) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}

//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle* particle1,
                                  const LHCb::Particle* particle2,
                                  const LHCb::Particle* particle3)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(3)" << endmsg ;
  if (!particle1 || !particle2 || !particle3) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}
//===========================================================================
bool FindCloneTool::foundOverlap( const LHCb::Particle* particle1,
                                  const LHCb::Particle* particle2,
                                  const LHCb::Particle* particle3,
                                  const LHCb::Particle* particle4)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "foundOverlap(4)" << endmsg ;
  if (!particle1 || !particle2 || !particle3|| !particle4 ) Exception("Null pointer");
  LHCb::Particle::ConstVector parts ;
  parts.push_back( particle1 );
  parts.push_back( particle2 );
  parts.push_back( particle3 );
  parts.push_back( particle4 );
  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;
  return foundOverlap( parts, all_protos, isclone );
}
//===========================================================================
// Check for duplicate use of all protoparticles to produce decay tree of
// any particle in vector. Removes found particles from vector.

//===========================================================================
StatusCode FindCloneTool::removeOverlap( LHCb::Particle::Vector& PV)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap( LHCb::Particle::Vector) in size is " << PV.size() << endmsg ;

  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;

  if (!foundOverlap( PV,all_protos,isclone )) return StatusCode::SUCCESS ;

  if (isclone.size() != PV.size()  )
  {
    err()<< "sizes of vectors are not equal, cannot remove overlap" << endmsg;
    return StatusCode::FAILURE;
  }


  LHCb::Particle::Vector Out ;

  unsigned int j=0;
  for ( LHCb::Particle::Vector::const_iterator i = PV.begin() ; j<isclone.size() && i!=PV.end() ; ++i)
  {
    if (!isclone[j])
    {
      Out.push_back(*i);
      if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap: Thinks this particle with"
                                            << " PID: " << (*i)->particleID().pid()
                                            << " and Pt : " << (*i)->pt()
                                            << " is a unique copy" << endmsg ;
    }
    else if (msgLevel(MSG::VERBOSE)) verbose() << "searchOverlap: Thinks this particle with"
                                               << " PID: " << (*i)->particleID().pid()
                                               << " and Pt : " << (*i)->pt()
                                               << " is a clone" << endmsg ;
    ++j;
  }
  PV = Out ;
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap( LHCb::Particle::Vector) out size is " << PV.size() << endmsg ;
  return StatusCode::SUCCESS ;
}

//===========================================================================
StatusCode FindCloneTool::removeOverlap( LHCb::Particle::ConstVector& PV)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap( LHCb::Particle::ConstVector) in size is " << PV.size() << endmsg ;

  std::vector< std::vector<const LHCb::ProtoParticle*> > all_protos(0);
  std::vector<bool> isclone;

  if (!foundOverlap( PV,all_protos,isclone )) return StatusCode::SUCCESS ;

  if (isclone.size() != PV.size()  )
  {
    err()<< "sizes of vectors are not equal, cannot remove overlap" << endmsg;
    return StatusCode::FAILURE;
  }


  LHCb::Particle::ConstVector Out ;

  unsigned int j=0;
  for ( LHCb::Particle::ConstVector::const_iterator i = PV.begin() ; j<isclone.size() && i!=PV.end() ; ++i)
  {
    if (!(isclone[j])) Out.push_back(*i);
    ++j;
  }
  PV = Out ;
  if (msgLevel(MSG::VERBOSE)) verbose() << "removeOverlap( LHCb::Particle::ConstVector) out size is " << PV.size() << endmsg ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Replace resonance by daughters in vector, helper to the Tree methods
//=============================================================================
StatusCode  FindCloneTool::addOrigins( const LHCb::Particle::ConstVector& parts,
                                       std::vector< std::vector<const LHCb::ProtoParticle*> >& protos)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() of " << parts.size() << endmsg ;

  for ( LHCb::Particle::ConstVector::const_iterator c = parts.begin() ; c !=  parts.end() ; ++c )
  {
    std::vector<const LHCb::ProtoParticle*> oneparticle;

    StatusCode sc = addOrigins(*c, oneparticle);
    if (!sc) return sc ;
    protos.push_back(oneparticle);

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() left with " << protos.size() << "sets of protos" << endmsg ;
  return StatusCode::SUCCESS ;
}

StatusCode  FindCloneTool::addOrigins( const LHCb::Particle::Vector& parts,
                                       std::vector< std::vector<const LHCb::ProtoParticle*> >& protos)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() of " << parts.size() << endmsg ;

  for ( LHCb::Particle::Vector::const_iterator c = parts.begin() ; c !=  parts.end() ; ++c )
  {
    std::vector<const LHCb::ProtoParticle*> oneparticle;

    StatusCode sc = addOrigins(*c, oneparticle);
    if (!sc) return sc ;
    protos.push_back(oneparticle);

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() left with " << protos.size() << "sets of protos" << endmsg ;
  return StatusCode::SUCCESS ;
}

StatusCode  FindCloneTool::addOrigins( const LHCb::Particle* part,
                                       std::vector< const LHCb::ProtoParticle* >& protos)
{
  /// Fill a vector with the protos used by a given particle.

  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins(1) " << endmsg ;

  if (msgLevel(MSG::VERBOSE))
    verbose() << "Particle " << part->particleID().pid()
              << " " << part->momentum() << endmsg ;
  if ( part->proto() ) //it's a basic particle
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "has an origin " << part->proto() << endmsg ;
    protos.push_back(part->proto());
  }
  else if ( !(part->daughters().empty()) )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "has a daughters " << part->daughters().size() << endmsg ;

    LHCb::Particle::ConstVector dau = part->daughtersVector()  ;

    for ( LHCb::Particle::ConstVector::const_iterator c = dau.begin() ; c !=  dau.end() ; ++c )
    {
      StatusCode sc = addOrigins(*c, protos); //interate
      if (!sc) return sc ;
    }

  }
  else //there's some error
  {
    if ( 0==m_ppSvc) m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
    const LHCb::ParticleProperty *pp = m_ppSvc->find(part->particleID());
    if (0!=pp) Warning(pp->particle()+" has no proto nor endVertex. Assuming it's from MC.",
                       StatusCode::SUCCESS) ;
    else err() << "Particle with unknown PID " << part->particleID().pid() << " has no endVertex. "
               <<  "Assuming it's from MC" << endmsg ;
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "addOrigins() left " << protos.size() << endmsg ;
  return StatusCode::SUCCESS ;

}
