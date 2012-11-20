      subroutine vbf_h_init_processes
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/LesHouches.h'
      logical debug
      parameter (debug=.false.)
      integer j,i,ii,jj,k
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      logical condition
      integer ferm_charge(5)
      character *3 flav(-5:5)
      data (flav(i),i=-5,5) 
     #     /'b~','c~','s~','u~','d~','g','d','u','s','c','b'/
      integer max_flav
      logical emit_Wp_upper,emit_Wm_upper,emit_Wp_lower,emit_Wm_lower
      integer flst_nreal_WW, flst_nborn_WW
      integer HZZ,HWW
      logical CKM_diag
      logical Higgsdecay
      common/vbf_h_cHiggsdecay/Higgsdecay
      integer flst_real_tmp(nlegreal,maxprocreal)
      logical vbf_h_flavequiv
      external vbf_h_flavequiv
      logical tag,newtag
      integer hdecaymode
      real * 8 vbf_h_powheginput
      external vbf_h_powheginput


c     decay products of the Higgs boson
      hdecaymode=vbf_h_powheginput('#hdecaymode')
      if (hdecaymode.lt.0) then
c     default: no Higgs boson decay
         hdecaymode=-1
      endif
c     change the LHUPI id of the process according to vector boson id
c     and decay
      lprup(1)=10000+hdecaymode ! 10000+idup of Higgs decay product of the W

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


c     index of the first light coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=4
      max_flav = 5
      flst_nborn=0
      flst_nreal=0
      call vbf_h_particle_identif(HWW,HZZ)


      goto 1234

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      j=0
      do i=-2,2
         do k=-2,2
            if(i.ne.0.and.k.ne.0) then
               j=j+1
               flst_born(1,j)=i
               flst_born(2,j)=k
               flst_born(3,j)=HZZ
               flst_born(4,j)=i
               flst_born(5,j)=k

               flst_real(1,j)=i
               flst_real(2,j)=k
               flst_real(3,j)=HZZ
               flst_real(4,j)=i
               flst_real(5,j)=k
               flst_real(6,j)=0               
            endif
         enddo
      enddo
      flst_nborn=j
      do i=-2,2
         do k=-2,2
            if(i.ne.0.and.k.ne.0) then
               j=j+1
               flst_real(1,j)=0
               flst_real(2,j)=k
               flst_real(3,j)=HZZ
               flst_real(4,j)=i
               flst_real(5,j)=k
               flst_real(6,j)=-i
            endif
         enddo
      enddo
      do i=-2,2
         do k=-2,2
            if(i.ne.0.and.k.ne.0) then
               j=j+1
               flst_real(1,j)=i
               flst_real(2,j)=0
               flst_real(3,j)=HZZ
               flst_real(4,j)=i
               flst_real(5,j)=k
               flst_real(6,j)=-k
            endif
         enddo
      enddo
      flst_nreal=j
      
c      do i=1,flst_nborn
c         write(*,*) (flst_born(k,i),k=1,5)
c      enddo

      ii=1
      call vbf_h_intassign(nlegreal,flst_real(1,1),flst_real_tmp(1,ii))
      do j=2,flst_nreal         
         do k=1,ii
           if(vbf_h_flavequiv(nlegreal,flst_real(1,j),flst_real_tmp(1,k)))then
              write(*,*) 'found two equivalent real'
              write(*,*) (flst_real(jj,j),jj=1,nlegreal)
              write(*,*) (flst_real_tmp(jj,k),jj=1,nlegreal)
              goto 987
           endif
         enddo
         ii=ii+1
         call vbf_h_intassign(nlegreal,flst_real(1,j),flst_real_tmp(1,ii))
 987  continue
      enddo

      flst_nreal=ii
c now copy back to flst_real      
      do i=1,flst_nreal
         call vbf_h_intassign(nlegreal,flst_real_tmp(1,i),flst_real(1,i))
      enddo

      return


 666  continue


      flst_nborn = 1
      
