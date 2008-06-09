#ifndef GAUCHO_MONH2F_H
#define GAUCHO_MONH2F_H 1

#include "Gaucho/MonObject.h"
#include "TH2F.h"
#include "AIDA/IHistogram2D.h"
#include <boost/serialization/list.hpp>


class FriendOfTH2F: public TH2F{
 public:
  friend class MonH2F;
};

class MonH2F: public MonObject {

protected:
  TH2F* m_hist;
  AIDA::IHistogram2D* m_aidaHist;

public:
  //BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonH2F(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonH2F();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);
  void save2(boost::archive::binary_oarchive  & ar);
  void save3(boost::archive::binary_oarchive  & ar);
  void load2(boost::archive::binary_iarchive  & ar);
  
  void createObject();
  void createObject(std::string name);
  void loadObject();
  void splitObject();   

  void setHist(TH2F * tH2F);
  TH2F* hist();
  std::string histName(){return sName;};
  std::string histTitle(){return sTitle;};

  void setAidaHisto(AIDA::IHistogram2D* iHistogram2D);
  AIDA::IHistogram2D* aidaHisto(){return m_aidaHist;}

  virtual void combine(MonObject * m2);
  virtual void copyFrom(MonObject* m2);
  virtual void print();
  virtual void write();
  virtual void reset();

  /**********/
  // data   
  /**********/
  int nbinsx;
  int nbinsy;

  float Xmin;
  float Xmax;
  float Ymin;
  float Ymax;
  int nEntries;

  std::string sName;
  std::string sTitle;

  bool bBinLabelX;
  bool bBinLabelY;

  float *binCont;
  float *binErr;
  std::string *binLabelX;
  std::string *binLabelY;

  int m_fDimension;
  //float m_fIntegral;
  float m_fMaximum;
  float m_fMinimum;
  float m_fTsumw;
  float m_fTsumw2;
  float m_fTsumwx;
  float m_fTsumwx2;
  float m_fTsumwxy;
  float m_fTsumwy;
  float m_fTsumwy2;

  int m_fSumSize;

  float *m_fSumw2;

  bool isLoaded;
  bool objectCreated;
  /**********/

};

#endif //GAUCHO_MONH2F_H

