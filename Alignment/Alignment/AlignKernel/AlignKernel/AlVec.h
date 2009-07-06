#ifndef SOLVKERNEL_ALVEC_H
#define SOLVKERNEL_ALVEC_H

// from STD
#include <iostream>
#include <ostream>
#include <vector>

class AlSymMat;
class AlMat;

class AlVec {


public:

  AlVec(size_t N);
  AlVec();
  AlVec(const AlVec& v);
 ~AlVec();

  double& operator[](size_t i);
  const double& operator[](size_t i) const;
  double& operator()(size_t i) { return  *(ptr_data+i); } 
  const double& operator()(size_t i) const { return  *(ptr_data+i); } 
  
  AlVec& operator=(const AlVec& v);
  //  AlVec& operator=(const double& v);
  AlVec  operator+(const AlVec& v);
  AlVec  operator+(const AlVec& v) const;
  AlVec& operator+=(const AlVec& v);
  AlVec  operator-(const AlVec& v);
  AlVec  operator-(const AlVec& v) const;
  AlVec& operator-=(const AlVec& v);
  double operator*(const AlVec& v);
  double operator*(const AlVec& v) const;
  AlVec  operator*(const AlMat& m);
  AlVec  operator*(const AlMat& m) const;
  AlVec  operator*(const AlSymMat& m);
  AlVec  operator*(const AlSymMat& m) const;
  AlVec  operator*(const double& d);
  AlVec  operator*(const double& d) const;
  AlVec& operator*=(const double& d);

//insert block from SMatrix types
  template <typename T>
  void insert(unsigned row, const T& rhs) {
    for (unsigned i = 0; i < T::kSize; ++i) (*this)[row+i] = rhs(i);
  }

  void reSize(size_t Nnew);
  void removeElement(size_t Nr);
  void ShrinkDofs(std::vector<bool>& mask);
  size_t size() const { return Nele; }

private:
  friend class AlSymMat;
  friend class AlMat;
  
  size_t Nele ;
  double* ptr_data;
};

inline std::ostream& operator<<(std::ostream& lhs, const AlVec& rhs) {
  lhs << "[ ";
  for (size_t i = 0u; i < rhs.size(); ++i) lhs << " " << rhs[i] << " ";     
  lhs << " ]" << std::endl;
  return lhs;
}

inline AlVec operator*(double d, const AlVec& vec) 
{
  AlVec rc = vec ;
  rc *= d ;
  return rc ;
}

#endif // SOLVKERNEL_ALVEC_H
