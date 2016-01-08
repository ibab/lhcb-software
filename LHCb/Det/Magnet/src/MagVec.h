#ifndef MAGNET_MAGVEC_H
#define MAGNET_MAGVEC_H





class MagVec {


public:

  MagVec(int N);
  MagVec();
  MagVec(const MagVec& v);
 ~MagVec();


  double& operator[](int i);
  const double& operator[](int i) const;


  MagVec& operator=(const MagVec& v);
  //  MagVec& operator=(const double& v);
  MagVec  operator+(const MagVec& v);
  MagVec  operator+(const MagVec& v) const;
  MagVec& operator+=(const MagVec& v);
  MagVec  operator-(const MagVec& v);
  MagVec  operator-(const MagVec& v) const;
  MagVec& operator-=(const MagVec& v);
  double operator*(const MagVec& v);
  double operator*(const MagVec& v) const;
  MagVec  operator*(const double& d);
  MagVec& operator*=(const double& d);

  void reSize(int Nnew);
  int n_elem() const;


private:



  int Nele;

  double* ptr_data;


};



#endif // MAGNET_MAGVEC_H
