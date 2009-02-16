// $Id: STCreateHistDBPages.h,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $

#ifndef STZSMONITORING_STCREATEHISTDBPAGES_H
#define STZSMONITORING_STCREATEHISTDBPAGES_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class STCreateHistDBPages STCreateHistDBPages.h STZSMonitoring/STCreateHistDBPages.h
 *
 *  Algorithm to generate pages in the OnlineHistDB based on python configuration files.
 * 
 *  @author Nicola Chiapolini 
 *  @date   2008-11-20
 */
class STCreateHistDBPages : public GaudiHistoAlg {

public:
  /// Constructor
  STCreateHistDBPages(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  /**
   * Method to create a page based on the definition given in the configuration file.
   * The configuration is taken form configuration vectors that are filled according to 
   * the python files.
   * (See the documentation on the configuration files for more details.)
   * 
   * @return StatusCode
   * @param *HistDB pointer to the database object
   * @param pageNr index of the page to created in the configuration vectors.
   */
  StatusCode createPage(OnlineHistDB *HistDB, int pageNr);
  

  StatusCode m_statusCode;
  
  // @option PasswordFile
  // name (including path, if necessary) of the file containing the password 
  // for the OnlineHistDB user HIST_WRITER
  // this file must be excluded from CVS
  // default value is "password"
  std::string m_passwordFile;
  
  // variable for temporary storage of histograms
  OnlineHistogram* tmpOnlineHisto;

  // @option DumpPageInfo 
  // dump page info after generation of a page?
  bool m_dumpPageInfo;
  
  // @option TaskName 
  // Fixed part of the histogram names comming form the Task
  std::string m_taskName;
  
  // @option AlgoBase
  // Fixed part of the histogram names comming form the Algorithm
  std::string m_algoBase;
  
  // @option PageBase 
  // Prefix for all page names, use it to identify your folder
  std::string m_pageBase;

  // @option PageNames
  // vector with the names of all pages to generate
  std::vector<std::string> m_pageNames;
  
  // @option PageLayout
  // Vector of groups of 4 values; each group defines the positions of a histograms on the page
  // order as in OnlineHistPage->declareHistogram: Xmin, Ymin, Xmax, Ymax
  // coordinates are between 0 and 1 with (0,0) = lower lefthand corner, x horizontaly)
  // one group for each histogram on the page
  std::vector<std::vector<double> > m_pageLayout;
  
  // @option histoNames
  // vector of lists; each list contains the names of the histograms to be placed on one page
  std::vector<std::vector<std::string> > m_histoNames;
};

#endif    // STZSMONITORING_STCREATEHISTDBPAGES_H