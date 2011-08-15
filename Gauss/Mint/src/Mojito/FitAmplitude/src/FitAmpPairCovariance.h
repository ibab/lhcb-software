#ifndef FIT_AMP_PAIR_COVARIANCE_HH
#define FIT_AMP_PAIR_COVARIANCE_HH

#include <map>
#include <string>
#include "TMatrixT.h"
#include "TMatrixTSym.h"

class FitAmpPairList;

class FitAmpPairCovariance{
  static unsigned int _maxSize;

  const FitAmpPairList* _myList;
  unsigned int _Nevents;
  unsigned int _size;
  bool _needToRecalculate;

  TMatrixT<float> _sum_x;
  TMatrixTSym<float> _sum_xy;

  TMatrixTSym<float> _cov2N, _corr2N, _cov, _corr;
  TMatrixTSym<float> _fractionCov;
  TMatrixTSym<float> _fractionSumCov;
  TMatrixTSym<float> _integralCov;

  TMatrixT<float> _dN_by_d2N;
  TMatrixT<float> _dFraction_by_dN;
  TMatrixT<float> _dFractionSum_by_dFraction;
  TMatrixT<float> _dIntegral_by_dN;

  std::string realName(int listPosition) const;
  std::string imagName(int listPosition) const;

  inline int indexReal(int listPosition)const{return 2*listPosition;}
  inline int indexImag(int listPosition)const{return 2*listPosition + 1;}
  inline int listPositionFromIndex(int idx)const {return idx/2;}

  bool make_dN_by_d2N();
  bool make_dFraction_by_dN();
  bool make_dFractionSum_by_dFraction();
  bool make_dIntegral_by_dN();

  bool resize();
  unsigned int maxSize()const{return _maxSize;}
  bool make2NCovariance();
  bool makeNCovariance();
  bool makeFractionCovariance();
  bool makeFractionSumCovariance();
  bool makeIntegralCovariance();

  bool clearAll();

  bool makeDirectory(const std::string& asSubdirOf=".") const;
  std::string dirName(const std::string& asSubdirOf=".") const;
  std::string matrix_x_fname(const std::string& asSubdirOf=".") const;
  std::string matrix_xy_fname(const std::string& asSubdirOf=".") const;
  bool save_sum_x(const std::string& inDirectory) const;
  bool save_sum_xy(const std::string& inDirectory) const;
  bool retrieve_sum_x(const std::string& inDirectory);
  bool retrieve_sum_xy(const std::string& inDirectory);

  bool isCompatibleWith(const FitAmpPairCovariance& other) const;
 public:
  FitAmpPairCovariance(const FitAmpPairList* myList);
  FitAmpPairCovariance(const FitAmpPairCovariance& other);
  FitAmpPairCovariance(const FitAmpPairCovariance& other
		       , const FitAmpPairList* newList);

  bool add(const FitAmpPairCovariance& other);
  bool reset();

  bool addLastEventFromList();
  bool calculateAll();

  double getFractionVariance(int i);
  double getFractionError(int i);
    
  double getFractionSumVariance();
  double getFractionSumError();

  double getIntegralVariance();
  double getIntegralError();

  unsigned int size() const;
  bool isValid() const;


  bool save(const std::string& asSubdirOf=".")const;
  bool retrieve(const std::string& fromDirectory=".");

  int Nevents()const{return _Nevents;}

  FitAmpPairCovariance& operator+=(const FitAmpPairCovariance& other);
  FitAmpPairCovariance operator+(const FitAmpPairCovariance& other) const;
};

#endif
//
