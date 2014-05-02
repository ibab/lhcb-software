#ifndef DALITZ_PDF_NORM_CHECKER_HH
#define DALITZ_PDF_NORM_CHECKER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "TRandom.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzPdf.h"

#include "Mint/counted_ptr.h"

#include <iostream>

class DalitzPdfNormChecker{
  IDalitzPdf* _pdf;
  TRandom* _rnd;
  DalitzEventPattern _pat;
  DalitzCoordinate _t01, _s12, _s23, _s34, _t40;

  double _askingPrecision;
  double _sum, _sumsq;
  int _NInside;
  double _sumPhaseSpace, _sumPhaseSpaceSq;
  long unsigned int _Nevents;

  void makeMiMa();

  MINT::counted_ptr<DalitzEvent> makeEventForOwner( double scale0
					      , double scale1
					      , double scale2
					      , double scale3
					      , double scale4
					      ) const;

  MINT::counted_ptr<DalitzEvent> make3Event() const;
  MINT::counted_ptr<DalitzEvent> make4Event() const;
  MINT::counted_ptr<DalitzEvent> makeEventForOwner() const;

  double currentAbsolutePrecision()const;
  double currentRelativePrecision()const;
  double boxSize() const;
  bool sufficientPrecision() const;
 public:
  DalitzPdfNormChecker(IDalitzPdf* pdf
		       , const DalitzEventPattern& pat
		       , double prec = 1.0e-2
		       , TRandom* rnd=gRandom);

  double checkNorm();
  double result() const;
  double resultError() const;

  void print(std::ostream& os = std::cout)const;
};

std::ostream& operator<<(std::ostream& os, const DalitzPdfNormChecker& nc);

#endif
//
