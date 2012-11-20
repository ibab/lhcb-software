c      program testfindregions
c      implicit none
c      integer n
c      parameter (n=6)
c      integer rflav(n),nregions,iregions(2,n*(n-1)/2),i
c      character  * 30 process
c
c     process = 'u~ u  -> e- e+ u  u~'
c     call w_from_madgraph_to_number(process,rflav) 
c     call w_find_regions(n,rflav,nregions,iregions)
c      
c     write(*,*) nregions
c     do i=1,nregions
c        write(*,*) iregions(1,i),iregions(2,i)
c     enddo
c      call w_genflavreglist
c      end



      subroutine w_mapflavours
c Sometimes it is convenient to treat lines with the same
c flavour as if they were different. In particular, in Higgs
c production by vector boson fusion, it is convenient to treat
c the lines attatched to the vector bosons as if they were all
c different. This is done by setting the arrays
c flst_realtags(nlegreal,flst_nreal) and flst_borntags(nlegborn,flst_nborn)
c to an integer labeling the particular fermion line.
c If the flst_borntags and flst_realtags are set, lines with different
c tags are treated as different lines from the point of view of finding
c the singular regions.
c
c The routine w_mapflavours changes the flavour value of each line in such
c a way that all lines that should be treated as different get a unique
c internal flavour number. The mapping between internal flavour number
c and real flavour and tags is held in the arrays fllist (flavour list),
c taglist (list of corresponding tags) and intfl (internal flavour list).
c
c Once the regions are all found, the internal flavour numbers are replaced
c by the original flavour numbers (routine unmapflavour)
c
      implicit none
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer fllist(nlegreal*maxprocreal),
     1    taglist(nlegreal*maxprocreal),intfl(nlegreal*maxprocreal),
     2    nflmap
      common/w_cmapflavours/fllist,taglist,intfl,nflmap
      integer l,ip,nflmapsav,k
      logical debug
      parameter(debug=.false.)
      nflmap=0
      do ip=1,flst_nreal
         do l=1,nlegreal
            call w_addflavour(flst_real(l,ip),flst_realtags(l,ip))
         enddo
      enddo
c now do the same with Born terms; nflmap should not change;
      nflmapsav=nflmap
      do ip=1,flst_nborn
         do l=1,nlegborn
            call w_addflavour(flst_born(l,ip),flst_borntags(l,ip))
         enddo
      enddo
      if(nflmap.gt.nflmapsav) then
         write(*,*) ' found Born flavour not present in real graphs'
         stop
      endif
      if (debug) then 
         write(*,*) ' flavour mapping'
         do k=1,nflmap
            write(*,*) fllist(k),taglist(k),intfl(k)
         enddo
         write(*,*) ' end flavour mapping'
      endif
      end

      subroutine w_unmapflavours
c Replace internal flavour numbers with real ones
c in all relevant flavour arrays
      implicit none
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      call w_unmaparr(flst_nreal,nlegreal,flst_real)
      call w_unmaparr(flst_nalr,nlegreal,flst_alr)
      call w_unmaparr(flst_nregular,nlegreal,flst_regular)
      call w_unmaparr(flst_nalr,nlegborn,flst_uborn)
      call w_unmaparr(flst_nborn,nlegborn,flst_born)
      end
      
      subroutine w_unmaparr(n,nlegs,arr)
      implicit none
      integer n,nlegs,arr(nlegs,n)
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer fllist(nlegreal*maxprocreal),
     1    taglist(nlegreal*maxprocreal),intfl(nlegreal*maxprocreal),
     2    nflmap
      common/w_cmapflavours/fllist,taglist,intfl,nflmap
      integer ileg,iproc,w_getrealflav
      external w_getrealflav
      do iproc=1,n
         do ileg=1,nlegs
            arr(ileg,iproc)=w_getrealflav(arr(ileg,iproc))
         enddo
      enddo
      end

      function w_getrealflav(fl)
      implicit none
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer w_getrealflav,fl
      integer fllist(nlegreal*maxprocreal),
     1    taglist(nlegreal*maxprocreal),intfl(nlegreal*maxprocreal),
     2    nflmap
      common/w_cmapflavours/fllist,taglist,intfl,nflmap
      integer ifl
      if(fl.eq.0) then
         w_getrealflav=0
         return
      endif
      do ifl=1,nflmap
         if(abs(fl).eq.intfl(ifl)) then
            w_getrealflav=fllist(ifl)*(fl/abs(fl))
            return
         endif
      enddo
      write(*,*) ' internal flavour not found in list'
      stop
      end

      subroutine w_addflavour(flav,tag)
