// $Id: EParser.h,v 1.2 2009-02-22 19:53:24 graven Exp $
#ifndef HLTBASE_EPARSER_H 
#define HLTBASE_EPARSER_H 1

// Include files
#include <string>

/** @class ParserDescriptor ParserDescriptor.h
 *  Helper class to book histogram from a string
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-03-01
 */

namespace EParser
{
  bool parseFilter(const std::string& decriptor,
                   std::string& namefilter, std::string& mode,
                   double& x0, double& xf);

};
#endif // HLTBASE_PARSERDESCRIPTOR_H
