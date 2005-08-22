C $Id: lunhep.F,v 1.2 2005-08-22 16:23:36 ranjard Exp $
      integer function lunhep(mconv)

C...Purpose: to convert JETSET event record contents to or from
C...the standard event record common block.
C...convert (mconv=1) from JETSET numbering scheme to PDG numbering scheme
C...     or (mconv=2) from PDG numbering scheme to JETSET numbering scheme
CC.. It returns the number of b quarks in the event


      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
C...Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
C...HEPEVT commonblock.
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
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

C...print version number if this is the first call
      if(lfirst)then
	   continue
c        call stdversn
        if(mconv.EQ.1) NEVHEP = 0
        lfirst=.FALSE.
      endif
C...Conversion from JETSET to standard, the easy part.
      if(mconv.EQ.1) then
        NEVHEP=NEVHEP + 1
        if(N.GT.NMXHEP) call pyerrm(8,
     &  '(LUNHEP:) no more space in /HEPEVT/')
        NHEP=MIN(N,NMXHEP)
        do 150 I=1,NHEP
          ISTHEP(I)=0
          if(K(I,1).GE.1.AND.K(I,1).LE.10) ISTHEP(I)=1
          if(K(I,1).GE.11.AND.K(I,1).LE.20) ISTHEP(I)=2
          if(K(I,1).GE.21.AND.K(I,1).LE.30) ISTHEP(I)=3
          if(K(I,1).GE.31.AND.K(I,1).LE.100) ISTHEP(I)=K(I,1)
CC.... Count if it is a b quark
          IPDG = ABS( K( I, 2) ) 
          IQ   = MOD( INT( IPDG / 100 ) , 10 )
          IQB  = MOD( INT( IPDG / 1000 ) , 10 )
          IQM  = 0
          IQMB = 0
          IF ( K( I, 3) .NE. 0 ) THEN
            IPDGM = ABS( K( K(I,3) , 2 ) ) 
            IQM   = MOD( INT( IPDGM / 100 ) , 10 )
            IQMB  = MOD( INT( IPDGM / 1000 ) , 10 )
          ENDIF

CC .... This is a B meson and the mother is not a B hadron
          IF ( IQ .EQ. 5 .AND. IQB .NE. 5 .AND. IQM .NE. 5 
     >      .AND. IQMB .NE. 5 ) THEN
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
          IDHEP(I)=lutran(K(I,2),1)
          JMOHEP(1,I)=K(I,3)
          JMOHEP(2,I)=0
          if(K(I,1).NE.3.AND.K(I,1).NE.13.AND.K(I,1).NE.14) then
            JDAHEP(1,I)=K(I,4)
            JDAHEP(2,I)=K(I,5)
          else
            JDAHEP(1,I)=0
            JDAHEP(2,I)=0
          endif
          do 100 J=1,5
            PHEP(J,I)=P(I,J)
 100      CONTINUE
          do 110 J=1,4
            VHEP(J,I)=V(I,J)
 110      CONTINUE
          
C...Check if new event (from pileup).
          IF(I.EQ.1) THEN
            INEW=1
          ELSE
            IF(K(I,1).EQ.21.AND.K(I-1,1).NE.21) INEW=I
          ENDIF
 
C...Fill in missing mother information.
          IF(I.GE.INEW+2.AND.K(I,1).EQ.21.AND.K(I,3).EQ.0) THEN
            IMO1=I-2
  120       IF(IMO1.GT.INEW.AND.K(IMO1+1,1).EQ.21.AND.K(IMO1+1,3).EQ.0)
     &      THEN
              IMO1=IMO1-1
              GOTO 120
            ENDIF
            JMOHEP(1,I)=IMO1
            JMOHEP(2,I)=IMO1+1
          elseif(K(I,2).GE.91.AND.K(I,2).LE.93) then
            I1=K(I,3)-1
 130        I1=I1+1
            if(I1.GE.I) call pyerrm(8,
     &        '(LUNHEP:) translation of inconsistent event history')
            if(I1.LT.I.AND.K(I1,1).NE.1.AND.K(I1,1).NE.11) GOTO 130
            KC=pycomp(K(I1,2))
            if(I1.LT.I.AND.KC.EQ.0) GOTO 130
            if(I1.LT.I.AND.KCHG(KC,2).EQ.0) GOTO 130
            JMOHEP(2,I)=I1
          elseif(K(I,2).EQ.94) then
            NJET=2
            if(NHEP.GE.I+3.AND.K(I+3,3).LE.I) NJET=3
            if(NHEP.GE.I+4.AND.K(I+4,3).LE.I) NJET=4
            JMOHEP(2,I)=MOD(K(I+NJET,4)/MSTU(5),MSTU(5))
            if(JMOHEP(2,I).EQ.JMOHEP(1,I)) JMOHEP(2,I)=
     &        MOD(K(I+1,4)/MSTU(5),MSTU(5))
          endif
          
