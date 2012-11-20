      subroutine vbf_wp_wp_init_processes
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_pdf.h'
      include 'src/Lib/vbf_wp_wp/LesHouches.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flg.h'
      include 'src/Lib/vbf_wp_wp/pwhg_par.h'
      include 'src/Lib/vbf_wp_wp/ww_widths.h'
      include 'src/Lib/vbf_wp_wp/cvecbos.h'
      integer i1,i2,i3,i4,i5,i6,i7,i8,i9,k,ii(nlegreal)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5)),(i6,ii(6)),(i7,ii(7)),
     #  (i8,ii(8)),(i9,ii(9))
      logical debug
      parameter (debug=.false.)
c      parameter (debug=.true.)
      integer j
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      integer fam(-6:6)
      data fam /-3,-3,-2,-2,-1,-1,0,1,1,2,2,3,3/
      logical condition
      real * 8 vbf_wp_wp_powheginput
      external vbf_wp_wp_powheginput
c     lepton masses
      real *8 lepmass(3),decmass
      common/vbf_wp_wp_clepmass/lepmass,decmass
      integer lprup1(100), lprup2(100) 

      integer flst_born_tmp(nlegborn,maxprocborn)
      integer flst_real_tmp(nlegreal,maxprocreal)
      logical vbf_wp_wp_flavequiv
      external vbf_wp_wp_flavequiv
      integer jj,ij,i

      integer max_flav
      logical tag,newtag
c
c******************************************************
c     Choose the process to be implemented
c******************************************************
c    ID of vector bosons produced (both same type)
      idvecbos=vbf_wp_wp_powheginput('idvecbos')
c   decay products of the two vector bosons
      vdecaymodew1=vbf_wp_wp_powheginput('vdecaymodew1')
      vdecaymodew2=vbf_wp_wp_powheginput('vdecaymodew2')

c symmetry factors depending on decays: 
      if(abs(vdecaymodew1).eq.abs(vdecaymodew2)) then
         wsymfact=0.5d0
         write(*,*)'No interference effects taken into account'
         write(*,*)'for identical leptons'
      else
         wsymfact=1d0
      endif

C     default if zerowidth = .true. 
      if (vbf_wp_wp_powheginput("#zerowidth").eq.0) then 
         zerowidth = .false. 
         write(*,*) 'Generating off-shell W-bosons'
      else
         zerowidth = .true. 
         write(*,*) 'Zero-width approximation for Ws' 
      endif

      if (lepmass(1).ne.0.51099891d-3) then
         write(*,*) 'block data lepmass not loaded. stop running' 
         stop
      endif
      
      if(idvecbos.eq.24) then
         if (((vdecaymodew1.ne.-11).and.(vdecaymodew1.ne.-13)
     $        .and.(vdecaymodew1.ne.-15)).or.(
     $        (vdecaymodew2.ne.-11).and.(vdecaymodew2.ne.-13)
     $        .and.(vdecaymodew2.ne.-15))) then
            write(*,*) 'ERROR: The decay mode you selected' /
     $           /' is not allowed '
            stop
         endif
         
         write(*,*) 
         write(*,*) ' POWHEG: W+ W+ + 2j production and decay ' 
         if (vdecaymodew1.eq.-11) write(*,*) '         to e+ ve '
         if (vdecaymodew1.eq.-13) write(*,*) '         to mu+ vmu'
         if (vdecaymodew1.eq.-15) write(*,*) '         to tau+ vtau'
         write(*,*)'            and'
         if (vdecaymodew2.eq.-11) write(*,*) '         e+ ve '
         if (vdecaymodew2.eq.-13) write(*,*) '         mu+ vmu'
         if (vdecaymodew2.eq.-15) write(*,*) '         tau+ vtau'
         write(*,*) 
      elseif(idvecbos.eq.-24) then
         if (((vdecaymodew1.ne.11).and.(vdecaymodew1.ne.13)
     $        .and.(vdecaymodew1.ne.15)).or.(
     $        (vdecaymodew2.ne.11).and.(vdecaymodew2.ne.13)
     $        .and.(vdecaymodew2.ne.15))) then
            write(*,*) 'ERROR: The decay mode you selected' /
     $           /' is not allowed '
            stop
         endif
         write(*,*) 
         write(*,*) ' POWHEG: W- W- + 2j production and decay '
         if (vdecaymodew1.eq.11) write(*,*) '         to e- ve~ '
         if (vdecaymodew1.eq.13) write(*,*) '         to mu- vmu~'
         if (vdecaymodew1.eq.15) write(*,*) '         to tau- vtau~'
         write(*,*)'            and'
         if (vdecaymodew2.eq.11) write(*,*) '         e- ve~'
         if (vdecaymodew2.eq.13) write(*,*) '         mu- vmu~'
         if (vdecaymodew2.eq.15) write(*,*) '         tau- vtau~'
         write(*,*)    
      else
         write(*,*) 'ERROR: The ID of vector boson you selected' 
     $        //' is not allowed (24: W+ -24: W-)'
         stop
      endif

