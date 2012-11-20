      subroutine zz_qqb_zz_g(P,msq)
c---Matrix element squared averaged over initial colors and spins
c     q(-p1)+qbar(-p2) -->  q'(p4)+bar{q'}(p5) + n(p6)+ebar(p7)+ g(p3)
c   for the moment --- radiation only from initial line
      implicit none 
      include 'src/Lib/zz/qcdcouple.h'
      include 'src/Lib/zz/zcouple.h'
      include 'src/Lib/zz/sprods_com.h'
      include 'src/Lib/zz/zprods_decl.h'
      include 'src/Lib/zz/ewcharge.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/pwhg_st.h'
      include 'src/Lib/zz/cvecbos.h'
      include 'src/Lib/zz/vvsettings.h' 
      double precision xn
      integer nf,mxpart
      parameter(xn=3d0,nf=5,mxpart=12)
      double precision two,aveqq,aveqg
      parameter (two=2d0,aveqq=0.25d0/xn**2,aveqg=0.25d0/xn/8d0)

      integer j,k,jk,polq,pol1,pol2,pol3,jp,kp
      double precision fac,fac1
      common/zz_pchoice/j,k
      double precision P(mxpart,4),qdks(mxpart,4),msq(-nf:nf,-nf:nf)
      double precision ave,v1(2),v2(2)
      double complex qqb(2,2,2,2),qbq(2,2,2,2)
      double complex qg(2,2,2,2),gq(2,2,2,2)
      double complex gqb(2,2,2,2),qbg(2,2,2,2)
      double complex qqb1(2,2,2,2),qbq1(2,2,2,2) 
      double complex qqb2(2,2,2,2),qbq2(2,2,2,2)
      double complex qg1(2,2,2,2),gq1(2,2,2,2)
      double complex qg2(2,2,2,2),gq2(2,2,2,2)
      double complex gqb1(2,2,2,2),qbg1(2,2,2,2) 
      double complex gqb2(2,2,2,2),qbg2(2,2,2,2)
      double complex amp
      double precision s127
      double complex prop34,prop56,prop127

      integer iloop,nloop
      double complex amp_SAVE(-nf:nf,-nf:nf,2,2,2,2)
      double complex zz_cpropfac
      external zz_cpropfac

      do jp=-nf,nf
      do kp=-nf,nf
      msq(jp,kp)=0d0
      enddo
      enddo
      
      amp_SAVE=0d0

      !TM set the qcd parameters here
      gsq = st_alpha*4d0*pi

      fac=-4D0*esq**2
      fac1=two*gsq*cf

      v1(1)=zz_L1
      v1(2)=r1
      v2(1)=zz_L2
      v2(2)=r2

C----Change the momenta to DKS notation 
c   We have --- q(p1)+qbar(p2)-->nu(p3)+e^+(p4)+b(p5)+bbar(p6)+g(p7)
c   DKS have--- q(q2)+qbar(q1)-->nu(q3)+e^+(q4)+b(q6)+bbar(q5)+g(q7)
      if (interference) then
         nloop=2
      else
         nloop=1
      endif

      do iloop=1,nloop

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
         do jp=1,4
            qdks(1,jp)=p(1,jp)
            qdks(2,jp)=p(2,jp)
            qdks(3,jp)=p(3,jp)
            qdks(4,jp)=p(6,jp)
            qdks(5,jp)=p(4,jp)
            qdks(6,jp)=p(5,jp)
            qdks(7,jp)=p(7,jp)
         enddo
      endif

      call zz_spinoru(7,qdks,za,zb)
c--   s returned from sprodx (common block) is 2*dot product

c--   calculate propagators
      if     (zerowidth) then
c      prop34=s(3,4)/dcmplx(s(3,4)-zmass**2,zmass*zwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-zmass**2,zmass*zwidth)
         prop34=zz_cpropfac(s(3,4),zmass,zwidth)
         prop56=zz_cpropfac(s(5,6),zmass,zwidth)
      else
         s127=s(1,2)+s(1,7)+s(2,7)
c      prop127=s127/dcmplx(s127-zmass**2,zmass*zwidth)
c      prop34=s(3,4)/dcmplx(s(3,4)-zmass**2,zmass*zwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-zmass**2,zmass*zwidth)
         prop127=zz_cpropfac(s127,zmass,zwidth)
         prop34=zz_cpropfac(s(3,4),zmass,zwidth)
         prop56=zz_cpropfac(s(5,6),zmass,zwidth)
      endif

c---case qbar-q
      call zz_zzamps(1,2,3,4,5,6,7,za,zb,qbq)
