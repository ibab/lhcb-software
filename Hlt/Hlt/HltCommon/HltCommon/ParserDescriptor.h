// $Id: ParserDescriptor.h,v 1.1 2006-09-22 11:36:07 hernando Exp $
#ifndef HLTCOMMON_PARSEDESCRIPTOR_H 
#define HLTCOMMON_PARSEDESCRIPTOR_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include <string>
#include <vector>

/** @class ProperyHistos ProperyHistos.h HltCommon/ProperyHistos.h
 *  
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
#endif // HLTCOMMON_PARSEDESCRIPTOR_H
