cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c convert POWHEG couplings and parameters into  
c format needed by EW amplitudes:
c
c adapted from subroutine "KOPPLN" in vbfnlo
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
C  Calculate the fermion-fermion-boson and three-boson couplings
C  in the standard model, without colour factors.
C  also calculate mass**2 and mass*width of the bosons.
C  V   = FFB vector coupling
C  A   = FFB axial-vector coupling
C  CLR = FFB left- and righthanded couplings
C  B   = BBB coupling
C  The fermion indices are:  1: neutrino of electron (muon,tau)
C                            2: electron (muon,tau)
C                            3: up quark (charm,top)
C                            4: down quark (strange,bottom)
C  The boson indices are     1: photon
C                            2: Z0 boson
C                            3: W+ boson
C                            4: W- boson
C                            5: gluon
C                            6: Higgs
C  The helicity indices are  -1: lefthanded
C                            +1: righthanded
C  No Kobayashi-Maskawa mixing is implemented here.
C  The W+- are identified as outgoing from the BBB vertex.
C  XM2 = boson mass**2
C  XMG = boson mass*width
c
C  The fundamental parameters:
C  vbf_wp_wp_alfa = The Q.E.D. fine structure constant
C  SIN2W = sin**2 of the weak mixing angle
C  ALFAS = the QCD fine structure constant
C  XMZ = mass of the Z0 boson in GeV
C  XMW = mass of the W+- bosons in GeV
C  XMH = mass of the Higgs boson in GeV
C  XMT = mass of the top quark in GeV
C
c
C===========================================================================
C
      SUBROUTINE vbf_wp_wp_COUP_POWHEG_TO_VBFNLO
c
      implicit none

      include 'src/Lib/vbf_wp_wp/PhysPars.h'
      include 'src/Lib/vbf_wp_wp/PhysPars_Higgs.h'
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/koppln_ew.inc'
c
c OUTPUT
c
      DOUBLE PRECISION CLR,XM2,XMG,B,V,A, XM2s(6),XMGs(6)
      COMMON /vbf_wp_wp_BKOPOU/ CLR(4,5,-1:1),XM2(6),XMG(6),B(6,6,6),V(4,5),A(4,5)
      COMMON /vbf_wp_wp_BKOPOUshort/ XM2s,XMGs
C
C local variables
      double precision e,g,s,c,g2,z,w,q
      double precision gwh,gzh,xgw,xgz,xgh
      integer i, j, k 
      double precision  zero
      parameter ( zero = 0.0d0 )
      logical cdebug
      parameter (cdebug = .false.)
c      parameter (cdebug = .true.)
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c strong coupling from powheg:
      call vbf_wp_wp_setscalesbtilde

c masses and widths:
      XMW = ph_Wmass
      XMZ = ph_Zmass
      XMH = ph_Hmass
c dummy value:
      xmt = 174.3d0

      xgw = ph_Wwidth
      xgz = ph_Zwidth
      xgh = ph_Hwidth
c
      SIN2W = ph_sthw2
      vbf_wp_wp_alfa  = ph_alphaem

      ALFAS = st_alpha

      G = SQRT(4*PI*ALFAS)
      S = SQRT(SIN2W)
      C = SQRT(1.d0 - SIN2W)
     
      E = SQRT(4.d0*PI*vbf_wp_wp_alfa)
      G2 = E/S

      Z = G2/4.d0/C
      W = G2/SQRT(8.d0)
      Q = G2*C
c
c THE FFB VECTOR COUPLING
c
      V(1,1) = zero
      V(2,1) = -E
      V(3,1) = 2.d0*E/3.d0
      V(4,1) = -E/3.d0
      V(1,2) =  Z
      V(2,2) = -Z*(1.d0 - 4.d0*SIN2W)
      V(3,2) =  Z*(1.d0 - 8.d0/3.d0*SIN2W)
      V(4,2) = -Z*(1.d0 - 4.d0/3.d0*SIN2W)
      DO 1 K = 1,4
         DO 1 J = 3,4
            V(K,J) = W
 1    continue
      V(1,5) = zero
      V(2,5) = zero
      V(3,5) = G
      V(4,5) = G
c
c THE FFB AXIAL-VECTOR COUPLING
c
      DO 2 K = 1,4
         DO 2 J = 1,5,4
            A(K,J) = zero
 2    continue
      A(1,2) = -Z
      A(2,2) =  Z
      A(3,2) = -Z
      A(4,2) =  Z
      do K = 1,4
         do J = 3,4
            A(K,J) = -W
         enddo
      enddo
c
c THE FFB LEFT- AND RIGHTHANDED COUPLINGS
c
      DO 4 K = 1,4
      DO 4 J = 1,5
      CLR(K,J,0) = zero
      CLR(K,J,-1)= V(K,J)-A(K,J)
      CLR(K,J,1) = V(K,J)+A(K,J)
    4 continue
