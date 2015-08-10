* lpair_photos.f =====================================================
* By A. Shamov
* PHOTOS control in LPAIR
*    call lpair_photos(flags,omegamin)
*       integer flags = 1 - radiation off muon lines
*                       2 - radiation off proton lines
*                       3 - both
*=====================================================================
      subroutine lpair_photos(flags,omegamin)
      implicit none
*--------------------------------------------------------------------      
      integer flags
      real omegamin
c
      integer PHOTOSon
      real PHOTomegaMin
      common /photosonoff/ PHOTOSon,PHOTomegaMin
c
      integer kPHOT
*--------------------------------------------------------------------
c
      kPHOT=mod(flags,10)
      if(kPHOT.ne.0) then
         PHOTOSon=flags
         print *
         if(kPHOT.ne.2)
     &        print *,'LPAIR_PHOTOS: raiation off muon lines is on'
         if(kPHOT.ge.2)
     &        print *,'LPAIR_PHOTOS: raiation off proton lines is on'
         print *
      else
         print *
         print *,'LPAIR_PHOTOS: real photon radiation is off'
         print *
         PHOTOSon=0
      endif
      if(omegamin.gt.0.) PHOTomegaMin=omegamin
c
      end
