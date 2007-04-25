#ifndef ALIGNSOLVTOOLS_ALVEC_H
#define ALIGNSOLVTOOLS_ALVEC_H




class AlSymMat;
class AlMat;

class AlVec {


public:

  AlVec(int N);
  AlVec();
  AlVec(const AlVec& v);
 ~AlVec();


  double& operator[](int i);
  const double& operator[](int i) const;

 
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
  AlVec& operator*=(const double& d);

  void reSize(int Nnew);
  int n_elem() const;


private:

  friend class AlSymMat;
  friend class AlMat;


  friend class AlignSolvTool;
  

  int Nele;

  double* ptr_data;


};



#endif // ALIGNSOLVTOOLS_ALVEC_H
