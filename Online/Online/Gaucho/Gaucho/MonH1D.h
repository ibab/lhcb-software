#ifndef GAUCHO_MONH1D_H
#define GAUCHO_MONH1D_H 1

#include "Gaucho/MonObject.h"
#include "TH1D.h"
#include "AIDA/IHistogram1D.h"

class FriendOfTH1D: public TH1D{
 public:
  friend class MonH1D;
};

class MonH1D: public MonObject {
  friend class boost::serialization::access;

protected:
  TH1D* m_hist;
  AIDA::IHistogram1D* m_aidaHist;

public:

  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonH1D(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonH1D();

  virtual void saveBinary(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void loadBinary(boost::archive::binary_iarchive & ar, const unsigned int version);

  virtual void saveText(boost::archive::text_oarchive & ar, const unsigned int version);
  virtual void loadText(boost::archive::text_iarchive & ar, const unsigned int version);

  template <class output_archive>
  void save2(output_archive  & ar);
  template <class output_archive>
  void save3(output_archive  & ar);
  template <class input_archive>
  void load2(input_archive  & ar);

  void createObject();
  void createObject(std::string name);
  void loadObject();
  void splitObject();   

  void setHist(TH1D * tH1D);
  TH1D* hist();
  std::string histName(){return sName;};
  std::string histTitle(){return sTitle;};

  void setAidaHisto(AIDA::IHistogram1D* iHistogram1D);
  AIDA::IHistogram1D* aidaHisto(){return m_aidaHist;}

  virtual void combine(MonObject * m2);
  virtual void copyFrom(MonObject* m2);
  virtual void print();
  virtual void write();
  virtual void reset();

  /**********/
  // data   
  /**********/
  int nbinsx;

  double Xmin;
  double Xmax;
  int nEntries;

  std::wstring sTeste;
  std::string sName;
  std::string sTitle;

  bool bBinLabel;

  double *binCont;
  double *binErr;
  std::string *binLabel;
  
  int m_fDimension;
  //double m_fIntegral;
  double m_fMaximum;
  double m_fMinimum;
  double m_fTsumw;
  double m_fTsumw2;
  double m_fTsumwx;
  double m_fTsumwx2;
  int m_fSumSize;

  double *m_fSumw2;

  bool isLoaded;
  bool objectCreated;
  /**********/

  

};

#endif //GAUCHO_MONH1D_H

