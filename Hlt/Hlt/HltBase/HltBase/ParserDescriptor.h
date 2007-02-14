// $Id: ParserDescriptor.h,v 1.3 2007-02-14 17:23:52 cattanem Exp $
#ifndef HLTBASE_PARSERDESCRIPTOR_H 
#define HLTBASE_PARSERDESCRIPTOR_H 1

// Include files
#include <string>
#include <vector>

/** @class ParserDescriptor ParserDescriptor.h
 *  Helper class to book histogram from a string
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-03-01
 */

namespace ParserDescriptor 
{
  std::vector<std::string> parse(const std::string& descriptor);

  bool parseHisto1D(const std::string& descriptor,
                    std::string& title,
                    int& n, float& x0, float& xf);
  

};
#endif // HLTBASE_PARSERDESCRIPTOR_H
