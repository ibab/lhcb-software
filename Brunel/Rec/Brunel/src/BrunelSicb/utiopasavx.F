*-- Author :    A.Tsaregorodtsev   04/10/94
      SUBROUTINE UTIOPASAVX
*****************************************************************
*                                                               *
C! Hacked version of UTIOPA to skip input streams in Brunel     *
C! Also skips opening database, already done by GAUDI           *
*                                                               *
*****************************************************************
#ifndef DOC
      IMPLICIT NONE
#include "inc/gcflag.inc"
#include "inc/cblist.inc"
#include "inc/cbfile.inc"
*----------------------------------------------------------------
      CHARACTER*4 NMLUN(NSTLUN),CHFILE,CHOPT,CHSTREAM
      INTEGER MXFNAME
      PARAMETER(MXFNAME=300)
      CHARACTER*300 FNAME
      CHARACTER*256 STRING
      INTEGER LUN(NSTLUN),IHLUN(NSTLUN),ISTAT
      EQUIVALENCE (LUSAVS,LUN(1))
      INTEGER I,J,INDX,IHFILE,IFILE,ILENSTR,IFST
      INTEGER IHSTAR,INDXN
      INTEGER IUCOMP,LNBLNK
      LOGICAL LSTFILE
      DATA NMLUN /'SAVS','SAVG','GETS','GETG','SAVX','GETX',
     >            'GETY','SAVE','SAVA','GETE','GETA','NTPL',
     >            'GET1','GET2','GET3','GET4','GETP','GETZ' /
      DATA CHFILE /'STDT'/
      INTEGER ISLATE
      COMMON /SLATE/ ISLATE(40)
      SAVE NMLUN,CHFILE
*----------------------------------------------------------------
*
      DO I = 1, NSTLUN
        CALL UCTOH(NMLUN(I),IHLUN(I),4,4)
      ENDDO
      CALL UCTOH(CHFILE,IHFILE,4,4)
      CALL UCTOH('****',IHSTAR,4,4)
CMC Do not overwrite streans already initialised in GAUDI!
CMC      CALL VZERO(ILSTREAM,NLUNMAX)
CMC      CALL VZERO(IRSTREAM,3*NLUNMAX)
CMC      CALL VZERO(IRSTORE,NMAXRUN)
      NRSTORE = 0
      IFILE = 0
*
*  Look for standard streams
*____________________________________
*
      DO J=1, NSTLUN
C Skip GETX, GETY, GETZ
	  IF( J. EQ. 6 .OR. J. EQ. 7 .OR. J .EQ. 18 ) GOTO 20
        CALL UTLOOK(NMLUN(J),MIOPLS,NIOPA,I)
        IF(I.NE.0) THEN
          IFST = IUCOMP(MIOPLS(I),IHSTREAM,IFILE)
          IF(IFST.NE.0) GOTO 20
          IFILE = IFILE + 1
          IF(IFILE.GT.NLUNMAX) THEN
            WRITE(LUPRNT,*) ' !!!!! UTIOPA: Too many streams '
            GOTO 999
          ENDIF
          CALL UHTOC(MIOPLS(I+1),4,CH_FOPT(IFILE),4)
          FNAME = ' '
          CALL UHTOC(MIOPLS(I+2),4,FNAME,MXFNAME)
          ILENSTR = INDEX(FNAME,'!')
          IF(ILENSTR.EQ.0) THEN
            print *,' utiopa: ststr', indx,indxn
            WRITE(LUPRNT,100) FNAME
100         FORMAT(' !!!!! UTIOPA: File name does not',
     >             ' end with ! mark ',A)
            GOTO 999
          ENDIF
          FNAME = FNAME(1:ILENSTR-1)
          IF(INDEX(FNAME,'$').NE.0) THEN
            CALL UTFNAME(FNAME,ISTAT)
            IF(ISTAT.NE.0) GOTO 999
          ENDIF
          ILENSTR = LNBLNK(FNAME)
          CH_FNAME(IFILE) = ' '
          IF(FNAME(1:2).EQ.'//') THEN
            CALL UHTOC(MIOPLS(I),4,CHSTREAM,4)
            IF(CHSTREAM(1:3).NE.'GET') THEN
              WRITE(LUPRNT,*)
     > ' !!!!! UTIOPA: Keywords allowed only for GETx type streams.',
     >         ' STOP here...'
              STOP
            ENDIF
            print *,' UTIOPA ',chstream(1:lnblnk(chstream)),ifile,
     >              ' ',fname(1:lnblnk(fname))
            CALL UTFRUNS(CHSTREAM,IFILE,FNAME,ISTAT)
            IF(ISTAT.EQ.2) GOTO 20
          ENDIF
          CH_FNAME(IFILE) = FNAME(1:ILENSTR)
          CALL UTLUNIT(LUN(J),'G')
          ILSTREAM(IFILE) = LUN(J)
          IHSTREAM(IFILE) = MIOPLS(I)
        ENDIF
20      CONTINUE
      ENDDO
*
*  Look for any other stream
*______________________________
*
      INDX = 0
      LSTFILE = .FALSE.
CMC Hack to find database streams already set in Gaudi
	DO I=IFILE+1,NLUNMAX
		IF( IHSTREAM(I) .EQ. 0 ) GOTO 10
	    IFILE = IFILE+1
	ENDDO
