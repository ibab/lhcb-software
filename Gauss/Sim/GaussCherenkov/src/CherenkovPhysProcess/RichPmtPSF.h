#ifndef RichPmtPSF_h
#define RichPmtPSF_h 1
// class with Pmt Point Spread functions.
// for now constant factor independent of the radius
// in hpd
// SE 1-3-02.
//
#include <vector>

class RichPmtPSF {

public:

  RichPmtPSF();
  RichPmtPSF(const int pmtnumber, const int richdetnumber);
  virtual ~RichPmtPSF();
  int curPmtNumber() const {return m_curPmtNumber; }
  int curRichDetNumber() const {return  m_curRichDetNumber; }
  double pmtPointSpreadFunction() const {return m_pmtPointSpreadFunction;}
  void setCurrentPmtPSF(const double PSFvalue);
  void resetPSFVect();
  double pmtPointSpreadFunctionVectSingleValue(int abin ) {
    double aVal=0.0;
    if(abin < (int) m_pmtPointSpreadFunctionVect.size() ) {
      aVal =  m_pmtPointSpreadFunctionVect[abin];

    }
    return aVal;
  } 
  
  double pmtPSFPhoEnergyVectSingleValue (int bbin ) {
    double bVal=0.0;
    if(bbin< (int) m_pmtPSFPhoEnergyVect.size() ) {
      bVal =  m_pmtPSFPhoEnergyVect[bbin];
    }
    return bVal;
  }
  double pmtPSFRadialPosVectSingleValue (int cbin ) {
    double cVal =0.0;
    if(cbin < (int) m_pmtPSFRadialPosVect.size() ) {
      cVal = m_pmtPSFRadialPosVect[cbin];

    }
    return cVal;
  }
  void setpmtPointSpreadFunctionVect( std::vector<double> aVect) {
    m_pmtPointSpreadFunctionVect=aVect;
  }
  void setpmtPSFPhoEnergyVect(  std::vector<double>aPhVect) {
    m_pmtPSFPhoEnergyVect= aPhVect;
  }
  void setpmtPSFRadialPosVect( std::vector<double>arVect) {
    m_pmtPSFRadialPosVect = arVect;
  }
private:

  int m_curPmtNumber;
  int m_curRichDetNumber;
  double m_pmtPointSpreadFunction;

  std::vector<double> m_pmtPointSpreadFunctionVect;
  std::vector<double> m_pmtPSFPhoEnergyVect;
  std::vector<double> m_pmtPSFRadialPosVect;
  
};

inline RichPmtPSF::RichPmtPSF(const int pmtnumber, 
                              const int richdetnumber) 
{
  m_curPmtNumber=pmtnumber;
  m_curRichDetNumber=richdetnumber;
}

inline void RichPmtPSF::setCurrentPmtPSF(const double PSFvalue)
{
  m_pmtPointSpreadFunction=PSFvalue;
}

#endif
