// $Id: EParser.h,v 1.1 2007-06-25 20:40:11 hernando Exp $
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
  
  bool parseHisto1D(const std::string& descriptor,
                    std::string& title,
                    int& n, float& x0, float& xf);
  
  bool parseFilter(const std::string& decriptor,
                   std::string& namefilter, std::string& mode,
                   float& x0, float& xf);

};
#endif // HLTBASE_PARSERDESCRIPTOR_H
