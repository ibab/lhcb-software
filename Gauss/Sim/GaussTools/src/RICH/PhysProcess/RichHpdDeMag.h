#ifndef RichHpdDeMag_h
#define RichHpdDeMag_h 1
// class for the hpd demagnification factor.
// the linear term is negative to account for the
// cross focussing effect.
//SE 1-3-02.
#include <vector>
class RichHpdDeMag {

 public:
  RichHpdDeMag();
  RichHpdDeMag(int hpdnumb, int richdetnumb);
  virtual ~RichHpdDeMag();
  double richHpdDemagLinearTerm() {return m_richHpdDemagLinearTerm; }
  double richHpdDemagQuadraticTerm() {return  m_richHpdDemagQuadraticTerm;}
  std::vector<double>HpdDemagVect(); 
  int curHpdNumb() {return  m_curHpdNumb; }
  int curRichDetNumb() {return m_curRichDetNumb; }
  void setCurrentHPDDemag(std::vector<double>demagVect);
 private:
  int m_curHpdNumb;
  int m_curRichDetNumb;
  double m_richHpdDemagLinearTerm;
  double m_richHpdDemagQuadraticTerm;

};
#endif
