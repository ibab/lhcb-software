c subroutines dijet_soft, dijet_collfsr, dijet_softcollfsr, dijet_collisrp, dijet_collisrm,
c             dijet_softcollisrp, dijet_softcollisrm 
c are called by dijet_btildereal; they return (1-y) csi^2 R_alr (if kn_emitter
c is >2, i.e. final state) or (1-y^2) csi^2 R (if kn_emitter <=2) in the
c collinear final state                      (dijet_collfsr)
c dijet_soft-collinear final state                 (dijet_softcollfsr)
c collinear initial state in the + direction (dijet_collisrp)
c collinear initial state in the - direction (dijet_collisrm)
c dijet_soft-collinear initial state +             (dijet_softcollisrp)
c dijet_soft-collinear initial state -             (dijet_softcollisrm)
c The dijet_soft limit is taken keeping y (i.e. kn_y) fixed
c The collinear limit is taken keeping csitilde (kn_csitilde) fixed
c
c The Born and real phase space must have been set before calling them.
c The subroutine fills the array argument rc(maxalr) with the alr
c contributions that have as emitter the current emitter kn_emitter.
c All the others are set to zero.
c
c
c Functions: dijet_collbtl and dijet_softbtl are required to compute the damping
c            factor in dijet_bornzerodamp. They are called by sigreal, and
c            sigreal is used by dijet_btilde (hence the btl suffix)

