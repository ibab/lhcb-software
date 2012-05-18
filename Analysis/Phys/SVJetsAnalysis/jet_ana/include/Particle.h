
class Particle:public TLorentzVector{

public:
Particle(float px_tmp=0, float py_tmp=0, float pz_tmp=0, float e_tmp=0,int pdgid_tmp=0){
	SetPxPyPzE(px_tmp,py_tmp,pz_tmp,e_tmp);
	momentum.SetPxPyPzE(px_tmp,py_tmp,pz_tmp,e_tmp);
	pdgid = pdgid_tmp;
}

int pdgid;
TLorentzVector momentum;

std::vector<Particle *> daughters_vector;

void add_daughter(Particle* tmp){
daughters_vector.push_back(tmp);
}

bool isInAcc(){return this->Pz() > 0 ? (Abs(this->Px()/this->Pz()) < Tan(0.3) && Abs(this->Py()/this->Pz()) < Tan(0.25)) ? true : false : false;}
};

typedef std::vector<Particle*> ParticlesVector;

//typedef std::vector<Particle*> ParticlesVector;
bool order_by_e (Particle *ip, Particle *yp) { return (ip->E()> yp->E()); }
bool order_by_pt (Particle *ip, Particle *yp) { return (ip->Pt()> yp->Pt()); }
 

void FillParticlesVector(ParticlesVector &vec,Float_t px[], Float_t py[], Float_t pz[], Float_t e[], Int_t pdgid[], Int_t maxindex){

}

