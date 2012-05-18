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

LOKI_MCALGORITHM(ZplusJetsMC)
{

  LoKi::GenTypes::GRange GQuarks	= gselect( "GQuarks" , 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID ); 
  LoKi::GenTypes::GRange GZ		= gselect( "GZ" , 23 == GABSID );
  LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS );
  LHCb::Particle::Range muons 		= get<LHCb::Particle::Range>("/Event/Phys/StdLooseMuons/Particles");
  LHCb::Particle::Range neutral 	= get<LHCb::Particle::Range>("/Event/Phys/StdLooseAllPhotons/Particles") ;
  LHCb::Particle::Range charged 	= get<LHCb::Particle::Range>("/Event/Phys/StdNoPIDsPions/Particles") ;
  LHCb::Particle::Range charged_dw 	= get<LHCb::Particle::Range>("/Event/Phys/StdNoPIDsDownPions/Particles") ;

  if(muons.size()<2) return StatusCode::SUCCESS;
  std::vector<const LHCb::Particle*> muonlist;
  std::vector<LorentzVector> muonvectors;
  LHCb::Vertex *PV = new LHCb::Vertex();
  const IVertexFit *vfit = vertexFitter();
  vfit->fit(*PV,*muons[0],*muons[1]);
  for(Range::iterator ip = muons.begin(); muons.end()!=ip; ++ip) {
    const LHCb::Particle* part = *ip; 
    if(part->pt()<20000) continue;
    muonlist.push_back(part);
    LorentzVector g = part->momentum();
    muonvectors.push_back(g);
  }
  if(muonlist.size()!=2) return StatusCode::SUCCESS;
  
  if((muonlist[0]->momentum()+muonlist[1]->momentum()).mass()<40000) return StatusCode::SUCCESS;

  HepMC::GenVertex *GV, *Z_GV_decay;
  HepMC::GenEvent *GE;
//  Tuple gentuple = nTuple("ZtupleGen","ZtupleGen");
  Tuple tuple = nTuple("Ztuple","Ztuple");

  for(LoKi::GenTypes::GRange::iterator ik1 = GZ.begin(); GZ.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *Z = *ik1;
    if(ik1==GZ.begin()) {GV = Z->production_vertex(); GE = Z->parent_event();
//    info() << "Z Status = " << Z->status() << " " << Z->momentum().phi() << endreq;
//    info() << "Vertex BAR = " << Z->end_vertex()->barcode() << endreq;
//    info() << "Event = " << GE->signal_process_id() << endreq;
    tuple->column("Z_px",Z->momentum().px());
    tuple->column("Z_py",Z->momentum().py());
    tuple->column("Z_pz",Z->momentum().pz());
    tuple->column("Z_e",Z->momentum().e());}
    else {
      Z_GV_decay = Z->end_vertex();
//      info() << "Vertex BAR = " << Z->end_vertex()->barcode() << " " << Z->momentum().px() << endreq;
    }
  }

  int Npu = 0, Npv = 0, NVertices = 1, VID = -9999;
  std::vector<float> pv_px;
  std::vector<float> pv_py;
  std::vector<float> pv_pz;
  std::vector<float> pv_e;
  std::vector<int> pv_pdgid;
  std::vector<float> pu_px;
  std::vector<float> pu_py;
  std::vector<float> pu_pz;
  std::vector<float> pu_e;
  std::vector<int> pu_pdgid;
  std::vector<int> vid;

  for(LoKi::GenTypes::GRange::iterator ik1 = GFinal.begin(); GFinal.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *gen = *ik1;
    if(GE->signal_process_id() == gen->parent_event()->signal_process_id()) {
      HepMC::FourVector lv = gen->momentum();
      pv_px.push_back(lv.px());
      pv_py.push_back(lv.py());
      pv_pz.push_back(lv.pz());
      pv_e.push_back(lv.e());
      pv_pdgid.push_back(gen->pdg_id());
      Npv++;
      //info() << "Final Status = " << gen->production_vertex()->barcode() << " " << gen->momentum().px() << endreq;
    } else {
      if(std::find(vid.begin(), vid.end(), gen->parent_event()->signal_process_id())==vid.end()) {vid.push_back(gen->parent_event()->signal_process_id()); NVertices++;}
      HepMC::FourVector lv = gen->momentum();
      pu_px.push_back(lv.px());
      pu_py.push_back(lv.py());
      pu_pz.push_back(lv.pz());
      pu_e.push_back(lv.e());
      pu_pdgid.push_back(gen->pdg_id());
      Npu++;
    }
  }
  tuple->column("NVertices",NVertices);
  tuple->farray("pv_px",pv_px,"Npv",10000);
  tuple->farray("pv_py",pv_py,"Npv",10000);
  tuple->farray("pv_pz",pv_pz,"Npv",10000);
  tuple->farray("pv_e",pv_e,"Npv",10000);
  tuple->farray("pv_pdgid",pv_pdgid,"Npv",10000);
  tuple->farray("pu_px",pu_px,"Npu",10000);
  tuple->farray("pu_py",pu_py,"Npu",10000);
  tuple->farray("pu_pz",pu_pz,"Npu",10000);
  tuple->farray("pu_e",pu_e,"Npu",10000);
  tuple->farray("pu_pdgid",pu_pdgid,"Npu",10000);

  for(LoKi::GenTypes::GRange::iterator ik1 = GQuarks.begin(); GQuarks.end() != ik1 ; ++ik1 ) {
    const HepMC::GenParticle *gen = *ik1;
    //info() << "Event = " << gen->parent_event()->signal_process_id() << endreq;
    //if(gen->production_vertex()) info() << gen->production_vertex()->barcode() << endreq;
    if(GE->signal_process_id() == gen->parent_event()->signal_process_id())
    if(GV->barcode() == gen->production_vertex()->barcode()) {
      tuple->column("q_px",gen->momentum().px());
      tuple->column("q_py",gen->momentum().py());
      tuple->column("q_pz",gen->momentum().pz());
      tuple->column("q_e",gen->momentum().e());
      tuple->column("q_pdgid",gen->pdg_id());
      break;
    }
//    info() << "Status = " << gen->pdg_id() << " " << gen->momentum().phi() << endreq;
  }


  //muons.sort(ptorder);
  //std::sort(muons.begin(),muons.end(),ptorder);


