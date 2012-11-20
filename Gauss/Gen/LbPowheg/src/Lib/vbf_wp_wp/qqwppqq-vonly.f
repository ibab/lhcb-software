c
c finite part of virtual contributions (NOT sum of vbf_wp_wp_born + virtuals!) 
c      
      subroutine vbf_wp_wp_qqwppqq_vonly(pbar,sign, nlo, L,bos,ans)
      implicit none
c
c	Barbara Jaeger, <jaegerba@uni-mainz.de>
c	Initial version:  2008 July
c	Last modified for POWHEG:    2011 Mar 
C
C  QQWPPQQ calculates the matrix elements**2 for electroweak
c  weak boson pair production by quark quark scattering
C
C        q1 q3    ---->   q2 q4 W+ W+,   W ---> f5 f6, W ---> f7 f8
C
c  where the final state leptons are 2 charged leptons + 2 neutrinos. 
C  Crossing related processes can be computed as well. Pauli interference 
c  terms for identical fermions are neglected. In particular, only the
c  t-channel exchange of elctroweak bosons is considered. s-channel
c  production of 3 weak bosons is NOT implemented.

      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'

      include 'src/Lib/vbf_wp_wp/global.inc'
      include 'src/Lib/vbf_wp_wp/tensorww.inc'
c
c electroweak couplings are taken from KOPPLN
c
      double precision  clr, xm2, xmg, b, v, a
      COMMON /vbf_wp_wp_BKOPOU/   CLR(4,5,-1:1),XM2(6),XMG(6),B(6,6,6),
     1                  V(4,5),A(4,5)
c alfas, scales etc:
      include 'src/Lib/vbf_wp_wp/scales.inc'
c
c variables for the main part of the program
c
      double precision  pbar(0:3,4+nv)
      integer sign(4+nv), nlo,L,bos
      double precision ans
 
      double precision  p(0:3,4+nv), p21(0:4), p43(0:4), pew(0:4),
     1                  puw(0:4), pww(0:4)
      integer  k,isig1,isig3,isig,i,mu,lk,kk
      integer js1,js3
      integer ic
      integer  ifl(4)
      double complex prop21(4), prop43(4)
      double complex mat(-1:1,-1:1,9), matv(-1:1,-1:1,9)

      double complex psi(2,-1:1,4), jqq(0:5,-1:1,2)

      double complex psiwe(2,-1:1,4),psiwu(2,-1:1,4),
     1               jew(0:5,-1:1,4),juw(0:5,-1:1,4)	 
      double precision fqwe(0:4,4),fqwu(0:4,4)
      double complex jj21we,jj21wu,jj43we,jj43wu,
     $ 		     zeu,zue
      double precision qee(0:4), quu(0:4)
      double complex prop_ee(2), prop_uu(2),zm2i(2:4)

      double complex jvew(0:5,-1:1,-1:1,4),jvuw(0:5,-1:1,-1:1,4)
      double complex j5eu(0:3,-1:1,2), j5ue(0:3,-1:1,2)
    
      double complex epsawe(0:3,2),epszwe(0:3,2),
     & 		     epswae(0:3,2),epswze(0:3,2),
     &		     epsawu(0:3,2),epszwu(0:3,2),
     & 		     epswau(0:3,2),epswzu(0:3,2)
      double complex qepszwe(2),qepszwu(2)
      double complex m1we,m2we,w2we,m1wu,m2wu,w2wu,
     &  	     m1ze,m2ze,m1ae,m2ae,m1e,m2e,
     &  	     m1zu,m2zu,m1au,m2au,m1u,m2u
      double complex m3we,m4we,w4we,m3wu,m4wu,w4wu,
     &  	     m3ze,m4ze,m3ae,m4ae,m3e,m4e,
     &  	     m3zu,m4zu,m3au,m4au,m3u,m4u
      double complex mv1we,mv2we,mv1wu,mv2wu,
     &  	     mv1ze,mv2ze,mv1ae,mv2ae,mv1e,mv2e,
     &  	     mv1zu,mv2zu,mv1au,mv2au,mv1u,mv2u
      double complex mv3we,mv4we,mv3wu,mv4wu,
     &  	     mv3ze,mv4ze,mv3ae,mv4ae,mv3e,mv4e,
     &  	     mv3zu,mv4zu,mv3au,mv4au,mv3u,mv4u

      double complex m5eu,m5ue
      
      integer ide,idu
      double complex mb(4,4),maeu,mzeu,maue,mzue
      double complex mbv(4,4,2),maeuv(2),mzeuv(2),mauev(2),mzuev(2)
      double complex mveu,mvue,xveu,xvue
      double complex mv5eu(2),mv5ue(2)
      
      double complex vbf_wp_wp_contract_Tjj, vbf_wp_wp_dotcc, vbf_wp_wp_dotrc, vbf_wp_wp_S1C
      external vbf_wp_wp_contract_Tjj, vbf_wp_wp_dotcc, vbf_wp_wp_dotrc, vbf_wp_wp_S1C
      double complex mww
      double complex m1
        
      double complex mm,mv12,mv34
      double precision res,resv
          
      logical linit
      data linit /.true./
      
c variables for powheg:
      double precision q2_up,q2_lo,rup,rlo,lrup,lrlo
      double precision cvirtl
c
c variables for virtual corrections
c   
      double precision c2,c2o4pi   
      parameter (c2=4d0/3d0, c2o4pi=c2/4d0/pi)

      logical bad_gauge,bad_gauge_sin,bad_gauge_isig
      common /vbf_wp_wp_gauge / bad_gauge,bad_gauge_sin,bad_gauge_isig

      double precision musq
      logical lgc(4)
      integer icount1,icount2,icb1,icb2
      data icb1/0/,icb2/0/,icount1/-1/,icount2/0/
      data lgc /4*.false./
      data lfp /4*.true./
