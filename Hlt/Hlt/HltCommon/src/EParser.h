// $Id: EParser.h,v 1.1 2009-01-21 12:04:32 graven Exp $
#ifndef HLTBASE_EPARSER_H 
#define HLTBASE_EPARSER_H 1

// Include files
#include <string>
#include <vector>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

/** @class ParserDescriptor ParserDescriptor.h
 *  Helper class to book histogram from a string
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-03-01
 */

namespace EParser
{
  std::vector<std::string> parse(const std::string& descriptor,
                                 const std::string& separator);
  
  bool parseFilter(const std::string& decriptor,
                   std::string& namefilter, std::string& mode,
                   double& x0, double& xf);

};
#endif // HLTBASE_PARSERDESCRIPTOR_H
