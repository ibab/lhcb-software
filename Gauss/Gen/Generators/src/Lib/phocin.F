C $Id: phocin.F,v 1.4 2005-12-21 15:47:36 robbep Exp $
      SUBROUTINE PHOCIN
C.----------------------------------------------------------------------
C.
C.    PHOTOS:   PHOton Common INitialisation
C.
C.    Purpose:  Initialisation of parameters in common blocks.
C.
C.    Input Parameters:   None
C.
C.    Output Parameters:  Commons /PHOLUN/, /PHOPHO/, /PHOCOP/, /PHPICO/
C.                                and /PHSEED/.
C.
C.    Author(s):  B. van Eijk                     Created at:  26/11/89
C.                Z. Was                          Last Update: 29/01/05
C.
C.----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER d_h_NMXHEP

CC  LHCb Change: remove include and set HEP sive to 10000 by hand
      PARAMETER (d_h_NMXHEP=10000)

      LOGICAL QEDRAD
      COMMON/PHOQED/QEDRAD(d_h_NMXHEP)
      INTEGER PHLUN
      COMMON/PHOLUN/PHLUN
      REAL*8 ALPHA,XPHCUT
      COMMON/PHOCOP/ALPHA,XPHCUT
      REAL*8 PI,TWOPI
      COMMON/PHPICO/PI,TWOPI
      INTEGER ISEED,I97,J97
      REAL*8 URAN,CRAN,CDRAN,CMRAN
      COMMON/PHSEED/ISEED(2),I97,J97,URAN(97),CRAN,CDRAN,CMRAN
      INTEGER PHOMES
      PARAMETER (PHOMES=10)
      INTEGER STATUS
      COMMON/PHOSTA/STATUS(PHOMES)
      LOGICAL INTERF,ISEC,ITRE,IEXP,IFTOP,IFW
      REAL*8 FINT,FSEC,EXPEPS
      COMMON /PHOKEY/ FSEC,FINT,EXPEPS,INTERF,ISEC,ITRE,IEXP,IFTOP,IFW
      INTEGER INIT,I
      SAVE INIT
      DATA INIT/ 0/
C--
C--   Return if already initialized...
      IF (INIT.NE.0) RETURN
      INIT=1
C--
C--   Preset switch  for  photon emission to 'TRUE' for each particle in
C--   /PH_HEPEVT/, this interface is needed for KORALB and KORALZ...
      DO 10 I=1,d_h_NMXHEP
   10 QEDRAD(I)=.TRUE.
C--
C--   Logical output unit for printing of PHOTOS error messages
CCCC   LHCb change to be able to remove PHOTOS printout
      IF ( PHLUN .NE. 73 ) PHLUN=6
C--
C--   Set cut parameter for photon radiation
      XPHCUT=0.01 D0 ! 0.0001D0! to go to low valuex (IEXP excepted) 
C--                            ! switch to - VARIANT B
C--
C--   Define some constants
      ALPHA=0.00729735039D0
      PI=3.14159265358979324D0
      TWOPI=6.28318530717958648D0
C--
C--   Default seeds Marsaglia and Zaman random number generator
      ISEED(1)=1802
      ISEED(2)=9373
C--
C--   Iitialization for extra options
C--   (1)
C--   Interference weight now universal. 
      INTERF=.TRUE.
C--   (2)
C--   Second order - double photon switch
      ISEC=.TRUE.
C--   Third/fourth order - triple (or quatric) photon switch, 
C--                        see dipswitch ifour
      ITRE=.FALSE.
C--   Exponentiation on:
CCC   LHCb Change Enable exponentiation
      IEXP=.TRUE. !.TRUE.
      IF (IEXP) THEN
      ISEC=.FALSE.
      ITRE=.FALSE.
      CALL PHCORK(5)  ! in case of exponentiation correction of ph space
                      ! is a default mandatory
      XPHCUT=0.000 000 1 
      EXPEPS=1D-4
      ENDIF
C--   (3)
C--   Emision in the hard process g g (q qbar) --> t tbar 
C--                                 t          --> W b
      IFTOP=.TRUE.
C--
C--   further initialization done automatically
C--   see places with - VARIANT A - VARIANT B - all over
C--   to switch between options.     
C ----------- SLOWER VARIANT A, but stable ------------------
C --- it is limiting choice for small XPHCUT in fixed orer
C --- modes of operation
      IF (INTERF) THEN
C--   best choice is if FINT=2**N where N+1 is maximal number 
C--   of charged daughters
C--   see report on overweihted events
        FINT=2.0D0
      ELSE
        FINT=1.0D0
      ENDIF
C ----------- FASTER VARIANT B  ------------------
C -- it is good for tests of fixed order and small XPHCUT
C -- but is less promising for more complex cases of interference
C -- sometimes fails because of that
C 
C      IF (INTERF) THEN
C       FINT=1.80D0
C      ELSE
C        FINT=0.0D0
C      ENDIF
C----------END VARIANTS A B -----------------------	

C--   Effects of initial state charge (in leptonic W decays)
C--   
      IFW=.TRUE.
C--   Initialise status counter for warning messages
      DO 20 I=1,PHOMES
   20 STATUS(I)=0
      RETURN
      END
