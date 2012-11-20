      subroutine gg_h_init_processes
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/pwhg_flg.h'
      include 'src/Lib/gg_h/LesHouches.h'
      integer i1,i2,i3,i4,k,ii(4)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4))
      logical debug
      parameter (debug=.false.)
      integer j
      logical condition
      logical gg_h_flavequiv
      external gg_h_flavequiv
      real * 8 gg_h_powheginput
      external gg_h_powheginput
      integer hdecaymode
      common/gg_h_chdecaymode/hdecaymode
c     check nlegborn. This is only a sanity check while we are TESTING 
c     the code and we change often from one process to the other
      if (nlegborn.ne.3) then
         write(*,*) ' ERROR: set nlegborn to the appropriate value'
         write(*,*) ' for this process in nlegborn.h'
         stop
      endif


c******************************************************
c     Choose the process to be implemented
c******************************************************
c     decay products of the Higgs boson
      hdecaymode=gg_h_powheginput('hdecaymode')
c     change the LHUPI id of the process according to vector boson id
c     and decay
      lprup(1)=10000+hdecaymode ! 10000+idup of Higgs decay product of the W
      

*********************************************************************
c     index of the first LIGHT coloured parton in the final state
      flst_lightpart=4
*********************************************************************

*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
   
      i1=0
      i2=0
      i3=25
      flst_nborn=1
      do k=1,3
         flst_born(k,flst_nborn)=ii(k)
      enddo
      if (debug) then
         write(*,*) ' born processes',flst_nborn
         do j=1,flst_nborn
            write(*,*) (flst_born(k,j),k=1,nlegborn)
         enddo
      endif
      
*********************************************************************
***********            REAL SUBPROCESSES              ***************
*********************************************************************
      flst_nreal=0
      condition=.false.
      do i1=-5,5
         do i2=-5,5
            do i4=-5,5
               condition=(((i1+i2).eq.0).and.(i4.eq.0)).or.
     $              (((i1+i2).eq.i4).and.((i1*i2).eq.0))
               if(condition) then
                  flst_nreal=flst_nreal+1
                  if(flst_nreal.gt.maxprocreal) goto 998
                  do k=1,4
                     flst_real(k,flst_nreal)=ii(k)
                  enddo
 10               continue
               endif
            enddo
         enddo
      enddo
      if (debug) then
         write(*,*) ' real processes',flst_nreal
         do j=1,flst_nreal
            write(*,*) (flst_real(k,j),k=1,nlegreal)
         enddo
      endif
      return
 998  write(*,*) 'gg_h_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'gg_h_init_processes: increase maxprocborn'
      stop
      end
 
 