c
      double precision xgc1, xgc2
      double complex  zero
      parameter (zero = (0d0,0d0) )
 
      integer icountmax
      integer lflavr(5:4+nv)  
      common/vbf_wp_wp_gauge_checks/icountmax   

      real *8 vbf_wp_wp_powheginput
      external vbf_wp_wp_powheginput 
      integer ncall2
      logical, save :: firsttime = .true. 

      save ifl, zm2i, linit, lgc, icount1, icount2, icb1, icb2
      save j5eu,j5ue,jvew,jvuw

      logical lnlo, lbox, lpent, lpt, lpq
      lnlo = NLO.ne.0    	         ! include some virtual stuff if T
      lbox = NLO.eq.1 .or. NLO.eq.-4 .or. NLO.eq.5    ! call boxline if T
      lpt  = NLO.eq.1 .or. NLO.le.-5     ! true pentagons only
      lpq  = NLO.eq.5 .or. NLO.eq.-4     ! q^mu/m_V terms of pentagons only
      lpent= lpt .or. lpq                ! include pentagons if T
c
       bad_gauge = .false. ! set F at beginning of each run
c
c----------------------------------------------------------
c----------------------------------------------------------

      if (bos.ne.33) then
        print*,'subroutine qqwppqq contains only ME for W+W+ production'
        print*,'but you called it with bos = ',bos
        stop
      endif	
		
      if (firsttime) then
        ncall2=vbf_wp_wp_powheginput('ncall2')
        firsttime = .false.
      endif
		
c-------------------------
c
c define flavors of external quarks for the 4 subprocesses
c
      if (linit) then        ! u(1)c(3)->d(2)s(4) etc.
         ifl(1) = 3
         ifl(2) = 4
         ifl(3) = 3
         ifl(4) = 4
c 	 
         zm2i(2) = 1/dcmplx(xm2(2),-xmg(2))
         zm2i(3) = 1/dcmplx(xm2(3),-xmg(3))
         zm2i(4) = 1/dcmplx(xm2(4),-xmg(4))	 

         if (ncall2.gt.10000) then 
            icountmax = ncall2
         else
            icountmax = 100000
	 endif
	 
         linit = .false.	 
      endif
    
      do isig1 = -1,1,2
         do isig3 = -1,1,2
            do i = 1,9
               mat(isig1,isig3,i) = 0
               matv(isig1,isig3,i) = 0
            enddo
         enddo
      enddo
c
c identify fermion line sign factors
c
      is1 = sign(1)
      is3 = sign(3)
      js1 = (3+sign(1))/2       ! 1 for sign1=-1,2 for sign1=+1
      js3 = (7+sign(3))/2       ! 3 for sign3=-1,4 for sign3=+1
c
c define the internal momenta
c
      do mu = 0,3
         do i = 1,4+nv
            p(mu,i) = pbar(mu,i)*sign(i)
         enddo
	 
           p21(mu) =   p(mu,2) - p(mu,1)
           p43(mu) =   p(mu,4) - p(mu,3)
           pew(mu) =   p(mu,6) - p(mu,5)
           puw(mu) =   p(mu,8) - p(mu,7)
           pww(mu) =   pew(mu) + puw(mu)
	 
      enddo
      p21(4) = p21(0)**2 - p21(1)**2 - p21(2)**2 - p21(3)**2
      p43(4) = p43(0)**2 - p43(1)**2 - p43(2)**2 - p43(3)**2
      pew(4) = pew(0)**2 - pew(1)**2 - pew(2)**2 - pew(3)**2
      puw(4) = puw(0)**2 - puw(1)**2 - puw(2)**2 - puw(3)**2
      pww(4) = pww(0)**2 - pww(1)**2 - pww(2)**2 - pww(3)**2

c get the vector boson propagator factors
c
      prop21(1) = 1/p21(4)
      prop21(2) = 1/dcmplx(p21(4)-xm2(2),xmg(2))
      prop21(3) = 1/dcmplx(p21(4)-xm2(3),xmg(3))
      prop21(4) = prop21(3)

      prop43(1) = 1/p43(4)
      prop43(2) = 1/dcmplx(p43(4)-xm2(2),xmg(2))
      prop43(3) = 1/dcmplx(p43(4)-xm2(3),xmg(3))
      prop43(4) = prop43(3)
c
c for box-box and BV graphs we need the propagators for t-channel bosons 
c between quark lines; as seen from upper line these W momenta are INCOMING. 
c They are OUTGOING as seen from lower line
      do mu = 0,3
         qee(mu) = pew(mu)+p(mu,2) - p(mu,1)	! We emitted on upper line
         quu(mu) = puw(mu)+p(mu,2) - p(mu,1)	! Wu emitted on upper line
      enddo
      
      qee(4) = qee(0)**2-qee(1)**2-qee(2)**2-qee(3)**2
      quu(4) = quu(0)**2-quu(1)**2-quu(2)**2-quu(3)**2

      prop_ee(1) = 1d0/qee(4)
      prop_ee(2) = 1d0/dcmplx(qee(4)-xm2(2),xmg(2))
      prop_uu(1) = 1d0/quu(4)
      prop_uu(2) = 1d0/dcmplx(quu(4)-xm2(2),xmg(2))
c
c get the external quark spinors (including factor sqrt(2E) )
c
      call vbf_wp_wp_PSI0M(4,pbar(0,1),sign(1),psi)
