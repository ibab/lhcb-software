#ifndef ONLINEFILESELECTOR_IONLINEBOOKKEEP_H 
#define ONLINEFILESELECTOR_IONLINEBOOKKEEP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

static const InterfaceID IID_IOnlineBookkeep ( "IOnlineBookkeep", 1, 0 );

/** @class IOnlineBookkeep IOnlineBookkeep.h OnlineFileSelector/IOnlineBookkeep.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-03-05
 */
class IOnlineBookkeep : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IOnlineBookkeep; }

  virtual std::string getRunFileNumber(const std::string /*whole path to file*/ , const char* ) = 0;
  virtual StatusCode markBookKept(const std::string,const int ) = 0;
  virtual StatusCode isBookKept(const std::string ) = 0;
  virtual StatusCode updateStatus(const std::string, int ) = 0;	
  virtual StatusCode connectToDb() = 0;
  virtual StatusCode printDB() = 0;
  virtual StatusCode isProcessed(const std::string ) = 0;
  


};
#endif // ONLINEFILESELECTOR_IONLINEBOOKKEEP_H
