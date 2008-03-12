#ifndef INITHISTOS_H 
#define INITHISTOS_H 1

class InitHistos { 
public:
  InitHistos();
  virtual ~InitHistos(); ///< Destructor
protected:
void starthistos(){
  initializeHisto(m_histoL0,"L0",0.,14.,14);
  initializeHisto(m_histoalleycall,"Alleys Called",0.,6.,6);
  initializeHisto(m_histoalleyacc,"Accepted by Alley",0.,6.,6);
  initializeHisto(m_histomuonalley,"Muon Alley fonctioning",0.,16.,16);
  initializeHisto(m_histohadronalley,"Hadron Alley fonctioning",0.,16.,16);
#define L0entry 0.5  
#define L0conf 1.5  
  m_histoL0corr = this->book2D("Correlated L0 bits",0.,14.,14,0.,14.,14);
  m_hcorrallcall = this->book2D("Correlation Calls",0.,6.,6,0.,6.,6);
  m_hcorrallacc = this->book2D("Correlated Alleys",0.,6.,6,0.,6.,6);
}
};
#endif // INITHISTOS_H
