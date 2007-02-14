#include "HltBase/ParserDescriptor.h"
#include <iostream>
#include <stdlib.h>

/** @file
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-03-01
 */


std::vector<std::string> ParserDescriptor::parse(const std::string& descriptor)
{
  std::string marker = ",";
  std::string cromo = descriptor;
  typedef std::string::size_type stype;
  std::vector<std::string> gens;
  stype i0 = cromo.find(marker);
  while (i0<cromo.size()) {
    std::string gen = std::string(cromo,0,i0);
    gens.push_back(gen);
    cromo = std::string(cromo,i0+1,cromo.size());
    // std::cout << " gen " << gen << " cromo " << cromo << std::endl;
    i0 = cromo.find(marker);
  }
  gens.push_back(cromo);
  return gens;
}


bool ParserDescriptor::parseHisto1D(const std::string& descriptor,
                                    std::string& title,
                                    int& n, float& x0, float& xf)
{ 
  std::vector<std::string> gens = ParserDescriptor::parse(descriptor);
  // for (std::vector<std::string>::const_iterator it = gens.begin();
  //     it != gens.end(); it++)
  //  std::cout << *it << std::endl;
  unsigned int nsize = gens.size();
  if (nsize<4) return false;
  title = gens[nsize-4];
  char* pend;
  n =  (int)   strtod(gens[nsize-3].c_str(),&pend);
  x0 = (float) strtod(gens[nsize-2].c_str(),&pend);
  xf = (float) strtod(gens[nsize-1].c_str(),&pend);
  return true;
}


