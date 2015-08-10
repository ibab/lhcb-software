*CMZ :  1.00/01 15/12/94  09.52.52  by  ZEUS Offline Group
*CMZ :  1.00/00 15/12/94  08.17.05  by  Unknown
*-- Author :    ZEUS Offline Group   17/08/94
        Subroutine ZDUINI
*       =================
*
*----------------------------------------------------------------------------
*
*    ZDUINI
*    ======
*
*   Initialisation routine for ZDis for LPair lepton pair generator.
*
*---------------------------------------------------------------------------
*
        Implicit NONE
*
*KEEP,BEAM.
      INTEGER          INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,IPAIR,
     & NQUARK
      REAL*8           INPE,INPP
      COMMON /BEAM/    INPE,INPP,INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,
     &                 IPAIR,NQUARK

*KEEP,CUTS.
      INTEGER      MODCUT
      REAL*4       THMAX,THMIN,MXMN,MXMX,Q2MN,Q2MX
      REAL*8       COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,QP2MIN,QP2MAX
      COMMON /CUTS/COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,
     &             THMAX,THMIN,QP2MIN,QP2MAX,MODCUT,MXMN,MXMX,Q2MN,Q2MX

*KEEP,VEGPAR.
      INTEGER          NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND
      COMMON /VEGPAR/  NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND

*KEND.
*
C*  End of common
*
        Integer JErr
*
*--------  Initialise
*
*..  No error so far:
        JErr = 0
*
*..  Header message:
        Write (6, 9000)
*
*--------  Set up LPair:
*
*..  Set default parameters:
        Call GMUINI
*
*..  Let data cards over write the defaults:
        Call GMUCHA
        Write (6, 9010) IBEG, IEND, NTreat, NPrin, NCVG, ITMX, NPoin,
     +                  INPP, PMOD, GPDF, SPDF, INPE, EMOD, IPAIR,
     +                  NQUARK,
     +                  MODCUT, THMAX, THMIN, ECUT, PTCUT, Q2MN, Q2MX,
     +                  MXMN, MXMX
*
*..  Store the generation parameters in USGRUN
!-        Call ZLPUSG
*
*..  Initialise cross sections:
        Call GMUBEG
*
*--------  No more - return
*
8900    Continue
        Return
*
*--------  Formats
*
9000    Format('1'////
     + 8X, '**********************************************************'/
     + 8X, '*                                                        *'/
     + 8X, '*  ZLPAIR. Interface to Vermaseren et al event generator.*'/
     + 8X, '*  =======                                               *'/
     + 8X, '*                                                        *'/
     + 8X, '*   ',
*KEEP,QFTITLE,N=30.
     + 30HZLPAIR   1.00/03  02/02/95  19
*KEND.
     +                                      ,'                       *'/
     + 8X, '**********************************************************'/
     +   )
*
9010    FORMAT(' Cut and parameter values used:'/
     +         ' ------------------------------'//
     + 8X, '  IBEG:', I8/
     + 8X, '  IEND:', I8/
     + 8X, 'NTreat:', I8/
     + 8X, ' NPrin:', I8/
     + 8X, '  NCVG:', I8/
     + 8X, '  ITMX:', I8/
     + 8X, ' NPoin:', I8/
     + 8X, '  INPP:', G16.4/
     + 8X, '  PMOD:', I8/
     + 8X, '  GPDF:', I8/
     + 8X, '  SPDF:', I8/
     + 8X, '  INPE:', G16.4/
     + 8X, '  EMOD:', I8/
     + 8X, ' IPair:', I8/
     + 8X, 'NQuark:', I8/
     + 8X, 'ModCut:', I8/
     + 8X, ' ThMax:', G16.4/
     + 8X, ' ThMin:', G16.4/
     + 8X, '  ECut:', G16.4/
     + 8X, ' PTCut:', G16.4/
     + 8X, '  Q2MN:', G16.4/
     + 8X, '  Q2MX:', G16.4/
     + 8X, '  MXMN:', G16.4/
     + 8X, '  MXMX:', G16.4/
     + )
*
        End
