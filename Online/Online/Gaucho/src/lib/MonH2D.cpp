#include "Gaucho/MonH2D.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>
#include <GaudiUtils/HistoTableFormat.h>
#include "Gaucho/Misc.h"

MonH2D::MonH2D(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monH2D;
  m_dimPrefix="MonH2D";
  isLoaded = false;
  objectCreated = false;
  m_hist = 0;
  binCont = 0;
  binErr = 0;
  m_fSumw2 = 0;  
}
  
MonH2D::~MonH2D(){
  if  (binCont!=0) delete []binCont;
  if (binErr !=0) delete []binErr;
 // if (bBinLabelX) {if (binLabelX!=0) delete []binLabelX;}
 // if (bBinLabelY) { if (binLabelY!=0) delete []binLabelY;}
  if (m_fSumw2!=0) delete []m_fSumw2;  
}

void MonH2D::setAidaHisto(AIDA::IHistogram2D* iHistogram2D){
  m_aidaHist = iHistogram2D;
  TH2D* hRoot = (TH2D*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaHist);
  sName = Gaudi::Utils::Histos::path ( m_aidaHist ) ;
  hRoot->SetName(sName.c_str());
  setHist(hRoot);
}

void MonH2D::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonH2D::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}
  
void MonH2D::load2(boost::archive::binary_iarchive  & ar){
  
  ar & nbinsx;
  ar & nbinsy;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabelX;
  ar & bBinLabelY;

  if (binCont == 0) binCont = new double[(nbinsx+2)*(nbinsy+2)];

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    ar & binCont[i];
  }

  if (binErr == 0) binErr = new double[(nbinsx+2)*(nbinsy+2)];

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    ar & binErr[i];
  }

  if (bBinLabelX){
    binLabelX.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      std::string labelX;
      ar & labelX;
      binLabelX.push_back(labelX);
    }
  }
  if (bBinLabelY){
    binLabelY.clear();
    for (int i = 1; i < (nbinsy+1) ; ++i){
      std::string labelY;
      ar & labelY;
      binLabelY.push_back(labelY);
    }
  }

  ar & m_fDimension;
  //ar & m_fIntegral;
  ar & m_fMaximum;
  ar & m_fMinimum;
  ar & m_fTsumw;
  ar & m_fTsumw2;
  ar & m_fTsumwx;
  ar & m_fTsumwx2;
  ar & m_fTsumwxy;
  ar & m_fTsumwy;
  ar & m_fTsumwy2;
  
  ar & m_fSumSize;
  
  if (m_fSumw2 == 0) m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
  isLoaded = true;
}

void MonH2D::save2(boost::archive::binary_oarchive  & ar){
  if (m_hist != 0) splitObject();
  save3(ar);
}

void MonH2D::save3(boost::archive::binary_oarchive  & ar){
  
  if (!isLoaded) return;  
  
  ar & nbinsx;
  ar & nbinsy;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabelX;
  ar & bBinLabelY;

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    ar & binErr[i];
  }
  if (bBinLabelX){
    binLabelX.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      std::string labelX;
      ar & labelX;
      binLabelX.push_back(labelX);
    }
  }
  if (bBinLabelX){
    binLabelX.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      std::string labelX;
      ar & labelX;
      binLabelX.push_back(labelX);
    }
  }
  ar & m_fDimension;
  //ar & m_fIntegral;
  ar & m_fMaximum;
  ar & m_fMinimum;
  ar & m_fTsumw;
  ar & m_fTsumw2;
  ar & m_fTsumwx;
  ar & m_fTsumwx2;
  ar & m_fTsumwxy;
  ar & m_fTsumwy;
  ar & m_fTsumwy2;
  
  ar & m_fSumSize;
  
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
}

