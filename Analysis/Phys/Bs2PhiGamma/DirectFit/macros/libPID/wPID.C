#include <TH2.h>
#include <map>
#include <TFile.h>
#include <iostream>
//========= PID-weights
class wPID{
public:
  wPID(){};
  TH2* declarePID(std::string fname,std::string hname,std::string nickname=""){
    TFile* f = TFile::Open(fname.c_str());
    if( NULL == f ){
      std::cout << "Cannot link " << fname << std::endl;
      return NULL;
    }
    TH2*   h = (TH2*) f->Get(hname.c_str());
    if( NULL == h ){
      std::cout << "Unknwon TH2 " << hname << std::endl;
      return NULL;
    }
    if( "" == nickname)nickname=hname; 
    if( NULL == getPID(nickname) ){
      m_map[nickname]=h; 
      std::cout << fname<<"[" << hname << "] registered as '" << nickname << "'"<< std::endl;
    }
    else{
      std::cout << "WARNING : nickname '" << nickname <<"' is already registered " << std::endl;
    }
    return h;
  }
  
  TH2* getPID(std::string name){
    std::map<std::string,TH2*>::iterator it = m_map.find(name);
    if( m_map.end() == it)return NULL;
    return m_map[name];
  }
  

  double getWeightValue(std::string name, double p,double e){
    return getWeight( getPID(name),p,e).first;
  }
  double getWeightError(std::string name, double p,double e){
    return getWeight( getPID(name),p,e).second;
  }
  std::pair<double,double> getWeight(std::string name, double p,double e){
    return getWeight( getPID(name),p,e);
  }
  //
  double getWeightValue(TH2* h2, double p,double e){
    return getWeight( h2,p,e).first;
  }
  double getWeightError(TH2* h2, double p,double e){
    return getWeight( h2,p,e).second;
  }  
  std::pair<double,double> getWeight(TH2* h2, double p,double e){
    if( NULL == h2 )return std::make_pair(-1.,-1.);
    
    double xmin = h2->GetXaxis()->GetXmin();
    double xmax = h2->GetXaxis()->GetXmax();
    int    xbin = h2->GetXaxis()->GetNbins();
    double ymin = h2->GetYaxis()->GetXmin();
    double ymax = h2->GetYaxis()->GetXmax();
    int    ybin = h2->GetYaxis()->GetNbins();
    
    if(  p < xmin || p > xmax)return std::make_pair(0.,0.);
    if(  e < ymin || e > ymax)return std::make_pair(0.,0.);
    
    int nx =  int((p-xmin)/(xmax-xmin)*double(xbin)) + 1;
    int ny =  int((e-ymin)/(ymax-ymin)*double(ybin)) + 1;
    double eff = h2->GetBinContent( nx ,ny);
    double err = h2->GetBinError( nx ,ny);
    return std::make_pair(eff,err);
  }
  
  
private:
  std::map<std::string,TH2*> m_map;
  
};

  

//========= PID-weighting data
class wData{
public:
  wData(std::string nam){
    m_name=nam;
    m_sum=0.;
    m_count=0.;
    m_esum=0.;
  }
  //  virtual ~Data();

  void push(std::pair<double,double> data){
    m_sum += data.first;
    m_esum += data.second;
    m_count+=1.;
  } 
  void pushCut(bool cut){
    m_sum +=  cut ? 1. : 0. ;
    m_esum = sqrt(m_sum);
    m_count+=1.;
  } 
  void pushWeight(bool cut,std::pair<double,double> data1,std::pair<double,double> data2){
    m_sum +=  (cut ? 1. : 0. )*data1.first*data2.first;
    m_esum +=  (cut ? 1. : 0. )*sqrt(pow( data1.first*data2.second,2)
                                     +pow(data1.second*data2.first,2)); // approximatif
    m_count+= data1.first*data2.first;
  } 
  void pushProduct(std::pair<double,double> data1,std::pair<double,double> data2){
    m_sum += data1.first*data2.first;
    m_esum += sqrt(pow( data1.first*data2.second,2)+pow(data1.second*data2.first,2)) ;
    m_count++;
  } 
  double value(){
    return (m_count>0) ? m_sum/m_count*100. : 0;
  }
  double error(){
    return (m_count>0) ? m_esum/m_count*100. : 0;
  }
  std::string name(){
    return m_name;
  }  
  int counter(){
    return m_count;
  }
  
private:
  double m_sum;
  double m_esum;
  double m_count;
  std::string m_name;
};

