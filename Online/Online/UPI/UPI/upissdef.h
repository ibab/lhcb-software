#ifndef __UPISSDEF_H
#define __UPISSDEF_H

/* function return codes */
#define UPI_NORMAL 0
#define UPI_ERROR (-1)
 
/* error codes */
#define UPI_FACILITY      80*256
 
#define UPI_NEXMENU      (UPI_FACILITY+1)      /* non existing menu id */
#define UPI_NEXCOMM      (UPI_FACILITY+2)      /* non existing command id */
#define UPI_DISCOMM      (UPI_FACILITY+3)      /* command disabled */

#define UPI_SS_DTBFULL	 (UPI_FACILITY+0x4)
#define UPI_SS_ABORTED	 (UPI_FACILITY+0x6)
#define UPI_SS_EMPTYMENU (UPI_FACILITY+0x8)
#define UPI_SS_INVWINDOW (UPI_FACILITY+0xC)
#define UPI_SS_INVFATHER (UPI_FACILITY+0xE)
#define UPI_SS_DUPLCOMM  (UPI_FACILITY+0x10)
#define UPI_SS_NOOPENMENU (UPI_FACILITY+0x12)
#define UPI_SS_ERRCONV    (UPI_FACILITY+0x14)
#define UPI_SS_INVFORM    (UPI_FACILITY+0x16)
#define UPI_SS_INVPARAM   (UPI_FACILITY+0x18)
#define UPI_SS_INVCOMMAND (UPI_FACILITY+0x1C)
#define UPI_SS_INVMENU    (UPI_FACILITY+0x1E)
#define UPI_SS_NORMAL     0
#define UPI___FACILITY     80

#endif /* __UPISSDEF_H */