void MonH2D::setHist(TH2D * tH2D){
  m_hist = tH2D;
  splitObject();
}
TH2D* MonH2D::hist(){  
  if (!objectCreated) createObject(sName);
  loadObject();
  return m_hist;
}
void MonH2D::createObject(std::string name){
  if (!isLoaded) return;
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::DEBUG<<"Creating TH1D " << name << endreq;
  if (m_hist == 0) m_hist = new TH2D(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax, nbinsy, Ymin, Ymax);
  objectCreated = true;
}
void MonH2D::createObject(){
  createObject(sName);
}
void MonH2D::write(){
  if (!objectCreated){ 
    createObject();
    loadObject();
  }
  //write the histogram with only its name
  std::vector<std::string> HistoFullName = Misc::splitString(m_hist->GetName(), "/");
  m_hist->Write(HistoFullName[HistoFullName.size()-1].c_str());

}
void MonH2D::loadObject(){
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    
    return;  
  }
  m_hist->Reset();
  FriendOfTH2D * fot = (FriendOfTH2D *)m_hist; 
  
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    m_hist->SetBinContent(i, binCont[i]);
  }
  
  m_hist->SetEntries(nEntries);
  
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    m_hist->SetBinError(i, binErr[i]);
  }

  if (bBinLabelX){
    std::vector<std::string>::iterator it;
    int i = 1;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      m_hist->GetXaxis()->SetBinLabel(i, (*it).c_str());
      i++;
    }
  }
  if (bBinLabelX){
    std::vector<std::string>::iterator it;
    int i = 1;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      m_hist->GetXaxis()->SetBinLabel(i, (*it).c_str());
      i++;
    }
  }

  fot->fDimension = m_fDimension;
  //ar & fot->fIntegral = m_fIntegral;
  fot->fMaximum = m_fMaximum;
  fot->fMinimum = m_fMinimum;
  fot->fTsumw = m_fTsumw;
  fot->fTsumw2 = m_fTsumw2;
  fot->fTsumwx = m_fTsumwx;
  fot->fTsumwx2 = m_fTsumwx2;
  fot->fTsumwxy = m_fTsumwxy;
  fot->fTsumwy = m_fTsumwy;
  fot->fTsumwy2 = m_fTsumwy2;

  fot->fSumw2.Set(m_fSumSize);

  for (int i=0 ; i < m_fSumSize; ++i) {
    fot->fSumw2[i] = m_fSumw2[i];
  }
}

void MonH2D::splitObject(){
  
  FriendOfTH2D * fot = (FriendOfTH2D *)m_hist; 

  nbinsx = m_hist->GetNbinsX();
  nbinsy = m_hist->GetNbinsY();
  Xmin = m_hist->GetXaxis()->GetXmin();
  Xmax = m_hist->GetXaxis()->GetXmax();
  Ymin = m_hist->GetYaxis()->GetXmin();
  Ymax = m_hist->GetYaxis()->GetXmax();
  nEntries = (int) m_hist->GetEntries();

  const char *cName  = m_hist->GetName();
  sName  = std::string(cName);
  const char *cTitle  = m_hist->GetTitle();
  sTitle  = std::string(cTitle);

  if (binCont==0) binCont = new double[(nbinsx+2)*(nbinsy+2)];
  if (binErr==0) binErr = new double[(nbinsx+2)*(nbinsy+2)];

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binCont[i] = ((double) (m_hist->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binErr[i] = ((double) (m_hist->GetBinError(i))); 
  }

  bBinLabelX = false;
  for (int i = 1; i < (nbinsx+1) ; ++i){
    std::string binLab = m_hist->GetXaxis()->GetBinLabel(i);
    if (binLab.length() > 0 ){
      bBinLabelX = true;
      break;
    }
  }
  
  if (bBinLabelX){
      binLabelX.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      binLabelX.push_back(m_hist->GetXaxis()->GetBinLabel(i));
    }
  }
  
  bBinLabelY = false;
  for (int i = 1; i < (nbinsx+1) ; ++i){
    std::string binLab = m_hist->GetYaxis()->GetBinLabel(i);
    if (binLab.length() > 0 ){
      bBinLabelY = true;
      break;
    }
  }
  
  if (bBinLabelY){
      binLabelY.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      binLabelY.push_back(m_hist->GetYaxis()->GetBinLabel(i));
    }
  }

  m_fDimension = fot->fDimension;
  //m_fIntegral = fot->fIntegral;
  m_fMaximum = fot->fMaximum;
  m_fMinimum = fot->fMinimum;
  m_fTsumw = fot->fTsumw;
  m_fTsumw2 = fot->fTsumw2;
  m_fTsumwx = fot->fTsumwx;
  m_fTsumwx2 = fot->fTsumwx2;
  m_fTsumwxy = fot->fTsumwxy;
  m_fTsumwy = fot->fTsumwy;
  m_fTsumwy2 = fot->fTsumwy2;

  m_fSumSize =  ((int)(fot->fSumw2.GetSize()));
  if (m_fSumw2==0) m_fSumw2 = new double[m_fSumSize];

  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }

  isLoaded = true;
}

