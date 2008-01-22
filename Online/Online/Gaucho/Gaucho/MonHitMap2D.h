#ifndef GAUCHO_MONHITMAP2D_H
#define GAUCHO_MONHITMAP2D_H 1

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "Gaucho/MonStatEntity.h"
#include <pthread.h>

class MonHitMap2D: public MonStatEntity{

private:
  unsigned int m_chanX;
  unsigned int m_chanY;
  unsigned int m_chan;
  int m_fromX; // inclusive
  int m_toX;  //inclusive
  int m_fromY;// inclusive
  int m_toY;//inclusive
  double* m_data;

  BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
  MonHitMap2D(IMessageSvc* msgSvc, const std::string& source, int version=0, std::string name="NewMonHM2D", int fromX=0, int toX=1,int fromY=0, int toY=1);
  MonHitMap2D(IMessageSvc* msgSvc, const std::string& source, std::string name="NewMonHM2D", int fromX=0, int toX=1,int fromY=0, int toY=1);
  virtual ~MonHitMap2D();

  void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  void save2(boost::archive::binary_oarchive & ar);
  void load(boost::archive::binary_iarchive & ar, const unsigned int version);
  void load2(boost::archive::binary_iarchive  & ar);

  std::string name(){return m_name;}  
  void setName(std::string name){m_name = name;}
  unsigned int channels(){return m_chan;}
  double data(unsigned int i){if (i<m_chan) return m_data[i];else return 0;}

  virtual void combine(MonObject * m2);
  virtual void copyFrom(MonObject* m2);  
  void addHitMap(MonHitMap2D &H, double fw=1);
  virtual void print();
  virtual void write(){};

  virtual void fill(int x, int y, double f = 1);

  double binContent(int x, int y);
  void setBinContent(int x, int y, double f);
  void reset();
}; 
#endif // GAUCHO_MONHITMAP2D_H

