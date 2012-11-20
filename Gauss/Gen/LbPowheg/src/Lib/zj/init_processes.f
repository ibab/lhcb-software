      subroutine zj_init_processes
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_flg.h'
      integer i1,i2,i3,i4,i5,i6,k,ii(6)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5)),(i6,ii(6))
      logical debug
      parameter (debug=.false.)
      integer vdecaymode,j
      real * 8 zj_powheginput
      external zj_powheginput
c     check nlegborn. This is only a sanity check while we are TESTING 
c     the code and we change often from one process to the other
      if (nlegborn.ne.5) then
         write(*,*) ' ERROR: set nlegborn to the appropriate value'
         write(*,*) ' for this process in nlegborn.h'
         stop
      endif
      vdecaymode=zj_powheginput("#vdecaymode")
      if(vdecaymode.eq.1) then
c     electronic decay
         i3=11
         i4=-11
      elseif(vdecaymode.eq.2) then
c     muonic decay
         i3=13
         i4=-13
      elseif(vdecaymode.eq.3) then
c     tauonic decay
         i3=15
         i4=-15
      else
c     default: muonic decay
         i3=13
         i4=-13
      endif

*********************************************************************
c     index of the first LIGHT coloured parton in the final state
      flst_lightpart=5
*********************************************************************

*********************************************************************
***********            REAL SUBPROCESSES              ***************
*********************************************************************
      flst_nreal=0
      do i1=-5,5
         do i2=-5,5
            if(i1*i2.gt.0) then
c both quarks or antiquarks
               i5=i1
               i6=i2
               goto 10
            endif
c a quark and a gluon
            if(i1*i2.eq.0) then
               if(i1.ne.0) then
                  i5=i1
                  i6=i2
                  goto 10
               elseif(i2.ne.0) then
                  i5=i2
                  i6=i1
                  goto 10
               endif
            endif
c two gluons or quark antiquark from here on
            if(i1+i2.ne.0) then
c different quark antiquark
               i5=i1
               i6=i2
               goto 10
            endif
c two gluons or quark antiquark of the same flavour
            do i5=0,5
               i6=-i5
c     exclude gg -> Z gg
               if (i1.eq.0.and.i5.eq.0) goto 1234                  
               flst_nreal=flst_nreal+1
               if(flst_nreal.gt.maxprocreal) goto 998
               do k=1,6
                  flst_real(k,flst_nreal)=ii(k)
               enddo
 1234          continue
            enddo
            goto 11
 10         continue
            flst_nreal=flst_nreal+1
            if(flst_nreal.gt.maxprocreal) goto 998
            do k=1,6
               flst_real(k,flst_nreal)=ii(k)
            enddo
 11         continue
         enddo
      enddo
      if (debug) then
         write(*,*) ' real processes',flst_nreal
         do j=1,flst_nreal
            write(*,*) (flst_real(k,j),k=1,nlegreal)
         enddo
      endif


*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
      flst_nborn=0
      do i1=-5,5
         do i2=-5,5
            if(i1.ne.0.and.i1+i2.eq.0) then
c q qbar
               i5=0
               goto 20
            endif
c a quark and a gluon
            if(i1*i2.eq.0) then
               if(i1.ne.0) then
                  i5=i1
                  goto 20
               elseif(i2.ne.0) then
                  i5=i2
                  goto 20
               endif
            endif
            goto 21
 20         continue
            flst_nborn=flst_nborn+1
            if(flst_nborn.gt.maxprocborn) goto 999
            do k=1,5
               flst_born(k,flst_nborn)=ii(k)
            enddo
 21         continue
         enddo
      enddo
      if (debug) then
         write(*,*) ' born processes',flst_nborn
         do j=1,flst_nborn
            write(*,*) (flst_born(k,j),k=1,5)
         enddo
      endif
      return
 998  write(*,*) 'zj_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'zj_init_processes: increase maxprocborn'
      end
 
