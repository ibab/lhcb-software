#ifndef ALIGNSOLVTOOLS_INTVEC_H
#define ALIGNSOLVTOOLS_INTVEC_H






class IntVec {


public:

  IntVec(int N);
  IntVec(int N, int init);
  IntVec();
  IntVec(const IntVec& v);
 ~IntVec();


  int& operator[](int i);
  const int& operator[](int i) const;


  IntVec& operator=(const IntVec& v);

  IntVec  operator+(const IntVec& v);
  IntVec& operator+=(const IntVec& v);
  IntVec  operator-(const IntVec& v);
  IntVec& operator-=(const IntVec& v);



  void reSize(int Nnew);
  int n_elem();


private:


  int Nele;

  int* ptr_data;


};



#endif // ALIGNSOLVTOOLS_INTVEC_H
