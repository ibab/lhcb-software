      subroutine st_tch_init_processes
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/pwhg_flst.h'
      include 'src/Lib/st_tch/pwhg_kn.h'
      include 'src/Lib/st_tch/LesHouches.h'
      include 'src/Lib/st_tch/PhysPars.h'
      
      integer ipl,imn,i1,i2
      integer nflav
      parameter (nflav=5)

      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      character *3 qcd_name(-6:6)
      data qcd_name /'tx ','bx ','cx ','sx ','ux ','dx ',
     #         'g  ','d  ','u  ','s  ','c  ','b  ','t  '/

      logical debug
      parameter (debug=.false.)

      logical condition_b_t,
     #condition_blike_t,condition_g1_t,condition_g2_t,condition_t 

      integer j,k
      real *8 st_tch_powheginput
      external st_tch_powheginput

      lprup(1)=10000


ccccccccccccccccccccccccccccccc
c     decide t or tbar process
      ttype=st_tch_powheginput('ttype')
      if(abs(ttype).ne.1) then
         write(*,*) 'Unrecognised ttype in input file'
         write(*,*) 'admitted st_tch_values: 1 for t, -1 for tbar'
         call exit(1)
      endif
ccccccccccccccccccccccccccccccc      


cccccccccccccccccccccccccccccc
c     t-channel subprocesses
cccccccccccccccccccccccccccccc


cccccccccccccccccccc
c     index of the first MASSLESS coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=4
cccccccccccccccccccc




*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
      write(*,*) 'POWHEG: looking for Born amplitudes'
      flst_nborn=0
      do ipl=-nflav,nflav
         do imn=-nflav,nflav
            do i1=-nflav,nflav
               condition_b_t=.false.
