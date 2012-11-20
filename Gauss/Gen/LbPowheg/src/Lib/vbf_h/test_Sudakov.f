      subroutine vbf_h_testsuda
c tests the implementation of the generation of radiation
c It computes the integral of the R/B (real over born)
c times theta(pt2(r)-pt2) as a function of pt2,
c and then computes the negative exponent of the integral
c (which is the probability of no emission).
c It then computes the probability of radiation above a given pt cut.
c This is related to the no-emission probability (1-m the above).
c the two results are compared.
      implicit none
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_rad.h'
      include 'src/Lib/vbf_h/pwhg_par.h'
      integer ncalls1,ncalls2,nbins
      parameter (ncalls1=20000,ncalls2=2000,nbins=40)
      real * 8 hist1(nbins),hist2(nbins),histsq1(nbins),av1,avsq1,err1
      real * 8 sig,born,ptsq,www,xjac,tmp
      real * 8 vbf_h_random,vbf_h_pwhg_pt2,vbf_h_powheginput
      character * 35 string
      external vbf_h_random,vbf_h_pwhg_pt2,vbf_h_powheginput
      logical vbf_h_refuse_pdf
      integer j,k,firstreg,lastreg
      integer iun
      logical ini
      data ini/.true./
      save ini,iun
      if(ini) then
         call vbf_h_newunit(iun)
         open(unit=iun,file='vbf_h_testsuda.top',
     #        status='unknown')
         firstreg=vbf_h_powheginput("#radregion")
         if(firstreg.le.0) then
            firstreg=1
            lastreg=rad_nkinreg
         else
            lastreg=firstreg
         endif
         ini=.false.
      endif
      do k=1,nbins
         hist1(k)=0
         hist2(k)=0
         histsq1(k)=0
      enddo
      write(*,*) ' computing sudakov integral'
      call vbf_h_randomsave
      do rad_kinreg=firstreg,lastreg
         if(rad_kinreg_on(rad_kinreg)) then
            do j=1,ncalls1
               tmp=vbf_h_random()
               kn_csitilde=tmp**2
               kn_csitilde=kn_csitilde*(1-par_isrtinycsi)+par_isrtinycsi

               xjac=2*tmp
               tmp=vbf_h_random()
               kn_y=1-2*tmp
               xjac=xjac*2
               xjac=xjac*1.5d0*(1-kn_y**2)*(1-par_fsrtinyy)

               kn_y=1.5d0*(kn_y-kn_y**3/3)
               kn_azi=2*pi*vbf_h_random()
               xjac=xjac*2*pi
               if(rad_kinreg.eq.1) then
c     initial state radiation
                  call vbf_h_gen_real_phsp_isr_rad0
               else
c     final state radiation
                  call vbf_h_gen_real_phsp_fsr_rad0
               endif
               ptsq=vbf_h_pwhg_pt2()
               if(ptsq.gt.rad_ptsqmin) then
                  call vbf_h_set_rad_scales(ptsq)
                  if(.not.vbf_h_refuse_pdf()) then
                     call vbf_h_sigborn_rad(born)
                     call vbf_h_sigreal_rad(sig)
                     www=sig/born*kn_jacreal*kn_csimax*xjac
                     do k=1,nbins
                        if(log(ptsq/rad_ptsqmin).gt.
     1                       k*log(kn_sbeams/rad_ptsqmin)/nbins)
     2                       then
                           hist1(k)=hist1(k)+www
                           histsq1(k)=histsq1(k)+www**2
                        endif
                     enddo
                  endif
               endif
            enddo
         endif
      enddo
      write(*,*) ' generating radiation'
      do j=1,ncalls2
         call vbf_h_gen_radiation
         ptsq=vbf_h_pwhg_pt2()
         if(ptsq.ge.rad_ptsqmin) then
            do k=1,nbins
               if(log(ptsq/rad_ptsqmin).gt.
     #      k*log(kn_sbeams/rad_ptsqmin)/nbins) then
                  hist2(k)=hist2(k)+1
               endif
            enddo
         endif
      enddo
      call vbf_h_randomrestore 
      write(iun,*) ' newplot'
      write(iun,*) ' Title top "',rad_ubornidx,'"'
      call  vbf_h_from_number_to_madgraph
     #         (nlegborn,flst_uborn(1,rad_ubornidx),-1,string)
      write(iun,*) '(',string
      write(iun,*) ' set order x y'
      write(iun,*) ' set scale x log'
      do k=1,nbins
         av1=hist1(k)/ncalls1
         write(iun,*) exp(k*log(kn_sbeams/rad_ptsqmin)/nbins)*
     #                   rad_ptsqmin,     exp(-av1)
      enddo
      write(iun,*) ' join'
      do k=1,nbins
         av1=hist1(k)/ncalls1
         avsq1=histsq1(k)/ncalls1
         err1=sqrt((avsq1-av1**2)/ncalls1)
         write(iun,*) exp(k*log(kn_sbeams/rad_ptsqmin)/nbins)*
     #                   rad_ptsqmin,     exp(-av1+err1)
      enddo
      write(iun,*) ' join'
      do k=1,nbins
         av1=hist1(k)/ncalls1
         avsq1=histsq1(k)/ncalls1
         err1=sqrt((avsq1-av1**2)/ncalls1)
         write(iun,*) exp(k*log(kn_sbeams/rad_ptsqmin)/nbins)*
     #                   rad_ptsqmin,     exp(-av1-err1)
      enddo
      write(iun,*) ' join'
      write(iun,*) ' set order x y dy'
      do k=1,nbins
         write(iun,*) exp(k*log(kn_sbeams/rad_ptsqmin)/nbins)*
     #                   rad_ptsqmin,     1-hist2(k)/ncalls2,
     2                   sqrt(dble(hist2(k)))/ncalls2
      enddo
      write(iun,*) ' join dashes'
      write(iun,*) ' plot'
      call flush(iun)
      end
