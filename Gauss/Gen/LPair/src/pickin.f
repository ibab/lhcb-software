*CMZ :  2.00/00 01/06/92  15.25.17  by  Olaf Duenger
*CMZ :  1.00/00 16/01/92  17.24.06  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.30  by  O. Duenger
*CMZ :  0.00/00 18/12/91  16.56.40  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      SUBROUTINE PICKIN(S,V1,V2,V3,V4,V5,DJ,NOPT,Y)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

*KEEP,CUTS.
      INTEGER      MODCUT
      REAL*4       THMAX,THMIN,MXMN,MXMX,Q2MN,Q2MX
      REAL*8       COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,QP2MIN,QP2MAX
      COMMON /CUTS/COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,
     &             THMAX,THMIN,QP2MIN,QP2MAX,MODCUT,MXMN,MXMX,Q2MN,Q2MX

*KEND.

      COMMON/PICKZZ/ W1,W2,W3,W4,W5,D1,D2,D5,D7,SL1
      COMMON/EXTRA/  S1,S2,T1,T2
      COMMON/LEVI/   GRAM,DD1,DD2,DD3,DD4,DD5,DELTA,G4,SA1,SA2
      COMMON/DOTP/   P12,P13,P14,P15,P23,P24,P25,P34,P35,P45,P1K2,P2K1
      COMMON/ACCURA/ ACC3,ACC4

      DIMENSION Y(4)

      DATA PI/3.14159265358979D+00/

      X1 = Y(1)
      X2 = Y(2)
      X3 = Y(3)
      W1 = V1*V1
      W2 = V2*V2
      W3 = V3*V3
      W4 = V4*V4
      W5 = V5*V5
      SIG= V4+V5
      SIG1=SIG*SIG
      SIG2=SIG1
      D1 = W3-W1
      D2 = W5-W2
      D5 = W1-W2
      D6 = W4-W5
      SS = S+D5
      RL1= SS*SS-4.*W1*S
      IF(RL1.LE.0)GO TO 20
      SL1= DSQRT(RL1)
      IF(NOPT.NE.0)GO TO 1
      SMAX=S+W3-2.*V3*DSQRT(S)
      CALL MAPS2(S2,X3,SIG1,SMAX,DS2)
      SIG1=S2
1     SP  =S+W3-SIG1
      D3  =SIG1-W2
      RL2 =SP*SP-4.*S*W3
      IF(RL2.LE.0)GO TO 20
      SL2 =SQRT(RL2)
      T1MAX=W1+W3-(SS*SP+SL1*SL2)/(2.*S)
      T1MIN=(D1*D3+(D3-D1)*(D3*W1-D1*W2)/S)/T1MAX
      IF (T1MAX.GT.QP2MIN .OR. T1MIN.LT.QP2MAX) GOTO 20
      IF (T1MAX .LT. QP2MAX) T1MAX=QP2MAX
      IF (T1MIN .GT. QP2MIN) T1MIN=QP2MIN
*      write(6,*) ' pickin : t1max =',t1max,'  t1min =',t1min,
*     &  '  qp2max =',qp2max,'  qp2min =',qp2min
      CALL MAPT1(T1,X1,T1MIN,T1MAX,DT1)
      D4  =W4-T1
      D8  =T1-W2
      T13 =T1-W1-W3
C
C     WRITE(6,*) T1,D1,W1
C
      SA1 =-(T1-D1)*(T1-D1)*0.25+W1*T1
      IF(SA1.GE.0)GO TO 20
      SL3 =DSQRT(-SA1)
      IF(W1.EQ.0)GO TO 3
      SB  =(S*(T1-D1)+D5*T13)/(2.*W1)+W3
      SD  =SL1*SL3/W1
C
C     WRITE(6,*) S,T1,T13,W2,D1,W3,D5,D8,W1
C
      SE  =(S*(T1*(S+T13-W2)-W2*D1)+W3*(D5*D8+W2*W3))/W1
      IF(DABS((SB-SD)/SD).LT.1.0)GO TO 2
      SPLUS=SB-SD
      S2MAX=SE/SPLUS
      GO TO 4
2     S2MAX=SB+SD
      SPLUS=SE/S2MAX
      GO TO 4
3     S2MAX=(S*(T1*(S+D8-W3)-W2*W3)+W2*W3*(W2+W3-T1))/SS/T13
      SPLUS=SIG2
4     S2X=S2MAX
      IF(NOPT)5,6,7
5     IF(SPLUS.GT.SIG2)SIG2=SPLUS
      IF(NOPT.LT.-1)CALL MAPS2(S2,X3,SIG2,S2MAX,DS2)
      IF(NOPT.EQ.-1)CALL MAPLA(S2,T1,W2,X3,SIG2,S2MAX,DS2)
6     S2X=S2
7     R1=S2X-D8
      R2=S2X-D6
      RL4=(R1*R1-4.*W2*S2X)*(R2*R2-4.*W5*S2X)
      IF(RL4.LE.0)GO TO 20
      SL4=DSQRT(RL4)
      T2MAX=W2+W5-(R1*R2+SL4)/(2.*S2X)
      T2MIN=(D2*D4+(D4-D2)*(D4*W2-D2*T1)/S2X)/T2MAX
      CALL MAPT2(T2,X2,T2MIN,T2MAX,DT2)
      D7=T1-T2
      R3=D4-T2
      R4=D2-T2
      B=R3*R4-2.*(T1+W2)*T2
      C=T2*D6*D8+(D6-D8)*(D6*W2-D8*W5)
      T25=T2-W2-W5
