#include <iostream>
#include <iomanip>
#include <math.h>
#include <float.h> //for DBL_EPSILON
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_eigen.h"
//#include <Nag/nag.h>
//#include <Nag/nagf03.h>
#include "AlSymMat.h"
#include "AlVec.h"
#include "AlMat.h"

extern "C" {
  void dsptrf_(char*, const int*, double*, int*, int* );
}

extern "C" {
  void dsptri_(char*, const int*, double*, int*, double*, int* );
  //  void f03aaf_(double*,int,int,double,double*,int);
  //  void dgetrf_(int,int,double*,int,int*,int);

}

extern "C" {
void dspev_(char*, char*, const int*, double*, double*, double*, const int*, double*, int* );
}




AlSymMat::AlSymMat() {
  Size = 0; Nele = 0;
  ptr_data = 0;  // set pointer to null
}


AlSymMat::AlSymMat(int N) {

  Size = N; Nele = N*(N+1)/2;
  ptr_data = new double[Nele];

 for( int i=0; i<Size; i++ ) {
   for( int j=0; j<=i; j++ ) {

     *(ptr_data+(i+1)*i/2+j)=0.;

   }
 }

}

AlSymMat::AlSymMat(const AlSymMat& m) {

  Size=m.Size; Nele = m.Nele; 
  ptr_data = new double[Nele];
  copy(m);

}

AlSymMat::~AlSymMat()
{delete [] ptr_data;}

void AlSymMat::copy(const AlSymMat&  m) {

  int n = Size <= m.Size ? Size : m.Size;

  for( int i=0; i<n; i++ ) {
    for( int j=0; j<=i; j++ ) {

      *(ptr_data+(i+1)*i/2+j) = m[i][j];
    }
  }

}

AlSymMat&  AlSymMat::operator=(const AlSymMat& m) {

 if( Size!=0 && Size != m.Size)
   { std::cerr << "AlSymMat-AlSymMat Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptr_data ) {
    reSize(m.Size);

    copy(m);

  }

  return *this;

}

AlSymMat AlSymMat::operator+(const AlSymMat& m) {

if( Size != m.Size) { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

 AlSymMat b(Size);

 for( int i=0; i<Size; i++ ) {
   for( int j=0; j<=i; j++ ) {

     b[i][j] = *(ptr_data+(i+1)*i/2+j) + m[i][j];

   }
 }

 return b;

} 

AlSymMat AlSymMat::operator+(const AlSymMat& m) const{

if( Size != m.Size) { std::cerr << "operator+: size do not match!" << std::endl; return *this; }

 AlSymMat b(Size);

 for( int i=0; i<Size; i++ ) {
   for( int j=0; j<=i; j++ ) {

     b[i][j] = *(ptr_data+(i+1)*i/2+j) + m[i][j];

   }
 }

 return b;

} 
 
AlSymMat&  AlSymMat::operator+=(const AlSymMat& m) {

 if( Size != m.Size)
   { std::cerr << "operator+=: size do not match!" << std::endl; return *this; }

  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<=i; j++ ) {

     *(ptr_data+(i+1)*i/2+j)  += m[i][j];

    };
  };

  return *this;
}

AlSymMat AlSymMat::operator-(const AlSymMat& m) {

if( Size != m.Size) { std::cerr << "operator-: size do not match!" << std::endl; return *this; }


 AlSymMat b(Size);

 for( int i=0; i<Size; i++ ) {
   for( int j=0; j<=i; j++ ) {

     b[i][j] = *(ptr_data+(i+1)*i/2+j) - m[i][j];

   }
 }



 return b;

}

AlSymMat AlSymMat::operator-(const AlSymMat& m) const {

if( Size != m.Size) { std::cerr << "operator-: size do not match!" << std::endl; return *this; }


 AlSymMat b(Size);

 for( int i=0; i<Size; i++ ) {
   for( int j=0; j<=i; j++ ) {

     b[i][j] = *(ptr_data+(i+1)*i/2+j) - m[i][j];

   }
 }



 return b;

}

AlMat AlSymMat::operator*(const AlSymMat& m) {

  if( Size != m.Size ) {  std::cerr << "operator*: size do not match!" << std::endl;

  AlMat b( Size, m.Size);

  return b; }

  AlMat b( Size, Size);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<Size; j++ ) {
      for( int k=0; k<Size; k++) {
	if (k<=i) b[i][j]+= *(ptr_data+(i+1)*i/2+k)*(m[k][j]);
	if (k>i)  b[i][j]+= *(ptr_data+(k+1)*k/2+i)*(m[k][j]);
      }
    }
  }

  return b;

}

