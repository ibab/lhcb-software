#include <TChain.h>
#include <TROOT.h>
#include <iostream>

class TChainer{
public:
  TChainer(){}
  virtual ~TChainer(){}
  void    add(std::string path){m_chains.push_back(path);}
  TChain* get(std::string tup,std::string macro="newChain",std::string ext=".C"){
    if( 0 == m_chains.size() )return NULL;
    TChain* train= new TChain(tup.c_str());

    int count=0;
    int nchain=0;
    for( std::vector<std::string>::iterator c = m_chains.begin() ; m_chains.end() != c ; c++ ){
      std::cout <<"loading macro " << *c <<"/"<< macro << ext << std::endl;
      gROOT->ProcessLine(std::string(".L "+ *c +"/"+ macro+ext).c_str());
      TChain* bunch=(TChain*)  gROOT->ProcessLine(std::string(macro+"(\""+tup+"\")").c_str());
      int num = train->Add( bunch );
      count += num;
      nchain++;
      std::cout<<"Added " << num << " files to the chain" <<std::endl;
      gROOT->ProcessLine(std::string(".U "+ *c +"/"+macro+ext).c_str());
    }
    std::cout<<" " << std::endl;
    std::cout<<" ======= TOTAL :  " << count << " root files chained from " << nchain << " input pathes" << std::endl;
    for( std::vector<std::string>::iterator c = m_chains.begin() ; m_chains.end() != c ; c++ ){
      std::cout <<"    -  " << *c <<std::endl;
    }
    return train;
  }
private:
  std::vector<std::string> m_chains;
  ClassDef(TChainer,1);
};

