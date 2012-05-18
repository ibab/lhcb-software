//Created by Murilo Rangel

// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/Algo.h"
#include "LoKi/CoreCuts.h"
//#include "LoKi/GenTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/IParticle2MCAssociator.h"
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#include "Utils.h"

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

//using namespace Utils;

LOKI_MCALGORITHM(AllMC)
{

  //LoKi::GenTypes::GRange GQuarks	= gselect( "GQuarks" , (4 == GABSID || 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID) && GPT > 5000); 
  Types::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS );
  //LHCb::Particle::Range neutral 	= get<LHCb::Particle::Range>("Phys/PFParticles/Particles");
  Range neutral 	= select("neutral",charge == 0);


  Tuple tuple = nTuple("JEStuple","JEStuple");

  LHCb::RecVertex::Range pvs = primaryVertices();

  tuple->column("NRecVertices",pvs.size());
  if(pvs.size()) tuple->column("RecVertex_z",pvs[0]->position().z());
  else tuple->column("RecVertex_z",-999.);

  int NVertices = 0, VID = -9999;

  std::vector<int> vid;

  std::vector<const HepMC::GenParticle *> PileUpGenParticles;


  for(LoKi::GenTypes::GRange::iterator ik1 = GFinal.begin(); GFinal.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *gen = *ik1;

      if(std::find(vid.begin(), vid.end(), gen->parent_event()->signal_process_id())==vid.end()) {vid.push_back(gen->parent_event()->signal_process_id()); NVertices++;}

      if(gen->momentum().pz()<0) continue;

      PileUpGenParticles.push_back(gen);
  }

  tuple->column("NVertices",NVertices);


  tuple->farray("pu_px",GPX,PileUpGenParticles.begin(),PileUpGenParticles.end(),"Npu",10000);
  tuple->farray("pu_py",GPY,PileUpGenParticles.begin(),PileUpGenParticles.end(),"Npu",10000);
  tuple->farray("pu_pz",GPZ,PileUpGenParticles.begin(),PileUpGenParticles.end(),"Npu",10000);
  tuple->farray("pu_e",GE,PileUpGenParticles.begin(),PileUpGenParticles.end(),"Npu",10000);
  tuple->farray("pu_pdgid",GID,PileUpGenParticles.begin(),PileUpGenParticles.end(),"Npu",10000);

/*
  for(Range::iterator ipn = neutral.begin(); neutral.end()!=ipn; ++ipn) {
    const LHCb::Particle* partn = *ipn;
    neutral_info.push_back((int) partn->info(951,-10)); // TODO - Create a functor for this

  }//info() << eneutral << " " << ecell << endreq;
*/

  tuple->farray("neutral_px",PX,neutral.begin(), neutral.end(),"Nneutral",10000);
  tuple->farray("neutral_py",PY,neutral.begin(), neutral.end(),"Nneutral",10000);
  tuple->farray("neutral_pz",PZ,neutral.begin(), neutral.end(),"Nneutral",10000);
  tuple->farray("neutral_pid",ID,neutral.begin(), neutral.end(),"Nneutral",10000);
  tuple->farray("neutral_e",E,neutral.begin(), neutral.end(),"Nneutral",10000);
  // Falta o info() partn->info(951,-10)

  // Falta implementar a parte das partículas carregadas

  tuple->write();
  return StatusCode::SUCCESS ;
}