c u u -> u u 
      flst_born(1,1)=2
      flst_born(2,1)=-2
      flst_born(3,1)=HZZ ! Higgs
      flst_born(4,1)=2
      flst_born(5,1)=-2
      
c      flst_born(1,2)=1
c      flst_born(2,2)=3
c      flst_born(3,2)=HZZ ! Higgs
c      flst_born(4,2)=2
c      flst_born(5,2)=2

      
      flst_nreal=3


      do i=1,5
         flst_real(i,1)=flst_born(i,1)
      enddo
      flst_real(6,1)=0 ! gluon     

      flst_real(1,2)=0
      do i=2,5
         flst_real(i,2)=flst_born(i,1)
      enddo
      flst_real(6,2)=-flst_born(1,1)


      flst_real(1,3)=flst_born(1,1)
      flst_real(2,3)=0
      do i=3,5
         flst_real(i,3)=flst_born(i,1)
      enddo
      flst_real(6,3)=-flst_born(2,1)
      
      return
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

 1234 continue



      Higgsdecay = .false.
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCC           BORN GRAPHS
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

c     consider a diagonal CKM matrix. CKM matrix elements will be added      
c     when the event is written on the Les Houches file
      CKM_diag = .true.

c     WW -> H case
      condition=.false.

      emit_Wp_upper = .true.
      emit_Wm_upper = .true.
      emit_Wp_lower = .true.
      emit_Wm_lower = .true.
 
      do i=-max_flav,max_flav
         do j=-max_flav,max_flav
            do ii=-max_flav,max_flav
               do jj=-max_flav,max_flav               
                  if (.not.((i.eq.0).or.(j.eq.0).or.
     #                    (ii.eq.0).or.(jj.eq.0))) then ! NOT a gluon
                     ferm_charge(1) = charge3(i)
                     ferm_charge(2) = charge3(j)
                     ferm_charge(3) = charge3(ii)
                     ferm_charge(4) = charge3(jj)

                     if (CKM_diag) then
                        emit_Wp_upper = (ii.eq.i-1)
                        emit_Wm_upper = (ii.eq.i+1)
                        emit_Wp_lower = (jj.eq.j-1)
                        emit_Wm_lower = (jj.eq.j+1)                 
                     endif

                     condition = 
c     W+ emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)+3)
     #                    .eq.0).and.(emit_Wp_upper)) .and.
c     W- emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)-3)
     #                    .eq.0).and.(emit_Wm_lower)))
     #                    .or.
c     W- emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)-3)
     #                    .eq.0).and.(emit_Wm_upper)) .and.
c     W+ emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)+3)
     #                    .eq.0).and.(emit_Wp_lower)))
                     
                     if (condition) then
                        flst_nborn=flst_nborn+1
                        if(flst_nborn.gt.maxprocborn) goto 999
                        flst_born(1,flst_nborn)=i
                        flst_born(2,flst_nborn)=j
                        flst_born(3,flst_nborn)=HWW ! Higgs
                        flst_born(4,flst_nborn)=ii
                        flst_born(5,flst_nborn)=jj
                        if (tag) then
                           flst_borntags(1,flst_nborn)=1
                           flst_borntags(2,flst_nborn)=2
                           flst_borntags(3,flst_nborn)=0
                           flst_borntags(4,flst_nborn)=4
                           flst_borntags(5,flst_nborn)=5
                           if (newtag) then
                              flst_borntags(1,flst_nborn)=1
                              flst_borntags(2,flst_nborn)=2
                              flst_borntags(3,flst_nborn)=0
                              flst_borntags(4,flst_nborn)=1 !4
                              flst_borntags(5,flst_nborn)=2 !5
                           endif
                        endif
                     endif
                  endif
               enddo
            enddo
         enddo
      enddo
      
      if (debug) then
         write(*,*) ' born processes: WW -> H ',flst_nborn
         do j=1,flst_nborn
            write(*,*) 'proc ',j,' ', (flst_born(k,j),k=1,nlegborn)
         enddo
      endif
      flst_nborn_WW = flst_nborn
      
