C $Id: common_hepevt.inc,v 1.1 2004-05-06 17:47:12 robbep Exp $
C----------------------------------------------------------------------
C  Purpose : Common HEPEVT declaration to be used for Photos
C
C  Author  : Patrick Robbe
C  Created : 2004-05-06
C----------------------------------------------------------------------
      INTEGER  d_h_nmxhep         ! maximum number of particles
      PARAMETER ( d_h_nmxhep=4000)
      REAL*8  d_h_phep,  d_h_vhep ! to be real*4 or *8  depending on host
      INTEGER d_h_nevhep,d_h_nhep,d_h_isthep,d_h_idhep,d_h_jmohep,
     $        d_h_jdahep
      COMMON /hepevt/
     $      d_h_nevhep,               ! serial number
     $      d_h_nhep,                 ! number of particles
     $      d_h_isthep(d_h_nmxhep),   ! status code
     $      d_h_idhep(d_h_nmxhep),    ! particle ident KF
     $      d_h_jmohep(2,d_h_nmxhep), ! parent particles
     $      d_h_jdahep(2,d_h_nmxhep), ! childreen particles
     $      d_h_phep(5,d_h_nmxhep),   ! four-momentum, mass [GeV]
     $      d_h_vhep(4,d_h_nmxhep)    ! vertex [mm]
      SAVE /HEPEVT/