c
c THE BBB COUPLINGS
c
      DO 5 I = 1,6
         DO 5 J = 1,6
            DO 5 K = 1,6
               B(I,J,K) = zero
 5    continue
      B(3,4,1) = -E
      B(4,1,3) = -E
      B(1,3,4) = -E
      B(1,4,3) =  E
      B(4,3,1) =  E
      B(3,1,4) =  E
      B(3,4,2) = -Q
      B(4,2,3) = -Q
      B(2,3,4) = -Q
      B(2,4,3) =  Q
      B(4,3,2) =  Q
      B(3,2,4) =  Q
      B(5,5,5) =  G
c
c Higgs couplings to W and Z
c
      GWH = G2
      GZH = GWH/C**2
      B(2,2,6) = GZH
      B(2,6,2) = GZH
      B(6,2,2) = GZH
      B(3,4,6) = GWH
      B(4,3,6) = GWH
      B(3,6,4) = GWH
      B(4,6,3) = GWH
      B(6,3,4) = GWH
      B(6,4,3) = GWH
c
c     mass and width info 
c
      XM2(1) = zero
      XM2(2) = XMZ**2
      XM2(3) = XMW**2
      XM2(4) = XM2(3)
      XM2(5) = zero
      xm2(6) = xmh**2
c
      XMG(1) = zero
      XMG(2) = XMZ*XGZ
      XMG(3) = XMW*XGW
      XMG(4) = XMG(3)
      XMG(5) = zero
      XMG(6) = xmh*xgh
c
c dummy xm2 and xmg for common block vbf_wp_wp_BKOPOUshort 
c
      do i = 1,6
         xm2s(i) = xm2(i)
      end do
      do i = 1,6
         xmgs(i) = xmg(i)
      end do     

c convert couplings into format needed by HELAS:
      call vbf_wp_wp_coup_trans

c
c print out the coupling constants
c
      if (cdebug) then
   10 FORMAT(20(1H-),a,20(1H-))
      write(6,10) ' START OF COUPLING INFO FOR EW PART'
      write(6,11) 1./vbf_wp_wp_alfa,SIN2W,ALFAS,XMZ,XMW,XMH,XMT
   11 FORMAT(/,
     & ' CALCULATE THE COUPLINGS IN THE STANDARD MODEL.'/,
     & ' THE INPUT PARAMETERS ARE :'/,
     & ' Q.E.D. FINE STRUCTURE CONSTANT  :  1/',F7.3/,
c     & ' G_FERMI                         :',1X,F14.12,' GeV^-2'/,
     & ' SIN**2 OF THE WEAK MIXING ANGLE :',F8.5/,
     & ' Q.C.D. FINE STRUCTURE CONSTANT  :',F8.5/,
     & ' MASS OF THE Z0 BOSON            :',F8.3,' GEV'/,
     & ' MASS OF THE W+/- BOSONS         :',F8.3,' GEV'/,
     & ' MASS OF THE HIGGS BOSON         :',F8.3,' GEV'/,
     & ' MASS OF THE TOP QUARK           :',F8.3,' GEV'/,
     & ' (ALL OTHER FERMIONS HAVE ZERO MASS)' )
      write(6,12) C,S,Z,W,Q
   12 FORMAT(/' C,S,Z,W,Q =',5F10.6)
      write(6,*)

         write(6,13) ((V(J,K),K=1,5),J=1,4)
         write(6,14) ((A(J,K),K=1,5),J=1,4)
         write(6,15) ((CLR(J,K,-1),K=1,5),J=1,4)
         write(6,16) ((CLR(J,K,1),K=1,5),J=1,4)
         write(6,17) (((B(I,J,K),K=1,6),J=1,6),I=1,6)

   13 FORMAT(' FFB VECTOR COUPLING ( ROW=F, COL= B )'/,4(5F10.6/))
   14 FORMAT(' FFB AXIAL-VECTOR COUPLING ( ROW=F, COL= B )'/,4(5F10.6/))
   15 FORMAT(' FFB LEFTHANDED COUPLING ( ROW=F, COL= B )'/,4(5F10.6/))
   16 FORMAT(' FFB RIGHTHANDED COUPLING ( ROW=F, COL= B )'/,4(5F10.6/))
   17 FORMAT(' BBB NON-ABELIAN COUPLINGS'/,6( 6(6F10.6/)/) )

      write(6,19) (XM2(J),XMG(J),J=1,6)
   19 FORMAT(/,' MASS**2 AND MASS*WIDTH VALUES'/,6(2F10.2/))
      write(6,20)
   20 FORMAT(20(1H-),' END OF COUPLING INFO   ',20(1H-))
   21 FORMAT(//' ',63('*'),/2(' *',61(' '),'*'/),' *       ',A,5(' '),
     &       '*'/,2(' *',61(' '),'*'/),' ',63('*') )
      endif !cdebug

      RETURN
      END
      
	