c     ZZ -> H case
      do i=-max_flav,max_flav
         do j=-max_flav,max_flav
            if (.not.((i.eq.0).or.(j.eq.0))) then
               ferm_charge(1) = charge3(i)
               ferm_charge(2) = charge3(j)
               ferm_charge(3) = charge3(i)
               ferm_charge(4) = charge3(j)
                     
               flst_nborn=flst_nborn+1
               if(flst_nborn.gt.maxprocborn) goto 999
               flst_born(1,flst_nborn)=i
               flst_born(2,flst_nborn)=j
               flst_born(3,flst_nborn)=HZZ ! Higgs
               flst_born(4,flst_nborn)=i
               flst_born(5,flst_nborn)=j
               if (tag) then
                  flst_borntags(1,flst_nborn)=1
                  flst_borntags(2,flst_nborn)=2
                  flst_borntags(3,flst_nborn)=0
                  flst_borntags(4,flst_nborn)=4
                  flst_borntags(5,flst_nborn)=5
                  if (newtag) then
                     flst_borntags(1,flst_nborn)=1
                     flst_borntags(2,flst_nborn)=2
                     flst_borntags(3,flst_nborn)=0
                     flst_borntags(4,flst_nborn)=1 !4
                     flst_borntags(5,flst_nborn)=2 !5
                  endif
               endif
            endif
         enddo
      enddo
      
      if (debug) then
         write(*,*) ' born processes: ZZ -> H ',flst_nborn
         do j=flst_nborn_WW+1,flst_nborn
            write(*,*) 'proc ',j,' ', (flst_born(k,j),k=1,nlegborn)
         enddo
      endif
      

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCC                REAL GRAPHS    
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     WW -> H case
c     q q -> H q q g
      do i=-max_flav,max_flav
         do j=-max_flav,max_flav
            do ii=-max_flav,max_flav
               do jj=-max_flav,max_flav               
                  if (.not.((i.eq.0).or.(j.eq.0).or.
     #                    (ii.eq.0).or.(jj.eq.0))) then ! NOT a gluon
                     ferm_charge(1) = charge3(i)
                     ferm_charge(2) = charge3(j)
                     ferm_charge(3) = charge3(ii)
                     ferm_charge(4) = charge3(jj)

                     if (CKM_diag) then
                        emit_Wp_upper = (ii.eq.i-1)
                        emit_Wm_upper = (ii.eq.i+1)
                        emit_Wp_lower = (jj.eq.j-1)
                        emit_Wm_lower = (jj.eq.j+1)                 
                     endif
                     
                     condition = 
c     W+ emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)+3)
     #                    .eq.0).and.(emit_Wp_upper)) .and.
c     W- emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)-3)
     #                    .eq.0).and.(emit_Wm_lower)))
     #                    .or.
c     W- emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)-3)
     #                    .eq.0).and.(emit_Wm_upper)) .and.
c     W+ emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)+3)
     #                    .eq.0).and.(emit_Wp_lower)))
                     
                     if (condition) then
                        flst_nreal=flst_nreal+1
                        if(flst_nreal.gt.maxprocreal) goto 998
                        flst_real(1,flst_nreal)=i
                        flst_real(2,flst_nreal)=j
                        flst_real(3,flst_nreal)=HWW ! Higgs
                        flst_real(4,flst_nreal)=ii
                        flst_real(5,flst_nreal)=jj
                        flst_real(6,flst_nreal)=0 ! gluon
                        if (tag) then
                           flst_realtags(1,flst_nreal)=1
                           flst_realtags(2,flst_nreal)=2
                           flst_realtags(3,flst_nreal)=0
                           flst_realtags(4,flst_nreal)=4
                           flst_realtags(5,flst_nreal)=5
                           flst_realtags(6,flst_nreal)=0
                           if (newtag) then
                              flst_realtags(1,flst_nreal)=1
                              flst_realtags(2,flst_nreal)=2
                              flst_realtags(3,flst_nreal)=0
                              flst_realtags(4,flst_nreal)=1 !4
                              flst_realtags(5,flst_nreal)=2 !5
                              flst_realtags(6,flst_nreal)=0
                           endif
                        endif
                     endif
                  endif
               enddo
            enddo
         enddo
      enddo