C...Fill in missing daughter information.
          if(K(I,2).EQ.94.AND.MSTU(16).NE.2) then
            do 140 I1=JDAHEP(1,I),JDAHEP(2,I)
              I2=MOD(K(I1,4)/MSTU(5),MSTU(5))
              JDAHEP(1,I2)=I
 140        CONTINUE
          endif
          if(K(I,2).GE.91.AND.K(I,2).LE.94) GOTO 150
          I1=JMOHEP(1,I)
          if(I1.LE.0.OR.I1.GT.NHEP) GOTO 150
          if(K(I1,1).NE.13.AND.K(I1,1).NE.14) GOTO 150
          if(JDAHEP(1,I1).EQ.0) then
            JDAHEP(1,I1)=I
          else
            JDAHEP(2,I1)=I
          endif
 150    CONTINUE
        do 160 I=1,NHEP
          if(K(I,1).NE.13.AND.K(I,1).NE.14) GOTO 160
          if(JDAHEP(2,I).EQ.0) JDAHEP(2,I)=JDAHEP(1,I)
 160    CONTINUE
        
C...Conversion from standard to JETSET, the easy part.
      else
        if(NHEP.GT.MSTU(4)) call pyerrm(8,
     &  '(LUNHEP:) insufficient space in /LUJETS/')
        N=MIN(NHEP,MSTU(4))
        NKQ=0
        KQSUM=0
        do 190 I=1,N
          K(I,1)=0
          if(ISTHEP(I).EQ.1) K(I,1)=1
          if(ISTHEP(I).EQ.2) K(I,1)=11
          if(ISTHEP(I).EQ.3) K(I,1)=21
CC .. Convert from PDG to PYTHIA code          
          K(I,2)=lutran(IDHEP(I),2)
          K(I,3)=JMOHEP(1,I)
          K(I,4)=JDAHEP(1,I)
          K(I,5)=JDAHEP(2,I)
          do 170 J=1,5
            P(I,J)=PHEP(J,I)
 170      CONTINUE
          do 180 J=1,4
            V(I,J)=VHEP(J,I)
 180      CONTINUE
          V(I,5)=0D0
          if(ISTHEP(I).EQ.2.AND.PHEP(4,I).GT.PHEP(5,I)) then
            I1=JDAHEP(1,I)
            if(I1.GT.0.AND.I1.LE.NHEP) V(I,5)=(VHEP(4,I1)-VHEP(4,I))*
     &        PHEP(5,I)/PHEP(4,I)
          endif
          
C...Fill in missing information on colour connection in jet systems.
          if(ISTHEP(I).EQ.1) then
            KC=pycomp(K(I,2))
            KQ=0
            if(KC.NE.0) KQ=KCHG(KC,2)*isign(1,K(I,2))
            if(KQ.NE.0) NKQ=NKQ+1
            if(KQ.NE.2) KQSUM=KQSUM+KQ
            if(KQ.NE.0.AND.KQSUM.NE.0) then
              K(I,1)=2
            elseif(KQ.EQ.2.AND.I.LT.N) then
              if(K(I+1,2).EQ.21) K(I,1)=2
            endif
          endif
 190    CONTINUE
        if(NKQ.EQ.1.OR.KQSUM.NE.0) call pyerrm(8,
     &    '(LUNHEP:) input parton configuration not colour singlet')
      endif
      lunhep = COUNTER
      return
      end

