c The subroutine wz_lhefinitemasses performs momentum reshuffling, giving
c masses to final state particles that were treated as massless, and
c rescaling the momenta accordingly, so that the total energy and
c momentum is conserved. The masses of resonances present are
c preserved. The subroutine works on the LHIUP (Les Houches Interface
c for User Processes). It assumes that initial state particles have
c imohep(1,)=0. All other particles have imohep(1,) either pointing to
c the initial state particle 1, or to a resonance.  The value of the
c masses are in the common block in pwhg_physpar.h.  The main routine is
c subroutine lhefinitemasses.


c default values for quark masses should be set here      
      subroutine wz_lhfm_setmasses
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      include 'src/Lib/wz/pwhg_st.h'
      include 'src/Lib/wz/pwhg_physpar.h'
      integer j,id
      do j=3,nup
         if(istup(j).eq.1) then
            id=abs(idup(j))
            if(id.ge.1.and.id.le.st_nlight) then
               pup(5,j)=physpar_mq(id)
            elseif(id.eq.11) then
               pup(5,j)=physpar_ml(1)
            elseif(id.eq.13) then
               pup(5,j)=physpar_ml(2)
            elseif(id.eq.15) then
               pup(5,j)=physpar_ml(3)
            endif
         endif
      enddo
      end

c If particle i has mothup(1,)=moth,  iret=1;
c Else, if moth is an ancestor of i,  iret=2;
c Else (i is not an ancestor of moth) iret=0;
      subroutine wz_lhfm_sonof(i,moth,iret)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i,moth,iret
      integer icurr
c      write(*,*) ' entering sonof'
      icurr=i
 11   continue
c      write(*,*) icurr,moth
      if(mothup(1,icurr).eq.moth) then
         if(icurr.eq.i) then
c direct son
            iret=1
         else
c indirect son
            iret=2
         endif
c         write(*,*) ' exiting sonof', iret
         return
      else
         icurr=mothup(1,icurr)
         if(icurr.ne.0) goto 11
      endif
      iret=0
c      write(*,*) ' exiting sonof', iret
      end

c Looks for subset of final state particles (istup=1) that need to be
c reshuffled because they come from the same resonance (istup=2), whose
c 4-momentum needs to be preserved. The subset is stored in the stack
c controlled by subroutine push. If any final state particle is indirect
c son of the resonance, the subset is abandoned.
c After a subset is reshuffled, the istup of its members is set to 1001,
c so that no further reshuffling will be performed on them. The resonance
c istup is changed to 1, so that it may become subject to further reshuffling.
c The program terminates when it does not find any more subsets to reshuffle.
      subroutine wz_lhefinitemasses
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i,j,moth,iret
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
c      call wz_lhefwritev(6)
      call wz_lhfm_checkmom
      call wz_lhfm_setmasses
      call wz_lhfm_addfakeinires
c initialize empty stack to remember resonances
      call wz_lhfm_pushmoth(1,0)
 1    continue
      do i=3,nup
c initialize empty stack
         call wz_lhfm_push(i,0)
         if(istup(i).eq.1) then
            moth=mothup(1,i)
            if(istup(moth).eq.2) then
c push i on stack
               call wz_lhfm_push(i,1)
               do j=i+1,nup
                  if(istup(j).eq.1) then
                     call wz_lhfm_sonof(j,moth,iret)
                     if(iret.eq.1) then
                        call wz_lhfm_push(j,1)
                     elseif(iret.eq.2) then
                        goto 11
                     endif
                  endif
               enddo
c found bunch of particles coming from the same resonance
               goto 12
            endif
         endif
 11      continue
      enddo
 12   continue
c reshuffle accumulated particles
c are there any particles in the stack?
      call wz_lhfm_push(i,2)
c if not it's all done
      if(i.eq.0) goto 99
      call wz_lhfm_reshuffle(iret)
      if(iret.eq.-1) then
         write(*,*) ' ***  Cannot reshuffle! ***'
         write(*,*) ' ***  kill event! ***'
         nup=0
         return
      endif
c     remember this a resonance
      call wz_lhfm_pushmoth(moth,1)
c set temporarily status to 1001 to final state particles,
c and set the status of moth to 1
      call wz_lhfm_changestatus
      goto 1
 99   continue
c reset appropriate status for final state particles and resonances
      call wz_lhfm_resetstatus
      call wz_lhfm_checkmom
