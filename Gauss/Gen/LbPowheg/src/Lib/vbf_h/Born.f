      subroutine vbf_h_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      integer j,k,mu,nu

c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_i j=B*(C_i+C_j-C_k)/2,
c where k#i,j
      call vbf_h_compborn(p,bflav,born,bbmunu)

      do j=1,nlegs
         if(abs(bflav(j)).le.6) then
            if(bflav(j).eq.0) then
               do mu=0,3
                  do nu=0,3
                     bmunu(mu,nu,j)=bbmunu(mu,nu)
                  enddo
               enddo
            endif                 
            do k=j+1,nlegs
               if (((j.eq.1).and.(k.eq.4)).or.
     #                 ((j.eq.2).and.(k.eq.5))) then   
                  colcf = cf            
               else
                  colcf = 0
               endif
               bornjk(j,k)=born*colcf
               bornjk(k,j)=bornjk(j,k)
            enddo
         endif
      enddo
      end


c     q q -> H q q 
      subroutine vbf_h_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer bflav(nleg)
      real * 8 pin(0:3,nleg)
      real * 8 amp2,born,bmunu(0:3,0:3)
      integer ferm_type(nleg)
      integer mu,nu     
c      real * 8 charge(-5:5)
      integer HZZ,HWW
      integer vbf_h_signn
      external vbf_h_signn
c      data flav
c     #     /'b~','c~','s~','u~','d~','g','d','u','s','c','b'/
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      integer ferm_charge(nleg)
      call vbf_h_particle_identif(HWW,HZZ)

c     assign momenta and flavor for vbf_h_q1_q2_to_q3_q4_H subroutine
      ferm_charge(1) = charge3(bflav(1))
      ferm_charge(2) = charge3(bflav(2))
      ferm_charge(3) = charge3(bflav(4))
      ferm_charge(4) = charge3(bflav(5))
      
      ferm_type(1) = vbf_h_signn(bflav(1))
      ferm_type(2) = vbf_h_signn(bflav(2))
      ferm_type(3) = vbf_h_signn(bflav(4))
      ferm_type(4) = vbf_h_signn(bflav(5))

c     consistency check wrt the vbf_h_q1_q2_to_q3_q4_H call
      if (bflav(3).eq.HWW) then
c     WW -> H
         if (.not.(((ferm_charge(1)-ferm_charge(3).eq.3).and.
     #           (ferm_charge(2)-ferm_charge(4).eq.-3)).or.
     #           ((ferm_charge(1)-ferm_charge(3).eq.-3).and.
     #           (ferm_charge(2)-ferm_charge(4).eq.3)))) then
            write(*,*) 'WRONG calling sequence to vbf_h_q1_q2_to_q3_q4_H: WWH'
            write(*,*) bflav
            write(*,*) ferm_charge
            stop
         endif
      else         
c     ZZ -> H
         if (.not.((ferm_charge(1)-ferm_charge(3).eq.0).or.
     #           (ferm_charge(2)-ferm_charge(4).eq.0))) then
            write(*,*) 'WRONG calling sequence to vbf_h_q1_q2_to_q3_q4_H: ZZH'
            write(*,*) bflav
            write(*,*) ferm_charge
            stop
         endif
      endif
      
      do mu=0,3
         pin(mu,1) = p(mu,1)
         pin(mu,2) = p(mu,2)
         pin(mu,3) = p(mu,4)
         pin(mu,4) = p(mu,5)
         pin(mu,5) = p(mu,3)
      enddo
      
      call vbf_h_q1_q2_to_q3_q4_H(pin,ferm_type,ferm_charge,amp2)     
      born=amp2
      do mu=0,3
         do nu=0,3
            bmunu(mu,nu)=0d0
         enddo
      enddo
      end



      subroutine vbf_h_borncolour_lh
      implicit none
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface

C We deal here with the CKM matrix elements for VBF Higgs boson production
      include 'src/Lib/vbf_h/LesHouches.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
c colours of incoming quarks, antiquarks
c      integer icolqi(2),icolai(2),icolgi(2),
c     #        icolqf(2),icolaf(2),icolgf(2)
c      data icolqi/ 501, 0   /
c      data icolai/ 0  , 502 /
c      data icolgi/ 502, 501 /
c      data icolqf/ 502, 0   /
c      data icolaf/ 0  , 501 /
c      data icolgf/ 501, 502 /
c      data icolqi/ 501, 0   /
c      data icolai/ 0  , 502 /
c      data icolqf/ 501, 0   /
c      data icolaf/ 0  , 502 /
c      save icolqi,icolai,icolgi,icolqf,icolaf,icolgf
      integer HWW,HZZ
      integer i
c neutral particles
c      icolup(1,3)=0
c      icolup(2,3)=0
c      icolup(1,4)=0
c      icolup(2,4)=0

c     fix CKM for VBF WW -> H 
      call vbf_h_particle_identif(HWW,HZZ)
      if (idup(3).eq.HWW) then   
         call vbf_h_CKM_reshuffling(idup(1),idup(4))
         call vbf_h_CKM_reshuffling(idup(2),idup(5))
      endif
      
      if(idup(1).gt.0) then
         icolup(1,1)=501
         icolup(2,1)=0
      else
         icolup(1,1)=0
         icolup(2,1)=501
      endif
      
      if(idup(2).gt.0) then
         icolup(1,2)=502
         icolup(2,2)=0
      else
         icolup(1,2)=0
         icolup(2,2)=502
      endif
c     Higgs Boson                
      icolup(1,3)=0
      icolup(2,3)=0
c     change my Higgs boson codification back to PDG
      idup(3) = 25
      
      do i=1,2
         icolup(i,4)=icolup(i,1)
         icolup(i,5)=icolup(i,2)
      enddo
      end



      subroutine vbf_h_finalize_lh
c     Set up the resonances whose vbf_h_mass must be preserved
c     on the Les Houches interface.
c     
c     Resonance Z -> e-(3) e+(4)
c      call vbf_h_add_resonance(23,3,4)
      end





c This subroutine computes the Born squared amplitude for the process
c
c q1(p1) q2(p2) -> q3(p3) q4(p4) H(p5) 
c
c with the weak boson V (W/Z) exchanged ONLY in the t-channel
c
c NO DECAY of the Higgs is implemented 
c The amplitude is computed NUMERICALLY, with the vbf_h_bra/vbf_h_ket formalism
c NO CKM matrix elements are attached to the amplitude!!
c INPUT:  
c       pphy(0:3,5) the PHYSICAL momenta of the particles
c       ferm_type(5) = +1 if fermion, -1 if antifermion
c       fermion_charge(5) = the charge of the fermions
c                         = +2/3, -1/3, -2/3, +1/3
c RETURN:
c       amp2 the amplitude squared, averaged over initial polarization and 
c       color, and summed over final polarization and color
c
c         q1 --->------------>------ q3
c                      |
c                      | V
c                      |
c                      |------- H (q5)
c                      |
c                      | V
c                      |
c         q2 --->------------>-----  q4


      subroutine vbf_h_q1_q2_to_q3_q4_H(pphy,ferm_type,fermion_charge,
     #     amp2)
      implicit none
      integer nf
      parameter (nf=5)
      integer ferm_type(nf)
      integer fermion_charge(nf)
      real * 8 pphy(0:3,nf)
      real * 8 amp2

      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3)
      real * 8 p13,p24,p5sq
c      real * 8 p12,p14,p15,p23,p25,p34,p35,p45,
      real * 8 vbf_h_dotp
c      complex * 16 vbf_h_cdotp
      complex * 16 vbf_h_ccdotp
c      complex*16 psi(2,-1:1,nf)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
c     #     psi5(2,-1:1),psi6(2,-1:1)
      complex * 16 prop13, prop24, fac, propH_sq
c      complex*16 jHdecay(0:3)
      complex*16 J24R(0:3),J24L(0:3),J13R(0:3),J13L(0:3)
      complex * 16 amp_LL, amp_LR, amp_RL, amp_RR
      integer mu,i
      real * 8 p(0:3,nf)
      integer utype_q1, utype_q2
      real * 8 q_q1,v_q1,a_q1,q_q2,v_q2,a_q2,L_q1,R_q1,L_q2,R_q2
      integer ferm_charge(nf),tmp
      real * 8 pi
      parameter (pi=3.141592653589793238462643383279502884197D0)

      logical Higgsdecay
      common/vbf_h_cHiggsdecay/Higgsdecay
      include 'src/Lib/vbf_h/PhysPars.h'
CCCCCCCCCCCCCCC   local physical parameters     CCCCCCCCCCCCCCCCCC
      real * 8 unit_e
      real * 8 cthw,sthw,mW2,mZ2,mH2
      real * 8 mZgammaZ,mWgammaW,mHgammaH
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
	      
      unit_e = ph_unit_e
      sthw = ph_sthw
      cthw = ph_cthw
      mz2 = ph_Zmass2
      mZgammaZ = ph_ZmZw
      mw2 = ph_Wmass2
      mWgammaW = ph_WmWw
      mH2 = ph_Hmass2
      mHgammaH = ph_HmHw
c      if ((ferm_type(5).ne.-1).and.(ferm_type(6).ne.1)) then
c         write(*,*) 'ERROR: this subroutine deals only with H decay'
c         stop
c      endif
     
c create a copy on local variables
      do i=1,nf
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
      enddo

c exchance order of momenta 1 and 3
      if (ferm_type(1).eq.-1) then
         if (ferm_type(3).eq.-1) then
            call vbf_h_exchange_momenta(p(0,1),p(0,3))
            tmp = ferm_charge(1)
            ferm_charge(1)=-ferm_charge(3)
            ferm_charge(3)=-tmp            
         else
            write(*,*) 'Error in the type of the quark 1-3'
            stop
         endif
      endif

c exchance order of momenta 2 and 4
      if (ferm_type(2).eq.-1) then
         if (ferm_type(4).eq.-1) then
            call vbf_h_exchange_momenta(p(0,2),p(0,4))
            tmp = ferm_charge(2)
            ferm_charge(2)=-ferm_charge(4)
            ferm_charge(4)=-tmp
         else
            write(*,*) 'Error in the type of the quark 2-4'
            stop
         endif
      endif
      
c utype = +1 if this is an up-type quark (u,c,ubar,cbar)
c utype = -1 otherwise
      if (abs(ferm_charge(1)).eq.2) then
         utype_q1 = +1
         q_q1 = 2d0/3
      elseif (abs(ferm_charge(1)).eq.1) then
         utype_q1 = -1
         q_q1 = -1d0/3
      else
         write(*,*) 'What charge is this??', ferm_charge(1)
         stop
      endif

      if (abs(ferm_charge(2)).eq.2) then
         utype_q2 = +1
         q_q2 = 2d0/3
      elseif (abs(ferm_charge(2)).eq.1) then
         utype_q2 = -1
         q_q2 = -1d0/3
      else
         write(*,*) 'What charge is this??',ferm_charge(2)
         stop
      endif
      

      v_q1 = utype_q1*1.d0/2 - 2*q_q1*sthw**2 
      a_q1 = utype_q1*1.d0/2

      v_q2 = utype_q2*1.d0/2 - 2*q_q2*sthw**2 
      a_q2 = utype_q2*1.d0/2
            
      L_q1 = v_q1 + a_q1
      R_q1 = v_q1 - a_q1

      L_q2 = v_q2 + a_q2
      R_q2 = v_q2 - a_q2

      do mu=0,3
         p1(mu) = ferm_type(1)*p(mu,1)
         p2(mu) = ferm_type(2)*p(mu,2)
         p3(mu) = ferm_type(3)*p(mu,3)
         p4(mu) = ferm_type(4)*p(mu,4)
         p5(mu) = p(mu,5)
      enddo

c      p12=vbf_h_dotp(p1,p2)
      p13=vbf_h_dotp(p1,p3)
c      p14=vbf_h_dotp(p1,p4)
c      p15=vbf_h_dotp(p1,p5)
c      p16=vbf_h_dotp(p1,p6)
c      p23=vbf_h_dotp(p2,p3)
      p24=vbf_h_dotp(p2,p4)
c      p25=vbf_h_dotp(p2,p5)
c      p26=vbf_h_dotp(p2,p6)
c      p34=vbf_h_dotp(p3,p4)
c      p35=vbf_h_dotp(p3,p5)
c      p36=vbf_h_dotp(p3,p6)
c      p45=vbf_h_dotp(p4,p5)
c      p46=vbf_h_dotp(p4,p6)
      p5sq=vbf_h_dotp(p5,p5)
      
      call vbf_h_ket(p(0,1),ferm_type(1),psi1)
      call vbf_h_ket(p(0,2),ferm_type(2),psi2)
      call vbf_h_bra(p(0,3),ferm_type(3),psi3)
      call vbf_h_bra(p(0,4),ferm_type(4),psi4)
c      call vbf_h_ket(p(0,5),ferm_type(5),psi5)
c      call vbf_h_bra(p(0,6),ferm_type(6),psi6)

c      call vbf_h_bra_gamma_ket(psi6,psi5,-1,jHdecay)
      call vbf_h_bra_gamma_ket(psi4,psi2,+1,J24R)
      call vbf_h_bra_gamma_ket(psi4,psi2,-1,J24L)
      call vbf_h_bra_gamma_ket(psi3,psi1,+1,J13R)
      call vbf_h_bra_gamma_ket(psi3,psi1,-1,J13L)
      
      if (ferm_charge(1).eq.ferm_charge(3)) then
c     Z exchange in the t channel
         if (-p13.le.0d0) then
            prop13 = 1d0/(-2*p13-mz2)        
         else
            prop13 = 1d0/dcmplx(-2*p13-mz2,mZgammaZ) 
         endif
         if (-p24.le.0d0) then
            prop24 = 1d0/(-2*p24-mz2)            
         else
            prop24 = 1d0/dcmplx(-2*p24-mz2,mZgammaZ)            
         endif
         
         amp_LL = L_q1*L_q2*vbf_h_ccdotp(J13L,J24L)
         amp_LR = L_q1*R_q2*vbf_h_ccdotp(J13L,J24R)
         amp_RL = R_q1*L_q2*vbf_h_ccdotp(J13R,J24L)
         amp_RR = R_q1*R_q2*vbf_h_ccdotp(J13R,J24R)

c     common factor
         fac = 1d0/
c     the common part of Z couplings to quarks
     #        (2*sthw*cthw)**2
c     propagators
     #        * prop13 * prop24 
c     Higgs coupling to VV
     #        * sqrt(mw2)/(sthw*cthw**2)
c     electric unit
     #        *unit_e**3
         
         amp_LL = amp_LL * fac
         amp_LR = amp_LR * fac
         amp_RL = amp_RL * fac
         amp_RR = amp_RR * fac

         amp2 = 
     #        amp_LL*DCONJG(amp_LL) + 
     #        amp_LR*DCONJG(amp_LR) + 
     #        amp_RL*DCONJG(amp_RL) + 
     #        amp_RR*DCONJG(amp_RR) 

c     1/4 from average over initial polarization
         amp2 = amp2 /4
         
      else
c     W exchange in the t channel
         if (-p13.le.0d0) then
            prop13 = 1d0/(-2*p13-mw2)            
         else
            prop13 = 1d0/dcmplx(-2*p13-mw2,mWgammaW) 
         endif
         if (-p24.le.0d0) then
            prop24 = 1d0/(-2*p24-mw2)            
         else
            prop24 = 1d0/dcmplx(-2*p24-mw2,mWgammaW)            
         endif
         
         amp_LL = vbf_h_ccdotp(J13L,J24L)

c     common factor
         fac = 1d0/
c     the common part of V couplings to quarks
     #        (2*sthw**2)
c     propagators
     #        * prop13 * prop24 
c     Higgs coupling to VV
     #        * sqrt(mw2)/(sthw)
c     electric unit
     #        *unit_e**3
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     NO CKM MATRIX ELEMENTS!!!!
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     #        * CKMaCs * CKMaDu

         amp_LL = amp_LL * fac
         amp2 = amp_LL*DCONJG(amp_LL)  
c     1/4 from average over initial polarization
         amp2 = amp2 /4         
      endif      

      if (Higgsdecay) then
c     Higgs propagator squared
         propH_sq = 1d0/((p5sq-mH2)**2 + mHgammaH**2)
         amp2 = amp2 * propH_sq
         
c     Multiply by 16 * pi * mH * gammaH  to compensate for the 
c     phase space integral in LIPSN
c     
c     Int dq^2/2pi * dOmega/(32 pi^2) * 1/((q^2-mH^2)^2+(mH*gammaH)^2)
c     
c     = 1/(16*pi*mH*gammaH)
         
         amp2 = amp2 * 16 * pi * mHgammaH
      endif
         
      end


c This subroutine returns 2*Re(M_V * M_B^*), that is twice the real part of
c the interference between the virtuals and the Born
c
c q1(p1) q2(p2) -> q3(p3) q4(p4) H(p5+p6) 
c
c with the weak boson V (W/Z) exchanged ONLY in the t-channel
c
c NO DECAY of the Higgs is implemented 
c The amplitude is computed NUMERICALLY, with the vbf_h_bra/vbf_h_ket formalism
c INPUT:  
c       pphy(0:3,6) the PHYSICAL momenta of the particles
c       ferm_type(6) = +1 if fermion, -1 if antifermion
c       fermion_charge(6) = the charge of the fermions
c                         = +2/3, -1/3, -2/3, +1/3
c       ren_scale = renormalization scale
c
c RETURN:
c       interf the interference between the virtuals and the Born
c       averaged over initial polarization and 
c       color, and summed over final polarization and color
c                     
c                    -----
c                   /     \ <- virtual gluon
c                  /       \
c         q1 --->------------>------ q3
c                      |
c                      | V
c                      |
c                      |------- H (q5+q6)
c                      |
c                      | V
c                      |
c         q2 --->------------>-----  q4
c
c  Same for the lower quark line
c
c
c  The vertex correction depicted below is given by:
c                    -----
c                   /     \ <- virtual gluon
c                  /       \
c         q1 --->------------>------ q3
c                      |
c                      V q 
c                      |
c   q1^2 = q2^2 = 0         q = q1-q3  
c   q^2 < 0
c
c     V^\nu = Gamma(1+ep) (4*Pi)^ep * CF * as/(4*Pi) * 
c             (-2/ep^2+(-2*ln(r)-3)/ep-ln(r)^2-3*ln(r)+1/3*Pi^2-7)*B^\nu
c
c     where B^\nu is the Born vertex and r = mu^2/(-q^2)
c     See my formula (2.11) in Phys.Rev.D68:073005,2003 [hep-ph/0306109] 
c
c     The factor Gamma(1+ep) (4*Pi)^ep IS NOT RETURNED by this subroutine
c     and it's thought as factorized in front of the real counterterms too.
c
      subroutine vbf_h_q1_q2_to_q3_q4_H_virt(pphy,ferm_type,
     #     fermion_charge,ren_scale, interf)
      implicit none
      integer nf
      parameter (nf=5)
      integer ferm_type(nf)
      integer fermion_charge(nf)
      real * 8 pphy(0:3,nf)
      real * 8 ren_scale,interf
      include 'src/Lib/vbf_h/PhysPars.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      real * 8 ampborn2,interf1,interf2,q2_up,q2_down,vbf_h_dotp,r,lr
      real * 8 cvirt
      parameter (cvirt = pi**2/3 - 7)
      real * 8 gs

      gs = sqrt(4*pi*st_alpha)
      
      call vbf_h_q1_q2_to_q3_q4_H(pphy,ferm_type,fermion_charge,
     #     ampborn2)

c     squared momentum of the weak boson connected with the upper line
      q2_up = -2*vbf_h_dotp(pphy(0,1),pphy(0,3))
c     squared momentum of the weak boson connected with the lower line
      q2_down = -2*vbf_h_dotp(pphy(0,2),pphy(0,4))
      
c upper-line correction 
      r = ren_scale**2/(-q2_up)
      if (r.lt.0d0) then
         write(*,*) 
     #        'Error in vbf_h_q1_q2_to_q3_q4_H_virt: q2_up should be < 0!!'
      endif      
      lr = log(r)
      interf1 = gs**2/(4*pi)**2 * CF * (-lr**2-3*lr+cvirt) * ampborn2
      
c lower-line correction 
      r = ren_scale**2/(-q2_down)
      if (r.lt.0d0) then
         write(*,*) 
     #        'Error in vbf_h_q1_q2_to_q3_q4_H_virt: q2_down should be < 0!!'
      endif      
      lr = log(r)
      interf2 = gs**2/(4*pi)**2 * CF * (-lr**2-3*lr+cvirt) * ampborn2
    
c     2 * Re[M_V * M_B^*]
      interf = 2 * (interf1 + interf2)
      end




      function vbf_h_signn(j)
      implicit none
      integer vbf_h_signn, j
      if (j.eq.0) then
         vbf_h_signn =0
      else
         vbf_h_signn =j/abs(j)
      endif
      end



      subroutine vbf_h_CKM_reshuffling(flin,flout)
      implicit none
      integer flin,flout
      integer i,j
      logical ini
      real * 8 CKM_sq(6,6)
      include 'src/Lib/vbf_h/PhysPars.h' 
      integer vbf_h_signn
      external vbf_h_signn
      save CKM_sq,ini
      data ini/.true./
      integer nf_max
c     no initial or final state t quark!!
      nf_max = 5
      if (ini) then
         do j=1,nf_max
            do i=1,nf_max
               CKM_sq(i,j) = ph_CKM_matrix(i,j)**2
            enddo
         enddo
         ini = .false.
      endif      
      call vbf_h_pick_random(nf_max,CKM_sq(1,abs(flin)),flout)
      flout = flout * vbf_h_signn(flin)
      end
