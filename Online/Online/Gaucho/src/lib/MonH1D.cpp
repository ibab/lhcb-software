#include "Gaucho/MonH1D.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>
#include <GaudiUtils/HistoTableFormat.h>
#include "Gaucho/Misc.h"

MonH1D::MonH1D(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monH1D;
  m_dimPrefix="MonH1D";
  isLoaded = false;
  objectCreated = false;
  m_hist = 0;
  binCont = 0;
  binErr = 0;
  m_fSumw2 = 0;  
  m_axisLabelX="";
  m_axisLabelY="";
  baxisLabelX="false";
  baxisLabelY="false";
}

MonH1D::~MonH1D(){
  if (binCont!=0) delete []binCont;
  if (binErr!=0) delete []binErr;
 // if (bBinLabelX) {if (binLabelX!=0) delete []binLabelX;}
  if (m_fSumw2!=0) delete []m_fSumw2;
}

void MonH1D::setAidaHisto(AIDA::IHistogram1D* iHistogram1D){
  m_aidaHist = iHistogram1D;
  TH1D* hRoot = (TH1D*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaHist);
  sName = Gaudi::Utils::Histos::path ( m_aidaHist ) ;
  hRoot->SetName(sName.c_str());
  setHist(hRoot);
}

void MonH1D::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonH1D::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar,version);
}
  
