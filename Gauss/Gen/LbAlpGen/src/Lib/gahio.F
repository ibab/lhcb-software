      INTEGER*4 FUNCTION AHIO_ADDRESS() 
      IMPLICIT NONE

      integer Nunit,NunitOut,NunitIni
      character*100 filename
c total number of input events
      common/AHio/Nunit,NunitOut,NunitIni,filename

      INTEGER*4 GETADDR 

      AHIO_ADDRESS = GETADDR(Nunit)
 999  RETURN
      END
