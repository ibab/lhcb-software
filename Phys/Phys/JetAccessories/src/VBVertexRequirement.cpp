// $Id: $
// Include files

// local
#include "VBVertexRequirement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VBVertexRequirement
//
// 2012-01-27 : Albert Bursche
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VBVertexRequirement )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VBVertexRequirement::VBVertexRequirement( const std::string& name,
                                          ISvcLocator* pSvcLocator)
: DaVinciAlgorithm ( name , pSvcLocator )
{
}

//=============================================================================
// Destructor
//=============================================================================
VBVertexRequirement::~VBVertexRequirement() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VBVertexRequirement::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  const LHCb::ParticleID pidRequired(23);
  const LHCb::ParticleID pidJet(98);
  const double thesholdD = 1*Gaudi::Units::mm;
  const double thesholdChiq = 1000;
  if ( msgLevel(MSG::DEBUG) && !particles().empty() )
    debug() << "Looking at " <<  particles().size() << " particles. PID" <<
      (*particles().begin()  )->particleID().pid() <<  endmsg;
  for (LHCb::Particle::Range::iterator it = particles().begin();
       it!=particles().end();++it)
  {
    if ((*it)->particleID().abspid() == pidRequired.abspid())
    {
      for (LHCb::Particle::Range::iterator jt = particles().begin();
           jt!=particles().end();++jt)
      {
        if ((*jt)->particleID().abspid() == pidJet.abspid())
        {
          double chiq = 100000;
          double dist = 100000;
          distanceCalculator("LoKi::DistanceCalculator")->distance(bestPV(*it),
                                                                   bestPV(*jt),
                                                                   dist,
                                                                   chiq);
          if ( msgLevel(MSG::DEBUG) ) debug() << "dist =" << dist << " chiq =" << chiq << endmsg;
          if (dist<thesholdD && chiq<thesholdChiq)
          {
            markParticle(new LHCb::Particle(**jt));
            //markTree(*jt);
            //  setFilterPassed(true);
            if ( msgLevel(MSG::DEBUG) ) debug() << "Accepted particle" << endmsg;
          }
        }
      }
    }
  }
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
