#ifndef GAUCHO_MONBUFFER_H
#define GAUCHO_MONBUFFER_H 1

// Include files
// #include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <TStopwatch.h>
#include <TBufferFile.h>
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

/** @class ClassName MonBuffer.h 
 *
 *  MonBuffer base class
 *
 *  @author Juan Otalora
 *  @date 16/02/2009
 */

  static const std::string s_monBuffer("MonBuffer");
  static const std::string s_monBufferObject("MonBufferObject");
  static const std::string s_pfixMonBuffer("MonBuf");
  static const std::string s_pfixMonBufferObject("MonBOb");

class MonBuffer{

protected:
  std::string m_typeName;
  std::string m_comments;
  std::string m_dimPrefix;
  unsigned int m_version;
  bool m_endOfRun;
  bool m_serviceActive;

public:
  MonBuffer(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonBuffer();

  virtual void save(TBufferFile* buffer, const unsigned int version);
  virtual void load(TBufferFile* buffer, const unsigned int version);

  std::string comments(){return m_comments;}
  void setComments(std::string comm){m_comments = comm;}

  bool endOfRun(){return m_endOfRun;}
  void setEndOfRun(bool endofrun){m_endOfRun = endofrun;}
  
  void setServiceActive (bool serviceActive) {m_serviceActive = serviceActive;}
  bool serviceActive () {return m_serviceActive;}

  virtual std::string dimPrefix() const {return m_dimPrefix;}
  void setDimPrefix(std::string dimPrefix){m_dimPrefix = dimPrefix;}

  virtual std::string typeName() const {return m_typeName;} 
  virtual int version(){return m_version;}

  virtual void combine(MonBuffer* monBuffer);
  virtual void copyFrom(MonBuffer* monBuffer);
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

};
#endif // GAUCHO_MONBUFFER_H

