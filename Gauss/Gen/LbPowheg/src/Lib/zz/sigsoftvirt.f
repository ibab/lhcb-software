      subroutine zz_btildevirt(resvirt)
      implicit none
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_br.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/pwhg_st.h'
      include 'src/Lib/zz/pwhg_flg.h'
      include 'src/Lib/zz/pwhg_par.h'
      real * 8 resvirt(flst_nborn)
      real * 8 c(-6:6),gamma(-6:6),gammap(-6:6)
      integer j,jb,fl1,fl2,fl,leg,legi,legj
      real * 8 Q,I,s,etot,e,eij,virt_arr(flst_nborn),tot,kij,arglog,
     #     loglog
      real * 8 pdfb1(-6:6),pdfb2(-6:6)      
      logical ini
      data ini/.true./
      real * 8 zz_DDILOG,zz_dotp
      external zz_DDILOG,zz_dotp
      save c,gamma,gammap,ini
      real * 8 tiny
      parameter (tiny=1d-14)
      real * 8 ll
      real * 8 zz_Intm_ep,zz_Int0m_0,zz_Int0m_ep,zz_Intmm_0,zz_Intmm_ep
      external zz_Intm_ep,zz_Int0m_0,zz_Int0m_ep,zz_Intmm_0,zz_Intmm_ep
      logical zz_pwhg_isfinite
      external zz_pwhg_isfinite
c from 2.100 of FNO2007
      if(ini) then
         do j=-6,6
            if(j.eq.0) then
               c(j)=ca
               gamma(j)=(11*ca-4*tf*st_nlight)/6
               gammap(j)=(67d0/9-2*pi**2/3)*ca-23d0/9*tf*st_nlight
            else
               c(j)=cf
               gamma(j)=3d0/2*cf
               gammap(j)=(13d0/2-2*pi**2/3)*cf
            endif
         enddo
         ini=.false.
      endif
c get pdfs
      call zz_pdfcall(1,kn_xb1,pdfb1)
      call zz_pdfcall(2,kn_xb2,pdfb2)      

      call zz_sigvirtual(virt_arr)

      etot=2*kn_cmpborn(0,1)
      s=etot**2
      tot=0d0
      ll = log(par_csicut**2*s/st_muren2)
      do jb=1,flst_nborn
         fl1=flst_born(1,jb)
         fl2=flst_born(2,jb)
c     initial-state parton contribution
         Q=-log(st_mufact2/st_muren2)*(gamma(fl1)+2*c(fl1)*
     1   log(par_csicut) +  gamma(fl2)+2*c(fl2)*log(par_csicut))
c     loop on final-state massless partons
         do leg=flst_lightpart,nlegborn
            fl=flst_born(leg,jb)
            e=kn_cmpborn(0,leg)
            Q=Q+gammap(fl)
     1   -log(s/st_muren2)*(gamma(fl)-2*c(fl)
     3    *log(2*e/(par_csicut*etot)))
     3   +2*c(fl)*(log(2*e/etot)**2-log(par_csicut)**2)
     4   -2*gamma(fl)*log(2*e/etot)
         enddo
c     loop on final-state massive partons
         do leg=3,flst_lightpart-1
            if (abs(flst_born(leg,jb)).le.6.and.kn_masses(leg).gt.0) 
     #              then
               Q = Q - c(flst_born(leg,jb))*
     #              (ll-0.5*zz_Intm_ep(kn_cmpborn(0,leg)))               
            endif
         enddo
         Q=Q*br_born(jb)
         I=0
         do legi=1,nlegborn
         do legj=legi+1,nlegborn