c     g q -> H q q q
c     loop on only HALF of the incoming upper-line quark, not to double count!
c     In fact, the real-radiation term contains TWO Feynman diagrams.
      do i=1,max_flav
         do j=-max_flav,max_flav
            do ii=-max_flav,max_flav
               do jj=-max_flav,max_flav               
                  if (.not.((i.eq.0).or.(j.eq.0).or.
     #                    (ii.eq.0).or.(jj.eq.0))) then ! NOT a gluon
                     ferm_charge(1) = 0
                     ferm_charge(2) = charge3(j)
                     ferm_charge(3) = charge3(ii)
                     ferm_charge(4) = charge3(jj)
                     ferm_charge(5) = charge3(-i)

                     if (CKM_diag) then
                        emit_Wp_upper = (ii.eq.i-1)
                        emit_Wm_upper = (ii.eq.i+1)
                        emit_Wp_lower = (jj.eq.j-1)
                        emit_Wm_lower = (jj.eq.j+1)                 
                     endif

                     condition = 
c     W+ emission from upper leg                        
     #                    (((-ferm_charge(5)-(ferm_charge(3)+3)
     #                    .eq.0).and.(emit_Wp_upper)) .and.
c     W- emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)-3)
     #                    .eq.0).and.(emit_Wm_lower)))
     #                    .or.
c     W- emission from upper leg                        
     #                    (((-ferm_charge(5)-(ferm_charge(3)-3)
     #                    .eq.0).and.(emit_Wm_upper)) .and.
c     W+ emission from lower leg                        
     #                    ((ferm_charge(2)-(ferm_charge(4)+3)
     #                    .eq.0).and.(emit_Wp_lower)))
                     
                     if (condition) then
                        flst_nreal=flst_nreal+1
                        if(flst_nreal.gt.maxprocreal) goto 998
                        flst_real(1,flst_nreal)=0 ! gluon
                        flst_real(2,flst_nreal)=j
                        flst_real(3,flst_nreal)=HWW ! Higgs
                        flst_real(4,flst_nreal)=ii
                        flst_real(5,flst_nreal)=jj
                        flst_real(6,flst_nreal)=-i
                        if (tag) then
                           flst_realtags(1,flst_nreal)=0
                           flst_realtags(2,flst_nreal)=2
                           flst_realtags(3,flst_nreal)=0
                           flst_realtags(4,flst_nreal)=4
                           flst_realtags(5,flst_nreal)=5
                           flst_realtags(6,flst_nreal)=1
                           if (newtag) then
                              flst_realtags(1,flst_nreal)=0
                              flst_realtags(2,flst_nreal)=2
                              flst_realtags(3,flst_nreal)=0
                              flst_realtags(4,flst_nreal)=1 !4
                              flst_realtags(5,flst_nreal)=2 !5
                              flst_realtags(6,flst_nreal)=1
                           endif
                        endif
                     endif
                  endif
               enddo
            enddo
         enddo
      enddo

c     q g -> H q q q
c     loop on only HALF of the incoming lower-line quark, not to double count!
c     In fact, the real-radiation term contains TWO Feynman diagrams.
      do i=-max_flav,max_flav
         do j=1,max_flav             
            do ii=-max_flav,max_flav
               do jj=-max_flav,max_flav               
                  if (.not.((i.eq.0).or.(j.eq.0).or.
     #                    (ii.eq.0).or.(jj.eq.0))) then ! NOT a gluon
                     ferm_charge(1) = charge3(i)
                     ferm_charge(2) = 0
                     ferm_charge(3) = charge3(ii)
                     ferm_charge(4) = charge3(jj)
                     ferm_charge(5) = charge3(-j)

                     if (CKM_diag) then
                        emit_Wp_upper = (ii.eq.i-1)
                        emit_Wm_upper = (ii.eq.i+1)
                        emit_Wp_lower = (jj.eq.j-1)
                        emit_Wm_lower = (jj.eq.j+1)                 
                     endif
                     
                     condition = 
c     W+ emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)+3)
     #                    .eq.0).and.(emit_Wp_upper)) .and.
