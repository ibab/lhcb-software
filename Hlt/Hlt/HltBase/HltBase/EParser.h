// $Id: EParser.h,v 1.3 2008-07-04 08:07:12 graven Exp $
#ifndef HLTBASE_EPARSER_H 
#define HLTBASE_EPARSER_H 1

// Include files
#include <string>
#include <vector>

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
