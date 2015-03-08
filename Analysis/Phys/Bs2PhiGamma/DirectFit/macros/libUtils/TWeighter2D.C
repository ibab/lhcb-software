#include <TH2.h>
#include <TFile.h>
#include <iostream>

class TWeighter2D{
public:
  TWeighter2D(){
    m_th2=NULL;
    m_file=NULL;
  }
  TWeighter2D(std::string fname,std::string hname="weight"){
    set(fname,hname);
  }

  virtual ~TWeighter2D(){}

  bool    set(std::string fname,std::string hname,bool force=true){
    if(NULL == m_file || force)m_file = TFile::Open(fname.c_str());
    if( NULL == m_file)return false;
    if(NULL == m_th2 || force)m_th2  = (TH2*) m_file->Get(hname.c_str());
    if( NULL == m_th2)return false;
    return true;
  }


  double weight(double val1,double val2){
    double min1= m_th2->GetXaxis()->GetXmin();
    double max1= m_th2->GetXaxis()->GetXmax();
    if(val1 < min1) val1=min1;
    if(val1 > max1) val1=max1;
    double min2= m_th2->GetYaxis()->GetXmin();
    double max2= m_th2->GetYaxis()->GetXmax();
    if(val2 < min2) val2=min2;
    if(val2 > max2) val2=max2;


    int nb1    = m_th2->GetXaxis()->FindBin(val1);
    int nb2    = m_th2->GetYaxis()->FindBin(val2);
    double w  = m_th2->GetBinContent(nb1,nb2);
    return w;
  }

  double operator()(double val1,double val2){
    return this->weight(val1,val2);
  }
  


  void close(){
    if(NULL != m_file){
      m_file->Close();
      delete m_file;
    }
    if(NULL != m_th2){
      delete m_th2;
    }
  }
  

private:
  TH2*  m_th2;
  TFile* m_file;

  ClassDef(TWeighter2D,1);
};