c look if the pair flav, tag is already in the list fllist,taglist.
c If so, replace flav with the corresponding intfl. If it corresponds
c to the antiparticle of something in the list, replace it with minus
c the corresponding intfl.
      implicit none
      integer flav,tag
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer fllist(nlegreal*maxprocreal),
     1    taglist(nlegreal*maxprocreal),intfl(nlegreal*maxprocreal),
     2    nflmap
      common/w_cmapflavours/fllist,taglist,intfl,nflmap
      integer jfl
      if(flav.eq.0) then
         if(tag.ne.0) then
            write(*,*) ' w_addflavour: should not tag a gluon!'
            stop
         endif
      endif
      do jfl=1,nflmap
         if(flav.eq.fllist(jfl).and.tag.eq.taglist(jfl)) then
            flav=intfl(jfl)
            return
         elseif(flav.eq.-fllist(jfl).and.tag.eq.taglist(jfl)) then
            flav=-intfl(jfl)
            return
         endif
      enddo
c     new flavour to add
      nflmap=nflmap+1
      taglist(nflmap)=tag
      fllist(nflmap)=abs(flav)
      intfl(nflmap)=abs(flav)+tag*10000
      if(flav.eq.0) return
c see if this value is already in use; make it unique
 10   continue
      do jfl=1,nflmap-1
         if(intfl(nflmap).eq.intfl(jfl)) then
            intfl(nflmap)=intfl(nflmap)+1000
            if(intfl(nflmap).gt.1000000000) then
               write(*,*) ' cannot make unique id!'
               stop
            endif
            goto 10
         endif
      enddo
c     now it is unique
      flav=intfl(nflmap)*(flav/abs(flav))
      end
                  

      subroutine w_find_regions(nleg,rflav,nregions,iregions)
c the process has n particles, 1,2: initial, > 2 final
c integer rflav(n):  flavour of particles; 1,2: incoming flavour,
c                    > 2 outgoing flavour.
c                    Particles are labelled with PDG number scheme,
c                    EXCEPT for gluons, that are numbered ZERO
c                    (instead of 21)
c
c It returns:
c integer nregions
c integer iregion(2,nregions): the indices of particles forming singular
c                              regions i,j (i<j).
c                              For initial state singularities, if the
c                              emitter can be both of the initial state 
c                              particles, 
c                              and if the radiated particle is a gluon,
c                              only one region is generated with first
c                              index equal to zero.
c It calls: logical w_validBorn(n-1,bflav), that returns true if the flavour
c                                         configuration bflav admits
c                                         a non-vanishing Born amplitude.
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer nleg,rflav(nleg),nregions,iregions(2,maxregions)
      logical ireg(2)
      logical w_validBorn
      external w_validBorn
      integer i,j,k,ibornfl,iborn,bflav(100)
      nregions=0
c final state regions
      do i=flst_lightpart,nleg
         do j=i+1,nleg
c find if they can arise from the same splitting
            if(rflav(i)+rflav(j).eq.0) then
c     It is g-> q qbar or g->gg: ibornfl is a gluon
               ibornfl=0
            elseif(rflav(i).eq.0) then
               ibornfl=rflav(j)
            elseif(rflav(j).eq.0) then
               ibornfl=rflav(i)
            else
