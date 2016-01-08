#ifndef MAGNET_MAGMAT_H
#define MAGNET_MAGMAT_H


#include <iostream>
#include <iomanip>



class MagMat

{


 public:

  MagMat(int N, int M);
  MagMat();
  MagMat(const MagMat& m);
 ~MagMat();



 class MagMat_row {
 public:
   inline MagMat_row(MagMat&,int);
   double & operator[](int);
 private:
   MagMat& _a;
   int _r;
 };
 class MagMat_row_const {
 public:
   inline MagMat_row_const(const MagMat&,int);
   const double & operator[](int) const;
 private:
   const MagMat& _a;
   int _r;
 };
 // helper class to implement m[i][j]

 inline MagMat_row operator[] (int);
 inline MagMat_row_const operator[] (int) const;

 MagMat&  operator=(const MagMat& m);
  MagMat   operator+(const MagMat& m);
 MagMat   operator+(const MagMat& m) const;
 MagMat&  operator+=(const MagMat& m);
 MagMat   operator-(const MagMat& m);
 MagMat   operator-(const MagMat& m) const;
 MagMat&   operator-=(const MagMat& m);
 MagMat   operator*(const MagMat& m);
 MagMat   operator*(const MagMat& m) const;
  MagMat&   operator*=(const double& d);

 // transposed matrix
 MagMat T() const;
 //
//invert sym matrix declared as non-symetric for convenience



 int nrow() const;
 int ncol() const;
 // reSize method
 void reSize (int Nnew, int Mnew);



  //  void invert(int ierr); // only if Ncol=Nrow !!

 private:


  friend class MagMat_row;
  friend class MagMat_row_const;


  void copy(const MagMat&  m);
  //  void copyS(const AlSymMat&  m);

  int Ncol, Nrow;
  int Nele;
  double* ptr_data;

  //  double* mat_p;
  // double** mat;

};

// inline operators

inline MagMat::MagMat_row MagMat::operator[] (int r)
{
  MagMat_row b(*this,r);
  return b;
}

inline MagMat::MagMat_row_const MagMat::operator[] (int r) const

{
  const MagMat_row_const b(*this,r);

  return b;
}

inline double &MagMat::MagMat_row::operator[](int c)
{
   if(_r<0||_r>=_a.Nrow || c<0||c>=_a.Ncol)
      std::cerr << "Range error in MagMat::operator[][]" << std::endl;

   return *(_a.ptr_data+_r*_a.Ncol+c);

}

inline const double & MagMat::MagMat_row_const::operator[](int c) const
{

   if(_r<0||_r>=_a.Nrow || c<0||c>=_a.Ncol)
     std::cerr << "Range error in MagMat::operator[][]" << std::endl;

   return *(_a.ptr_data+_r*_a.Ncol+c);

}

inline MagMat::MagMat_row::MagMat_row(MagMat &a,int r)
   : _a(a), _r(r)
{}

inline MagMat::MagMat_row_const::MagMat_row_const
(const MagMat&a,int r)
   : _a(a), _r(r)
{}

#endif // MAGNET_MAGMAT_H
