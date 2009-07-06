#include <iostream>
#include <iomanip>
#include "AlignKernel/AlMat.h"
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"

AlMat::AlMat() {

  Ncol = 0; Nrow = 0; Nele = 0; 
  ptr_data = 0;  // set pointer to null

}


AlMat::AlMat(size_t N, size_t M) {

  Nrow = N; Ncol = M; Nele = N*M; 
  ptr_data = new double[Nele];

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

      *(ptr_data+i*Ncol+j) = 0.;

    } 
  }

}

AlMat::AlMat(const AlMat& m) {
  Nrow = m.Nrow; Ncol = m.Ncol; Nele = Nrow*Ncol; 
  ptr_data = new double[Nele];
  copy(m);
}


AlMat::~AlMat()
{delete [] ptr_data;}


void AlMat::copy(const AlMat&  m) {

  size_t n = Nrow <= m.Nrow ? Nrow : m.Nrow;
  size_t l = Ncol <= m.Ncol ? Ncol : m.Ncol;

  for( size_t i=0; i<n; i++ ) {
    for( size_t j=0; j<l; j++ ) {

      *(ptr_data+i*Ncol+j) = *(m.ptr_data+i*m.Ncol+j);

    } 
  }

}

void AlMat::copyS(const AlSymMat&  m) {

  size_t n = Nrow <= m.Size ? Nrow : m.Size;
  size_t l = Ncol <= m.Size ? Ncol : m.Size;

  for( size_t i=0; i<n; i++ ) {
    for( size_t j=0; j<l; j++ ) {

      if( j<=i ) {
	*(ptr_data+i*Ncol+j) = *(m.ptr_data+(i+1)*i/2+j);
      }
      else {
	*(ptr_data+i*Ncol+j) = *(m.ptr_data+(j+1)*j/2+i);
      }

    }
  }

}


AlMat&  AlMat::operator=(const AlMat& m) {

  if(( Nrow!=0 || Ncol!=0) && (Nrow != m.Nrow || Ncol != m.Ncol ))
     { std::cerr << "AlMat AlMat Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptr_data ) {
    reSize(m.Nrow, m.Ncol);
    copy(m);
  };
  return (*this);

}

AlMat&  AlMat::operator=(const AlSymMat& m) {

  if( ( Nrow!=0 || Ncol!=0) && (Nrow != m.Size || Ncol != m.Size ))
      { std::cerr << "AlMat AlSymMat Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptr_data ) {
    reSize(m.Size, m.Size);
    copyS(m);
  }
  return (*this);
}

AlMat AlMat::operator+(const AlMat& m) {

  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

  AlMat b( Nrow, Ncol );

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)+m[i][j];

    }
  }

  return b;
} 

AlMat AlMat::operator+(const AlMat& m) const {

  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

  AlMat b( Nrow, Ncol );

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)+m[i][j];

    }
  }

  return b;
} 

AlMat&  AlMat::operator+=(const AlMat& m) {

 if( Nrow != m.Nrow || Ncol != m.Ncol )
   { std::cerr << "operator+=: size do not match!" << std::endl; return *this; }

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

     *(ptr_data+i*Ncol+j)  += *(m.ptr_data+i*m.Ncol+j);

    };
  };

  return *this;
}

AlMat   AlMat::operator-(const AlMat& m) {
  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator-: size do not match!" << std::endl; return *this; }

  AlMat b( Nrow, Ncol );

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)-m[i][j];

    }
  }

  return b;
}

AlMat AlMat::operator-(const AlMat& m) const {
  if( Nrow != m.Nrow || Ncol != m.Ncol )
    { std::cerr << "operator-: size do not match!" << std::endl; return *this; }

  AlMat b( Nrow, Ncol );

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

      *(b.ptr_data+i*b.Ncol+j) = *(ptr_data+i*Ncol+j)-m[i][j];

    }
  }

  return b;
}

AlMat&  AlMat::operator-=(const AlMat& m) {

 if( Nrow != m.Nrow || Ncol != m.Ncol )
   { std::cerr << "operator-=: size do not match!" << std::endl; return *this; }

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<Ncol; j++ ) {

     *(ptr_data+i*Ncol+j)  -= *(m.ptr_data+i*m.Ncol+j);

    };
  };

  return *this;
}

AlMat AlMat::operator*(const AlMat& m) {

  if( Ncol != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  AlMat b( Nrow, m.Ncol);


  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<m.Ncol; j++ ) {
      for( size_t k=0; k<m.Nrow; k++) b[i][j]+= *(ptr_data+i*Ncol+k)*(m[k][j]);
    }
  }

  return b;

}

AlMat AlMat::operator*(const AlMat& m) const {

  if( Ncol != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  AlMat b( Nrow, m.Ncol);


  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<m.Ncol; j++ ) {
      for( size_t k=0; k<m.Nrow; k++) b[i][j]+= *(ptr_data+i*Ncol+k)*(m[k][j]);
    }
  }

  return b;

}

AlMat AlMat::operator*(const AlSymMat& m) {
  if( Ncol != m.Size) {  std::cerr << "operator*: size do not match!" << std::endl;
  return *this; }

  AlMat b( Nrow, m.Size);

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<m.Size; j++ ) {
      for( size_t k=0; k<m.Size; k++) *(b.ptr_data+i*b.Ncol+j) += *(ptr_data+i*Ncol+k)* m[k][j];
    }
  }
  return b;

}

