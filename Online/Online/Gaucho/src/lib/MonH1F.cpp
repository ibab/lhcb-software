#include "Gaucho/MonH1F.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>

MonH1F::MonH1F(IMessageSvc* msgSvc, const std::string& source, int version):
  MonObject(msgSvc, source, version)
{
  isLoaded = false ;
  objectCreated = false;
  m_hist = 0;
  m_typeName = s_monH1F;
  m_dimPrefix = "MonH1F";
  binCont = 0;
  binErr = 0;
  binLabel = 0;
  m_fSumw2 = 0;  
}

MonH1F::~MonH1F(){
//   MsgStream msgStream = createMsgStream();
//   msgStream <<MSG::DEBUG<<"deleting binCont" << endreq;
  delete binCont;
//   msgStream <<MSG::DEBUG<<"deleting binErr" << endreq;
  delete binErr;
//   msgStream <<MSG::DEBUG<<"deleting binLabel" << endreq;
  if (bBinLabel) delete binLabel;
//   msgStream <<MSG::DEBUG<<"deleting m_fSumw2" << endreq;
  delete m_fSumw2;
// BUGG...I dont know yet why I can't do it..
/*  if (m_hist!=0) {
    msgStream <<MSG::DEBUG<<"deleting m_hist" << endreq;
    delete m_hist; 
    msgStream <<MSG::DEBUG<<"m_hist = 0" << endreq;
    m_hist = 0;
    msgStream <<MSG::DEBUG<<"after deleting m_hist" << endreq;
  }*/
}

void MonH1F::setAidaHisto(AIDA::IHistogram1D* iHistogram1D){
  m_aidaHist = iHistogram1D;
  setHist((TH1F*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaHist));
}

void MonH1F::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonH1F::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}