c
c get the f-fbar currents J21^mu=jqq(mu,*,1), J43^mu=jqq(mu,*,2)
c		 for upper/lower quark line in t-/u-channel: 
      call vbf_wp_wp_curr6(-1,psi(1,-1,2),p(0,2),psi(1,-1,1),p(0,1),jqq(0,-1,1))
      call vbf_wp_wp_curr6(-1,psi(1,-1,4),p(0,4),psi(1,-1,3),p(0,3),jqq(0,-1,2))
            
c ------------------------------------------------------------------------
c
c contract with vbf_wp_wp_WWtoWW tensors to get Vertex-Vertex scattering diagrams
c
      isig1 = -1
      isig3 = -1

      mww = vbf_wp_wp_contract_Tjj(wppwpp(0,0,L),jqq(0,isig1,1),jqq(0,isig3,2))
      mat(isig1,isig3,1) = mww*clr(ifl(1),3,isig1)*clr(ifl(3),3,isig3)
      matv(isig1,isig3,1) = (0d0,0d0)
c
c -------------------------------------------------------------------
c
c  prepare box diagrams: attach W+ to external spinors
c 
c      isig = -1   : lefthanded spinors only coupling to W's
c
c  Notation for virtual 2-component spinors and momenta
c
c  W->ev   attached to quark number i: psiwe(*,i) with momentum fqe(mu,i)
c  W->muvm attached to quark number i: psiwu(*,i) with momentum fqu(mu,i)
c  
c  the fermion current corresponding to a quark line with the real emitted W+
c  attached next to quark number i is stored in jwe/jwu(mu,isig,i). 
c 
c  For the virtual amplitudes the notation, e.g. jvwe(mu,isig,is,i)
c  is used for the boxline correction to a quark line with 
c  one W+ attached next to
c  quark #i and a free Lorentz index mu for the second attached EW boson. 
c  is=+-1 refers to the sign factor of this quark (vs.antiquark line).

      isig = -1
	 
      do i = 1,3,2
c W+ -> e+ve:	 
         call vbf_wp_wp_KET2C(psi(1,isig,i),.true.,p(0,i),isig,qe,we,
     1              psiwe(1,isig,i),fqwe(0,i))     
	 call vbf_wp_wp_BRA2C(psi(1,isig,i+1),.true.,p(0,i+1),isig,qe,we,
     1              psiwe(1,isig,i+1),fqwe(0,i+1))
c W+ -> mu+vmu:	 
         call vbf_wp_wp_KET2C(psi(1,isig,i),.true.,p(0,i),isig,qu,wu,
     1              psiwu(1,isig,i),fqwu(0,i))
	 call vbf_wp_wp_BRA2C(psi(1,isig,i+1),.true.,p(0,i+1),isig,qu,wu,
     1              psiwu(1,isig,i+1),fqwu(0,i+1))
     
      enddo
     
      do i = 1,3,2
c W+ -> e+ve:	
         call vbf_wp_wp_curr6(-1,psi(1,-1,i+1),p(0,i+1),
     1                 psiwe(1,-1,i),fqwe(0,i), jew(0,-1,i)   )
         call vbf_wp_wp_curr6(-1,psiwe(1,-1,i+1),fqwe(0,i+1),
     1                 psi(1,-1,i),p(0,i), jew(0,-1,i+1) )
 	 
c W+ -> mu+vmu:	
         call vbf_wp_wp_curr6(-1,psi(1,-1,i+1),p(0,i+1),
     1                 psiwu(1,-1,i),fqwu(0,i), juw(0,-1,i)   )
         call vbf_wp_wp_curr6(-1,psiwu(1,-1,i+1),fqwu(0,i+1),
     1                 psi(1,-1,i),p(0,i), juw(0,-1,i+1) )
    
      enddo
                   
      if (lbox ) then
      if (lfp(js1)) then
         do i = 1,2
            call vbf_wp_wp_boxlinec(-1,psi(1,-1,1),psi(1,-1,2),p(0,1),p(0,2),
     1                    .true., we,3-i,jew(0,-1,i), 
     2                    jvew(0,-1,is1,i) )	      

            call vbf_wp_wp_boxlinec(-1,psi(1,-1,1),psi(1,-1,2),p(0,1),p(0,2),
     1                    .true., wu,3-i,juw(0,-1,i), 
     2                    jvuw(0,-1,is1,i) )	      
         enddo !i
      endif !lfp1
      
      if (lfp(js3)) then
         do i = 3,4	 
            call vbf_wp_wp_boxlinec(-1,psi(1,-1,3),psi(1,-1,4),p(0,3),p(0,4),
     1                    .true., we,5-i,   jew(0,-1,i), 
     2                    jvew(0,-1,is3,i) )       

            call vbf_wp_wp_boxlinec(-1,psi(1,-1,3),psi(1,-1,4),p(0,3),p(0,4),
     1                    .true., wu,5-i,   juw(0,-1,i), 
     2                    jvuw(0,-1,is3,i) )       
         enddo !i	 
      endif !lfp3
      endif !lbox
      
c -------------------------------------------------------------------------
c
c  next do the box-box graphs with one boson (We/Wu) emitted from the 
c  upper and the other from the lower line. 
c	   
        isig3 = -1
        isig1 = -1 

	  do ide = 1,4
	     do idu = 1,4
	      mb(ide,idu) = 0d0
	      mbv(ide,idu,1) = 0d0
	      mbv(ide,idu,2) = 0d0
	     enddo
	  enddo   
	  
	  maeu = 0d0
	  mzeu = 0d0 
	  maue = 0d0
	  mzue = 0d0	  	   
	  
	  do ic = 1,2
	    maeuv(ic) = 0d0
	    mzeuv(ic) = 0d0 
	    mauev(ic) = 0d0
	    mzuev(ic) = 0d0
	  enddo !ic

