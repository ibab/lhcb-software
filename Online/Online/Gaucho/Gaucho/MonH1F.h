#ifndef GAUCHO_MONH1F_H
#define GAUCHO_MONH1F_H 1

#include "Gaucho/MonObject.h"
#include "TH1F.h"
#include "AIDA/IHistogram1D.h"
#include <boost/serialization/list.hpp>


class FriendOfTH1F: public TH1F{
 public:
  friend class MonH1F;
};

class MonH1F: public MonObject {

protected:
  TH1F* m_hist;
  AIDA::IHistogram1D* m_aidaHist;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonH1F(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonH1F();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);
  void save2(boost::archive::binary_oarchive  & ar);
  void save3(boost::archive::binary_oarchive  & ar);
  void load2(boost::archive::binary_iarchive  & ar);

  virtual void createObject();
  void createObject(std::string name);
  void loadObject();
  void splitObject();   

  void setHist(TH1F * tH1F);
  TH1F* hist();
  std::string histName(){return sName;};
  std::string histTitle(){return sTitle;};

  void setAidaHisto(AIDA::IHistogram1D* iHistogram1D);
  AIDA::IHistogram1D* aidaHisto(){return m_aidaHist;}

  virtual void combine(MonObject * m2);
  virtual void copyFrom(MonObject* m2);
  virtual void print();
  virtual void printObject();
  virtual void write();
  virtual void reset();

  /**********/
  // data   
  /**********/
  int nbinsx;

  float Xmin;
  float Xmax;
  int nEntries;

  std::string sName;
  std::string sTitle;

  bool bBinLabel;

  float *binCont;
  float *binErr;
  std::string *binLabel;

  int m_fDimension;
  //float m_fIntegral;
  float m_fMaximum;
  float m_fMinimum;
  float m_fTsumw;
  float m_fTsumw2;
  float m_fTsumwx;
  float m_fTsumwx2;
  int m_fSumSize;

  float *m_fSumw2;

  bool isLoaded;
  bool objectCreated;
  /**********/

};

#endif //GAUCHO_MONH1F_H

