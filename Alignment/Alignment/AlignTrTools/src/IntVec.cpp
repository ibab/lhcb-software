#include <iostream>
#include <iomanip>
#include "IntVec.h"


IntVec::IntVec() {

  Nele = 0;
  ptr_data = 0;  // set pointer to null

}


IntVec::IntVec(int N) {

  Nele = N;
  ptr_data = new int[Nele];
  for(int i=0;i<Nele;i++) *(ptr_data+i)=0;
}

//initialize with a given integer

IntVec::IntVec(int N, int init) {

  Nele = N;
  ptr_data = new int[Nele];
  for(int i=0;i<Nele;i++) *(ptr_data+i)=init;
}

IntVec::IntVec(const IntVec& v) {

  Nele = v.Nele;
  ptr_data = new int[Nele]; 
  for(int i=0;i<Nele;i++) *(ptr_data+i)=v[i];

}

IntVec::~IntVec()
{delete [] ptr_data;}


int& IntVec::operator[](int i) {

  if( i < 0 ) { std::cerr << "IntVec: Index < zero! " << std::endl; return ptr_data[0]; }; 
  if( i >= Nele ) { std::cerr << "IntVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

const int& IntVec::operator[](int i) const {

  if( i < 0 ) { std::cerr << "IntVec: Index < zero! " << std::endl; return ptr_data[0]; }; 
  if( i >= Nele ) { std::cerr << "IntVec: Index too large! " << std::endl; return ptr_data[0]; }; 

  return  *(ptr_data+i);

}

IntVec& IntVec::operator=(const IntVec& v) {

  if(Nele!=0 && Nele!=v.Nele) {
std::cerr << "IntVec Assignment: size does not match!" << std::endl; return *this;
  }
 
  if ( ptr_data != v.ptr_data ) {
    reSize(v.Nele);
    for(int i=0;i<Nele;i++) *(ptr_data+i) = v[i];
  }

  return *this;

}

IntVec IntVec::operator+(const IntVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }; 
  IntVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) + v[i];
  return b;

}

IntVec& IntVec::operator+=(const IntVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }; 

 for (int i=0;i<Nele;i++) *(ptr_data+i)+=v[i];
  return *this;

}

IntVec  IntVec::operator-(const IntVec& v) {

  if( Nele != v.Nele ) { std::cerr << "operator+: vectors size does not match!" << std::endl; return *this; }
  IntVec b(Nele);

  for (int i=0;i<Nele;i++) b[i] = *(ptr_data+i) - v[i];
  return b;

}

IntVec& IntVec::operator-=(const IntVec& v) {

 if( Nele != v.Nele ) { std::cerr << "operator+=: vectors size does not match!" << std::endl; return *this; }


 for (int i=0;i<Nele;i++) *(ptr_data+i)-=v[i];
  return *this;

}




void IntVec::reSize(int Nnew) {

 
  if ( Nnew>=0 && Nnew != Nele ) {
    int*  p = ptr_data;
    int Nele_old = Nele;
    ptr_data = new int[Nnew];
    Nele = Nnew;
    int k = Nele <= Nele_old ? Nele : Nele_old;

    p += k;
    int*  q = ptr_data + k;
    while (q > ptr_data) *(--q) = *(--p);
 
    delete [] p;
  }



}

int IntVec::n_elem() {

  return Nele;

}