c We from upper line, Wu from lower line:
	do ide = 1,2 ! fermion id for external We coupling
	  do idu = 3,4 ! fermion id for external Wu coupling
	     mb(ide,idu) = vbf_wp_wp_dotcc(jew(0,-1,ide),juw(0,-1,idu))
	  enddo !idu
	enddo !ide
	
c Wu from upper line, We from lower line:
	do ide = 3,4 
	  do idu = 1,2 
	     mb(ide,idu) = vbf_wp_wp_dotcc(jew(0,-1,ide),juw(0,-1,idu))
	  enddo !idu
	enddo !ide
	

c  for the q^mu*q^nu/M_V^2 terms in the Z boson propagators we need
     	jj21wu = vbf_wp_wp_dotcc(wu,jqq(0,-1,1))
     	jj21we = vbf_wp_wp_dotcc(we,jqq(0,-1,1))

     	jj43wu = vbf_wp_wp_dotcc(wu,jqq(0,-1,2))
     	jj43we = vbf_wp_wp_dotcc(we,jqq(0,-1,2))
        	
        zeu = jj21we*jj43wu*zm2i(2)
        zue = jj21wu*jj43we*zm2i(2)
	
	
	do ide = 1,2 ! fermion id for external We coupling
	do idu = 3,4 ! fermion id for external Wu coupling
	
	   maeu = maeu + mb(ide,idu)*(
     &	   	      clr(ifl(3-ide),1,-1)*clr(ifl(7-idu),1,-1)*
     &		     			     prop_ee(1))
     
	   mzeu = mzeu + ( mb(ide,idu) + 
     &	   	     	zeu*(-2*ide+3)*(-2*idu+7) )*
     &	   	     	clr(ifl(3-ide),2,-1)*clr(ifl(7-idu),2,-1)*
     &	   	     	prop_ee(2)
     			
	enddo !idu
	enddo !ide
	
	do idu = 1,2 
	do ide = 3,4 
	
	   maue = maue + mb(ide,idu)*(
     &	   	      clr(ifl(7-ide),1,-1)*clr(ifl(3-idu),1,-1)*
     &		     			     prop_uu(1))
	   mzue = mzue + ( mb(ide,idu)
     &	            +	zue*(-2*idu+3)*(-2*ide+7) )*
     &	   	     	clr(ifl(7-ide),2,-1)*clr(ifl(3-idu),2,-1)*
     &	        	prop_uu(2)
	     
	enddo !ide
	enddo !idu
		
	mat(isig1,isig3,6) = (maeu+mzeu+maue+mzue)*clr(3,3,-1)*clr(3,3,-1)


c -------------------------------

	if (lbox) then

c We from upper line, Wu from lower line:
	do ide = 1,2 ! fermion id for external We coupling
	  do idu = 3,4 ! fermion id for external Wu coupling
	     !box on upper line:
	     mbv(ide,idu,1) = vbf_wp_wp_dotcc(jvew(0,-1,is1,ide),juw(0,-1,idu))  
	     !box on lower line:
	     mbv(ide,idu,2) = vbf_wp_wp_dotcc(jew(0,-1,ide),jvuw(0,-1,is3,idu))
	  enddo !idu
	enddo !ide
	
c Wu from upper line, We from lower line:
	do ide = 3,4 
	  do idu = 1,2 
	     mbv(ide,idu,1) = vbf_wp_wp_dotcc(jew(0,-1,ide),jvuw(0,-1,is1,idu))
	     mbv(ide,idu,2) = vbf_wp_wp_dotcc(jvew(0,-1,is3,ide),juw(0,-1,idu))
	  enddo !idu
	enddo !ide


	do ide = 1,2 ! fermion id for external We coupling
	do idu = 3,4 ! fermion id for external Wu coupling
	
	  do ic = 1,2 !box on upper/lower line
	   maeuv(ic) = maeuv(ic) + mbv(ide,idu,ic)*(
     &	   	      clr(ifl(3-ide),1,-1)*clr(ifl(7-idu),1,-1)*
     &		     			     prop_ee(1))
     
	   mzeuv(ic) = mzeuv(ic) + mbv(ide,idu,ic)*
     &	   	     	clr(ifl(3-ide),2,-1)*clr(ifl(7-idu),2,-1)*
     &	   	     			     prop_ee(2)
     			
	  enddo !ic		
	enddo !idu
	enddo !ide
	
	do idu = 1,2 
	do ide = 3,4 
	
	  do ic = 1,2 !box on upper/lower line
	   mauev(ic) = mauev(ic) + mbv(ide,idu,ic)*(
     &	   	      clr(ifl(7-ide),1,-1)*clr(ifl(3-idu),1,-1)*
     &		     			     prop_uu(1))
     
	   mzuev(ic) = mzuev(ic) + mbv(ide,idu,ic)*
     &	   	     	clr(ifl(7-ide),2,-1)*clr(ifl(3-idu),2,-1)*
     &	        	prop_uu(2)
	  enddo !ic		
	     
	enddo !ide
	enddo !idu

	!box on upper line	
	matv(isig1,isig3,6) = (maeuv(1)+mzeuv(1)+mauev(1)+mzuev(1))*
     &				clr(3,3,-1)*clr(3,3,-1)

	!box on lower line	
	matv(isig1,isig3,9) = (maeuv(2)+mzeuv(2)+mauev(2)+mzuev(2))*
     &				clr(3,3,-1)*clr(3,3,-1)

	endif !lbox

c -----------------------------------------------------------------------
c
c  next do the vertex-box graphs 
c 	with one boson (We/Wu) emitted from a quark line and
c	one lepton-neutrino pair from the t-channel:

        isig3 = -1
        isig1 = -1 
	