AlMat AlSymMat::operator*(const AlSymMat& m) const {

  if( Size != m.Size ) {  std::cerr << "operator*: size do not match!" << std::endl;

  AlMat b( Size, m.Size);

  return b; }

  AlMat b( Size, Size);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<Size; j++ ) {
      for( int k=0; k<Size; k++) {
	if (k<=i) b[i][j]+= *(ptr_data+(i+1)*i/2+k)*(m[k][j]);
	if (k>i)  b[i][j]+= *(ptr_data+(k+1)*k/2+i)*(m[k][j]);
      }
    }
  }

  return b;

}

AlMat  AlSymMat::operator*(const AlMat& m) {

 if( Size != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  AlMat b( Size, m.Ncol);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<m.Ncol; j++ ) {
      for( int k=0; k<Size; k++) {
	if (k<=i) b[i][j]+= *(ptr_data+(i+1)*i/2+k)*(m[k][j]);
	if (k>i)  b[i][j]+= *(ptr_data+(k+1)*k/2+i)*(m[k][j]);
      }
    }
  }

  return b;

}

AlMat  AlSymMat::operator*(const AlMat& m) const {

 if( Size != m.Nrow ) {  std::cerr << "operator*: size do not match!" << std::endl;
  return m; }

  AlMat b( Size, m.Ncol);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<m.Ncol; j++ ) {
      for( int k=0; k<Size; k++) {
	if (k<=i) b[i][j]+= *(ptr_data+(i+1)*i/2+k)*(m[k][j]);
	if (k>i)  b[i][j]+= *(ptr_data+(k+1)*k/2+i)*(m[k][j]);
      }
    }
  }

  return b;

}

AlVec AlSymMat::operator*(const AlVec& v) {

 if( Size != v.Nele )
    { std::cerr << "operator*: size do not match! " << std::endl; return v; }

  AlVec b(Size);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<Size; j++ ) { 
      if (j<=i) b[i] += *(ptr_data+(i+1)*i/2+j)*v[j];
      if (j>i) b[i] += *(ptr_data+(j+1)*j/2+i)*v[j];
    }
  }

  return b;

}

AlVec AlSymMat::operator*(const AlVec& v) const {

 if( Size != v.Nele )
    { std::cerr << "operator*: size do not match! " << std::endl; return v; }

  AlVec b(Size);


  for( int i=0; i<Size; i++ ) {
    for( int j=0; j<Size; j++ ) { 
      if (j<=i) b[i] += *(ptr_data+(i+1)*i/2+j)*v[j];
      if (j>i) b[i] += *(ptr_data+(j+1)*j/2+i)*v[j];
    }
  }

  return b;

}


AlSymMat&  AlSymMat::operator*=(const double& d) {

 double*  p = ptr_data+Nele;

  while (p > ptr_data) *(--p) *= d;

  return *this;
}






void AlSymMat::reSize(int Nnew) {

  if ( Nnew != Size ) {

    double*  p = ptr_data;
    Nele = Nnew*(Nnew+1)/2;
    ptr_data = new double[Nele];
    int Size_old = Size;
    Size = Nnew;
    int k = Size <= Size_old ? Size : Size_old;
    for( int i=0; i<k; i++ ) {
      for( int j=0; j<=i; j++ ) {
        *(ptr_data+(i+1)*i/2+j) = *(p+(i+1)*i/2+j);
      }
    }
  
    delete [] p;
  }


}


void AlSymMat::invert(int& ierr) {

  // add a protection to detect singular matrices:
   
  double det = (*this).determinant();
  double detcut = 1.E-20;

  if (fabs(det)>detcut) {

  char  uplo = 'U';
  int   N = Size;
  int*    ipiv =  new int[N];
  double* work =  new double[N];
  double* mx = ptr_data;

  dsptrf_(&uplo, &N, mx, ipiv, &ierr);

  dsptri_(&uplo, &N, mx, ipiv, work, &ierr);

  delete [] ipiv;
  delete [] work;

  } else {


    ierr=999;

  }




}

