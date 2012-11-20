      subroutine w_init_processes
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      include 'src/Lib/w/pwhg_kn.h'
      include 'src/Lib/w/pwhg_pdf.h'
      include 'src/Lib/w/LesHouches.h'
      include 'src/Lib/w/pwhg_flg.h'
      integer i1,i2,i3,i4,i5,k,ii(nlegreal)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5))
      logical debug
      parameter (debug=.false.)
      integer j
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      logical condition
      real * 8 w_powheginput
      external w_powheginput
c     vector boson id and decay
      integer idvecbos,vdecaymode,tmp
      common/w_cvecbos/idvecbos,vdecaymode
c     lepton masses
      real *8 lepmass(3),decmass
      common/w_clepmass/lepmass,decmass

c******************************************************
c     Choose the process to be implemented
c******************************************************
c    ID of vector boson produced
      idvecbos=w_powheginput('idvecbos')
c   decay products of the vector boson
      tmp=w_powheginput('vdecaymode')
 
      if (lepmass(1).ne.0.51099891d-3) then
         write(*,*) 'block data lepmass not loaded. stop running' 
         stop
      endif
      
      if(idvecbos.eq.24) then
         select case(tmp)
         case (1)
            vdecaymode=-11
         case (2)
            vdecaymode=-13
         case (3)
            vdecaymode=-15
         case default
            write(*,*) 'ERROR: The decay mode you selected' /
     $           /' is not allowed '
            stop
         end select  
         write(*,*) 
         write(*,*) ' POWHEG: Single W+ production and decay ' 
         if (vdecaymode.eq.-11) write(*,*) '         to e+ ve '
         if (vdecaymode.eq.-13) write(*,*) '         to mu+ vmu'
         if (vdecaymode.eq.-15) write(*,*) '         to tau+ vtau'
         write(*,*) 
      elseif(idvecbos.eq.-24) then
         select case(tmp)
         case (1)
            vdecaymode= 11
         case (2)
            vdecaymode= 13
         case (3)
            vdecaymode= 15
         case default
            write(*,*) 'ERROR: The decay mode you selected' /
     $           /' is not allowed '
            stop
         end select
         write(*,*) 
         write(*,*) ' POWHEG: Single W- production and decay '
         if (vdecaymode.eq.11) write(*,*) '         to e- ve~ '
         if (vdecaymode.eq.13) write(*,*) '         to mu- vmu~'
         if (vdecaymode.eq.15) write(*,*) '         to tau- vtau~'
         write(*,*)    
      else
         write(*,*) 'ERROR: The ID of vector boson you selected' 
     $        //' is not allowed (24: W+ -24: W-)'
         stop
      endif

c     change the LHUPI id of the process according to vector boson id
c     and decay
      lprup(1)=10000+vdecaymode ! 10000+idup of charged decay product of the W
      
      if(lprup(1).eq.10011) then
         decmass=lepmass(1)
         
      elseif(lprup(1).eq.(10000-11)) then
         decmass=lepmass(1)
        
      elseif(lprup(1).eq.10013) then
         decmass=lepmass(2)
         
      elseif(lprup(1).eq.(10000-13)) then
         decmass=lepmass(2)

      elseif(lprup(1).eq.10015) then
         decmass=lepmass(3)
         
      elseif(lprup(1).eq.(10000-15)) then
         decmass=lepmass(3) 
  
      else
c     not yet implemented
         write(*,*) 'non leptonic W decays '//
     #        'not yet implemented'
         stop
      endif   
c*********************************************************     
c
c     index of the first coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=5
      i3=vdecaymode
      if ((idvecbos.eq.24).and.(vdecaymode.lt.0)) then
         i4=-vdecaymode+1
      elseif ((idvecbos.eq.-24).and.(vdecaymode.gt.0)) then
         i4=-(vdecaymode+1)
      endif
c     Born graphs
      flst_nborn=0
      condition=.false.
      do i1=-5,5
         do i2=-5,5
            condition=(charge3(i1)+charge3(i2)).eq.(sign(3,idvecbos))
            if(condition) then
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
     
c     Real graphs    
      flst_nreal=0
      condition=.false.
      do i1=-5,5
         do i2=-5,5
            if (abs(i1).eq.abs(i2)) goto 11
            do i5=-5,5
               condition=.false.
               if ((i1.eq.0).and.(i2.ne.0)) then
                  condition=(charge3(i2)-charge3(i5))
     $                 .eq.(sign(3,idvecbos))    
               endif
               if ((i2.eq.0).and.(i1.ne.0)) then
                  condition=(charge3(i1)-charge3(i5))
     $                 .eq.(sign(3,idvecbos))
               endif
               if (i5.eq.0) then
                condition=(charge3(i1)+charge3(i2))
     $                 .eq.(sign(3,idvecbos))
               endif   
               if(condition) then
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
 998  write(*,*) 'w_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'w_init_processes: increase maxprocborn'
      end
      
      block data w_lepmass_data 
      real *8 lepmass(3),decmass
      common/w_clepmass/lepmass,decmass
      data lepmass /0.51099891d-3,0.1056583668d0,1.77684d0/
      end
