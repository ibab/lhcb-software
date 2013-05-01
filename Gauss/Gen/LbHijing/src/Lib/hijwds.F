C*************************
C
C
C
C
C ********************************************************
C ************************              WOOD-SAX
        SUBROUTINE HIJWDS(IA,IDH,XHIGH)
        IMPLICIT REAL*8(A-H,O-Z)
C     SETS UP HISTOGRAM IDH WITH RADII FOR
C     NUCLEUS IA DISTRIBUTED ACCORDING TO THREE PARAM WOOD SAXON
	COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
	SAVE  /HIPARNT/
        COMMON/WOOD/R,D,FNORM,W
        SAVE  /WOOD/
        DIMENSION IAA(20),RR(20),DD(20),WW(20),RMS(20)
        EXTERNAL RWDSAX,WDSAX
C
C   PARAMETERS OF SPECIAL NUCLEI FROM ATOMIC DATA AND NUC DATA TABLES
C     VOL 14, 5-6 1974
        DATA IAA/2,4,12,16,27,32,40,56,63,93,184,197,208,7*0/
        DATA RR/0.01d0,.964d0,2.355d0,2.608d0,2.84d0,3.458d0,3.766d0,
     1        3.971d0,4.214d0,
     1        4.87d0,6.51d0,6.38d0,6.624d0,7*0.d0/
        DATA DD/0.5882d0,.322d0,.522d0,.513d0,.569d0,.61d0,.586d0,
     1        .5935d0,.586d0,.573d0,
     1        .535d0,.535d0,.549d0,7*0.d0/
        DATA WW/0.0d0,.517d0,-0.149d0,-0.051d0,0.d0,-0.208d0,-0.161d0,
     1        13*0.d0/
        DATA RMS/2.11d0,1.71d0,2.46d0,2.73d0,3.05d0,3.247d0,3.482d0,
     1        3.737d0,3.925d0,4.31d0,
     1        5.42d0,5.33d0,5.521d0,7*0.d0/
C
      	A=IA
C
C 		********SET WOOD-SAX PARAMS FIRST  AS IN DATE ET AL
      	D=0.54d0
C			********D IS WOOD SAX DIFFUSE PARAM IN FM
	R=1.19d0*A**(1.d0/3.d0) - 1.61d0*A**(-1.d0/3.d0)
C 			********R IS RADIUS PARAM
	W=0.d0
C 		********W IS The third of three WOOD-SAX PARAM
C
C      		********CHECK TABLE FOR SPECIAL CASES
	DO 10 I=1,13
		IF (IA.EQ.IAA(I)) THEN
			R=RR(I)
     			D=DD(I)
      			W=WW(I)
      			RS=RMS(I)
      		END IF
10    	CONTINUE
C     			********FNORM is the normalize factor
      	FNORM=1.0d0
      	XLOW=0.d0
      	XHIGH=R+ 12.d0*D
      	IF (W.LT.-0.01d0)  THEN
      		IF (XHIGH.GT.R/SQRT(ABS(W))) XHIGH=R/SQRT(ABS(W))
      	END IF
      	FGAUS=GAUSS1(RWDSAX,XLOW,XHIGH,0.001d0)
      	FNORM=1.d0/FGAUS
C
        IF (IDH.EQ.1) THEN
           HINT1(72)=R
           HINT1(73)=D
           HINT1(74)=W
           HINT1(75)=FNORM/4.0d0/HIPR1(40)
        ELSE IF (IDH.EQ.2) THEN
           HINT1(76)=R
           HINT1(77)=D
           HINT1(78)=W
           HINT1(79)=FNORM/4.0d0/HIPR1(40)
        ENDIF
C
C     	NOW SET UP HBOOK FUNCTIONS IDH FOR  R**2*RHO(R)
C     	THESE HISTOGRAMS ARE USED TO GENERATE RANDOM RADII
      	CALL HIFUN(IDH,XLOW,XHIGH,RWDSAX)
      	RETURN
      	END
