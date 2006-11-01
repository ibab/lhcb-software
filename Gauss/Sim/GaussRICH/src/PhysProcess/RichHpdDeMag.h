#ifndef RichHpdDeMag_h
#define RichHpdDeMag_h 1

// class for the hpd demagnification factor.
// the linear term is negative to account for the
// cross focussing effect.
//SE 1-3-02.

#include <vector>
#include "G4ThreeVector.hh"
#include "DetDesc/TabulatedProperty.h"

class RichHpdDeMag {

 public:

  RichHpdDeMag();
  virtual ~RichHpdDeMag();

  RichHpdDeMag(IDataProviderSvc* , int hpdnumb, int richdetnumb);

  double richHpdDemagLinearTerm() const {return m_richHpdDemagLinearTerm; }
  double richHpdDemagQuadraticTerm() const {return  m_richHpdDemagQuadraticTerm;}
  std::vector<double> HpdDemagVect() const; 

  int curHpdNumb() {return  m_curHpdNumb; }
  int curRichDetNumb() {return m_curRichDetNumb; }
  void setCurrentHPDDemag(const std::vector<double> & demagVect);

  void setCurrentDemagnification(TabulatedProperty::Table& );
  G4ThreeVector getPositionOnAnode( double , double ) ;

 private:

  int m_curHpdNumb;
  int m_curRichDetNumb;
  double m_richHpdDemagLinearTerm;
  double m_richHpdDemagQuadraticTerm;

  double m_RichHpdQWToSiMaxDist, m_Rcurv, m_activeRadius;
  TabulatedProperty::Table m_demag;

};
#endif
