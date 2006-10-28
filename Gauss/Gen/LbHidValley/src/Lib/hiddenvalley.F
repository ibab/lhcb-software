C $Id: hiddenvalley.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C =====================================================================
C CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
C =====================================================================
C $Log: not supported by cvs2svn $
C =====================================================================
C  Purpose : The main routine for "Hidded Valley" production, 
C            The lines comes from HV0.403 code by Matt Strassler
C            <strasslr@noether.phys.washington.edu>
C
C  @author: 
C  @date: 2006-10-02
C =====================================================================
      INTEGER FUNCTION hiddenvalley (IRNDM,NEVENTS,CFILE) 
C =====================================================================
C
C   Makes q q --> Z' --> v-quarks -----> vpions -----> visible particles
C
C =====================================================================
      IMPLICIT NONE 
C =====================================================================
C Include files 
C =====================================================================
C Pythia
C =====================================================================
#include "Pythia.inc"
C =====================================================================
C Local 
C =====================================================================
#include "HidValley.inc"
C =====================================================================
C Arguments & Local varinable 
C =====================================================================
      DOUBLE PRECISION ECMHAT      
      INTEGER IRNDM,NEVENTS 
      CHARACTER*(*) CFILE 
      DOUBLE PRECISION ECM,VQSCALE
      DOUBLE PRECISION HVACTION 
      EXTERNAL         HVACTION
      INTEGER          HVINIT,HVEVENT  
      EXTERNAL         HVINIT,HVEVENT  
      INTEGER          I,IEVENT,LUN,IDUMMY
C =====================================================================
      
C ----------------- PYTHIA SETUP ---------------------------------
      WRITE(*,*) 'This program makes p p -> vpion events'
      
C... Read In Parameters ZPMASS,PIMASS,PIZLIF,PIPLIF
C... Could read in others if deserved including NEV,IRNDM, ITEV
C... Remember To Recompile When Changing This File!!!
      
      WRITE(*,*)" Parameters to be used: "
      WRITE(*,*)" ZPMASS: ", ZPMASS
      WRITE(*,*)" PIMASS: ", PIMASS
      WRITE(*,*)" PIZLIF " , PIZLIF
      WRITE(*,*)" PIPLIF " , PIPLIF
      
      IF (PIMASS.LT.25.D0) THEN ! Error control
        WRITE (*,*) ""
        WRITE (*,*) "**************************************************"
        WRITE (*,*)
     &    "WARNING: PYTHIA BUG MAY OCCUR WITH SMALL V-PI MASSES"
        WRITE (*,*) "PYOFSH ROUTINE FOULS UP OFF-SHELL DECAYS TO ZZ, WW"
        WRITE (*,*) "THIS BUG WILL DISAPPEAR WHEN HIGGS IS NO LONGER"
        WRITE (*,*) "USED AS STAND-IN FOR V-PIONS AND ALL DECAYS ARE"
        WRITE (*,*) "PROPERLY CALCULATED"
        WRITE (*,*) "**************************************************"
        WRITE (*,*) ""
      ENDIF
      
      MRPY(1) = IRNDM 
      MRPY(2) = 0 
      
C...Final Les Houches Event File, obtained by combining above two.
      LUN = 23 
      OPEN(LUN,FILE=CFILE,STATUS='unknown',ERR=900)
      MSTP(163)=LUN
      
C ----------------- EVENT LOOP -----------------------------------
      
      DO IEVENT=1,NEVENTS
        
        IF ( SUCCESS .NE. HVEVENT( IDUMMY )  ) THEN 
          WRITE(*,*)'HIDDENVALLEY: error from HVEVENT'
          GO TO 800 
        ENDIF 
        
      ENDDO                     ! end of loop over events 
      
      CALL PYSTAT(1)
      
      IF ( 0 .LT. LUN ) THEN 
        WRITE(LUN,'(A)',ERR=900) '</LesHouchesEvents>'
        CLOSE(LUN,ERR=900)
      ENDIF
      
      hiddenvalley = SUCCESS 
      RETURN
 900  CONTINUE 
      WRITE(*,*)'HIDDENVALLEY: error in open/write/close: "',
     &  CFILE,'"' 
 800  CONTINUE 
      hiddenvalley = FAILURE 
      RETURN
      END
C ======================================================================

C ======================================================================
C The END 
C ======================================================================
      
