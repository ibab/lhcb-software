      subroutine wbb_init_processes
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      include 'src/Lib/wbb/LesHouches.h'
      include 'src/Lib/wbb/pwhg_flg.h'
c      include 'src/Lib/wbb/pwhg_kn.h'
      logical debug
      parameter (debug=.false.)
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      integer Wcharge3
      integer idvecbos,vdecaymode      
      real * 8 wbb_powheginput
      external wbb_powheginput
      integer i1,i2,j,k
      logical condition
      integer nflav
      
c     index of the first coloured LIGHT parton in the final state
      flst_lightpart=6

c    ID of vector boson produced
      idvecbos=wbb_powheginput('idvecbos')
      vdecaymode=wbb_powheginput('vdecaymode')


c******************************************************
c     Choose the process to be implemented
c******************************************************
      if(idvecbos.eq.24) then
         Wcharge3=+3
         write(*,*) 
         write(*,*) ' POWHEG: W+ b b~ production' 
         write(*,*) 
      elseif(idvecbos.eq.-24) then
         Wcharge3=-3
         write(*,*) 
         write(*,*) ' POWHEG: W- b b~ production'
         write(*,*)    
      else
         write(*,*) 'ERROR: The ID of vector boson you selected' 
     $        //' is not allowed (24: W+ -24: W-)'
         call exit(1)
      endif
      
      if (.not.(vdecaymode.eq.0.or.vdecaymode.eq.1.or.
     $     vdecaymode.eq.2.or.vdecaymode.eq.3)) then
         write(*,*) 'ERROR: wrong vector-boson decay mode'
         write(*,*) 'vdecaymode = ',vdecaymode,' NOT ALLOWED'
         call exit(1)
      endif     

c     for Wbb set flg_withdamp ALWAYS to true
      flg_withdamp=.true.

      flst_nborn=0
      flst_nreal=0
c     change the LHUPI id of the process according to vector boson id
c     and decay
      lprup(1)=10000 
      
      nflav=4
c     Born graphs
      flst_nborn=0
      do i1=-nflav,nflav
         do i2=-nflav,nflav
            condition=((charge3(i1)+charge3(i2)).eq.Wcharge3)
            if(condition) then
               flst_nborn=flst_nborn+1
               if(flst_nborn.gt.maxprocborn) goto 999
               flst_born(1,flst_nborn)=i1
               flst_born(2,flst_nborn)=i2
               flst_born(3,flst_nborn)=idvecbos
               flst_born(4,flst_nborn)=+5
               flst_born(5,flst_nborn)=-5
            endif
         enddo
      enddo
      if (debug) then
         write(*,*) ' born processes',flst_nborn
         do j=1,flst_nborn
            write(*,*) (flst_born(k,j),k=1,nlegborn)
         enddo
      endif


c     real graphs
      flst_nreal=0
      do i1=-nflav,nflav
         do i2=-nflav,nflav
            condition=((charge3(i1)+charge3(i2)).eq.Wcharge3)
            if(condition) then
               flst_nreal=flst_nreal+1
               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=i1
               flst_real(2,flst_nreal)=i2
               flst_real(3,flst_nreal)=idvecbos
               flst_real(4,flst_nreal)=+5
               flst_real(5,flst_nreal)=-5
               flst_real(6,flst_nreal)=0
            endif
         enddo
      enddo

      do i1=-nflav,nflav
         do i2=-nflav,nflav
c     incoming plus particle is a gluon
            condition=(charge3(i2).eq.Wcharge3+charge3(i1))
            if(condition) then
               flst_nreal=flst_nreal+1
               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=0
               flst_real(2,flst_nreal)=i2
               flst_real(3,flst_nreal)=idvecbos
               flst_real(4,flst_nreal)=+5
               flst_real(5,flst_nreal)=-5
               flst_real(6,flst_nreal)=i1
            endif
c     incoming minus particle is a gluon
            condition=(charge3(i1).eq.Wcharge3+charge3(i2))
            if(condition) then
               flst_nreal=flst_nreal+1
               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=i1
               flst_real(2,flst_nreal)=0
               flst_real(3,flst_nreal)=idvecbos
               flst_real(4,flst_nreal)=+5
               flst_real(5,flst_nreal)=-5
               flst_real(6,flst_nreal)=i2
            endif
         enddo
      enddo

      if (debug) then
         write(*,*) ' real processes',flst_nreal
         do j=1,flst_nreal
            write(*,*) (flst_real(k,j),k=1,nlegreal)
         enddo
      endif


c      ii=1
c      call wbb_intassign(nlegreal,flst_real(1,1),flst_real_tmp(1,ii))
c      do j=2,flst_nreal         
c         do k=1,ii
c           if(wbb_flavequiv(nlegreal,flst_real(1,j),flst_real_tmp(1,k)))then
c              write(*,*) 'found two equivalent real'
c              write(*,*) (flst_real(jj,j),jj=1,nlegreal)
c              write(*,*) (flst_real_tmp(jj,k),jj=1,nlegreal)
c              goto 987
c           endif
c         enddo
c         ii=ii+1
c         call wbb_intassign(nlegreal,flst_real(1,j),flst_real_tmp(1,ii))
c 987  continue
c      enddo
c
c      flst_nreal=ii
cc now copy back to flst_real      
c      do i=1,flst_nreal
c         call wbb_intassign(nlegreal,flst_real_tmp(1,i),flst_real(1,i))
c      enddo

      return
 998  write(*,*) 'wbb_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'wbb_init_processes: increase maxprocborn'
      stop
      end
      


