#include "RichHpdPSF.h"
RichHpdPSF::RichHpdPSF() { ; }
RichHpdPSF::RichHpdPSF(int hpdnumber, int richdetnumber) {

  m_curHpdNumber=hpdnumber;
  m_curRichDetNumber=richdetnumber;

}
RichHpdPSF::~RichHpdPSF() { ; }
void RichHpdPSF::setCurrentHpdPSF(double PSFvalue) {

  m_hpdPointSpreadFunction=PSFvalue;
}