c lk=1 and kk=2 is for "box correction" to upper line
c lk=2 and kk=1 is for "box correction" to lower line
	
	lk = 1
        kk = 3-lk
	! VW-> e ve: 	
	    call vbf_wp_wp_contract_T1j(awen(0,0,lk,L),jqq(0,-1,kk),epsawe(0,lk)) !for AW
	    call vbf_wp_wp_contract_T1j(zwen(0,0,lk,L),jqq(0,-1,kk),epszwe(0,lk)) !for ZW
	! VW-> mu vm:
	    call vbf_wp_wp_contract_T1j(awmu(0,0,lk,L),jqq(0,-1,kk),epsawu(0,lk)) !for AW
	    call vbf_wp_wp_contract_T1j(zwmu(0,0,lk,L),jqq(0,-1,kk),epszwu(0,lk)) !for ZW
	    	    
	lk = 2
        kk = 3-lk
	! VW-> e ve: 	
	    call vbf_wp_wp_contract_T1j(waen(0,0,lk,L),jqq(0,-1,kk),epswae(0,lk)) !for WA
	    call vbf_wp_wp_contract_T1j(wzen(0,0,lk,L),jqq(0,-1,kk),epswze(0,lk)) !for WZ
	! VW-> mu vm:
	    call vbf_wp_wp_contract_T1j(wamu(0,0,lk,L),jqq(0,-1,kk),epswau(0,lk)) !for WA
	    call vbf_wp_wp_contract_T1j(wzmu(0,0,lk,L),jqq(0,-1,kk),epswzu(0,lk)) !for WZ
	  	
		   
c  vbf_wp_wp_gauge term for Z/W propagator:
	  lk = 1! W from upper line
	  qepszwe(lk) = -vbf_wp_wp_dotrc(quu(0),epszwe(0,lk))*zm2i(2)
	  qepszwu(lk) = -vbf_wp_wp_dotrc(qee(0),epszwu(0,lk))*zm2i(2)
	  
	  lk = 2 ! W from lower line
	  qepszwe(lk) = -vbf_wp_wp_dotrc(qee(0),epswze(0,lk))*zm2i(2)
	  qepszwu(lk) = -vbf_wp_wp_dotrc(quu(0),epswzu(0,lk))*zm2i(2)	   
     
ccccccccccccc
	   
c vbf_wp_wp_gauge boson from upper quark line:	   
	  lk = 1 
	   	   
	  m2we = vbf_wp_wp_dotcc(jew(0,-1,1),epszwu(0,lk))    
	  m1we = vbf_wp_wp_dotcc(jew(0,-1,2),epszwu(0,lk))	    
	  w2we = qepszwu(lk)*jj21we    
	 
	  m2ze = (m2we-w2we)*clr(ifl(1),3,-1)*clr(ifl(2),2,-1)	  
	  m2ae = vbf_wp_wp_dotcc(jew(0,-1,1),epsawu(0,lk))*
     &	  	 clr(ifl(1),3,-1)*clr(ifl(2),1,-1)	  
	  m2e = m2ze+m2ae
	      
	  m1ze = (m1we+w2we)*clr(ifl(2),3,-1)*clr(ifl(1),2,-1)	  
	  m1ae = vbf_wp_wp_dotcc(jew(0,-1,2),epsawu(0,lk))*
     &	  	 clr(ifl(2),3,-1)*clr(ifl(1),1,-1)	  
	  m1e = m1ze+m1ae
	   
	   	   
	  m2wu = vbf_wp_wp_dotcc(juw(0,-1,1),epszwe(0,lk))    
	  m1wu = vbf_wp_wp_dotcc(juw(0,-1,2),epszwe(0,lk))	    
	  w2wu = qepszwe(lk)*jj21wu    
	 
	  m2zu = (m2wu-w2wu)*clr(ifl(1),3,-1)*clr(ifl(2),2,-1)	  
	  m2au = vbf_wp_wp_dotcc(juw(0,-1,1),epsawe(0,lk))*
     &	  	 clr(ifl(1),3,-1)*clr(ifl(2),1,-1)	  
	  m2u = m2zu+m2au
	      
	  m1zu = (m1wu+w2wu)*clr(ifl(2),3,-1)*clr(ifl(1),2,-1)	  
	  m1au = vbf_wp_wp_dotcc(juw(0,-1,2),epsawe(0,lk))*
     &	  	 clr(ifl(2),3,-1)*clr(ifl(1),1,-1)	  
	  m1u = m1zu+m1au
	   	   
	   
	  mat(isig1,isig3,2) =  -(m1e+m1u+m2e+m2u)*clr(3,3,-1)
	  
	  if (lbox) then
	  
	  lk = 1 
	   	   
	  mv2we = vbf_wp_wp_dotcc(jvew(0,-1,is1,1),epszwu(0,lk))    
	  mv1we = vbf_wp_wp_dotcc(jvew(0,-1,is1,2),epszwu(0,lk))
	 
	  mv2ze = mv2we*clr(ifl(1),3,-1)*clr(ifl(2),2,-1)	  
	  mv2ae = vbf_wp_wp_dotcc(jvew(0,-1,is1,1),epsawu(0,lk))*
     &	  	  clr(ifl(1),3,-1)*clr(ifl(2),1,-1)	  
	  mv2e  = mv2ze+mv2ae
	      
	  mv1ze = mv1we*clr(ifl(2),3,-1)*clr(ifl(1),2,-1)	  
	  mv1ae = vbf_wp_wp_dotcc(jvew(0,-1,is1,2),epsawu(0,lk))*
     &	  	  clr(ifl(2),3,-1)*clr(ifl(1),1,-1)	  
	  mv1e  = mv1ze+mv1ae
	  
	  	   	   
	  mv2wu = vbf_wp_wp_dotcc(jvuw(0,-1,is1,1),epszwe(0,lk))    
	  mv1wu = vbf_wp_wp_dotcc(jvuw(0,-1,is1,2),epszwe(0,lk))	    
	 
	  mv2zu = mv2wu*clr(ifl(1),3,-1)*clr(ifl(2),2,-1)	  
	  mv2au = vbf_wp_wp_dotcc(jvuw(0,-1,is1,1),epsawe(0,lk))*
     &	  	  clr(ifl(1),3,-1)*clr(ifl(2),1,-1)	  
	  mv2u  = mv2zu+mv2au
	      
	  mv1zu = mv1wu*clr(ifl(2),3,-1)*clr(ifl(1),2,-1)	  
	  mv1au = vbf_wp_wp_dotcc(jvuw(0,-1,is1,2),epsawe(0,lk))*
     &	  	 clr(ifl(2),3,-1)*clr(ifl(1),1,-1)	  
	  mv1u = mv1zu+mv1au
	   	   	  
	  matv(isig1,isig3,2) =  -(mv1e+mv1u+mv2e+mv2u)*clr(3,3,-1)
	  
	  endif !lbox
	   
