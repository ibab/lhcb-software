C********************************************************************** 
C    Modified by S.Alioli and E.Re, 27-11-2007: now is y2k compliant!
C    Modified by P. Nason, 28-6-91
C    SIMPLE HISTOGRAMMING PACKAGE --  SIMPLIFIED VERSION OF HBOOK
C    BY Michelangelo Mangano    NOVEMBER 1988
C    LAST REVISED NOVEMBER 9, 1988
C    LAST REVISED JUNE 12, 1989  (ADD SCATTER PLOTS)
C    LAST REVISED oct 1990 (Add multi-plots on one page, routines MULTITOP,
C         			wbb_PWHGTFILL,...)
C**********************************************************************
C
C Fills up to nmh histograms with up to nxm bins. 
C Gives a data file (to be specified in the calling program by assigning 
C a file name to unit 98) and a topdrawer file (to be specified in the 
C calling program by assigning a file name to unit 99).
C
C INITIALIZATION:
C Call once INIHIST; this just resets a few counters and logicals
C Call wbb_PWHGBOOK(N,'TITLE',DEL,XMIN,XMAX) for each histogram to be booked.
C N (an integer) is the label of the histogram;
C 'TITLE' is the name of the histogram (no more then 100 characters);
C DEL (real*4) is the bin size;
C XMIN (real*4) is the lower limit of the first bin;
C XMAX (real*4)is the upper limit of the last  bin
C Example:
C      call wbb_PWHGBOOK(2,'pt distribution',1.,10,70)
C This call initializes histogram number 2, called 'pt distribution';
C The bin size will be 1. (possibly GeV, if that's what you want), the
C first bin being  10<x<11. and the last one being 69.<x<70
C
C FILLING:
C When it's time, call wbb_PWHGFILL(N,X,Y); this will add Y (real*4) to the bin 
C in which X (real*4) happens to be, within histogram N. 
C
C PLAYING AROUND:
C At the end of the day you may want to sum, divide, cancel, etc.etc.
C various histograms (bin by bin). Then you call wbb_PWHGOPERA(I,'O',J,K,X,Y). 
C The 1-character string O can take the following values:
C +  : sums       X*(hist I) with Y*(hist J) and puts the result in hist K;
C -  : subtracts  X*(hist I) with Y*(hist J) and puts the result in hist K;
C *  : multiplies X*(hist I) with Y*(hist J) and puts the result in hist K;
C /  : divides    X*(hist I) with Y*(hist J) and puts the result in hist K;
C F  : multiplies hist I by the factor X, and puts the result in hist K;
C R  : takes the square root of  hist  I, and puts the result in hist K;if
C      the value at a given bin is less than or equal to 0, puts 0 in K
C S  : takes the square      of  hist  I, and puts the result in hist K;
C L  : takes the log_10 of  hist  I, and puts the result in hist K; if the
C      value at a given bin is less than or equal to 0, puts 0 in K
C M  : statistical wbb_analysis; if I contains the weights (let's say WGT),
C      J contains variable times weight (F*WGT) and K contains the
C      variable squared times the weight (F**2*WGT), then, after using 'M',
C      J will contain the average value of the variable <F> and K will 
C      contain the sigma of the average: sigma=sqrt(<F**2>-<F>**2).
C      If WGT=1. for all the entries, then it is enough to put I=J, and
C      it is not necessary to book a hist with the weights.
C V  : estimates errors for vegas evaluation of differential distributions.
C      Fill I with the values of
C      the functions do integrate times the Vegas weight (fun*wgt); fill
C      J with fun**2*wgt; then K will contain an estimate of the error
C      of the integration. Putting X=1/(#of iterations) performs the 
C      avegare over the iterations, and gives the right normalization to 
C      the differential distribution, I, and to the errors, K. J stays the same.
C
C FINAL ACCOUNTING:
C Now we can finalize our histograms; wbb_PWHGFINAL(N) will calculate the integral
C of the histogram N, the mean value of the X variable and its RMS.
C If we now want to renormalize the hist's, we can call wbb_PWHGNORM(N,X), which
C will normalize the integral to X  -- CAUTION: do not call wbb_PWHGNORM before
C wbb_PWHGFINAL, it will blow up.
C
C OUTPUT:
C To get a .dat file containing the values of the histograms, together with
C some information (like integral, mean values, etc.etc.) call wbb_PWHGPRINT(N),
C for each hist N that you want in the .dat file. Before the call to wbb_PWHGPRINT
C you want to open unit 98 and give it a name:
C     OPEN(UNIT=98,NAME='NAME.DAT',STATUS='NEW')
C If you want a topdrawer file with a plot of the hist values, call 
C wbb_PWHGTOP(N,M,'X','Y','wbb_scale'). The points of the plot will be taken from histogram
C N, the error bars from histogram M. 'wbb_scale', character*(*), determines
C the wbb_scale for y, logarithmic or linear (wbb_scale=LOG,LIN).
C If you do not want error bars, keep
C a histogram of zeros, or just call a hist that had not been booked.
C X will appear as a 'bottom title', and Y will appear as a 'left title'.
C The top title is by default the name of the histogram itself.
C A little box below the plot will contain some information on the plot
C itself. Before calling wbb_PWHGTOP,
C     OPEN(UNIT=99,NAME='NAME.TOP',STATUS='NEW')
C--------------------------------------------------------------------------
C
C  COMMON/wbb_histo/  Histogram N
C                           
C   BOOK(N),      Three-letter character-string: 'NO' if histogram was not 
C		  Booked, 'YES' otherwise
C   TITLE(N),     Title of the histogram
C
C   HMIN(N),      Min value of x range
C   HMAX(N),      Max value of x range
C   HDEL(N),      Bin width
C   NBIN(N),      Total number of bins
C   USCORE(N),    Total integral of underscores with x < HMIN(N)
C   OSCORE(N),    Total integral of onderscores with x > HMAX(N)
C   IUSCORE(N),   Number of entries with x < HMIN(N)
C   IOSCORE(N),   Number of entries with x > HMAX(N)
C   IENT(N),      Total number of entries within x range HMIN(N)<x<HMAX(N)
C   HINT(N),      Integral of the histogram within HMIN(N)<x<HMAX(N)
C   HAVG(N),      Average value of x, weighted over the x range of the wbb_histo
C   HSIG(N),      Quadratic dispersion of x around the average
C   HIST(N,L),    Value of bin L-th
C   XHIS(N,L),    Central x value of bin L-th
C   IHIS(N,L),    Number of entries within bin L-th
C   NHIST         Total number of booked histograms
C


      SUBROUTINE wbb_pwhgINIHIST
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer i
      DO 1, I=1,NMB             
   1  BOOK(I)=' NO'
      DO 2 I=1,10
   2  BOOK2(I)=' NO'
      call wbb_pwhgresetmultitop
      END  


      subroutine wbb_pwhggettitle(N,TIT)
      implicit none
      character *(*) TIT
      integer n
      include 'src/Lib/wbb/pwhg_book.h'
      TIT=TITLE(N)
      end
      


      SUBROUTINE wbb_PWHGBOOK(N,TIT,DEL,XMIN,XMAX)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer n,i
      real * 8 del,xmin,xmax,onep
      CHARACTER*(*) TIT
      DATA ONEP/1.00001d0/
      IF(N.GT.NMB) THEN
	 CALL wbb_pwhgwarn('wbb_PWHGBOOK')
         WRITE(*,*) 'wbb_PWHGBOOK: no more then ',NMB,' histograms'
         STOP
      ENDIF
      NHIST = MAX(N,NHIST)
      IF(BOOK(N).NE.' NO') THEN
	 CALL wbb_pwhgwarn('wbb_PWHGBOOK')
         WRITE(*,*) 'Histogram',N,TITLE(N),' already in use. '
         WRITE(*,*) 'superseded by ',TIT
      ENDIF
      BOOK(N) = 'YES'
      TITLE(N) = ' '//TIT
1     HDEL(N) = DEL
      NBIN(N) = INT(ONEP*(XMAX-XMIN)/DEL)
      IF(NBIN(N).GT.nxm) THEN
	WRITE(*,*) 'TOO MANY BINS (',NBIN(N),') REQUIRED IN HIST ',N
	WRITE(*,*) 'RE-ENTER BIN SIZE DELTA (OLD BIN = ',DEL,' ):'
	READ(*,*) DEL
	GO TO 1
      ENDIF
      HMIN(N) = XMIN
      HMAX(N) = NBIN(N)*DEL+XMIN
      IF(ABS(HMAX(N)-XMAX)/(XMAX-XMIN).GT.0.1E-5) THEN
	 CALL wbb_pwhgwarn('wbb_PWHGBOOK')
         WRITE(*,*)
     #'Histogram ', TIT, ' Change of upper limit:',xmax,'-->',HMAX(N)
      ENDIF
      IENT(N) = 0
      IUSCORE(N) = 0
      IOSCORE(N) = 0
      USCORE(N) = 0
      OSCORE(N) = 0
      HAVG(N) = 0
      HINT(N) = 0
      HSIG(N) = 0
      DO I=1,NBIN(N)
         XHIS(N,I)=HMIN(N)+HDEL(N)*(FLOAT(I)-0.5D0)
         IHIS(N,I)=0
         HIST(N,I)=0
      ENDDO
      END

      SUBROUTINE wbb_PWHGFILL(N,X,Y)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      real * 8 x,y,xi
      integer n,i
C
      XI=((X-HMIN(N))/HDEL(N))
      I=INT(XI)+1
C
      IF(X.LT.HMIN(N)) I=0
      IF(X.GT.HMAX(N)) I=NBIN(N)+1
C
      IF(I.LT.1) THEN
         USCORE(N) = USCORE(N) + Y
         IUSCORE(N) = IUSCORE(N) + 1
      ELSEIF(I.GT.NBIN(N)) THEN
         OSCORE(N) = OSCORE(N) + Y
         IOSCORE(N) = IOSCORE(N) + 1
      ELSE
         IENT(N)=IENT(N)+1
         IHIS(N,I)=IHIS(N,I)+1
         HIST(N,I)=HIST(N,I)+Y
      ENDIF
      END


      SUBROUTINE wbb_PWHGINTEG(NIN,NOUT,IDIR,IPOW)
C If IPOW=1 performs the integral of the distribution contained in histogram
C NIN up to the value specified by the abscissa (if IDIR=1) or from this
C value on (if IDIR=-1). The resulting integral distribution is put into 
C NOUT, which is automatically booked if NOUT.ne.NIN .  Choosing IPOW=2
C the routine will return the square root of the integral of the squares,
C as is required, for example, for the propagation of the mean quadratic error
C of a given distribution. Overscores and underscores are included.
      implicit none
      integer nin,nout,idir,ipow,m,i,l
      real * 8 wbb_PWHGSUMPOW
      include 'src/Lib/wbb/pwhg_book.h'
      CHARACTER*14  C
      DIMENSION C(2) 
      DATA C/' INTEG BELOW X',' INTEG ABOVE X'/
      M = NBIN(NIN)                                           
      I = (IDIR + 3)/2
      IF(NOUT.NE.NIN) THEN
      	CALL wbb_PWHGBOOK(NOUT,TITLE(NIN)//C(I), 
     &                HDEL(NIN),HMIN(NIN),HMAX(NIN))
      ENDIF
      IF(IDIR.EQ.1) THEN
         HIST(NOUT,1) = wbb_PWHGSUMPOW(HIST(NIN,1),USCORE(NIN),IPOW)
         IHIS(NOUT,1) = IHIS(NIN,1) + IUSCORE(NIN)
         XHIS(NOUT,1) = XHIS(NIN,1) + HDEL(NIN)/2
         DO L=2,M                      
            HIST(NOUT,L) = wbb_PWHGSUMPOW(HIST(NIN,L),HIST(NOUT,L-1),IPOW)
            IHIS(NOUT,L) = IHIS(NIN,L) + IHIS(NOUT,L-1) 
            XHIS(NOUT,L) = XHIS(NIN,L) + HDEL(NIN)/2
         ENDDO
         OSCORE(NOUT) = wbb_PWHGSUMPOW(OSCORE(NIN),HIST(NIN,M),IPOW)
         IOSCORE(NOUT) = IOSCORE(NIN) + IHIS(NIN,M)
      ELSEIF(IDIR.EQ.-1) THEN
         HIST(NOUT,M) = wbb_PWHGSUMPOW(HIST(NIN,M),OSCORE(NIN),IPOW)
         IHIS(NOUT,M) = IHIS(NIN,M) + IOSCORE(NIN)
         XHIS(NOUT,M) = XHIS(NIN,M) - HDEL(NIN)/2
         DO L=M-1,1,-1                        
            HIST(NOUT,L) = wbb_PWHGSUMPOW(HIST(NIN,L),HIST(NOUT,L+1),IPOW)
            IHIS(NOUT,L) = IHIS(NIN,L) + IHIS(NOUT,L+1)
            XHIS(NOUT,L) = XHIS(NIN,L) - HDEL(NIN)/2
         ENDDO
         USCORE(NOUT) = wbb_PWHGSUMPOW(USCORE(NIN),HIST(NIN,1),IPOW)
         IUSCORE(NOUT) = IUSCORE(NIN)+IHIS(NIN,1)
      ELSE                                 
         CALL wbb_pwhgwarn('wbb_PWHGINTEG')
         WRITE(*,*) 'Wrong idir in wbb_PWHGINTEG: OPERATION NOT PERFORMED'
         STOP
      ENDIF
      END

      FUNCTION wbb_PWHGSUMPOW(X,Y,IPOW)
      implicit none
      real * 8 x,y,wbb_PWHGSUMPOW
      integer ipow
      IF(IPOW.EQ.1) THEN
         wbb_PWHGSUMPOW = X + Y
      ELSEIF(IPOW.EQ.2) THEN
         wbb_PWHGSUMPOW = SQRT(X**2+Y**2)
      ELSEIF(IPOW.EQ.0) THEN
         CALL wbb_pwhgwarn('wbb_PWHGSUMPOW')
         WRITE(*,*)'Error: IPOW=0 not allowed in wbb_PWHGSUMPOW'
         STOP
      ELSE
         wbb_PWHGSUMPOW = (X**IPOW+Y**IPOW)**(1./IPOW)
      ENDIF
      END

      SUBROUTINE wbb_PWHGOPERA(I,OPER,J,K,X,Y)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer i,j,k,l
      real * 8 x,y,xxx,xsum,xsumsq,xsqavg,xnorm,xavg
      character * 1 oper
      IF(NBIN(I).NE.NBIN(J).AND.(OPER.EQ.'+'.OR.OPER.EQ.'-'.OR.OPER.EQ.
     &    '*'.OR.OPER.EQ.'/'.OR.OPER.EQ.'M'.OR.OPER.EQ.'A')) THEN
	  CALL wbb_pwhgwarn('wbb_PWHGOPERA')
          WRITE(20,*) I,J                               
  20      FORMAT(' ****** INCOMPATIBLE OPERATION HIST ',I2,' &',I2,
     &    '*******'/)
          STOP
      ENDIF
      IF(OPER.EQ.'E') THEN
c If I contains the accumulated weights, J the accumulated squares of the
c weights and IHIS(J,1) the number of accumulated entries, 'E' will add
c the average value of I to K and will put in J the quadratic dispersion.
         IF(IHIS(J,1).NE.0) THEN
            XXX = 1./IHIS(J,1)
         ELSE
            XXX = 0
         ENDIF
         DO L=1,NBIN(I)
            XSUM   = HIST(I,L)
            XSUMSQ = HIST(J,L)
            HIST(K,L)=HIST(K,L) + XXX*XSUM
            IHIS(K,L)=IHIS(K,L) + IHIS(I,L)
            HIST(J,L)=XXX*SQRT(ABS(XSUMSQ-XSUM**2*XXX))
         ENDDO
         IENT(K)=IENT(K)+IENT(I)
         XSUM = USCORE(I)
         XSUMSQ = USCORE(J)
         USCORE(K) = USCORE(K)+XXX*XSUM
         IUSCORE(K) = IUSCORE(K)+IUSCORE(I)
         USCORE(J) = XXX*SQRT(ABS(XSUMSQ-XSUM**2*XXX))
         XSUM = OSCORE(I)
         XSUMSQ = OSCORE(J)
         OSCORE(K) = OSCORE(K)+XXX*XSUM
         IOSCORE(K) = IOSCORE(K)+IOSCORE(I)
         OSCORE(J) = XXX*SQRT(ABS(XSUMSQ-XSUM**2*XXX))
      ELSEIF(OPER.EQ.'Q') THEN
         DO L=1,NBIN(I)
            HIST(K,L) = SQRT(HIST(J,L)**2+HIST(I,L)**2)
         ENDDO
         USCORE(K) = SQRT(USCORE(J)**2+USCORE(I)**2)
         OSCORE(K) = SQRT(OSCORE(J)**2+OSCORE(I)**2)
      ELSEIF(OPER.EQ.'A') THEN
         DO L=1,NBIN(I)
            HIST(J,L) = HIST(J,L) + HIST(I,L)
            IHIS(J,L) = IHIS(J,L) + IHIS(I,L)
            HIST(K,L) = HIST(K,L) + HIST(I,L)**2
            IHIS(K,L) = IHIS(K,L) + 1
            IENT(K) = IENT(K)+1
            HIST(I,L) = 0
            IHIS(I,L) = 0
         ENDDO
         IENT(J) = IENT(J)+IENT(I)
         IUSCORE(J) = IUSCORE(J) + IUSCORE(I)
         USCORE(J) = USCORE(J) + USCORE(I)
         IOSCORE(J) = IOSCORE(J) + IOSCORE(I)
         OSCORE(J) = OSCORE(J) + OSCORE(I)
         IUSCORE(K) = IUSCORE(K) + 1
         USCORE(K) = USCORE(K) + USCORE(I)**2
         IOSCORE(K) = IOSCORE(K) + 1
         OSCORE(K) = OSCORE(K) + OSCORE(I)**2
         IENT(I) = 0
         IUSCORE(I) = 0
         IOSCORE(I) = 0
         USCORE(I) = 0
         OSCORE(I) = 0
      ELSE
        DO L=1,NBIN(I)
      	IF(OPER.EQ.'+') THEN
       	  HIST(K,L)=X*HIST(I,L) + Y*HIST(J,L)
      	ELSEIF(OPER.EQ.'-') THEN
      	  HIST(K,L)=X*HIST(I,L) - Y*HIST(J,L)
      	ELSEIF(OPER.EQ.'*') THEN
      	  HIST(K,L)=X*HIST(I,L) * Y*HIST(J,L)
      	ELSEIF(OPER.EQ.'/') THEN
          IF(Y.EQ.0..OR.HIST(J,L).EQ.0.) THEN
            HIST(K,L)=0.
          ELSE
            HIST(K,L)=X*HIST(I,L) / (Y*HIST(J,L))
          ENDIF
       	ELSEIF(OPER.EQ.'F') THEN
      	  HIST(K,L)=X*HIST(I,L)
      	ELSEIF(OPER.EQ.'R') THEN
          IF(HIST(I,L).GT.0.) THEN
            HIST(K,L)=X*SQRT(HIST(I,L))
          ELSE                           
            HIST(K,L)=0.
          ENDIF
      	ELSEIF(OPER.EQ.'S') THEN
          HIST(K,L)=X*HIST(I,L)**2
      	ELSEIF(OPER.EQ.'L') THEN  
          IF(HIST(I,L).EQ.0..OR.J.EQ.0.) THEN
             HIST(K,L)=0.
           ELSE
             HIST(K,L)=X*LOG10(Y*HIST(I,L))
           ENDIF
      	ELSEIF(OPER.EQ.'M') THEN
           IF(I.NE.J) XNORM=HIST(I,L)
           IF(I.EQ.J) XNORM=FLOAT(IHIS(J,L))
           IF(XNORM.NE.0.) THEN
             XAVG=HIST(J,L)/XNORM
             HIST(K,L)=
     &       SQRT(ABS(-XAVG**2+HIST(K,L)/XNORM)/FLOAT(IHIS(I,L)))
             HIST(J,L)=XAVG 
           ELSE 
             HIST(K,L)=0.
             HIST(J,L)=0.
           ENDIF
      	ELSEIF(OPER.EQ.'V') THEN                 
           XAVG=HIST(I,L)*X
           XSQAVG=HIST(J,L)*X
           XNORM=FLOAT(IHIS(I,L))*X
           IF(XNORM.NE.0.) THEN
              HIST(K,L)=SQRT(ABS(XSQAVG-XAVG**2)/XNORM)
              HIST(I,L)=XAVG
           ELSE  
              HIST(K,L)=0.
           ENDIF 
      	ELSE 
	 CALL wbb_pwhgwarn('wbb_PWHGOPERA')
         WRITE(*,*) OPER
   5     FORMAT(' ****** OPERATION ="',A1,'" UNKNOWN ********'/)
         STOP
        ENDIF
        ENDDO
      	IF(OPER.EQ.'+') THEN
       	  USCORE(K)=X*USCORE(I) + Y*USCORE(J)  
       	  OSCORE(K)=X*OSCORE(I) + Y*OSCORE(J)  
      	ELSEIF(OPER.EQ.'-') THEN     
      	  USCORE(K)=X*USCORE(I) - Y*USCORE(J)
      	  OSCORE(K)=X*OSCORE(I) - Y*OSCORE(J)
      	ELSEIF(OPER.EQ.'*') THEN     
      	  USCORE(K)=X*USCORE(I) * Y*USCORE(J)
      	  OSCORE(K)=X*OSCORE(I) * Y*OSCORE(J)
      	ELSEIF(OPER.EQ.'/') THEN     
          IF(Y.EQ.0..OR.USCORE(J).EQ.0.) THEN
            USCORE(K)=0.
          ELSE
            USCORE(K)=X*USCORE(I) / (Y*USCORE(J))
          ENDIF
          IF(Y.EQ.0..OR.OSCORE(J).EQ.0.) THEN
            OSCORE(K)=0.
          ELSE
            OSCORE(K)=X*OSCORE(I) / (Y*OSCORE(J))
          ENDIF
       	ELSEIF(OPER.EQ.'F') THEN
      	  USCORE(K)=X*USCORE(I)
      	  OSCORE(K)=X*OSCORE(I)
      	ELSEIF(OPER.EQ.'R') THEN
          IF(USCORE(I).GT.0.) THEN
            USCORE(K)=X*SQRT(USCORE(I))
          ELSE                           
            USCORE(K)=0.
          ENDIF     
          IF(OSCORE(I).GT.0.) THEN
            OSCORE(K)=X*SQRT(OSCORE(I))
          ELSE                           
            OSCORE(K)=0.
          ENDIF     
      	ELSEIF(OPER.EQ.'S') THEN
          USCORE(K)=X*USCORE(I)**2
          OSCORE(K)=X*OSCORE(I)**2
      	ELSEIF(OPER.EQ.'L') THEN  
          IF(USCORE(I).EQ.0..OR.J.EQ.0.) THEN
             USCORE(K)=0.
           ELSE
             USCORE(K)=X*LOG10(Y*USCORE(I))
           ENDIF                         
          IF(OSCORE(I).EQ.0..OR.J.EQ.0.) THEN
             OSCORE(K)=0.
           ELSE
             OSCORE(K)=X*LOG10(Y*OSCORE(I))
           ENDIF                         
        ENDIF
      ENDIF
      RETURN
      END

      SUBROUTINE wbb_PWHGZERO(N)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer i,n
      BOOK(N)='RES'
      IENT(N)=0
      IUSCORE(N)=0
      IOSCORE(N)=0
      HAVG(N)=0.
      HINT(N)=0.
      DO 1 I=1,NBIN(N)
   1  HIST(N,I)=0
      END

      SUBROUTINE wbb_PWHGRESET(N)
      implicit none
      integer n
      include 'src/Lib/wbb/pwhg_book.h'
      BOOK(N)='RES'
      END

      SUBROUTINE wbb_PWHGPUTTAG(J,NAME)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer j
c Per marcare un istogramma
      CHARACTER * (*) NAME, TAG
      BOOK(J) = NAME
      RETURN
      ENTRY wbb_pwhgGETTAG(J,TAG)
      TAG = BOOK(J)
      END

      SUBROUTINE wbb_PWHGFINAL(N)
      implicit none
      integer n
      real * 8 avg,xin,sig,x
      integer j,if
      include 'src/Lib/wbb/pwhg_book.h'
      AVG=0
      XIN=0                                  
      SIG=0
      IF=0
      DO J=1,NBIN(N)
         X=HIST(N,J)
 	 AVG=AVG+X*XHIS(N,J)
         XIN=XIN+X
	 IF(X.NE.0) IF=1
      ENDDO             
      IF(XIN.EQ.0) GO TO 10
      AVG = AVG/XIN
      DO J=1,NBIN(N)
         SIG=HIST(N,J)*(XHIS(N,J)-AVG)**2+SIG
      ENDDO
      SIG=SQRT(ABS(SIG/XIN))
 10   CONTINUE
      HINT(N) = XIN
      HAVG(N) = AVG
      HSIG(N) = SIG
      IF(IF.EQ.0) BOOK(N)='RES'
      END               

      SUBROUTINE wbb_PWHGNORM(N,X)
      implicit none
      integer n,i
      real * 8 x,y
      include 'src/Lib/wbb/pwhg_book.h'
      IF(BOOK(N).NE.'YES')RETURN
      IF(HINT(N).EQ.0.) THEN
	CALL wbb_pwhgwarn('wbb_PWHGNORM')
	WRITE(*,*)' INTEGRAL HIST ',N,' IS ZERO: CANNOT RENORMALIZE'
	RETURN               
      ELSE
	Y=X/HINT(N)
      ENDIF
      DO 1, I=1,NBIN(N)
    1 HIST(N,I)=HIST(N,I)*Y
      HINT(N)=X            
      OSCORE(N)=OSCORE(N)*Y
      USCORE(N)=USCORE(N)*Y
      END                  

      SUBROUTINE wbb_PWHGPRINT(N)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer ini,imon,iday,iyear,n,j,itime(3)
      save imon,iday,iyear,ctime,ini
      character * 8 ctime
      DATA INI/0/
      data ctime/'        '/
      IF(INI.EQ.0) THEN
c      CALL IDATE(IMON,IDAY,IYEAR) ! not y2k compliant
      CALL IDATE(ITIME)
      imon=itime(2)
      iday=itime(1)
      iyear=itime(3)
c      CALL TIME(CTIME)
      INI=1
      ENDIF
      IF(BOOK(N).NE.'YES') RETURN
      WRITE(98,7) N,IDAY,IMON,IYEAR,CTIME(1:3)
      WRITE(98,*) TITLE(N)
      DO 1 J=1,NBIN(N)
      IF(HIST(N,J).EQ.0.) GO TO 1
      WRITE(98,'(3X,F10.4,2X,E15.4)')  
     &                            XHIS(N,J),HIST(N,J)
    1 CONTINUE
      WRITE(98,15) HAVG(N),HSIG(N),HINT(N)
      WRITE(98,20) IENT(N),IUSCORE(N),IOSCORE(N)
    7 FORMAT(4X,'HIST = ',I3,' ',I2,'/',I2,'/',I4,1X,A5/)
   10 FORMAT(4X,2E10.3)
   15 FORMAT(/' AVG =',E10.3,4X,' RMS =',E10.3,' INTEGRAL =',E10.3,/)
   20 FORMAT(' ENTRIES=',I5,4X,'UNDERSCORE=',I5,4x,'OVERSCORE=',I5,//)
      END

      SUBROUTINE wbb_PWHGTOP(N,M,BTIT,LTIT,wbb_scale)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer  imon,iday,iyear,ini,n,m,j,itime(3)
      CHARACTER CTIME*8
      CHARACTER*(*) LTIT,BTIT,wbb_scale
      save imon,iday,iyear,ctime,ini
      DATA INI/0/
      data ctime/'        '/
      IF(INI.EQ.0) THEN
c      CALL IDATE(IMON,IDAY,IYEAR) ! not y2k compliant
      CALL IDATE(ITIME)
      imon=itime(2)
      iday=itime(1)
      iyear=itime(3)
c      CALL TIME(CTIME)
      INI=1
      ENDIF
      IF(BOOK(N).NE.'YES') RETURN
      WRITE(99,100) TITLE(N),BTIT,LTIT,wbb_scale,HMIN(N),HMAX(N)
  100 FORMAT( /1x,                               
     &' SET WINDOW Y 2.5 TO 9.'/,1X,
     &' SET WINDOW X 2.5 TO 10.'/,1X,
     &'(SET FONT SIMPLEX '/1X, 
     &' SET SYMBOL 5O SIZE 1.8'/,1X,
     &' TITLE TOP ','"',A,'"',/1X,
     &' TITLE BOTTOM ','"',A,'"',/1X,
     &' TITLE LEFT ','"',A,'"',/1X,
     &' SET wbb_scale Y ',A,/1X,
     &' (SET TICKS TOP OFF)   '/1x,     
     &' SET LIMITS X ',F10.5,' ',F10.5,/1X,
     &' SET ORDER X Y DY ')
      DO 1 J=1,NBIN(N)
      IF(HIST(N,J).EQ.0.) GO TO 1
      WRITE(99,'(3X,F10.4,2(2X,E15.4))')  
     &                            XHIS(N,J),HIST(N,J),HIST(M,J)
    1 CONTINUE
      WRITE(99,200)
  200 FORMAT('   HIST')
      WRITE(99,300) HINT(N),HAVG(N),HSIG(N),IENT(N),IUSCORE(N)
     &   ,IOSCORE(N),USCORE(N),OSCORE(N),IDAY,IMON,IYEAR,CTIME(1:3)
  300 FORMAT( /1x,                               
     &' BOX 6.25 0.9 SIZE 7.5 1.2'/,1X,
     &' SET WINDOW Y 0. TO 2.'/,1X,
     &' SET TITLE SIZE -1.5'/1X,
     &'(SET FONT SIMPLEX '/1X,
     &' TITLE 2.8 1.2 "INT =',1PE10.3,'   AVG =',1PE10.3,
     &             '   RMS =',1PE10.3,'"',/1X,
     &' TITLE 2.8 0.9 "Entries =',I8,2x,'Undersc =',I6,2X
     &                                 ,'Oversc =',I6,'"',/1X,
     &' TITLE 2.8 0.6 "Ufloat=',1PE10.3,' Ofloat=',
     &      1PE10.3,'"',/1X,
     &' TITLE 7.5 0.6 "',I2,'/',I2,'/',I4,2X,A5,'"',/1X,                
     &' SET TITLE SIZE -2')                            
      WRITE(99,400)
  400 FORMAT('   NEW PLOT')
      END
 
      SUBROUTINE wbb_PWHGMNRTOP(N,M,BTIT,LTIT,wbb_scale)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer imon,iday,iyear,ini,n,m,j,itime(3)
      CHARACTER CTIME*8
      CHARACTER*(*) LTIT,BTIT,wbb_scale
      save imon,iday,iyear,ctime,ini
      DATA INI/0/
      data ctime/'        '/
      IF(INI.EQ.0) THEN
c        CALL IDATE(IMON,IDAY,IYEAR)
         CALL IDATE(ITIME)
         imon=itime(2)
         iday=itime(1)
         iyear=itime(3)
c         CALL TIME(CTIME)
         INI=1
      ENDIF
      CALL wbb_PWHGFINAL(N)
      WRITE(99,100) TITLE(N)(1:20),BTIT,LTIT,wbb_scale,HMIN(N),HMAX(N)
  100 FORMAT( /1x,                               
     &' SET WINDOW Y 2.5 TO 7.'/,1X,
     &' SET WINDOW X 2.5 TO 10.'/,1X,
     &'(SET FONT DUPLEX '/1X, 
     &' SET SYMBOL 5O SIZE 1.8'/,1X,
     &' TITLE TOP ','"',A,'"',/1X,
     &' TITLE BOTTOM ','"',A,'"',/1X,
     &' TITLE LEFT ','"',A,'"',/1X,
     &' SET wbb_scale Y ',A,/1X,
     &' (SET TICKS TOP OFF)   '/1x,     
     &' SET LIMITS X ',F10.5,' ',F10.5,/1X,
     &' SET ORDER X Y DY ')
      DO 1 J=1,NBIN(N)
      WRITE(99,'(3X,F10.4,2(2X,E10.3))')  
     &                            XHIS(N,J),HIST(N,J),HIST(M,J)
    1 CONTINUE
      WRITE(99,200)
  200 FORMAT('   PLOT')
      WRITE(99,300) HINT(N),HAVG(N),HSIG(N),IENT(N),IUSCORE(N)
     &   ,IOSCORE(N),USCORE(N),OSCORE(N),IDAY,IMON,IYEAR,CTIME(1:3)
  300 FORMAT( /1x,                               
     &' BOX 6.25 0.9 SIZE 7.5 1.2'/,1X,
     &' SET WINDOW Y 0. TO 2.'/,1X,
     &' SET TITLE SIZE -1.5'/1X,
     &'(SET FONT SIMPLEX '/1X,
     &' TITLE 2.8 1.2 "INT =',1PE10.3,'   AVG =',1PE10.3,
     &             '   RMS =',1PE10.3,'"',/1X,
     &' TITLE 2.8 0.9 "Entries =',I9,2x,'Usc =',I8,2X
     &                                 ,'Osc =',I8,'"',/1X,
     &' TITLE 2.8 0.6 "Ufloat=',1PE10.3,' Ofloat=',
     &      1PE10.3,'"',/1X,
     &' TITLE 7.5 0.6 "',I2,'/',I2,'/',I4,2X,A5,'"',/1X,                
     &' SET TITLE SIZE -2')
      WRITE(99,400)
  400 FORMAT('   NEW PLOT')
      END


      SUBROUTINE wbb_PWHGBOOK2(N,TIT,DELX,XMIN,XMAX,DELY,YMIN,YMAX)
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      CHARACTER*(*) TIT
      NHIST2=MAX(N,NHIST2)
      TITLE2(N)='   '//TIT                     
      BOOK2(N)='YES'
C-- setup x-boundaries
      HDEL2(1,N)=DELX
      HMIN2(1,N)=XMIN  
      HMAX2(1,N)=XMAX
      NBIN2(1,N)=INT((XMAX-XMIN)/DELX)
C-- setup y-boundaries
      HDEL2(2,N)=DELY
      HMIN2(2,N)=YMIN  
      HMAX2(2,N)=YMAX
      NBIN2(2,N)=INT((YMAX-YMIN)/DELY)
      IENT2(N)=0                      
      IOSCORE2(N)=0
      HAVG2(1,N)=0.
      HAVG2(2,N)=0.
      HINT2(N)=0.
      DO 1 I=1,NBIN2(1,N)
      XHIS2(N,I)=HMIN2(1,N)+HDEL2(1,N)*(FLOAT(I)-0.5D0)
   1  CONTINUE
      DO 2 I=1,NBIN2(2,N)
      YHIS2(N,I)=HMIN2(2,N)+HDEL2(2,N)*(FLOAT(I)-0.5D0)
   2  CONTINUE
      DO 3 I=1,NBIN2(1,N)
      DO 3 J=1,NBIN2(2,N)
      HIST2(N,I,J)=0.                   
   3  CONTINUE
      END        

      SUBROUTINE wbb_PWHGFILL2(N,X,Y,WGT)
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
C-- find x-coordinate
      XI=((X-HMIN2(1,N))/HDEL2(1,N))+1.
      IF(XI.GT.100.OR.XI.LT.0.) THEN
      IOSCORE2(N)=IOSCORE2(N)+1 
      RETURN              
      ENDIF  
      I=INT(XI)
C-- find y-coordinate
      YI=((Y-HMIN2(2,N))/HDEL2(2,N))+1.
      IF(YI.GT.100.OR.YI.LT.0.) THEN
      IOSCORE2(N)=IOSCORE2(N)+1 
      RETURN              
      ENDIF
      J=INT(YI)
      IF(I.GT.0.AND.I.LE.NBIN2(1,N).AND.J.GT.0.AND.J.LE.NBIN2(2,N))
     *                                             THEN        
      IENT2(N)=IENT2(N)+1
      IHIS2(N,I,J)=IHIS2(N,I,J)+1
      HIST2(N,I,J)=HIST2(N,I,J)+WGT
      ELSE
      IOSCORE2(N)=IOSCORE2(N)+1
      ENDIF
      END


      SUBROUTINE wbb_PWHGOPERA2(I,OPER,J,K,X,Y)
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      CHARACTER OPER*1
      IF(NBIN2(1,I).NE.NBIN2(1,J).OR.NBIN2(2,I).NE.NBIN2(2,J).
     &AND.(OPER.EQ.'+'.OR.OPER.EQ.'-'.OR.OPER.EQ.         
     &'*'.OR.OPER.EQ.'/'.OR.OPER.EQ.'M')) GO TO 10
      DO L1=1,NBIN2(1,I)
      DO L2=1,NBIN2(2,I)
      IF(OPER.EQ.'+') THEN
      HIST2(K,L1,L2)=X*HIST2(I,L1,L2) + Y*HIST2(J,L1,L2)
      ELSEIF(OPER.EQ.'-') THEN
      HIST2(K,L1,L2)=X*HIST2(I,L1,L2) - Y*HIST2(J,L1,L2)
      ELSEIF(OPER.EQ.'*') THEN
      HIST2(K,L1,L2)=X*HIST2(I,L1,L2) * Y*HIST2(J,L1,L2)
      ELSEIF(OPER.EQ.'/') THEN
        IF(Y.EQ.0..OR.HIST2(J,L1,L2).EQ.0.) THEN
          HIST2(K,L1,L2)=0.
          ELSE
          HIST2(K,L1,L2)=X*HIST2(I,L1,L2) / (Y*HIST2(J,L1,L2))
        ENDIF
      ELSEIF(OPER.EQ.'F') THEN
      HIST2(K,L1,L2)=X*HIST2(I,L1,L2)
      ELSEIF(OPER.EQ.'R') THEN
        IF(HIST2(I,L1,L2).GT.0.) THEN
        HIST2(K,L1,L2)=X*SQRT(HIST2(I,L1,L2))
        ELSE
        HIST2(K,L1,L2)=0.
        ENDIF
      ELSEIF(OPER.EQ.'S') THEN
      HIST2(K,L1,L2)=X*HIST2(I,L1,L2)**2
      ELSEIF(OPER.EQ.'L') THEN
        IF(HIST2(I,L1,L2).EQ.0..OR.J.EQ.0.) THEN
             HIST2(K,L1,L2)=0.
             ELSE
             HIST2(K,L1,L2)=X*LOG10(Y*HIST2(I,L1,L2))
        ENDIF
      ELSE
      WRITE(98,5) OPER
   5  FORMAT(' ****** OPERATION ="',A1,'" UNKNOWN ********'/)
      RETURN
      ENDIF
      END DO
      ENDDO
      RETURN
  10  WRITE(98,20) I,J
  20  FORMAT(' ****** INCOMPATIBLE OPERATION HIST2 ',I2,' &',I2,
     &                                                   '*******'/)
      END
     
      SUBROUTINE wbb_PWHGFINAL2(N)
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      IF(BOOK2(N).NE.'YES') RETURN
      XIN=0.                                  
C-- projection on the x-axis
      DO 2 I=1,NBIN2(1,N)
        DO 1 J=1,NBIN2(2,N)
   1    XPROJ(N,I)=XPROJ(N,I)+HIST2(N,I,J)
   2  XIN=XIN+XPROJ(N,I)
      IF(XIN.EQ.0.) GO TO 10
C-- projection on the y-axis
      DO 3 J=1,NBIN2(2,N)
        DO 3 I=1,NBIN2(1,N)
   3    YPROJ(N,J)=YPROJ(N,J)+HIST2(N,I,J)
      HINT2(N)=XIN
      RETURN
  10  BOOK2(N)=' NO'
      END               

      SUBROUTINE wbb_PWHGPROHIS(N,M,IAX)
C-- projects the scatter plot N onto the IAX axis (x,y->IAX=1,2) and
C   put the contents in histogram M, after automatically booking it.
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      BOOK(M)='YES'      
      NBIN(M)=NBIN2(IAX,N)
      HDEL(M)=HDEL2(IAX,N)
      HMIN(M)=HMIN2(IAX,N)
      HMAX(M)=HMAX2(IAX,N)
      NHIST=MAX(NHIST,M)
      TITLE(M)=TITLE2(N)//'(PROJ)'
      DO I=1,NBIN(M)
      IF(IAX.EQ.1)      THEN
      HIST(M,I)=XPROJ(N,I)
      XHIS(M,I)=XHIS2(N,I)
      ELSEIF(IAX.EQ.2)      THEN
      HIST(M,I)=YPROJ(N,I)
      XHIS(M,I)=YHIS2(N,I)
      ENDIF
      ENDDO
      END                             


      SUBROUTINE wbb_PWHGNORM2(N,X)    
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      IF(BOOK2(N).NE.'YES')RETURN
      DO 1, I=1,NBIN2(1,N)
      DO 1, J=1,NBIN2(2,N)
    1 HIST2(N,I,J)=HIST2(N,I,J)/HINT2(N)*X
      HINT2(N)=X                      
      END  

      SUBROUTINE wbb_PWHGPRINT2(N)
      implicit real * 8 (a-h,o-z)
      include 'src/Lib/wbb/pwhg_book.h'
      integer imon,iday,iyear,ini,itime(3)
      CHARACTER CTIME*8
      save imon,iday,iyear,ctime,ini
      DATA INI/0/    
      data ctime/'        '/           
      IF(INI.EQ.0) THEN
c      CALL IDATE(IMON,IDAY,IYEAR)
      CALL IDATE(ITIME)
      imon=itime(2)
      iday=itime(1)
      iyear=itime(3)
c      CALL TIME(CTIME)
      INI=1
      ENDIF
      IF(BOOK2(N).NE.'YES') RETURN
      WRITE(98,7) N,IDAY,IMON,IYEAR,CTIME(1:3)
      WRITE(98,*) TITLE2(N)
      WRITE(98,10) ((XHIS2(N,I),YHIS2(N,J),HIST2(N,I,J),J=1,NBIN2(2,N))
     *                            ,I=1,NBIN2(1,N))
      WRITE(98,20) IENT2(N),HINT2(N),IOSCORE2(N)
    7 FORMAT(4X,'HIST = ',I3,' ',I2,'/',I2,'/',I4,1X,A5/)
   10 FORMAT(4X,3E10.3)
   20 FORMAT(' ENTRIES=',I5,4X,' INTEGRAL =',E10.3,4x,
     *              'OVERSCORE=',I5,//)
      END


c$$$      SUBROUTINE PWHGTOP2(N,BTIT,LTIT,PLOPT)
c$$$      implicit real * 8 (a-h,o-z)
c$$$      include 'src/Lib/wbb/pwhg_book.h'
c$$$      CHARACTER CTIME*8
c$$$      integer  imon,iday,iyear,ini,iseed,itime(3)
c$$$      CHARACTER*(*) LTIT,BTIT,PLOPT
c$$$      save imon,iday,iyear,ctime,ini,iseed
c$$$      real * 8 wbb_random
c$$$      DATA INI/0/                  
c$$$      IF(INI.EQ.0) THEN
c$$$      ISEED=1
c$$$c      CALL IDATE(IMON,IDAY,IYEAR)
c$$$      CALL IDATE(ITIME)
c$$$      imon=itime(2)
c$$$      iday=itime(1)
c$$$      iyear=itime(3)
c$$$c      CALL TIME(CTIME)
c$$$      INI=1
c$$$      ENDIF
c$$$      IF(BOOK2(N).NE.'YES') RETURN
c$$$      WRITE(99,100) TITLE2(N),BTIT,LTIT,HMIN2(1,N),HMAX2(1,N),
c$$$     *   HMIN2(2,N),HMAX2(2,N)
c$$$  100 FORMAT( /1x,                               
c$$$     &' SET WINDOW Y 2.5 TO 9.'/,1X,
c$$$     &' SET WINDOW X 2.5 TO 10.'/,1X,
c$$$     &'(SET FONT SIMPLEX '/1X, 
c$$$     &' TITLE TOP ','"',A,'"',/1X,
c$$$     &' TITLE BOTTOM ','"',A,'"',/1X,
c$$$     &' TITLE LEFT ','"',A,'"',/1X,
c$$$     &' (SET TICKS TOP OFF)   '/1x,     
c$$$     &' SET LIMITS X ',F10.5,' ',F10.5,/1X,
c$$$     &' SET LIMITS Y ',F10.5,' ',F10.5,/1X,
c$$$     &' SET ORDER X Y ')
c$$$
c$$$C-- squares with area proportional to the bin weight
c$$$      IF(PLOPT.EQ.'BOX') THEN
c$$$      DO 1, I=1,NBIN2(1,N)
c$$$      DO 1, J=1,NBIN2(2,N)
c$$$      SCMAX=MAX(SCMAX,HIST2(N,I,J))
c$$$  1   CONTINUE
c$$$      SCMAX=0.8*SCMAX
c$$$      DO 2, I=1,NBIN2(1,N)
c$$$      DO 2, J=1,NBIN2(2,N)
c$$$      IF(HIST2(N,I,J).EQ.0.) GO TO 2      
c$$$      WRITE(99,200) XHIS2(N,I),YHIS2(N,J),HIST2(N,I,J)/SCMAX*0.3
c$$$  2   CONTINUE
c$$$  200 FORMAT(2X,'BOX',F14.8,2X,F14.8,3X,'DATA SIZE',F14.8)
c$$$      WRITE(99,300)
c$$$  300 FORMAT('   PLOT')
c$$$      WRITE(99,400) HINT2(N),IENT2(N)
c$$$     &   ,IOSCORE2(N),IDAY,IMON,IYEAR,CTIME(1:5)
c$$$  400 FORMAT( /1x,                               
c$$$     &' BOX 6.25 1.0 SIZE 7.5 1.'/,1X,
c$$$     &' SET WINDOW Y 0. TO 2.'/,1X,
c$$$     &' SET TITLE SIZE -1.5'/1X,
c$$$     &'(SET FONT SIMPLEX '/1X,
c$$$     &' TITLE 2.8 1.2 "INT =',1PE10.3,'    Entries =',I8,2x
c$$$     &                                 ,'Oversc =',I6,'"',/1X,
c$$$     &' TITLE 2.8 0.8 "',I2,'/',I2,'/',I4,2X,A5,'"'/1X,
c$$$     &' SET TITLE SIZE -2')
c$$$      WRITE(99,500)
c$$$  500 FORMAT('   NEW PLOT')
c$$$
c$$$C-- dots - as many as the value of HINT2(n), distributed according to wgt
c$$$      ELSEIF(PLOPT.EQ.'wbb_dot') THEN                                        
c$$$      WRITE(99,*) '  SET INTENSITY 3'
c$$$      DO 3, I=1,NBIN2(1,N)          
c$$$      DO 3, J=1,NBIN2(2,N)
c$$$      IF(HIST2(N,I,J).EQ.0.) GO TO 3
c$$$      NDOTS=INT(HIST2(N,I,J))
c$$$      RN=wbb_random()
c$$$      REST=HIST2(N,I,J)-FLOAT(NDOTS)
c$$$      IF(RN.LT.REST) NDOTS=NDOTS+1
c$$$      DO ND=1,NDOTS
c$$$      X=XHIS2(N,I)+(2.*wbb_random()-1.)*HDEL2(1,N)
c$$$      Y=YHIS2(N,J)+(2.*wbb_random()-1.)*HDEL2(2,N)
c$$$      WRITE(99,*) X,Y
c$$$      ENDDO
c$$$      NTOT=NTOT+NDOTS
c$$$  3   CONTINUE       
c$$$      WRITE(99,*) '  SET INTENSITY 1'
c$$$      WRITE(99,300)                 
c$$$      WRITE(99,600) HINT2(N),IENT2(N),NTOT
c$$$     &   ,IDAY,IMON,IYEAR,CTIME(1:5)
c$$$  600 FORMAT( /1x,                               
c$$$     &' BOX 6.25 1.0 SIZE 7.5 1.'/,1X,
c$$$     &' SET WINDOW Y 0. TO 2.'/,1X,
c$$$     &' SET TITLE SIZE -1.5'/1X,
c$$$     &'(SET FONT SIMPLEX '/1X,
c$$$     &' TITLE 2.8 1.2 "INT =',1PE10.3,'    Entries =',I8,2x
c$$$     &                                 ,'Points =',I6,'"',/1X,
c$$$     &' TITLE 2.8 0.8 "',I2,'/',I2,'/',I4,2X,A5,'"'/1X,
c$$$     &' SET TITLE SIZE -2')
c$$$      WRITE(99,500)
c$$$
c$$$      ENDIF
c$$$      END
            
      SUBROUTINE wbb_PWHGMULTITOP(NH,NE,N,M,BTIT,LTIT,SCA,INI)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      CHARACTER CTIME*8,wbb_scale*3
      CHARACTER*(*) LTIT,BTIT,SCA
      CHARACTER*7  PLOT(4)
      REAL * 8 LAB0,LABS,WIDTH,HEIGHT,XCORN,YCORN,XPFRAC,YPFRAC,
     #TIT0,TIC0,tits,tics,xtit0,xd,sred,yd,ytit0,xl,yl
     #,xu,yu,fmn,fmx,xmx,x,fmin,fmax,fexp,xtit,ytit
      integer imon,iday,iyear,ini,iframe,nh,ne,n,m,ifrmax,nold,mold
     #,i,j,ns,ip,ibin,itime(3)
      save imon,iday,iyear,ctime
c,ini
      DATA PLOT/'SOLID','DASHES','DOTS','DOTDASH'/
C  PLOT SIZE, CORNERS
      DATA WIDTH,HEIGHT/7.d0,8.d0/,XCORN,YCORN/2.d0,1.d0/
C  PLOT VERSUS TEXT FRACTION
      DATA XPFRAC,YPFRAC/0.75d0,0.80d0/
C  DEFAULT SIZES                                          
c      DATA TIT0,LAB0,TIC0/-1.8d0,-1.8d0,0.06d0/
      DATA TIT0,LAB0,TIC0/1.8d0,1.8d0,0.06d0/
cccccccccccc      DATA INI/0/
      data ctime/'        '/
      IF(INI.EQ.0) THEN
c      CALL IDATE(IMON,IDAY,IYEAR)
      CALL IDATE(ITIME)
      imon=itime(2)
      iday=itime(1)
      iyear=itime(3)
c      CALL TIME(CTIME)
      IFRAME=0
      WRITE(99,71) IDAY,IMON,IYEAR,CTIME(1:3)
   71 FORMAT(4X,' ( ',I2,'/',I2,'/',I4,1X,A5,/)
      WRITE(99,*) 'SET BAR Y SIZE=0.02 PERMANENT'
      INI=1         
      ENDIF
      IF(SCA.EQ.'REF') THEN
	IFRAME=0
	RETURN
      ENDIF
      IF(BOOK(NH)(1:1).NE.'Y') RETURN
      IFRMAX=N*M         
      IFRAME=IFRAME+1
      IF(IFRAME.GT.IFRMAX.OR.N.NE.NOLD.OR.M.NE.MOLD) THEN
      	IFRAME=1
        WRITE(99,202)   
c        WRITE(99,1) IMON,IDAY,CTIME(1:3)
c  1     FORMAT(' (SET FONT SIMPLEX',/,
c     +      ' TITLE 9.5 9 ANGLE -90 ','" MLM   ',I2,'-',I2,1X,A5,'"')
      ENDIF                                
      IF(IFRAME.EQ.1) THEN
    	I=1
	J=1
      ELSEIF(IFRAME.LE.IFRMAX) THEN
	IF(I.LE.N) I=I+1
        IF(I.GT.N) THEN
		I=1
		J=J+1
	ENDIF
      ENDIF
      IF(N.EQ.NOLD) GO TO 10
      NS=N-1
      XD=WIDTH/FLOAT(N)
      SRED=(FLOAT(N))
      TITS=TIT0/SRED          
      LABS=LAB0/SRED
      TICS=TIC0/SRED
      XTIT0=0.6*XPFRAC*XD
      NOLD=N            
10    IF(M.EQ.MOLD) GO TO 20
      YD=HEIGHT/FLOAT(M)
      YTIT0=0.04*YD
      MOLD=M        
20    CONTINUE
      XL=(I-1)*XD + XCORN
      YL=(M-J)*YD + YCORN
      XU=XL+XD*XPFRAC
      YU=YL+YD*YPFRAC        
      IP=0
      FMN=MAX(HINT(NH)*NBIN(NH),1.D12)
      FMX=-FMN                        
      XMX=0.
      DO IBIN=1,NBIN(NH)
	X=HIST(NH,IBIN)
      	IF(X.NE.0.) FMX=MAX(FMX,X)
      	IF(X.NE.0.) FMN=MIN(FMN,X)
	XMX=MAX(XMX,ABS(X))
      ENDDO                
      IF(XMX.EQ.0.) GO TO 203
      wbb_scale=SCA
50    IF(wbb_scale.EQ.'LIN') THEN
	IF(FMN.GE.0.)	FMIN=0.
	IF(FMN.LT.0.)	FMIN=FMN*1.3
	IF(FMX.GT.0.)	FMAX=FMX*1.3
	IF(FMX.LT.0.)	FMAX=0.
      ELSEIF(wbb_scale.EQ.'LOG') THEN 
        IF(FMN.LE.0.) THEN
	     	wbb_scale='LIN'
		GO TO 50
	ENDIF
	FMIN=10.**AINT(LOG10(FMN))
	FEXP=AINT(LOG10(FMX))+1
	FMAX=10.**(FEXP)
      ENDIF                         
      WRITE(99,100) TITS,LABS,TICS,XL,XU,YL,YU
100   FORMAT(2X,'(SET FONT SIMPLEX',/,                           
     *       2X,'SET TITLE SIZE ',F8.4,/,
     *       2X,'SET LABEL SIZE ',F8.4,/,
     *       2X,'SET TICKS TOP OFF SIZE ',F8.4,/,
     *       2X,'SET WINDOW X ',F8.4,' TO ',F8.4,/,
     *       2X,'SET WINDOW Y ',F8.4,' TO ',F8.4)
      XTIT=XL+XTIT0
      YTIT=YU+YTIT0
      WRITE(99,101) XL,YTIT,TITLE(NH)(1:)
101   FORMAT('  TITLE ',2(F8.4,1X),'"',A,'"')                  
      YTIT=YTIT-2.*YTIT0
      WRITE(99,102) XTIT,YTIT,HINT(NH)
102   FORMAT('  TITLE ',2(F8.4,1X),'" INT=',1PE10.3,'"')                  
      YTIT=YTIT-YTIT0
      WRITE(99,103) XTIT,YTIT,IENT(NH)
103   FORMAT('  TITLE ',2(F8.4,1X),'" ENT=',I9,'"')                  
      YTIT=YTIT-YTIT0                         
      IF(USCORE(NH).NE.0.) THEN
        WRITE(99,104) XTIT,YTIT,USCORE(NH)
104     FORMAT('  TITLE ',2(F8.4,1X),'" UFL=',1PE10.3,'"')                  
        YTIT=YTIT-YTIT0                      
      ENDIF
      IF(OSCORE(NH).NE.0.) THEN
        WRITE(99,105) XTIT,YTIT,OSCORE(NH)
105     FORMAT('  TITLE ',2(F8.4,1X),'" OFL=',1PE10.3,'"')                  
        YTIT=YTIT-YTIT0                      
      ENDIF      
      WRITE(99,106) XTIT,YTIT,XU,YTIT,XTIT,YTIT,XTIT,YU
106   FORMAT(2X,'SET ORD X Y ',/,2(F8.4,1X),/,2(F8.4,1X),/,
     *       2X,'JOIN TEXT',/,
     *       2X,2(F8.4,1X),/,2(F8.4,1X),/,
     *       2X,'JOIN TEXT')                                    
      WRITE(99,107) BTIT,LTIT,wbb_scale,HMIN(NH),HMAX(NH),FMIN,FMAX
107   FORMAT(                                           
     &' TITLE BOTTOM ','"',A,'"',/1X,
     &' TITLE LEFT ','"',A,'"',/1X,
     &' SET wbb_scale Y ',A,/1X,
     &' SET TICKS TOP OFF   '/1x,     
     &' SET LIMITS X ',F10.5,' ',F10.5,/1X,
     &' SET LIMITS Y ',1PE10.3,' ',1PE10.3,/1X,
     &' SET ORDER X Y DY')               
C                       
C  END HEADER , FILL TOPDRAWER WITH DATA
C
      ENTRY wbb_PWHGTFILL(NH,NE,N,M,BTIT,LTIT,SCA)
      IP=IP+1                             
      IF(IP.GT.4) IP=1
      WRITE(99,110) TITLE(NH),HINT(NH),IENT(NH)
110   FORMAT(' ( ',A,/,' ( INT=',1PE10.3,'  ENTRIES=',I12)
      DO 200 IBIN=1,NBIN(NH)           
      IF(HIST(NH,IBIN).EQ.0..AND.HIST(NE,IBIN).EQ.0.) GO TO 200
c PER AVERE POCHE CIFRE NEL FILE *.TOP. SCOMMENTARE LA SEGUENTE RIGA 
c      WRITE(99,'(3X,F10.4,2(2X,E15.4))') 
c PER AVERE MAGGIORI CIFRE NEL FILE *.TOP. SCOMMENTARE LA SEGUENTE RIGA 
c      WRITE(99,'(3X,F10.9,2(2X,E15.4))')  
      WRITE(99,'(3X,F10.4,2(2X,E15.8))')  
     &          XHIS(NH,IBIN),HIST(NH,IBIN),HIST(NE,IBIN)
200   CONTINUE                                           
      WRITE(99,201)  PLOT(IP)
      IF(BOOK(NE).NE.'NO')   WRITE(99,*)  '  PLOT'
201   FORMAT(2X,'HIST ',A)           
202   FORMAT('   NEW PLOT',/,/)
203   RETURN     
      entry wbb_pwhgresetmultitop
      iframe=0
      END                      

      SUBROUTINE wbb_PWHGNEWPLOT
      WRITE(99,202) 
202   FORMAT('   NEW PLOT',/,/)
      CALL wbb_PWHGMULTITOP(iDUM,iDUM,iDUM,iDUM,' ',' ','REF',0)
      END


c  added by SA and ER to use with MC@NLO
      SUBROUTINE wbb_PWHGCOPY(NIN,NOUT)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer nin,nout,i

C If the histogram is not booked or reset, do not copy it
      IF(BOOK(NIN)(1:1).NE.'Y'.AND.BOOK(NIN)(1:1).NE.'R') RETURN
C If the histogram was reset without having being booked, do not copy it.
C This may happen if wbb_PWHGFINAL is called prior to wbb_PWHGCOPY
      IF(BOOK(NIN)(1:1).EQ.'R'.AND.HMIN(NIN).EQ.HMAX(NIN)) RETURN
      IF(BOOK(NOUT)(1:1).NE.'Y'.AND.BOOK(NOUT)(1:1).NE.'R') 
     &  CALL wbb_PWHGBOOK(NOUT,TITLE(NIN)(2:),HDEL(NIN),HMIN(NIN),HMAX(NIN))
      IF(BOOK(NIN).EQ.'RES')BOOK(NOUT)='RES'
      HDEL(NOUT)=HDEL(NIN)
      HMIN(NOUT)=HMIN(NIN)
      HMAX(NOUT)=HMAX(NIN)
      NBIN(NOUT)=NBIN(NIN)
      IENT(NOUT)=IENT(NIN)
      IUSCORE(NOUT)=IUSCORE(NIN)
      IOSCORE(NOUT)=IOSCORE(NIN)
      USCORE(NOUT)=USCORE(NIN)
      OSCORE(NOUT)=OSCORE(NIN)
      HAVG(NOUT)=HAVG(NIN)
      HINT(NOUT)=HINT(NIN)
      HSIG(NOUT)=HSIG(NIN)
      DO I=1,NBIN(NOUT)
        HIST(NOUT,I)=HIST(NIN,I)
        XHIS(NOUT,I)=XHIS(NIN,I)
        IHIS(NOUT,I)=IHIS(NIN,I)
      ENDDO
      END
      

      subroutine wbb_pwhgwarn(str)
      character *(*) str
      write(*,*) '********** WARNING **********'
      write(*,*) '*********  ',str,'  *********'
      end



      subroutine wbb_pwhgbookup(n,string,wbb_scale,del,xl,xu)
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      real * 8 del,xl,xu
      integer n
      character * (*) string
      character * 3 wbb_scale
c     For each required histogram n, 5 histograms are booked:
c     n, n+nmh,n+nmh2,n+nmh3,n+nmh4 (nmh2=2*nmh, etc.)
c     Results from a correlated set of output configurations
c     are accumulated in n.
c     A call to wbb_pwhgaccumup causes
c     the histogram n to be summed to the n+nmh, its square is
c     summed into n+nmh2. and n is zeroed.
c
c     When the subroutine wbb_pwhgaddout is called, the statistical
c     wbb_analysis of the n+nmh and n+nmh2 content is performed, the
c     average (with respect to the number of calls to wbb_pwhgaccumup)
c     is added to the n+nmh3, and the error is summed in quadrature
c     to the n+nmh4; n+nmh and n+nmh2 are zeroed.
c
c     If instead wbb_pwhgsetout is called, the statistical
c     wbb_analysis of the n+nmh and n+nmh2 content is performed, the
c     average (with respect to the number of calls to wbb_pwhgaccumup)
c     is stored in the n+nmh3, and the error is stored in the n+nmh4;
c     n+nmh and n+nmh2 are left untouched
c.
      if(n.gt.nmh) then
         write(*,*) ' error: no more than ',nmh,' histograms'
         write(*,*) ' increase nmh in pwhg_book.h'
         call exit(1)
      endif
      call wbb_PWHGBOOK(n,  string,del,xl,xu)
      call wbb_PWHGBOOK(n+nmh,'tmp ',del,xl,xu)
      call wbb_PWHGBOOK(n+nmh2,'tmp square',del,xl,xu)
c this is the histogram that will be output
      call wbb_PWHGBOOK(n+nmh3,string,del,xl,xu)
c the wbb_scale for plotting (log or lin) is encoded in the title
      call wbb_PWHGBOOK(n+nmh4,wbb_scale//'error ',del,xl,xu)
      call wbb_PWHGPUTTAG(n,'YST')
      return
      end

      

      subroutine wbb_pwhgaccumup
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer j
      character * 3 tag
      real *8 dummy
      integer l
      real * 8 xxx
C
c     nmb and nxm defined in  pwhg_book.h 
c     nmb is the number of histograms, nxm the number of bins   
c
c     Accumulates values (j+nmh) and squared values (j+nmh2) for
c     statistical wbb_analysis, then empty the original histogram j.
c
      do j=1,nmh
         call wbb_pwhgGETTAG(j,tag)
         if(tag.eq.'YST') then
            do l=1,nbin(j)
               hist(j+nmh,l)=hist(j+nmh,l)+hist(j,l)
               hist(j+nmh2,l)=hist(j+nmh2,l)+hist(j,l)*hist(j,l)
               hist(j,l)=0
            enddo
            ient(j+nmh)=ient(j+nmh)+1
            uscore(j+nmh)=uscore(j+nmh)+uscore(j)
            uscore(j+nmh2)=uscore(j+nmh2)+uscore(j)*uscore(j)
            oscore(j+nmh)=oscore(j+nmh)+oscore(j)
            oscore(j+nmh2)=oscore(j+nmh2)+oscore(j)*oscore(j)
            uscore(j)=0
            oscore(j)=0
         endif
      enddo
      return
      end

      subroutine wbb_pwhgaddout
c     When the subroutine wbb_pwhgaddout is called, the statistical
c     wbb_analysis of the n+nmh and n+nmh2 content is performed, the
c     average (with respect to the number of calls to wbb_pwhgaccumup)
c     is added to the n+nmh3, and the error is summed in quadrature
c     to the n+nmh4; n+nmh and n+nmh2 are zeroed.
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      integer j
      character * 3 tag
      real *8 dummy
      integer l
      real * 8 xxx,xsum,xsumsq
      logical set
      set=.false.
      goto 10
      entry wbb_pwhgsetout
c     If instead wbb_pwhgsetout is called, the statistical
c     wbb_analysis of the n+nmh and n+nmh2 content is performed, the
c     average (with respect to the number of calls to wbb_pwhgaccumup)
c     is stored in the n+nmh3, and the error is stored in the n+nmh4;
c     n+nmh and n+nmh2 are left untouched
      set=.true.
 10   continue
c
      do j=1,nmb
c     finish statistical wbb_analysis
         call wbb_pwhgGETTAG(j,tag)
         if(tag.eq.'YST') then
c     accumulates j+nmh rescaled in j (mean value) and set in j+nmh2 the
c     error estimate (squared standard deviation).  The rescaling factor
c     for the mean value is the number of calls to wbb_pwhgaccumup, which in
c     turn corresponds to the number of entries of any bin of the j+nmh2-th
c     histogram, as filled by pwhgaccumup.
            if(set) then
               do l=1,nbin(j)
                  hist(j+nmh3,l)=0
                  hist(j+nmh4,l)=0
               enddo
               oscore(j+nmh3)=0
               uscore(j+nmh3)=0
               oscore(j+nmh4)=0
               uscore(j+nmh4)=0
               ient(j+nmh3)=0
            endif
            xxx=1d0/ient(j+nmh)
            do l=1,nbin(j)
               xsum=hist(j+nmh,l)
               xsumsq=hist(j+nmh2,l)
               hist(j+nmh3,l)=hist(j+nmh3,l)+xxx*xsum
               hist(j+nmh4,l)=sqrt(hist(j+nmh4,l)**2
     1             + xxx**2*(abs(xsumsq-xsum**2*xxx)))
               
            enddo
            xsum=oscore(j+nmh)
            xsumsq=oscore(j+nmh2)
            oscore(j+nmh3)=oscore(j+nmh3)+xxx*xsum
            oscore(j+nmh4)=sqrt(oscore(j+nmh4)**2
     1             + xxx**2*(abs(xsumsq-xsum**2*xxx)))
            xsum=uscore(j+nmh)
            xsumsq=uscore(j+nmh2)
            uscore(j+nmh3)=uscore(j+nmh3)+xxx*xsum
            uscore(j+nmh4)=sqrt(uscore(j+nmh4)**2
     1             + xxx**2*(abs(xsumsq-xsum**2*xxx)))
            ient(j+nmh3)=ient(j+nmh3)+ient(j+nmh)
            if(.not.set) then
               do l=1,nbin(j)
                  hist(j+nmh,l)=0
                  hist(j+nmh2,l)=0
               enddo
               oscore(j+nmh)=0
               oscore(j+nmh2)=0
               uscore(j+nmh)=0
               uscore(j+nmh2)=0
               ient(j+nmh)=0
            endif
         endif
      enddo
      end
      


      subroutine wbb_pwhgtopout
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      character * 50 title0,wbb_scale
      character * 3 tag
      integer i,ini
      ini=0
      do i=1,nmb
         call wbb_pwhgGETTAG(i,tag)
         if(tag.eq.'YST') then
            call wbb_PWHGFINAL(i+nmh3)
            call wbb_PWHGFINAL(i+nmh4)
            call wbb_pwhggettitle(i,title0)
            call wbb_pwhggettitle(i+nmh4,wbb_scale)
            call wbb_PWHGMULTITOP(i+nmh3,i+nmh4,2,3,title0,' ',wbb_scale(2:4)
     $           ,ini)
         endif
      enddo
      end            


      subroutine wbb_pwhgrescale(wbb_scale)
C********************************************************************
C The following function rescale the content of the histogram by the
C wbb_scale factor.  IMPORTANT: must be called AFTER the statistical
C wbb_analysis has been performed and before histograms are written to the
C TOPDRAWER file. For example : 
C    call wbb_pwhgsetout ( or wbb_pwhgaddout)
C    call wbb_pwhgrescale(wbb_scale)
C    call wbb_pwhgtopout 
C**********************************************************************
      implicit none
      include 'src/Lib/wbb/pwhg_book.h'
      real * 8 wbb_scale,dummy_real
      integer i,dummy_int
      character * 3 tag
      do i=1,nmb
         call wbb_pwhgGETTAG(i,tag)
         if(tag.eq.'YST') then
C wbb_PWHGOPERA(I,'F',J,K,X,Y) multiplies hist I by the factor X, and puts
C the result in hist K;
            call wbb_PWHGOPERA(i+nmh3,'F',dummy_int,i+nmh3,wbb_scale,dummy_real)
C i+nmh3 contains the values 
            call wbb_PWHGOPERA(i+nmh4,'F',dummy_int,i+nmh4,wbb_scale,dummy_real)
C i+nmh4 contains the errors
         endif
      enddo
      end           
      


C*******************************************************************
C     END OF THE HISTOGRAMMING PACKAGE
C*******************************************************************
