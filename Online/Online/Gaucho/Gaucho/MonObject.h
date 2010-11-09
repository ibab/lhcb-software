//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/Gaucho/MonObject.h,v 1.22 2010-11-09 16:34:32 evh Exp $
#ifndef GAUCHO_MONOBJECT_H
#define GAUCHO_MONOBJECT_H 1

// Include files
// #include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <TStopwatch.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
// boost v 1.35.00 ??
// #include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/string.hpp>
//#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
// #include <boost/serialization/list.hpp>
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <vector>

/** @class ClassName MonObject.h OnlineKernel/MonObject.h
 *
 *  MonObject base class
 *
 *  @author Claus Buszello
 *  @author Eric van Herwijnen
 *  @author Juan Otalora
 *  @date 08/01/2008
 */

  static const std::string s_monObject("MonObject");
  static const std::string s_monH1F("MonH1F");
  static const std::string s_monH1D("MonH1D");
  static const std::string s_monH2F("MonH2F");
  static const std::string s_monH2D("MonH2D");
  static const std::string s_monProfile("MonProfile");
  static const std::string s_monInt("MonInt");
  static const std::string s_monDouble("MonDouble");
  static const std::string s_monFloat("MonFloat");
  static const std::string s_monString("MonString");
  static const std::string s_monLong("MonLong");
  static const std::string s_monBool("MonBool");
  static const std::string s_monPairDD("MonPairDD");
  static const std::string s_monPairII("MonPairII");
  static const std::string s_monPairDI("MonPairDI");
  static const std::string s_monHitMap2D("MonHitMap2D");
  static const std::string s_monStatEntity("MonStatEntity");
  static const std::string s_monVectorI("MonVectorI");
  static const std::string s_monVectorD("MonVectorD");
  static const std::string s_monRate("MonRate");

  static const std::string s_pfixMonObject("MonObj");
  static const std::string s_pfixMonH1F("MonH1F");
  static const std::string s_pfixMonH1D("MonH1D");
  static const std::string s_pfixMonH2F("MonH2F");
  static const std::string s_pfixMonH2D("MonH2D");
  static const std::string s_pfixMonProfile("MonP1");
  static const std::string s_pfixMonInt("MonI");
  static const std::string s_pfixMonDouble("MonD");
  static const std::string s_pfixMonFloat("MonF");
  static const std::string s_pfixMonString("MonS");
  static const std::string s_pfixMonLong("MonL");
  static const std::string s_pfixMonBool("MonB");
  static const std::string s_pfixMonPairDD("MonPDD");
  static const std::string s_pfixMonPairII("MonPII");
  static const std::string s_pfixMonPairDI("MonPDI");
  static const std::string s_pfixMonHitMap2D("MonHM2D");
  static const std::string s_pfixMonStatEntity("MonSE");
  static const std::string s_pfixMonVectorI("MonVI");
  static const std::string s_pfixMonVectorD("MonVD");
  static const std::string s_pfixMonRate("MonR");

class MonObject{

protected:
  std::string m_typeName;
  std::string m_comments;
  std::string m_dimPrefix;
  unsigned int m_version;
  bool m_endOfRun;
  bool m_serviceActive;
  
public:
  MonObject(IMessageSvc* msgSvc, const std::string& source, int version=1);
  virtual ~MonObject();

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version){
    if (version != m_version) m_version = version;
    ar & m_typeName; 
    ar & m_version;
  }  
  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive & ar);

  std::string comments(){return m_comments;}
  void setComments(std::string comm){m_comments = comm;}

  bool endOfRun(){return m_endOfRun;}
  void setEndOfRun(bool endofrun){m_endOfRun = endofrun;}
  
  void setServiceActive (bool serviceActive) {m_serviceActive = serviceActive;}
  bool serviceActive () {return m_serviceActive;}

  virtual std::string dimPrefix() const {return m_dimPrefix;}
  void setDimPrefix(std::string dimPrefix){m_dimPrefix = dimPrefix;}

  virtual std::string typeName() const {return m_typeName;} 
  virtual int mon_version(){return m_version;}

  virtual void combine(MonObject* monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void subtract();
  virtual void print();
  virtual void printObject(){};
  virtual void createObject(){};
  virtual void write(){};
  virtual void loadObject(){};

  // bool islocked();
   TStopwatch watchOut;
   TStopwatch watch;

protected:
  IMessageSvc*  m_msgSvc;
  std::string   m_source;
  std::string   m_name;
  MsgStream createMsgStream();

private:

  friend class boost::serialization::access;
};
#endif // GAUCHO_MONOBJECT_H