c      call wz_lhefwritev(6)
      end
      
c set status of subset of reshuffled particles and of mother
      subroutine wz_lhfm_changestatus
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      integer j
      do j=1,n
         istup(arr(j))=1001
      enddo
c set status of (common) mother particle as final state
      istup(mothup(1,arr(1)))=1
      end

c Adds a fake resonance with total momentum equal to the total
c incoming momentum. Sons of the initial state (mothup(1,)=1)
c will be set to point to this resonance. Notice: should not
c increase nup, this fake resonance will never be reshuffled.
      subroutine wz_lhfm_addfakeinires
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i,mu,nfk
c if moth=1 it means we are dealing with the whole production process;
c set up a fake mother with the correct momentum on the LHI
      nfk=nup+1
      do i=3,nup
         if(istup(mothup(1,i)).eq.-1) then
            mothup(1,i)=nfk
         endif
      enddo
      istup(nfk)=2
c This fake resonance better not have ancestors:
      mothup(1,nfk)=0
c Set up momentum of fake resonance
      pup(5,nfk)=0
      do mu=1,4
         pup(mu,nfk)=pup(mu,1)+pup(mu,2)
         if(mu.lt.4) then
            pup(5,nfk)=pup(5,nfk)-pup(mu,nfk)**2
         else
            pup(5,nfk)=pup(5,nfk)+pup(mu,nfk)**2
         endif
      enddo
      pup(5,nfk)=sqrt(pup(5,nfk))
      end

c reset correct status for all particles
      subroutine wz_lhfm_resetstatus 
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer n,arr(maxnup)
      common/wz_clhfm_pushmoth/arr,n
      integer j
      do j=3,nup
         if(istup(j).eq.1001) istup(j)=1
         if(mothup(1,j).eq.nup+1) then
            if(mothup(2,j).ne.1) then
               mothup(1,j)=1
            else
               mothup(1,j)=2
            endif
         endif
      enddo
c resonances are reset to 2
      do j=1,n
         istup(arr(j))=2
      enddo
      end



c imode=0: initialize stack
c imode=1: push value in stack
c imode=2: set i to current length of stack
      subroutine wz_lhfm_push(i,imode)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i,imode
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      save /wz_clhfm_push/
      if(imode.eq.0) then
         n=0
      elseif(imode.eq.1) then
         n=n+1
         arr(n)=i
      elseif(imode.eq.2) then
         i=n
      endif
      end

c imode=0: initialize stack
c imode=1: push value in stack
      subroutine wz_lhfm_pushmoth(i,imode)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i,imode
      integer n,arr(maxnup)
      common/wz_clhfm_pushmoth/arr,n
      save /wz_clhfm_pushmoth/
      if(imode.eq.0) then
         n=0
      else
         n=n+1
         arr(n)=i
      endif
      end

c reshuffle momenta for the n particles in common/wz_clhfm_push/arr,n;
c The total momentum of the n particles equals the momentum of the
c mother resonance.
c The system is boosted longitudinally to a frame where its
c total longitudinal momentum is zero, then to a frame where its
c transverse momentum is zero.
c These boosts are performed on each particle in arr, and also on
c all of its (direct or indirect) sons. They are also performed for convenience
c upone the mother resonance.
c Momenta are then rescaled by a common factor, such that the total
c energy of the system equals the mother resonance energy.
c The rescaling factor is found by Newton's method, using up to the second
c derivative of the energy with respect to the rescaling factor.
c If a particle with sons had its momentum rescaled, all its son's (direct or indirect)
c are boosted to conserve momentum (subroutine reboost).
c After all this operations, the initial longitudinal and perpendicular boosts
c are performed again in reverse.  
      subroutine wz_lhfm_reshuffle(iret)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer iret
      real * 8 beta(2),betaz,sc,de,en,diffen,diff2en,mass
      integer moth,j,mu,ipart
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      iret=0
c check that reshuffling is possible
      mass=0
      do j=1,n
         mass=mass+pup(5,arr(j))
      enddo
      moth=mothup(1,arr(1))
      if(mass.gt.pup(5,moth)) then
         write(*,*) ' cannot reshuffle!'
         iret=-1
         return
      endif
c reshuffle momenta in arr(1...n)
      betaz=pup(3,moth)/pup(4,moth)
