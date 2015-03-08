#include <TH1.h>
#include <TFile.h>
#include <iostream>

class TWeighter1D{
public:
  TWeighter1D(){
    m_th1=NULL;
    m_file=NULL;
  }
  TWeighter1D(std::string fname,std::string hname="weight"){
    set(fname,hname);
  }

  virtual ~TWeighter1D(){}

  bool    set(std::string fname,std::string hname,bool force=true){
    if(NULL == m_file || force)m_file = TFile::Open(fname.c_str());
    if( NULL == m_file)return false;
    if(NULL == m_th1 || force)m_th1  = (TH1*) m_file->Get(hname.c_str());
    if( NULL == m_th1)return false;
    return true;
  }


  double weight(double val){
    double min= m_th1->GetXaxis()->GetXmin();
    double max= m_th1->GetXaxis()->GetXmax();
    if(val < min) val=min;
    if(val > max) val=max;
    int nb    = m_th1->GetXaxis()->FindBin(val);
    double w  = m_th1->GetBinContent(nb);
    return w;
  }

  double operator()(double val){
    return this->weight(val);
  }


  void close(){
    if(NULL != m_file){
      m_file->Close();
      delete m_file;
    }
    if(NULL != m_th1){
      delete m_th1;
    }
  }
  

private:
  TH1*  m_th1;
  TFile* m_file;

  ClassDef(TWeighter1D,1);
};

