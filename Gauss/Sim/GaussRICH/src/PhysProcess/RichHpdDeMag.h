#ifndef RichHpdDeMag_h
#define RichHpdDeMag_h 1

#include <vector>
#include "G4ThreeVector.hh"
#include "RichDet/Rich1DTabFunc.h"

// class for the hpd demagnification factor.

class RichHpdDeMag {

 public:

  RichHpdDeMag();
  virtual ~RichHpdDeMag();

  RichHpdDeMag(IDataProviderSvc* , int hpdnumb, int richdetnumb);

  double richHpdDemagLinearTerm()    const {return m_richHpdDemagLinearTerm; }
  double richHpdDemagQuadraticTerm() const {return  m_richHpdDemagQuadraticTerm;}
  std::vector<double> HpdDemagVect() const; 

  int curHpdNumb()     {return m_curHpdNumb; }
  int curRichDetNumb() {return m_curRichDetNumb; }
  void setCurrentHPDDemag(const std::vector<double> & demagVect);

  void setCurrentDemagnification(const Rich1DTabFunc*, const Rich1DTabFunc* );
  G4ThreeVector getPositionOnAnode( double , double ) ;

 private:

  int m_curHpdNumb;
  int m_curRichDetNumb;
  double m_richHpdDemagLinearTerm;
  double m_richHpdDemagQuadraticTerm;

  double m_RichHpdQWToSiMaxDist, m_Rcurv, m_activeRadius;

  const Rich1DTabFunc *m_Rdemag, *m_Phidemag;

};
#endif
