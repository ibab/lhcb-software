// $Id: CreateHistDBPages.h,v 1.2 2009-03-30 14:04:16 nchiapol Exp $

#ifndef HISTDBPYTHON_CREATEHISTDBPAGES_H
#define HISTDBPYTHON_CREATEHISTDBPAGES_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
//#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class CreateHistDBPages CreateHistDBPages.h 
 *
 *  Algorithm to generate pages in the OnlineHistDB based on python configuration files.
 *  
 *  Algorithm properties:
 *  - \b PasswordFile: String containing the name (including path, if needed) of the file 
 *    containing the password for the OnlineHistDB user HIST_WRITER.
 *    this file must be excluded from CVS. The default value is "password".
 *  - \b DumpPageInfo: If true dump page info after generation of a page.
 *  - \b HistoBase: Fixed part of the histogram names. (e.g. Task and Algorithm)
 *  - \b HistoNames: list of lists; each list contains the names of the histograms 
 *    to be placed on one page
 *  - \b HistoLinks: Map linking histogram names in HistoNames with the page
 *    that should be displayed when the histogram is clicked on.  
 *  - \b PageBase: Prefix for all page names, use it to identify your folder
 *  - \b PageNames: List with the names of all pages to generate
 *  - \b PageLayout: List of groups of 4 values; each group defines the positions of 
 *    a histograms on the page order as in OnlineHistPage->declareHistogram: 
 *      Xmin, Ymin, Xmax, Ymax
 *    coordinates are between 0 and 1 with (0,0) = lower lefthand corner, x horizontaly
 *    if Xmin is negative, the histogram will be overlayed onto the previouse one
 *    The list needs to cantain one group for each histogram on the page
 * 
 *  For more details on how to use this algrithm have a look at the examples in the 
 *  doc/python directory. 
 * 
 *  @author Nicola Chiapolini 
 *  @date   2009-03-16
 */
class CreateHistDBPages : public GaudiHistoAlg {

public:
  /// Constructor
  CreateHistDBPages(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  /**
   * Method to create a page based on the definition given in the configuration file.
   * The configuration is taken form configuration vectors that can be filled via  
   * python files.
   * (See the documentation on the configuration files for more details.)
   * 
   * @return StatusCode
   * @param *HistDB pointer to the database object
   * @param pageNr index in the configuration vectors for the page to create. 
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
  OnlineHistogram* tmpActiveHisto;
  OnlineHistogram* tmpLastHisto;

  // @option DumpPageInfo 
  // dump page info after generation of a page?
  bool m_dumpPageInfo;
  
  // @option HistoBase 
  // Fixed part of the histogram names (e.g. Task and Algorithm)
  std::string m_histoBase;
  
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
  
  // @option HistoNames
  // vector of lists; each list contains the names of the histograms to be placed on one page
  std::vector<std::vector<std::string> > m_histoNames;
  
  // @option HistoLinks
  // map the first string is the histogram name, the second string the page that should 
  // be displayed when the histogram is clicked on.  
  std::map<std::string, std::string> m_histoLinks;
  std::map<std::string, std::string>::iterator m_histoLinksEndIterator;
};

#endif // HISTDBPYTHON_CREATEHISTDBPAGES_H

