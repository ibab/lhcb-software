      integer function hilunhep(beam)

      INTEGER BEAM
      
C...Purpose: to convert HIJING event record contents to
C...the standard event record common block.
C...returns number of b quarks in the event
C...(why not measure bbbar cross section with beam-gas events?)
      
      DOUBLE PRECISION PYMASS

C      DOUBLE PRECISION DUMMY
      INTEGER DUMMY
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
      JJ=0
      do 150 I=1,NHEP

        if(PATT(I,1).EQ.0..AND.PATT(I,2).EQ.0.) GOTO 150

        JJ=JJ+1

        ISTHEP(JJ)=0
        if(KATT(I,4).EQ.1) ISTHEP(JJ)=1
        if(KATT(I,4).EQ.11) ISTHEP(JJ)=2
 
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
        IDHEP(JJ)=lutran(KATT(I,1),1)
        JMOHEP(1,JJ)=KATT(I,3)
        JMOHEP(2,JJ)=0

CC.... no daughter particles, event history is not retained        
        JDAHEP(1,JJ)=0
        JDAHEP(2,JJ)=0
        
        do 100 J=1,3
          PHEP(J,JJ)=PATT(I,J)
 100    CONTINUE
        
c revert 3 momentum if beam2, of fifty-fifty if beam12
        if (beam.eq.2) then
          PHEP(1,JJ)=-PHEP(1,JJ)
          PHEP(2,JJ)=-PHEP(2,JJ)
          PHEP(3,JJ)=-PHEP(3,JJ)
        else if(beam.eq.3) then
          if(revert.eq.1) then
            PHEP(1,JJ)=-PHEP(1,JJ)
            PHEP(2,JJ)=-PHEP(2,JJ)
            PHEP(3,JJ)=-PHEP(3,JJ)
          end if
        end if
        
Ccc     use nominal mass and shutup
        PHEP(5,JJ)=PYMASS(KATT(I,1))
        PHEP(4,JJ)=SQRT(PHEP(1,JJ)**2+PHEP(2,JJ)**2+
     .                  PHEP(3,JJ)**2+PHEP(5,JJ)**2)
 
cc        CALL PYNAME(IDHEP(JJ),CHAU)
cc        PRINT *, '--->', JJ, ' ', CHAU, PHEP(1,JJ), PHEP(2,JJ),
cc     .                           PHEP(3,JJ), PHEP(4,JJ), PHEP(5,JJ)
       
        do 110 J=1,4
          VHEP(J,JJ)=0.
 110    CONTINUE
        
 150  CONTINUE

      NHEP=JJ
      
      hilunhep = COUNTER
      return
      end
      
      