void MonH1F::load2(boost::archive::binary_iarchive  & ar){
  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabel;

//  std::cout << "==============================================================" << std::endl;
//  std::cout << "========================LOAD==================================" << std::endl;
//  std::cout << "  nEntries = " <<  nEntries << std::endl;
//  std::cout << "==============================================================" << std::endl;
  
  if (binCont == 0) binCont = new float[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }

  if (binErr == 0) binErr = new float[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binErr[i];
  }

  if (bBinLabel){
    if (binLabel == 0) binLabel = new std::string[(nbinsx+2)];

    for (int i = 0; i < (nbinsx+2) ; ++i){
      ar & binLabel[i];
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

  ar & m_fSumSize;

  if (m_fSumw2 == 0) m_fSumw2 = new float[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
  isLoaded = true;
}

void MonH1F::save2(boost::archive::binary_oarchive  & ar){
  if (0 != m_hist) splitObject();
  save3(ar);
}

void MonH1F::save3(boost::archive::binary_oarchive  & ar){
  if (!isLoaded) return;  

  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabel;

/*  std::cout << "==============================================================" << std::endl;
  std::cout << "========================SAVE==================================" << std::endl;
  std::cout << "  nEntries = " <<  nEntries << std::endl;
  std::cout << "==============================================================" << std::endl;*/
      
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binErr[i];
  }
  if (bBinLabel){
    for (int i = 0; i < (nbinsx+2) ; ++i){
      ar & binLabel[i];
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

  ar & m_fSumSize;

  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
}

void MonH1F::setHist(TH1F * tH1F){
  m_hist = tH1F;
  splitObject();
}
TH1F* MonH1F::hist(){  
  if (!objectCreated) createObject(sName);
  loadObject();
  return m_hist;
}
void MonH1F::createObject(std::string name){
  if (!isLoaded) return;
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"Creating TH1F " << name << endreq;
  if (m_hist == 0) m_hist = new TH1F(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax);
  objectCreated = true;
}
void MonH1F::createObject(){
  createObject(sName);
}
void MonH1F::write(){
  MsgStream msgStream = createMsgStream();
  if (!objectCreated){
    createObject();
    loadObject();
  }
  m_hist->Write();
}
void MonH1F::loadObject(){
  MsgStream msgStream = createMsgStream();
  if (!objectCreated) {
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    return;
  }
  m_hist->Reset();
  FriendOfTH1F * fot = (FriendOfTH1F *)m_hist; 
  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_hist->SetBinContent(i, binCont[i]);
  }
  m_hist->SetEntries(nEntries);
  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_hist->SetBinError(i, binErr[i]);
  }
  if (bBinLabel){
    for (int i = 0; i < (nbinsx+2) ; ++i){
      m_hist->GetXaxis()->SetBinLabel(i, binLabel[i].c_str());
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

  fot->fSumw2.Set(m_fSumSize);
  for (int i=0 ; i < m_fSumSize; ++i) {
    fot->fSumw2[i] = m_fSumw2[i];
  }
}

void MonH1F::splitObject(){

  FriendOfTH1F * fot = (FriendOfTH1F *)m_hist; 

  nbinsx = m_hist->GetNbinsX();
  Xmin = m_hist->GetXaxis()->GetXmin();
  Xmax = m_hist->GetXaxis()->GetXmax();
  nEntries = (int) m_hist->GetEntries();

  const char *cName  = m_hist->GetName();
  sName  = std::string(cName);
  
  const char *cTitle  = m_hist->GetTitle();
  sTitle  = std::string(cTitle);

  if (binCont==0) binCont = new float[(nbinsx+2)];
  if (binErr==0) binErr = new float[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = ((float) (m_hist->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binErr[i] = ((float) (m_hist->GetBinError(i)));
  }

  bBinLabel = false;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    std::string binLab = m_hist->GetXaxis()->GetBinLabel(i);
    if (binLab.length() > 0 ){
      bBinLabel = true;
      break;
    }
  }

  if (bBinLabel){
    if (binLabel==0)  binLabel = new std::string[(nbinsx+2)];
    for (int i = 0; i < (nbinsx+2) ; ++i){
      binLabel[i] = m_hist->GetXaxis()->GetBinLabel(i);
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
  m_fSumSize =  ((int)(fot->fSumw2.GetSize()));
  if (m_fSumw2==0) m_fSumw2 = new float[m_fSumSize];
  
  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }
  
  isLoaded = true;
}

void MonH1F::combine(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  if (H->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  
  if (!isLoaded){
    copyFrom(H);
    return;
  }

  // add the two histos
  MonH1F *HH = (MonH1F*)H;

  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
  if (sTitle !=  HH->sTitle) matchParam = false;
  if (!matchParam){
    msg << MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msg << MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msg << MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin << endreq;
    msg << MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax << endreq;
    msg << MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle << endreq;
    return;
  }

  for (int i = 0; i < (nbinsx+2); ++i){
    binCont[i] += HH->binCont[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }

  nEntries += HH->nEntries;

  m_fTsumw += HH->m_fTsumw;
  m_fTsumw2 += HH->m_fTsumw2;
  m_fTsumwx += HH->m_fTsumwx;
  m_fTsumwx2 += HH->m_fTsumwx2;
  
  if (m_fSumSize == HH->m_fSumSize){
    for (int i=0 ; i < m_fSumSize; ++i) {
      m_fSumw2[i] += HH->m_fSumw2[i];
    }
  }
}

void MonH1F::copyFrom(MonObject * H){
  MsgStream msg = createMsgStream();
  
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }

  MonH1F *HH = (MonH1F*)H;
  m_endOfRun = HH->endOfRun();
  m_comments=HH->comments();

  nbinsx = HH->nbinsx;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  nEntries = HH->nEntries;
  sName = HH->sName;
  sTitle = HH->sTitle;

  if (binCont != 0) delete binCont;
  binCont = new float[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = HH->binCont[i];
  }

  if (binErr != 0) delete binErr;
  binErr = new float[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binErr[i] = HH->binErr[i];
  }

  bBinLabel = HH->bBinLabel;

  if (bBinLabel){
    if (binLabel != 0) delete binLabel;
    binLabel = new std::string[(nbinsx+2)];
    for (int i = 0; i < (nbinsx+2) ; ++i){
      binLabel[i] = HH->binLabel[i];
    }
  }

  m_fDimension = HH->m_fDimension;
  m_fMaximum = HH->m_fMaximum;
  m_fMinimum = HH->m_fMinimum;
  m_fTsumw = HH->m_fTsumw;
  m_fTsumw2 = HH->m_fTsumw2;
  m_fTsumwx = HH->m_fTsumwx;
  m_fTsumwx2 = HH->m_fTsumwx2;
  m_fSumSize = HH->m_fSumSize;

  if (m_fSumw2 != 0) delete m_fSumw2;
  m_fSumw2 = new float[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }

  isLoaded = true;
}

void MonH1F::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject" << endreq;
    return;
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
  }

  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = 0;
  }
  
  nEntries = 0;
}

void MonH1F::print(){
  MonObject::print();
  if (!isLoaded) return;
  
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"********H1F Histogram***************"<<endreq;
  msgStream <<MSG::INFO<<"Title="<<sTitle<<endreq;
  msgStream <<MSG::INFO<<"nEntries="<<nEntries<<endreq;
  msgStream <<MSG::INFO<<"nbinsx="<<nbinsx<<endreq;
  msgStream <<MSG::INFO<<"Xmin="<<Xmin<<endreq;
  msgStream <<MSG::INFO<<"Xmax="<<Xmax<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinContents:"<<endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msgStream <<binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msgStream <<binErr[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  if (bBinLabel){
    msgStream <<MSG::INFO<<"BinLabels:"<<endreq;
    for (int i = 0; i < (nbinsx+2) ; ++i){
      msgStream <<binLabel[i]<<" ";
    }
    msgStream << endreq;
    msgStream <<MSG::INFO<<"*************************************"<<endreq;
  }
  msgStream <<MSG::INFO<<"Dimension="<< m_fDimension << endreq;
  //msgStream << MSG::INFO<<"Integral="<< m_fIntegral << endreq;
  msgStream <<MSG::INFO<<"Maximum="<< m_fMaximum << endreq;
  msgStream <<MSG::INFO<<"Minimum="<< m_fMinimum << endreq;
  msgStream <<MSG::INFO<<"Tsumw="<< m_fTsumw << endreq;
  msgStream <<MSG::INFO<<"Tsumw2="<< m_fTsumw2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwx="<< m_fTsumwx << endreq;
  msgStream <<MSG::INFO<<"Tsumwx2="<< m_fTsumwx2 << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Sumw2:"<<endreq;
  for (int i=0;i<m_fSumSize;++i) {
    msgStream << m_fSumw2[i] <<" ";
  }
  msgStream << endreq;
  msgStream << MSG::INFO<<"*************************************"<<endreq;
  
}

void MonH1F::printObject(){
  MsgStream msgStream = createMsgStream();
  if (0==m_hist) {
    msgStream <<MSG::INFO<<"Sorry, The Histogram is 0...!!! "<<endreq;
    return;
  }
  if (NULL==m_hist) {
    msgStream <<MSG::INFO<<"Sorry, The Histogram is NULL...!!! "<<endreq;
    return;
  }
  FriendOfTH1F * fot = (FriendOfTH1F *)m_hist; 
  
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"********H1F Histogram***************"<<endreq;
  msgStream <<MSG::INFO<<"Title="<<m_hist->GetTitle()<<endreq;
  msgStream <<MSG::INFO<<"nEntries="<<(int) m_hist->GetEntries()<<endreq;
  msgStream <<MSG::INFO<<"nbinsx="<<m_hist->GetNbinsX()<<endreq;
  msgStream <<MSG::INFO<<"Xmin="<<m_hist->GetXaxis()->GetXmin()<<endreq;
  msgStream <<MSG::INFO<<"Xmax="<<m_hist->GetXaxis()->GetXmax()<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinContents:"<<endreq;
  for (int i = 0; i < (m_hist->GetNbinsX()+2) ; ++i){
    msgStream <<((float) (m_hist->GetBinContent(i)))<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (m_hist->GetNbinsX()+2) ; ++i){
    msgStream <<((float) (m_hist->GetBinError(i)))<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  bool bBinLabel = false;
  for (int i = 0; i < (m_hist->GetNbinsX()+2) ; ++i){
    std::string binLab = m_hist->GetXaxis()->GetBinLabel(i);
    if (binLab.length() > 0 ){
      bBinLabel = true;
      break;
    }
  }

  if (bBinLabel){
    msgStream <<MSG::INFO<<"BinLabels:"<<endreq;
    for (int i = 0; i < (m_hist->GetNbinsX()+2) ; ++i){
      msgStream <<m_hist->GetXaxis()->GetBinLabel(i)<<" ";
    }
    msgStream << endreq;
    msgStream <<MSG::INFO<<"*************************************"<<endreq;
  }
  msgStream <<MSG::INFO<<"Dimension="<< fot->fDimension << endreq;
  //msgStream << MSG::INFO<<"Integral="<< m_fIntegral << endreq;
  msgStream <<MSG::INFO<<"Maximum="<< fot->fMaximum << endreq;
  msgStream <<MSG::INFO<<"Minimum="<< fot->fMinimum << endreq;
  msgStream <<MSG::INFO<<"Tsumw="<< fot->fTsumw << endreq;
  msgStream <<MSG::INFO<<"Tsumw2="<< fot->fTsumw2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwx="<< fot->fTsumwx << endreq;
  msgStream <<MSG::INFO<<"Tsumwx2="<< fot->fTsumwx2 << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Sumw2:"<<endreq;
  for (int i=0;i<((int)(fot->fSumw2.GetSize()));++i) {
    msgStream << fot->fSumw2[i] <<" ";
  }
  msgStream << endreq;
  msgStream << MSG::INFO<<"*************************************"<<endreq;

}
