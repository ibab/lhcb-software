#include <iostream>
#include <iomanip>
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlMat.h"
#include "AlignKernel/AlSymMat.h"
#include <string.h>

AlVec::AlVec() {

  Nele = 0;
  ptr_data = 0;  // set pointer to null

}


AlVec::AlVec(size_t N) {

  Nele = N;
  ptr_data = new double[Nele];
  for(size_t i=0;i<Nele;i++) *(ptr_data+i)=0.;
}

AlVec::AlVec(const AlVec& v) {

  Nele = v.Nele;
  ptr_data = new double[Nele]; 
  memcpy(ptr_data, v.ptr_data, Nele*sizeof(double)) ;
}

AlVec::~AlVec()
{delete [] ptr_data;}


double& AlVec::operator[](size_t i) {

  if( i >= Nele ) { std::cerr << "AlVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

const double& AlVec::operator[](size_t i) const {

  if( i >= Nele ) { std::cerr << "AlVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

AlVec& AlVec::operator=(const AlVec& v) 
{
  if( size() != v.size() ) {
    delete[] ptr_data ;
    Nele=v.Nele ;
    ptr_data = new double[Nele];
  }
  memcpy(ptr_data, v.ptr_data, Nele*sizeof(double)) ;
  return *this;
}

AlVec AlVec::operator+(const AlVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }; 
  AlVec b(Nele);

  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

AlVec AlVec::operator+(const AlVec& v) const {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }; 
  AlVec b(Nele);
 
  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

AlVec& AlVec::operator+=(const AlVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }; 

 for (size_t i=0;i<Nele;i++) *(ptr_data+i)+=v[i];
  return *this;

}

AlVec  AlVec::operator-(const AlVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  AlVec b(Nele);

  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

AlVec  AlVec::operator-(const AlVec& v) const{

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  AlVec b(Nele);
 
  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

AlVec& AlVec::operator-=(const AlVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }



 for (size_t i=0;i<Nele;i++) *(ptr_data+i)-=v[i];
  return *this;

}

double AlVec::operator*(const AlVec& v) {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (size_t i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}

double AlVec::operator*(const AlVec& v) const {
  double  b=0.;
  if( Nele != v.Nele ) { std::cerr << "scalar product: vectors size does not match!" << std::endl; return b; }

 for (size_t i=0;i<Nele;i++) b += *(ptr_data+i)*v[i];
  return b;
}

AlVec  AlVec::operator*(const AlMat& m) {

  if (Nele != m.Nrow) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Ncol);

  for (size_t i=0;i<m.Ncol;i++) {
     for (size_t j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlMat& m) const {

  if (Nele != m.Nrow) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Ncol);

  for (size_t i=0;i<m.Ncol;i++) {
     for (size_t j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlSymMat& m) {

  if (Nele != m.Size) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Size);

  for (size_t i=0;i<m.Size;i++) {
     for (size_t j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec  AlVec::operator*(const AlSymMat& m) const {

  if (Nele != m.Size) { std::cerr << "Left hand vector-matrix multiplication: size does not match!" << std::endl; return *this; }

  AlVec b(m.Size);

  for (size_t i=0;i<m.Size;i++) {
     for (size_t j=0;j<Nele;j++) b[i]+= *(ptr_data+j)*m[j][i];
  }

  return b;
}

AlVec& AlVec::operator*=(const double& d) {

  for (size_t i=0;i<Nele;i++) *(ptr_data+i)*= d;

  return *this;

}

AlVec AlVec::operator*(const double& d) {
  AlVec b(Nele);
  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i)*d;
  return b;
}

AlVec AlVec::operator*(const double& d) const {
  AlVec b(Nele);
  for (size_t i=0;i<Nele;i++) b[i] = *(ptr_data+i)*d;
  return b;
}


void AlVec::reSize(size_t Nnew) {

 
  if ( Nnew != Nele && Nele!=0) {
    double*  p = ptr_data;
    size_t Nele_old = Nele;
    ptr_data = new double[Nnew];
    Nele = Nnew;
    size_t k = Nele <= Nele_old ? Nele : Nele_old;

    p += k;
    double*  q = ptr_data + k;
    while (q > ptr_data) *(--q) = *(--p);

    delete [] p;
  }

  if(Nele==0) *this = AlVec(Nnew);
}

void AlVec::removeElement(size_t Nr) {

  //reorganize vector by filling holes, etc...
  for(size_t i=Nr;i<Nele-1;i++) (*this)[i] = (*this)[i+1];  
  
  //delete obsolete space and shrink vector size by one unit
  delete (ptr_data+Nele-1);
  Nele--;
}

void AlVec::ShrinkDofs(std::vector<bool>& mask) {

  if (mask.size() != Nele) { std::cerr<<"Beware, mask size different from vector size, skip..."<<std::endl; return;
  }

  size_t offset = 0;
  
  for (size_t iR=0;iR<mask.size();iR++) {
    if(!mask[iR]) {
      removeElement(iR-offset);
      offset++;
    }
  }
  
 
 
}