c     W- emission from lower leg                        
     #                    ((-ferm_charge(5)-(ferm_charge(4)-3)
     #                    .eq.0).and.(emit_Wm_lower)))
     #                    .or.
c     W- emission from upper leg                        
     #                    (((ferm_charge(1)-(ferm_charge(3)-3)
     #                    .eq.0).and.(emit_Wm_upper)) .and.
c     W+ emission from lower leg                        
     #                    ((-ferm_charge(5)-(ferm_charge(4)+3)
     #                    .eq.0).and.(emit_Wp_lower)))
                     if (condition) then
                        flst_nreal=flst_nreal+1
                        if(flst_nreal.gt.maxprocreal) goto 998
                        flst_real(1,flst_nreal)=i
                        flst_real(2,flst_nreal)=0 ! gluon
                        flst_real(3,flst_nreal)=HWW ! Higgs
                        flst_real(4,flst_nreal)=ii
                        flst_real(5,flst_nreal)=jj
                        flst_real(6,flst_nreal)=-j
                        if (tag) then
                           flst_realtags(1,flst_nreal)=1
                           flst_realtags(2,flst_nreal)=0
                           flst_realtags(3,flst_nreal)=0
                           flst_realtags(4,flst_nreal)=4
                           flst_realtags(5,flst_nreal)=5
                           flst_realtags(6,flst_nreal)=2
                           if (newtag) then
                              flst_realtags(1,flst_nreal)=1
                              flst_realtags(2,flst_nreal)=0
                              flst_realtags(3,flst_nreal)=0
                              flst_realtags(4,flst_nreal)=1 !4
                              flst_realtags(5,flst_nreal)=2 !5
                              flst_realtags(6,flst_nreal)=2
                           endif                           
                        endif
                     endif
                  endif
               enddo
            enddo
         enddo
      enddo              


      if (debug) then
         write(*,*) ' real processes: WW -> H ',flst_nreal
         do j=1,flst_nreal
            write(*,*) 'proc ',j,' ',(flav(flst_real(k,j)),k=1,nlegreal)
         enddo
      endif
      flst_nreal_WW = flst_nreal


c     ZZ -> H case
c     q q -> H q q g
      do i=-max_flav,max_flav
         do j=-max_flav,max_flav
            if (.not.((i.eq.0).or.(j.eq.0))) then
               ferm_charge(1) = charge3(i)
               ferm_charge(2) = charge3(j)
               ferm_charge(3) = charge3(i)
               ferm_charge(4) = charge3(j)
                     
               flst_nreal=flst_nreal+1
               
               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=i
               flst_real(2,flst_nreal)=j
               flst_real(3,flst_nreal)=HZZ ! Higgs
               flst_real(4,flst_nreal)=i
               flst_real(5,flst_nreal)=j
               flst_real(6,flst_nreal)=0 ! gluon
               if (tag) then
                  flst_realtags(1,flst_nreal)=1
                  flst_realtags(2,flst_nreal)=2
                  flst_realtags(3,flst_nreal)=0
                  flst_realtags(4,flst_nreal)=4
                  flst_realtags(5,flst_nreal)=5
                  flst_realtags(6,flst_nreal)=0
                  if (newtag) then
                     flst_realtags(1,flst_nreal)=1
                     flst_realtags(2,flst_nreal)=2
                     flst_realtags(3,flst_nreal)=0
                     flst_realtags(4,flst_nreal)=1 !4
                     flst_realtags(5,flst_nreal)=2 !5
                     flst_realtags(6,flst_nreal)=0
                  endif
               endif
            endif
         enddo
      enddo
