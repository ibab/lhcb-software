      integer function hilunhep(beam)
      
      INTEGER BEAM
      
C...Purpose: to convert HIJING event record contents to
C...the standard event record common block.
C...returns number of b quarks in the event
C...(why not measure bbbar cross section with beam-gas events?)
      
      DOUBLE PRECISION DUMMY
      INTEGER REVERT
      
      CHARACTER*16 CHAU
      
      COMMON/HIMAIN1/NATT,EATT,JATT,NT,NP,N0,N01,N10,N11 
      COMMON/HIMAIN2/KATT(130000,4),PATT(130000,4) 
      
C...HEPEVT commonblock.
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &  JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
      
C...      NEVHEP       - event number
C...      NHEP         - number of entries in this event
C...      ISTHEP(..)   - status code
C...      IDHEP(..)    - particle ID, P.D.G. standard
C...      JMOHEP(1,..) - position of mother particle in list
C...      JMOHEP(2,..) - position of second mother particle in list
C...      JDAHEP(1,..) - position of first daughter in list
C...      JDAHEP(2,..) - position of last daughter in list
C...      PHEP(1,..)   - x momentum in GeV/c
C...      PHEP(2,..)   - y momentum in GeV/c
C...      PHEP(3,..)   - z momentum in GeV/c
C...      PHEP(4,..)   - energy in GeV
C...      PHEP(5,..)   - mass in GeV/c**2
C...      VHEP(1,..)   - x vertex position in mm
C...      VHEP(2,..)   - y vertex position in mm
C...      VHEP(3,..)   - z vertex position in mm
C...      VHEP(4,..)   - production time in mm/c
C
C -------------------------------------------------------------
C
      
      logical lfirst
      data lfirst/.TRUE./
      save lfirst
      INTEGER COUNTER
      
      COUNTER = 0 
      
C...first call
      if(lfirst)then
        NEVHEP = 0
        lfirst=.FALSE.
      endif
      
C...decide if beam1 or beam2 in case of random beams
C...use LbHijing ran function as generator
      revert=0
      if(ran(dummy).gt.0.5) revert=1
      
C...Conversion
      NEVHEP=NEVHEP + 1
      if(NATT.GT.NMXHEP) call pyerrm(8,
     &  '(LUNHEP:) no more space in /HEPEVT/')
      NHEP=MIN(NATT,NMXHEP)
      do 150 I=1,NHEP
        ISTHEP(I)=0
        if(KATT(I,4).EQ.1) ISTHEP(I)=1
        if(KATT(I,4).EQ.11) ISTHEP(I)=2
CC.... Count if it is a b quark
        IPDG = ABS( KATT( I, 1) )
        IQ   = MOD( INT( IPDG / 100 ) , 10 )
        IQB  = MOD( INT( IPDG / 1000 ) , 10 )
        IQM  = 0
        IQMB = 0
        IF ( KATT( I, 3) .NE. 0 ) THEN
          IPDGM = ABS( KATT( KATT(I,3) , 1 ) )
          IQM   = MOD( INT( IPDGM / 100 ) , 10 )
          IQMB  = MOD( INT( IPDGM / 1000 ) , 10 )
        ENDIF
        
CC .... This is a B meson and the mother is not a B hadron
        IF ( IQ .EQ. 5 .AND. IQB .NE. 5 .AND. IQM .NE. 5 
     >    .AND. IQMB .NE. 5 ) THEN
          COUNTER = COUNTER + 1
          IQD     = MOD( INT( IPDG / 10 ) , 10 ) 
CC .... Check if it is a bb meson
          IF ( IQD .EQ. 5 ) COUNTER = COUNTER + 1
        ENDIF
        
CC .... This is a B baryon and the mother is not a B hadron
        IF ( IQB .EQ. 5 .AND. IQM .NE. 5 .AND. IQMB .NE. 5 ) THEN
          COUNTER = COUNTER + 1
CC .... Check it is not a bb baryon
          IF ( IQ .EQ. 5 ) COUNTER = COUNTER + 1
        ENDIF
        
CC.... transform to PDG code to store in HepMC
        IDHEP(I)=lutran(KATT(I,1),1)
        CALL PYNAME(IDHEP(I),CHAU)
c        print *, '::::::::::>',katt(I,1), IDHEP(I), 
c     .                         PYMASS(KATT(I,1)), ' ', CHAU
        JMOHEP(1,I)=KATT(I,3)
        JMOHEP(2,I)=0
        
C...Look for daughters of this particle
        MMIN=999999999
        MMAX=0
        DO M=1,NATT
c          PRINT *, '==========> ', M, KATT(M,3), I
          IF(KATT(M,3).eq.I) THEN
            MMIN=MIN(M,MMIN)
            MMAX=MAX(M,MMAX)
          END IF
        END DO
        
        IF(MMIN.eq.999999999) THEN
          JDAHEP(1,I)=0
          JDAHEP(2,I)=0
        ELSE
          JDAHEP(1,I)=MMIN
          JDAHEP(2,I)=MMAX
          
c          DO M=MMIN,MMAX
c            PRINT *, '------> ', MMIN, MMAX, M, KATT(M,3)
c          END DO
          
        END IF
        
        
        do 100 J=1,4
          PHEP(J,I)=PATT(I,J)
 100    CONTINUE
        
c revert 3 momentum if beam2, of fifty-fifty if beam12
        if (beam.eq.2) then
          PHEP(1,I)=-PHEP(1,I)
          PHEP(2,I)=-PHEP(2,I)
          PHEP(3,I)=-PHEP(3,I)
        else if(beam.eq.3) then
          if(revert.eq.1) then
            PHEP(1,I)=-PHEP(1,I)
            PHEP(2,I)=-PHEP(2,I)
            PHEP(3,I)=-PHEP(3,I)
          end if
        end if
        
c        PHEP(5,I)=SQRT(PATT(I,4)**2-PATT(I,1)**2-
c     .                 PATT(I,2)**2-PATT(I,3)**2)
        
Ccc     use nominal mass and shutup
        PHEP(5,I)=PYMASS(KATT(I,1))
        
        do 110 J=1,4
          VHEP(J,I)=0.
 110    CONTINUE
        
c	print *,"PHEP =========> ", (PHEP(J,I),J=1,5)
c	print *,"VHEP =========> ", (VHEP(J,I),J=1,4)
        
 150  CONTINUE
      
      hilunhep = COUNTER
      return
      end
      
      
