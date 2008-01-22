#include "Gaucho/MonH2F.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>

MonH2F::MonH2F(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonH2F";
  m_dimPrefix="MonH2F";
  isLoaded = false;
  objectCreated = false;
  m_hist = 0;
}
  
MonH2F::~MonH2F(){
}

void MonH2F::setAidaHisto(AIDA::IHistogram2D* iHistogram2D){
  m_aidaHist = iHistogram2D;
  setHist((TH2F*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaHist));
}

void MonH2F::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonH2F::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}
  
void MonH2F::load2(boost::archive::binary_iarchive  & ar){
  
  ar & nbinsx;
  ar & nbinsy;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  
  binCont = new float[(nbinsx+1)*(nbinsy+1)];
    
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    ar & binCont[i];
  }
  
  binErr = new float[(nbinsx+1)*(nbinsy+1)];
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    ar & binErr[i];
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
  
  m_fSumw2 = new float[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
  isLoaded = true;
}

void MonH2F::save2(boost::archive::binary_oarchive  & ar){
  if (m_hist != 0) splitObject();
  save3(ar);
}

void MonH2F::save3(boost::archive::binary_oarchive  & ar){

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
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    ar & binErr[i];
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

void MonH2F::setHist(TH2F * tH2F){
  m_hist = tH2F;
  splitObject();
}
TH2F* MonH2F::hist(){  
  if (!objectCreated) createObject(sName);
  loadObject();
  return m_hist;
}
void MonH2F::createObject(std::string name){
  if (!isLoaded) return;
  m_hist = new TH2F(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax, nbinsy, Ymin, Ymax);
  objectCreated = true;
}
void MonH2F::createObject(){
  createObject(sName);
}
void MonH2F::write(){
  if (!objectCreated){ 
    createObject();
    loadObject();
  }
  m_hist->Write();
}
void MonH2F::loadObject(){
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  m_hist->Reset();
  FriendOfTH2F * fot = (FriendOfTH2F *)m_hist; 
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    m_hist->SetBinContent(i, binCont[i]);
  }
  
  m_hist->SetEntries(nEntries);
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    m_hist->SetBinError(i, binErr[i]);
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

void MonH2F::splitObject(){

  FriendOfTH2F * fot = (FriendOfTH2F *)m_hist; 

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

  binCont = new float[(nbinsx+1)*(nbinsy+1)];
  binErr = new float[(nbinsx+1)*(nbinsy+1)];

  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binCont[i] = ((float) (m_hist->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binErr[i] = ((float) (m_hist->GetBinError(i))); 
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
  m_fSumw2 = new float[m_fSumSize];

  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }

  isLoaded = true;
}

void MonH2F::combine(MonObject * H){
  if (H->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  if (!isLoaded){
    copyFrom(H);
    return;
  }

  // add the two histos
  MonH2F *HH = (MonH2F*)H;

  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (nbinsy != HH->nbinsy) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
  if (Ymin !=  HH->Ymin) matchParam = false;
  if (Ymax !=  HH->Ymax) matchParam = false;
  if (sTitle !=  HH->sTitle) matchParam = false;

  if (!matchParam){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msgStream <<MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msgStream <<MSG::ERROR<<"  nbinsy ="<<nbinsy << "; HH->nbinsy="<<HH->nbinsy << endreq;
    msgStream <<MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin<<endreq;;
    msgStream <<MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax<<endreq;;
    msgStream <<MSG::ERROR<<"  Ymin ="<<Xmin << "; HH->Xmin="<<HH->Xmin<<endreq;;
    msgStream <<MSG::ERROR<<"  Ymax ="<<Ymax << "; HH->Ymax="<<HH->Ymax<<endreq;;
    msgStream <<MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle<<std::endl;
    doOutputMsgStream(msgStream);
    return;
  }

  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binCont[i] += HH->binCont[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }

  nEntries += HH->nEntries;
}

void MonH2F::copyFrom(MonObject * H){
  if (H->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  MonH2F *HH = (MonH2F*)H;

  m_comments=HH->comments();

  nbinsx = HH->nbinsx;
  nbinsy = HH->nbinsy;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  Ymin = HH->Ymin;
  Ymax = HH->Ymax;
  nEntries = HH->nEntries;
  sTitle = HH->sTitle;
  
  binCont = new float[(nbinsx+1)*(nbinsy+1)];
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binCont[i] = HH->binCont[i];
  }
  
  binErr = new float[(nbinsx+1)*(nbinsy+1)];
  
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binErr[i] = HH->binErr[i];
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

  m_fSumw2 = new float[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }

  isLoaded = true;
}

void MonH2F::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject " << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
  }
  
  nEntries = 0;
}

void MonH2F::print(){
  MonObject::print();
  if (!isLoaded) return;

  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"********H2F Histogram***************"<<endreq;
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
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    msgStream << binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (nbinsx+1)*(nbinsy+1) ; ++i){
    msgStream << binErr[i] << " ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
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
  doOutputMsgStream(msgStream);
}
