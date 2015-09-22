#ifndef KERNEL_STLOSTREAMREDIRECT_H 
#define KERNEL_STLOSTREAMREDIRECT_H 1

#include <iostream>
#include <unistd.h>
#include <stdio.h>

namespace STL
{

  /** @class OStreamRedirect STLOStreamRedirect.h Kernel/STLOStreamRedirect.h
   *  
   *  Simple gaurd like class that redirects all cout and cerr printout
   *  to the given address, as long as the gaurd exists.
   *
   *  @author Chris Jones
   *  @date   2015-09-22
   */
  class OStreamRedirect 
  {
    
  public: 
    
    /// Constructor. Starts redirection.
    explicit OStreamRedirect( const std::string & loc = "/dev/null" )
    {
      original_stdout = dup(fileno(stdout));
      fflush(stdout);
      freopen(loc.c_str(),"w",stdout);
      original_stderr = dup(fileno(stderr));
      fflush(stderr);
      freopen(loc.c_str(),"w",stderr);
    }
    
    /// Destructor. Put streams back to normal.
    ~OStreamRedirect( )
    {
      fflush(stdout);
      dup2(original_stdout,fileno(stdout));
      close(original_stdout);
      fflush(stderr);
      dup2(original_stderr,fileno(stderr));
      close(original_stderr);
    }
    
  private:
    
    int original_stdout; ///< Original stdout
    int original_stderr; ///< original stderr
    
  };

}

#endif // KERNEL_STLOSTREAMREDIRECT_H
