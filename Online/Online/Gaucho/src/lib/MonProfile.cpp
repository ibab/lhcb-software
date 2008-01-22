#include "Gaucho/MonProfile.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>

MonProfile::MonProfile(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName="MonProfile";
  m_dimPrefix="MonP1";
  isLoaded = false;
  objectCreated = false;
}

MonProfile::~MonProfile(){
}

void MonProfile::setAidaProfile(AIDA::IProfile1D* iProfile1D){
  m_aidaProfile = iProfile1D;
  setProfile((TProfile*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaProfile));
}

void MonProfile::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  save2(ar);
}

void MonProfile::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  load2(ar);
}

void MonProfile::load2(boost::archive::binary_iarchive  & ar){

  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
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

  binEntries = new double[(nbinsx+1)];

  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binEntries[i];
  }

  ar & m_fDimension;
  //ar & m_fIntegral;
  ar & m_fMaximum;
  ar & m_fMinimum;
  ar & m_fTsumw;
  ar & m_fTsumw2;
  ar & m_fTsumwx;
  ar & m_fTsumwx2;
  ar & m_fTsumwy;
  ar & m_fTsumwy2;

  ar & m_fSumSize;

  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }

  isLoaded = true;

}

void MonProfile::save2(boost::archive::binary_oarchive  & ar){
  if (m_profile != 0) splitObject();
  save3(ar);
}

void MonProfile::save3(boost::archive::binary_oarchive  & ar) {

  if (!isLoaded) return;  

  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
  ar & nEntries;
  ar & sName;
  ar & sTitle; 

  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binErr[i];
  }
  for (int i = 0; i < (nbinsx+1) ; ++i){
    ar & binEntries[i];
  }

  ar & m_fDimension;
  //ar & m_fIntegral;
  ar & m_fMaximum;
  ar & m_fMinimum;
  ar & m_fTsumw;
  ar & m_fTsumw2;
  ar & m_fTsumwx;
  ar & m_fTsumwx2;
  ar & m_fTsumwy;
  ar & m_fTsumwy2;

  ar & m_fSumSize;

  for (int i = 0; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }
}

void MonProfile::setProfile(TProfile * tProfile){
  m_profile = tProfile;
  splitObject();
}

TProfile* MonProfile::profile(){  
  if (!objectCreated) createObject(sName);
  loadObject();
  return m_profile;
}
void MonProfile::createObject(std::string name){
  if (!isLoaded) return;
  m_profile = new TProfile(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax, Ymin, Ymax);
  objectCreated = true;
}
void MonProfile::createObject(){
  createObject(sName);
}
void MonProfile::write(){
  if (!objectCreated){ 
    createObject();
    loadObject();
  }
  m_profile->Write();
}
void MonProfile::loadObject(){
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    doOutputMsgStream(msgStream);
    return;  
  }
  m_profile->Reset();
  FriendOfTProfile * fot = (FriendOfTProfile *)m_profile; 

  for (int i = 0; i < (nbinsx+1) ; ++i){
    m_profile->SetBinContent(i, binCont[i]);
  }

  //SetEntries must be after SetBinContents
  m_profile->SetEntries(nEntries);

  for (int i = 0; i < (nbinsx+1) ; ++i){
    m_profile->SetBinError(i, binErr[i]);
  }

  for (int i = 0; i < (nbinsx+1) ; ++i){
    m_profile->SetBinEntries(i, binEntries[i]);
  }

  fot->fDimension = m_fDimension;
  //ar & fot->fIntegral = m_fIntegral;
  fot->fMaximum = m_fMaximum;
  fot->fMinimum = m_fMinimum;
  fot->fTsumw = m_fTsumw;
  fot->fTsumw2 = m_fTsumw2;
  fot->fTsumwx = m_fTsumwx;
  fot->fTsumwx2 = m_fTsumwx2;
  fot->fTsumwy = m_fTsumwy;
  fot->fTsumwy2 = m_fTsumwy2;

  fot->fSumw2.Set(m_fSumSize);

  for (int i=0 ; i < m_fSumSize; ++i) {
    fot->fSumw2[i] = m_fSumw2[i];
  }
}

void MonProfile::splitObject(){

  FriendOfTProfile * fot = (FriendOfTProfile *)m_profile; 

  nbinsx = m_profile->GetNbinsX();
  Xmin = m_profile->GetXaxis()->GetXmin();
  Xmax = m_profile->GetXaxis()->GetXmax();
  Ymin = m_profile->GetYmin();
  Ymax = m_profile->GetYmax();
  nEntries = (int) m_profile->GetEntries();

  const char *cName  = m_profile->GetName();
  sName  = std::string(cName);
  const char *cTitle  = m_profile->GetTitle();
  sTitle  = std::string(cTitle);

  binCont = new double[(nbinsx+1)];
  binErr = new double[(nbinsx+1)];
  binEntries = new double[(nbinsx+1)];

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binCont[i] = ((double) (m_profile->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binErr[i] = ((double) (m_profile->GetBinError(i))); 
  }

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binEntries[i] = ((double) (m_profile->GetBinEntries(i))); 
  }

  m_fDimension = fot->fDimension;
  //ar & fot->fIntegral = m_fIntegral;
  m_fMaximum = fot->fMaximum;
  m_fMinimum = fot->fMinimum;
  m_fTsumw = fot->fTsumw;
  m_fTsumw2 = fot->fTsumw2;
  m_fTsumwx = fot->fTsumwx;
  m_fTsumwx2 = fot->fTsumwx2;
  m_fTsumwy = fot->fTsumwy;
  m_fTsumwy2 = fot->fTsumwy2;
  m_fSumSize =  ((int)(fot->fSumw2.GetSize()));
  m_fSumw2 = new double[m_fSumSize];

  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }

  isLoaded = true;

}

