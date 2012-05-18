//Created by Murilo Rangel

// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/Algo.h"
#include "LoKi/CoreCuts.h"
#include "LoKi/GenTypes.h"
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

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

LOKI_MCALGORITHM(VplusJetsMC)
{

  LoKi::GenTypes::GRange GQuarks	= gselect( "GQuarks" , (4 == GABSID || 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID) && GPT > 5000); 
  LoKi::GenTypes::GRange GVB		= gselect( "GVB" , 24 == GABSID || 23 == GABSID );
  LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS );

  LHCb::Particle::Range rpfparticles			= get<LHCb::Particle::Range>("Phys/PFParticles/Particles");
  LHCb::Particle::Range rpfbannedparticles 		= get<LHCb::Particle::Range>("Phys/PFBannedParticles/Particles");

  std::vector<const LHCb::Particle *> allpfparticles;

  for (LHCb::Particle::Range::iterator ipart = rpfparticles.begin() ; ipart != rpfparticles.end() ; ++ipart){

	  const LHCb::Particle* part = *ipart;
	  allpfparticles.push_back(part);

  }

  for (LHCb::Particle::Range::iterator ipart = rpfbannedparticles.begin() ; ipart != rpfbannedparticles.end() ; ++ipart){

	  const LHCb::Particle* part = *ipart;
	  allpfparticles.push_back(part);

  }

  Range neutralpfparticles = select("neutralpfparticlesl",allpfparticles.begin(), allpfparticles.end(), Q == 0);
  Range chargedpfparticles = select("chargedpfparticlesl",allpfparticles.begin(), allpfparticles.end(), Q != 0);

  LHCb::RecVertex::Range pvs = primaryVertices();

  LHCb::Vertex *PV = new LHCb::Vertex();

  HepMC::GenVertex *GV;
  HepMC::GenEvent *VBosonEvent;
  Tuple tuple = nTuple("JEStuple","JEStuple");
  tuple->column("NRecVertices",pvs.size());

  for(LoKi::GenTypes::GRange::iterator ik1 = GVB.begin(); GVB.end() != ik1 ; ++ik1 ) {
	  const HepMC::GenParticle *Vgen = *ik1;

	  if(ik1==GVB.begin()) {
		  GV = Vgen->production_vertex(); VBosonEvent = Vgen->parent_event();
		  const Gaudi::XYZPoint xyz(GV->point3d().x(),GV->point3d().y(),GV->point3d().z());
		  PV->setPosition(xyz);
		  tuple->column("V_px",Vgen->momentum().px());
		  tuple->column("V_py",Vgen->momentum().py());
		  tuple->column("V_pz",Vgen->momentum().pz());
		  tuple->column("V_e",Vgen->momentum().e());
	  }

	  break;
  }

  int Npu = 0, Npv = 0, Nq = 0, NVertices = 1, VID = -9999;
  std::vector<int> vid;

  std::vector<const HepMC::GenParticle *> genparts_from_pv_vboson;
  std::vector<const HepMC::GenParticle *> genparts_from_pu_vboson;

  for(LoKi::GenTypes::GRange::iterator ik1 = GFinal.begin(); GFinal.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *gen = *ik1;
    if(gen->momentum().pz()<0) continue;
    if(VBosonEvent->signal_process_id() == gen->parent_event()->signal_process_id()) {

	    genparts_from_pv_vboson.push_back(gen);

    } else {
      if(std::find(vid.begin(), vid.end(), gen->parent_event()->signal_process_id())==vid.end()) {vid.push_back(gen->parent_event()->signal_process_id()); NVertices++;}

      genparts_from_pu_vboson.push_back(gen);

    }
  }

  tuple->column("NVertices",NVertices);

  tuple->farray("pv_px",GPX,genparts_from_pv_vboson.begin(),genparts_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("pv_py",GPY,genparts_from_pv_vboson.begin(),genparts_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("pv_pz",GPZ,genparts_from_pv_vboson.begin(),genparts_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("pv_e",GE,genparts_from_pv_vboson.begin(),genparts_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("pv_pdgid",GID,genparts_from_pv_vboson.begin(),genparts_from_pv_vboson.end(),"Npv",10000);

  tuple->farray("pu_px",GPX,genparts_from_pu_vboson.begin(),genparts_from_pu_vboson.end(),"Npu",10000);
  tuple->farray("pu_py",GPY,genparts_from_pu_vboson.begin(),genparts_from_pu_vboson.end(),"Npu",10000);
  tuple->farray("pu_pz",GPZ,genparts_from_pu_vboson.begin(),genparts_from_pu_vboson.end(),"Npu",10000);
  tuple->farray("pu_e",GE,genparts_from_pu_vboson.begin(),genparts_from_pu_vboson.end(),"Npu",10000);
  tuple->farray("pu_pdgid",GID,genparts_from_pu_vboson.begin(),genparts_from_pu_vboson.end(),"Npu",10000);

  std::vector<const HepMC::GenParticle*> quarks_from_pv_vboson;

  for(LoKi::GenTypes::GRange::iterator ik1 = GQuarks.begin(); GQuarks.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *gen = *ik1;
    if(VBosonEvent->signal_process_id() == gen->parent_event()->signal_process_id()) {

    quarks_from_pv_vboson.push_back(gen);

    }
  }


  tuple->farray("q_px",GPX,quarks_from_pv_vboson.begin(),quarks_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("q_py",GPY,quarks_from_pv_vboson.begin(),quarks_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("q_pz",GPZ,quarks_from_pv_vboson.begin(),quarks_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("q_e",GE,quarks_from_pv_vboson.begin(),quarks_from_pv_vboson.end(),"Npv",10000);
  tuple->farray("q_pdgid",GID,quarks_from_pv_vboson.begin(),quarks_from_pv_vboson.end(),"Npv",10000);

  tuple->column("PVx",PV->position().X());
  tuple->column("PVy",PV->position().Y());
  tuple->column("PVz",PV->position().Z());


  std::vector<float> neutral_px;
  std::vector<float> neutral_py;
  std::vector<float> neutral_pz;
  std::vector<float> neutral_e;
  std::vector<float> charged_px;
  std::vector<float> charged_py;
  std::vector<float> charged_pz;
  std::vector<float> charged_e;
  std::vector<float> charged_IP;
  std::vector<int> charged_charge;
  std::vector<bool> charged_hasTT;
  std::vector<bool> charged_hasT;
  std::vector<bool> charged_hasVelo;
  std::vector<float> charged_ghostprob;
  std::vector<float> charged_chi2perdof;
  std::vector<float> charged_IPz;
  std::vector<float> SV_z;
  std::vector<float> SV_x;
  std::vector<float> SV_y;
  std::vector<float> SV_M;
  const std::vector<std::vector<double> > neutral_matrix;

  tuple->farray("neutral_px",PX,neutralpfparticles.begin(),neutralpfparticles.end(),"Nneutral",10000);
  tuple->farray("neutral_py",PY,neutralpfparticles.begin(),neutralpfparticles.end(),"Nneutral",10000);
  tuple->farray("neutral_pz",PZ,neutralpfparticles.begin(),neutralpfparticles.end(),"Nneutral",10000);
  tuple->farray("neutral_e",E,neutralpfparticles.begin(),neutralpfparticles.end(),"Nneutral",10000);

  const IDistanceCalculator* idist = distanceCalculator();// = IAlgTool::create("IDistanceCalculator","id");// = LoKi::Vertices::ImpactParamTool::tool();
  LoKi::Vertices::ImpactParamTool *imp = new LoKi::Vertices::ImpactParamTool(idist);
  Gaudi::XYZVector *impact = new Gaudi::XYZVector();
  int NSV = 0;
  LHCb::Vertex *SV = new LHCb::Vertex();
  const IVertexFit *vfit = vertexFitter();
  for(Range::iterator ipc = charged.begin(); charged.end()!=ipc; ++ipc) {
    const LHCb::Particle* partc = *ipc;
    LorentzVector lv = partc->momentum();
    const LHCb::Track *itrk = partc->proto()->track();
    if(lv.Pt()<200) continue;
    charged_px.push_back(lv.Px());
    charged_py.push_back(lv.Py());
    charged_pz.push_back(lv.Pz());
    charged_e.push_back(lv.E());
    idist->distance(partc,PV,*impact);
    charged_IP.push_back(impact->R());
    charged_IPz.push_back(impact->Z());
    charged_charge.push_back(itrk->charge());
    charged_ghostprob.push_back(itrk->ghostProbability());
    charged_chi2perdof.push_back(itrk->chi2PerDoF());
    charged_hasT.push_back(itrk->hasT());
    charged_hasTT.push_back(itrk->hasTT());
    charged_hasVelo.push_back(itrk->hasVelo());
/*
    if(lv.Pt()<500) continue;
    for(Range::iterator ipc2 = ipc; charged.end()!=ipc2; ++ipc2) {
      if(ipc==ipc2) continue;
      const LHCb::Particle* partc2 = *ipc2;
      vfit->fit(*SV,ipc,ipc2);
      double SVdist = -1e6;
      idist->distance(SV,PV,SVdist);
      if(SVdist<-1e6+1) continue; if(SVdist<0.5) continue; 
      if(std::sqrt(std::pow(SV->position().X()-PV->position().X(),2.)+std::pow(SV->position().Y()-PV->position().Y(),2.))<50) continue;
      SV_x.push_back(SV->position().X());
      SV_y.push_back(SV->position().Y());
      SV_z.push_back(SV->position().Z());
      SV_M.push_back((partc->momentum()+partc2->momentum()).mass());
      NSV++;
    }
*/
  }

  for(Range::iterator ipc = charged_dw.begin(); charged_dw.end()!=ipc; ++ipc) {
    const LHCb::Particle* partc = *ipc;
    LorentzVector lv = partc->momentum();
    if(lv.Pt()<200) continue;
    const LHCb::Track *itrk = partc->proto()->track();
    charged_px.push_back(lv.Px());
    charged_py.push_back(lv.Py());
    charged_pz.push_back(lv.Pz());
    charged_e.push_back(lv.E());
    idist->distance(partc,PV,*impact);
    //imp->setParticle(partc);
    charged_IP.push_back(impact->R());
    charged_IPz.push_back(impact->Z());
    charged_charge.push_back(itrk->charge());
    charged_ghostprob.push_back(itrk->ghostProbability());
    charged_chi2perdof.push_back(itrk->chi2PerDoF());
    charged_hasT.push_back(itrk->hasT());
    charged_hasTT.push_back(itrk->hasTT());
    charged_hasVelo.push_back(itrk->hasVelo());
/*
    if(lv.Pt()<500) continue;
    for(Range::iterator ipc2 = ipc; charged_dw.end()!=ipc2; ++ipc2) {
      if(ipc==ipc2) continue;
      const LHCb::Particle* partc2 = *ipc2;
      vfit->fit(*SV,ipc,ipc2);
      double SVdist = -1e6;
      idist->distance(SV,PV,SVdist);
      if(SVdist<-1e6+1) continue; if(SVdist<0.5) continue; 
      if(std::sqrt(std::pow(SV->position().X()-PV->position().X(),2.)+std::pow(SV->position().Y()-PV->position().Y(),2.))<50) continue;
      SV_x.push_back(SV->position().X());
      SV_y.push_back(SV->position().Y());
      SV_z.push_back(SV->position().Z());
      SV_M.push_back((partc->momentum()+partc2->momentum()).mass());
      NSV++;
    }
*/
    }

  tuple->farray("charged_px",charged_px,"Ncharged",10000);
  tuple->farray("charged_py",charged_py,"Ncharged",10000);
  tuple->farray("charged_pz",charged_pz,"Ncharged",10000);
  tuple->farray("charged_e",charged_e,"Ncharged",10000);
  tuple->farray("charged_IP",charged_IP,"Ncharged",10000);
  tuple->farray("charged_IPz",charged_IPz,"Ncharged",10000);
  tuple->farray("charged_charge",charged_charge,"Ncharged",10000);
  tuple->farray("charged_chi2perdof",charged_chi2perdof,"Ncharged",10000);
  tuple->farray("charged_hasT",charged_hasT,"Ncharged",10000);
  tuple->farray("charged_hasTT",charged_hasTT,"Ncharged",10000);
  tuple->farray("charged_hasVelo",charged_hasVelo,"Ncharged",10000);
  tuple->farray("charged_ghostprob",charged_ghostprob,"Ncharged",10000);
/*
  tuple->farray("SV_x",SV_x,"NSV",10000);
  tuple->farray("SV_y",SV_y,"NSV",10000);
  tuple->farray("SV_z",SV_z,"NSV",10000);
  tuple->farray("SV_M",SV_M,"NSV",10000);
*/

  tuple->write();
  return StatusCode::SUCCESS ;

}
