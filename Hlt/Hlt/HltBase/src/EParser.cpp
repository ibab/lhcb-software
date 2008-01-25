#include "HltBase/EParser.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/lexical_cast.hpp"

/** @file
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-03-01
 */


std::vector<std::string> EParser::parse(const std::string& descriptor,
                                        const std::string& marker)
{
  std::vector<std::string> x;
  return boost::algorithm::split(x,descriptor,boost::algorithm::is_any_of(marker));
}


bool EParser::parseFilter(const std::string& descriptor,
                          std::string& title, std::string& mode,
                          float& x0, float& xf)
{ 
  std::vector<std::string> gens = EParser::parse(descriptor,",");
  unsigned int nsize = gens.size();
  if (nsize<3) return false;
  title = gens[0];
  mode = gens[1];
  x0 = boost::lexical_cast<float>(gens[2]);
  if (nsize >3) xf =boost::lexical_cast<float>(gens[3]); 
  return true;
}