c wz_boost all momenta in arr (and its sons)
c to the resonance rest frame
      call wz_lhfm_boostz(-betaz)
      beta(1)=-pup(1,moth)/pup(4,moth)
      beta(2)=-pup(2,moth)/pup(4,moth)
      call wz_lhfm_boostperp(beta)
c rescale momenta
c find scaling factor (Newton method generalized to 2nd derivative)
      sc=1
c returns in en,diffen and diff2en the total energy and its first and second derivative
c for a given scaling factor sc.
 10   call wz_lhfm_totalen(sc,en,diffen,diff2en)
      de=pup(5,moth)-en
      if(abs(de)/en.gt.1d-10) then
         sc=sc+de/diffen-(de/diffen)**2*diff2en/(2*diffen)
         goto 10
      endif
c      call checkmomres
      do j=1,n
         ipart=arr(j)
         do mu=1,3
            pup(mu,ipart)=pup(mu,ipart)*sc
         enddo
         pup(4,ipart)=sqrt(pup(1,ipart)**2+pup(2,ipart)**2
     1        +pup(3,ipart)**2+pup(5,ipart)**2)
c     if any rescaled momentum was a resonance,
c     find appropriate wz_boost for its sons;
         call wz_lhfm_reboost(ipart,sc)
      enddo
c      call checkmomres
      beta(1)=-beta(1)
      beta(2)=-beta(2)
      call wz_lhfm_boostperp(beta)
      call wz_lhfm_boostz(betaz)
      end

c check momentum balance in array arr,n
      subroutine wz_lhfm_checkmomres
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      real * 8 p(4),diff
      integer moth,mu,j
      integer ipart
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n      
      do mu=1,4
         p(mu)=0
      enddo
      moth=mothup(1,arr(1))
      do j=1,n
         ipart=arr(j)
         do mu=1,4
            p(mu)=p(mu)+pup(mu,ipart)
         enddo
      enddo
      diff=0
      do mu=1,4
         p(mu)=p(mu)-pup(mu,moth)
         diff=diff+abs(p(mu))
      enddo
      if(diff.gt.1d-8) write(*,*)' checkmom:',p
      end

      subroutine wz_lhfm_reboost(moth,sc)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer moth
      real * 8 sc
      real * 8 beta,vec(3),gamma,gammamo,pparallel,e,p,ep,pp,m,tot
      real * 8 total(5)
      integer ipart,mu,iret
      logical ini
      ini=.true.
      do ipart=3,nup
         if(istup(ipart).eq.1001) then
            call wz_lhfm_sonof(ipart,moth,iret)
            if(iret.ne.0) then
               if(ini) then
c check momentum conservation at the end
                  do mu=1,4
                     total(mu)=pup(mu,moth)
                  enddo
c find wz_boost velocity
                  e=pup(4,moth)
                  m=pup(5,moth)
                  p=sqrt(pup(1,moth)**2+pup(2,moth)**2+pup(3,moth)**2)
                  pp=p/sc
                  ep=sqrt(pp**2+m**2)
                  beta=(e*pp-ep*p)/(p*pp-e*ep)
                  do mu=1,3
                     vec(mu)=pup(mu,moth)/p
                  enddo
                  gamma=1/sqrt(1-beta**2)
c gamma-1, better precision at small beta
                  gammamo=beta**2/(sqrt(1-beta**2)+1-beta**2)
                  ini=.false.
               endif
               pparallel=pup(1,ipart)*vec(1)
     1              +pup(2,ipart)*vec(2)+pup(3,ipart)*vec(3)
               do mu=1,3
                  pup(mu,ipart)=pup(mu,ipart)
     1                 +vec(mu)*(gammamo*pparallel
     2                 +gamma*beta*pup(4,ipart))
               enddo
               pup(4,ipart)=pup(4,ipart)+gammamo*pup(4,ipart)
     1              +gamma*pparallel*beta
               do mu=1,4
c only for direct sons
                  if(iret.eq.1) total(mu)=total(mu)-pup(mu,ipart)
               enddo
            endif
         endif
      enddo
      if(.not.ini) then
         tot=abs(total(1))+abs(total(2))+abs(total(3))+abs(total(4))
         if(tot.gt.1d-6) then
            write(*,*) ' lhfm_reboosts, mom. cons. violation:',tot
            write(*,*) ' lhfm_reboosts, mom. cons. violation:',total
         endif
      endif
      end

      subroutine wz_lhfm_totalen(s,en,diffen,diff2en)
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      real * 8 s,en,diffen,diff2en
      real * 8 k,m,e
      integer j,ipart
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      en = 0
      diffen = 0
      diff2en = 0
      do j=1,n
         ipart=arr(j)
         k = sqrt(pup(1,ipart)**2+pup(2,ipart)**2+pup(3,ipart)**2)
         m = pup(5,ipart)
         e = sqrt( (k*s)**2 + m**2 )
         en = en + e
         diffen = diffen + k**2*s/e
         diff2en = diff2en + k**2*m**2/e**3
      enddo
      end
      
      subroutine wz_lhfm_boostz(beta)
