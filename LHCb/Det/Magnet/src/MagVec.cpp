#include <iostream>
#include <iomanip>
#include "MagVec.h"


MagVec::MagVec() {

  Nele = 0;
  ptr_data = 0;  // set pointer to null

}


MagVec::MagVec(int N) {

  Nele = N;
  ptr_data = new double[Nele];
  for(int i=0;i<Nele;i++) *(ptr_data+i)=0.;
}

MagVec::MagVec(const MagVec& v) {

  Nele = v.Nele;
  ptr_data = new double[Nele];
  for(int i=0;i<Nele;i++) *(ptr_data+i)=v[i];

}

MagVec::~MagVec()
{delete [] ptr_data;}


double& MagVec::operator[](int i) {

  if( i < 0 ) { std::cerr << "MagVec: Index < zero! " << std::endl; return ptr_data[0]; };
  if( i >= Nele ) { std::cerr << "MagVec: Index too large! " << std::endl; return ptr_data[0]; };

  return  *(ptr_data+i);

}

const double& MagVec::operator[](int i) const {

  if( i < 0 ) { std::cerr << "MagVec: Index < zero! " << std::endl; return ptr_data[0]; };
  if( i >= Nele ) { std::cerr << "MagVec: Index too large! " << std::endl; return ptr_data[0]; };

  return  *(ptr_data+i);

}

MagVec& MagVec::operator=(const MagVec& v) {

  if(Nele!=0 && Nele!=v.Nele) {
std::cerr << "MagVec Assignment: size does not match!" << std::endl; return *this;
  }

  if ( ptr_data != v.ptr_data ) {
    reSize(v.Nele);
    for(int i=0;i<Nele;i++) *(ptr_data+i) = v[i];
  }

  return *this;

}

MagVec MagVec::operator+(const MagVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; };
  MagVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

MagVec MagVec::operator+(const MagVec& v) const {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; };
  MagVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

MagVec& MagVec::operator+=(const MagVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; };

 for (int i=0;i<Nele;i++) *(ptr_data+i)+=v[i];
  return *this;

}

MagVec  MagVec::operator-(const MagVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  MagVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

MagVec  MagVec::operator-(const MagVec& v) const{

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  MagVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

MagVec& MagVec::operator-=(const MagVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }



 for (int i=0;i<Nele;i++) *(ptr_data+i)-=v[i];
  return *this;

}

double MagVec::operator*(const MagVec& v) {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (int i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}

double MagVec::operator*(const MagVec& v) const {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (int i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}



MagVec& MagVec::operator*=(const double& d) {

  for (int i=0;i<Nele;i++) *(ptr_data+i)*= d;

  return *this;

}


void MagVec::reSize(int Nnew) {


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

int MagVec::n_elem() const {

  return Nele;

}
