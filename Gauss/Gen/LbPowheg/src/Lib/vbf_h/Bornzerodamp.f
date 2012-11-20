      subroutine vbf_h_bornzerodamp(alr,r0,rc,rs,dampfac)
c given the R_alpha region (i.e. the alr) and the associated
c real contribution r (without pdf factor),
c returns in dampfac the damping factor to be applied to
c the real contribution to implement Born zero suppression
      implicit none
      include 'src/Lib/vbf_h/pwhg_flg.h'
      integer alr
      real * 8 r0,rc,rs,dampfac,h,vbf_h_pwhg_pt2,pt2,vbf_h_powheginput
      logical ini
      data ini/.true./
      save ini,h
      external vbf_h_pwhg_pt2,vbf_h_powheginput
      if(ini) then
         h=vbf_h_powheginput("#hfact")
         if(h.gt.0) then
            write(*,*)'***************************************'
            write(*,*)' Using a damping factor h**2/(pt2+h**2)'
            write(*,*)' to separate real contributions between'
            write(*,*)' Sudakov and remnants    h=',h,' GeV.' 
            write(*,*)'***************************************'
         endif
         ini=.false.
      endif
c local variables
      if(h.gt.0) then
         pt2=vbf_h_pwhg_pt2()
         dampfac=h**2/(pt2+h**2)
      else
         dampfac=1
      endif
      if(r0.gt.5*rc.and.r0.gt.5*rs) then
         dampfac=0
      endif
c      write(*,*) ' vbf_h_bornzerodamp: r0=',r0,'  rc=',rc,'  rs=',rs,
c     # ' dampfac=',dampfac
      end
