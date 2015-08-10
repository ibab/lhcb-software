*CMZ :  2.00/00 03/06/92  13.04.23  by  Unknown
*CMZ :  1.00/00 16/01/92  17.24.06  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.30  by  O. Duenger
*CMZ :  0.00/00 18/12/91  16.56.41  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      SUBROUTINE ORIENT(S,V1,V2,V3,V4,V5,DJ,NOPT,Y)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      COMMON/VARIAB/ E,E1,E2,E3,E4,E5,P,P3,P4,P5,CT3,ST3,CT4,ST4,CT5,
     &               ST5,CP3,SP3,CP5,SP5
      COMMON/VARIAC/ AL3,AL4,BE4,BE5,DE3,DE5,PP3,PP4,PP5
      COMMON/PICKZZ/ W1,W2,W3,W4,W5,W31,W52,W12,TAU,SL1
      COMMON/EXTRA/  S1,S2,T1,T2
      COMMON/LEVI/   GRAM,DD1,DD2,DD3,DD4,DD5,DELTA,G4,SA1,SA2
      COMMON/DOTP/   P12,P13,P14,P15,P23,P24,P25,P34,P35,P45,Q1,Q2

      REAL*8       ME,MU,MP
      COMMON /INPU/ME,MU,MP

      DIMENSION Y(4)

      CALL PICKIN(S,V1,V2,V3,V4,V5,DJ,NOPT,Y)
      IF(DJ.EQ.0)GO TO 10
      E   = DSQRT(S)
      RE  = 0.5/E
      E1  = RE*(S+W12)
      E2  = RE*(S-W12)
      P   = RE*SL1
      DE3 = RE*(S2-W3+W12)
      DE5 = RE*(S1-W5-W12)
      E3  = E1-DE3
      E4  = DE3+DE5
      E5  = E2-DE5
      IF(E4.LT.V4)GO TO 10
      P3  = DSQRT(E3*E3-W3)
      P4  = DSQRT((E4-V4)*(E4+V4))
      IF(P4.EQ.0)GO TO 10
      P5  = DSQRT(E5*E5-W5)

      if(p5.ge.0) then
      elseif(p5.ge.0) then
      else
         print *,'ORIENT p5 ',p5,' e5,w5,M^2',e5,w5,MP**2
      endif
C
C     WRITE(6,*) DD1,S,P
C
      PP3 = DSQRT(DD1/S)/P
      PP5 = DSQRT(DD3/S)/P
      ST3 = PP3/P3
      ST5 = PP5/P5
      if(dd3.ge.0) then
      elseif(dd3.lt.0) then
      else  ! NaN
         print *,'dd3',dd3
      endif
      if(dd1.ge.0) then
      elseif(dd1.lt.0) then
      else  ! NaN
         print *,'dd1',dd1
      endif
      IF(ST3.GT.1..OR.ST5.GT.1.)GO TO 10
      CT3 = DSQRT(1.-ST3*ST3)
      CT5 = DSQRT(1.-ST5*ST5)
      IF(E1*E3.LT.P13)CT3=-CT3
      IF(E2*E5.GT.P25)CT5=-CT5
      AL3 = ST3*ST3/(1.+CT3)
      BE5 = ST5*ST5/(1.-CT5)
      IF(DD5.LT.0)GO TO 10
      PP4 = DSQRT(DD5/S)/P
      ST4 = PP4/P4
      IF(ST4.GT.1.)GO TO 10
      CT4 = DSQRT(1.-ST4*ST4)
      IF(E1*E4.LT.P14)CT4 =-CT4
      AL4 = 1.-CT4
      BE4 = 1.+CT4
      IF(CT4.LT.0)BE4 = ST4*ST4/AL4
      IF(CT4.GE.0)AL4 = ST4*ST4/BE4
      RR  = DSQRT(-GRAM/S)/(P*PP4)
      SP3 = RR/PP3
      SP5 =-RR/PP5
      IF(DABS(SP3).GT.1..OR.DABS(SP5).GT.1.)GO TO 10
      CP3 =-DSQRT(1.-SP3*SP3)
      CP5 =-DSQRT(1.-SP5*SP5)
      A1  = PP3*CP3-PP5*CP5
      IF(DABS(PP4+PP3*CP3+CP5*PP5).LT.DABS(DABS(A1)-PP4))GO TO 1
      IF(A1.LT.0)CP5 =-CP5
      IF(A1.GE.0)CP3 =-CP3

1     RETURN

10    DJ  = 0.

      RETURN
      END
C   13/02/92 202131749  MEMBER NAME  EDITFILE (H1MUP)    M


