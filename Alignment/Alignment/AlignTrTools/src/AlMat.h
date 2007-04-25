#ifndef ALIGNSOLVTOOLS_ALMAT_H
#define ALIGNSOLVTOOLS_ALMAT_H


#include <iostream>
#include <iomanip>

class AlVec;
class AlSymMat;

class AlMat

{


 public:

  AlMat(int N, int M);
  AlMat();
  AlMat(const AlMat& m);
 ~AlMat();

 

 class AlMat_row {
 public:
   inline AlMat_row(AlMat&,int);
   double & operator[](int);
 private:
   AlMat& _a;
   int _r;
 };
 class AlMat_row_const {
 public:
   inline AlMat_row_const(const AlMat&,int);
   const double & operator[](int) const;
 private:
   const AlMat& _a;
   int _r;
 };
 // helper class to implement m[i][j]

 inline AlMat_row operator[] (int);
 inline AlMat_row_const operator[] (int) const;

 AlMat&  operator=(const AlMat& m);
 AlMat&  operator=(const AlSymMat& m);
 AlMat   operator+(const AlMat& m);  
 AlMat   operator+(const AlMat& m) const;  
 AlMat&  operator+=(const AlMat& m);
 AlMat   operator-(const AlMat& m);
 AlMat   operator-(const AlMat& m) const;
 AlMat&   operator-=(const AlMat& m);
 AlMat   operator*(const AlMat& m);
 AlMat   operator*(const AlMat& m) const;
 AlMat   operator*(const AlSymMat& m);
 AlMat   operator*(const AlSymMat& m) const;
 AlVec   operator*(const AlVec& v);
 AlVec   operator*(const AlVec& v) const;
 AlMat&   operator*=(const double& d);

 // transposed matrix
 AlMat T() const;
 //
//invert sym matrix declared as non-symetric for convenience

 void invertS(int& ierr, double Norm);


 int nrow() const;
 int ncol() const;
 // reSize method
 void reSize (int Nnew, int Mnew);

 
  void copyS(const AlSymMat&  m);

  //  void invert(int ierr); // only if Ncol=Nrow !!

 private:

  friend class AlVec;
  friend class AlSymMat;
  friend class AlMat_row;
  friend class AlMat_row_const;

  friend class AlignSolvTool;

  void copy(const AlMat&  m);
  //  void copyS(const AlSymMat&  m);

  int Ncol, Nrow;
  int Nele;
  double* ptr_data;

  //  double* mat_p;
  // double** mat;

};

// inline operators

inline AlMat::AlMat_row AlMat::operator[] (int r)
{
  AlMat_row b(*this,r);
  return b;
}

inline AlMat::AlMat_row_const AlMat::operator[] (int r) const

{
  const AlMat_row_const b(*this,r);

  return b;
}

inline double &AlMat::AlMat_row::operator[](int c)
{
   if(_r<0||_r>=_a.Nrow || c<0||c>=_a.Ncol)
      std::cerr << "Range error in AlMat::operator[][]" << std::endl;

   return *(_a.ptr_data+_r*_a.Ncol+c);

}

inline const double & AlMat::AlMat_row_const::operator[](int c) const 
{

   if(_r<0||_r>=_a.Nrow || c<0||c>=_a.Ncol)
     std::cerr << "Range error in AlMat::operator[][]" << std::endl;
   
   return *(_a.ptr_data+_r*_a.Ncol+c);
 
}

inline AlMat::AlMat_row::AlMat_row(AlMat &a,int r) 
   : _a(a), _r(r)
{}

inline AlMat::AlMat_row_const::AlMat_row_const
(const AlMat&a,int r) 
   : _a(a), _r(r)
{}

#endif // ALIGNSOLVTOOLS_ALMAT_H