c     change the LHUPI id of the process according to vector boson id
c     and decay
c      lprup(1)=10000+vdecaymode ! 10000+idup of charged decay product of the W
      lprup1(1)=10000+vdecaymodew1 ! 10000+idup of first decay product of W1
      lprup2(1)=10000+vdecaymodew2 ! 10000+idup of first decay product of W2
      
      if(lprup1(1).eq.10011) then
         decmass=lepmass(1)         
      elseif(lprup1(1).eq.(10000-11)) then
         decmass=lepmass(1)        
      elseif(lprup1(1).eq.10013) then
         decmass=lepmass(2)         
      elseif(lprup1(1).eq.(10000-13)) then
         decmass=lepmass(2)
      elseif(lprup1(1).eq.10015) then
         decmass=lepmass(3)         
      elseif(lprup1(1).eq.(10000-15)) then
         decmass=lepmass(3)   
      else
c     not yet implemented
         write(*,*) 'non leptonic W decays '//
     #       'not yet implemented'
         stop
      endif   
      if(lprup2(1).eq.10011) then
         decmass=lepmass(1)         
      elseif(lprup2(1).eq.(10000-11)) then
         decmass=lepmass(1)        
      elseif(lprup2(1).eq.10013) then
         decmass=lepmass(2)         
      elseif(lprup2(1).eq.(10000-13)) then
         decmass=lepmass(2)
      elseif(lprup2(1).eq.10015) then
         decmass=lepmass(3)         
      elseif(lprup2(1).eq.(10000-15)) then
         decmass=lepmass(3)   
      else
c     not yet implemented
         write(*,*) 'non leptonic W decays '//
     #       'not yet implemented'
         stop
      endif   
c*********************************************************     
c
      tag = .true.
      newtag = .true.

      if (.not.tag) then
         do i=1,nlegborn
            do j=1,maxprocborn
               flst_borntags(i,j)=0
            enddo
         enddo
         do i=1,nlegreal
            do j=1,maxprocreal
               flst_realtags(i,j)=0
            enddo
         enddo
      endif

      if (.not.tag) then
         write(*,*) 'jet tagging '//
     #        'must be switched on'
         stop   
      endif

c     index of the first light coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=7
      max_flav = 4 ! no b in initial state
      flst_nborn=0
      flst_nreal=0

      i3=vdecaymodew1
      if ((idvecbos.eq.24).and.(vdecaymodew1.lt.0)) then
         i4=-vdecaymodew1+1
      elseif ((idvecbos.eq.-24).and.(vdecaymodew1.gt.0)) then
         i4=-(vdecaymodew1+1)
      endif
c
      i5=vdecaymodew2
      if ((idvecbos.eq.24).and.(vdecaymodew2.lt.0)) then
         i6=-vdecaymodew2+1
      elseif ((idvecbos.eq.-24).and.(vdecaymodew2.gt.0)) then
         i6=-(vdecaymodew2+1)
      endif

c     vbf_wp_wp_born graphs
      flst_nborn=0
      condition=.false.

      do i1=-max_flav,max_flav
      do i2=-max_flav,max_flav
      do i7=-max_flav,max_flav
      do i8=-max_flav,max_flav
c
C     charge conservation 
                  condition=(charge3(i1)+charge3(i2))
     .                 .eq.(2*sign(3,idvecbos)+charge3(i7)+charge3(i8))