c cannot come from the same splitting
               goto 10
            endif
c build the underlying born flavour structure in bflav
            iborn=0
            do k=1,nleg
               if(k.eq.i) then
                  iborn=iborn+1
                  bflav(iborn)=ibornfl
               elseif(k.ne.j) then
                  iborn=iborn+1
                  bflav(iborn)=rflav(k)
               endif
            enddo
            if(w_validBorn(nleg-1,bflav)) then
               nregions=nregions+1
               iregions(1,nregions)=i
               iregions(2,nregions)=j
            endif
 10         continue
         enddo
      enddo
c initial state region
      do j=flst_lightpart,nleg
         do i=1,2
            ireg(i)=.false.
            if(rflav(j).eq.rflav(i)) then
               ibornfl=0
            elseif(rflav(j).eq.0) then
               ibornfl=rflav(i)
            elseif(rflav(i).eq.0) then
               ibornfl=-rflav(j)
            else
               goto 11
            endif
            iborn=0
            do k=1,nleg
               if(k.eq.i) then
                  iborn=iborn+1
                  bflav(iborn)=ibornfl
               elseif(k.ne.j) then
                  iborn=iborn+1
                  bflav(iborn)=rflav(k)
               endif
            enddo
            if(w_validBorn(nleg-1,bflav)) then
               ireg(i)=.true.
            endif
 11         continue
         enddo
         if(ireg(1).and.ireg(2).and.rflav(j).eq.0) then
c if both regions are singular and the radiated parton is a gluon
c emit a single region with emitter 0
            nregions=nregions+1
            iregions(1,nregions)=0
            iregions(2,nregions)=j
         else
            if(ireg(1)) then
               nregions=nregions+1
               iregions(1,nregions)=1
               iregions(2,nregions)=j
            endif
            if(ireg(2)) then
               nregions=nregions+1
               iregions(1,nregions)=2
               iregions(2,nregions)=j
            endif
         endif
      enddo
      end

      subroutine w_ubornflav(n,j,rflav,bflav)
c finds the underlying Born flavour
c integer n: number of legs in real graph
c integer rflav(n): flavours of legs in real graph
c                   (1 and 2 incoming)
c integer j:        singularity in region j,n
c                   j=0 (1 and 2), j=1, j=2: initial state sing.
c                   j>2 final state sing.
      integer n,j,rflav(n),bflav(n-1)
      integer ibornfl
      if(j.eq.0) then
         if(rflav(n).ne.0) goto 998
      elseif(j.gt.2) then
         if(rflav(j)+rflav(n).ne.0.and.rflav(n).ne.0.) goto 998
      else
         if(rflav(j)-rflav(n).ne.0.and.rflav(n)*rflav(j).ne.0.) goto 998
      endif
      if(j.eq.0) then
         continue
      elseif(j.gt.2) then
         ibornfl=rflav(j)+rflav(n)
      else
         ibornfl=rflav(j)-rflav(n)
      endif
      do l=1,n-1
         if(l.eq.j) then
            bflav(l)=ibornfl
         else
            bflav(l)=rflav(l)
         endif
      enddo
      return
 998  continue
      write(*,*) ' w_ubornflav: error'
      write(*,*) ' rflav:',rflav
      write(*,*) ' j: ',j
      stop
      end

      function w_flavequiv(n,aflav,bflav)
c returns true if the flavour structures aflav and bflav are
c equivalent up to a permutation of the final state lines,
c false otherwise.
      implicit none
      logical w_flavequiv
      integer n, aflav(n),bflav(n)
c we need the parameter nlegreal
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer j,k,itmp,ib(nlegreal)
      call w_intassign(n,bflav,ib)
      do j=1,n
         if(aflav(j).ne.ib(j)) then
            if(j.le.2) then
               w_flavequiv=.false.
               return
            endif
            do k=j+1,n
               if(aflav(j).eq.ib(k)) then
                  itmp=ib(j)
                  ib(j)=ib(k)
                  ib(k)=itmp
                  goto 10
               endif
            enddo
            w_flavequiv=.false.
            return
         endif
 10      continue
      enddo
      w_flavequiv=.true.
      end

      function w_validBorn(n,bflav)
