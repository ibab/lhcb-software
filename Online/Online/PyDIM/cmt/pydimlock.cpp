/* ensure that with the DIM lock the python interpreter lock is also taken */

#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>

void dim_lock()
{
  	static void  (*func)(void);

  	if(!func) {
		dlerror();
		if (!(func = (void (*)()) dlsym(RTLD_NEXT, "dim_lock"))) printf("%s\n", dlerror());
	}
  printf("dim_lock() is called\n"); 
  func();	    
  return;
}

dim_unlock()
{
	
  static void  (*func)(void);

  	if(!func) {
		dlerror();
		if (!(func = (void (*)()) dlsym(RTLD_NEXT, "dim_unlock"))) printf("%s\n", dlerror());
	}
  printf("dim_unlock() is called\n"); 
  func();	    
  return;
}
