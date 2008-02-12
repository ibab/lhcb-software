#ifndef InsertDB_InsertDB_H
#define InsertDB_InsertDB_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include<time.h>
#include "OnlineHistDB/OnlineHistDB.h"

/** @class InsertDB


*/

// Forward declarations
class IIncidentSvc;
class InsertDB : public Algorithm, virtual public IIncidentListener {
public:
  /// Constructor of this form must be provided
  InsertDB(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  virtual void handle(const Incident& inc);
  OnlineHistDB* HistDB;
private:
  std::string m_nodename;
  std::string m_dimclientdns;
  std::string m_topleveltaskname;
  std::string m_taskname;
  std::string command;
  

};

#endif    // InsertDB_InsertDB_H
