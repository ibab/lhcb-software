
#ifndef RICHMARKOV_CONFIGURATION_H
#define RICHMARKOV_CONFIGURATION_H

/* There is one "Configuration" that tells the rest of the program
   how to work differently in Rich1 or Rich2 scenarios etc.
   This (and the rest of the program) is not thread-safe. */

/* I'm may or may not want all configurations to inherit from a common
   base, so in case, here is a very simple base class that could grow
   later if desired. */

namespace RichMarkov {
  
  class Configuration { 
  protected:
    Configuration() {};
  };

}

#endif
