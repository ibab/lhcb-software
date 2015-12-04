// $Id: OMAEnv.h,v 1.21 2010-10-21 10:17:32 ggiacomo Exp $
#ifndef OMALIB_OMAENV_H 

#define OMALIB_OMAENV_H 1

// Include files

#include <map>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"

class TFile;
class TH1;

/** @class OMAEnv OMAEnv.h OMAlib/OMAEnv.h
 *  
 *  environment variables and methods for the Online Monitoring Analysis library
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-08 
 */

namespace OMAconstants {
  static const int AlgListID = 22;
  static const std::string version = "v5r1";
  
  static const int AlarmExpTime = 3456000; // s =  40 days
  static const double epsilon = 1.e-10;
}

class OMAEnv  {
public: 
  /// Standard constructor
  OMAEnv(OnlineHistDB* HistDB = NULL,  
         std::string Name="");
  virtual ~OMAEnv();
  /// get path of reference histogram files
  inline std::string& refRoot() {return m_RefRoot;}
  /// set path of reference histogram files
  inline void setRefRoot(std::string NewPath) { m_RefRoot = NewPath ;}
  /// get link to HistDB session
  inline OnlineHistDB* dbSession() {
    return m_histDB; }
  /// get debug level
  inline int omaDebugLevel() { return m_debug; }
  /// set debug level (0 for nothing, the higher the more verbose)
  inline void setDebug(int DebugLevel) { m_debug = DebugLevel;}

protected:
  std::string VirtualTaskName(std::string& taskname) {
    return (taskname+"_ANALYSIS");
  }

  

  int m_debug;
  std::string m_anaTaskname;
  OnlineHistDB* m_histDB;
  bool m_localDBsession;
  void setDefRefRoot();
private:
  std::string m_RefRoot;
};


#endif // OMALIB_OMAENV_H