c***********************************************************************
c
c     This subroutine sets up the couplings, masses and widths 
c     needed by HELAS

      subroutine vbf_wp_wp_coup_trans
c
c***********************************************************************

      implicit none      
 
c common blocks which need to be filled for HELAS 
c (masses, widths, couplings):		
      include 'src/Lib/vbf_wp_wp/coupl.inc'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
			
      double precision g2,ee2,sw,cw,vv,sc2
			
c common block with input values (only needed for top mass xmt):
      double precision  alphas, xmt, vbf_wp_wp_alfa, xmz, xmw,s2w, xmh
      common /vbf_wp_wp_bkopin/   alphas, xmt, vbf_wp_wp_alfa, xmz, xmw, s2w, xmh
	
c common block which has been filled in koppln.f and contains 
c FFB, BBB, and Higgs couplings (but not in form needed by HELAS):	
      DOUBLE PRECISION CLR,XM2,XMG,B,V,A
      COMMON /vbf_wp_wp_BKOPOU/ CLR(4,5,-1:1),XM2(6),XMG(6),B(6,6,6),V(4,5),A(4,5)
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc           
c
c fill basic parameters:
	G      =  B(5,5,5)
	GG(1)  = -G
	GG(2)  = -G
	alfas  =  G**2/(pi*4.d0)  ! strong coupling at fixed ren.scale

	ee     = -CLR(2,1,-1)
	ee2    =  ee**2
	alpha  =  ee2/(pi*4.d0)
	
	g2     = sqrt(2.d0)*CLR(2,3,-1)
	cw     = B(2,4,3)/g2
	sin2w  = 1.d0 - cw**2
	sw     = sqrt(sin2w)
        
        gfermi = vbf_wp_wp_alfa*pi/dsqrt(2d0)/xm2(3)/(1d0-xm2(3)/xm2(2))
	
c fill masses:
	hmass = sqrt(xm2(6))
	wmass = sqrt(xm2(3))
	zmass = sqrt(xm2(2))
	amass = 0.d0
	tmass = xmt
	bmass = 4.5d0
	cmass = 0.d0
	lmass = 0.d0
		
c  fill widths:			
	hwidth = xmg(6)/hmass 
	wwidth = xmg(3)/wmass
	zwidth = xmg(2)/zmass
	awidth = 0.d0	
	lwidth = 0.d0
	twidth = 1.6d0
	
c  auxiliary variables:	
	vv  = 2.d0*wmass*sw/ee
	sc2 = sin2w*(1.d0-sin2w)
	
c  fill vbf_wp_wp_gauge couplings: 
c	fermion-fermion-vector couplings: 
	gal(1) = -dcmplx(clr(2,1,-1))	! photon
	gal(2) = -dcmplx(clr(2,1, 1))
	gau(1) = -dcmplx(clr(3,1,-1))	
	gau(2) = -dcmplx(clr(3,1, 1))	
	gad(1) = -dcmplx(clr(4,1,-1))	
	gad(2) = -dcmplx(clr(4,1, 1))	
	
	gwf(1) = -dcmplx(clr(1,3,-1))	! W+- boson (same for all fermions)
	gwf(2) = -dcmplx(clr(1,3, 1))	! zero
	
	gzn(1) = -dcmplx(clr(1,2,-1))	! Z boson
	gzn(2) = -dcmplx(clr(1,2, 1))	! zero	
	gzl(1) = -dcmplx(clr(2,2,-1))
	gzl(2) = -dcmplx(clr(2,2, 1))	
	gzu(1) = -dcmplx(clr(3,2,-1))
	gzu(2) = -dcmplx(clr(3,2, 1))	
	gzd(1) = -dcmplx(clr(4,2,-1))
	gzd(2) = -dcmplx(clr(4,2, 1))
	
c  	vector boson couplings:
	gw   = ee/sw
        gwwa = -b(3,4,1)
        gwwz = -b(3,4,2)
	
c	vbf_wp_wp_gauge-higgs boson couplings:
	gwwh  = dcmplx(b(3,4,6)*wmass)	
	gzzh  = dcmplx(b(2,2,6)*wmass)	
	ghhh  = dcmplx(-hmass**2/vv*3d0)
	gwwhh = dcmplx( ee2/sin2w*0.5d0 )
        gzzhh = dcmplx( ee2/sc2*0.5d0)
        ghhhh = ghhh/vv
	
	return
	end
	
	
	
	
	
	
	
