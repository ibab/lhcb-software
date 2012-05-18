#include "ROOTOscar.h"

//typedef std::vector<Particle*> ParticlesVector;
bool order_by_e (Particle *ip, Particle *yp) { return (ip->E()> yp->E()); }
bool order_by_pt (Particle *ip, Particle *yp) { return (ip->Pt()> yp->Pt()); }

bool truep(Particle *p){return true;}
 
typedef std::vector<Particle*> ParticlesVector;

void FillParticlesVector(ParticlesVector &vec,Float_t px[], Float_t py[], Float_t pz[], Float_t e[], Int_t maxindex){
	for (int i=0;i<maxindex;i++){
		Particle *p = new Particle(px[i], py[i], pz[i], e[i], 0);
		vec.push_back(p);
	}
}

void FillParticlesVector(ParticlesVector &vec,Float_t px[], Float_t py[], Float_t pz[], Float_t e[], Int_t pdgid[], Int_t maxindex){
	for (int i=0;i<maxindex;i++){
		Particle *p = new Particle(px[i], py[i], pz[i], e[i], pdgid[i]);
		vec.push_back(p);
	}
}

void FillParticlesVector(ParticlesVector &vec,Float_t px[], Float_t py[], Float_t pz[], Float_t e[], Float_t pdgid[], Int_t maxindex){
	for (int i=0;i<maxindex;i++){
		Particle *p = new Particle(px[i], py[i], pz[i], e[i], pdgid[i]);
		vec.push_back(p);
	}
}

void FillAditionalInfo(ParticlesVector &vec,Float_t n90[], Float_t mtf[], Float_t bpvz[], Float_t tustos[], Float_t N_HasPVInfo[], Int_t maxindex){
	for (int i=0;i<maxindex;i++){
		vec[i]->SetAditionalInfo(n90[i],mtf[i],bpvz[i], tustos[i], N_HasPVInfo[i]);
	}
}

void Fillbpvz(ParticlesVector &vec, Float_t bpvz[], Int_t maxindex){
	for (int i=0;i<maxindex;i++){
		vec[i]->Setbpvz(bpvz[i]);
	}
}
