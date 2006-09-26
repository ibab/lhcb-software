#ifndef HIST_STRUCT_DEF
#define HIST_STRUCT_DEF
#include "hist_types.h"
/*  
                     header histogram block descriptor  
                                                               */ 
typedef struct  
{ 
		int		hleng;			// Header Length in Bytes
		int		hver;			// Header Version
		int		valid;
        int		*next;          /*  next block address
									Self-Relative, i.e. as offset from the start of block.
									if equal to block start then eof    
								*/ 
        int		*previous;      /*  previous block address
										Self-Relative, i.e. as offset from the start of block.
										if equal to block start then eof    
								*/ 
        bindesc		*h_start;        /*  start histo
									Self-Relative, i.e. as offset (in Bytes from the start of block)
								*/
		int		flags;
        int		id;				/*  histo id number    >0            */ 
        int		length;			/*  block length    in bytes         */ 
        int		type;			/*  histo type     0 float   1 int   */ 
//        int		enable;			/*  histo enable   0 disable enab  */ 
        int		l_title;		/*  title length	in bytes          */ 
        char	title[L_TITLE]; /*  title characters        */ 
        int		xnbin;			/*  n bin in x                       */ 
        int		ynbin;			/*  n bin in y                       */ 
        int		entries;		/*  nb. entries	statistics       */ 
        bintype	xmin;           /*  min in x		bintype            */ 
        bintype	xmax;           /*  max in x		bintype            */ 
		bintype	xbinw;          /*  bin width in x	bintype            */ 
		bintype	ymin;           /*  min in y        bintype            */ 
		bintype	ymax;           /*  max in y        bintype            */ 
		bintype	ybinw;          /*  bin in y        bintype            */ 
}HDescriptor;            

#define HFLAG_AUTO_PUBLISH (1<<0)
#define HFLAG_HIST_DISABLED (1<<1)


/*  ------------------------------------------------------------------- */    
/* 
                data structure in the histo 
				ALL bins are actually bin descriptors (see hist_types.h)
                
                     n in X direction 
                     m in Y direction 
                      
                     ( y...... ) * dimension of x +  (x.......) 
                
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for underflow.y 
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for bin_1.y              
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for bin_2.y              
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for bin_3.y              
  
 ....................................................................... 
 
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for bin_m.y              
 underflow.x, bin_1.x, bin_2.x, bin_3.x, bin_4.x,    ,bin_n.x, overflow.x  / for overflow.y              
                                                                        */ 
/*  ------------------------------------------------------------------- */    

#endif