ccccc
	   
c vbf_wp_wp_gauge boson from lower quark line:	   
	  lk = 2 
	   	   
	  m4we = vbf_wp_wp_dotcc(jew(0,-1,3),epswzu(0,lk))    
	  m3we = vbf_wp_wp_dotcc(jew(0,-1,4),epswzu(0,lk))	    
	  w4we = qepszwu(lk)*jj43we    
	 
	  m4ze = (m4we+w4we)*clr(ifl(3),3,-1)*clr(ifl(4),2,-1)	  
	  m4ae = vbf_wp_wp_dotcc(jew(0,-1,3),epswau(0,lk))*
     &	  	 clr(ifl(3),3,-1)*clr(ifl(4),1,-1)	  
	  m4e = m4ze+m4ae
	      
	  m3ze = (m3we-w4we)*clr(ifl(4),3,-1)*clr(ifl(3),2,-1)	  
	  m3ae = vbf_wp_wp_dotcc(jew(0,-1,4),epswau(0,lk))*
     &	  	 clr(ifl(4),3,-1)*clr(ifl(3),1,-1)	  
	  m3e = m3ze+m3ae
	   
	   	   
	  m4wu = vbf_wp_wp_dotcc(juw(0,-1,3),epswze(0,lk))    
	  m3wu = vbf_wp_wp_dotcc(juw(0,-1,4),epswze(0,lk))	    
	  w4wu = qepszwe(lk)*jj43wu 
	 
	  m4zu = (m4wu+w4wu)*clr(ifl(3),3,-1)*clr(ifl(4),2,-1)	  
	  m4au = vbf_wp_wp_dotcc(juw(0,-1,3),epswae(0,lk))*
     &	  	 clr(ifl(3),3,-1)*clr(ifl(4),1,-1)	  
	  m4u = m4zu+m4au
	      
	  m3zu = (m3wu-w4wu)*clr(ifl(4),3,-1)*clr(ifl(3),2,-1)	  
	  m3au = vbf_wp_wp_dotcc(juw(0,-1,4),epswae(0,lk))*
     &	  	 clr(ifl(4),3,-1)*clr(ifl(3),1,-1)	  
	  m3u = m3zu+m3au
	   	   
	   
	  mat(isig1,isig3,3) =  -(m3e+m3u+m4e+m4u)*clr(3,3,-1)
	
	
	  if (lbox) then
	  
	  lk = 2 
	   	   
	  mv4we = vbf_wp_wp_dotcc(jvew(0,-1,is3,3),epswzu(0,lk))    
	  mv3we = vbf_wp_wp_dotcc(jvew(0,-1,is3,4),epswzu(0,lk))
	 
	  mv4ze = mv4we*clr(ifl(3),3,-1)*clr(ifl(4),2,-1)	  
	  mv4ae = vbf_wp_wp_dotcc(jvew(0,-1,is3,3),epswau(0,lk))*
     &	  	 clr(ifl(3),3,-1)*clr(ifl(4),1,-1)	  
	  mv4e  = mv4ze+mv4ae
	      
	  mv3ze = mv3we*clr(ifl(4),3,-1)*clr(ifl(3),2,-1)	  
	  mv3ae = vbf_wp_wp_dotcc(jvew(0,-1,is3,4),epswau(0,lk))*
     &	  	  clr(ifl(4),3,-1)*clr(ifl(3),1,-1)	  
	  mv3e  = mv3ze+mv3ae
	   
	   	   
	  mv4wu = vbf_wp_wp_dotcc(jvuw(0,-1,is3,3),epswze(0,lk))    
	  mv3wu = vbf_wp_wp_dotcc(jvuw(0,-1,is3,4),epswze(0,lk))
	 
	  mv4zu = mv4wu*clr(ifl(3),3,-1)*clr(ifl(4),2,-1)	  
	  mv4au = vbf_wp_wp_dotcc(jvuw(0,-1,is3,3),epswae(0,lk))*
     &	  	  clr(ifl(3),3,-1)*clr(ifl(4),1,-1)	  
	  mv4u  = mv4zu+mv4au
	      
	  mv3zu = mv3wu*clr(ifl(4),3,-1)*clr(ifl(3),2,-1)	  
	  mv3au = vbf_wp_wp_dotcc(jvuw(0,-1,is3,4),epswae(0,lk))*
     &	  	  clr(ifl(4),3,-1)*clr(ifl(3),1,-1)	  
	  mv3u  = mv3zu+mv3au
	   	   
	   
	  matv(isig1,isig3,3) =  -(mv3e+mv3u+mv4e+mv4u)*clr(3,3,-1)
	  
	  endif !lbox
	  
