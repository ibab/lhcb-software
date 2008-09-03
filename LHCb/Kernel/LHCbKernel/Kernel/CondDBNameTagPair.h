// $Id: CondDBNameTagPair.h,v 1.2 2008-09-03 15:36:09 cattanem Exp $
#ifndef KERNEL_CONDDBNAMETAGPAIR_H 
#define KERNEL_CONDDBNAMETAGPAIR_H 1

#include <string>
#include <utility>

namespace LHCb 
{
  /// DB name, tag pair
  typedef std::pair<std::string,std::string> CondDBNameTagPair;
}

#endif // KERNEL_CONDDBNAMETAGPAIR_H
