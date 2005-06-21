C $Id: containsb.F,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
C
C Function which returns 1 if the Pythia Event contains
C a b (or b bar) quark or a B hadron
C
      INTEGER FUNCTION CONTAINSB( ) 
*
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
*
* Pythia Common blocks
*
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*
*
      INTEGER NPART, ITRK, ISTATUS, ID, NQ1, NQ2, NQ3
*
      CONTAINSB = 0
*
      NPART = N
      DO ITRK = 9, NPART 
        ISTATUS = K(ITRK,1) 
        ID     = ABS( K(ITRK,2) )
        NQ1    = MOD( ID/1000 , 10 )
        NQ2    = MOD( ID/100  , 10 )
        NQ3    = MOD( ID/10   , 10 )
        IF ( ( ( ISTATUS.EQ.1 ) .OR. ( ISTATUS.EQ.2 ) )
     >    .AND.
     >    ( ( ID.EQ.5 ) .OR. ( NQ1.EQ.5 ) .OR. ( NQ2.EQ.5 )
     >    .OR. ( NQ3.EQ.5 ) ) ) THEN
          CONTAINSB = 1
          GOTO 20
        ENDIF
      ENDDO
*
 20   RETURN
      END
