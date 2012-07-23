      INTEGER*4 FUNCTION IOUNITS_ADDRESS() 
      IMPLICIT NONE

      integer niosta,niopar,niowgt,niounw
      common/iounits/niosta,niopar,niowgt,niounw

      INTEGER*4 GETADDR 

      IOUNITS_ADDRESS = GETADDR(niosta)
 999  RETURN
      END