c
C assume diagonal CKM and no s-channel contributions:
                  condition = condition .and. (
     .                 (-fam(i1)==-fam(i7) .and. -fam(i2)==-fam(i8))
     .                 )
                  if(condition) then
                     flst_nborn=flst_nborn+1
                     if(flst_nborn.gt.maxprocborn) goto 999
                     do k=1,nlegborn
                        flst_born(k,flst_nborn)=ii(k)
                     enddo
                        if (tag) then
                           flst_borntags(1,flst_nborn)=1
                           flst_borntags(2,flst_nborn)=2
                           flst_borntags(3,flst_nborn)=0
                           flst_borntags(4,flst_nborn)=0
                           flst_borntags(5,flst_nborn)=0
                           flst_borntags(6,flst_nborn)=0
                           flst_borntags(7,flst_nborn)=7
                           flst_borntags(8,flst_nborn)=8
                           if (newtag) then
                              flst_borntags(1,flst_nborn)=1
                              flst_borntags(2,flst_nborn)=2
                              flst_borntags(3,flst_nborn)=0
                              flst_borntags(4,flst_nborn)=0
                              flst_borntags(5,flst_nborn)=0
                              flst_borntags(6,flst_nborn)=0
                              flst_borntags(7,flst_nborn)=1 
                              flst_borntags(8,flst_nborn)=2 
                           endif !newtag
                        endif !tag
                  endif
      enddo !i8
      enddo !i7
      enddo !i2
      enddo !i1
      if (debug) then
         write(*,*) ' vbf_wp_wp_born processes',flst_nborn
         do j=1,flst_nborn
            write(*,*) (flst_born(k,j),k=1,nlegborn)
         enddo
      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     Real graphs
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      flst_nreal=0
      condition=.false.

C the qq->qqg case:
      do i1=-max_flav,max_flav
      do i2=-max_flav,max_flav
      do i7=-max_flav,max_flav
      do i8=-max_flav,max_flav
      do i9=0,0
c
C     charge conservation 
         condition=(charge3(i1)+charge3(i2))
     .        .eq.(2*sign(3,idvecbos)+charge3(i7)+charge3(i8)
     .        +charge3(i9))
C 
c    assume diagonal CKM and no s-channel annihilation:
         condition = condition .and. (
     .    (-fam(i1)==-fam(i7).and.-fam(i2)==-fam(i8).and.fam(i9)==0) 
     .    )    
         if(condition) then
            flst_nreal=flst_nreal+1
            if(flst_nreal.gt.maxprocreal) goto 998
            do k=1,nlegreal
               flst_real(k,flst_nreal)=ii(k)
            enddo
            if (tag) then
                flst_realtags(1,flst_nreal)=1
                flst_realtags(2,flst_nreal)=2
                flst_realtags(3,flst_nreal)=0
                flst_realtags(4,flst_nreal)=0
                flst_realtags(5,flst_nreal)=0
                flst_realtags(6,flst_nreal)=0
                flst_realtags(7,flst_nreal)=7
                flst_realtags(8,flst_nreal)=8
                flst_realtags(9,flst_nreal)=0
                if (newtag) then
                    flst_realtags(1,flst_nreal)=1
                    flst_realtags(2,flst_nreal)=2
                    flst_realtags(3,flst_nreal)=0
                    flst_realtags(4,flst_nreal)=0
                    flst_realtags(5,flst_nreal)=0
                    flst_realtags(6,flst_nreal)=0
                    flst_realtags(7,flst_nreal)=1 !7
                    flst_realtags(8,flst_nreal)=2 !8
                    flst_realtags(9,flst_nreal)=0
                endif !newtag
            endif !tag
         endif
      enddo !i9
      enddo !i8
      enddo !i7
      enddo !i2
      enddo !i1
c
ccccc
c
C the gq->qqq case: 
      do i1=0,0 
      do i2=-max_flav,max_flav
      do i7=-max_flav,max_flav
      do i8=-max_flav,max_flav
      do i9=-max_flav,-1
c
C     charge conservation 
         condition=(charge3(i1)+charge3(i2))
     .        .eq.(2*sign(3,idvecbos)+charge3(i7)+charge3(i8)
     .        +charge3(i9))
