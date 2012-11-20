      subroutine vbf_h_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2
      integer mu
      integer nf
      parameter (nf=7)
      integer ferm_type(nf),ferm_charge(nf)
      real * 8 pphy(0:3,nf)      
      integer charge3(-6:6)
      data charge3 /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      integer vbf_h_signn
      external vbf_h_signn
      integer HZZ,HWW
      call vbf_h_particle_identif(HWW,HZZ)
c     assign momenta
      do mu=0,3
         pphy(mu,1)=p(mu,1)
         pphy(mu,2)=p(mu,2)
         pphy(mu,3)=p(mu,4)
         pphy(mu,4)=p(mu,5)
         pphy(mu,5)=p(mu,3)
         pphy(mu,6)=0d0
         pphy(mu,7)=p(mu,6)
      enddo

      ferm_charge(1) = charge3(rflav(1))
      ferm_charge(2) = charge3(rflav(2))
      ferm_charge(3) = charge3(rflav(4))
      ferm_charge(4) = charge3(rflav(5))
      ferm_charge(7) = charge3(rflav(6)) 

      ferm_type(1) = vbf_h_signn(rflav(1))
      ferm_type(2) = vbf_h_signn(rflav(2))
      ferm_type(3) = vbf_h_signn(rflav(4))
      ferm_type(4) = vbf_h_signn(rflav(5))
      ferm_type(7) = vbf_h_signn(rflav(6))
           

c     consistency check wrt the vbf_h_q1_q2_to_q3_q4_H_g, vbf_h_g_q2_to_q3_q4_H_q1bar and
c     vbf_h_q1_g_to_q3_q4_H_q2bar calls

      if (rflav(3).eq.HWW) then
