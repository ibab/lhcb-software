// $Id: ParserDescriptor.h,v 1.2 2007-02-05 08:40:16 hernando Exp $
#ifndef HLTBASE_PARSEDESCRIPTOR_H 
#define HLTBASE_PARSEDESCRIPTOR_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include <string>
#include <vector>

/** @class ProperyHistos ProperyHistos.h HltBase/ProperyHistos.h
 *  
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
#endif // HLTBASE_PARSEDESCRIPTOR_H