c
c    assume diagonal CKM and no s-channel annihilation:
         condition = condition .and. (
     .    (fam(i9)==-fam(i7).and.-fam(i2)==-fam(i8).and.-fam(i1)==0)
     .     )    
         if(condition) then
            flst_nreal=flst_nreal+1
            if(flst_nreal.gt.maxprocreal) goto 998
            do k=1,nlegreal
               flst_real(k,flst_nreal)=ii(k)
            enddo
            if (tag) then
                flst_realtags(1,flst_nreal)=0
                flst_realtags(2,flst_nreal)=2
                flst_realtags(3,flst_nreal)=0
                flst_realtags(4,flst_nreal)=0
                flst_realtags(5,flst_nreal)=0
                flst_realtags(6,flst_nreal)=0
                flst_realtags(7,flst_nreal)=7
                flst_realtags(8,flst_nreal)=8
                flst_realtags(9,flst_nreal)=1
                if (newtag) then
                    flst_realtags(1,flst_nreal)=0
                    flst_realtags(2,flst_nreal)=2
                    flst_realtags(3,flst_nreal)=0
                    flst_realtags(4,flst_nreal)=0
                    flst_realtags(5,flst_nreal)=0
                    flst_realtags(6,flst_nreal)=0
                    flst_realtags(7,flst_nreal)=1 
                    flst_realtags(8,flst_nreal)=2
                    flst_realtags(9,flst_nreal)=1
                endif !newtag
            endif !tag
         endif
      enddo !i9
      enddo !i8
      enddo !i7
      enddo !i2
      enddo !i1

C the qg->qqq case:
      do i1=-max_flav,max_flav
      do i2=0,0
      do i7=-max_flav,max_flav
      do i8=-max_flav,max_flav
      do i9=-max_flav,-1
c
C     charge conservation 
         condition=(charge3(i1)+charge3(i2))
     .        .eq.(2*sign(3,idvecbos)+charge3(i7)+charge3(i8)
     .        +charge3(i9))
c
c    assume diagonal CKM and not s-channel annihilation:
         condition = condition .and. (
     .    (-fam(i1)==-fam(i7).and.fam(i9)==-fam(i8).and.-fam(i2)==0)!.or. 
     .     )    
         if(condition) then
            flst_nreal=flst_nreal+1
            if(flst_nreal.gt.maxprocreal) goto 998
            do k=1,nlegreal
               flst_real(k,flst_nreal)=ii(k)
            enddo
            if (tag) then
                flst_realtags(1,flst_nreal)=1
                flst_realtags(2,flst_nreal)=0
                flst_realtags(3,flst_nreal)=0
                flst_realtags(4,flst_nreal)=0
                flst_realtags(5,flst_nreal)=0
                flst_realtags(6,flst_nreal)=0
                flst_realtags(7,flst_nreal)=7
                flst_realtags(8,flst_nreal)=8
                flst_realtags(9,flst_nreal)=2
                if (newtag) then
                    flst_realtags(1,flst_nreal)=1
                    flst_realtags(2,flst_nreal)=0
                    flst_realtags(3,flst_nreal)=0
                    flst_realtags(4,flst_nreal)=0
                    flst_realtags(5,flst_nreal)=0
                    flst_realtags(6,flst_nreal)=0
                    flst_realtags(7,flst_nreal)=1 !7
                    flst_realtags(8,flst_nreal)=2 !8
                    flst_realtags(9,flst_nreal)=2
                endif !newtag
            endif !tag
         endif
      enddo !i9
      enddo !i8
      enddo !i7
      enddo !i2
      enddo !i1

      if (debug) then
         write(*,*) ' real processes',flst_nreal
         do j=1,flst_nreal
            write(*,*) (flst_real(k,j),k=1,nlegreal)
         enddo
      endif
      return
 998  write(*,*) 'vbf_wp_wp_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'vbf_wp_wp_init_processes: increase maxprocborn'
      end
      
      block data vbf_wp_wp_lepmass_data 
      real *8 lepmass(3),decmass
      common/vbf_wp_wp_clepmass/lepmass,decmass
      data lepmass /0.51099891d-3,0.1056583668d0,1.77684d0/
      end 