void MonProfile::combine(MonObject * H){
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
  MonProfile *HH = (MonProfile*)H;

  bool matchParam = true;
  if (nbinsx != HH->nbinsx) matchParam = false;
  if (Xmin !=  HH->Xmin) matchParam = false;
  if (Xmax !=  HH->Xmax) matchParam = false;
  if (Ymin !=  HH->Ymin) matchParam = false;
  if (Ymax !=  HH->Ymax) matchParam = false;
  if (sTitle !=  HH->sTitle) matchParam = false;

  if (!matchParam){
    MsgStream msgStream = createMsgStream();
    msgStream << MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msgStream << MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msgStream << MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin << endreq;
    msgStream << MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax << endreq;
    msgStream << MSG::ERROR<<"  Ymin ="<<Xmin << "; HH->Ymin="<<HH->Xmin << endreq;
    msgStream << MSG::ERROR<<"  Ymax ="<<Xmax << "; HH->Ymax="<<HH->Xmax << endreq;
    msgStream << MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  for (int i = 0; i < (nbinsx+1); ++i){
    binCont[i] += HH->binCont[i];
    binEntries[i] += HH->binEntries[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }

  nEntries += HH->nEntries;
}

void MonProfile::copyFrom(MonObject * H){
  if (H->typeName() != this->typeName()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  MonProfile *HH = (MonProfile*)H;

  nbinsx = HH->nbinsx;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  Ymin = HH->Ymin;
  Ymax = HH->Ymax;
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

  binEntries = new double[(nbinsx+1)];

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binEntries[i] = HH->binEntries[i];
  }

  m_fDimension = HH->m_fDimension;
  //m_fIntegral = HH->m_fIntegral;
  m_fMaximum = HH->m_fMaximum;
  m_fMinimum = HH->m_fMinimum;
  m_fTsumw = HH->m_fTsumw;
  m_fTsumw2 = HH->m_fTsumw2;
  m_fTsumwx = HH->m_fTsumwx;
  m_fTsumwx2 = HH->m_fTsumwx2;
  m_fTsumwy = HH->m_fTsumwy;
  m_fTsumwy2 = HH->m_fTsumwy2;
  m_fSumSize = HH->m_fSumSize;
  
  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }

  isLoaded = true;
}

void MonProfile::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject" << endreq;
    doOutputMsgStream(msgStream);
    return;
  }

  for (int i = 0; i < (nbinsx+1) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
    binEntries[i] = 0; 
  }

  nEntries = 0;
}

void MonProfile::print(){
  MonObject::print();
  if (!isLoaded) return;

  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"**************Profile 1D*************"<<endreq;
  msgStream <<MSG::INFO<<"Title="<<sTitle<<endreq;
  msgStream <<MSG::INFO<<"nEntries="<<nEntries<<endreq;
  msgStream <<MSG::INFO<<"nbinsx="<<nbinsx<<endreq;
  msgStream <<MSG::INFO<<"Xmin="<<Xmin<<endreq;
  msgStream <<MSG::INFO<<"Xmax="<<Xmax<<endreq;
  msgStream <<MSG::INFO<<"Ymin="<<Ymin<<endreq;
  msgStream <<MSG::INFO<<"Ymax="<<Ymax<<endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinContents:"<<endreq;
  for (int i = 0; i < (nbinsx+1) ; ++i){
    msgStream <<binCont[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (nbinsx+1) ; ++i){
    msgStream << binErr[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"BinEntries:"<<endreq;
  for (int i = 0; i < (nbinsx+1) ; ++i){
    msgStream << binEntries[i]<<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Dimension="<< m_fDimension << endreq;
  //msgStream <<MSG::INFO<<"Integral="<< m_fIntegral << endreq;
  msgStream <<MSG::INFO<<"Maximum="<< m_fMaximum << endreq;
  msgStream <<MSG::INFO<<"Minimum="<< m_fMinimum << endreq;
  msgStream <<MSG::INFO<<"Tsumw="<< m_fTsumw << endreq;
  msgStream <<MSG::INFO<<"Tsumw2="<< m_fTsumw2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwx="<< m_fTsumwx << endreq;
  msgStream <<MSG::INFO<<"Tsumwx2="<< m_fTsumwx2 << endreq;
  msgStream <<MSG::INFO<<"Tsumwy="<< m_fTsumwy << endreq;
  msgStream <<MSG::INFO<<"Tsumwy2="<< m_fTsumwy2 << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  msgStream <<MSG::INFO<<"Sumw2:"<<endreq;
  for (int i=0;i<m_fSumSize;++i) {
    msgStream << m_fSumw2[i] <<" ";
  }
  msgStream << endreq;
  msgStream <<MSG::INFO<<"*************************************"<<endreq;
  doOutputMsgStream(msgStream);
}
