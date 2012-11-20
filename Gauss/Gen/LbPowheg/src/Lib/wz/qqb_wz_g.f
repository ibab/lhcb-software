      subroutine wz_qqb_wz_g(P,msq)
C---Author John Campbell Fri Feb 19 11:06:08 CST 1999
C---Modified to include supplementary diagrams by JC on Feb 24
c---Matrix element squared averaged over initial colors and spins
c---  averaged(summed) over initial(final) colours and spins
C For wz_nwz=+1
c     u(-p1)+dbar(-p2)-->mu^-(p5)+mu^+(p6)+n(p3)+e^+(p4)+g(p7)
C For wz_nwz=-1
c     d(-p1)+ubar(-p2)-->mu^-(p5)+mu^+(p6)+e^-(p3)+nbar(p4)+g(p7)
c---
c   for the moment --- radiation only from initial line
      implicit none 
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/qcdcouple.h'
      include 'src/Lib/wz/zwcouple.h'
      include 'src/Lib/wz/ckm.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      include 'src/Lib/wz/ewcharge.h'
      include 'src/Lib/wz/anomcoup.h'
      include 'src/Lib/wz/nwz.h'
      include 'src/Lib/wz/pwhg_st.h'
      include 'src/Lib/wz/pwhg_math.h'
      include 'src/Lib/wz/vvsettings.h'
      integer j,k,polg,polz,minus,mplus,jp,kp
      double precision FAC,FACM,FAC1
      double complex prop12,prop34,prop56
      character*2 wz_plabel(mxpart)
      common/wz_plabel/wz_plabel
      common/wz_pchoice/j,k
      double precision P(mxpart,4),qdks(mxpart,4),msq(-nf:nf,-nf:nf)
      double precision ave,cotw,s127,wwflag
      double complex 
     .  qu_qb(10,2,2),qu_gg(10,2,2),gg_qb(10,2,2),
     .  qb_qu(10,2,2),qb_gg(10,2,2),gg_qu(10,2,2),
     .  A(2,2)
      double precision v2(2),cl1,cl2,en1,en2,xfac
      double complex ZgLR(nf,2),c1(2),c2(2)
      common/wz_xanomcoup/xdelg1_z,xdelg1_g,xlambda_g,xlambda_z,
     . xdelk_g,xdelk_z
      data minus,mplus/1,2/
      data cl1,cl2,en1,en2/4*1d0/
      include 'src/Lib/wz/cvecbos.h'
      double complex A_SAVE(-nf:nf,-nf:nf,2,2)
      integer iloop,nloop
      double complex wz_cpropfac
      external wz_cpropfac

      FAC = -2D0*gwsq*esq

      !TM set gsq QCD here
      gsq = st_alpha*4d0*pi


c      gsq = (1.4828317324943823d0)**1
c      write(*,*)'MG gg',gsq

      FAC1=two*gsq*cf


      if (wz_nwz.eq.1)then
      FACM= FAC
      elseif (wz_nwz.eq.-1) then
      FACM= -FAC
      else
      write(6,*) 'wz_nwz .ne. +1 or -1 in wz_qqb_wz_g.f'
      stop
      endif 
      if     (wz_nwz.eq.-1) then
        cl1=1d0
        cl2=0d0
        en1=le
        en2=ln
      elseif (wz_nwz.eq.+1) then
        cl1=0d0
        cl2=1d0
        en1=ln
        en2=le
      endif
      v2(1)=wz_L1
      v2(2)=r1


c--- wwflag=1 for most cases, indicating presence of diagram with 2 W's
      wwflag=1d0
c--- but for Z -> bbbar this diagram contains |V_tb|**2 which we take 0
      if (wz_plabel(5) .eq. 'qb') then    
        wwflag=0d0
      endif

c-- if Z -> neutrinos, we need to switch c1 and c2
      if ((vdecaymodeZ.eq.12).or.(vdecaymodeZ.eq.14).or.
     .     (vdecaymodeZ.eq.16)) then
        cl1=1d0-cl1
        cl2=1d0-cl2
      endif
      
      do jp=-nf,nf
      do kp=-nf,nf
      msq(jp,kp)=0d0
      A_SAVE(jp,kp,:,:)=0d0
      enddo
      enddo

      if (interference) then
         nloop=2
      else
         nloop=1
      endif

      do iloop=1,nloop


