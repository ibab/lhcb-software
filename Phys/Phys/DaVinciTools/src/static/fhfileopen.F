*
      SUBROUTINE FHFileOpen(istatus,cname,fname)
*
********************************************************************
*                                                                  *
* Subroutine FHFileOpen                                             *
*                                                                  *
*                                                                  *
* Called by: AxSelAlgorithm                                        *
*                                                                  *
* Author: G.Corti, CERN                                            *
* Date  : 18 May 2001                                              *
*                                                                  *
********************************************************************
*
      IMPLICIT NONE
*-----------------------------------------------------------------
* Input variables
      CHARACTER*4 cname
      CHARACTER*(*) fname
      INTEGER istatus
* External
      INTEGER LENOCC
* Local variables
      INTEGER istat, lf 
*-----------------------------------------------------------------
*
* Set initial values in common block as in input arguments
*
      lf=LENOCC(fname)
      CALL HROPEN(111,cname,fname(1:lf),'N',1024,istat)
      IF(istat.NE.0) THEN
	istatus = istat
        GOTO 999
      ENDIF
*
*   Book hists. Note that you should not work in the PAWC root directory.
*   The subdirectory has the same name as the output stream.
      CALL HMDIR(cname,'S')
*
      CALL HCDIR('//PAWC',' ')
      CALL HMDIR(cname,'S')

      CALL HCDIR('//PAWC/'//cname,' ')
*
999   RETURN
      END
*
* End of FHFileOpen
