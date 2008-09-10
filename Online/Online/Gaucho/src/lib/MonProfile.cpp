#include "Gaucho/MonProfile.h"
#include "AIDA/IAxis.h"
#include <GaudiUtils/Aida2ROOT.h>
#include <GaudiUtils/HistoTableFormat.h>

MonProfile::MonProfile(IMessageSvc* msgSvc, const std::string& source, int version):
MonObject(msgSvc, source, version)
{
  m_typeName=s_monProfile;
  m_dimPrefix="MonP1";
  isLoaded = false;
  objectCreated = false;
  m_profile = 0;
  binCont = 0;
  binErr = 0;
  binEntries = 0;
  m_fSumw2 = 0;  
}

MonProfile::~MonProfile(){
//   MsgStream msgStream = createMsgStream();
//   msgStream <<MSG::DEBUG<<"deleting binCont" << endreq;
  if (binCont!=0) delete []binCont;
//   msgStream <<MSG::DEBUG<<"deleting binErr" << endreq;
  if (binErr!=0) delete []binErr;
//   msgStream <<MSG::DEBUG<<"deleting binLabelX" << endreq;
//   msgStream <<MSG::DEBUG<<"deleting m_fSumw2" << endreq;
  if (m_fSumw2!=0) delete []m_fSumw2;
// BUGG...I dont know yet why I can't do it..  
/*  if (m_profile!=0) {
    msgStream <<MSG::DEBUG<<"deleting m_profile" << endreq;
    delete m_profile; m_profile = 0;
  }*/
}

void MonProfile::setAidaProfile(AIDA::IProfile1D* iProfile1D){
  m_aidaProfile = iProfile1D;
  TProfile* hRoot = (TProfile*)Gaudi::Utils::Aida2ROOT::aida2root(m_aidaProfile);
  sName = Gaudi::Utils::Histos::path ( m_aidaProfile ) ;
  hRoot->SetName(sName.c_str());
  setProfile(hRoot);
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
   MsgStream msg = createMsgStream();

  ar & nbinsx;
  ar & Xmin;
  ar & Xmax;
  ar & Ymin;
  ar & Ymax;
  ar & nEntries;
  ar & sName;
  ar & sTitle;
  ar & bBinLabelX;

  msg <<MSG::DEBUG<<"creating MsgStream" << endreq;
  
  if (binCont==0) binCont = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }

  if (binErr==0) binErr = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binErr[i];
  }

  if (binEntries==0) binEntries = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binEntries[i];
  }

  //msg <<MSG::DEBUG<<"before binLabelX" << endreq;
  if (bBinLabelX){
    //if (binLabelX==0) binLabelX = new std::string[(nbinsx+1)];
    //msg <<MSG::DEBUG<<"binLabelX clear" << endreq;
    binLabelX.clear();

    for (int i = 1; i < (nbinsx+1) ; ++i){
      //ar & binLabelX[i];
      std::string labelX;
      ar & labelX;
      //msg <<MSG::DEBUG<<"binLabelX[" << i << "]" << labelX << endreq;
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
  ar & m_fTsumwy;
  ar & m_fTsumwy2;

  ar & m_fSumSize;

  //msg <<MSG::DEBUG<<"m_fSumSize" << m_fSumSize << endreq;

  if (m_fSumw2==0) m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    ar & m_fSumw2[i];
  }

  //msg <<MSG::DEBUG<<"is Loaded" << endreq;

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
  ar & bBinLabelX;

  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binCont[i];
  }
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binErr[i];
  }
  for (int i = 0; i < (nbinsx+2) ; ++i){
    ar & binEntries[i];
  }
  if (bBinLabelX){
    for (int i = 0; i < (int)binLabelX.size() ; ++i){
//      std::cout << "save ===>label["<< i << "]=" << binLabelX[i] << std::endl;
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
  //print();
  loadObject();
  return m_profile;
}

void MonProfile::createObject(std::string name){
  if (!isLoaded) return;
  MsgStream msgStream = createMsgStream();
  msgStream <<MSG::DEBUG<<"Creating TProfile " << name << endreq;
  if (m_profile==0) m_profile = new TProfile(name.c_str(), sTitle.c_str(), nbinsx, Xmin, Xmax, Ymin, Ymax);
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
  MsgStream msg = createMsgStream();
  if (!objectCreated) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Can't load object non created" << endreq;
    
    return;  
  }
  m_profile->Reset();
  FriendOfTProfile * fot = (FriendOfTProfile *)m_profile; 

  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_profile->SetBinContent(i, binCont[i]);
  }

  //SetEntries must be after SetBinContents
  m_profile->SetEntries(nEntries);

  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_profile->SetBinError(i, binErr[i]);
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    m_profile->SetBinEntries(i, binEntries[i]);
  }

  if (bBinLabelX){
    std::vector<std::string>::iterator it;
    int i = 1;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      m_profile->GetXaxis()->SetBinLabel(i, (*it).c_str());
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
  fot->fTsumwy = m_fTsumwy;
  fot->fTsumwy2 = m_fTsumwy2;

  fot->fSumw2.Set(m_fSumSize);

  for (int i=0 ; i < m_fSumSize; ++i) {
    fot->fSumw2[i] = m_fSumw2[i];
  }
}

