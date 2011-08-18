#ifndef FIT_AMP_PAIR_FIT_COVARIANCE_HH
#define FIT_AMP_PAIR_FIT_COVARIANCE_HH

#include <map>
#include <string>
#include "TMatrixT.h"
#include "TMatrixTSym.h"

class FitAmpPairList;

class FitAmpPairFitCovariance{
  static unsigned int _maxSize;
  const FitAmpPairList* _myList;
  TMatrixTSym<double> _fitCov;
  unsigned int _Nevents;
  unsigned int _size;
  bool _needToRecalculate;

  TMatrixTSym<double> _fitResult;

  TMatrixTSym<double> _pairCov, _pairCorr;
  TMatrixTSym<double> _fractionCov;
  TMatrixTSym<double> _fractionSumCov;
  TMatrixTSym<double> _interferenceFracSumCov;
  TMatrixTSym<double> _totalFractionSumCov; // debug only

  TMatrixT<double> _dFitAmpPair_by_dFitParameters;
  TMatrixT<double> _dFraction_by_dFitAmpPair;
  TMatrixT<double> _dFractionSum_by_dFraction;
  TMatrixT<double> _dInterferenceFracSum_by_dFraction;
  TMatrixT<double> _dTotalFractionSum_by_dFraction; // debug only

  std::string realName(int i) const;
  std::string imagName(int i) const;

  inline int indexReal(int i){return 2*i;}
  inline int indexImag(int i){return 2*i + 1;}
  inline int listPositionFromIndex(int idx){return idx/2;}

  bool make_dFitAmpPair_by_dFitParameters();
  bool make_dFraction_by_dFitAmpPair();
  bool make_dFractionSum_by_dFraction();
  bool make_dInterferenceFracSum_by_dFraction();
  bool make_dTotalFractionSum_by_dFraction();

  bool resize();
  unsigned int maxSize()const{return _maxSize;}

  bool makeFitAmpPairCov();
  bool makeFractionCovariance();
  bool makeFractionSumCovariance();
  bool makeInterferenceFracSumCovariance();
  bool makeTotalFractionSumCovariance(); // debug only

 public:
  FitAmpPairFitCovariance(const FitAmpPairList* myList
			  , const TMatrixTSym<double>& fitCov);
  FitAmpPairFitCovariance(const FitAmpPairFitCovariance& other);

  bool addLastEventFromList();
  bool calculateAll();

  double getPairVariance(int i, int j);
  double getPairError(int i, int j);

  double getFractionVariance(int i);
  double getFractionError(int i);
  
  double getFractionSumVariance();
  double getFractionSumError();

  double getInterferenceFracSumVariance();
  double getInterferenceFracSumError();

  double getTotalFractionSumVariance(); //debug only - should always return 0.
  double getTotalFractionSumError(); //debug only - should always return 0.
  unsigned int size() const;
  bool isValid()const;
};

#endif
//
