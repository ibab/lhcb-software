#ifndef RichHpdPSF_h
#define RichHpdPSF_h 1
// class with Hpd Point Spread functions.
// for now constant factor independent of the radius
// in hpd
// SE 1-3-02.
//
class RichHpdPSF {

public:

  RichHpdPSF();
  RichHpdPSF(const int hpdnumber, const int richdetnumber);
  virtual ~RichHpdPSF();
  int curHpdNumber() const {return m_curHpdNumber; }
  int curRichDetNumber() const {return  m_curRichDetNumber; }
  double hpdPointSpreadFunction() const {return m_hpdPointSpreadFunction;}
  void setCurrentHpdPSF(const double PSFvalue);

private:

  int m_curHpdNumber;
  int m_curRichDetNumber;
  double m_hpdPointSpreadFunction;

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
