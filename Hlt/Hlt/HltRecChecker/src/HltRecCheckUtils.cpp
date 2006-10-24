// $Id: HltRecCheckUtils.cpp,v 1.1 2006-10-24 09:50:49 hernando Exp $
// Include files

// local
#include "HltRecCheckUtils.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : HltRecCheckUtils
//
// 2006-10-03 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


int MCHlt::ancestors( const MCParticle& part,
                              std::vector<MCParticle*>& xancestors ) 
{
  const MCParticle* mother = part.mother();
  if (!mother) return xancestors.size();
  
  xancestors.push_back( (MCParticle*) mother);
  return MCHlt::ancestors(*mother,xancestors);
}

const MCParticle& MCHlt::ancestor( const MCParticle& part )
{
  const MCParticle* mother = part.mother();
  if (!mother) return part;
  return MCHlt::ancestor(*mother);
}

int MCHlt::iquark(const LHCb::MCParticle& par) {
  if (par.particleID().hasBottom()) return 1;
  else if (par.particleID().hasCharm()) return 2;
  return 3;
}

int MCHlt::iquark(const MCParticles& mcpars) 
{
  int quark = 3;
  for (MCParticles::const_iterator it = mcpars.begin(); 
       it != mcpars.end(); it++) {
    const MCParticle& mcpar = *(*it);
    const MCParticle& mother = MCHlt::ancestor(mcpar);
    int q = MCHlt::iquark(mother);
    if (q == 1) return 1;
  }
  return quark;
}

void MCHlt::toTrack(const MCParticle& mcpar, Track& track) {
  State state;
  double x = mcpar.originVertex()->position().x();
  double y = mcpar.originVertex()->position().y();
  double z = mcpar.originVertex()->position().z();
  double px = mcpar.momentum().x();
  double py = mcpar.momentum().y();
  double pz = mcpar.momentum().z();
  double pp = mcpar.p();
  double tx = px/pz;
  double ty = py/pz;
  state.setState(x,y,z,tx,ty,1/pp);
  state.setLocation(State::ClosestToBeam);
  track.addToStates(state);
}

void MCHlt::to2DTrack(const MCParticle& mcpar, Track& track) {
  State state;
  double x = mcpar.originVertex()->position().x();
  double y = mcpar.originVertex()->position().y();
  double z = mcpar.originVertex()->position().z();
  double r = sqrt(x*x+y*y);
  double phi = asin(x/r);
  double px = mcpar.momentum().x();
  double py = mcpar.momentum().y();
  double pz = mcpar.momentum().z();
  double pp = mcpar.p();
  double tx = px/pz;
  double ty = py/pz;
  state.setState(r,phi,z,tx,ty,1/pp);
  state.setLocation(State::ClosestToBeam);
  track.addToStates(state);
}

RecVertex MCHlt::toOriginVertex(const MCParticle& mcpar) {
  RecVertex vertex;
  double x = mcpar.originVertex()->position().x();
  double y = mcpar.originVertex()->position().y();
  double z = mcpar.originVertex()->position().z();
  Gaudi::XYZPoint pos(x,y,z);
  vertex.setPosition( pos );
  return vertex;
}

