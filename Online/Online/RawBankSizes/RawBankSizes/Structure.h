// $Id: Structure.h,v 1.1 2010-02-15 16:00:32 jost Exp $
#ifndef BEAT_STRUCTURE_H 
#define BEAT_STRUCTURE_H 1

// Include files
#include "GaudiKernel/StatusCode.h"
#include <string>

namespace HParam
{
	class HistParams   
	{
	public:
	  int n_bin;
	  float xmin;
	  float xmax;
	  int s_min;
	  int s_max;
    std::string det;

	  HistParams()
	  {
	  	n_bin = -2;
		  xmin = 0.0;
		  xmax = 0.0;
		  s_min = 0;
		  s_max = -2;
      det = "Unknown";
	  };
	};
}
// ============================================================================
namespace Gaudi
{
  namespace Utils 
  {
    std::ostream& toStream 
    ( const HParam::HistParams& , std::ostream&   ) ;
  }
}

// ============================================================================
namespace Gaudi
{
  namespace Parsers 
  {
    StatusCode  parse ( HParam::HistParams& o , 
                        const std::string& input ) ;
  }
}
// ============================================================================
// The END 
// ============================================================================
#endif // BEAT_STRUCTURE_H
// ============================================================================
