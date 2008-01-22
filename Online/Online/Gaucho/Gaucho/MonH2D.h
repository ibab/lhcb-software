#ifndef GAUCHO_MONH2D_H
#define GAUCHO_MONH2D_H 1

#include "Gaucho/MonObject.h"
#include "TH2D.h"
#include "AIDA/IHistogram2D.h"
#include <boost/serialization/list.hpp>


class FriendOfTH2D: public TH2D{
 public:
  friend class MonH2D;
};

class MonH2D: public MonObject {

protected:
  TH2D* m_hist;
  AIDA::IHistogram2D* m_aidaHist;

public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonH2D(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonH2D();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);
  void save2(boost::archive::binary_oarchive  & ar);
  void save3(boost::archive::binary_oarchive  & ar);
  void load2(boost::archive::binary_iarchive  & ar);

  void createObject();
  void createObject(std::string name);
  void loadObject();
  void splitObject();   

  void setHist(TH2D * tH2D);
  TH2D* hist();
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

  double Xmin;
  double Xmax;
  double Ymin;
  double Ymax;
  int nEntries;

  std::string sName;
  std::string sTitle;
  std::string sXTitle;
  std::string sYTitle;

  double *binCont;
  double *binErr;

  int m_fDimension;
  //float m_fIntegral;
  double m_fMaximum;
  double m_fMinimum;
  double m_fTsumw;
  double m_fTsumw2;
  double m_fTsumwx;
  double m_fTsumwx2;
  double m_fTsumwxy;
  double m_fTsumwy;
  double m_fTsumwy2;

  int m_fSumSize;

  double *m_fSumw2;

  bool isLoaded;
  bool objectCreated;
  /**********/

};

#endif //GAUCHO_MONH2D_H