c---------------------------------------------------------- c
c
c compute pentagon contributions, i.e. three bosons emitted 
c from the same quark line

      isig3 = -1 
      isig1 = -1
c
c prerequisites for virtual corrections:	
      if (lpent.and.icount1.eq.-1) then
c	 print*, 'pent. vbf_wp_wp_gauge check criterion chosen: mv5/mvc.gt.0.1d0'
         icount1 = icount1+1
      endif !icount1
      if (lpt) then
	 if (icountmax.eq.1) icountmax = 10	 
      endif !lpt   

      if (lpent ) then     
      
      if (lfp(js1)) then     ! need new pentagon graphs for 12 line
         musq = -p21(4)
	 k = 2 
         if (NLO.eq.1) then
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              we, wu, zero, zero, k, -5, j5eu(0,is1,1))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              wu, we, zero, zero, k, -5, j5ue(0,is1,1))                   
	       icount1= icount1+1
	        
  	 elseif (nlo.eq.-5) then

	       call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              wet, wut, xxew, xxuw, k, -5, j5eu(0,is1,1))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              wut, wet, xxuw, xxew, k, -5, j5ue(0,is1,1))               
	       icount1= icount1+1
	
	 else
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              wet, wut, xxew, xxuw, k, 5, j5eu(0,is1,1))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,1,2,sign, musq,
     1              wut, wet, xxuw, xxew, k, 5, j5ue(0,is1,1))               
                    
         endif !nlo

         lgc(js1) = .false.
c         lfp(js1) = .false.
        
	 if (lpt) then	 
c
c vbf_wp_wp_gauge check (compare contraction of pentagonline with momentum 
c	of attached current to corresponding boxline result 
c	... should give ratio ~1 ):

            call vbf_wp_wp_gauge_check0_5(j5eu(0,is1,1),
     &           p43(0),1,psi,p,1,2,wet,wut,0d0)
	      if(bad_gauge_sin) bad_gauge = .true.
              
	    call vbf_wp_wp_gauge_check0_5(j5ue(0,is1,1)
     &           ,p43(0),1,psi,p,1,2,wut,wet,0d0)
	      if(bad_gauge_sin) bad_gauge = .true.
	 
	    if(bad_gauge) then
               icb1 = icb1+1
               lgc(js1) = .true.
            endif

c	    if (mod(icount1,icountmax).eq.0 ) then
c     	       print*,'icb1,icount1=',icb1,icount1
c              print*,'bad vbf_wp_wp_gauge check fraction (pent upper line)',
c     #				       real(icb1)/real(icount1)
c           endif
	 endif !lpt for test

      endif !lfp1
  
     
c pentagon corr. to lower line:
       
      if (lfp(js3)) then     ! need new pentagon graphs for 34 line
         musq = -p43(4)
	 k = 2 
         if (NLO.eq.1) then
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              we, wu, zero, zero, k, -5, j5eu(0,is3,2))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              wu, we, zero, zero, k, -5, j5ue(0,is3,2)) 
	       icount2= icount2+1
  
  	 elseif (nlo.eq.-5) then
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              wet, wut, xxew, xxuw, k, -5, j5eu(0,is3,2))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              wut, wet, xxuw, xxew, k, -5, j5ue(0,is3,2))               
	       icount2= icount2+1

	 else
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              wet, wut, xxew, xxuw, k, 5, j5eu(0,is3,2))
               call vbf_wp_wp_J_virtual_tri_box_pent45(psi,p,3,4,sign, musq,
     1              wut, wet, xxuw, xxew, k, 5, j5ue(0,is3,2)) 
                   
         endif !nlo
c
         lgc(js3) = .false.
c         lfp(js3) = .false.
 
	 if (lpt) then
            call vbf_wp_wp_gauge_check0_5(j5eu(0,is3,2),
     &           p21(0),1,psi,p,3,4,wet,wut,0d0)
	      if(bad_gauge_sin) bad_gauge = .true.

	    call vbf_wp_wp_gauge_check0_5(j5ue(0,is3,2),
     &           p21(0),1,psi,p,3,4,wut,wet,0d0)
	      if(bad_gauge_sin) bad_gauge = .true.
	    
	    	    
	    if(bad_gauge) then
               icb2 = icb2+1
               lgc(js3) = .true.
            endif
	    
c	    if (mod(icount2,icountmax).eq.0 ) then
c               print*,'icb2,icount2=',icb2,icount2
c               print*,'bad vbf_wp_wp_gauge check fraction (pent lower line)',
c     #				       real(icb2)/real(icount2)
c           endif
	 endif !lpt for test

      endif !lfp3
                   
      endif !lpent

c ------------------------------

c compute matrix elements for pentagon on upper line:

      isig3 = -1 
      isig1 = -1
      
c  eps1=v,eps2=j_lower,eps3=v'
      m5ue = vbf_wp_wp_S1C(psiwu(1,isig1,2),jqq(0,isig3,2),
     &           .true.,isig1,psiwe(1,isig1,1))
      m5eu = vbf_wp_wp_S1C(psiwe(1,isig1,2),jqq(0,isig3,2),
     &           .true.,isig1,psiwu(1,isig1,1))

      mat(isig1,isig3,7) = clr(3,3,-1)**4*prop43(3)*(m5ue+m5eu)
      
      if (lpent) then
      
         mv5eu(1) = vbf_wp_wp_dotcc(j5eu(0,is1,1),jqq(0,isig3,2))
         mv5ue(1) = vbf_wp_wp_dotcc(j5ue(0,is1,1),jqq(0,isig3,2))

         matv(isig1,isig3,7) = 
     &           clr(3,3,-1)**4*prop43(3)*(mv5ue(1)+mv5eu(1))
       
      endif !lpent
 	