c     boosts the m vectors vin(0:3,m) into the vectors vout(0:3,m) (that can
c     be the same) in the direction of vec(3) (|vec|=1) with velocity
c     beta.  Lorents convention: (wz_t,x,y,z).
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer moth,iret
      real * 8 beta,gamma,pparallel
      integer ipart
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      if(beta.eq.0) return
      gamma=1/sqrt(1-beta**2)
      moth=mothup(1,arr(1))
      do ipart=3,nup
         call wz_lhfm_sonof(ipart,moth,iret)
         if(iret.ne.0.or.ipart.eq.moth) then
            pparallel=pup(3,ipart)
            pup(3,ipart)=pup(3,ipart)
     1           +((gamma-1)*pparallel
     2           +gamma*beta*pup(4,ipart))
            pup(4,ipart)=gamma*(pup(4,ipart)+pparallel*beta)
         endif
      enddo
      end
      
      subroutine wz_lhfm_boostperp(betav)
c     boosts the m vectors vin(0:3,m) into the vectors vout(0:3,m) (that can
c     be the same) in the direction of vec(3) (|vec|=1) with velocity
c     beta.  Lorents convention: (wz_t,x,y,z).
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer ipart,moth,iret
      real * 8 betav(2),beta
      real * 8 gamma
      real * 8 pparallel
      integer n,arr(maxnup)
      common/wz_clhfm_push/arr,n
      if(betav(1).eq.0.and.betav(2).eq.0) return
      beta=sqrt(betav(1)**2+betav(2)**2)
      gamma=1/sqrt(1-beta**2)
      moth=mothup(1,arr(1))
      do ipart=3,nup
         call wz_lhfm_sonof(ipart,moth,iret)
         if(iret.ne.0.or.ipart.eq.moth) then
            pparallel=(pup(1,ipart)*betav(1)+pup(2,ipart)*betav(2))/beta
            pup(1,ipart)=pup(1,ipart)
     1           +betav(1)/beta*((gamma-1)*pparallel
     2           +gamma*beta*pup(4,ipart))
            pup(2,ipart)=pup(2,ipart)
     1           +betav(2)/beta*((gamma-1)*pparallel
     2           +gamma*beta*pup(4,ipart))
            pup(4,ipart)=gamma*(pup(4,ipart)+pparallel*beta)
         endif
      enddo
      end


      subroutine wz_lhfm_checkmom
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      real * 8 sum,tmp,total(4)
      integer mu,i,moth,iret
      sum()=abs(total(1))+abs(total(2))+abs(total(3))+abs(total(3))
      do mu=1,4
         total(mu)=pup(mu,1)+pup(mu,2)
      enddo
      do i=3,nup
         tmp=pup(1,i)**2+pup(2,i)**2+pup(3,i)**2+pup(5,i)**2-pup(4,i)**2
         tmp=abs(tmp)
         if(tmp.gt.1d-6) then
            write(*,*) 'iup',i,' check mass:',tmp
            write(*,*) pup(1,i),pup(2,i),pup(3,i),pup(4,i),pup(5,i)
         endif
         if(istup(i).eq.1) then
            do mu=1,4
               total(mu)=total(mu)-pup(mu,i)
            enddo
         endif
      enddo
      if(sum().gt.1d-6) then
         write(*,*) ' total mom. cons.:',sum()
      endif
      do moth=3,nup
         if(istup(moth).eq.2) then
            do mu=1,4
               total(mu)=pup(mu,moth)
            enddo
            do i=3,nup
               call wz_lhfm_sonof(i,moth,iret)
               if(iret.eq.1) then
                  do mu=1,4
                     total(mu)=total(mu)-pup(mu,i)
                  enddo
               endif
            enddo
            if(sum().gt.1d-6) then
               write(*,*) 'res.',moth,' mom. cons.:',sum()
            endif
         endif
      enddo
      end
