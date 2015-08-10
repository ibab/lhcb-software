* lpair_vacpol.f=======================================================
* By A. Shamov
* Vacuum polarization control in LPAIR
*    call lpair_vacpol(flags)
*      integer flags = 0 - no corrections (default)
*                   != 0 - simplest way of correction as in
*                           Berend's GG_MMGGEN
*=====================================================================
      subroutine lpair_vacpol(flags)
      implicit none
*--------------------------------------------------------------------      
      integer flags
c
      integer mVacPol
      common /vacpol_cb/ mVacPol
*--------------------------------------------------------------------
c
      mVacPol=flags
c
      print *
      print *,'LPAIR_VACPOL: vacuum polarization option',mVacPol
      print *
c
      end