c---case q-qbar
      call zz_zzamps(2,1,3,4,5,6,7,za,zb,qqb)
c---case qbar-g
      call zz_zzamps(1,7,3,4,5,6,2,za,zb,qbg)
c---case q-g
      call zz_zzamps(7,1,3,4,5,6,2,za,zb,qg)
c---case g-q
      call zz_zzamps(7,2,3,4,5,6,1,za,zb,gq)
c---case g-qbar
      call zz_zzamps(2,7,3,4,5,6,1,za,zb,gqb)

c -- singly resonant contributions
      if (.not.dronly) then
c --- two calls to swap lepton pairs (3,4) <--> (5,6)

c     -- channel qbq         
         call zz_zzsingres(1,2,3,4,5,6,7,za,zb,qbq1)
         call zz_zzsingres(1,2,6,5,4,3,7,za,zb,qbq2)
c     -- channel qqb
         call zz_zzsingres(2,1,3,4,5,6,7,za,zb,qqb1)
         call zz_zzsingres(2,1,6,5,4,3,7,za,zb,qqb2)
c     -- channel qbg
         call zz_zzsingres(1,7,3,4,5,6,2,za,zb,qbg1)
         call zz_zzsingres(1,7,6,5,4,3,2,za,zb,qbg2)
c     -- channel qg
         call zz_zzsingres(7,1,3,4,5,6,2,za,zb,qg1)
         call zz_zzsingres(7,1,6,5,4,3,2,za,zb,qg2)
c     -- channel gq
         call zz_zzsingres(7,2,3,4,5,6,1,za,zb,gq1)
         call zz_zzsingres(7,2,6,5,4,3,1,za,zb,gq2)
c     -- channel gqb
         call zz_zzsingres(2,7,3,4,5,6,1,za,zb,gqb1)
         call zz_zzsingres(2,7,6,5,4,3,1,za,zb,gqb2)
      endif



      do j=-nf,nf
         do k=-nf,nf
           if( j .ne. 0 .and. k .ne. 0 .and. j .ne. -k) goto 19
           jk=max(j,k)
           ave=xn*aveqq
           if (j .eq. 0 .or. k .eq. 0) then
              jk=j+k
              ave=xn*aveqg
           endif
           
      if (jk .eq. 0) goto 19
        
      do polq=1,2
         do pol2=1,2
            do pol1=1,2
               do pol3=1,2
                  
                  amp=0d0
                  
                  if    ((j .lt. 0).and.(k .gt. 0)) then