CMC
*
10    CONTINUE
*
*  Interpret file with standard file locations
*______________________________________________
*
      IF(MIOPLS(INDX+1).EQ.IHFILE) THEN
        CALL UHTOC(MIOPLS(INDX+3),4,FNAME,MXFNAME)
        ILENSTR = INDEX(FNAME,'!')
        IF(ILENSTR.EQ.0) THEN
          WRITE(LUPRNT,100)
          GOTO 999
        ENDIF
        FNAME = FNAME(1:ILENSTR-1)
        ISTAT = 0
        IF(INDEX(FNAME,'$').NE.0) CALL UTFNAME(FNAME,ISTAT)
        IF(ISTAT.NE.0) THEN
          WRITE(LUPRNT,*) ' !!!!! utiopa: List of standard dbase ',
     >                    'files is illegal: ',FNAME(1:ILENSTR-1)
          WRITE(LUPRNT,*) ' !!!!! Program stops'
          STOP
        ENDIF
        CALL UTLIST(FNAME,IFILE)
        LSTFILE = .TRUE.
*
*  Interpret streams
*____________________
*
      ELSEIF(MIOPLS(INDX+2).EQ.IHSTAR) THEN
        GOTO 998    ! unused part of MIOPLS reached
*
      ELSEIF(IUCOMP(MIOPLS(INDX+1),IHLUN,NSTLUN).EQ.0) THEN
        CALL UHTOC(MIOPLS(INDX+2),4,CHOPT,4)
        CALL UHTOC(MIOPLS(INDX+3),4,FNAME,MXFNAME)
c        print *,' utiopa: CHOPT,FNAME ',CHOPT,' ',FNAME
        ILENSTR = INDEX(FNAME,'!')
        IF(ILENSTR.EQ.0) THEN
          CALL UHTOC(MIOPLS(INDX),4,CHOPT,4)
c           print *,' utiopa: **** ',chopt
          CALL UHTOC(MIOPLS(INDX+1),4,CHOPT,4)
          WRITE(LUPRNT,100) FNAME,chopt
          GOTO 999
        ENDIF
        FNAME = FNAME(1:ILENSTR-1)
        IF(INDEX(FNAME,'$').NE.0) THEN
          CALL UTFNAME(FNAME,ISTAT)
          IF(ISTAT.NE.0) GOTO 999
        ENDIF
*
        IF(INDEX(CHOPT,'D').NE.0) THEN
          CALL UBDDF(FNAME)
        ELSEIF(INDEX(CHOPT,'C').NE.0) THEN
          CALL UTCONST(FNAME)
        ELSEIF(INDEX(CHOPT,'L').NE.0) THEN
          CALL UTLIST(FNAME,IFILE)
        ELSE
          IFILE = IFILE + 1
          IF(IFILE.GT.NLUNMAX) THEN
            WRITE(LUPRNT,*) ' !!!!! UTIOPA: Too many streams '
            GOTO 999
          ENDIF
          CH_FNAME(IFILE) = FNAME
          CH_FOPT(IFILE) = CHOPT
          CALL UTLUNIT(ILSTREAM(IFILE),'G')
          IHSTREAM(IFILE) = MIOPLS(INDX+1)
        ENDIF
      ENDIF
*
*  Find beginning of the next stream definition
*_______________________________________________
*
      CALL UTSYMBOL('!',MIOPLS(INDX+1),NIOPA-INDX,INDXN)
      IF(INDXN.NE.0) THEN
        INDX = INDX+INDXN
        GOTO 10
      ELSEIF(INDX.NE.0) THEN
        print *,' utiopa: indx', indx,indxn
        WRITE(LUPRNT,100) FNAME
        GOTO 999
      ENDIF
*
*  Interpret default file with standard file locations
*______________________________________________________
*
998   CONTINUE
CMC database already opened by Gaudi
	LSTFILE = .TRUE.
CMC
      IF(.NOT.LSTFILE) THEN
        FNAME = ' '
        CALL GETENVF('LHCBDBASE',STRING)
*
*  IF $LHCBDBASE is defined take file from this directory
*    otherwise from $LHCBROOT/$LHCBVERS/dbase
*
        IF(ISLATE(1).EQ.0) THEN
          FNAME = '$LHCBROOT/$LHCBVERS/dbase/'
        ELSE
          FNAME = '$LHCBDBASE/'
        ENDIF
        CALL UTFNAME(FNAME,ISTAT)
        IF(ISTAT.NE.0) THEN
          WRITE(LUPRNT,*) ' !!!!! UTIOPA: Data base location unknown ',
     >                    'fname ',FNAME
          STOP
        ENDIF
        WRITE(LUPRNT,*) ' ----- UTIOPA: Data base will be taken from ',
     >                  'directory:'
        WRITE(LUPRNT,*) ' ----- ',FNAME(1:LNBLNK(FNAME)-1)
        FNAME = FNAME(1:LNBLNK(FNAME))//'standard.stream'
        CALL UTLIST(FNAME,IFILE)
*
      ENDIF     !      .NOT.LSTFILE
*
      NTSTREAM = IFILE
      IF(ISWIT(5).EQ.1) THEN
        print *,'-------------------------'
        print *,'   I/O streams: '
        do i = 1,NTSTREAM
          CALL UHTOC(IHSTREAM(I),4,CHSTREAM,4)
          print '(1x,I4,A1,A4,3I5,A1,A4,A1,A)',
     >           i,' ',CHSTREAM,ILSTREAM(I),IRSTREAM(1,i),IRSTREAM(2,i),
     >           ' ',CH_FOPT(I),' ',CH_FNAME(I)(1:LNBLNK(CH_FNAME(I)))
        enddo
      ENDIF
*
999   CONTINUE
*                                               !   end UTIOPA
      END
#endif
