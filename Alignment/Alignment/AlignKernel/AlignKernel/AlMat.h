#ifndef SOLVKERNEL_ALMAT_H
#define SOLVKERNEL_ALMAT_H


#include <iostream>
#include <iomanip>

class AlVec;
class AlSymMat;

class AlMat

{


 public:

  AlMat(size_t N, size_t M);
  AlMat();
  AlMat(const AlMat& m);
 ~AlMat();

 

 class AlMat_row {
 public:
   inline AlMat_row(AlMat&,size_t);
   double & operator[](size_t);
 private:
   AlMat& _a;
   size_t _r;
 };
 class AlMat_row_const {
 public:
   inline AlMat_row_const(const AlMat&,size_t);
   const double & operator[](size_t) const;
 private:
   const AlMat& _a;
   size_t _r;
 };
 // helper class to implement m[i][j]

 inline AlMat_row operator[] (size_t);
 inline AlMat_row_const operator[] (size_t) const;
 double operator() (size_t irow, size_t icol) const { return (*this)[irow][icol] ; }
 double& operator() (size_t irow, size_t icol) { return (*this)[irow][icol] ; }

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


 size_t nrow() const;
 size_t ncol() const;
  // reSize method
  void reSize (size_t Nnew, size_t Mnew);
  void removeRow(size_t Nr);
  void removeCol(size_t Nr);
  
 
  void copyS(const AlSymMat&  m);

  //  void invert(int ierr); // only if Ncol=Nrow !!

 private:

  friend class AlVec;
  friend class AlSymMat;
  friend class AlMat_row;
  friend class AlMat_row_const;

  void copy(const AlMat&  m);
  //  void copyS(const AlSymMat&  m);

  size_t Ncol, Nrow;
  size_t Nele;
  double* ptr_data;

  //  double* mat_p;
  // double** mat;

};

// inline operators

inline AlMat::AlMat_row AlMat::operator[] (size_t r)
{
  AlMat_row b(*this,r);
  return b;
}

inline AlMat::AlMat_row_const AlMat::operator[] (size_t r) const

{
  const AlMat_row_const b(*this,r);

  return b;
}

inline double &AlMat::AlMat_row::operator[](size_t c)
{
   if(_r>=_a.Nrow || c>=_a.Ncol)
      std::cerr << "Range error in AlMat::operator[][]" << std::endl;

   return *(_a.ptr_data+_r*_a.Ncol+c);

}

inline const double & AlMat::AlMat_row_const::operator[](size_t c) const 
{

   if(_r>=_a.Nrow || c>=_a.Ncol)
     std::cerr << "Range error in AlMat::operator[][]" << std::endl;
   
   return *(_a.ptr_data+_r*_a.Ncol+c);
 
}

inline AlMat::AlMat_row::AlMat_row(AlMat &a,size_t r) 
   : _a(a), _r(r)
{}

inline AlMat::AlMat_row_const::AlMat_row_const
(const AlMat&a,size_t r) 
   : _a(a), _r(r)
{}

#endif // SOLVKERNEL_ALMAT_H