c Find if the flavour structure bflav is equivalent to an element
c in the list of Born processes. Equivalence means that it can be
c made identical with a permutation of final state particles.
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer n, bflav(n)
      logical w_validBorn
      integer j,k,kb,itmp,ib(nlegborn)
      call w_intassign(n,bflav,ib)
      do kb=1,flst_nborn
         do j=1,n
c recursive exit
            if(j.eq.n.and.flst_born(j,kb).eq.ib(j)) then
               w_validBorn=.true.
               return
            endif
            if(flst_born(j,kb).ne.ib(j)) then
               if(j.le.2) then
                  goto 999
               endif
               do k=j+1,n
                  if(flst_born(j,kb).eq.ib(k)) then
                     itmp=ib(j)
                     ib(j)=ib(k)
                     ib(k)=itmp
                     goto 10
                  endif
               enddo
               goto 999
            endif
 10         continue
         enddo         
 999     continue
      enddo
c      write(*,*) ' w_validBorn false,',bflav
      w_validBorn=.false.
      end

c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      subroutine w_from_madgraph_to_number(stringa,ferm_flav)
      implicit none
      integer nmax
      parameter (nmax=30)
      character stringa(nmax)
      integer ferm_flav(*)

      integer i, parton
      character *2 flav(-5:5)
      real * 8 charge(-5:5)
      common/w_flav_ordering/charge,flav

      parton = 0
      do i=1,nmax
         if (stringa(i).eq.'g') then
            parton = parton + 1
            ferm_flav(parton) = 0
         elseif (stringa(i).eq.'H') then
            parton = parton + 1
            ferm_flav(parton) = 503
         elseif (stringa(i).eq.'d') then
            parton = parton + 1
            ferm_flav(parton) = +1
         elseif (stringa(i).eq.'u') then
            parton = parton + 1
            ferm_flav(parton) = +2
         elseif (stringa(i).eq.'s') then
            parton = parton + 1
            ferm_flav(parton) = +3
         elseif (stringa(i).eq.'c') then
            parton = parton + 1
            ferm_flav(parton) = +4
         elseif (stringa(i).eq.'b') then
            parton = parton + 1
            ferm_flav(parton) = +5
         elseif (stringa(i).eq.'t') then
            parton = parton + 1
            ferm_flav(parton) = +6
         elseif (stringa(i).eq.'~') then
            ferm_flav(parton) = -ferm_flav(parton)
         elseif (stringa(i).eq.' ') then
         elseif (stringa(i).eq.'Z') then
            parton = parton + 1
            ferm_flav(parton) = +10
            parton = parton + 1
            ferm_flav(parton) = -10
          elseif (stringa(i).eq.'e') then
            parton = parton + 1
            ferm_flav(parton) = +10
          elseif (stringa(i).eq.'+') then
            ferm_flav(parton) = -ferm_flav(parton)           
          elseif (stringa(i).eq.'/') then
             return
        endif            
      enddo

      end




      subroutine w_genflavreglist
      implicit none
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/pwhg_st.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer nregions,iregions(2,maxregions)
      integer iflregl,k,l,ipart,j,itmp,nreg,iret,tmpfl
      logical w_equalintlists
      external w_equalintlists
      logical verbose
      parameter (verbose=.true.)
      logical w_flavequiv
      external w_flavequiv      
      if (flg_lightpart_check) then
