#ifndef ALIGNSOLVTOOLS_ALSYMMAT_H
#define ALIGNSOLVTOOLS_ALSYMMAT_H

#include <iostream>
#include <iomanip>


class AlVec;
class AlMat;


class AlSymMat  {


 public:

  AlSymMat(int N);
  AlSymMat();
  AlSymMat(const AlSymMat& m);
 ~AlSymMat();

 class AlSymMat_row {
 public:
   inline AlSymMat_row(AlSymMat&,int);
   double & operator[](int);
 private:
   AlSymMat& _a;
   int _r;
 };
 class AlSymMat_row_const {
 public:
   inline AlSymMat_row_const(const AlSymMat&,int);
   const double & operator[](int) const;
 private:
   const AlSymMat& _a;
   int _r;
 };
 // helper class to implement m[i][j]

 inline AlSymMat_row operator[] (int);
 inline AlSymMat_row_const operator[] (int) const;




  AlSymMat&  operator=(const AlSymMat& m);
  AlSymMat   operator+(const AlSymMat& m);  
  AlSymMat   operator+(const AlSymMat& m) const;  
  AlSymMat&  operator+=(const AlSymMat& m);
  AlSymMat   operator-(const AlSymMat& m);
  AlSymMat   operator-(const AlSymMat& m) const;
  AlMat   operator*(const AlSymMat& m);
  AlMat   operator*(const AlSymMat& m) const;
  AlVec    operator*(const AlVec& v);
  AlVec    operator*(const AlVec& v) const;
  AlMat  operator*(const AlMat& m);
  AlMat  operator*(const AlMat& m) const;
  AlSymMat&  operator*=(const double& d);

  int size() const; 


 // reSize method
  void reSize(int Nnew);

  void invert(int& ierr);

  double determinant();

  int diagonalize(char jobz, AlVec& w, AlMat& z);

  void diagonalize_GSL(AlVec& egval, AlMat& egvec);
  

 private:


  friend class AlVec;
  friend class AlMat;
  friend class AlSymMat_row;
  friend class AlSymMat_row_const;

  friend class AlignSolvTool;
  


  void copy(const AlSymMat&  m);
  double* ptr_data;
  int Size; int Nele;
};

// inline operators

inline AlSymMat::AlSymMat_row AlSymMat::operator[] (int r)
{
  AlSymMat_row b(*this,r);
  return b;
}

inline AlSymMat::AlSymMat_row_const AlSymMat::operator[] (int r) const

{
  const AlSymMat_row_const b(*this,r);

  return b;
}

inline double &AlSymMat::AlSymMat_row::operator[](int c)
{
   if(_r<0||_r>=_a.Size || c<0||c>=_a.Size)
      std::cerr << "Range error in AlSymMat::operator[][]" << std::endl;

   if (_r >= c ) {
      return *(_a.ptr_data + (_r+1)*_r/2 + c);
   } else {
      return *(_a.ptr_data + (c+1)*c/2 + _r);
   }
}

inline const double & AlSymMat::AlSymMat_row_const::operator[](int c) const 
{

   if(_r<0||_r>=_a.Size || c<0||c>=_a.Size)
      std::cerr << "Range error in AlSymMat::operator[][]" << std::endl;

   if (_r >= c ) {
      return *(_a.ptr_data + (_r+1)*_r/2 + c);
   } else {
      return *(_a.ptr_data + (c+1)*c/2 + _r);
   }
}

inline AlSymMat::AlSymMat_row::AlSymMat_row(AlSymMat &a,int r) 
   : _a(a), _r(r)
{}

inline AlSymMat::AlSymMat_row_const::AlSymMat_row_const
(const AlSymMat&a,int r) 
   : _a(a), _r(r)
{}


#endif // ALIGNSOLVTOOLS_ALSYMMAT_H

