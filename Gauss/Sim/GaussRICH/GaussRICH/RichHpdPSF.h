#ifndef RichHpdPSF_h
#define RichHpdPSF_h 1
// class with Hpd Point Spread functions.
// for now constant factor independent of the radius
// in hpd
// SE 1-3-02.
//
#include <vector>

class RichHpdPSF {

public:

  RichHpdPSF();
  RichHpdPSF(const int hpdnumber, const int richdetnumber);
  virtual ~RichHpdPSF();
  int curHpdNumber() const {return m_curHpdNumber; }
  int curRichDetNumber() const {return  m_curRichDetNumber; }
  double hpdPointSpreadFunction() const {return m_hpdPointSpreadFunction;}
  void setCurrentHpdPSF(const double PSFvalue);
  void resetPSFVect();
  double hpdPointSpreadFunctionVectSingleValue(int abin ) {
    double aVal=0.0;
    if(abin < (int) m_hpdPointSpreadFunctionVect.size() ) {
      aVal =  m_hpdPointSpreadFunctionVect[abin];

    }
    return aVal;
  } 
  
  double hpdPSFPhoEnergyVectSingleValue (int bbin ) {
    double bVal=0.0;
    if(bbin< (int) m_hpdPSFPhoEnergyVect.size() ) {
      bVal =  m_hpdPSFPhoEnergyVect[bbin];
    }
    return bVal;
  }
  double hpdPSFRadialPosVectSingleValue (int cbin ) {
    double cVal =0.0;
    if(cbin < (int) m_hpdPSFRadialPosVect.size() ) {
      cVal = m_hpdPSFRadialPosVect[cbin];

    }
    return cVal;
  }
  double hitSmearSingleValue (int kbin ){
    double kVal=0.0;
    if(kbin < (int)  m_hitSmearValueVect.size() ) {
      kVal=  m_hitSmearValueVect[kbin];
      
    }
    return kVal; 
  }
  
  void sethpdPointSpreadFunctionVect( std::vector<double> aVect) {
    m_hpdPointSpreadFunctionVect=aVect;
  }
  void sethpdPSFPhoEnergyVect(  std::vector<double>aPhVect) {
    m_hpdPSFPhoEnergyVect= aPhVect;
  }
  void sethpdPSFRadialPosVect( std::vector<double>arVect) {
    m_hpdPSFRadialPosVect = arVect;
  }
  void sethitSmearValueVect(std::vector<double>asVect) {
    m_hitSmearValueVect=asVect;
    
  }
  
private:

  int m_curHpdNumber;
  int m_curRichDetNumber;
  double m_hpdPointSpreadFunction;

  std::vector<double> m_hpdPointSpreadFunctionVect;
  std::vector<double> m_hpdPSFPhoEnergyVect;
  std::vector<double> m_hpdPSFRadialPosVect;
  std::vector<double> m_hitSmearValueVect;
  
  
};

inline RichHpdPSF::RichHpdPSF(const int hpdnumber, 
                              const int richdetnumber) 
{
  m_curHpdNumber=hpdnumber;
  m_curRichDetNumber=richdetnumber;
}

inline void RichHpdPSF::setCurrentHpdPSF(const double PSFvalue)
{
  m_hpdPointSpreadFunction=PSFvalue;
}

#endif