c---  case qbar-q
                     if (polq .eq. 1) then
                        amp=(prop56*v2(pol1)*l(k)+q2*q(k))
     .                       *(prop34*v1(pol2)*l(k)+q1*q(k))
     .                       *qbq(polq,pol1,pol2,pol3)
                        if (.not.dronly) then
                                amp=amp+
     .                          (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                          *(prop127*v1(pol2)*l(k)+q1*q(k))*
     .                          qbq1(polq,pol1,pol2,pol3) +
     .                          (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                          *(prop127*v2(pol1)*l(k) + q2*q(k))
     .                          *qbq2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(k)+q2*q(k))
     .                            *(prop34*v1(pol2)*r(k)+q1*q(k))
     .                            *qbq(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                              amp = amp+
     .                          (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                          *(prop127*v1(pol2)*r(k)+q1*q(k))*
     .                          qbq1(polq,pol1,pol2,pol3) +
     .                          (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                          *(prop127*v2(pol1)*r(k) + q2*q(k))
     .                          *qbq2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       elseif((j .gt. 0).and.(k .lt. 0)) then
c---  case q-qbar
                          if (polq .eq. 1) then
                             amp=(prop56*v2(pol1)*l(j)+q2*q(j))
     .                            *(prop34*v1(pol2)*l(j)+q1*q(j))
     .                            *qqb(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp=amp+
     .                               (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                               *(prop127*v1(pol2)*l(j)+q1*q(j))*
     .                               qqb1(polq,pol1,pol2,pol3) +
     .                               (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                               *(prop127*v2(pol1)*l(j) + q2*q(j))
     .                               *qqb2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(j)+q2*q(j))
     .                            *(prop34*v1(pol2)*r(j)+q1*q(j))
     .                            *qqb(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp = amp+
     .                               (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                               *(prop127*v1(pol2)*r(j)+q1*q(j))*
     .                               qqb1(polq,pol1,pol2,pol3) +
     .                               (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                               *(prop127*v2(pol1)*r(j) + q2*q(j))
     .                               *qqb2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       elseif((j .lt. 0).and.(k .eq. 0)) then
c---  case qbar-g
                          if (polq .eq. 1) then
                             amp=(prop56*v2(pol1)*l(-jk)+q2*q(-jk))
     .                            *(prop34*v1(pol2)*l(-jk)+q1*q(-jk))
     .                            *qbg(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp=amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*l(-jk)+q1*q(-jk))*
     .                            qbg1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*l(-jk) + q2*q(-jk))
     .                            *qbg2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(-jk)+q2*q(-jk))
     .                            *(prop34*v1(pol2)*r(-jk)+q1*q(-jk))
     .                            *qbg(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp = amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*r(-jk)+q1*q(-jk))*
     .                            qbg1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*r(-jk) + q2*q(-jk))
     .                            *qbg2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       elseif((k .lt. 0).and.(j .eq. 0)) then
c---  case g-qbar
                          if (polq .eq. 1) then
                             amp=(prop56*v2(pol1)*l(-jk)+q2*q(-jk))
     .                            *(prop34*v1(pol2)*l(-jk)+q1*q(-jk))
     .                            *gqb(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp=amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*l(-jk)+q1*q(-jk))*
     .                            gqb1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*l(-jk) + q2*q(-jk))
     .                            *gqb2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(-jk)+q2*q(-jk))
     .                            *(prop34*v1(pol2)*r(-jk)+q1*q(-jk))
     .                            *gqb(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp = amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*r(-jk)+q1*q(-jk))*
     .                            gqb1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*r(-jk) + q2*q(-jk))
     .                            *gqb2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       elseif((j .gt. 0).and.(k .eq. 0)) then
c---  case q-g
                          if (polq .eq. 1) then
                             amp=(prop56*v2(pol1)*l(jk)+q2*q(jk))
     .                            *(prop34*v1(pol2)*l(jk)+q1*q(jk))
     .                            *qg(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp=amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*l(jk)+q1*q(jk))*
     .                            qg1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*l(jk) + q2*q(jk))
     .                            *qg2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(jk)+q2*q(jk))
     .                            *(prop34*v1(pol2)*r(jk)+q1*q(jk))
     .                            *qg(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp = amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*r(jk)+q1*q(jk))*
     .                            qg1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*r(jk) + q2*q(jk))
     .                            *qg2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       elseif((k .gt. 0).and.(j .eq. 0)) then
c---  case g-q
                          if (polq .eq. 1) then
                             amp=(prop56*v2(pol1)*l(jk)+q2*q(jk))
     .                            *(prop34*v1(pol2)*l(jk)+q1*q(jk))
     .                            *gq(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp=amp+
     .                               (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                               *(prop127*v1(pol2)*l(jk)+q1*q(jk))*
     .                               gq1(polq,pol1,pol2,pol3) +
     .                               (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                              *(prop127*v2(pol1)*l(jk) + q2*q(jk))
     .                               *gq2(polq,pol2,pol1,pol3)
                             endif
                          elseif (polq .eq. 2) then
                             amp=(prop56*v2(pol1)*r(jk)+q2*q(jk))
     .                            *(prop34*v1(pol2)*r(jk)+q1*q(jk))
     .                            *gq(polq,pol1,pol2,pol3)
                             if (.not.dronly) then
                                amp = amp+
     .                            (prop56*v1(pol2)*v2(pol1)+q2*q1)
     .                            *(prop127*v1(pol2)*r(jk)+q1*q(jk))*
     .                            gq1(polq,pol1,pol2,pol3) +
     .                            (prop34*v1(pol2)*v2(pol1)+q1*q2)
     .                            *(prop127*v2(pol1)*r(jk) + q2*q(jk))
     .                            *gq2(polq,pol2,pol1,pol3)
                             endif
                          endif
                       endif
                       
                       amp=amp*FAC
                       msq(j,k)=msq(j,k)+fac1*ave*abs(amp)**2

                                !set-up interference terms
                       if ((interference).and.(iloop.eq.1)) then
                          amp_SAVE(j,k,polq,pol1,pol2,pol3)=amp
                       elseif (iloop.eq.2) then
                          if (pol1.eq.pol2) then
                          msq(j,k)=msq(j,k)-fac1*ave*(dconjg(amp_SAVE
     &  (j,k,polq,pol1,pol2,pol3))*amp+amp_SAVE(j,k,polq,pol1,pol2,pol3)
     &                         *dconjg(amp))
                          endif
                       endif

                       
                    enddo
                 enddo
              enddo
           enddo

 19        continue
        enddo
      enddo
      
      
      enddo !iloop


c     symmetry factor
      msq=msq*vsymfact

      return
      end
      
      
      