c     flavour conditions to yield a valid amplitude
               condition_b_t=(
     #         !t channel
     #          (((three_ch(ipl)-three_ch(6)).eq.-3).and.
     #         ((three_ch(imn)-three_ch(i1)).eq.3)).or.
     #         !t channel (exchange ipl-imn)          
     #          (((three_ch(imn)-three_ch(6)).eq.-3).and.
     #         ((three_ch(ipl)-three_ch(i1)).eq.3)))    

               if(condition_b_t) then
                  flst_nborn=flst_nborn+1
                  if(flst_nborn.gt.maxprocborn) goto 999
                  flst_born(1,flst_nborn)=ipl
                  flst_born(2,flst_nborn)=imn
                  flst_born(3,flst_nborn)=6
                  flst_born(4,flst_nborn)=i1
               endif
            enddo
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
      write(*,*) 'POWHEG: looking for real amplitudes'
      flst_nreal=0
      do ipl=-nflav,nflav
         do imn=-nflav,nflav
            do i1=-nflav,nflav
               do i2=-nflav,nflav
c     require final state partons to be sorted: gluons last, increasing flavours.
c     top always in position 3 (first outgoing particle)
                  if(i2.eq.0.or.(i1.ne.0.and.i2.ge.i1)) then

c     condition_blike_t=.false.
                     condition_g1_t=.false.
                     condition_g2_t=.false.
                     
c     flavour conditions to yield a valid amplitude

c     born-like + outgoing gluon (leg 5, i.e. i2=0)
                     condition_blike_t=(i2.eq.0).and.
     #                          !t channel
     #((((three_ch(ipl)-three_ch(6)).eq.-3).and.
     #((three_ch(imn)-three_ch(i1)).eq.3)).or.
     #                          !t channel (exchange ipl-imn)          
     #(((three_ch(imn)-three_ch(6)).eq.-3).and.
     #((three_ch(ipl)-three_ch(i1)).eq.3)))
                     
c     if a gluon is ingoing, we use the following conditions to
c     find real processes qg or gq: we ask for:
c     1) charge conservation
c     2a) s-channel: top+i1=W (or top+i2=W)
c     2b) t-channel: top-W=ip (or top-W=im)
c     3) no outgoing gluons
c     4) no gg processes.
                     condition_g1_t=((ipl.eq.0).and.
     #(three_ch(imn).eq.(three_ch(6)+three_ch(i1)+three_ch(i2))).and.
     #(((three_ch(i1)+3).eq.three_ch(imn)).or.
     #(((three_ch(i2)+3).eq.three_ch(imn))).or.
     #(((three_ch(6)-3).eq.three_ch(imn)))).and.                
     #(.not.(i1.eq.0.or.i2.eq.0)).and.
     #(.not.(ipl.eq.0.and.imn.eq.0)))

                     condition_g2_t=((imn.eq.0).and.
     #(three_ch(ipl).eq.(three_ch(6)+three_ch(i1)+three_ch(i2))).and.
     #(((three_ch(i1)+3).eq.three_ch(ipl)).or.
     #(((three_ch(i2)+3).eq.three_ch(ipl))).or.
     #(((three_ch(6)-3).eq.three_ch(ipl)))).and.                
     #(.not.(i1.eq.0.or.i2.eq.0)).and.
     #(.not.(ipl.eq.0.and.imn.eq.0)))

                     condition_t=condition_blike_t.or.condition_g1_t.or.
     #condition_g2_t

                     if(condition_t) then
c     check that the conditions of t-channel are reciprocally exclusive
                        if(condition_blike_t.and.condition_g1_t.or.
     #condition_blike_t.and.condition_g2_t.or.
     #condition_g1_t.and.condition_g2_t) then
                           write(*,*) 
     #'problem in looking for real subprocesses (t-ch)'
                           call exit(1)
                        endif

                        flst_nreal=flst_nreal+1
                        if(flst_nreal.gt.maxprocreal) goto 998
                        
                        flst_real(1,flst_nreal)=ipl
                        flst_real(2,flst_nreal)=imn
                        flst_real(3,flst_nreal)=6

                        if(condition_blike_t) then
                        !outgoing legs are sorted with a rule (gluon last)
                           flst_real(4,flst_nreal)=i1
                           flst_real(5,flst_nreal)=i2
                        elseif(condition_g1_t.or.condition_g2_t) then 
                        !If possible, sort outgoing legs with leg 3 and 5 coming from 
                        !a W+ vertex.
                        !Otherwise it means that legs 4 and 5 come from a W- vertex.
                        !In this last case sort with increasing flavour
                           if((three_ch(i1)+three_ch(6)).eq.3) then
                              flst_real(5,flst_nreal)=i1
                              flst_real(4,flst_nreal)=i2
                           elseif((three_ch(i2)+three_ch(6)).eq.3) then
                              flst_real(5,flst_nreal)=i2
                              flst_real(4,flst_nreal)=i1
                           elseif((three_ch(i1)+three_ch(i2)).eq.-3)then
                              flst_real(5,flst_nreal)=i2
                              flst_real(4,flst_nreal)=i1
                           else
                              write(*,*) 'error in sorting flav_real'
                        write(*,*) three_ch(6),three_ch(i1),three_ch(i2)
                              call exit(1)
                           endif 
                        else
                           write(*,*) 'error in prepareids_singlet'
                           call exit(1)
                        endif
                     endif
                  endif
               enddo
            enddo
         enddo
      enddo
      if (debug) then
         write(*,*) ' real processes',flst_nreal
         do j=1,flst_nreal
            write(*,*) (flst_real(k,j),k=1,nlegreal)
c$$$            if(flst_real(4,j).gt.flst_real(5,j)) then
c$$$               write(*,*) '                 |__ reverse order'
c$$$            endif
         enddo
      endif


ccccccccccccccccccccccccccccccccccccccc
c     charge conjugation
      if(ttype.eq.-1) then
         do j=1,flst_nborn
            flst_born(1,j)=-flst_born(1,j)
            flst_born(2,j)=-flst_born(2,j)
            flst_born(3,j)=-flst_born(3,j)
            flst_born(4,j)=-flst_born(4,j)
         enddo
         do j=1,flst_nreal
            flst_real(1,j)=-flst_real(1,j)
            flst_real(2,j)=-flst_real(2,j)
            flst_real(3,j)=-flst_real(3,j)
            flst_real(4,j)=-flst_real(4,j)
            flst_real(5,j)=-flst_real(5,j)
         enddo
      endif
cccccccccccccccccccccccccccccccccccccc

      return
 998  write(*,*) 'st_tch_init_processes: increase maxprocreal'
      call exit(1)
 999  write(*,*) 'st_tch_init_processes: increase maxprocborn'
      call exit(1)
      end
 