c     both particles are colored
         if (abs(flst_born(legi,jb)).le.6.and.
     #           abs(flst_born(legj,jb)).le.6) then
c     massless-massless case
            if (kn_masses(legi).eq.0.and.kn_masses(legj).eq.0) then
               kij=zz_dotp(kn_cmpborn(0,legi),kn_cmpborn(0,legj))
               eij=kn_cmpborn(0,legi)*kn_cmpborn(0,legj)
               arglog = abs(1-kij/(2*eij))
               if (arglog.lt.tiny) then
                  loglog = 0d0
               else
                  loglog = log(arglog)*log(kij/(2*eij))
               endif                     
               I=I+(1d0/2*ll**2
     #              +ll*log(kij/(2*eij))
     #              -zz_DDILOG(kij/(2*eij))+1d0/2*log(kij/(2*eij))**2
     #              -loglog)*
     #              br_bornjk(legi,legj,jb)
            endif
c     massless-massive case
            if (kn_masses(legi).eq.0.and.kn_masses(legj).gt.0) then
               I = I + (0.5*(0.5*ll**2 - pi**2/6)
     #            +0.5*zz_Int0m_0(kn_cmpborn(0,legi),kn_cmpborn(0,legj))*ll
     #            -0.5*zz_Int0m_ep(kn_cmpborn(0,legi),kn_cmpborn(0,legj)))*
     #              br_bornjk(legi,legj,jb)
            endif
c     massive-massless case
            if (kn_masses(legi).gt.0.and.kn_masses(legj).eq.0) then
               I = I + (0.5*(0.5*ll**2 - pi**2/6)
     #            +0.5*zz_Int0m_0(kn_cmpborn(0,legj),kn_cmpborn(0,legi))*ll
     #            -0.5*zz_Int0m_ep(kn_cmpborn(0,legj),kn_cmpborn(0,legi)))*
     #              br_bornjk(legj,legi,jb)
            endif
c     massive-massive case
            if (kn_masses(legi).gt.0.and.kn_masses(legj).gt.0) then
               I = I + (0.5*ll*
     #            zz_Intmm_0(kn_cmpborn(0,legi),kn_cmpborn(0,legj))
     #            -0.5*zz_Intmm_ep(kn_cmpborn(0,legi),kn_cmpborn(0,legj)))
     #            *br_bornjk(legi,legj,jb)
               
            endif
         endif
         enddo
         enddo
c         write(*,*) 'jb,Q,I',jb,Q,I
            
c we only summed over j>i, multiply by 2
         I=I*2
         resvirt(jb)=(Q+I+virt_arr(jb))*st_alpha/(2*pi)
     #       *pdfb1(fl1)*pdfb2(fl2)*kn_jacborn
         tot=tot+resvirt(jb)
      enddo
      if (.not.zz_pwhg_isfinite(tot)) then
         do jb=1,flst_nborn
            resvirt(jb)=0d0
         enddo
      endif
      end



c I_ep(k,m)
      function zz_Int0m_ep(k,m)
c               /                                           [                  ]
c               |           d phi                           [   k.m       k0   ]
c zz_Int0m_ep = -2 | d cos th  ----- log(sin th sin phi)  zz_L0^2 [ ------- - ------ ]
c               |             pi                            [ k.l m.l   k.l zz_L0 ]
c               /                                           [                  ]
c the range in phi is 0<phi<pi.
      implicit none
      real * 8 zz_Int0m_ep, k(0:3),m(0:3)
      real * 8 kh(0:3),mh(0:3),khmh,b
      integer j
      real * 8 zz_DDILOG,zz_dotp
      external zz_DDILOG,zz_dotp
      do j=0,3
         kh(j)=k(j)/k(0)
         mh(j)=m(j)/m(0)
      enddo
      b=sqrt(1-zz_dotp(mh,mh))
      khmh=zz_dotp(kh,mh)
      zz_Int0m_ep=-2*(log((1-b)/(1+b))**2/4+log(khmh/(1+b))*log(khmh/(1-b))
     # +zz_DDILOG(1-khmh/(1+b))+zz_DDILOG(1-khmh/(1-b)))
      end



c I_0(k,m)
c            /                       [                  ]
c            |           d phi       [   k.m       k0   ]
c  I_0     = | d cos th  -----  zz_L0^2 [ ------- - ------ ]
c            |             pi        [ k.l m.l   k.l zz_L0 ]
c            /                       [                  ]
c the range in phi is 0<phi<pi.
      function zz_Int0m_0(k,m)
      implicit none
      real * 8 zz_Int0m_0, k(0:3),m(0:3)
      real * 8 kh(0:3),mh(0:3)
      integer j
      real * 8 zz_dotp
      external zz_DDILOG,zz_dotp
      do j=0,3
         kh(j)=k(j)/k(0)
         mh(j)=m(j)/m(0)
      enddo
      zz_Int0m_0=log( zz_dotp(kh,mh)**2/zz_dotp(mh,mh) )
      end



      function zz_Intm_ep(p)
c               /                                              2
c               |           d phi                             p  
c  zz_Intm_ep = -2 | d cos th  ----- log(sin th sin phi) zz_L0^2  -------
c               |             pi                            (p.l)^2
c               /
c the range in phi is 0<phi<pi.
      implicit none
      real * 8 p(0:3),zz_Intm_ep
      real * 8 beta2,beta
      integer j
      beta2=0
      do j=1,3
         beta2=beta2+(p(j)/p(0))**2
      enddo
      beta=sqrt(beta2)
      zz_Intm_ep=2*log((1+beta)/(1-beta))/beta
      end


      function zz_Intmm_ep(p1,p2)
c               / 
c               |           d phi                             p1.p2  
c zz_Intmm_ep= -2 | d cos th  ----- log(sin th sin phi) k0^2 ----------
c               |             pi                           p1.k  p2.k
c               /
c p1^2>0, p2^2>0.
c The range in phi is 0<phi<pi.
      implicit none
      real * 8 p1(0:3),p2(0:3), softintmm3,zz_Intmm_ep
      real * 8 i,z,zm,zmob,zp,z1,z1ob,z2,a,b,x1,x2,b1(3),b2(3),
     1 bb1,bb2,b1b2
      integer j
      real * 8 zz_DDILOG
      external zz_DDILOG
      i(z)=-1d0/2*log((z-zm)*(zp-z)/((zp+z)*(z+zm)))**2
     # -2*zz_DDILOG(2*zm/(zp-zm)*(zp-z)/(zm+z))
     # -2*zz_DDILOG(-2*zp/(zp-zm)*(zm+z)/(zp-z))
      do j=1,3
         b1(j)=p1(j)/p1(0)
         b2(j)=p2(j)/p2(0)
      enddo
      bb1=0
      bb2=0
      b1b2=0
      do j=1,3
         bb1=bb1+b1(j)**2
         bb2=bb2+b2(j)**2
         b1b2=b1b2+b1(j)*b2(j)
      enddo
      a=bb1+bb2-2*b1b2
      x1=bb1-b1b2
      x2=bb2-b1b2
      b=bb1*bb2-b1b2**2
      zp=sqrt(a)+sqrt(a-b)
      zm=sqrt(a)-sqrt(a-b)
c zm over b (introduce to handle back-to-back case without generating 0/0)
      zmob=1/zp
      z1=sqrt(x1**2+b)-x1
c z1 over b (same as before
      z1ob=1/(sqrt(x1**2+b)+x1)
      z2=sqrt(x2**2+b)+x2
      zz_Intmm_ep=(i(z2)
     1 +1d0/2*log((z1ob-zmob)*(zp-z1)/((zp+z1)*(z1ob+zmob)))**2
     2 +2*zz_DDILOG(2*zmob/(zp-zm)*(zp-z1)/(zmob+z1ob))
     3 +2*zz_DDILOG(-2*zp/(zp-zm)*(zm+z1)/(zp-z1)) )
     4  *(1-b1b2)/sqrt(a-b)
      end

      function zz_Intmm_0(p1,p2)
c            / 
c            |           d phi        p1.p2  
c  zz_Intmm_0 = | d cos th  ----- k0^2 ----------
c            |             pi       p1.k  p2.k
c            /
c the range in phi is 0<phi<pi.
      implicit none
      real * 8 p1(0:3),p2(0:3), zz_Intmm_0
      real * 8 kh(3),gh(3),k2,g2,kg,beta
      integer j
      do j=1,3
         kh(j)=p1(j)/p1(0)
         gh(j)=p2(j)/p2(0)
      enddo
      k2=0
      g2=0
      kg=0
      do j=1,3
         k2=k2+kh(j)**2
         g2=g2+gh(j)**2
         kg=kg+kh(j)*gh(j)
      enddo
      beta=sqrt(1-(1-k2)*(1-g2)/(1-kg)**2)
      zz_Intmm_0=log((1+beta)/(1-beta))/beta
      end

