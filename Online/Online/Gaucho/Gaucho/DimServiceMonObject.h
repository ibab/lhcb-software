#ifndef DIMSERVICEMONOBJECT_H
#define DIMSERVICEMONOBJECT_H 1

#include <string>
#include <iostream>
#include "dis.hxx"
#include <TStopwatch.h>
#include <sstream>

//  Author: jotalo, 22/10/2007
class MonObject;
class DimServiceMonObject : public DimService {

private:
  int           m_size;
  void*         m_data;
  MonObject*    m_monObject;
  std::string   m_svcName;
  std::stringstream m_ss;
  bool          m_enableHandler;

public: 
  DimServiceMonObject(std::string svcName, MonObject *monObject, int size);
  virtual ~DimServiceMonObject();
  
  void enableHandler(){m_enableHandler = true;}
  void disableHandler(){m_enableHandler = false;}
  
  void serviceHandler();
  
  void updateService(bool endOfRun);
  void setDataFromMonObject();
  MonObject* monObject(){return m_monObject;};
};

#endif // DIMSERVICEMONOBJECT_H