c check that there are no coloured light partons before flst_lightpart
      do j=1,flst_nreal
         do ipart=3,flst_lightpart-1
            if(abs(flst_real(ipart,j)).le.st_nlight) then
               write(*,*) 
     1      ' w_genflavreglist: light parton before flst_lightpart'
               stop
            endif
         enddo
         do ipart=flst_lightpart,nlegreal
            if(abs(flst_real(ipart,j)).gt.st_nlight) then
               write(*,*) 
     1      ' w_genflavreglist: not a light parton after flst_lightpart'
               stop
            endif
         enddo
      enddo
      do j=1,flst_nborn
         do ipart=3,flst_lightpart-1
            if(abs(flst_born(ipart,j)).le.st_nlight) then
               write(*,*) 
     1      ' w_genflavreglist: light parton before flst_lightpart'
               stop
            endif
         enddo
         do ipart=flst_lightpart,nlegborn
            if(abs(flst_born(ipart,j)).gt.st_nlight) then
               write(*,*) 
     1      ' w_genflavreglist: not a light parton after flst_lightpart'
               stop
            endif
         enddo
      enddo
      endif
c map flavours to internal flavour numbers      
      call w_mapflavours
c sanity check on real flavour configurations;
c they should all be inequivalent
      do j=1,flst_nreal
         do k=j+1,flst_nreal
            if(w_flavequiv(nlegreal,flst_real(1,j),flst_real(1,k))) then
               write(*,*)'found two equivalent real flavour processes:'
               write(*,*)'process',j,', flavours ',
     1                     (flst_real(l,j),l=1,nlegreal)
               write(*,*)'process',k,', flavours ',
     1                     (flst_real(l,k),l=1,nlegreal)
               stop
            endif
         enddo
      enddo
c sanity check on Born flavour configurations;
c they should all be inequivalent
      do j=1,flst_nborn
         do k=j+1,flst_nborn
            if(w_flavequiv(nlegborn,flst_born(1,j),flst_born(1,k))) then
               write(*,*)'found two equivalent Born flavour processes:'
               write(*,*)'process',j,', flavours ',
     1                     (flst_born(l,j),l=1,nlegborn)
               write(*,*)'process',k,', flavours ',
     1                     (flst_born(l,k),l=1,nlegborn)
               stop
            endif
         enddo
      enddo
c Start search for regions (i.e. alr)
c current number of alr found
      iflregl=0
      flst_nregular=0
      if(flst_nreal.gt.maxprocreal) then
         write(*,*)' w_genflavreglist: increase maxprocreal'
         stop
      endif
      flg_withreg=.false.
      do k=1,flst_nreal
         call w_find_regions(nlegreal,flst_real(1,k),nregions,iregions)
         if(nregions.eq.0) then
            flst_nregular=flst_nregular+1
c There are remnants! set up the appropriate flag:
            flg_withreg=.true.
            call w_intassign
     #(nlegreal,flst_real(1,k),flst_regular(1,flst_nregular))
         endif
         do l=1,nregions
            iflregl=iflregl+1
            if(iregions(1,l).le.2) then
               flst_emitter(iflregl)=iregions(1,l)
            else
               flst_emitter(iflregl)=nlegreal-1
            endif
            if(iflregl.ge.maxalr) then
               write(*,*)' w_genflavreglist: increase maxalr'
               stop
            endif
            ipart=0
