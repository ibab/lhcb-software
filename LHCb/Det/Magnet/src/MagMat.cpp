#include <iostream>
#include <iomanip>
#include "MagMat.h"


MagMat::MagMat() {

  Ncol = 0; Nrow = 0; Nele = 0;
  ptr_data = 0;  // set pointer to null

}


MagMat::MagMat(int N, int M) {

  Nrow = N; Ncol = M; Nele = N*M;
  ptr_data = new double[Nele];

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

      *(ptr_data+i*Ncol+j) = 0.;

    }
  }

}

MagMat::MagMat(const MagMat& m) {
  Nrow = m.Nrow; Ncol = m.Ncol; Nele = Nrow*Ncol;
  ptr_data = new double[Nele];
  copy(m);
}


MagMat::~MagMat()
{delete [] ptr_data;}


void MagMat::copy(const MagMat&  m) {

  int n = Nrow <= m.Nrow ? Nrow : m.Nrow;
  int l = Ncol <= m.Ncol ? Ncol : m.Ncol;

  for( int i=0; i<n; i++ ) {
    for( int j=0; j<l; j++ ) {

      *(ptr_data+i*Ncol+j) = *(m.ptr_data+i*m.Ncol+j);

    }
  }

}




MagMat&  MagMat::operator=(const MagMat& m) {

  if(( Nrow!=0 || Ncol!=0) && (Nrow != m.Nrow || Ncol != m.Ncol ))
     { std::cerr << "MagMat MagMat Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptr_data ) {
    reSize(m.Nrow, m.Ncol);
    copy(m);
  };
  return (*this);

}



MagMat MagMat::operator+(const MagMat& m) {

  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

  MagMat b( Nrow, Ncol );

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)+m[i][j];

    }
  }

  return b;
}

MagMat MagMat::operator+(const MagMat& m) const {

  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

  MagMat b( Nrow, Ncol );

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)+m[i][j];

    }
  }

  return b;
}

MagMat&  MagMat::operator+=(const MagMat& m) {

 if( Nrow != m.Nrow || Ncol != m.Ncol )
   { std::cerr << "operator+=: size do not match!" << std::endl; return *this; }

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

     *(ptr_data+i*Ncol+j)  += *(m.ptr_data+i*m.Ncol+j);

    };
  };

  return *this;
}

MagMat   MagMat::operator-(const MagMat& m) {
  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator-: size do not match!" << std::endl; return *this; }

  MagMat b( Nrow, Ncol );

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)-m[i][j];

    }
  }

  return b;
}

MagMat MagMat::operator-(const MagMat& m) const {
  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator-: size do not match!" << std::endl; return *this; }

  MagMat b( Nrow, Ncol );

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)-m[i][j];

    }
  }

  return b;
}

MagMat&  MagMat::operator-=(const MagMat& m) {

 if( Nrow != m.Nrow || Ncol != m.Ncol )
   { std::cerr << "operator-=: size do not match!" << std::endl; return *this; }

  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<Ncol; j++ ) {

     *(ptr_data+i*Ncol+j)  -= *(m.ptr_data+i*m.Ncol+j);

    };
  };

  return *this;
}

MagMat MagMat::operator*(const MagMat& m) {

  if( Ncol != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  MagMat b( Nrow, m.Ncol);


  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<m.Ncol; j++ ) {
      for( int k=0; k<m.Nrow; k++) b[i][j]+= *(ptr_data+i*Ncol+k)*(m[k][j]);
    }
  }

  return b;

}

MagMat MagMat::operator*(const MagMat& m) const {

  if( Ncol != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  MagMat b( Nrow, m.Ncol);


  for( int i=0; i<Nrow; i++ ) {
    for( int j=0; j<m.Ncol; j++ ) {
      for( int k=0; k<m.Nrow; k++) b[i][j]+= *(ptr_data+i*Ncol+k)*(m[k][j]);
    }
  }

  return b;

}



MagMat&   MagMat::operator*=(const double& d) {

 double*  p = ptr_data+Nele;

  while (p > ptr_data) *(--p) *= d;

  return *this;

}



// transposition


MagMat MagMat::T() const {
  MagMat b(Ncol,Nrow);
  for( int i=0; i<b.Nrow; i++ ) {
    for( int j=0; j<b.Ncol; j++ ) {
       b[i][j]= *(ptr_data+j*Ncol+i);
    }
  }
  return b;
}






// reSize

void MagMat::reSize(int Nnew, int Mnew) {


  if ( Nnew != Nrow || Mnew != Ncol ) {

    double*  p = ptr_data;
    Nele = Nnew*Mnew;
    ptr_data = new double[Nele];
    int Nrow_old = Nrow;
    int Ncol_old = Ncol;

    Nrow = Nnew;
    Ncol = Mnew;
    int k = Nrow <= Nrow_old ? Nrow : Nrow_old;
    int l = Ncol <= Ncol_old ? Ncol : Ncol_old;

    for( int i=0; i<k; i++ ) {
      for( int j=0; j<l; j++ ) {
        *(ptr_data+i*Ncol+j) = *(p+i*Ncol_old+j);
      }
    }


    delete [] p;
  }



}



int MagMat::nrow() const {

  return Nrow;

}

int MagMat::ncol() const {

  return Ncol;

}

