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
  RichHpdPSF(int hpdnumber, int richdetnumber);
  virtual ~RichHpdPSF();
  int curHpdNumber() {return m_curHpdNumber; }
  int curRichDetNumber() {return  m_curRichDetNumber; }
  double hpdPointSpreadFunction() {return m_hpdPointSpreadFunction;}
  void setCurrentHpdPSF(double PSFvalue);
 private:
  int m_curHpdNumber;
  int m_curRichDetNumber;
  double m_hpdPointSpreadFunction;

};

#endif