c final state singularity
            if(iregions(1,l).gt.2) then
               do j=1,nlegreal
                  if(j.ne.iregions(1,l)
     #                  .and.j.ne.iregions(2,l)) then
                     ipart=ipart+1
                     flst_alr(ipart,iflregl)=flst_real(j,k)
                  endif
               enddo
               ipart=ipart+1
               flst_alr(ipart,iflregl)=flst_real(iregions(1,l),k)
               ipart=ipart+1
               flst_alr(ipart,iflregl)=flst_real(iregions(2,l),k)
c put always in the order q g and q q~, i.e. fl(i)>fl(j)
               if(
     #    (flst_alr(nlegreal,iflregl)*flst_alr(nlegreal-1,iflregl).eq.0
     #    .and.flst_alr(nlegreal,iflregl).ne.0) .or.
     #    (flst_alr(nlegreal,iflregl)*flst_alr(nlegreal-1,iflregl).ne.0
     #    .and.flst_alr(nlegreal,iflregl).gt.0)) then
                  itmp=flst_alr(nlegreal,iflregl)
                  flst_alr(nlegreal,iflregl)=
     #                flst_alr(nlegreal-1,iflregl)
                  flst_alr(nlegreal-1,iflregl)=itmp
               endif
            else
c initial state singularity
               do j=1,nlegreal
                  if(j.ne.iregions(2,l)) then
                     ipart=ipart+1
                     flst_alr(ipart,iflregl)=flst_real(j,k)
                  endif
               enddo
               ipart=ipart+1
               flst_alr(ipart,iflregl)=flst_real(iregions(2,l),k)
            endif
c            write(*,*) (flst_alr(ipart,iflregl),ipart=1,nlegreal),
c     #     '   em:',flst_emitter(iflregl)
         enddo
      enddo
      nreg=iflregl
c bunch together identical elements, increasing their multiplicities
      do j=1,nreg
         flst_mult(j)=1
      enddo
      do j=1,nreg
         if(flst_mult(j).gt.0) then
            do k=j+1,nreg
c Previously was:
c               if(flst_emitter(j).eq.flst_emitter(k).and.
c     #  w_equalintlists(nlegreal,flst_alr(1,j),flst_alr(1,k))) then
c now accounts for equivalece by permutation of final state lines.
c Notice: identity of emitter and radiated parton must be valid
c  without permutations
               if(flst_mult(k).ne.0) then
                  if(flst_emitter(j).eq.flst_emitter(k).and.
c     ISR: is ISR, has same radiated parton, is equivalent
c          (excluding the radiated parton)
     1           ( (flst_emitter(j).lt.3 .and.
     2              flst_alr(nlegreal,j).eq.flst_alr(nlegreal,k).and.
     3              w_flavequiv(nlegreal-1,flst_alr(1,j),flst_alr(1,k)))
     4                   .or.
c     FSR: has the same radiated and emitter parton, is equivalent
c          (excluding emitter and emitted parton)
     5             (flst_alr(nlegreal,j).eq.flst_alr(nlegreal,k).and.
     6              flst_alr(nlegborn,j).eq.flst_alr(nlegborn,k).and.
     7              w_flavequiv(nlegreal-2,flst_alr(1,j),flst_alr(1,k)))
     8           )) then
                     flst_mult(j)=flst_mult(j)+flst_mult(k)
                     flst_mult(k)=0
                  endif
               endif
            enddo
         endif
      enddo
c browse the list, put together identical elements, compute
c associated underlying Born
      iflregl=0
      do j=1,nreg
         if(flst_mult(j).gt.0) then
            iflregl=iflregl+1
            if(j.gt.iflregl) then
               flst_emitter(iflregl)=flst_emitter(j)
               call w_intassign
     #          (nlegreal,flst_alr(1,j),flst_alr(1,iflregl))
               flst_mult(iflregl) = flst_mult(j)
            endif
            call w_ubornflav(nlegreal,flst_emitter(iflregl),
     #flst_alr(1,iflregl),flst_uborn(1,iflregl))
         endif
      enddo
      flst_nalr=iflregl

c
c Build unique list of underlying Born; reorder flavours in alpha_r, uborn, emitter
c so that the underlying Born matches exactly a Born flavour structure in the flst_born array
c flavour structures arising as underlying Born
      do j=1,flst_nalr
         do k=1,flst_nborn
c are they the same permutation?
            call w_reorder_regions(nlegborn,flst_born(1,k),
     #  flst_uborn(1,j),flst_alr(1,j),flst_emitter(j),iret)
c            if(iret.eq.1) write(*,*) ' reordering took place'
            if(iret.ne.-1) goto 11
         enddo
c they are inequivalent
         write(*,*) ' error: underlying born not present in born list'
         write(*,*) (flst_uborn(l,k),l=1,nlegborn)
         stop
 11      continue
      enddo


c Build pointers from alpha_r -> born
      do j=1,flst_nalr
         do k=1,flst_nborn
            if(w_equalintlists(nlegborn,flst_uborn(1,j),flst_born(1,k)))
     #      then
               flst_alr2born(j)=k
            endif
         enddo
      enddo
c Build pointers from born -> alpha_r
      do j=1,flst_nborn
         flst_born2alr(0,j)=0
         do k=1,flst_nalr
            if(w_equalintlists(nlegborn,flst_uborn(1,k),flst_born(1,j)))
     #      then
               flst_born2alr(0,j)=flst_born2alr(0,j)+1
               flst_born2alr(flst_born2alr(0,j),j)=k
            endif
         enddo
c Sanity check: each Born should be the underlying Born of some alr
         if(flst_born2alr(0,j).eq.0) then
            write(*,*) ' Born graph ',j,' is never the underlying Born'
     #           //' of some alr'
            stop
         endif
      enddo
c Find regions for each alpha_r
      do j=1,flst_nalr
         call w_find_regions(nlegreal,flst_alr(1,j),nregions,iregions)
         do k=1,nregions
            flst_allreg(1,k,j)=iregions(1,k)
            flst_allreg(2,k,j)=iregions(2,k)
         enddo
         flst_allreg(1,0,j)=nregions
      enddo
c For each region, compute the underlying Born multiplicity
      do j=1,flst_nalr
         if(flst_emitter(j).gt.2) then
            flst_ubmult(j)=0
c find flavour of emitter IN THE UNDERLYING BORN
            tmpfl=flst_uborn(flst_emitter(j),j)
            do k=3,nlegborn
               if(flst_uborn(k,j).eq.tmpfl) then
                  flst_ubmult(j)=flst_ubmult(j)+1
               endif
            enddo
         else
            flst_ubmult(j)=1
         endif
      enddo
      call w_unmapflavours
c     debug information
      if (verbose) then
         call w_pretty_print_flst
      endif
      end

      subroutine w_from_number_to_madgraph(n,flav,emitter,string)
      implicit none
      integer n,flav(n),emitter
      include 'src/Lib/w/nlegborn.h'
      character * (*) string
      integer max_partnames
      parameter (max_partnames=16)
      character * 3 partnames(-max_partnames:max_partnames)
      data partnames/'vt~','ta+','vm~','mu+','ve~','e+',' ','  ',' ',
     $     '  ','t~','b~','c~','s~','u~','d~','g ','d ','u ','s ' ,'c ',
     $     'b ','t ','  ','  ','',' ','e-','ve','mu-','vmu','ta-','vta'/
      integer j,nsp
      parameter (nsp=4)
      if(len(string).lt.nsp*(n+1)+7) then
         write(*,*)'w_from_number_to_madgraph: string too short;'
         write(*,*)'Increase its size'
         call exit(-1)
      endif
      string=' '
      do j=1,n
         if (abs(flav(j)).le.max_partnames) then
            string(nsp*j:nsp*j+1)=partnames(flav(j))
         else 
            string(nsp*j:nsp*j+1)='**'
         endif
      enddo
      string(nsp*j:nsp*j)='|'
      if(emitter.gt.0) then
         string(nsp*emitter-1:nsp*emitter-1)='('
         string(nsp*emitter+2:nsp*emitter+2)=')'
      elseif(emitter.eq.0) then
         string(nsp-1:nsp-1)='('
         string(3*nsp-2:3*nsp-2)=')'
      endif
      do j=len(string)-7,2*nsp+1,-1
         string(j+7:j+7)=string(j:j)
      enddo
      string(2*nsp+3:2*nsp+10)='  ==>  '
      end

      subroutine w_pretty_print_flst
      implicit none
      include 'src/Lib/w/nlegborn.h'
      character * 200 string,stringb
      include 'src/Lib/w/pwhg_flst.h'
      integer j,k,l,iun,lstring,lstringb
      call w_newunit(iun)
      open(unit=iun,file='FlavRegList',status='unknown')
      do j=1,flst_nalr
         call w_from_number_to_madgraph
     #         (nlegreal,flst_alr(1,j),flst_emitter(j),string)
         call w_from_number_to_madgraph
     #         (nlegborn,flst_uborn(1,j),-1,stringb)
         do lstring=len(string),0,-1
            if(string(lstring:lstring).ne.' ') goto 10
         enddo
 10      continue
         do lstringb=len(stringb),0,-1
            if(stringb(lstringb:lstringb).ne.' ') goto 11
         enddo
 11      continue
         write(iun,'(a,i3)') string(1:lstring)//' mult=', flst_mult(j)
         write(iun,'(a,i3)') stringb(1:lstringb)//' uborn, mult=',
     1        flst_ubmult(j)
         write(iun,'(20(1x,2(1x,i1)))')
     #   ((flst_allreg(l,k,j),l=1,2),k=1,flst_allreg(1,0,j))
      enddo
      close(iun)
      end


      subroutine w_intassign(n,iarr1,iarr2)
      implicit none
      integer n,iarr1(n),iarr2(n)
      integer j
      do j=1,n
         iarr2(j)=iarr1(j)
      enddo
      end

      function w_equalintlists(n,iarr1,iarr2)
      implicit none
      integer n,iarr1(n),iarr2(n)
      logical w_equalintlists
      integer j
      do j=1,n
         if(iarr2(j).ne.iarr1(j)) then
            w_equalintlists=.false.
            return
         endif
      enddo
      w_equalintlists=.true.
      end

      subroutine w_reorder_regions(n,uborn0,uborn,rflav,emit,iret)
c reorders the amplitude in uborn according to the amplitude in uborn0
c It also reorders rflav, and sets the emitter to its appropriate value
c If the amplitudes have inequivalent flavour structures, it returns -1
c without any other action.
c If the flavour structures are identical, it returns 0, with no other action.
c If the flavour structures have been reordered, it returns 1.
      implicit none
      integer n,uborn0(n),uborn(n),rflav(n),emit,iret
      integer j,k,itmp,ib(100)
      iret=0
      call w_intassign(n,uborn,ib)
      do j=1,n
         if(uborn0(j).ne.ib(j)) then
            if(j.le.2) then
               iret=-1
               goto 999
            endif
            iret=1
            do k=j+1,n
               if(uborn0(j).eq.ib(k)) then
                  itmp=ib(j)
                  ib(j)=ib(k)
                  ib(k)=itmp
                  goto 10
               endif
            enddo
c they differ in flavour content
            iret=-1
            goto 999
 10         continue
         endif
      enddo
c they are identical; no reordering needed
      if(iret.eq.0) return
c reorder
      do j=3,n
         if(uborn0(j).ne.uborn(j)) then
            iret=1
            do k=j+1,n
               if(uborn0(j).eq.uborn(k)) then
                  itmp=uborn(j)
                  uborn(j)=uborn(k)
                  uborn(k)=itmp
                  itmp=rflav(j)
                  rflav(j)=rflav(k)
                  rflav(k)=itmp
                  if(emit.eq.j) emit=k
                  if(emit.eq.k) emit=j
                  goto 11
               endif
            enddo
            write(*,*) ' should never get here'
 11         continue
         endif
      enddo
 999  continue
      end

      function w_valid_emitter(j)
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      logical w_valid_emitter
      integer j,alr
      integer validarr(0:nlegborn)
      data validarr/nlegreal*0/
      save validarr
      if(validarr(j).eq.1) then
         w_valid_emitter=.true.
         return
      elseif(validarr(j).eq.-1) then
         w_valid_emitter=.false.
         return
      else
         do alr=1,flst_nalr
            if(j.eq.flst_emitter(alr)) then
               w_valid_emitter=.true.
               validarr(j)=1
               return
            endif
         enddo
         w_valid_emitter=.false.
         validarr(j)=-1
         return
      endif
      end