void MonH2D::combine(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
 // if (H->endOfRun() != this->endOfRun()){
 //   msg <<MSG::DEBUG<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
 //   return;
 // }
  if (!isLoaded){
    copyFrom(H);
    return;
  }

  // add the two histos
  MonH2D *HH = (MonH2D*)H;

  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (nbinsy != HH->nbinsy) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
  if (Ymin !=  HH->Ymin) matchParam = false;
  if (Ymax !=  HH->Ymax) matchParam = false;
  if (sTitle !=  HH->sTitle) matchParam = false;

  if (!matchParam){
    msg <<MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msg <<MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msg <<MSG::ERROR<<"  nbinsy ="<<nbinsy << "; HH->nbinsy="<<HH->nbinsy << endreq;
    msg <<MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin<<endreq;;
    msg <<MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax<<endreq;;
    msg <<MSG::ERROR<<"  Ymin ="<<Xmin << "; HH->Xmin="<<HH->Xmin<<endreq;;
    msg <<MSG::ERROR<<"  Ymax ="<<Ymax << "; HH->Ymax="<<HH->Ymax<<endreq;;
    msg <<MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle<<std::endl;
    
    return;
  }

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binCont[i] += HH->binCont[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }

  nEntries += HH->nEntries;

  m_fTsumw += HH->m_fTsumw;
  m_fTsumw2 += HH->m_fTsumw2;
  m_fTsumwx += HH->m_fTsumwx;
  m_fTsumwx2 += HH->m_fTsumwx2;
  m_fTsumwxy += HH->m_fTsumwxy;
  m_fTsumwy += HH->m_fTsumwy;
  m_fTsumwy2 += HH->m_fTsumwy2;
  
  bBinLabelX = HH->bBinLabelX;
  if (bBinLabelX){
    binLabelX.clear();
    std::vector<std::string>::iterator it;
    for ( it=HH->binLabelX.begin() ; it < HH->binLabelX.end(); it++ ) {
      std::string labelX = *it;
      binLabelX.push_back(labelX);
    }
  }
  
  bBinLabelY = HH->bBinLabelY;
  if (bBinLabelY){
    binLabelY.clear();
    std::vector<std::string>::iterator it;
    for ( it=HH->binLabelY.begin() ; it < HH->binLabelY.end(); it++ ) {
      std::string labelY = *it;
      binLabelY.push_back(labelY);
    }
  }
}

void MonH2D::copyFrom(MonObject * H){
    MsgStream msgStream = createMsgStream();
  if (H->typeName() != this->typeName()){
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  std::vector<std::string>::iterator it;
  MonH2D *HH = (MonH2D*)H;
  m_endOfRun = HH->endOfRun();
  m_comments=HH->comments();

  nbinsx = HH->nbinsx;
  nbinsy = HH->nbinsy;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  Ymin = HH->Ymin;
  Ymax = HH->Ymax;
  nEntries = HH->nEntries;
  sName = HH->sName;
  sTitle = HH->sTitle;
  
  if (binCont != 0) delete []binCont;
  binCont = new double[(nbinsx+2)*(nbinsy+2)];
  
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binCont[i] = HH->binCont[i];
  }
  
  if (binErr != 0) delete []binErr;
  binErr = new double[(nbinsx+2)*(nbinsy+2)];
  
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binErr[i] = HH->binErr[i];
  }

  bBinLabelX = HH->bBinLabelX;

  if (bBinLabelX){
    binLabelX.clear();
    for ( it=HH->binLabelX.begin() ; it < HH->binLabelX.end(); it++ ) {
      std::string labelX = *it;
      binLabelX.push_back(labelX);
    }
  }

  bBinLabelY = HH->bBinLabelY;

  if (bBinLabelY){
    binLabelY.clear();
    for ( it=HH->binLabelY.begin() ; it < HH->binLabelY.end(); it++ ) {
      std::string labelY = *it;
      binLabelY.push_back(labelY);
    }
  }

  m_fDimension = HH->m_fDimension;
  m_fMaximum = HH->m_fMaximum;
  m_fMinimum = HH->m_fMinimum;
  m_fTsumw = HH->m_fTsumw;
  m_fTsumw2 = HH->m_fTsumw2;
  m_fTsumwx = HH->m_fTsumwx;
  m_fTsumwx2 = HH->m_fTsumwx2;
  m_fTsumwxy = HH->m_fTsumwxy;
  m_fTsumwy = HH->m_fTsumwy;
  m_fTsumwy2 = HH->m_fTsumwy2;

  m_fSumSize = HH->m_fSumSize;

  if (m_fSumw2 != 0) delete []m_fSumw2;
  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }
  isLoaded = true;
}

