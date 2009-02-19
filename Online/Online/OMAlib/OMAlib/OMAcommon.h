// $Id: OMAcommon.h,v 1.8 2009-02-19 10:49:50 ggiacomo Exp $
#ifndef OMALIB_OMACOMMON_H 

#define OMALIB_OMACOMMON_H 1

// Include files

#include <TFile.h>
#include <TH1.h>
#include <map>
#include "OMAlib/OMAMsgInterface.h"
#include "OMAlib/OMAFitFunction.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"


/** @class OMAcommon OMAcommon.h OMAlib/OMAcommon.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-08 
 */


class OMAcommon : public OMAMsgInterface {
public: 
  /// Standard constructor
  OMAcommon(OnlineHistDB* HistDB = NULL,  
            std::string Name="") : 
    OMAMsgInterface(HistDB, Name), m_debug(2) {
    setDefRefRoot();
  }
  virtual ~OMAcommon();

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
  /// get reference histogram
  TH1* getReference(OnlineHistogram* h,
                    int startrun = 1,
                    std::string DataType = "default");
  inline OMAFitFunction* getFitFunction(std::string &Name){    
    return m_fitfunctions[Name];}
protected:
  void setDefRefRoot();
  void doFitFuncList();
  int m_debug;
  std::map<std::string, OMAFitFunction*> m_fitfunctions;
private:
  std::string m_RefRoot;
};


#endif // OMALIB_OMACOMMON_H
