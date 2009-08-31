#ifndef GAUCHO_MONPROFILE_H
#define GAUCHO_MONPROFILE_H 1

#include "Gaucho/MonObject.h"
#include "TProfile.h"
#include "AIDA/IProfile1D.h"
#include <boost/serialization/list.hpp>

class FriendOfTProfile: public TProfile{
 public:
  friend class MonProfile;
};

class MonProfile: public MonObject {

protected:
  TProfile* m_profile;
  AIDA::IProfile1D* m_aidaProfile;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonProfile(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonProfile();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);
  void save2(boost::archive::binary_oarchive  & ar);
  void save3(boost::archive::binary_oarchive  & ar);

  void load2(boost::archive::binary_iarchive  & ar);

  void createObject();
  void createObject(std::string name);
  void loadObject();
  void splitObject();   

  void setProfile(TProfile * tProfile);
  TProfile* profile();
  void deleteProfile();
  
  std::string profileName(){return sName;};
  std::string profileTitle(){return sTitle;};

  void setAidaProfile(AIDA::IProfile1D* iProfile1D);
  AIDA::IProfile1D* aidaProfile(){return m_aidaProfile;};

  virtual void combine(MonObject * m2);
  virtual void copyFrom(MonObject* m2);
//  virtual void add(MonProfile &MonProfile);
  virtual void print();
  virtual void write();
  virtual void reset();
  void synchronizeLabelNames(MonObject * H);

  double binEntry(int i) {return binEntries[i];}
  double binContent(int i){if((this->typeName()=="MonRate") && (((i==5)||(i==6))||(i==7))) return binSum[i]; else { if(0!=binEntries[i]) return binSum[i]/binEntries[i]; else return 0;} };
  double binError(int i);


  



  std::string binLabX(int i) {return binLabelX[i];}
  int numbinx() {return nbinsx;}


  void printLabels();
  /**********/
  // data   
  /**********/
  int nbinsx;

  double Xmin;
  double Xmax;
  double Ymin;
  double Ymax;
  int nEntries;

  std::string sName;
  std::string sTitle;

  bool bBinLabelX;

  double *binSum;
  //double *binErr;
  double *binEntries;
  
  int m_fDimension;
  //double m_fIntegral;
  double m_fMaximum;
  double m_fMinimum;
  double m_fTsumw;
  double m_fTsumw2;
  double m_fTsumwx;
  double m_fTsumwx2;
  double m_fTsumwy;
  double m_fTsumwy2;
  int m_fSumSize;

  double *m_fSumw2;

  bool isLoaded;
  bool objectCreated;
  /**********/
  
private:
  std::vector<std::string> binLabelX;

  
};

#endif //GAUCHO_MONPROFILE_H

