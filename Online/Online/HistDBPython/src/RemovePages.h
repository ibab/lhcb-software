// $Id: RemovePages.h,v 1.1 2009-04-06 12:58:54 nchiapol Exp $

#ifndef HISTDBPYTHON_RemovePages_H
#define HISTDBPYTHON_RemovePages_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
//#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class RemovePages RemovePages.h 
 *
 *  Delete pages from DB recursively
 *  
 *  @author Nicola Chiapolini 
 *  @date   2009-03-24
 */
class RemovePages : public GaudiHistoAlg {

public:
  /// Constructor
  RemovePages(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  StatusCode RemovePages::parseTree(OnlineHistDB *HistDB, std::string startPoint, int level);
  
  StatusCode m_statusCode;
 
  
  // @option PasswordFile
  // name (including path, if necessary) of the file containing the password 
  // for the OnlineHistDB user HIST_WRITER
  // this file must be excluded from CVS
  // default value is "password"
  std::string m_passwordFile;
  
  std::vector<std::string > m_pageNames;
  std::vector<std::string > m_folderNames;
  std::vector<std::string > m_startFolders;

  bool                      m_dryRun;
  bool                      m_protectTopLevel;
  
};

#endif // HISTDBPYTHON_RemovePages_H

