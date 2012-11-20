      subroutine z_init_processes
      implicit none
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      include 'src/Lib/z/pwhg_kn.h'
      include 'src/Lib/z/LesHouches.h'
      integer i1,i2,i3,i4,i5,k,ii(5)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5))
      logical debug
      parameter (debug=.false.)
      integer j
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      real * 8 z_powheginput
      external z_powheginput
c     vector boson id and decay
      integer idvecbos,vdecaymode,tmp
      common/z_cvecbos/idvecbos,vdecaymode
c     lepton masses
      real *8 lepmass(3),decmass
      common/z_clepmass/lepmass,decmass
      logical condition

c******************************************************
c     Choose the process to be implemented
c******************************************************
c     ID of vector boson produced
      idvecbos=23
c     decay products of the vector boson
      tmp=z_powheginput('vdecaymode')
      select case(tmp)
      case (1)
         vdecaymode=11
      case (2)
         vdecaymode=13
      case (3)
         vdecaymode=15
      case (4)
         vdecaymode=12
      case (5)
         vdecaymode=14
      case (6)
         vdecaymode=16
      case default
         write(*,*) 'ERROR: The decay mode you selected'
     #  //' is not allowed (Up to now only leptonic decays)'
         stop
      end select
      if (lepmass(1).ne.0.51099891d-3) then
         write(*,*) 'block data lepmass not loaded. stop running' 
         stop
      endif

      if ((vdecaymode.lt.11).or.(vdecaymode.gt.16)) then
         write(*,*) 'ERROR: The decay mode you selected'
     #  //' is not allowed (Up to now only leptonic decays)'
         stop
      endif
            
      write(*,*) 
      write(*,*) ' POWHEG: Single Z production and decay'
      if (vdecaymode.eq.11) write(*,*) '         to e- e+ '
      if (vdecaymode.eq.12) write(*,*) '         to ve ve~ '
      if (vdecaymode.eq.13) write(*,*) '         to mu- mu+ '
      if (vdecaymode.eq.14) write(*,*) '         to vmu vmu~ '
      if (vdecaymode.eq.15) write(*,*) '         to tau- tau+ '
      if (vdecaymode.eq.16) write(*,*) '         to vtau vtau~ '
      write(*,*) 
      
c     change the LHUPI id of the process according to vector boson id
c     and decay
      lprup(1)=10000+vdecaymode ! 10000+idup of first decay product of the Z

      if(lprup(1).eq.10011) then
         decmass=lepmass(1)
      elseif(lprup(1).eq.10012) then
         decmass=0d0   
      elseif(lprup(1).eq.10013) then
         decmass=lepmass(2)
      elseif(lprup(1).eq.10014) then
         decmass=0d0   
      elseif(lprup(1).eq.10015) then
         decmass=lepmass(3)     
      elseif(lprup(1).eq.10016) then
         decmass=0d0   
      else
c     not yet implemented
         write(*,*) 'non leptonic Z decays '//
     #        'not yet implemented'
         stop
      endif


c     index of the first coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=5
c     Z decay products
      i3=vdecaymode
      i4=-i3

*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
      flst_nborn=0
      do i1=-5,5
         do i2=-5,5
            if(i1.ne.0.and.i1+i2.eq.0) then
c     q qbar
               flst_nborn=flst_nborn+1
               if(flst_nborn.gt.maxprocborn) goto 999
               do k=1,nlegborn
                  flst_born(k,flst_nborn)=ii(k)
               enddo
            endif
         enddo
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
      do i1=-5,5
         do i2=-5,5
            do i5=-5,5
               condition=.false.
               if(.not.(i1.eq.0.and.i2.eq.0)) then
c     exclude gg
                  if((i1.ne.0).and.(i1+i2.eq.0).and.(i5.eq.0)) then
c     q qbar -> g
                     condition=.true.
                  elseif((i1.eq.0).and.(i2.eq.i5)) then
c     g q
                     condition=.true.
                  elseif((i2.eq.0).and.(i1.eq.i5)) then
c     q g
                     condition=.true.
                  endif
               endif
               if(condition) then
                  flst_nreal=flst_nreal+1
                  if(flst_nreal.gt.maxprocreal) goto 998
                  do k=1,nlegreal
                     flst_real(k,flst_nreal)=ii(k)
                  enddo
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
 998  write(*,*) 'z_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'z_init_processes: increase maxprocborn'
      stop
      end
 

      block data z_lepmass_data
      real *8 lepmass(3),decmass
      common/z_clepmass/lepmass,decmass
      data lepmass /0.51099891d-3,0.1056583668d0,1.77684d0/
      end
