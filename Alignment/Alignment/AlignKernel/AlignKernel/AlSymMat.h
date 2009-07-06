#ifndef SOLVKERNEL_ALSYMMAT_H
#define SOLVKERNEL_ALSYMMAT_H

// from STD
#include <iostream>
#include <ostream>
#include <iomanip>
#include <vector>

class AlVec;
class AlMat;


class AlSymMat  {


 public:

  AlSymMat(size_t N);
  AlSymMat();
  AlSymMat(const AlSymMat& m);
 ~AlSymMat();

 class AlSymMat_row {
 public:
   inline AlSymMat_row(AlSymMat&,size_t);
   double & operator[](size_t);
 private:
   AlSymMat& _a;
   size_t _r;
 };
 class AlSymMat_row_const {
 public:
   inline AlSymMat_row_const(const AlSymMat&,size_t);
   const double & operator[](size_t) const;
 private:
   const AlSymMat& _a;
   size_t _r;
 };
 // helper class to implement m[i][j]

 inline AlSymMat_row operator[] (size_t);
 inline AlSymMat_row_const operator[] (size_t) const;

  // access method that requires irow>=icol without checking
  double fast(size_t irow, size_t icol) const { return *(ptr_data + (irow+1)*irow/2 + icol) ; }
  double& fast(size_t irow, size_t icol)      { return *(ptr_data + (irow+1)*irow/2 + icol) ; }
  double operator() (size_t irow, size_t icol) const { return irow >= icol ? fast(irow,icol) : fast(icol,irow) ; }
  double& operator() (size_t irow, size_t icol)      { return irow >= icol ? fast(irow,icol) : fast(icol,irow) ; }
  
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

  //insert block from SMatrix types
  template <typename T>  
  void insert(unsigned col, unsigned row, const T& rhs) {
    for (unsigned i = 0; i < T::kCols; ++i)
	    for (unsigned j = 0; j < T::kRows; ++j)
        (*this)[col+i][row+j] = rhs(i,j);
  }

  // reSize method
  void reSize(size_t Nnew);
  size_t size() const { return Size ; }
  size_t nrow() const { return size() ; }
  size_t ncol() const { return size() ; }

  void removeElement(size_t Nr);
  void ShrinkDofs(std::vector<bool>& mask);
  
  void invert(int& ierr);
  double determinant();
  int diagonalize(char jobz, AlVec& w, AlMat& z);
  void diagonalize_GSL(AlVec& egval, AlMat& egvec);
  

 private:


  friend class AlVec;
  friend class AlMat;
  friend class AlSymMat_row;
  friend class AlSymMat_row_const;

  void copy(const AlSymMat&  m);
  double* ptr_data;
  size_t Size; size_t Nele;
};

// inline operators
inline std::ostream& operator<<(std::ostream& lhs, const AlSymMat& rhs) {
  lhs << "[ ";
  for ( size_t i = 0u; i < rhs.size(); ++i) { 
    for ( size_t j = 0u; j < rhs.size(); ++j) {
      lhs << " " << rhs[i][j] << " ";
    }
  lhs << std::endl;     
  }
  lhs << " ]" << std::endl;
  return lhs;
}


inline AlSymMat::AlSymMat_row AlSymMat::operator[] (size_t r)
{
  AlSymMat_row b(*this,r);
  return b;
}

inline AlSymMat::AlSymMat_row_const AlSymMat::operator[] (size_t r) const

{
  const AlSymMat_row_const b(*this,r);

  return b;
}

inline double &AlSymMat::AlSymMat_row::operator[](size_t c)
{
   if(_r>=_a.Size || c>=_a.Size)
      std::cerr << "Range error in AlSymMat::operator[][]" << std::endl;

   if (_r >= c ) {
      return *(_a.ptr_data + (_r+1)*_r/2 + c);
   } else {
      return *(_a.ptr_data + (c+1)*c/2 + _r);
   }
}

inline const double & AlSymMat::AlSymMat_row_const::operator[](size_t c) const 
{

   if(_r>=_a.Size ||c>=_a.Size)
      std::cerr << "Range error in AlSymMat::operator[][]" << std::endl;

   if (_r >= c ) {
      return *(_a.ptr_data + (_r+1)*_r/2 + c);
   } else {
      return *(_a.ptr_data + (c+1)*c/2 + _r);
   }
}

inline AlSymMat::AlSymMat_row::AlSymMat_row(AlSymMat &a,size_t r) 
   : _a(a), _r(r)
{}

inline AlSymMat::AlSymMat_row_const::AlSymMat_row_const
(const AlSymMat&a,size_t r) 
   : _a(a), _r(r)
{}

inline AlSymMat operator*(double d, const AlSymMat& m) 
{
  AlSymMat rc = m ;
  rc *= d ;
  return rc ;
}

#endif // SOLVKERNEL_ALSYMMAT_H

