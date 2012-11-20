      subroutine wz_init_processes
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/nwz.h'
      include 'src/Lib/wz/ckm.h'
      include 'src/Lib/wz/cabibbo.h'
      include 'src/Lib/wz/pwhg_flst.h'
      include 'src/Lib/wz/pwhg_flg.h'
      include 'src/Lib/wz/pwhg_kn.h'
      include 'src/Lib/wz/pwhg_par.h'
      include 'src/Lib/wz/LesHouches.h'
      integer i1,i2,i3,i4,i5,i6,i7,k,ii(7)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5)),(i6,ii(6)),(i7,ii(7))
      logical debug
      parameter (debug=.true.)
      integer j
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      real * 8 wz_powheginput
      external wz_powheginput
c     lepton masses
      real *8 lepmass(3),decmass,decmass1,decmass2
      common/wz_clepmass/lepmass,decmass,decmass1,decmass2
      logical condition,condition2
      integer ckmint, srint
      include 'src/Lib/wz/cvecbos.h'
      include 'src/Lib/wz/vvsettings.h'
      
c      par_isrtinycsi = 1d-6
c      par_isrtinyy = 1d-6
c      par_fsrtinycsi = 1d-5
c      par_fsrtinyy = 1d-5



c******************************************************
c     Choose the process to be implemented
c******************************************************

c     decay products of the vector bosons
      vdecaymodeW=wz_powheginput('vdecaymodeW')
      vdecaymodeZ=wz_powheginput('vdecaymodeZ')
      vdecaymodeZ=abs(vdecaymodeZ)

      if (wz_powheginput("#zerowidth").eq.1) then 
         zerowidth = .true. 
         write(*,*) 'Zerowidth approximation' 
      else
         zerowidth = .false. 
         write(*,*) 'Generating off-shell Z-bosons '
      endif         
      
      if (wz_powheginput("#dronly").eq.1) then 
         dronly = .true. 
         write(*,*) 'Double resonant diagrams only'
c  cant have srdiags if zerowidth is true
      elseif (zerowidth) then
         write(*,*) 'Single resonant diagrams require off-shell bosons'
         write(*,*) 'Double resonant diagrams only'
         dronly = .true.
      else
         dronly = .false. 
         write(*,*)  'Including single resonant diagrams'
      endif

      interference = (wz_powheginput('#withinterference').ne.0)

C     ensure no interference for e mu
      interference=interference.and.
     .  (abs(vdecaymodeW).eq.abs(vdecaymodeZ))
      
c no interference if zerowidth is true
      interference=(interference).and.(.not. zerowidth)

      if (interference) then
         vsymfact=0.5d0
         write(*,*)'Interference effects taken into account'
      else
         vsymfact=1d0
         write(*,*)'No interference effects taken into account'
      endif

      if(wz_powheginput("#withdamp").ne.0) then
         flg_withdamp = .true.
      else
         flg_withdamp = .false.
      endif



c--   see if there is a diagonal wz_CKM matrix
      if (wz_powheginput("#diagCKM").eq.1) then 
         diagonal_CKM = .true. 
         write(*,*)'Using diagonal wz_CKM matrix'
      else
         diagonal_CKM = .false. 
         write(*,*)'Using non-diagonal wz_CKM matrix'
      endif


      if ((vdecaymodeZ.ne.11).and.(vdecaymodeZ.ne.13).and.
     .    (vdecaymodeZ.ne.12).and.(vdecaymodeZ.ne.14).and.
     .    (vdecaymodeZ.ne.16)) stop 'decay of Z is not included, can go to 
     .    11,13,or neutrinos (12,14,16)'
      
      if ( (abs(vdecaymodeW).ne.11).and.(abs(vdecaymodeW).ne.13))
     .     stop 'decay of W can only go to -11,11,-13,13'


      !TM set MCFM things: wz_nwz 
      if (vdecaymodeW.lt.0) then
         wz_nwz = +1
         write(*,*)'POWHEG: W+ Z production and decay'
      elseif (vdecaymodeW.gt.0) then
         wz_nwz = -1
         write(*,*)'POWHEG: W- Z production and decay'
      endif

      if (vdecaymodeW.eq.-11) write(*,*)'     W+ decays to ve e+'
      if (vdecaymodeW.eq.-13) write(*,*)'     W+ decays to vmu me+'
      if (vdecaymodeW.eq.11) write(*,*)'      W- deacys to e- ve~'
      if (vdecaymodeW.eq.13) write(*,*)'      W- decays to mu- vmu~'      
      if (vdecaymodeZ.eq.11) write(*,*)' and Z decays to e- e+'
      if (vdecaymodeZ.eq.13) write(*,*)' and Z decays to mu- mu+'
      
c     change the LHUPI id of the process according to vector boson id
c     and decay
c     10000+idup of first decay product of W + decay product of Z
      lprup(1)=10000+vdecaymodeW+vdecaymodeZ


c     index of the first coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=7

      if (wz_nwz.eq.1) then
c W+: i3 is the neutrino, i4 is the antilepton
         i3=-vdecaymodeW+1
         i4=vdecaymodeW
      elseif (wz_nwz.eq.-1) then
c W-: i3 is the lepton, i4 is the antineutrino
         i3=vdecaymodeW
         i4= -(vdecaymodeW+1)
      endif

c     Z decay products
      i5=vdecaymodeZ
      i6=-i5
      

*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
       flst_nborn=0
      condition=.false.
      condition2=.false.
      do i1=-4,4
         do i2=-4,4
            condition=(charge3(i1)+charge3(i2)).eq.(sign(3,wz_nwz))
            if (diagonal_CKM) then
               condition2=(i1+i2).eq.(wz_nwz)
            else 
               condition2=.true.
            endif
            if((condition).and.(condition2)) then
c     q qbar'
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
      condition=.false.
      do i1=-4,4
         do i2=-4,4
            if (abs(i1).eq.abs(i2)) goto 11
            do i7=-4,4
               condition=.false.
               condition2=.false.
               if ((i1.eq.0).and.(i2.ne.0)) then
                  condition=(charge3(i2)-charge3(i7))
     $                 .eq.(sign(3,wz_nwz))  
                  if (diagonal_CKM) then
                     condition2=(abs(i2)+abs(i7)).ne.(5)
                  else
                     condition2=.true.
                  endif
               endif
               if ((i2.eq.0).and.(i1.ne.0)) then
                  condition=(charge3(i1)-charge3(i7))
     $                 .eq.(sign(3,wz_nwz))
                  if (diagonal_CKM) then
                     condition2=(abs(i1)+abs(i7)).ne.(5)
                  else
                     condition2=.true.
                  endif               
               endif
               if (i7.eq.0) then
                condition=(charge3(i1)+charge3(i2))
     $                 .eq.(sign(3,wz_nwz))
                if (diagonal_CKM) then
                   condition2=(i1+i2).eq.wz_nwz
                else
                   condition2=.true.
                endif
               endif   
               if((condition).and.(condition2)) then
                  flst_nreal=flst_nreal+1
                  if(flst_nreal.gt.maxprocreal) goto 998
                  do k=1,nlegreal
                     flst_real(k,flst_nreal)=ii(k)
                  enddo
               endif
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
      return
 998  write(*,*) 'wz_init_processes: increase maxprocreal'
      call exit(-1)
 999  write(*,*) 'wz_init_processes: increase maxprocborn'
      call exit(-1)
      end
 

      block data wz_lepmass_data
      real *8 lepmass(3),decmass
      common/wz_clepmass/lepmass,decmass
      data lepmass /0.51099891d-3,0.1056583668d0,1.77684d0/
      end