void MonProfile::splitObject(){
  MsgStream msg = createMsgStream();
  FriendOfTProfile * fot = (FriendOfTProfile *)m_profile; 
  
  int nbinsxOld = 0;
  if (nbinsx != 0) nbinsxOld = nbinsx;
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

  MsgStream msgStream = createMsgStream();
  if (binCont != 0) {
    delete []binCont;
  }
  binCont = new double[(nbinsx+2)];
  if (binErr != 0) delete []binErr;
  binErr = new double[(nbinsx+2)];
  if (binEntries != 0) delete []binEntries;
  binEntries = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = ((double) (m_profile->GetBinContent(i))); 
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binErr[i] = ((double) (m_profile->GetBinError(i))); 
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binEntries[i] = ((double) (m_profile->GetBinEntries(i))); 
  }

  bBinLabelX = false;
  for (int i = 1; i < (nbinsx+1) ; ++i){
    std::string binLab = m_profile->GetXaxis()->GetBinLabel(i);
    if (binLab.length() > 0 ){
      bBinLabelX = true;
      break;
    }
  }
  
  if (bBinLabelX){
      binLabelX.clear();
    for (int i = 1; i < (nbinsx+1) ; ++i){
      binLabelX.push_back(m_profile->GetXaxis()->GetBinLabel(i));
    }
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
  if (m_fSumw2 != 0) delete []m_fSumw2;
  m_fSumw2 = new double[m_fSumSize];

  for (int i=0;i<fot->fSumw2.GetSize();++i) {
    m_fSumw2[i] = fot->fSumw2[i]; 
  }

  isLoaded = true;
}

void MonProfile::combine(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
//  if (H->endOfRun() != this->endOfRun()){
 //   msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
 //   return;
 // }
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
    msg << MSG::ERROR<<"Trying to combine uncompatible MonObjects" << endreq;
    msg << MSG::ERROR<<"  nbinsx ="<<nbinsx << "; HH->nbinsx="<<HH->nbinsx << endreq;
    msg << MSG::ERROR<<"  Xmin ="<<Xmin << "; HH->Xmin="<<HH->Xmin << endreq;
    msg << MSG::ERROR<<"  Xmax ="<<Xmax << "; HH->Xmax="<<HH->Xmax << endreq;
    msg << MSG::ERROR<<"  Ymin ="<<Xmin << "; HH->Ymin="<<HH->Xmin << endreq;
    msg << MSG::ERROR<<"  Ymax ="<<Xmax << "; HH->Ymax="<<HH->Xmax << endreq;
    msg << MSG::ERROR<<"  sTitle ="<<sTitle << "; HH->sTitle="<<HH->sTitle << endreq;
    return;
  }

  for (int i = 0; i < (nbinsx+2); ++i){
    binCont[i] += HH->binCont[i];
    binEntries[i] += HH->binEntries[i];
    binErr[i] = sqrt(pow(binErr[i],2)+pow(HH->binErr[i],2));
  }
  
  m_fTsumw += HH->m_fTsumw;
  m_fTsumw2 += HH->m_fTsumw2;
  m_fTsumwx += HH->m_fTsumwx;
  m_fTsumwx2 += HH->m_fTsumwx2;
  m_fTsumwy += HH->m_fTsumwy;
  m_fTsumwy2 += HH->m_fTsumwy2;

  if (m_fSumSize == HH->m_fSumSize){
    for (int i=0 ; i < m_fSumSize; ++i) {
      m_fSumw2[i] += HH->m_fSumw2[i];
    }
  }
  nEntries += HH->nEntries;
}

void MonProfile::printLabels(){
  MsgStream msg = createMsgStream();
  if (bBinLabelX){
    std::vector<std::string>::iterator it;
    std::cout << "size:" << binLabelX.size() << std::endl;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      msg <<MSG::INFO<< *it << endreq;
    }
  }  
}

