// $Id$

#include <algorithm>

// from DaVinci
#include "Kernel/ICheckOverlap.h"

// local
#include "RemoveClones.h"


//-----------------------------------------------------------------------------
/// Implementation file for class : RemoveClones
/**
 * 2008-06-06 : Robert LAMBERT
 */
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RemoveClones )

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
  RemoveClones::RemoveClones( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
    , m_checkOverlap(NULL)
    , m_NpartIn(0)
    , m_NpartOut(0)
{
  ///<which overlap checker to use. Default will check for clones.
  declareProperty( "FindCloneTool", m_findCloneTool = "FindCloneTool" );

  ///whether to look for unique particles in classes of ID.
  /*true by default as this is the loosest.*/
  declareProperty( "FilterByPid", m_byPID = true );

  ///whether to look for unique particles in classes of abs(ID).
  /*true by default as this is the loosest.
   * with both these set to false, all particles using the same protos
   * will be removed.
   */
  declareProperty( "FilterByAbsPid", m_byAbsPID = false );
}

//=============================================================================
/// Destructor
//=============================================================================
RemoveClones::~RemoveClones() {}

//#############################################################################
/// Initialization
//#############################################################################
StatusCode RemoveClones::initialize()
{

  StatusCode sc = DaVinciAlgorithm::initialize();
  if (!sc) return sc;

  m_checkOverlap = tool<ICheckOverlap>(m_findCloneTool, this ) ;

  return sc;

}
//#############################################################################
/// Execution
//#############################################################################
StatusCode RemoveClones::execute() 
{

  StatusCode sc = StatusCode::SUCCESS;

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.

  // define vectors of particles
  const LHCb::Particle::ConstVector& particles=this->i_particles();
  std::vector< LHCb::Particle::ConstVector > particlesbyPID;
  LHCb::Particle::ConstVector accepted;

  // filter into classes
  if ( m_byPID || m_byAbsPID ) 
  {
    sc=FilterById(particles,particlesbyPID);
  }
  else 
  {
    particlesbyPID.push_back(particles);
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << particles.size() << " particles"
                                        << "split into : " << particlesbyPID.size()
                                        << " different PID groups" << endmsg ;

  if(!sc) return sc;
  unsigned long long NpartIn(0), NpartOut(0);

  //find out which are clones
  for ( std::vector< LHCb::Particle::ConstVector >::iterator i = particlesbyPID.begin();
        i != particlesbyPID.end(); ++i )
  {
    NpartIn += i->size();

    sc = m_checkOverlap->removeOverlap(*i);
    //i should now be smaller, and not include any clones
    if(!sc) return sc;

    NpartOut += i->size();

    //add these unique particles back to a vector for cloning to local storage
    for(LHCb::Particle::ConstVector::const_iterator j=i->begin(); j!=i->end();j++)
    {
      // Mark clones of unique particles for saving
      this->cloneAndMarkTree(*j);
    }

  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Particles sent in: " << NpartIn
              << " | Particles sent out:" << NpartOut << endmsg ;
  }

  if(!sc) return sc;

  m_NpartIn  += NpartIn;
  m_NpartOut += NpartOut;

  setFilterPassed(NpartOut>0);
  return sc;

}
//#############################################################################
/// Finalization
//#############################################################################
StatusCode RemoveClones::finalize() 
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  info() << "Filtered " << m_NpartIn << " to " << m_NpartOut
         << ", removing " << m_NpartIn-m_NpartOut << " clones" << endmsg ;
  return DaVinciAlgorithm::finalize();
}
//#############################################################################
/// FilterById
//#############################################################################
StatusCode
RemoveClones::FilterById(const LHCb::Particle::ConstVector & parts,
                         std::vector< LHCb::Particle::ConstVector > & particlesbyPID) const
{
  if ( parts.empty() ) return StatusCode::SUCCESS;

  //just want to make sure I don't have any dead pointers
  //and to function correctly if the alg is called twice
  particlesbyPID.clear();
  std::vector<int> IDs;

  for ( LHCb::Particle::ConstVector::const_iterator i = parts.begin(); 
        i != parts.end(); ++i )
  {
    //if the ID is in the list push it into the same ID
    //otherwise push the ID back and make a new vector
    bool found=false;
    for ( unsigned int j=0; !found && j<IDs.size() && j<particlesbyPID.size(); ++j )
    {
      if(m_byAbsPID && abs((*i)->particleID().pid())==abs(IDs[j]) )
      {
        particlesbyPID[j].push_back(*i);
        found=true;
      }
      else if((*i)->particleID().pid()==IDs[j])
      {
        particlesbyPID[j].push_back(*i);
        found=true;
      }

    }
    if(!found)
    {
      // if it hasn't been added to the list, make a new list
      LHCb::Particle::ConstVector newvector;
      newvector.push_back(*i);
      IDs.push_back((*i)->particleID().pid());
      particlesbyPID.push_back(newvector);

    }

  }

  if(IDs.size() != particlesbyPID.size())
  {
    err() << "error in RemoveClones::FilterById, Vectors are not the same size" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
