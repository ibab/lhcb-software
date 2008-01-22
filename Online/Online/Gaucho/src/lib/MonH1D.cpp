#include "Gaucho/MonH1D.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>

MonH1D::MonH1D(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonH1D";
  m_dimPrefix="MonH1D";
  isLoaded = false;
  objectCreated = false;
  m_hist = 0;
}
  
MonH1D::~MonH1D(){
}

void MonH1D::setAidaHisto(AIDA::IHistogram1D* iHistogram1D){
  m_aidaHist = iHistogram1D;
  setHist(Gaudi::Utils::Aida2ROOT::aida2root(m_aidaHist));
}

void MonH1D::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonH1D::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}
  
void MonH1D::load2(boost::archive::binary_iarchive  & ar){
  
  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  
  binCont = new double[(nbinsx+1)];
    
  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binCont[i];
  }
  
  binErr = new double[(nbinsx+1)];
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
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
  
  ar & m_fSumSize;
  
  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
  isLoaded = true;
}

void MonH1D::save2(boost::archive::binary_oarchive  & ar){
  if (m_hist != 0) splitObject();
  save3(ar);
}

void MonH1D::save3(boost::archive::binary_oarchive  & ar){
  if (!isLoaded) return;  

  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+1) ; ++i){
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
  
  ar & m_fSumSize;
  
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
}

void MonH1D::setHist(TH1D * tH1D){
  m_hist = tH1D;
  splitObject();
}
TH1D* MonH1D::hist(){  
  if (!objectCreated) createObject(sName);
  loadObject();
  return m_hist;
}
void MonH1D::createObject(std::string name){
  if (!isLoaded) return;
  m_hist = new TH1D(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax);
  objectCreated = true;
}
void MonH1D::createObject(){
  createObject(sName);
}
void MonH1D::write(){
  if (!objectCreated){ 
    createObject();
    loadObject();
  }
  m_hist->Write();
}
void MonH1D::loadObject(){
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    doOutputMsgStream(msgStream);
    return;  
  }
  m_hist->Reset();
  FriendOfTH1D * fot = (FriendOfTH1D *)m_hist; 
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
    m_hist->SetBinContent(i, binCont[i]);
  }
  
  m_hist->SetEntries(nEntries);
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
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
  
  fot->fSumw2.Set(m_fSumSize);
  
  for (int i=0 ; i < m_fSumSize; ++i) {
    fot->fSumw2[i] = m_fSumw2[i];
  }
}
  
void MonH1D::splitObject(){
  
  FriendOfTH1D * fot = (FriendOfTH1D *)m_hist; 

  nbinsx = m_hist->GetNbinsX();
  Xmin = m_hist->GetXaxis()->GetXmin();
  Xmax = m_hist->GetXaxis()->GetXmax();
  nEntries = (int) m_hist->GetEntries();

  const char *cName  = m_hist->GetName();
  sName  = std::string(cName);
  const char *cTitle  = m_hist->GetTitle();
  sTitle  = std::string(cTitle);

  binCont = new double[(nbinsx+1)];
  binErr = new double[(nbinsx+1)];
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
    binCont[i] = ((double) (m_hist->GetBinContent(i))); 
  }
    
  for (int i = 0; i < (nbinsx+1) ; ++i){
    binErr[i] = ((double) (m_hist->GetBinError(i))); 
  }

  m_fDimension = fot->fDimension;
  //m_fIntegral = fot->fIntegral;
  m_fMaximum = fot->fMaximum;
  m_fMinimum = fot->fMinimum;
  m_fTsumw = fot->fTsumw;
  m_fTsumw2 = fot->fTsumw2;
  m_fTsumwx = fot->fTsumwx;
  m_fTsumwx2 = fot->fTsumwx2;
  m_fSumSize =  ((int)(fot->fSumw2.GetSize()));
  m_fSumw2 = new double[m_fSumSize];
  
  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }
  
  isLoaded = true;
}

void MonH1D::combine(MonObject * H){
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
  MonH1D *HH = (MonH1D*)H;
    
  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
  if (sTitle !=  HH->sTitle) matchParam = false;
  if (!matchParam){
    MsgStream msgStream = createMsgStream();
    msgStream << MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msgStream << MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msgStream << MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin << endreq;
    msgStream << MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax << endreq;
    msgStream << MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  for (int i = 0; i < (nbinsx+1); ++i){
    binCont[i] += HH->binCont[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }
  nEntries += HH->nEntries;
}

void MonH1D::copyFrom(MonObject * H){
  if (H->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  MonH1D *HH = (MonH1D*)H;

  m_comments=HH->comments();

  nbinsx = HH->nbinsx;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  nEntries = HH->nEntries;
  sTitle = HH->sTitle;

  binCont = new double[(nbinsx+1)];

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binCont[i] = HH->binCont[i];
  }
  
  binErr = new double[(nbinsx+1)];
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
    binErr[i] = HH->binErr[i];
  }

  m_fDimension = HH->m_fDimension;
  m_fMaximum = HH->m_fMaximum;
  m_fMinimum = HH->m_fMinimum;
  m_fTsumw = HH->m_fTsumw;
  m_fTsumw2 = HH->m_fTsumw2;
  m_fTsumwx = HH->m_fTsumwx;
  m_fTsumwx2 = HH->m_fTsumwx2;
  m_fSumSize = HH->m_fSumSize;
  
  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }
  
  isLoaded = true;
}

void MonH1D::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject" << endreq;
    doOutputMsgStream(msgStream);
    return;
  }
  
  for (int i = 0; i < (nbinsx+1) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
  }
  
  nEntries = 0;
}

void MonH1D::print(){
  MonObject::print();
  if (!isLoaded) return;

  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"*********H1D Histogram***************"<<endreq;
  msgStream <<MSG::INFO<<"Title="<<sTitle << endreq;
  msgStream <<MSG::INFO<<"nEntries="<<nEntries << endreq;
  msgStream <<MSG::INFO<<"nbinsx="<<nbinsx << endreq;
  msgStream <<MSG::INFO<<"Xmin="<<Xmin << endreq;
  msgStream <<MSG::INFO<<"Xmax="<<Xmax << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinContents:" << endreq;
  for (int i = 0; i < (nbinsx+1) ; ++i){
    msgStream <<binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:" << endreq;
  for (int i = 0; i < (nbinsx+1) ; ++i){
    msgStream <<binErr[i]<<" ";
  }
  msgStream <<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Dimension="<< m_fDimension << endreq;
  //msgStream <<MSG::INFO<<"Integral="<< m_fIntegral << endreq;
  msgStream <<MSG::INFO<<"Maximum="<< m_fMaximum << endreq;
  msgStream <<MSG::INFO<<"Minimum="<< m_fMinimum << endreq;
  msgStream <<MSG::INFO<<"Tsumw="<< m_fTsumw << endreq;
  msgStream <<MSG::INFO<<"Tsumw2="<< m_fTsumw2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwx="<< m_fTsumwx << endreq;
  msgStream <<MSG::INFO<<"Tsumwx2="<< m_fTsumwx2 << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Sumw2:"<< endreq;
  for (int i=0;i<m_fSumSize;++i) {
    msgStream << m_fSumw2[i] <<" ";
  }
  msgStream <<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}

