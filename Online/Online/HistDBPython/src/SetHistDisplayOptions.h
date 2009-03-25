// $Id: SetHistDisplayOptions.h,v 1.1.1.1 2009-03-25 09:50:53 nchiapol Exp $

#ifndef HISTDBPYTHON_SETHISTDISPLAYOPTIONS_H
#define HISTDBPYTHON_SETHISTDISPLAYOPTIONS_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
//#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class SetHistDisplayOptions SetHistDisplayOptions.h 
 *
 *  Algorithm to set the display options of histograms.
 *
 *  Algorithm properties:
 *  - \b PasswordFile: String containing the name (including path, if needed) of the file 
 *    containing the password for the OnlineHistDB user HIST_WRITER.
 *    this file must be excluded from CVS. The default value is "password".
 *  - \b HistoBase: Fixed part of the histogram names. (e.g. Task and Algorithm)
 *  - \b HistoNames: List or pairs. First value is the name of the histogram to edit,
 *    second value is a boolean. If true, saveHistoSetDisplayOptions() will be called
 *    i.e. the changes will be applied for the whole set
 *  - \b UnsetOptions: boolean, if true unset all options of a histogram before setting new ones.
 *  - \b intOptions: List with all integer options
 *  - \b floatOptions: List with all float options 
 *  - \b stringOptions: List with all string options
 *  
 *  For more details on how to use this algrithm have a look at the examples in the 
 *  doc/python directory.
 *   
 *  @author Nicola Chiapolini 
 *  @date   2008-11-20
 */
class SetHistDisplayOptions : public GaudiHistoAlg {
public:
  /// Constructor
  SetHistDisplayOptions(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  StatusCode m_statusCode;
  
  // @property PasswordFile
  // path to file containing the password for the OnlineHistDB user HIST_WRITER
  // this file should be excluded from CVS to prevent a password leak
  std::string m_passwordFile;
  
  // @property HistoBase
  // Fixed part of the histogram names comming form the Algorithm
  std::string m_histoBase;
  
  // @property HistoNames
  // map storing the names of the histograms that will be edited
  // If the Boolean is true, saveHistoSetDisplayOptions() will be called
  // i.e. the changes will be applied for the whole set
  std::map<std::string, int> m_histoNames;
   
  // Lists of Options.
  // These are the same for all Histogramms defined in m_histoNames
  bool m_unsetOptions;                              // @property UnsetOptions  boolean, if true unset all options first 
  std::map<std::string, int> m_intOptions;          // @property intOptions    List with all integer options
  std::map<std::string, double> m_doubleOptions;    // @property floatOptions  List with all float options
                                                    // As there is no mapping from python to map<string, float>
                                                    // double is used and the value is casted before using.
  std::map<std::string, std::string> m_strOptions;  // @property stringOptions List with all string options
};

#endif // HISTDBPYTHON_SETHISTDISPLAYOPTIONS_H