//  std::sort(muonlist.begin(), muonlist.end(), ptorder);

//  info() << "Event with #Muons = " << muons.size() << endreq;
//  info() << "#Neutrals = " << neutral.size() << endreq;
//  info() << "#Charged = " << charged.size() << endreq;
  int Nneutral = neutral.size();
  int Ncharged = charged.size();

  tuple->column("m",(muonlist[0]->momentum()+muonlist[1]->momentum()).mass());
  tuple->column("muon1_Px",muonlist[0]->momentum().Px());
  tuple->column("muon1_Py",muonlist[0]->momentum().Py());
  tuple->column("muon1_Pz",muonlist[0]->momentum().Pz());
  tuple->column("muon1_E",muonlist[0]->momentum().E());
  tuple->column("muon2_Px",muonlist[1]->momentum().Px());
  tuple->column("muon2_Py",muonlist[1]->momentum().Py());
  tuple->column("muon2_Pz",muonlist[1]->momentum().Pz());
  tuple->column("muon2_E",muonlist[1]->momentum().E());
  tuple->column("PVx",PV->position().X());
  tuple->column("PVy",PV->position().Y());
  tuple->column("PVz",PV->position().Z());
//  tuple->column("Nneutral",Nneutral);
//  tuple->column("Ncharged",Ncharged);
  std::vector<float> neutral_px;
  std::vector<float> neutral_py;
  std::vector<float> neutral_pz;
  std::vector<float> neutral_e;
  std::vector<float> charged_px;
  std::vector<float> charged_py;
  std::vector<float> charged_pz;
  std::vector<float> charged_e;
  std::vector<float> charged_IP;
  std::vector<float> charged_IPz;
  std::vector<float> SV_z;
  std::vector<float> SV_x;
  std::vector<float> SV_y;
  std::vector<float> SV_M;
  const std::vector<std::vector<double> > neutral_matrix;

  for(Range::iterator ipn = neutral.begin(); neutral.end()!=ipn; ++ipn) {
    const LHCb::Particle* partn = *ipn;
    LorentzVector lv = partn->momentum();
    neutral_px.push_back(lv.Px());
    neutral_py.push_back(lv.Py());
    neutral_pz.push_back(lv.Pz());
    neutral_e.push_back(lv.E());
  }
  tuple->farray("neutral_px",neutral_px,"Nneutral",10000);
  tuple->farray("neutral_py",neutral_py,"Nneutral",10000);
  tuple->farray("neutral_pz",neutral_pz,"Nneutral",10000);
  tuple->farray("neutral_e",neutral_e,"Nneutral",10000);
  const IDistanceCalculator* idist = distanceCalculator();// = IAlgTool::create("IDistanceCalculator","id");// = LoKi::Vertices::ImpactParamTool::tool();
  LoKi::Vertices::ImpactParamTool *imp = new LoKi::Vertices::ImpactParamTool(idist);
  Gaudi::XYZVector *impact = new Gaudi::XYZVector();
  int NSV = 0;
  LHCb::Vertex *SV = new LHCb::Vertex();
  for(Range::iterator ipc = charged.begin(); charged.end()!=ipc; ++ipc) {
    const LHCb::Particle* partc = *ipc;
    LorentzVector lv = partc->momentum();
    if(lv.Pt()<200) continue;
    charged_px.push_back(lv.Px());
    charged_py.push_back(lv.Py());
    charged_pz.push_back(lv.Pz());
    charged_e.push_back(lv.E());
    idist->distance(partc,PV,*impact);
    //imp->setParticle(partc);
    charged_IP.push_back(impact->R());
    charged_IPz.push_back(impact->Z());
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
  }

  for(Range::iterator ipc = charged_dw.begin(); charged_dw.end()!=ipc; ++ipc) {
    const LHCb::Particle* partc = *ipc;
    LorentzVector lv = partc->momentum();
    if(lv.Pt()<200) continue;
    charged_px.push_back(lv.Px());
    charged_py.push_back(lv.Py());
    charged_pz.push_back(lv.Pz());
    charged_e.push_back(lv.E());
    idist->distance(partc,PV,*impact);
    //imp->setParticle(partc);
    charged_IP.push_back(impact->R());
    charged_IPz.push_back(impact->Z());
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
  }

  tuple->farray("charged_px",charged_px,"Ncharged",10000);
  tuple->farray("charged_py",charged_py,"Ncharged",10000);
  tuple->farray("charged_pz",charged_pz,"Ncharged",10000);
  tuple->farray("charged_e",charged_e,"Ncharged",10000);
  tuple->farray("charged_IP",charged_IP,"Ncharged",10000);
  tuple->farray("charged_IPz",charged_IPz,"Ncharged",10000);

  tuple->farray("SV_x",SV_x,"NSV",10000);
  tuple->farray("SV_y",SV_y,"NSV",10000);
  tuple->farray("SV_z",SV_z,"NSV",10000);
  tuple->farray("SV_M",SV_M,"NSV",10000);
  tuple->write();
  return StatusCode::SUCCESS ;
}