c     WW -> H
         if (rflav(6).eq.0) then
            if (.not.(((ferm_charge(1)-ferm_charge(3).eq.3).and.
     #              (ferm_charge(2)-ferm_charge(4).eq.-3)).or.
     #              ((ferm_charge(1)-ferm_charge(3).eq.-3).and.
     #              (ferm_charge(2)-ferm_charge(4).eq.3)))) then
               write(*,*) 'WRONG calling sequence: WWH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         elseif (rflav(1).eq.0) then
            if (.not.(((-ferm_charge(7)-ferm_charge(3).eq.3).and.
     #              (ferm_charge(2)-ferm_charge(4).eq.-3)).or.
     #              ((-ferm_charge(7)-ferm_charge(3).eq.-3).and.
     #              (ferm_charge(2)-ferm_charge(4).eq.3)))) then
               write(*,*) 'WRONG calling sequence: WWH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         elseif (rflav(2).eq.0) then
            if (.not.(((ferm_charge(1)-ferm_charge(3).eq.3).and.
     #              (-ferm_charge(7)-ferm_charge(4).eq.-3)).or.
     #              ((ferm_charge(1)-ferm_charge(3).eq.-3).and.
     #              (-ferm_charge(7)-ferm_charge(4).eq.3)))) then
               write(*,*) 'WRONG calling sequence: WWH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         endif
      else         
c     ZZ -> H
         if (rflav(6).eq.0) then
            if (.not.((ferm_charge(1)-ferm_charge(3).eq.0).or.
     #              (ferm_charge(2)-ferm_charge(4).eq.0))) then
               write(*,*) 'WRONG calling sequence: ZZH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         elseif (rflav(1).eq.0) then
            if (.not.((-ferm_charge(7)-ferm_charge(3).eq.0).or.
     #              (ferm_charge(2)-ferm_charge(4).eq.0))) then
               write(*,*) 'WRONG calling sequence: ZZH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         elseif (rflav(2).eq.0) then
            if (.not.((ferm_charge(1)-ferm_charge(3).eq.0).or.
     #              (-ferm_charge(7)-ferm_charge(4).eq.0))) then
               write(*,*) 'WRONG calling sequence: ZZH'
               write(*,*) rflav
               write(*,*) ferm_charge
               stop
            endif
         endif
      endif

      if (rflav(6).eq.0) then          
         call vbf_h_q1_q2_to_q3_q4_H_g(pphy,ferm_type,ferm_charge,amp2)
      elseif (rflav(1).eq.0) then     
         call vbf_h_g_q2_to_q3_q4_H_q1bar(pphy,ferm_type,ferm_charge,amp2)
      elseif (rflav(2).eq.0) then     
         call vbf_h_q1_g_to_q3_q4_H_q2bar(pphy,ferm_type,ferm_charge,amp2)
      else
         write(*,*) 'ERROR in vbf_h_real_ampsq'
         stop
      endif
c     cancel as/(2pi) associated with amp2. It will be put back by vbf_h_real_ampsq
      amp2 = amp2/(st_alpha/(2*pi))

      end


c This subroutine computes the squared amplitude for the process
c
c  q1(p1) q2(p2) -> q3(p3) q4(p4) H(p5+p6) g(p7) 
c
c NO DECAY of the Higgs is implemented 
c The amplitude is computed NUMERICALLY, with the vbf_h_bra/vbf_h_ket formalism
c
c INPUT:  
c       pphy(0:3,7) the PHYSICAL momenta of the particles
c       ferm_type(7) = +1 if fermion, -1 if antifermion
c       fermion_charge(7) = the charge of the fermions
c                         = +2/3, -1/3, -2/3, +1/3 (irrelevant for gluons)
c
c       flag = +1 = upper line only   gluon connected ONLY to the 
c                                     UPPER quark line
c       flag = -1 = lower line only   gluon connected ONLY to the 
c                                     LOWER quark line
c       flag = "any other number"  gluon connected to both lines
c
c RETURN:
c       amp2 the amplitude squared, averaged over initial polarization and 
c       color, and summed over final polarization and color
c
c
c                     / g(q7)
c                    /
c                   /
c                  /  
c         q1 --->------------>------ q3
c                      |
c                      |
c                      |------- H (q5+q6)
c                      |
c                      |
c         q2 --->------------>-----  q4


      subroutine vbf_h_q1_q2_to_q3_q4_H_g_flag(pphy,ferm_type,
     #     fermion_charge,flag,amp2)
      implicit none
      integer nf
      parameter (nf=7)
      integer ferm_type(nf),fermion_charge(nf)
      real * 8 pphy(0:3,nf)
      integer flag            
      real * 8 amp2

      integer upper_line_only,lower_line_only
      parameter (upper_line_only=+1,lower_line_only=-1)
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3),p7(0:3),
     #     pH(0:3)
c      include 'src/Lib/vbf_h/coupl.inc'
c      include 'src/Lib/vbf_h/couplings.h'
      real * 8 p13,p17,p24,p27,p37,p47,p56sq

      real * 8 vbf_h_dotp
      complex * 16 vbf_h_ccdotp
      complex * 16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
c      complex * 16 psi5(2,-1:1),psi6(2,-1:1)
c      complex * 16 jHdecay(0:3)
      complex * 16 J24R(0:3),J24L(0:3),J13R(0:3),J13L(0:3)
      complex * 16 J24gR(0:3),J24gL(0:3),J13gR(0:3),J13gL(0:3)
      complex * 16 amp_LL_up_line, amp_LR_up_line, amp_RL_up_line, 
     #     amp_RR_up_line
      complex * 16 amp_LL_down_line, amp_LR_down_line, amp_RL_down_line, 
     #     amp_RR_down_line
      complex * 16 prop13, prop24, fac, propH_sq
      integer mu,i
      real * 8 p(0:3,7),pp1(0:3),pp2(0:3),pp3(0:3),pp4(0:3),pp7(0:3)
      integer utype_q1, utype_q2
      real * 8 q_q1,v_q1,a_q1,q_q2,v_q2,a_q2,L_q1,R_q1,L_q2,R_q2
      integer ferm_charge(nf), tmp
      real * 8 eps(0:3,1:2)     
      integer pol

      logical Higgsdecay
      common/vbf_h_cHiggsdecay/Higgsdecay
      include 'src/Lib/vbf_h/PhysPars.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
CCCCCCCCCCCCCCC   local physical parameters     CCCCCCCCCCCCCCCCCC
      real * 8 unit_e,gs_sq
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
      gs_sq = 4*pi*st_alpha
      
c      if ((ferm_type(5).ne.-1).and.(ferm_type(6).ne.1)) then
c         write(*,*) 'ERROR: this subroutine deals only with H decay'
c         stop
c      endif


c create a copy of local variables
      do i=1,nf
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
      enddo
      do i=1,nf
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
         p6(mu) = p(mu,6)
         p7(mu) = p(mu,7)
      enddo

      do mu=0,3
         pH(mu) = p5(mu)+p6(mu)
      enddo

c      p12=vbf_h_dotp(p1,p2)
      p13=vbf_h_dotp(p1,p3)
c      p14=vbf_h_dotp(p1,p4)
c      p15=vbf_h_dotp(p1,p5)
c      p16=vbf_h_dotp(p1,p6)
      p17=vbf_h_dotp(p1,p7)
c      p23=vbf_h_dotp(p2,p3)
      p24=vbf_h_dotp(p2,p4)
c      p25=vbf_h_dotp(p2,p5)
c      p26=vbf_h_dotp(p2,p6)
      p27=vbf_h_dotp(p2,p7)
c      p34=vbf_h_dotp(p3,p4)
c      p35=vbf_h_dotp(p3,p5)
c      p36=vbf_h_dotp(p3,p6)
      p37=vbf_h_dotp(p3,p7)
c      p45=vbf_h_dotp(p4,p5)
c      p46=vbf_h_dotp(p4,p6)
      p47=vbf_h_dotp(p4,p7)
      p56sq=vbf_h_dotp(pH,pH)
c      p57=vbf_h_dotp(p5,p7)
c      p67=vbf_h_dotp(p6,p7)      
      
      if (p(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -p(mu,1)
         enddo         
         call vbf_h_ket(pp1,ferm_type(1),psi1)
      else
         call vbf_h_ket(p(0,1),ferm_type(1),psi1)
      endif

      if (p(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -p(mu,2)
         enddo         
         call vbf_h_ket(pp2,ferm_type(2),psi2)
      else
         call vbf_h_ket(p(0,2),ferm_type(2),psi2)
      endif
     
      if (p(0,3).lt.0d0) then
         do mu=0,3
            pp3(mu) = -p(mu,3)
         enddo    
         call vbf_h_bra(pp3,ferm_type(3),psi3)
      else
         call vbf_h_bra(p(0,3),ferm_type(3),psi3)
      endif

      if (p(0,4).lt.0d0) then
         do mu=0,3
            pp4(mu) = -p(mu,4)
         enddo    
         call vbf_h_bra(pp4,ferm_type(4),psi4)
      else
         call vbf_h_bra(p(0,4),ferm_type(4),psi4)
      endif

c      call vbf_h_ket(p(0,5),ferm_type(5),psi5)
c      call vbf_h_bra(p(0,6),ferm_type(6),psi6)
c      call vbf_h_bra_gamma_ket(psi6,psi5,-1,jHdecay)
      
      amp2 = 0.d0

      call vbf_h_bra_gamma_ket(psi4,psi2,+1,J24R)
      call vbf_h_bra_gamma_ket(psi4,psi2,-1,J24L)
      call vbf_h_bra_gamma_ket(psi3,psi1,+1,J13R)
      call vbf_h_bra_gamma_ket(psi3,psi1,-1,J13L)
      
c     cycle over gluon polarizations
      do pol=1,2         
         amp_LL_up_line = 0d0
         amp_LR_up_line = 0d0
         amp_RL_up_line = 0d0
         amp_RR_up_line = 0d0
         amp_LL_down_line = 0d0
         amp_LR_down_line = 0d0
         amp_RL_down_line = 0d0
         amp_RR_down_line = 0d0

         if (p7(0).lt.0d0) then
            do mu=0,3
               pp7(mu) = -p7(mu)
            enddo         
            call vbf_h_POLVEC(pp7,pol,eps(0,pol))
         else
            call vbf_h_POLVEC(p7,pol,eps(0,pol))
         endif
         
         call vbf_h_bra_gamma_ket_gluon(psi3,psi1,+1,p3,p1,p7,eps(0,pol),
     #        J13gR)
         call vbf_h_bra_gamma_ket_gluon(psi3,psi1,-1,p3,p1,p7,eps(0,pol),
     #        J13gL)
         call vbf_h_bra_gamma_ket_gluon(psi4,psi2,+1,p4,p2,p7,eps(0,pol),
     #        J24gR) 
         call vbf_h_bra_gamma_ket_gluon(psi4,psi2,-1,p4,p2,p7,eps(0,pol),
     #        J24gL)          

         if (ferm_charge(1).eq.ferm_charge(3)) then
c     Z exchange in the t channel
            if (flag.ne.lower_line_only) then
c     correction to the upper line: change ONLY prop13
               if (2*p37-2*p13-2*p17.le.0d0) then
                  prop13 = 1d0/(2*p37-2*p13-2*p17-mz2)            
               else
                  prop13 = 1d0/dcmplx(2*p37-2*p13-2*p17-mz2,mZgammaZ) 
               endif
               if (-p24.le.0d0) then
                  prop24 = 1d0/(-2*p24-mz2)            
               else
                  prop24 = 1d0/dcmplx(-2*p24-mz2,mZgammaZ)            
               endif
               
               amp_LL_up_line = L_q1*L_q2*vbf_h_ccdotp(J13gL,J24L)
               amp_LR_up_line = L_q1*R_q2*vbf_h_ccdotp(J13gL,J24R)
               amp_RL_up_line = R_q1*L_q2*vbf_h_ccdotp(J13gR,J24L)
               amp_RR_up_line = R_q1*R_q2*vbf_h_ccdotp(J13gR,J24R)
               
c     common factor
               fac = 1d0/
c     the common part of Z couplings to quarks
     #              (2*sthw*cthw)**2
c     propagators
     #              * prop13 * prop24 
c     Higgs coupling to VV
     #              * sqrt(mw2)/(sthw*cthw**2)
c     electric unit
     #              *unit_e**3
         
               amp_LL_up_line = amp_LL_up_line * fac
               amp_LR_up_line = amp_LR_up_line * fac
               amp_RL_up_line = amp_RL_up_line * fac
               amp_RR_up_line = amp_RR_up_line * fac
            endif

            if (flag.ne.upper_line_only) then
c     correction to the lower line: change ONLY prop24
               if (-p13.le.0d0) then
                  prop13 = 1d0/(-2*p13-mz2)            
               else
                  prop13 = 1d0/dcmplx(-2*p13-mz2,mZgammaZ) 
               endif
               if (2*p47-2*p24-2*p27.le.0d0) then
                  prop24 = 1d0/(2*p47-2*p24-2*p27-mz2)            
               else
                  prop24 = 1d0/dcmplx(2*p47-2*p24-2*p27-mz2,mZgammaZ)
               endif
               
               amp_LL_down_line = L_q1*L_q2*vbf_h_ccdotp(J13L,J24gL)
               amp_LR_down_line = L_q1*R_q2*vbf_h_ccdotp(J13L,J24gR)
               amp_RL_down_line = R_q1*L_q2*vbf_h_ccdotp(J13R,J24gL)
               amp_RR_down_line = R_q1*R_q2*vbf_h_ccdotp(J13R,J24gR)
               
c     common factor
               fac = 1d0/
c     the common part of Z couplings to quarks
     #              (2*sthw*cthw)**2
c     propagators
     #              * prop13 * prop24 
c     Higgs coupling to VV
     #              * sqrt(mw2)/(sthw*cthw**2)
c     electric unit
     #              *unit_e**3
         
               amp_LL_down_line = amp_LL_down_line * fac
               amp_LR_down_line = amp_LR_down_line * fac
               amp_RL_down_line = amp_RL_down_line * fac
               amp_RR_down_line = amp_RR_down_line * fac
            endif

            amp2 = amp2 +
     #           abs(amp_LL_up_line)**2 + abs(amp_LL_down_line)**2 + 
     #           abs(amp_LR_up_line)**2 + abs(amp_LR_down_line)**2 + 
     #           abs(amp_RL_up_line)**2 + abs(amp_RL_down_line)**2 + 
     #           abs(amp_RR_up_line)**2 + abs(amp_RR_down_line)**2 
         else
c     W exchange in the t channel
            if (flag.ne.lower_line_only) then
c     correction to the upper line: change ONLY prop13               
               if (2*p37-2*p13-2*p17.le.0d0) then
                  prop13 = 1d0/(2*p37-2*p13-2*p17-mw2)            
               else
                  prop13 = 1d0/dcmplx(2*p37-2*p13-2*p17-mw2,mWgammaW) 
               endif
               if (-p24.le.0d0) then
                  prop24 = 1d0/(-2*p24-mw2)            
               else
                  prop24 = 1d0/dcmplx(-2*p24-mw2,mWgammaW)            
               endif
               
               amp_LL_up_line = vbf_h_ccdotp(J13gL,J24L)
               
c     common factor
               fac = 1d0/
c     the common part of V couplings to quarks
     #              (2*sthw**2)
c     propagators
     #              * prop13 * prop24 
c     Higgs coupling to VV
     #              * sqrt(mw2)/(sthw)
c     electric unit
     #              *unit_e**3
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     NO CKM MATRIX ELEMENTS!!!!
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     #              * CKMaCs * CKMaDu
               
               amp_LL_up_line = amp_LL_up_line * fac
            endif
            
            if (flag.ne.upper_line_only) then
c     correction to the lower line: change ONLY prop24               
               if (-p13.le.0d0) then
                  prop13 = 1d0/(-2*p13-mw2)            
               else
                  prop13 = 1d0/dcmplx(-2*p13-mw2,mWgammaW) 
               endif
               if (2*p47-2*p24-2*p27.le.0d0) then
                  prop24 = 1d0/(2*p47-2*p24-2*p27-mw2)            
               else
                  prop24 = 1d0/dcmplx(2*p47-2*p24-2*p27-mw2,mWgammaW)
               endif
               
               amp_LL_down_line = vbf_h_ccdotp(J13L,J24gL)
               
c     common factor
               fac = 1d0/
c     the common part of V couplings to quarks
     #              (2*sthw**2)
c     propagators
     #              * prop13 * prop24 
c     Higgs coupling to VV
     #              * sqrt(mw2)/(sthw)
c     electric unit
     #              *unit_e**3
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     NO CKM MATRIX ELEMENTS!!!!
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     #              * CKMaCs * CKMaDu
               
               amp_LL_down_line = amp_LL_down_line * fac
            endif

c            write(*,*) 'singole mie ampiezze pol', pol
c            write(*,*) amp_LL_up_line,amp_LL_down_line
            
            amp2 = amp2  
     #           + abs(amp_LL_up_line)**2 
     #           + abs(amp_LL_down_line)**2
            
         endif            
c     end loop on gluon polarization
      enddo
         
c     1/4 from average over initial polarization
      amp2 = amp2 /4 * gs_sq * CF 


      if (Higgsdecay) then
c     Higgs propagator squared
         propH_sq = 1d0/((p56sq-mH2)**2 + mHgammaH**2)
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


c     Same as vbf_h_q1_q2_to_q3_q4_H_g_flag with NO FLAG option
      subroutine vbf_h_q1_q2_to_q3_q4_H_g(pphy,fermion_type,
     #     fermion_charge,amp2)
      implicit none
      integer nf
      parameter (nf=7)
      real * 8 pphy(0:3,nf)
      integer fermion_type(nf), fermion_charge(nf)
      real * 8 amp2
      integer flag
      
      flag = 0      
      call vbf_h_q1_q2_to_q3_q4_H_g_flag(pphy,fermion_type,fermion_charge,
     #     flag,amp2)

      end


c This subroutine computes the squared amplitude for the process
c
c g(p1) q2(p2) -> q3(p3) q4(p4) H(p5+p6) q1bar(p7) 
c
c NO DECAY of the Higgs is implemented 
c The amplitude is computed NUMERICALLY, with the vbf_h_bra/vbf_h_ket formalism
c
c INPUT:  
c       pphy(0:3,7) the PHYSICAL momenta of the particles
c       ferm_type(7) = +1 if fermion, -1 if antifermion
c       fermion_charge(7) = the charge of the fermions
c                         = +2/3, -1/3, -2/3, +1/3 (irrelevant for gluons)
c
c RETURN:
c       amp2 the amplitude squared, averaged over initial polarization and 
c       color, and summed over final polarization and color
c

      subroutine vbf_h_g_q2_to_q3_q4_H_q1bar(pphy,fermion_type,fermion_charge,
     #     amp2)
      implicit none
c the 7 4-momentum vectors
c pphy(i,1) is the i-th component of vector p1... 
      integer nf
      parameter (nf=7)
      integer fermion_type(nf),fermion_charge(nf)
      real * 8 pphy(0:3,nf)
      real * 8 pp(0:3,nf)
      integer ferm_type(nf), ferm_charge(nf)
      real * 8 amp2
      integer mu,i,flag
      
      do i = 1,nf
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo
  
      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,7) = -pphy(mu,1)
         pp(mu,1) = -pphy(mu,7)
      enddo
c no useful information is in ferm_type(1) or ferm_charge(1), 
c since it's the gluon, BEFORE the following exchange
      ferm_type(1) = -ferm_type(7)
c NOTE the MINUS sign!!!
      ferm_charge(1) = -ferm_charge(7)

c flag = +1 corrections on the upper-quark line ONLY
c flag = -1 corrections on the lower-quark line ONLY

      flag = + 1
      call vbf_h_q1_q2_to_q3_q4_H_g_flag(pp,ferm_type,ferm_charge,flag,
     #     amp2)

c correct for color average
      amp2 = amp2 * 3d0/8

      end



c This subroutine computes the squared amplitude for the process
c
c q1(p1) g(p2) -> q3(p3) q4(p4) H(p5+p6) q2bar(p7) 
c
c NO DECAY of the Higgs is implemented 
c The amplitude is computed NUMERICALLY, with the vbf_h_bra/vbf_h_ket formalism
c
c INPUT:  
c       pphy(0:3,7) the PHYSICAL momenta of the particles
c       ferm_type(7) = +1 if fermion, -1 if antifermion
c       fermion_charge(7) = the charge of the fermions
c                         = +2/3, -1/3, -2/3, +1/3 (irrelevant for gluons)
c
c RETURN:
c       amp2 the amplitude squared, averaged over initial polarization and 
c       color, and summed over final polarization and color
c

      subroutine vbf_h_q1_g_to_q3_q4_H_q2bar(pphy,fermion_type,
     #     fermion_charge,amp2)
      implicit none
c the 7 4-momentum vectors
c pphy(i,1) is the i-th component of vector p1... 
      integer nf
      parameter (nf=7)
      integer fermion_type(nf), fermion_charge(nf)
      real * 8 pphy(0:3,nf)
      real * 8 pp(0:3,nf)
      integer ferm_type(nf),ferm_charge(nf)
      real * 8 amp2
      integer mu,i,flag
      
      do i = 1,nf
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo
  
      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,7) = -pphy(mu,2)
         pp(mu,2) = -pphy(mu,7)
      enddo
c no useful information is in ferm_type(2) or ferm_charge(2), 
c since it's the gluon, BEFORE the following exchange
      ferm_type(2) = -ferm_type(7)
c NOTE the MINUS sign!!!
      ferm_charge(2) = -ferm_charge(7)

c flag = +1 corrections on the upper-quark line ONLY
c flag = -1 corrections on the lower-quark line ONLY
      flag = - 1
      call vbf_h_q1_q2_to_q3_q4_H_g_flag(pp,ferm_type,ferm_charge,flag,
     #     amp2)

c correct for color average
      amp2 = amp2 * 3d0/8

      end