c     g q -> H q q q
c     loop on only HALF of the incoming upper-line quark, not to double count!
c     In fact, the real-radiation term contains TWO Feynman diagrams.
      do i=1,max_flav
         do j=-max_flav,max_flav
            if (.not.((i.eq.0).or.(j.eq.0))) then
               ferm_charge(1) = 0
               ferm_charge(2) = charge3(j)
               ferm_charge(3) = charge3(i)
               ferm_charge(4) = charge3(j)
               ferm_charge(5) = -charge3(i)
               
               flst_nreal=flst_nreal+1

               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=0 ! gluon
               flst_real(2,flst_nreal)=j
               flst_real(3,flst_nreal)=HZZ ! Higgs
               flst_real(4,flst_nreal)=i
               flst_real(5,flst_nreal)=j
               flst_real(6,flst_nreal)=-i
               if (tag) then
                  flst_realtags(1,flst_nreal)=0
                  flst_realtags(2,flst_nreal)=2
                  flst_realtags(3,flst_nreal)=0
                  flst_realtags(4,flst_nreal)=4
                  flst_realtags(5,flst_nreal)=5
                  flst_realtags(6,flst_nreal)=1
                  if (newtag) then
                     flst_realtags(1,flst_nreal)=0
                     flst_realtags(2,flst_nreal)=2
                     flst_realtags(3,flst_nreal)=0
                     flst_realtags(4,flst_nreal)=1 !4
                     flst_realtags(5,flst_nreal)=2 !5
                     flst_realtags(6,flst_nreal)=1
                  endif
               endif
            endif
         enddo
      enddo

c     q g -> H q q q
c     loop on only HALF of the incoming lower-line quark, not to double count!
c     In fact, the real-radiation term contains TWO Feynman diagrams.
      do i=-max_flav,max_flav
         do j=1,max_flav  
            if (.not.((i.eq.0).or.(j.eq.0))) then
               ferm_charge(1) = charge3(i)
               ferm_charge(2) = 0
               ferm_charge(3) = charge3(i)
               ferm_charge(4) = charge3(j)
               ferm_charge(5) = -charge3(j)
               
               flst_nreal=flst_nreal+1

               if(flst_nreal.gt.maxprocreal) goto 998
               flst_real(1,flst_nreal)=i
               flst_real(2,flst_nreal)=0 ! gluon
               flst_real(3,flst_nreal)=HZZ ! Higgs
               flst_real(4,flst_nreal)=i
               flst_real(5,flst_nreal)=j
               flst_real(6,flst_nreal)=-j
               if (tag) then
                  flst_realtags(1,flst_nreal)=1
                  flst_realtags(2,flst_nreal)=0
                  flst_realtags(3,flst_nreal)=0
                  flst_realtags(4,flst_nreal)=4
                  flst_realtags(5,flst_nreal)=5
                  flst_realtags(6,flst_nreal)=2
                  if (newtag) then
                     flst_realtags(1,flst_nreal)=1
                     flst_realtags(2,flst_nreal)=0
                     flst_realtags(3,flst_nreal)=0
                     flst_realtags(4,flst_nreal)=1 !4
                     flst_realtags(5,flst_nreal)=2 !5
                     flst_realtags(6,flst_nreal)=2
                  endif
               endif
            endif
         enddo
      enddo              

      if (debug) then
         write(*,*) ' real processes: ZZ -> H ',flst_nreal-flst_nreal_WW
         do j=flst_nreal_WW+1,flst_nreal
            write(*,*) 'proc ',j-flst_nreal_WW,' ',
     #           (flav(flst_real(k,j)),k=1,nlegreal)
         enddo
      endif

c      stop


      return
 998  write(*,*) 'vbf_h_init_processes: increase maxprocreal:',maxprocreal
      stop
 999  write(*,*) 'vbf_h_init_processes: increase maxprocborn:',maxprocborn
      stop
      end
      


