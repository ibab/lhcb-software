#include <iostream>
#include <iomanip>
#include "AlVec.h"
#include "AlMat.h"
#include "AlSymMat.h"

AlVec::AlVec() {

  Nele = 0;
  ptr_data = 0;  // set pointer to null

}


AlVec::AlVec(int N) {

  Nele = N;
  ptr_data = new double[Nele];
  for(int i=0;i<Nele;i++) *(ptr_data+i)=0.;
}

AlVec::AlVec(const AlVec& v) {

  Nele = v.Nele;
  ptr_data = new double[Nele]; 
  for(int i=0;i<Nele;i++) *(ptr_data+i)=v[i];

}

AlVec::~AlVec()
{delete [] ptr_data;}


double& AlVec::operator[](int i) {

  if( i < 0 ) { std::cerr << "AlVec: Index < zero! " << std::endl; return ptr_data[0]; }; 
  if( i >= Nele ) { std::cerr << "AlVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

const double& AlVec::operator[](int i) const {

  if( i < 0 ) { std::cerr << "AlVec: Index < zero! " << std::endl; return ptr_data[0]; }; 
  if( i >= Nele ) { std::cerr << "AlVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

AlVec& AlVec::operator=(const AlVec& v) {

  if(Nele!=0 && Nele!=v.Nele) {
std::cerr << "AlVec Assignment: size does not match!" << std::endl; return *this;
  }
 
  if ( ptr_data != v.ptr_data ) {
    reSize(v.Nele);
    for(int i=0;i<Nele;i++) *(ptr_data+i) = v[i];
  }

  return *this;

}

AlVec AlVec::operator+(const AlVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }; 
  AlVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

AlVec AlVec::operator+(const AlVec& v) const {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }; 
  AlVec b(Nele);
 
  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

AlVec& AlVec::operator+=(const AlVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }; 

 for (int i=0;i<Nele;i++) *(ptr_data+i)+=v[i];
  return *this;

}

AlVec  AlVec::operator-(const AlVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  AlVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

AlVec  AlVec::operator-(const AlVec& v) const{

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  AlVec b(Nele);
 
  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

AlVec& AlVec::operator-=(const AlVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }



 for (int i=0;i<Nele;i++) *(ptr_data+i)-=v[i];
  return *this;

}

double AlVec::operator*(const AlVec& v) {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (int i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}

double AlVec::operator*(const AlVec& v) const {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (int i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}

AlVec  AlVec::operator*(const AlMat& m) {

  if (Nele != m.Nrow) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Ncol);

  for (int i=0;i<m.Ncol;i++) {
     for (int j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlMat& m) const {

  if (Nele != m.Nrow) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Ncol);

  for (int i=0;i<m.Ncol;i++) {
     for (int j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlSymMat& m) {

  if (Nele != m.Size) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Size);

  for (int i=0;i<m.Size;i++) {
     for (int j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlSymMat& m) const {

  if (Nele != m.Size) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Size);

  for (int i=0;i<m.Size;i++) {
     for (int j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec& AlVec::operator*=(const double& d) {

  for (int i=0;i<Nele;i++) *(ptr_data+i)*= d;

  return *this;

}


void AlVec::reSize(int Nnew) {

 
  if ( Nnew>=0 && Nnew != Nele ) {
    double*  p = ptr_data;
    int Nele_old = Nele;
    ptr_data = new double[Nnew];
    Nele = Nnew;
    int k = Nele <= Nele_old ? Nele : Nele_old;

    p += k;
    double*  q = ptr_data + k;
    while (q > ptr_data) *(--q) = *(--p);

    delete [] p;
  }



}

int AlVec::n_elem() const {

  return Nele;

}
