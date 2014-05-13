#ifndef POLARIZATION_VECTOR_HH
#define POLARIZATION_VECTOR_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status:  Tu 6 May 2014 

// Spin 1 polarization vectors taken from:
// S.U. Chung, Spin Formalisms (CERN Yellow Report No. CERN 71-8, Geneva, Switzerland, 1971)

#include "TLorentzVectorC.h"
#include "TLorentzVector.h"

using namespace std;

class polVector : public TLorentzVectorC{

 public:
  
    polVector(const TLorentzVector p,const double m, const int lambda) :TLorentzVectorC(){
      if(lambda==-1){
          _v[0].SetXYZT(m+p.Px()*p.Px()/(p.E()+m),p.Px()*p.Py()/(p.E()+m),p.Px()*p.Pz()/(p.E()+m),p.Px());
          _v[0]*=1./(m*sqrt(2));
          _v[1].SetXYZT(p.Px()*p.Py()/(p.E()+m),m+p.Py()*p.Py()/(p.E()+m),p.Py()*p.Pz()/(p.E()+m),p.Py());
          _v[1]*=-1./(m*sqrt(2));      
      }
      
      else if(lambda==0){
          _v[0].SetXYZT(p.Pz()*p.Px()/(p.E()+m),p.Pz()*p.Py()/(p.E()+m),m+p.Pz()*p.Pz()/(p.E()+m),p.Pz());
          _v[0]*=1./m;
          _v[1].SetXYZT(0,0,0,0);
      }
      else if(lambda==1){
          _v[0].SetXYZT(m+p.Px()*p.Px()/(p.E()+m),p.Px()*p.Py()/(p.E()+m),p.Px()*p.Pz()/(p.E()+m),p.Px());
          _v[0]*=-1./(m*sqrt(2));
          _v[1].SetXYZT(p.Px()*p.Py()/(p.E()+m),m+p.Py()*p.Py()/(p.E()+m),p.Py()*p.Pz()/(p.E()+m),p.Py());
          _v[1]*=-1./(m*sqrt(2));      
      }
      else if(lambda==-999){
           TLorentzVectorC tmp = polVector(p,m,-1) + polVector(p,m,0) + polVector(p,m,1);
          _v[0]= tmp.Re();
          _v[1]= tmp.Im();
      }
        
      else {
          std::cout << "I can't handle spin > 1 particles. I'll set everything to 0. "  << std::endl;
          _v[0].SetXYZT(0,0,0,0);
          _v[1].SetXYZT(0,0,0,0);
      }
        
    }  
    
    polVector(const polVector& other, bool conj = false) :TLorentzVectorC(other,conj){}

    
};
#endif
//
