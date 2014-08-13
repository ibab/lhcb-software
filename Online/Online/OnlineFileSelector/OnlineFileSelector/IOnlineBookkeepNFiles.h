#ifndef ONLINEFILESELECTOR_IONLINEBOOKKEEPNFILES_H 
#define ONLINEFILESELECTOR_IONLINEBOOKKEEPNFILES_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

static const InterfaceID IID_IOnlineBookkeepNFiles ( "IOnlineBookkeepNFiles", 1, 0 );

/** @class IOnlineBookkeep IOnlineBookkeepNFiles.h OnlineFileSelector/IOnlineBookkeepNFiles.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-03-05
 */
class IOnlineBookkeepNFiles : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOnlineBookkeepNFiles; }

  virtual std::string getRunFileNumber(const std::string /*whole path to file*/ , const char* ) = 0;
  virtual StatusCode markBookKept(const std::string,const int ) = 0;
  virtual StatusCode isBookKept(const std::string ) = 0;
  virtual StatusCode updateStatus(const std::string, int ) = 0;	
  virtual StatusCode connectToDb() = 0;
  virtual StatusCode printDB() = 0;
  virtual StatusCode isProcessed(const std::string ) = 0;
  virtual StatusCode isDefect(const std::string ) = 0;
  virtual StatusCode increaseCounter(const std::string ) = 0;  
  virtual StatusCode decreaseCounter(const std::string ) = 0;
  virtual int getCounter(const std::string ) = 0;
  virtual StatusCode insertRun(const std::string ) = 0;
  virtual StatusCode existsRun(const std::string ) = 0;
  virtual StatusCode SetInProcess(const std::string, const int ) = 0;
  virtual StatusCode inProcess(const std::string ) = 0;
  virtual StatusCode isReady(const std::string ) = 0;
  virtual StatusCode setReady(const std::string ) = 0;
  virtual StatusCode setFinishedRun(const std::string ) = 0; 
  int m_remainingFiles;

};
#endif // ONLINEFILESELECTOR_IONLINEBOOKKEEPNFILES_H