AlMat AlMat::operator*(const AlSymMat& m) const {
  if( Ncol != m.Size) {  std::cerr << "operator*: size do not match!" << std::endl;
  return *this; }

  AlMat b( Nrow, m.Size);

  for( size_t i=0; i<Nrow; i++ ) {
    for( size_t j=0; j<m.Size; j++ ) {
      for( size_t k=0; k<m.Size; k++) *(b.ptr_data+i*b.Ncol+j) += *(ptr_data+i*Ncol+k)* m[k][j];
    }
  }
  return b;

}

AlVec AlMat::operator*(const AlVec& v) {

 if( Ncol != v.Nele )
    { std::cerr << "operator*: size do not match! " << std::endl; return v; }

  AlVec b(Nrow);

  
  for( size_t i=0; i<Nrow; i++ ) {
  
    for( size_t j=0; j<Ncol; j++ ) b[i] += (*(ptr_data+i*Ncol+j))*(v[j]);
     
  }

  return b;

}

AlVec AlMat::operator*(const AlVec& v) const {

 if( Ncol != v.Nele )
    { std::cerr << "operator*: size do not match! " << std::endl; return v; }

  AlVec b(Nrow);

  
  for( size_t i=0; i<Nrow; i++ ) {
  
    for( size_t j=0; j<Ncol; j++ ) b[i] += (*(ptr_data+i*Ncol+j))*(v[j]);
     
  }

  return b;

}

AlMat&   AlMat::operator*=(const double& d) {

 double*  p = ptr_data+Nele;

  while (p > ptr_data) *(--p) *= d;

  return *this;

}



// transposition


AlMat AlMat::T() const {
  AlMat b(Ncol,Nrow);
  for( size_t i=0; i<b.Nrow; i++ ) {
    for( size_t j=0; j<b.Ncol; j++ ) {
       b[i][j]= *(ptr_data+j*Ncol+i);
    }
  }
  return b;
}




//invert sym matrix declared as non-symetric for convenience

void AlMat::invertS(int& ierr, double Norm = 1.) {

  if(Nrow!=Ncol) { std::cerr << "AlMat invertS: non-square matrix!" << std::endl; return;}

  AlSymMat b(Nrow);
  //  double minelem=99999999.;
  // double maxelem=0.;

  for( size_t i=0; i<b.Size; i++ ) {
    for( size_t j=0; j<=i; j++ ) {

      b[i][j] = (*this)[i][j];
      //   if (fabs(b[i][j])<fabs(minelem)) minelem=b[i][j];
      //  if (fabs(b[i][j])>fabs(maxelem)) maxelem=b[i][j];      

    }
  }

  //proceed to regularization before and after inversion, if needed

  
  //  std::cout<<" >>>>> MAXELEM: "<<maxelem<<std::endl;
  //    Norm = 1./maxelem;
  //  std::cout<<" >>>>> NORM: "<<Norm<<std::endl;
  /*   if ((log(fabs(maxelem)))<0.) {
     size_t factor = size_t(log(fabs(maxelem)))-1;
     Norm=1./1e(factor);
     }*/
  b*=Norm;

  b.invert(ierr);
  b*=Norm;

  if (ierr==0) (*this).copyS(b);

}

// reSize

void AlMat::reSize(size_t Nnew, size_t Mnew) {

 
  if ( Nnew != Nrow || Mnew != Ncol ) {

    double*  p = ptr_data;
    Nele = Nnew*Mnew;
    ptr_data = new double[Nele];
    size_t Nrow_old = Nrow;
    size_t Ncol_old = Ncol;

    Nrow = Nnew;
    Ncol = Mnew;
    size_t k = Nrow <= Nrow_old ? Nrow : Nrow_old;
    size_t l = Ncol <= Ncol_old ? Ncol : Ncol_old;

    for( size_t i=0; i<k; i++ ) {
      for( size_t j=0; j<l; j++ ) {
        *(ptr_data+i*Ncol+j) = *(p+i*Ncol_old+j);
      }
    }


    delete [] p;
  }



}


void AlMat::removeRow(size_t Nr) {

  //reorganize matrix by filling holes, etc...
  for(size_t i=Nr;i<Nrow-1;i++) {
    for(size_t j=0;j<Ncol;j++) {      
      (*this)[i][j] = (*this)[i+1][j];
    }    
  }
  //delete obsolete space and shrink matrix row size by one unit
  size_t max=Nrow-1;
  for (size_t col=0;col<Ncol;col++) delete (ptr_data+max*Ncol+col);
  
  Nrow--;
}

void AlMat::removeCol(size_t Nr) {

  //reorganize matrix by filling holes, etc...
  for(size_t i=0;i<Nrow;i++) {
    for(size_t j=Nr;j<Ncol-1;j++) {      
      (*this)[i][j] = (*this)[i][j+1];
    }    
  }
  //delete obsolete space and shrink matrix column size by one unit
  size_t max=Ncol-1;
  for (size_t row=0;row<Nrow;row++) delete (ptr_data+row*Ncol+max);
  
  Ncol--; 
}




size_t AlMat::nrow() const {

  return Nrow;

}

size_t AlMat::ncol() const {

  return Ncol;

}