void MonH2D::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject " << endreq;
    
    return;
  }

  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
  }
  
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = 0;
  }
  
  nEntries = 0;
  m_fTsumw   = 0;
  m_fTsumw2  = 0;
  m_fTsumwx  = 0;
  m_fTsumwx2 = 0;
  m_fTsumwy  = 0;
  m_fTsumwy2 = 0;
  m_fTsumwxy = 0;

//  m_fSumSize = 0;
}

void MonH2D::print(){
  MonObject::print();
  if (!isLoaded) return;

  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"********H2D Histogram***************"<<endreq;
  msgStream <<MSG::INFO<<"Title="<<sTitle<<endreq;
  msgStream <<MSG::INFO<<"nEntries="<<nEntries<<endreq;
  msgStream <<MSG::INFO<<"nbinsx="<<nbinsx<<endreq;
  msgStream <<MSG::INFO<<"nbinsy="<<nbinsy<<endreq;
  msgStream <<MSG::INFO<<"Xmin="<<Xmin<<endreq;
  msgStream <<MSG::INFO<<"Xmax="<<Xmax<<endreq;
  msgStream <<MSG::INFO<<"Ymin="<<Ymin<<endreq;
  msgStream <<MSG::INFO<<"Ymax="<<Ymax<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinContents:"<<endreq;
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    msgStream << binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (nbinsx+2)*(nbinsy+2) ; ++i){
    msgStream << binErr[i] << " ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  if (bBinLabelX){
    msgStream <<MSG::INFO<<"BinLabelsX:"<<endreq;
    std::vector<std::string>::iterator it;
    msgStream <<MSG::INFO << "size:" << binLabelX.size() << endreq;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      msgStream <<MSG::INFO<< *it << " " ;
    }
    msgStream << endreq;
    msgStream <<MSG::INFO<<"*************************************"<<endreq;
  }
  if (bBinLabelY){
    msgStream <<MSG::INFO<<"BinLabelsY:"<<endreq;
    std::vector<std::string>::iterator it;
    msgStream <<MSG::INFO << "size:" << binLabelY.size() << endreq;
    for ( it=binLabelY.begin() ; it < binLabelY.end(); it++ ) {
      msgStream <<MSG::INFO<< *it << " " ;
    }
    msgStream << endreq;
    msgStream <<MSG::INFO<<"*************************************"<<endreq;
  }
  msgStream <<MSG::INFO<<"Dimension="<< m_fDimension << endreq;
  //msgStream <<MSG::INFO <<"Integral="<< m_fIntegral << endreq;
  msgStream <<MSG::INFO<<"Maximum="<< m_fMaximum << endreq;
  msgStream <<MSG::INFO<<"Minimum="<< m_fMinimum << endreq;
  msgStream <<MSG::INFO<<"Tsumw="<< m_fTsumw << endreq;
  msgStream <<MSG::INFO<<"Tsumw2="<< m_fTsumw2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwx="<< m_fTsumwx << endreq;
  msgStream <<MSG::INFO<<"Tsumwx2="<< m_fTsumwx2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwxy="<< m_fTsumwxy << endreq;
  msgStream <<MSG::INFO<<"Tsumwy="<< m_fTsumwy << endreq;
  msgStream <<MSG::INFO<<"Tsumwy2="<< m_fTsumwy2 << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Sumw2:"<<endreq;
  for (int i = 0; i < m_fSumSize; ++i) {
    msgStream << m_fSumw2[i] <<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  
}