c ------------------------------

c compute matrix elements for pentagon on lower line:

c  eps1=v,eps2=j_upper,eps3=v'
      m5ue = vbf_wp_wp_S1C(psiwu(1,isig3,4),jqq(0,isig1,1),
     &           .true.,isig3,psiwe(1,isig3,3))
      m5eu = vbf_wp_wp_S1C(psiwe(1,isig3,4),jqq(0,isig1,1),
     &           .true.,isig3,psiwu(1,isig3,3))

      mat(isig1,isig3,8) = clr(3,3,-1)**4*prop21(3)*(m5ue+m5eu)
      
c
      if (lpent) then
    
         mv5eu(2) = vbf_wp_wp_dotcc(j5eu(0,is3,2),jqq(0,isig1,1))
         mv5ue(2) = vbf_wp_wp_dotcc(j5ue(0,is3,2),jqq(0,isig1,1))

         matv(isig1,isig3,8) = 
     &           clr(3,3,-1)**4*prop21(3)*(mv5ue(2)+mv5eu(2))
      
      endif !lpent
 
  
c -------------------------------------------------------------------
c -------------------------------------------------------------------
c     
c sum the graphs, square them and map them onto "ans" etc.

c i = 1		VV
c i = 2,3	BV
c i = 6,9	BB
c i = 7,8	P		      
        	
      if (lpt) then    
         xgc1 = real(icount1+1)/real(icount1-icb1+1)
         xgc2 = real(icount2+1)/real(icount2-icb2+1)      
      endif !lpt
		      
      res = 0d0
      resv = 0d0
      isig1 = -1
      isig3 = -1
      mm = 0d0
      do i = 1,8
     	 mm = mm +  mat(isig1,isig3,i)
      enddo 
      res = res + dreal(mm)**2 + dimag(mm)**2     

      if (lnlo) then
          mv12 = matv(isig1,isig3,2) + 
     2           matv(isig1,isig3,6) + matv(isig1,isig3,7) 
          mv34 = matv(isig1,isig3,3) +
     2           matv(isig1,isig3,9) + matv(isig1,isig3,8)
  
          ! Pentagons are considered for 'good PS points' only:
          if (lpt) then   
             if (lgc(js1)) then
                mv12=0d0
             else
                mv12=mv12*xgc1
             endif !lgc
             if (lgc(js3)) then
                mv34=0d0
             else
                mv34=mv34*xgc2
              endif !lgc
          endif !lpt

c  add vbf_wp_wp_born type term and multiply by F_q = alphas*C_2/4pi;
c  the factor pi^2/3+9/2 for the vbf_wp_wp_born term is 
c  after adding the subtraction term
c  and the counter term for the renormalization of the pdfs
c
c  comply with POWHEG normalization:
c  vertex corrections in dreg:
c
c   V^nu = (4*Pi)^ep * Gamma(1+ep) * CF * as/(4*Pi) * 
c           (-2/ep^2+(-2*ln(r)-3)/ep-ln(r)^2-3*ln(r)+Pi^2/3-7)*B^nu
c
c         = (4*Pi)^ep / Gamma(1-ep) * CF * as/(4*Pi) * 
c           (-2/ep^2+(-2*ln(r)-3)/ep-ln(r)^2-3*ln(r)+Pi^2/3-7-Pi^2/3)*B^nu
c
c     The factor  (4*Pi)^ep/Gamma(1-ep) IS NOT RETURNED by this subroutine
c     and it's thought as factorized in front of the real counterterms too.
c
c     if (4*Pi)^ep / Gamma(1-ep) is collected in front then cvirt:
c     set parameter in dred (cvirtl = -8d0)
c
c     squared momentum of the weak boson connected with the upper line
          q2_up = p21(4)
c     squared momentum of the weak boson connected with the lower line
          q2_lo = p43(4)

          rup = st_muren2/(-q2_up)
          if (rup.lt.0d0) then
              write(*,*) 'Error in vbf_wp_wp_setvirtual: q2_up should be < 0!!'
              stop
          endif      
          rlo = st_muren2/(-q2_lo)
          if (rlo.lt.0d0) then
              write(*,*) 'Error in vbf_wp_wp_setvirtual: q2_lo should be < 0!!'
              stop
          endif      
          lrup = log(rup)
          lrlo = log(rlo) 

          cvirtl = -8d0

          if (nlo.gt.0) then
             mv12 = als(1,1)*c2o4pi*
     1             ( mv12 + mm*(-lrup**2-3*lrup+cvirtl) )
             mv34 = als(2,1)*c2o4pi*
     1             ( mv34 + mm*(-lrlo**2-3*lrlo+cvirtl) )
          else
             mv12 = als(1,1)*c2o4pi*mv12
             mv34 = als(2,1)*c2o4pi*mv34
          endif !nlo
                  resv = resv + 2*dreal(mm*conjg( mv12+mv34 ))
      endif !lnlo

     	    
      if (nlo.ge.0) then
c sum of vbf_wp_wp_born+virt
c     	 res = (res+resv)*9d0	   ! 9 is the color sum factor
c virt only:
     	 res = (resv)*9d0	   ! 9 is the color sum factor
      else
     	 res = resv*9d0 	   ! 9 is the color sum factor
      endif
c
c----------------------------------------------------------
c----------------------------------------------------------
c
      ans = res	!ucds 
      
      return
      end


