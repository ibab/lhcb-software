#ifndef ROOTOscar_h
#define ROOTOscar_h

#include <TROOT.h>
#include <TRint.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <stdio.h>
#include <iostream>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TFile.h>
#include <string.h>

using namespace TMath;
using namespace std;

//typedef std::vector<Particle*> ParticlesVector;

class Particle:public TLorentzVector{

public:
Particle(float px_tmp=0, float py_tmp=0, float pz_tmp=0, float e_tmp=0,int pdgid_tmp=0){
	SetPxPyPzE(px_tmp,py_tmp,pz_tmp,e_tmp);
	momentum.SetPxPyPzE(px_tmp,py_tmp,pz_tmp,e_tmp);
	pdgid = pdgid_tmp;
}

Particle(TLorentzVector p4,int pdgid_tmp=0){
	SetPxPyPzE(p4.Px(), p4.Py(), p4.Pz(), p4.E());
	momentum.SetPxPyPzE(p4.Px(), p4.Py(), p4.Pz(), p4.E());
	pdgid = pdgid_tmp;
}

~Particle(){ 
	daughters_vector.clear();
	muon_daughters_vector.clear();
	jet_daughters_vector.clear();
		}
Float_t n90, mtf, bpvz, Ntustostriggers, N_HasPVInfo;
int pdgid;
TLorentzVector momentum;

std::vector<Particle *> daughters_vector;
std::vector<Particle *> muon_daughters_vector;
std::vector<Particle *> jet_daughters_vector;

void SetAditionalInfo(Float_t n90_tmp, Float_t mtf_tmp, Float_t bpvz_tmp, Float_t tustos_tmp, Float_t N_HasPVInfo_tmp){
n90 = n90_tmp;
mtf=mtf_tmp;
bpvz=bpvz_tmp;
Ntustostriggers=tustos_tmp;
N_HasPVInfo=N_HasPVInfo_tmp;
}

void Setbpvz(Float_t bpvz_tmp){
bpvz=bpvz_tmp;
}

void add_daughter(Particle* tmp){
daughters_vector.push_back(tmp);
}

bool isInAcc(){return this->Pz() > 0 ? (Abs(this->Px()/this->Pz()) < Tan(0.3) && Abs(this->Py()/this->Pz()) < Tan(0.25)) ? true : false : false;}

};

//typedef std::vector<Particle*> ParticlesVector;
bool order_by_e (Particle*, Particle *) ;
bool order_by_pt (Particle *, Particle *); 
 
typedef std::vector<Particle*> ParticlesVector;

void FillParticlesVector(ParticlesVector&,Float_t[],Float_t[], Float_t[], Float_t[], Int_t[], Int_t);
void FillParticlesVector(ParticlesVector&,Float_t[],Float_t[], Float_t[], Float_t[], Float_t[], Int_t);
void FillParticlesVector(ParticlesVector&,Float_t[],Float_t[], Float_t[], Float_t[], Int_t);

void FillAditionalInfo(ParticlesVector&,Float_t[], Float_t[], Float_t[], Float_t[], Float_t[], Int_t);
void Fillbpvz(ParticlesVector&, Float_t[], Int_t);

bool truep(Particle *);
#endif // #ifdef ROOTOscar_cxx
