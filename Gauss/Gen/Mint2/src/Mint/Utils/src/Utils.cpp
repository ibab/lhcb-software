// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/Utils.h"

using namespace MINT;

std::ostream& operator<<(std::ostream& out, const TLorentzVector& v){
  out << "(";
  for(int i=0; i<3; i++) out << v[i] << ", ";
  out << v[3] << ")";
  return out;
}

std::string MINT::stringtime(double ddt){
  int dt = (int) ddt;
  std::string str;
  int sec = 1;
  int min = sec*60;
  int h   = min*60;
  if(dt >= h)   str += MINT::anythingToString( (int) (dt/h) ) + " h ";
  if(dt >= min) str += MINT::anythingToString( (int) (dt%h)/min ) + " min ";
  str               += MINT::anythingToString(dt%min) + " s";
  return str;
}

int MINT::nearestInt(double f){
  if(f >= 0) return ((int)(f+0.5));
  
  f *= -1;
  
  return - ((int)(f+0.5));
}

bool MINT::A_is_in_B(const std::string& a, const std::string& b){
  unsigned int pos = b.find(a);
  return pos < b.size();
}

// need to move thise to Dalitz fitter:

int MINT::LeviCita(int a, int b){
  std::vector<int> v(2);
  v[0]=a; v[1]=b;
  return MINT::LeviCita(v);
}
int MINT::LeviCita(int a, int b, int c){
  std::vector<int> v(3);
  v[0]=a; v[1]=b; v[2]=c;
  return MINT::LeviCita(v);
}
int MINT::LeviCita(int a, int b, int c, int d){
  std::vector<int> v(4);
  v[0]=a; v[1]=b; v[2]=c; v[3]=d;
  return MINT::LeviCita(v);
}
int MINT::LeviCita(int a, int b, int c, int d, int e){
  std::vector<int> v(5);
  v[0]=a; v[1]=b; v[2]=c; v[3]=d; v[4]=e;
  return MINT::LeviCita(v);
}

int MINT::LeviCita(const std::vector<int>& v){
  if(v.size() <=1) return 1;
  
  std::vector<int> checkMultiples(v.size(), 0);
  int p=1;
  for(unsigned int i=0; i<v.size(); i++){
    if(v[i] < 0 || v[i] >= (int) v.size()) return 0;
    if(++(checkMultiples[i]) > 1) return 0;
  }
  for(unsigned int i=0; i<v.size()-1; i++){
    for(unsigned int j=i+1; j < v.size(); j++){
      if(v[i] > v[j]) p*= -1;
    }
  }
  return p;
}

//
