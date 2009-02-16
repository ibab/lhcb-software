// $Id: STSetHistDisplayOptions.h,v 1.1 2009-02-16 10:37:42 ggiacomo Exp $

#ifndef STZSMONITORING_STSETHISTDISPLAYOPTIONS_H
#define STZSMONITORING_STSETHISTDISPLAYOPTIONS_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class STSetHistDisplayOptions STSetHistDisplayOptions.h STZSMonitoring/STSetHistDisplayOptions.h
 *
 *  Algorithm to set the display options of the histograms.
 * 
 *  @author Nicola Chiapolini 
 *  @date   2008-11-20
 */
class STSetHistDisplayOptions : public GaudiHistoAlg {
public:
  /// Constructor
  STSetHistDisplayOptions(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  StatusCode m_statusCode;
  
  // @option PasswordFile
  // path to file containing the password for the OnlineHistDB user HIST_WRITER
  // this file should be excluded from CVS to prevent a password leak
  std::string m_passwordFile;
  
  // @option TaskName 
  // Fixed part of the histogram names comming form the Task
  std::string m_taskName;
  
  // @option AlgoBase
  // Fixed part of the histogram names comming form the Algorithm
  std::string m_algoBase;
  
  // @option HistosToEdit
  // map storing the names of the histograms that will be edited
  // If the Boolean is true, saveHistoSetDisplayOptions() will be called
  // i.e. the changes will be applied for the whole set
  std::map<std::string, int> m_histosToEdit;
   
  // Lists of Options.
  // These are the same for all Histogramms defined in m_histosToEdit
  std::vector<std::string> m_dispOptsList;          // @option OptionsList   List with all options to be reset (should contain all existing options)
  std::map<std::string, int> m_intOptions;          // @option intOptions    List with all integer options
  std::map<std::string, double> m_doubleOptions;    // @option floatOptions  List with all float options
                                                    // As there is no mapping from python to map<string, float>
                                                    // double is used and the value is casted before using.
  std::map<std::string, std::string> m_strOptions;  // @option stringOptions List with all string options
};

#endif // STZSMONITORING_STSETHISTDISPLAYOPTIONS_H