C----Change the momenta to DKS notation 
c   We have --- d(-p1)+ubar(-p2)-->nu(p3)+e^+(p4)+mu^-(p5)+mu^+(p6)+g(p7)
c   DKS have--- u( q2)+dbar( q1)-->nu(q3)+e^+(q4)+mu^-(q6)+mu^+(q5)+g(p7)

      if (iloop.eq.1) then
         do jp=1,4
            qdks(1,jp)=p(1,jp)
            qdks(2,jp)=p(2,jp)
            qdks(3,jp)=p(3,jp)
            qdks(4,jp)=p(4,jp)
            qdks(5,jp)=p(6,jp)
            qdks(6,jp)=p(5,jp)
            qdks(7,jp)=p(7,jp)
         enddo
      elseif (iloop.eq.2) then
         if (wz_nwz.eq.1) then
         do jp=1,4
            qdks(1,jp)=p(1,jp)
            qdks(2,jp)=p(2,jp)
            qdks(3,jp)=p(3,jp)
            qdks(4,jp)=p(6,jp)
            qdks(5,jp)=p(4,jp)
            qdks(6,jp)=p(5,jp)
            qdks(7,jp)=p(7,jp)
         enddo
         elseif (wz_nwz.eq.-1) then
         do jp=1,4
            qdks(1,jp)=p(1,jp)
            qdks(2,jp)=p(2,jp)
            qdks(3,jp)=p(5,jp)
            qdks(4,jp)=p(4,jp)
            qdks(5,jp)=p(6,jp)
            qdks(6,jp)=p(3,jp)
            qdks(7,jp)=p(7,jp)
         enddo
         endif
      endif

      call wz_spinoru(7,qdks,za,zb)
c--   s returned from sprodx (common block) is 2*dot product


c--   calculate propagators
      cotw=dsqrt((one-xw)/xw)
      s127=s(1,2)+s(1,7)+s(2,7)

c---  Always use these propagators
c      prop12=s127/dcmplx(s127-wmass**2,wmass*wwidth)  
c      prop34=s(3,4)/dcmplx(s(3,4)-wmass**2,wmass*wwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-zmass**2,zmass*zwidth)
      prop12=wz_cpropfac(s127,wmass,wwidth)
      prop34=wz_cpropfac(s(3,4),wmass,wwidth)
      prop56=wz_cpropfac(s(5,6),zmass,zwidth)

c--- apply a dipole form factor to anomalous couplings
      xfac=1d0/(1d0+s127/(tevscale*1d3)**2)**2
      xdelg1_z=xfac*delg1_z
      xdelg1_g=xfac*delg1_g
      xdelk_z=xfac*delk_z
      xdelk_g=xfac*delk_g
      xlambda_z=xfac*lambda_z
      xlambda_g=xfac*lambda_g

c---case dbar-u
      call wz_wzamps(1,2,3,4,5,6,7,za,zb,qb_qu)
c---case u-dbar
      call wz_wzamps(2,1,3,4,5,6,7,za,zb,qu_qb)

c---case g-u
      call wz_wzamps(7,2,3,4,5,6,1,za,zb,gg_qu)
c---case u-g
      call wz_wzamps(7,1,3,4,5,6,2,za,zb,qu_gg)

c---case dbar-g
      call wz_wzamps(1,7,3,4,5,6,2,za,zb,qb_gg)
c---case g-dbar
      call wz_wzamps(2,7,3,4,5,6,1,za,zb,gg_qb)

c---set up left/right handed couplings for both Z and gamma*
c---note that the second label corresponds to the helicity
c---of the LEPTON coupling v2, NOT the quarks (all L)
      do j=1,nf
        ZgLR(j,minus)=L(j)*v2(1)*prop56+Q(j)*q1           
        ZgLR(j,mplus)=L(j)*v2(2)*prop56+Q(j)*q1           
      enddo


      do polz=1,2
      if(wz_nwz.eq.1) then
        c1(polz)=ZgLR(2,polz)
        c2(polz)=ZgLR(1,polz)
      else
        c1(polz)=ZgLR(1,polz)
        c2(polz)=ZgLR(2,polz)
      endif
      enddo
      
      do j=-nf,nf
      if (((j.eq.+1).or.(j.eq.+3).or.(j.eq.+5)
     . .or.(j.eq.-2).or.(j.eq.-4)) .and. (wz_nwz .eq. +1))
     . go to 20
      if (((j.eq.-1).or.(j.eq.-3).or.(j.eq.-5)
     . .or.(j.eq.+2).or.(j.eq.+4)) .and. (wz_nwz .eq. -1))
     . go to 20
      do k=-nf,nf
      if (((k.eq.+1).or.(k.eq.+3).or.(k.eq.+5)
     . .or.(k.eq.-2).or.(k.eq.-4)) .and. (wz_nwz .eq. +1))
     . go to 19
      if (((k.eq.-1).or.(k.eq.-3).or.(k.eq.-5)
     . .or.(k.eq.+2).or.(k.eq.+4)) .and. (wz_nwz .eq. -1))
     . go to 19
    
      if     ((j .gt. 0) .and. (k .lt. 0)) then

