// $Id: OMAcommon.h,v 1.4 2008-09-26 14:36:31 ggiacomo Exp $
#ifndef OMALIB_OMACOMMON_H 

#define OMALIB_OMACOMMON_H 1

// Include files

#include <TFile.h>
#include <TH1.h>
#include <sstream>
#include "OMAlib/OMAMsgInterface.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"

/** @class OMAcommon OMAcommon.h OMAlib/OMAcommon.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-08 
 */

namespace OMAconstants {
  static const int AlgListID = 6;
  static const std::string version = "v1r4";
  static const double epsilon = 1.e-10;
}


class OMAcommon : public OMAMsgInterface {
public: 
  /// Standard constructor
  OMAcommon(OnlineHistDB* HistDB = NULL) : m_histDB(HistDB), m_debug(2) {
    setDefRefRoot();
  }
  virtual ~OMAcommon( ) {} ///< Destructor

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
protected:
  void setDefRefRoot();
  OnlineHistDB* m_histDB;
  int m_debug;
private:
  std::string m_RefRoot;
};


#endif // OMALIB_OMACOMMON_H
