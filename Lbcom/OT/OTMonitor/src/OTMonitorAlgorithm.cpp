// $Id: OTMonitorAlgorithm.cpp,v 1.4 2004-12-10 08:10:56 jnardull Exp $

// Gaudi
#include "GaudiKernel/xtoa.h"
#include <stdio.h>

// local
#include "OTMonitorAlgorithm.h"


/** @file OTMonitorAlgorithm.cpp 
 *
 *  Implementation of OTMonitorAlgorithm
 *  
 *  @author M. Needham
 *  @date   21/4/2001
 *  @Adapted by J. Nardulli
 *  @date 20/07/2004
 */

OTMonitorAlgorithm::OTMonitorAlgorithm(const std::string& name,
			 ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  /// OTMonitorAlgorithm constructor
  declareProperty("fullDetail", m_fullDetail = false);
}

OTMonitorAlgorithm::~OTMonitorAlgorithm()
{
  /// OTMonitorAlgorithm destructor
}

std::string OTMonitorAlgorithm::histoDirPath() const
{ 
  /// Return histogram base directory path for this algorithm.

  // construct relative path
  std::string tAlgRelDirPath = this->name();
  std::string::size_type iChar = 0;
  
  // enum to char
  char ch[1];
  sprintf(ch,"%c",IDataProviderSvc::SEPARATOR);

  // can only append char to string !!!!
  std::string dirString ="";
  dirString += ch;

  // replace _ with /
  while ((iChar = tAlgRelDirPath.find(".")) != std::string::npos) {
    tAlgRelDirPath.replace(iChar, 1, dirString);
  }
  tAlgRelDirPath += dirString;
  
  std::string tDirPath = "OT/" + tAlgRelDirPath;

  return tDirPath;
}

std::string OTMonitorAlgorithm::intToString(const int id) const {
 
  // int to string 
  char buffer[32];
  return ::_itoa(id,buffer,10);
}
