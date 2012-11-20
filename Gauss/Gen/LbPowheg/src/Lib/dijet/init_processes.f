      subroutine dijet_init_processes
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_flg.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_par.h'
      include 'src/Lib/dijet/pwhg_rad.h'
      integer i1,i2,i3,i4,i5,k,ii(nlegreal)
      equivalence (i1,ii(1)),(i2,ii(2)),(i3,ii(3)),
     #  (i4,ii(4)),(i5,ii(5))
      logical debug
      parameter (debug=.false.)
      integer j,tmpfl(5)
      logical condition,newcond
      logical dijet_flavequiv
      external dijet_flavequiv
      real * 8 dijet_powheginput,tmp
      external dijet_powheginput
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C  DON'T FORGET TO  SET THIS TO THE NUMBER OF FLAVOURS REQUIRED 
      integer nflav
      parameter (nflav=5)
C  AND CHANGE THE madgraph SYMBOLIC LINK ACCORDINGLY
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
c     check nlegborn. This is only a sanity check while we are TESTING 
c     the code and we change often from one process to the other
      if (nlegborn.ne.4) then
         write(*,*) ' ERROR: set nlegborn to the appropriate value'
         write(*,*) ' for this process in nlegborn.h'
         stop
      endif

c This are set to 1 by default. For jets, we need to change them
      par_diexp = 2
      par_dijexp= 2
      par_2gsupp=2
      tmp=dijet_powheginput('#par_diexp')
      if(tmp.gt.0) then
         par_diexp = tmp
         par_dijexp= tmp
      endif
      tmp=dijet_powheginput('#par_2gsupp')
      if(tmp.gt.0) then
         par_2gsupp = tmp
      endif
      tmp=dijet_powheginput('#jacsing')
      if(tmp.eq.1) flg_jacsing=.true.
c the default value, 1d-6, causes problems in HELAC routines
      par_isrtinycsi = 1d-4
      par_isrtinyy = 1d-4
      par_fsrtinycsi = 1d-4
      par_fsrtinyy = 1d-4
c Overraid generl default; make 1 the default
      rad_iupperfsr=dijet_powheginput("#iupperfsr")
      if(rad_iupperfsr.lt.0) rad_iupperfsr=1      
*********************************************************************
c     number of light flavors
      st_nlight = nflav
*********************************************************************
*********************************************************************
c     index of the first LIGHT coloured parton in the final state
      flst_lightpart=3
*********************************************************************

*********************************************************************
***********            BORN SUBPROCESSES              ***************
*********************************************************************
      flst_nborn=0
      do i1=-nflav,nflav
         do i2=-nflav,nflav
            do i3=-nflav,nflav
               do i4=-nflav,nflav
                  condition=(i1+i2).eq.(i3+i4)
                  condition=condition.and.((
     $                 ((i1.eq.i3).or.(i1.eq.i4))
     $                 .and.
     $                 (i2.eq.i3).or.(i2.eq.i4))
     $                 .or.
     $             ((abs(i1).eq.abs(i2)).and.(abs(i3).eq.abs(i4))))
                  tmpfl(1)=i1
                  tmpfl(2)=i2
                  tmpfl(3)=-i3
                  tmpfl(4)=-i4
                  do j=1,4
                     if(tmpfl(j).ne.0) then
                        do k=j+1,4
                           if(tmpfl(k).eq.-tmpfl(j)) then
                              tmpfl(k)=0
                              tmpfl(j)=0
                              goto 33
                           endif
                        enddo
                     endif
 33                  continue
                  enddo
                  newcond=tmpfl(1).eq.0.and.tmpfl(2).eq.0.and.
     1                    tmpfl(3).eq.0.and.tmpfl(4).eq.0
                  if(newcond.eqv.condition) then
                     continue
                  else
                     write(*,*) ' HAHAHHAH!'
                  endif
                  if(condition) then
                     do j=1,flst_nborn
c     Check that an inequivalent configuration is generated
                        if(dijet_flavequiv(nlegborn,flst_born(1,j),ii(1)))
     $                       goto 20
                     enddo
                     flst_nborn=flst_nborn+1
                     if(flst_nborn.gt.maxprocborn) goto 999
                     do k=1,nlegborn
                        flst_born(k,flst_nborn)=ii(k)
                     enddo
 20                  continue
                  endif
               enddo
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
***********             REAL SUBPROCESSES              ***************
*********************************************************************
      flst_nreal=0
      condition=.false.
      do i1=-nflav,nflav
         do i2=-nflav,nflav
            do i3=-nflav,nflav
               do i4=-nflav,nflav 
                  do i5=-nflav,nflav
                     condition=(i1+i2).eq.(i3+i4+i5)
                     condition=condition.and.((three_ch(i1) +
     $                    three_ch(i2)).eq. (three_ch(i3) + three_ch(i4)
     $                    + three_ch(i5)))
                     condition=condition.and.(((((i1.eq.i3).or.(i1.eq.i4
     $                    ).or.(i1.eq.i5)) .or. (i2.eq.i3).or.(i2.eq.i4
     $                    ).or.(i2.eq.i5))).or.((i1+i2.eq.0).and.(i3+i4
     $                    +i5.eq.0)))
                     tmpfl(1)=i1
                     tmpfl(2)=i2
                     tmpfl(3)=-i3
                     tmpfl(4)=-i4
                     tmpfl(5)=-i5
                     do j=1,5
                        if(tmpfl(j).ne.0) then
                           do k=j+1,5
                              if(tmpfl(k).eq.-tmpfl(j)) then
                                 tmpfl(k)=0
                                 tmpfl(j)=0
                                 goto 34
                              endif
                           enddo
                        endif
 34                     continue
                     enddo
                     newcond=tmpfl(1).eq.0.and.tmpfl(2).eq.0.and.
     1                    tmpfl(3).eq.0.and.tmpfl(4).eq.0.and.
     2                    tmpfl(5).eq.0
                     if(newcond.eqv.condition) then
                        continue
                     else
                        write(*,*) 'real HAHAHHAH!'
                     endif
                     if(condition) then
                        do j=1,flst_nreal
c     Check that an inequivalent configuration is generated
                           if(dijet_flavequiv(nlegreal,flst_real(1,j),ii(1)))
     $                          goto 10
                        enddo
                        flst_nreal=flst_nreal+1
                        if(flst_nreal.gt.maxprocreal) goto 998
                        do k=1,nlegreal
                           flst_real(k,flst_nreal)=ii(k)
                        enddo
 10                     continue
                     endif
                  enddo
               enddo
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
 998  write(*,*) 'dijet_init_processes: increase maxprocreal'
      stop
 999  write(*,*) 'dijet_init_processes: increase maxprocborn'
      stop
      end
 
 