C
C     WRITE(6,*) R4,W2,T2
C
      SA2=-R4*R4*0.25+W2*T2
      IF(SA2.GE.0)GO TO 20
      SL6=2.*DSQRT(-SA2)
      G4=-0.25*R3*R3+T1*T2
      IF(G4.GE.0)GO TO 20
      SL7=SQRT(-G4)*2.
      SL5=SL6*SL7
      IF(DABS((SL5-B)/SL5).LT.1.0)GO TO 8
      S2P=(SL5-B)/(2.*T2)
      S2MIN=C/(T2*S2P)
      GO TO 9
8     S2MIN=(-SL5-B)/(2.*T2)
      S2P=C/(T2*S2MIN)
9     IF(NOPT.GT.1)CALL MAPS2(S2,X3,S2MIN,S2MAX,DS2)
      IF(NOPT.EQ.1)CALL MAPLA(S2,T1,W2,X3,S2MIN,S2MAX,DS2)
      AP=-(S2+D8)*(S2+D8)*0.25+S2*T1
      IF(W1.EQ.0)GO TO 10
      DD1=-W1*(S2-S2MAX)*(S2-SPLUS)*0.25
      GO TO 11
10    DD1=SS*T13*(S2-S2MAX)*0.25
11    DD2=-T2*(S2-S2P)*(S2-S2MIN)*0.25
      if(dd1.ge.0) then
      elseif(dd1.lt.0) then
      else ! NaN
         print *,'pickin: dd1',dd1,' SS,T13,S2,S2MAX',SS,T13,S2,S2MAX
         print *,'X1-3',X1,X2,X3
      endif
      if(dd2.ge.0) then
      elseif(dd2.lt.0) then
      else ! NaN
         print *,'pickin: dd2',dd2,t2,s2,s2p,s2,s2min
         print *,'y',y
         print *,'t2m',t2min,t2max
      endif
      YY4=DCOS(PI*Y(4))
      DD=DD1*DD2
      P12=0.5*(S-W1-W2)
      ST=S2-T1-W2
      DELB=(2.*W2*R3+R4*ST)*(4.*P12*T1-(T1-D1)*ST)/(16.*AP)
      IF(DD.LE.0)GO TO 20
      DELTA=DELB-YY4*ST*DSQRT(DD)/(2.*AP)
      S1=T2+W1+(2.*P12*R3-4.*DELTA)/ST
      IF(AP.GE.0)GO TO 20
      DJ=DS2*DT1*DT2*PI*PI/(8.*SL1*DSQRT(-AP))
      GRAM=(1.-YY4)*(1.+YY4)*DD/AP
      P13=-T13*0.5
      P14=(D7+S1-W3)*0.5
      P15=(S+T2-S1-W2)*0.5
      P23=(S+T1-S2-W1)*0.5
      P24=(S2-D7-W5)*0.5
      P25=-T25*0.5
      P34=(S1-W3-W4)*0.5
      P35=(S+W4-S1-S2)*0.5
      P45=(S2-W4-W5)*0.5
      P1K2=(S1-T2-W1)*0.5
      P2K1=ST*0.5
      IF(W2.EQ.0)GO TO 14
      SBB=(S*(T2-D2)-D5*T25)/(2.*W2)+W5
      SDD=SL1*SL6/(2.*W2)
      SEE=(S*(T2*(S+T25-W1)-W1*D2)+W5*(W1*W5-D5*(T2-W1)))/W2
      IF(SBB/SDD.LT.0)GO TO 12
      S1P=SBB+SDD
      S1M=SEE/S1P
      GO TO 13
12    S1M=SBB-SDD
      S1P=SEE/S1M
13    DD3=-W2*(S1P-S1)*(S1M-S1)*0.25
      if(dd3.ge.0) then
      elseif(dd3.lt.0) then
      else ! NaN
         print *,'pickin: dd3',dd3,w2,s1,s1p,s1m,s1
         print *,'y',y
      endif
      GO TO 15
14    S1P=(S*(T2*(S-W5+T2-W1)-W1*W5)+W1*W5*(W1+W5-T2))/T25/(S-D5)
      DD3=-T25*(S-D5)*(S1P-S1)*0.25
15    ACC3=(S1P-S1)/(S1P+S1)
      SSB=T2+W1-R3*(D1-T1)*0.5/T1
      SSD=SL3*SL7/T1
      SSE=(T2-W1)*(W4-W3)+(T2-W4+D1)*((T2-W1)*W3-(W4-W3)*W1)/T1
      IF(SSB/SSD.LT.0)GO TO 16
      S1PP=SSB+SSD
      S1PM=SSE/S1PP
      GO TO 17
16    S1PM=SSB-SSD
      S1PP=SSE/S1PM
17    DD4=-T1*(S1-S1PP)*(S1-S1PM)*0.25
      ACC4=(S1-S1PM)/(S1+S1PM)
      DD5=DD1+DD3+((P12*(T1-D1)*0.5-W1*P2K1)*(P2K1*(T2-D2)-W2*R3)
     &    -DELTA*(2.*P12*P2K1-W2*(T1-D1)))/P2K1

      RETURN

20    DJ=0.

      RETURN
      END
