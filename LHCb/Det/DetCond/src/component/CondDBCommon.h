#ifndef CONDDBCOMMON_H_
#define CONDDBCOMMON_H_
// $Id: CondDBCommon.h,v 1.1 2008-06-26 14:22:45 marcocle Exp $

#include <string>
#include <vector>
#include "DetCond/ICondDBReader.h"

// forward declaration
namespace cool{
  class RecordSpecification;
}

/** @file Utility functions shared among DetCond components.
 * 
 * @author Marco Clemencic
 */
namespace CondDB {
  void generateXMLCatalog(const std::string &name,
                          const std::vector<std::string> &fldrs,
                          const std::vector<std::string> &fldrsets,
                          std::string &data);
  
  StatusCode generateXMLCatalog(ICondDBReader *reader, const std::string &path,
                                ICondDBReader::DataPtr &data);

  const cool::RecordSpecification& getXMLStorageSpec();
}

#endif /*CONDDBCOMMON_H_*/