double AlSymMat::determinant() {

 double deter = 1.;

  // get determinant using LU decomposition + Crout algorithm

  AlMat A(Size,Size);
  A.copyS(*this);

  double AMAX,DUM,SUM;
  int CODE;
  double TINY = 1.E-20;
  double D;
  int N=Size;





  D=1.; CODE=0;

  for(int I=0;I<N;I++) {
   AMAX=0.;
   for(int J=0;J<N;J++) {
     if (fabs(A[I][J])>AMAX) AMAX=fabs(A[I][J]);
   }
   if(AMAX<TINY) {
     CODE = 1; //matrix is singular
     return 0;
   }
 
  }

 for(int J=0;J<N;J++) {

 

   for(int I=0;I<N;I++) {

     SUM = A[I][J];
     int KMAX = (I<J)?I:J;
     for(int K=0;K<KMAX;K++)
       SUM = SUM - A[I][K]*A[K][J]; 
     A[I][J]=SUM;
   }

   //find pivot and exchange if necessary

   int IMAX=J;
   for (int I=J+1;I<N;I++) {
     if(fabs(A[I][J])>fabs(A[IMAX][J])) IMAX=I;
   }

   if(IMAX!=J) {
     for(int K=0;K<N;K++) {
       DUM=A[IMAX][K];
       A[IMAX][K]=A[J][K];
       A[J][K]=DUM;
     }
     D=-D;
   }

   


   if(J<N && (fabs(A[J][J])>TINY)) {

     DUM = 1./ A[J][J];
     for(int I=J+1;I<N;I++) A[I][J] = A[I][J]*DUM;
   
   }

 }

 if(CODE==0) {
   deter=deter*D;
   for(int I=0;I<N;I++) {
     deter = deter*A[I][I];
   }
   return deter;

 } else {
   return 0;
 }

 

}

int AlSymMat::size() const {

  return Size;

}


int AlSymMat::diagonalize(char jobz, AlVec& w, AlMat& z) {



  int   info = 0;
  char  uplo = 'U';
  int   N = Size;
  double* work =  new double[3*N];

  double* ap = ptr_data;

 

    dspev_(&jobz, &uplo, &N, ap, w.ptr_data, z.ptr_data, &N, work, &info);


  delete [] work;

  return info;

}

void AlSymMat::diagonalize_GSL(AlVec& egval, AlMat& egvec) 
{

  int N = Size;
  
  std::cout<<" Debug...before gsl_matrix_alloc"<<std::endl;
  
  gsl_matrix *m = gsl_matrix_alloc(N,N);

std::cout<<" Debug...after gsl_matrix_alloc"<<std::endl;

  

//  if (1) {
    //    AlMat V(N,N); 
    //    V.copyS((*this)); 
    //    m->data = V.ptr_data;
    for(int ig=0;ig<N;ig++) {
      for(int jg=0;jg<N;jg++) {
        gsl_matrix_set(m,ig,jg,(*this)[ig][jg]);
      }      
    }
    //    gsl_matrix_set(m,0,0,temp00);
    //  std::cout<<"gsl00: "<<gsl_matrix_get(m,0,0)<<std::endl;
    
    // }

  std::cout<<"Debut....printing gsl matrix elements"<<std::endl;
  
  for(int ig=0;ig<N;ig++) {
    for(int jg=0;jg<N;jg++) {
      std::cout<<gsl_matrix_get(m,ig,jg)<<"   ";
    }
    std::cout<<std::endl;
    
  }
  

std::cout<<" Debug...after copy ptr data"<<std::endl;

  gsl_vector *eval = gsl_vector_alloc(N);
  gsl_matrix *evec = gsl_matrix_alloc(N,N);

  gsl_eigen_symmv_workspace* w = gsl_eigen_symmv_alloc(N);
  gsl_eigen_symmv(m,eval,evec,w);

  gsl_eigen_symmv_free(w);

std::cout<<" Debug...after diagonalization"<<std::endl;
  
  //sort eigen values/eigen vectors in ascending order
  gsl_eigen_symmv_sort(eval,evec,GSL_EIGEN_SORT_VAL_ASC);

   egval.ptr_data = eval->data;
   egvec.ptr_data = evec->data;  

   // for(int ig=0;ig<N;ig++) {
    //  egval[ig]=gsl_vector_get(eval,ig);
   // for(int jg=0;jg<N;jg++) {
   //    egvec[ig][jg]=gsl_matrix_get(evec,ig,jg);
   //  }      
   //}
  
}
