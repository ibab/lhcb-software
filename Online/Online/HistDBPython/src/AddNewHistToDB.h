// $Id: AddNewHistToDB.h,v 1.2 2009-03-30 14:37:58 nchiapol Exp $

#ifndef HISTDBPYTHON_AddNewHistToDB_H
#define HISTDBPYTHON_AddNewHistToDB_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
//#include "AIDA/IHistogram1D.h"

// Online Hist DB
#include <stdio.h>
#include "OnlineHistDB/OnlineHistDB.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class AddNewHistToDB AddNewHistToDB.h 
 *
 *  Algorithm to add new histograms to the database. 
 *  
 *  Algorithm properties:
 *  - \b PasswordFile: String containing the name (including path, if needed) of the file 
 *    containing the password for the OnlineHistDB user HIST_WRITER.
 *    this file must be excluded from CVS. The default value is "password".
 *  - \b TaskName
 *  - \b AlgorithmName
 *  - \b HistoNames: list of strings containing the names of the histograms to create.
 *  - \b HistoType: Type of the histograms. All histograms need to have the same type.
 *    Possible values are defined in OnlineHistDBEnv (H1D, H2D, P1D, P2D, CNT)
 *  
 *  For more details on how to use this algrithm have a look at the examples in the 
 *  doc/python directory. 
 * 
 *  @author Nicola Chiapolini 
 *  @date   2009-03-24
 */
class AddNewHistToDB : public GaudiHistoAlg {

public:
  /// Constructor
  AddNewHistToDB(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:
  StatusCode m_statusCode;
  
  // @option PasswordFile
  // name (including path, if necessary) of the file containing the password 
  // for the OnlineHistDB user HIST_WRITER
  // this file must be excluded from CVS
  // default value is "password"
  std::string m_passwordFile;
  
  // variable for temporary storage of histograms
  //OnlineHistogram* tmpActiveHisto;

  // Fixed part of the histogram names (e.g. Task and Algorithm)
  std::string m_taskName;
  
  // Prefix for all page names, use it to identify your folder
  std::string m_algoName;

  // @option HistoType
  std::string               m_histTypeName;

  // @option HistoNames
  // vector of lists; each list contains the names of the histograms to be placed on one page
  //std::vector<std::vector<std::string> > m_histoNames;
  std::vector<std::string > m_histoNames;
  
};

#endif // HISTDBPYTHON_AddNewHistToDB_H