void MonH1D::load2(boost::archive::binary_iarchive  & ar, const unsigned int version){
  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabelX;
  
  if (binCont == 0 ) binCont = new double[(nbinsx+2)];
    
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }
  
  if (binErr == 0 ) binErr = new double[(nbinsx+2)];
  
  for (int i = 0; i < (nbinsx+2) ; ++i){
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

  ar & m_fDimension;
  //ar & m_fIntegral;
  ar & m_fMaximum;
  ar & m_fMinimum;
  ar & m_fTsumw;
  ar & m_fTsumw2;
  ar & m_fTsumwx;
  ar & m_fTsumwx2;
  
  ar & m_fSumSize;
  
  if (m_fSumw2 == 0 ) m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
  
  //put axislabels at the end
  if (version > 0) {
     ar & baxisLabelX;
     ar & baxisLabelY;
  
     if (baxisLabelX){
         ar & m_axisLabelX;
     }
     if (baxisLabelY){
         ar & m_axisLabelY;
     } 
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
  ar & bBinLabelX;
  
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binErr[i];
  }
  if (bBinLabelX){
    for (int i = 0; i < (int)binLabelX.size() ; ++i){
      std::string labelX = binLabelX[i];
      ar & labelX;
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
  
  // axis labels at the end  
  ar & baxisLabelX;
  ar & baxisLabelY;
  
  
  if (baxisLabelX){
      ar & m_axisLabelX;
  }
  if (baxisLabelY){
      ar & m_axisLabelY;
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
  MsgStream msgStream = createMsgStream();
 // msgStream <<MSG::DEBUG<<"Creating TH1D " << name << endreq;
  if (m_hist == 0 )  m_hist = new TH1D(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax);
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
  //write the histogram with only its name
  std::vector<std::string> HistoFullName = Misc::splitString(m_hist->GetName(), "/");
  m_hist->Write(HistoFullName[HistoFullName.size()-1].c_str());
}
void MonH1D::loadObject(){
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;    
    return;  
  }
  m_hist->Reset();
  FriendOfTH1D * fot = (FriendOfTH1D *)m_hist; 
  
  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_hist->SetBinContent(i, binCont[i]);
  }
  
  m_hist->SetEntries(nEntries);
  
  for (int i = 0; i < (nbinsx+2) ; ++i){
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
  
  if (baxisLabelX) m_hist->GetXaxis()->SetTitle(m_axisLabelX.c_str());
      
  if (baxisLabelY) m_hist->GetYaxis()->SetTitle(m_axisLabelY.c_str());


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
  
  if (binCont == 0) binCont = new double[(nbinsx+2)];
  if (binErr == 0) binErr = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = ((double) (m_hist->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
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

  m_axisLabelX = m_hist->GetXaxis()->GetTitle();
  if (m_axisLabelX.length() > 0) baxisLabelX = true;
      
  m_axisLabelY = m_hist->GetYaxis()->GetTitle();
  if (m_axisLabelY.length() > 0) baxisLabelY = true;

  m_fDimension = fot->fDimension;
  //m_fIntegral = fot->fIntegral;
  m_fMaximum = fot->fMaximum;
  m_fMinimum = fot->fMinimum;
  m_fTsumw = fot->fTsumw;
  m_fTsumw2 = fot->fTsumw2;
  m_fTsumwx = fot->fTsumwx;
  m_fTsumwx2 = fot->fTsumwx2;
  m_fSumSize =  ((int)(fot->fSumw2.GetSize()));
  if (m_fSumw2 == 0) m_fSumw2 = new double[m_fSumSize];
  
  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }
  
  isLoaded = true;
}

void MonH1D::combine(MonObject * H){

  MsgStream msg = createMsgStream();
  if (H==0) {
     msg <<MSG::INFO<<"Combining a null MonH1D object. Returning." << endreq; 
     return;
  }
  // msg <<MSG::INFO<<"Combining "<<H->typeName() <<" EOR "<<H->endOfRun() << endreq;
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  if (H->endOfRun() != this->endOfRun()){
    msg <<MSG::DEBUG<<"Can't combine two objects with different endOfRun flags." << endreq;
    return;
  }
  
  if (!isLoaded){
    copyFrom(H);
    return;
  }
// msg <<MSG::INFO<<"Combining "<<H->typeName() <<" EOR "<<H->endOfRun() << endreq;
  // add the two histos
  MonH1D *HH = (MonH1D*)H;
  
  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
 // if (sTitle !=  HH->sTitle) matchParam = false;
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
  
  bBinLabelX = HH->bBinLabelX;
  if (bBinLabelX){
    binLabelX.clear();
    std::vector<std::string>::iterator it;
    for ( it=HH->binLabelX.begin() ; it < HH->binLabelX.end(); it++ ) {
      std::string labelX = *it;
      binLabelX.push_back(labelX);
    }
  }
  
  baxisLabelX = HH->baxisLabelX;
  if (baxisLabelX) {
     m_axisLabelX= HH->m_axisLabelX;
  }   
      
  baxisLabelY = HH->baxisLabelY;
  if (baxisLabelY) {
     m_axisLabelY= HH->m_axisLabelY;
  }  
  
  if (m_fSumSize == HH->m_fSumSize){
    for (int i=0 ; i < m_fSumSize; ++i) {
      m_fSumw2[i] += HH->m_fSumw2[i];
    }
  }
}

void MonH1D::copyFrom(MonObject * H){
  MsgStream msg = createMsgStream();
  std::vector<std::string>::iterator it;
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }

  MonH1D *HH = (MonH1D*)H;
  m_endOfRun = HH->endOfRun();
  m_comments=HH->comments();

  nbinsx = HH->nbinsx;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  nEntries = HH->nEntries;
  sName = HH->sName;
  sTitle = HH->sTitle;

  if (binCont != 0) delete []binCont;
  binCont = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = HH->binCont[i];
  }
  
  if (binErr != 0) delete []binErr;
  binErr = new double[(nbinsx+2)];
  
  for (int i = 0; i < (nbinsx+2) ; ++i){
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

  baxisLabelX = HH->baxisLabelX;
  if (baxisLabelX) {
     m_axisLabelX= HH->m_axisLabelX;
  }   
      
  baxisLabelY = HH->baxisLabelY;
  if (baxisLabelY) {
     m_axisLabelY= HH->m_axisLabelY;
  }  

  m_fDimension = HH->m_fDimension;
  m_fMaximum = HH->m_fMaximum;
  m_fMinimum = HH->m_fMinimum;
  m_fTsumw = HH->m_fTsumw;
  m_fTsumw2 = HH->m_fTsumw2;
  m_fTsumwx = HH->m_fTsumwx;
  m_fTsumwx2 = HH->m_fTsumwx2;
  m_fSumSize = HH->m_fSumSize;

  if (m_fSumw2 !=0) delete []m_fSumw2;
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
  m_fTsumw   = 0;
  m_fTsumw2  = 0;
  m_fTsumwx  = 0;
  m_fTsumwx2 = 0;
  //m_fSumSize = 0;
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
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msgStream <<binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:" << endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msgStream <<binErr[i]<<" ";
  }
  msgStream <<endreq;
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
  
      
  if (baxisLabelX) {
    msgStream <<MSG::INFO<<  "X axis label: " << m_axisLabelX << endreq;
  }   
      
  if (baxisLabelY) {
    msgStream <<MSG::INFO<<  "Y axis label: "<<  m_axisLabelY << endreq;
  }   
  
  
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
  
}
BOOST_CLASS_VERSION(MonH1D, 1)