void MonProfile::copyFrom(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  std::vector<std::string>::iterator it;
  
  MonProfile *HH = (MonProfile*)H;
  m_endOfRun = HH->endOfRun();
  nbinsx = HH->nbinsx;
  Xmin = HH->Xmin;
  Xmax = HH->Xmax;
  Ymin = HH->Ymin;
  Ymax = HH->Ymax;
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

  if (binEntries != 0) delete []binEntries;
  binEntries = new double[(nbinsx+2)];

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binEntries[i] = HH->binEntries[i];
  }
  bBinLabelX = HH->bBinLabelX;
  
  if (bBinLabelX){
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      msg <<MSG::INFO<< *it << endreq;
    }
    binLabelX.clear();
    for ( it=HH->binLabelX.begin() ; it < HH->binLabelX.end(); it++ ) {
      std::string labelX = *it;
      binLabelX.push_back(labelX);
    }
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
  
  if (m_fSumw2 != 0) delete []m_fSumw2;
  m_fSumw2 = new double[m_fSumSize];
  for (int i=0 ; i < m_fSumSize; ++i) {
    m_fSumw2[i] = HH->m_fSumw2[i];
  }

  isLoaded = true;
}

void MonProfile::synchronizeLabelNames(MonObject * H){
  MsgStream msg = createMsgStream();
  if (H->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<H->typeName() << " failed." << endreq;
    return;
  }
  if (binLabelX.size() <= 0 ) return;
  std::vector<std::string>::iterator it;
  
  MonProfile *HH = (MonProfile*)H;
  
  if (bBinLabelX){
    binLabelX.clear();
    for ( it=HH->binLabelX.begin() ; it < HH->binLabelX.end(); it++ ) {
      std::string labelX = *it;
      binLabelX.push_back(labelX);
    }
  }
}



void MonProfile::reset(){
  if (!isLoaded){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to reset unloaded MonObject" << endreq;
    
    return;
  }

  for (int i = 0; i < (nbinsx+2) ; ++i){
    binCont[i] = 0;
    binErr[i] = 0; 
    binEntries[i] = 0; 
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

  m_fSumSize = 0;
}

void MonProfile::print(){
  MonObject::print();
  if (!isLoaded) return;

  MsgStream msg = createMsgStream();
  msg <<MSG::INFO<<"*************************************"<<endreq;
  msg <<MSG::INFO<<"**************Profile 1D*************"<<endreq;
  msg <<MSG::INFO<<"Title="<<sTitle<<endreq;
  msg <<MSG::INFO<<"nEntries="<<nEntries<<endreq;
  msg <<MSG::INFO<<"nbinsx="<<nbinsx<<endreq;
  msg <<MSG::INFO<<"Xmin="<<Xmin<<endreq;
  msg <<MSG::INFO<<"Xmax="<<Xmax<<endreq;
  msg <<MSG::INFO<<"Ymin="<<Ymin<<endreq;
  msg <<MSG::INFO<<"Ymax="<<Ymax<<endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  msg <<MSG::INFO<<"BinContents:"<<endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msg << (int) binCont[i]<<" ";
  }
  msg << endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  msg <<MSG::INFO<<"BinErrors:"<<endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msg << binErr[i]<<" ";
  }
  msg << endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  msg <<MSG::INFO<<"BinEntries:"<<endreq;
  for (int i = 0; i < (nbinsx+2) ; ++i){
    msg << binEntries[i]<<" ";
  }
  msg << endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  if (bBinLabelX){
    msg <<MSG::INFO<<"BinLabelsX:"<<endreq;
    std::vector<std::string>::iterator it;
    std::cout << "size:" << binLabelX.size() << std::endl;
    for ( it=binLabelX.begin() ; it < binLabelX.end(); it++ ) {
      msg <<MSG::INFO<< *it << " " ;
    }
    msg << endreq;
    msg <<MSG::INFO<<"*************************************"<<endreq;
  }  
  msg <<MSG::INFO<<"Dimension="<< m_fDimension << endreq;
  //msg <<MSG::INFO<<"Integral="<< m_fIntegral << endreq;
  msg <<MSG::INFO<<"Maximum="<< m_fMaximum << endreq;
  msg <<MSG::INFO<<"Minimum="<< m_fMinimum << endreq;
  msg <<MSG::INFO<<"Tsumw="<< m_fTsumw << endreq;
  msg <<MSG::INFO<<"Tsumw2="<< m_fTsumw2 << endreq;
  msg <<MSG::INFO<<"Tsumwx="<< m_fTsumwx << endreq;
  msg <<MSG::INFO<<"Tsumwx2="<< m_fTsumwx2 << endreq;
  msg <<MSG::INFO<<"Tsumwy="<< m_fTsumwy << endreq;
  msg <<MSG::INFO<<"Tsumwy2="<< m_fTsumwy2 << endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  msg <<MSG::INFO<<"Sumw2:"<<endreq;
  for (int i=0;i<m_fSumSize;++i) {
    msg << m_fSumw2[i] <<" ";
  }
  msg << endreq;
  msg <<MSG::INFO<<"*************************************"<<endreq;
  
}