c dijet_softbtl: computes the same quantity as dijet_soft, but does not include 
c          the luminosity
c
c dijet_collbtl: if kn_emitter is in the final state, computes the same quantity as
c          dijet_collfsr, but does not include the luminosity
c          if kn_emitter is 1 or 2  computes the same quantity as
c          dijet_collisrp or dijet_collisrm, without luminosity
c          if kn_emitter is 0 computes the average of the return values
c          of dijet_collisrp and dijet_collisrm, weighted with the factors 
c          (1+y)/2 and (1-y)/2.
c
c 
c
c Functions: dijet_collrad, dijet_softrad  are required to compute the damping
c            factor in dijet_bornzerodamp. They are called by dijet_sigreal_rad,
c            i.e. they are used in the generation of radiation.
c            They compute the same quantities as dijet_collbtl and dijet_softbtl,
c            except that they only fill alr values corresponding to
c            the current radiation underlying Born (i.e. the alr list
c            rad_alr_list(1...rad_alr_nlist)) and that share the current
c            radiation kinematics (i.e. the rad_kinreg value). Furthermore
c            they are divided by kn_csi^2(1-kn_y^2) (for initial state
c            kinematic regions) or  kn_csi^2(1-kn_y) (for final state
c            kinematic regions).

      subroutine dijet_collfsr(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      real * 8 pdf1(-6:6),pdf2(-6:6)
      integer alr
      call dijet_collfsrnopdf(rc)
      call dijet_pdfcall(1,kn_x1,pdf1)
      call dijet_pdfcall(2,kn_x2,pdf2)
      do alr=1,flst_nalr
         rc(alr)=rc(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end

      subroutine dijet_collfsrnopdf(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      integer alr,em
      real * 8 kperp(0:3),kperp2,q0,xocsi,csi,x,phi
      em=kn_emitter
      q0=2*kn_cmpborn(0,1)
      xocsi=q0/2/kn_cmpborn(0,em)
c for fsr csi is y independent
      csi=kn_csi
      phi=kn_azi
      x=csi*xocsi
      call dijet_buildkperp(em,phi,kperp,kperp2)
      do alr=1,flst_nalr
         if(em.eq.flst_emitter(alr)) then
            call dijet_collfsralr(alr,csi,xocsi,x,q0,kperp,kperp2,rc(alr))
         else
            rc(alr)=0
         endif
      enddo
      end

      subroutine dijet_buildkperp(em,phi,kperp,kperp2)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      integer em
      real * 8 phi,kperp(0:3),kperp2,dir(1:3)
c     Construct kperp; First construct a vector in the plane of p_em and
c     the third axis, orthogonal to p_em.
c     Then rotate it counterclockwise around the em direction
      kperp(1)=kn_cmpborn(1,em)
      kperp(2)=kn_cmpborn(2,em)
      kperp(3)=-(kn_cmpborn(2,em)**2+kn_cmpborn(1,em)**2)
     #/kn_cmpborn(3,em)
      kperp2=(kperp(1)**2+kperp(2)**2+kperp(3)**2)
      dir(1)=kn_cmpborn(1,em)/kn_cmpborn(0,em)
      dir(2)=kn_cmpborn(2,em)/kn_cmpborn(0,em)
      dir(3)=kn_cmpborn(3,em)/kn_cmpborn(0,em)
      call dijet_mrotate(dir,sin(phi),cos(phi),kperp(1))
      kperp(0)=0
      end
      

      subroutine dijet_softcollfsr(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      real * 8 pdf1(-6:6),pdf2(-6:6)
      integer alr
      real * 8 tmp
      tmp=kn_csi
      kn_csi=0
      call dijet_collfsrnopdf(rc)
      kn_csi=tmp
      call dijet_pdfcall(1,kn_xb1,pdf1)
      call dijet_pdfcall(2,kn_xb2,pdf2)
      do alr=1,flst_nalr
         rc(alr)=rc(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end



      subroutine dijet_collisrp(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      real * 8 rc(maxalr)
      call dijet_collisr(1,rc)
      end

      subroutine dijet_collisrm(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      real * 8 rc(maxalr)
      call dijet_collisr(2,rc)
      end

      subroutine dijet_softcollisrp(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      real * 8 pdf1(-6:6),pdf2(-6:6)
      integer alr
      real * 8 tmp
      tmp=kn_csip
      kn_csip=0
      call dijet_collisrnopdf(1,rc)
      kn_csip=tmp
      call dijet_pdfcall(1,kn_xb1,pdf1)
      call dijet_pdfcall(2,kn_xb2,pdf2)
      do alr=1,flst_nalr
         rc(alr)=rc(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end

      subroutine dijet_softcollisrm(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      real * 8 pdf1(-6:6),pdf2(-6:6)
      integer alr
      real * 8 tmp
      tmp=kn_csim
      kn_csim=0
      call dijet_collisrnopdf(2,rc)
      kn_csim=tmp
      call dijet_pdfcall(1,kn_xb1,pdf1)
      call dijet_pdfcall(2,kn_xb2,pdf2)
      do alr=1,flst_nalr
         rc(alr)=rc(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end

      subroutine dijet_collisr(i,rc)
      implicit none
      integer i
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      real * 8 pdf1(-6:6),pdf2(-6:6),x1,x2
      integer alr
      call dijet_collisrnopdf(i,rc)
      if(i.eq.1) then
         x1=kn_xb1/(1-kn_csip)
         x2=kn_xb2
      elseif(i.eq.2) then
         x1=kn_xb1
         x2=kn_xb2/(1-kn_csim)
      endif
      call dijet_pdfcall(1,x1,pdf1)
      call dijet_pdfcall(2,x2,pdf2)
      do alr=1,flst_nalr
         rc(alr)=rc(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end

      subroutine dijet_collisrnopdf(i,rc)
      implicit none
      integer i
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      real * 8 rc(maxalr)
      integer alr
      real * 8 kperp(0:3),csi,phi
      if(i.eq.1) then
         csi=kn_csip
      else
         csi=kn_csim
      endif
      phi=kn_azi
c Construct kperp
      kperp(1)=sin(phi)
      kperp(2)=cos(phi)
      kperp(3)=0
      kperp(0)=0
      do alr=1,flst_nalr
         if(flst_emitter(alr).eq.kn_emitter) then
            call dijet_collisralr(alr,i,csi,kperp,rc(alr))
         else
            rc(alr)=0
         endif
      enddo
      end



      subroutine dijet_soft(r0)
c blegs:        integer, number of legs of born
c bflav(nlegs): integer, flavours of the incoming partons, according to PDG conventions,
c               MODIFIED TO HAVE 0 FOR GLUONS (instead of 21)
c p(0:3,nleg):  real * 8, momenta, 0 is time component
c softvec(0:3): real * 8, 4-vector of dijet_soft gluon normalized to softvec(0)=1
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_br.h'
      real * 8 r0(maxalr)
      integer alr,em
      real * 8 y
      real * 8 pdf1(-6:6),pdf2(-6:6)
c Boost Born momenta to their rest frame
c find boost velocity
      em=kn_emitter
      y=kn_y
      do alr=1,flst_nalr
         call dijet_softalr(alr,em,y,r0(alr))
      enddo
      call dijet_pdfcall(1,kn_xb1,pdf1)
      call dijet_pdfcall(2,kn_xb2,pdf2)
      do alr=1,flst_nalr
         r0(alr)=r0(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo         
      end


      subroutine dijet_collfsralr(alr,csi,xocsi,x,q0,kperp,kperp2,res)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_br.h'
      include 'src/Lib/dijet/pwhg_par.h'
      integer alr
      real * 8 csi,xocsi,x,q0,kperp(0:3),kperp2,res
      integer iub,em,emflav,raflav,mu,nu
      real * 8 gtens(0:3,0:3),ap
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      save gtens
      iub=flst_alr2born(alr)
      em=flst_emitter(alr)
      emflav=flst_alr(em,alr)
      raflav=flst_alr(nlegreal,alr)
      if(emflav.eq.0.and.raflav.eq.0) then
         ap=0
         do mu=0,3
            do nu=0,3
               ap=ap+(-gtens(mu,nu)*(csi*x/(1-x)+(1-x)/xocsi)
     #+2*x*(1-x)*csi*
     #(kperp(mu)*kperp(nu))/kperp2)*br_bmunu(mu,nu,em,iub)
            enddo
         enddo
         ap=ap*2*ca
c     In case of two equal gluon we also supply e E_em^p/(E_em^p+E_rad^p)
c     factor, and divide by 2 for the identical particles
         ap=ap*(1-x)**par_2gsupp/((1-x)**par_2gsupp+x**par_2gsupp)
      elseif(emflav+raflav.eq.0) then
         ap=0d0
         do mu=0,3
            do nu=0,3
               ap=ap+(-gtens(mu,nu)
     #-4*x*(1-x)*(kperp(mu)*kperp(nu))/kperp2)*br_bmunu(mu,nu,em,iub)
            enddo
         enddo
         ap=ap*tf*csi
      elseif(raflav.eq.0.and.emflav.ne.0) then
         ap=cf*(1+(1-x)**2)/xocsi*br_born(iub)
      else
         write(*,*) 'coll (fsr): unammissible flavour structure'
      endif
c     1/(p_em . p_ra) = 1/(p_bar_em(0,em)**2* x * (1-x) * (1-y);
c     we multiply everything by (1-y) csi^2; one csi is included
c     above; the other here.
      res=ap/kn_cmpborn(0,em)**2/(xocsi*(1-x))
     #*(4*pi*st_alpha)
c provide multiplicity of emitter in underlyng Born
      res=res*flst_ubmult(alr)
      end


      subroutine dijet_collisralr(alr,i,csi,kperp,res)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_br.h'
      integer alr,i,em
      real * 8 csi,kperp(0:3),res
      integer iub,emflav,raflav,mu,nu
      real * 8 gtens(0:3,0:3),ap,x
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      save gtens
      x=1-csi
      iub=flst_alr2born(alr)
      em=flst_emitter(alr)
      emflav=flst_alr(i,alr)
      raflav=flst_alr(nlegreal,alr)
      if(emflav.eq.0.and.raflav.eq.0) then
         ap=0
         do mu=0,3
            do nu=0,3
               ap=ap+(-gtens(mu,nu)*(x+x*(1-x)**2)
     #+2*(1-x)**2/x*
     #(kperp(mu)*kperp(nu)))*br_bmunu(mu,nu,i,iub)
            enddo
         enddo
         ap=ap*2*ca
      elseif(raflav-emflav.eq.0) then
         ap=0d0
         do mu=0,3
            do nu=0,3
               ap=ap+(-gtens(mu,nu)*x
     #+4*(1-x)/x*(kperp(mu)*kperp(nu)))*br_bmunu(mu,nu,i,iub)
            enddo
         enddo
         ap=ap*cf*(1-x)
      elseif(raflav.eq.0.and.emflav.ne.0) then
         ap=cf*(1+x**2)*br_born(iub)
      elseif(emflav.eq.0.and.raflav.ne.0) then
         ap=tf*(x**2+(1-x)**2)*br_born(iub)*(1-x)
      else
         write(*,*) 'coll (isr): unammissible flavour structure'
      endif
c     In the real CM frame:
c     1/(p_ra . p_i)=1/(p^0_ra*p^0_i*(1-y))=1/(p^0_i)^2 /[(1-x)*(1-y)]
c     where 1/(p^0_i)^2=1/(p^0_1 * p^0_2)=1/(pborn^0_1 * pborn^0_2/x),
c     the last expression being boost invariant.
c     Supplying che csi^2 (1-y^2) factor in the collinear limit,
c     using csi=1-x we get
      res=ap/(kn_pborn(0,1)*kn_pborn(0,2)/x) * 2
c     strong coupling:
     # *(4*pi*st_alpha)
c     The remaining csi=1-x factor has been applied earlier
      end


      subroutine dijet_softalr(alr,em,y,res)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_br.h'
      integer alr,em
      real * 8 y,res
      integer iub,j,k
      real * 8 pjsq,sumdijinv,result
      real * 8 dijet_dotp
      external dijet_dotp
      if(flst_emitter(alr).eq.em) then
         if(flst_alr(nlegreal,alr).eq.0) then
            iub=flst_alr2born(alr)
c     loop over pairs of coloured particles
            result=0
            do j=1,nlegborn
               if(abs(flst_born(j,iub)).le.6) then
                  do k=j+1,nlegborn
                     if(abs(flst_born(k,iub)).le.6) then
                        result=result+
     #dijet_dotp(kn_cmpborn(0,j),kn_cmpborn(0,k))
     #/(dijet_dotp(kn_cmpborn(0,j),kn_softvec)*
     #dijet_dotp(kn_cmpborn(0,k),kn_softvec)) * br_bornjk(j,k,iub)
                     endif
                  enddo
               endif
            enddo
c     the previous sum should run over all indexes. Since br_bornjk is
c     symmetric, multiply by 2
            result = result*2
            do j=1,nlegborn
               if(abs(flst_born(j,iub)).le.6.and.
     #flst_born(j,iub).ne.0) then
                  pjsq=kn_cmpborn(0,j)**2-kn_cmpborn(1,j)**2-
     #kn_cmpborn(2,j)**2-kn_cmpborn(3,j)**2
                  result=result-pjsq/dijet_dotp(kn_cmpborn(0,j),
     #kn_softvec)**2*br_born(iub)*cf
               endif
            enddo
c     having chosen a dijet_soft four momentum of energy 1, supply
c     1/esoft^2. Multiply by csi^2, csi=2*esoft/q0, so net
c     factor 4/q0^2
            if(em.gt.2) then
               res=result*4/
     #(4*kn_cmpborn(0,1)*kn_cmpborn(0,2))*(1-y)
            else
               res=result*4/
     #(4*kn_cmpborn(0,1)*kn_cmpborn(0,2))*(1-y**2)
            endif
c     Coupling:
            res=res*(4*pi*st_alpha)
c     The case of the emitter being a gluon requires no special treatment here!
c     the extra (1-x) factor is simply 1!
         else
            res=0
         endif
      else
         res=0
      endif
c     Multiply dijet_soft result by (dijet_soft limit Sij appropriate factors)
      if(res.ne.0) then
         sumdijinv=0
         do j=1,flst_allreg(1,0,alr)
            if(flst_allreg(2,j,alr).eq.nlegreal) then
               sumdijinv=sumdijinv
     #+1/kn_dijterm_soft(flst_allreg(1,j,alr))
            endif
         enddo
         res=res/kn_dijterm_soft(em)/sumdijinv
      endif
      res=res*flst_ubmult(alr)
      end



      subroutine dijet_softbtl(r0)
c blegs:        integer, number of legs of born
c bflav(nlegs): integer, flavours of the incoming partons, according to PDG conventions,
c               MODIFIED TO HAVE 0 FOR GLUONS (instead of 21)
c p(0:3,nleg):  real * 8, momenta, 0 is time component
c softvec(0:3): real * 8, 4-vector of dijet_soft gluon normalized to softvec(0)=1
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_br.h'
      real * 8 r0(maxalr)
      integer alr,em
      real * 8 y
c Boost Born momenta to their rest frame
c find boost velocity
      em=kn_emitter
      y=kn_y
      do alr=1,flst_nalr
         call dijet_softalr(alr,em,y,r0(alr))
      enddo
      end


c This returns in rc the collinear approximation to
c the real cross section (multiplied by csi^2(1-y^2) or (1-y))
c to be used to construct the damping factor in the real
c cross section used in dijet_btilde
      subroutine dijet_collbtl(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      real * 8 rc(maxalr)
      integer alr,em,i
      real * 8 kperp(0:3),kperp2,q0,xocsi,csi,x,phi,r1,r2
      em=kn_emitter
      if(em.gt.2) then
         q0=2*kn_cmpborn(0,1)
         xocsi=q0/2/kn_cmpborn(0,em)
c     for fsr csi is y independent
         csi=kn_csi
         phi=kn_azi
         x=csi*xocsi
         call dijet_buildkperp(em,phi,kperp,kperp2)
         do alr=1,flst_nalr
            if(em.eq.flst_emitter(alr)) then
               call dijet_collfsralr(alr,csi,xocsi,x,q0,kperp,kperp2,rc(alr))
            else
               rc(alr)=0
            endif
         enddo
      else
         phi=kn_azi
c Construct kperp
         kperp(1)=sin(phi)
         kperp(2)=cos(phi)
         kperp(3)=0
         kperp(0)=0
         do alr=1,flst_nalr
            if(flst_emitter(alr).eq.em) then
               if(em.ne.2) then
                  i=1
                  csi=kn_csi*kn_csimaxp/kn_csimax
                  call dijet_collisralr(alr,i,csi,kperp,r1)
               endif
               if(em.ne.1) then
                  i=2
                  csi=kn_csi*kn_csimaxm/kn_csimax
                  call dijet_collisralr(alr,i,csi,kperp,r2)
               endif
               if(em.eq.0) then
                  rc(alr)=(r1*(1+kn_y)+r2*(1-kn_y))/2
               elseif(em.eq.1) then
                  rc(alr)=r1
               elseif(em.eq.2) then
                  rc(alr)=r2
               endif
            else
               rc(alr)=0
            endif
         enddo
      endif
      end

c This returns in rc the collinear approximation to
c the real cross section to be used to construct the damping factor
c in the real radiation cross section
      subroutine dijet_collrad(rc)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_rad.h'
      real * 8 rc(maxalr)
      integer alr,em
      real * 8 q0,xocsi,csi,phi,x,kperp(0:3),kperp2,r1,r2
      integer j
      do j=1,rad_alr_nlist
         alr=rad_alr_list(j)
         em=flst_emitter(alr)
c     check if emitter corresponds to current radiation region (i.e. rad_kinreg):
         if(rad_kinreg.eq.1.and.em.le.2) then
            phi=kn_azi
c     Construct kperp
            kperp(1)=sin(phi)
            kperp(2)=cos(phi)
            kperp(3)=0
            kperp(0)=0
            if(em.ne.2) then
               csi=kn_csi*kn_csimaxp/kn_csimax
               call dijet_collisralr(alr,1,csi,kperp,r1)
            endif
            if(em.ne.1) then
               csi=kn_csi*kn_csimaxm/kn_csimax
               call dijet_collisralr(alr,2,csi,kperp,r2)
            endif
            if(em.eq.0) then
               rc(alr)=(r1*(1+kn_y)+r2*(1-kn_y))/2
            elseif(em.eq.1) then
               rc(alr)=r1
            elseif(em.eq.2) then
               rc(alr)=r2
            endif
            rc(alr)=rc(alr)/(kn_csi**2*(1-kn_y**2))
         elseif(flst_lightpart+rad_kinreg-2.eq.em) then
            csi=kn_csi
            phi=kn_azi
            q0=2*kn_cmpborn(0,1)
            xocsi=q0/2/kn_cmpborn(0,em)
            x=csi*xocsi
            call dijet_buildkperp(em,phi,kperp,kperp2)
            call dijet_collfsralr(alr,csi,xocsi,x,q0,kperp,kperp2,rc(alr))
            rc(alr)=rc(alr)/csi**2/(1-kn_y)
         else
            rc(alr)=0
         endif
      enddo
      end


      subroutine dijet_softrad(r0)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_rad.h'
      real * 8 r0(maxalr)
      integer alr,em,j
      real * 8 y,csi
      y=kn_y
      csi=kn_csi
      do j=1,rad_alr_nlist
         alr=rad_alr_list(j)
         em=flst_emitter(alr)
         if(rad_kinreg.eq.1.and.em.le.2) then
            call dijet_softalr(alr,em,y,r0(alr))
            r0(alr)=r0(alr)/csi**2/(1-y**2)
         elseif(flst_lightpart+rad_kinreg-2.eq.em) then
            call dijet_softalr(alr,em,y,r0(alr))
            r0(alr)=r0(alr)/csi**2/(1-y)
         else
            r0(alr)=0
         endif
      enddo
      end