c---case u-db
          do polg=1,2
          do polz=1,2
          A(polg,polz)=((ZgLR(+j,polz)*qu_qb(2,polg,polz)
     .                  +ZgLR(-k,polz)*qu_qb(3,polg,polz))*FAC
     .                  +(cotw*v2(polz)*prop56*qu_qb(1,polg,polz)
     .                                     +q1*qu_qb(10,polg,polz))
     .                    *prop12*FACM)*prop34
     .            +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .                   *prop12*qu_qb(5,polg,polz)
     .                 +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .                   *prop12*qu_qb(4,polg,polz)
     .    +wwflag*0.5d0*prop34*prop12/xw*qu_qb(6,polg,polz)*cl1
     .     +wwflag*0.5d0*prop34*prop12/xw*qu_qb(7,polg,polz)*cl2)
     
c          A(polg,polz)=((L(+j)*qu_qb(2,polg,polz)
c     .                  +L(-k)*qu_qb(3,polg,polz))*FAC
c     .                  +cotw*prop12*qu_qb(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
          enddo
          enddo
          ave=xn*aveqq*Vsq(j,k)

      elseif ((j .lt. 0) .and. (k .gt. 0)) then


c---case db-u
          do polg=1,2
          do polz=1,2
          A(polg,polz)=((ZgLR(+k,polz)*qb_qu(2,polg,polz)
     .                  +ZgLR(-j,polz)*qb_qu(3,polg,polz))*FAC
     .                  +(cotw*v2(polz)*prop56*qb_qu(1,polg,polz)
     .                                     +q1*qb_qu(10,polg,polz))
     .                    *prop12*FACM)*prop34
     .            +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .                   *prop12*qb_qu(5,polg,polz)
     .                 +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .                   *prop12*qb_qu(4,polg,polz)
     .    +wwflag*0.5d0*prop34*prop12/xw*qb_qu(6,polg,polz)*cl1
     .    +wwflag*0.5d0*prop34*prop12/xw*qb_qu(7,polg,polz)*cl2)

c          A(polg,polz)=((L(+k)*qb_qu(2,polg,polz)
c     .                  +L(-j)*qb_qu(3,polg,polz))*FAC
c     .                  +cotw*prop12*qb_qu(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
          enddo
          enddo
          ave=xn*aveqq*Vsq(j,k)

      elseif ((j .gt. 0) .and. (k .eq. 0)) then
c---case u-g

         if (Vsum(j).ne.0d0) then
            do polg=1,2
               do polz=1,2
                  A(polg,polz)=((c1(polz)*qu_gg(2,polg,polz)
     .              +c2(polz)*qu_gg(3,polg,polz))*FAC
     .              +(cotw*v2(polz)*prop56*qu_gg(1,polg,polz)
     .              +q1*qu_gg(10,polg,polz))
     .              *prop12*FACM)*prop34
     .              +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .              *prop12*qu_gg(5,polg,polz)
     .              +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .              *prop12*qu_gg(4,polg,polz)
     .    +wwflag*0.5d0*prop34*prop12/xw*qu_gg(6,polg,polz)*cl1
     .    +wwflag*0.5d0*prop34*prop12/xw*qu_gg(7,polg,polz)*cl2)
                  
c          A(polg,polz)=((c1*qu_gg(2,polg,polz)
c     .                  +c2*qu_gg(3,polg,polz))*FAC
c     .                  +cotw*prop12*qu_gg(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
               enddo
            enddo
         endif
            ave=xn*aveqg*Vsum(j)
      elseif ((j .lt. 0) .and. (k .eq. 0)) then
c---case db-g
         if (Vsum(j).ne.0d0) then
            do polg=1,2
               do polz=1,2
                  A(polg,polz)=((c1(polz)*qb_gg(2,polg,polz)
     .             +c2(polz)*qb_gg(3,polg,polz))*FAC
     .             +(cotw*v2(polz)*prop56*qb_gg(1,polg,polz)
     .             +q1*qb_gg(10,polg,polz))
     .             *prop12*FACM)*prop34
     .             +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .             *prop12*qb_gg(5,polg,polz)
     .             +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .             *prop12*qb_gg(4,polg,polz)
     .     +wwflag*0.5d0*prop34*prop12/xw*qb_gg(6,polg,polz)*cl1
     .     +wwflag*0.5d0*prop34*prop12/xw*qb_gg(7,polg,polz)*cl2)
                  
c          A(polg,polz)=((c1*qb_gg(2,polg,polz)
c     .                  +c2*qb_gg(3,polg,polz))*FAC
c     .                  +cotw*prop12*qb_gg(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
               enddo
            enddo
         endif
            
            ave=xn*aveqg*Vsum(j)
      elseif ((j .eq. 0) .and. (k .gt. 0)) then
c---case g-u
         if (Vsum(k).ne.0d0) then
            
            do polg=1,2
               do polz=1,2
                  A(polg,polz)=((c1(polz)*gg_qu(2,polg,polz)
     .                 +c2(polz)*gg_qu(3,polg,polz))*FAC
     .                +(cotw*v2(polz)*prop56*gg_qu(1,polg,polz)
     .               +q1*gg_qu(10,polg,polz))
     .               *prop12*FACM)*prop34
     .               +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .               *prop12*gg_qu(5,polg,polz)
     .               +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .               *prop12*gg_qu(4,polg,polz)
     .     +wwflag*0.5d0*prop34*prop12/xw*gg_qu(6,polg,polz)*cl1
     .     +wwflag*0.5d0*prop34*prop12/xw*gg_qu(7,polg,polz)*cl2)
                  
c     A(polg,polz)=((c1(polz)*gg_qu(2,polg,polz)
c     .                  +c2(polz)*gg_qu(3,polg,polz))*FAC
c     .                  +cotw*prop12*gg_qu(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
               enddo
            enddo
         endif
            ave=xn*aveqg*Vsum(k)
      elseif ((j .eq. 0) .and. (k .lt. 0)) then
c---case g-db

         if (Vsum(k).ne.0d0) then
            
            do polg=1,2
               do polz=1,2
                  A(polg,polz)=((c1(polz)*gg_qb(2,polg,polz)
     .                +c2(polz)*gg_qb(3,polg,polz))*FAC
     .                +(cotw*v2(polz)*prop56*gg_qb(1,polg,polz)
     .                +q1*gg_qb(10,polg,polz))
     .                *prop12*FACM)*prop34
     .                +FAC*((en1*v2(polz)*prop56+q1*(-1d0)*cl1)
     .                *prop12*gg_qb(5,polg,polz)
     .                +(en2*v2(polz)*prop56+q1*(-1d0)*cl2)
     .                *prop12*gg_qb(4,polg,polz)
     .      +wwflag*0.5d0*prop34*prop12/xw*gg_qb(6,polg,polz)*cl1
     .      +wwflag*0.5d0*prop34*prop12/xw*gg_qb(7,polg,polz)*cl2)
                  
c     A(polg,polz)=((c1*gg_qb(2,polg,polz)
c     .                  +c2*gg_qb(3,polg,polz))*FAC
c     .                  +cotw*prop12*gg_qb(1,polg,polz)*FACM)
c     .                 *prop34*prop56*v2(polz)
                  
               enddo
            enddo
         endif
            
            ave=xn*aveqg*Vsum(k)

      else
          ave=0d0
      endif

      if (ave.gt.0d0) then


      msq(j,k)=msq(j,k) + FAC1*ave
     .          *(cdabs(A(mplus,minus))**2+cdabs(A(minus,minus))**2
     .           +cdabs(A(mplus,mplus))**2+cdabs(A(minus,mplus))**2)

      if (iloop.eq.1) then
         if (interference) then
            A_SAVE(j,k,mplus,mplus) = A(mplus,mplus)
            A_SAVE(j,k,minus,mplus) = A(minus,mplus)
            A_SAVE(j,k,mplus,minus) = A(mplus,minus)
            A_SAVE(j,k,minus,minus) = A(minus,minus)
         endif
      elseif(iloop.eq.2) then
         msq(j,k)=msq(j,k) - FAC1*ave
     .          *(dconjg(A(mplus,minus))*A_SAVE(j,k,mplus,minus) + 
     .        A(mplus,minus)*dconjg(A_SAVE(j,k,mplus,minus)) +
     .        dconjg(A(minus,minus))*A_SAVE(j,k,minus,minus) + 
     .        A(minus,minus)*dconjg(A_SAVE(j,k,minus,minus))
     .           )
      endif
               
      endif

 19   continue
      enddo
 20   continue
      enddo

      enddo !iloop
      
      !symmetry factor
      msq = msq*vsymfact

      return
      end

      
      




