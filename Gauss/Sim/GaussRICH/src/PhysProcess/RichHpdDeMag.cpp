#include "RichHpdDeMag.h"
RichHpdDeMag::RichHpdDeMag(){ ; }
RichHpdDeMag::RichHpdDeMag(int hpdnumb, int richdetnumb){
  
  m_curHpdNumb=hpdnumb;
  m_curRichDetNumb=richdetnumb;

}
RichHpdDeMag::~RichHpdDeMag(){;  }
std::vector<double> RichHpdDeMag::HpdDemagVect(){
  std::vector<double>Demag(2);
  Demag[0]= m_richHpdDemagLinearTerm;
  Demag[1]= m_richHpdDemagQuadraticTerm;
  return Demag;

}
void RichHpdDeMag::setCurrentHPDDemag(std::vector<double>demagVect) {

  
  m_richHpdDemagLinearTerm=demagVect[0];
  m_richHpdDemagQuadraticTerm=demagVect[1];
  
}
