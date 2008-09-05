
#ifndef _RATESERVICE_DEBUGMACRO_H_
#define _RATESERVICE_DEBUGMACRO_H_

#include <iostream>

#define DISPLAY__DEBUG 1

/* Debug display macro printing file name and line number plus a given message
 * (any ostream) wherever it is called.
 */

#if DISPLAY__DEBUG
  #define COUT_DEBUG(c){ std::cout << __FILE__ << " l." << __LINE__ << " " << c << std::endl; }
#else
  #define COUT_DEBUG(c) {}
#endif




#endif
