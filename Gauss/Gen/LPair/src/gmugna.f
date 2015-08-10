*CMZ :  1.00/00 18/08/94  22.09.34  by  ZEUS Offline Group
*-- Author :    ZEUS Offline Group   18/08/94
      SUBROUTINE GMUGNA
C
C  MODIFICATION FROM VERMASERENS GENERA
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
       EXTERNAL F
*KEEP,VEGPAR.
      INTEGER          NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND
      COMMON /VEGPAR/  NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND
      save /vegpar/

*KEEP,COMGNA.
      INTEGER NGNA
      COMMON /COMMUP/  NGNA
      save /commup/
*KEND.
C
      COMMON/VGMAXI/MDUM,MBIN,FFMAX,FMAX(20000),NM(20000)
      save /vgmaxi/
      DIMENSION X(10),N(10)
      save x,n
C
      SAVE WEIGHT,CORREC,J
C
      save corre2,fmdiff,fmold,fmax2
      data j/0/
c
c
      NGNA=NGNA+1
C
       AMI=1.0D0/DBLE(MBIN)
       MAX=MBIN**NDIM
C
C CORRECTION CYCLES ARE STARTED <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
       IF (J .NE. 0) THEN
4         CONTINUE
!          print *,'gmugna: correction',j,correc,corre2
          IF (CORREC .LT. 1.0)THEN
             IF(VGRAN(0.0) .GE. CORREC) GOTO 7
             CORREC=-1.0
          ELSE
             CORREC=CORREC-1.0
          ENDIF
C
C SEL X VALUES IN VEGAS BIN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          DO 6 K=1,NDIM
             X(K)=(VGRAN(0.0)+N(K))*AMI
!             if(x(k).lt.0.or.x(k).gt.1) then
!                print *,'correction, X',x(k),k
!             endif
6         CONTINUE
C
C COMPUTE WEIGHT FOR X VALUES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          IF (NTREAT .GT. 0) WEIGHT=TREAT(F,X,NDIM)
          IF (NTREAT .LE. 0) WEIGHT=F(X)
C
C PARAMETER FOR CORRECTION OF CORRECTION <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          IF(WEIGHT .GT. FMAX(J)) THEN
             IF (WEIGHT .GT. FMAX2) FMAX2=WEIGHT
             CORRE2=CORRE2-1.0
             CORREC=CORREC+1.0
          ENDIF
C
C ACCEPT EVENT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          IF (WEIGHT .GE. FMDIFF*VGRAN(0.0)+FMOLD) THEN
!             print *,'gmugna: done',weight,' x',(x(ipr),ipr=1,NDIM)
             RETURN
          ENDIF
          GOTO 4
C
C CORRECTION IF TOO BIG WEIGHT IS FOUND WHILE CORRECTION <<<<<<<<<<<<
7         CONTINUE
          IF (FMAX2 .GT. FMAX(J)) THEN
             FMOLD=FMAX(J)
             FMAX(J)=FMAX2
             FMDIFF=FMAX2-FMOLD
!             print *,'using CORRE2',corre2
             IF(FMAX2 .LE. FFMAX) THEN
                CORREC=(NM(J)-1.0)*FMDIFF/FFMAX-CORRE2
             ELSE
                FFMAX=FMAX2
                CORREC=(NM(J)-1.0)*FMDIFF/FFMAX*FMAX2/FFMAX-CORRE2
             ENDIF
             CORRE2=0.0
             FMAX2=0.0
             GOTO 4
          ENDIF
       ENDIF
C
C  NORMAL GENERATION CYCLE STARTS HERE !!!!!!!       *******************
C
!          print *,'gmugna: generation'

C  SEL A VEGAS BIN AND REJECT IF FMAX IS TOO LITTLE <<<<<<<<<<<<<<<<<<<<

1         CONTINUE
            J=VGRAN(0.0)*MAX+1.
            Y=VGRAN(0.0)*FFMAX
            NM(J)=NM(J)+1
          IF(Y.GT.FMAX(J)) GOTO 1
!          print *,'try OK'

C
C  SEL X VALUES IN THIS VEGAS BIN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          JJ=J-1
          DO 2 K=1,NDIM
             JJJ=JJ/MBIN
             N(K)=JJ-JJJ*MBIN
             X(K)=(VGRAN(0.0)+N(K))*AMI
!             if(x(k).lt.0.or.x(k).gt.1) then
!                print *,'correction, I,X',k,x(k)
!             endif
             JJ=JJJ
2         CONTINUE
C
C  GET WEIGHT FOR SEL X VALUES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          IF(NTREAT.GT.0) THEN
             WEIGHT=TREAT(F,X,NDIM)
          ELSE
             WEIGHT=F(X)
          ENDIF

C  REJECT IF WEIGHT IS TOO LOW <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          IF(Y .GT. WEIGHT) then
!             print *,'Rejected Y>WEIGHT',Y,WEIGHT
             GO TO 1
          endif
          IF(WEIGHT .LE. FMAX(J)) THEN
           J=0
C
C  INIT CORRECTION CYCLES IF WEIGHT IS HIGHER THEN FMAX OR FFMAX <<<<<<
          ELSE IF(WEIGHT .LE. FFMAX) THEN
           FMOLD=FMAX(J)
           FMAX(J)=WEIGHT
           FMDIFF=WEIGHT-FMOLD
           CORREC=(NM(J)-1.0)*FMDIFF/FFMAX-1.0
!           print *,'need correction of FFMAX',weight,ffmax,correc
          ELSE
           FMOLD=FMAX(J)
           FMAX(J)=WEIGHT
           FMDIFF=WEIGHT-FMOLD
           FFMAX=WEIGHT
           CORREC=(NM(J)-1.0)*FMDIFF/FFMAX*WEIGHT/FFMAX-1.0
!           print *,'need correction of FMAX',j,weight,ffmax,correc
          ENDIF
C
C RETURN WITH AN ACCEPTED EVENT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!          print *,'gmugna: done',weight,' x',(x(ipr),ipr=1,NDIM)

        RETURN
       END
