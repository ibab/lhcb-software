*-----------------------------------------------------------------------
*         Very Soft Inelastic Scattering Structure Functions.
*       A.G.Shamov, BINP, Novosibirsk, Russia (shamov@inp.nsk.su).
*
* subroutine SoftSF(rS,rQs,rW1,rW2,iSF)
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Input arguments:
*   double precision rS   - Mandelstam's variable 's'
*   double precision rQs  - Q^2 = -q^2
*   integer          iSF  - Structure Function choice:
*                           0 - Suri & Yennie
*                           1 - real photon cross section for q^2=0,
*                               q^2 dependence as for Delta(1232)
*                           2 - real photon cross section for q^2=0,
*                               q^2 dependence:
*                               resonant contribution as for Delta(1232)
*                               nonresonant contribution according to S&Y
*                           3 - real photon cross section for q^2=0,
*                               q^2 dependence according to S&Y
*                           4 - real photon cross section for q^2=0,
*                               q^2 dependence:
*                               resonant contribution as for Delta(1232)
*                               some fit for nonresonant contribution
* Output arguments:
*   double precision rW1   - W_1(s,q^2)
*   double precision rW2   - W_2(s,q^2)
*
* Remarks:
*   iSF = 2,4 are for Monte Carlo only (a radon number is used to choose
*           between resonant and nonresonant contributions).
*
* About structure functions:
*   iSF = 2 seems preferable. It takes into account the production of re-
* sonances, reproduces the real photon cross section and uses theoretical
* motivation of Suri & Yennie. iSF = 3 can be used to estimate the effect
* of resonances. iSF = 1 and 4 are some extremes and are good for pissi-
* mistics (reliable) estimates of systematic errors.
*   In all structure functions the ratio of the transverse and longitudinal
* \gamma,p cross sections is fixed according to Suri & Yennie.
*
* subroutine forPERIPP(rS,rQs,iSF,rU1,rU2)
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* For use in  PERRIP routine of ZLPAIR.
*
* Input arguments:
*     as for SoftSF
*
* Output arguments:
*   double precision rU1 - U1/V1 of PERIPP
*   double precision rU2 - U2/V2 of PERIPP
*
*------------------------------------------------------------------------
* Structure function calculation
*------------------------------------------------------------------------
      subroutine SoftSF(rS, rQs, rW1, rW2, iSF)
      double precision rS,rQs,rW1,rW2
c
      common /softSFcb/ Ires  ! = 1 for a resonance produced
c
c  gamma,p cross section for inelastic process
c    Ngp - number of points  
c    gpEn(i) - CMS energy value
c    gpCS(i) - cross section in mb
c    Ngp - number of points for non-resonant fraction
c    gpBr(i) - non-resonant fraction, = 0 for i > NgpNr
c
      real gpEn,gpCS,gpNr
      common /SSF_pgamma/ Ngp,gpEn(290),gpCS(290),NgpR,gpNr(290)
c
c  magnetic form-factor for resonances
c    Ngm - Number of points
c    Gm0,GmB - fit constant for big Q^2: Gm0*exp(-GmB*q^2)
c    GmV(i) - values of Gm
C    GmQ(i) - q^2 values 
c
      real Gm0,GmB,GmV,GmQ
      common /SSF_gpgm/ Ngm,Gm0,GmB,GmV(19),GmQ(19)
c
      save /SSF_pgamma/,/SSF_gpgm/
c
      data Ngm/19/,Gm0/1./,GmB/0.984/
c
c   original data
!      data GmV/
!     * 3.000,1.770,1.380,1.170, .978, .961, .964, .766, .735, .719,
!     *  .570, .572, .553, .556, .446, .326, .269, .209, .102/
c   one point is corrected
      data GmV/
     * 3.000,1.770,1.380,1.170, .978, .961, .964, .766, .735, .719,
     *  .570, .572, .553, .460, .446, .326, .269, .209, .102/
      data GmQ/
     *  .000, .200, .300, .400, .470, .480, .500, .600, .630, .630,
     *  .770, .780, .790, .970, .980,1.150,1.340,1.570,2.340/
c
      data Ngp/290/,NgpNr/290/
      data gpCS/
     +0.000,0.052,0.108,0.175,0.424,0.487,0.527,0.478,0.407,0.334,
     +0.244,0.225,0.200,0.178,0.177,0.187,0.194,0.212,0.223,0.233,
     +0.211,0.240,0.265,0.279,0.276,0.261,0.245,0.201,0.221,0.206,
     +0.214,0.209,0.202,0.193,0.205,0.201,0.212,0.218,0.215,0.192,
     +0.191,0.175,0.165,0.182,0.159,0.162,0.150,0.149,0.144,0.156,
     +0.150,0.147,0.154,0.154,0.154,0.147,0.154,0.154,0.144,0.152,
     +0.151,0.156,0.154,0.146,0.139,0.156,0.150,0.150,0.145,0.139,
     +0.145,0.146,0.142,0.141,0.142,0.143,0.149,0.154,0.135,0.115,
     +0.148,0.144,0.144,0.143,0.142,0.149,0.144,0.148,0.138,0.132,
     +0.145,0.138,0.145,0.136,0.138,0.138,0.139,0.136,0.129,0.136,
     +0.140,0.144,0.133,0.139,0.139,0.143,0.140,0.140,0.139,0.134,
     +0.141,0.130,0.136,0.124,0.132,0.128,0.130,0.142,0.132,0.134,
     +0.139,0.133,0.144,0.133,0.135,0.136,0.130,0.134,0.134,0.111,
     +0.130,0.131,0.130,0.129,0.140,0.134,0.138,0.129,0.144,0.128,
     +0.133,0.132,0.127,0.128,0.127,0.124,0.124,0.133,0.127,0.127,
     +0.121,0.134,0.129,0.134,0.123,0.129,0.132,0.121,0.137,0.123,
     +0.130,0.135,0.127,0.129,0.128,0.126,0.123,0.122,0.120,0.119,
     +0.128,0.134,0.132,0.125,0.122,0.126,0.114,0.127,0.135,0.122,
     +0.140,0.125,0.136,0.116,0.136,0.124,0.131,0.127,0.146,0.136,
     +0.120,0.131,0.142,0.132,0.129,0.132,0.134,0.133,0.133,0.127,
     +0.128,0.116,0.125,0.127,0.124,0.122,0.134,0.122,0.128,0.122,
     +0.122,0.130,0.118,0.125,0.124,0.121,0.116,0.122,0.129,0.122,
     +0.120,0.118,0.126,0.122,0.124,0.124,0.122,0.120,0.119,0.123,
     +0.115,0.124,0.114,0.123,0.117,0.120,0.122,0.128,0.124,0.112,
     +0.128,0.121,0.122,0.115,0.115,0.118,0.116,0.111,0.115,0.118,
     + .119, .113, .113, .115, .114, .113, .115, .117, .115, .114,
     + .114, .114, .115, .112, .114, .115, .115, .115, .114, .113,
     + .114, .115, .115, .114, .114, .117, .116, .116, .116, .114,
     + .117, .116, .114, .116, .120, .116, .118, .143, .143, .143/
      data gpEn/
     +1.070,1.100,1.130,1.150,1.174,1.194,1.213,1.232,1.251,1.270,
     +1.288,1.306,1.324,1.342,1.359,1.376,1.393,1.410,1.426,1.443,
     +1.456,1.459,1.475,1.491,1.506,1.522,1.537,1.543,1.552,1.567,
     +1.582,1.597,1.612,1.617,1.626,1.640,1.655,1.669,1.683,1.697,
     +1.710,1.724,1.738,1.743,1.751,1.764,1.778,1.791,1.804,1.817,
     +1.830,1.830,1.843,1.855,1.868,1.880,1.893,1.898,1.905,1.917,
     +1.922,1.930,1.942,1.954,1.966,1.978,1.989,2.001,2.013,2.025,
     +2.036,2.041,2.048,2.059,2.070,2.082,2.093,2.104,2.115,2.117,
     +2.126,2.137,2.148,2.153,2.153,2.159,2.170,2.174,2.181,2.191,
     +2.202,2.213,2.223,2.234,2.238,2.244,2.255,2.265,2.275,2.286,
     +2.296,2.300,2.306,2.316,2.320,2.326,2.336,2.346,2.356,2.360,
     +2.366,2.376,2.386,2.396,2.400,2.406,2.415,2.419,2.425,2.435,
     +2.444,2.454,2.464,2.473,2.477,2.483,2.492,2.501,2.511,2.513,
     +2.520,2.529,2.533,2.539,2.548,2.551,2.557,2.566,2.575,2.584,
     +2.593,2.602,2.611,2.620,2.624,2.629,2.638,2.642,2.647,2.649,
     +2.656,2.665,2.674,2.682,2.691,2.695,2.700,2.708,2.717,2.726,
     +2.734,2.743,2.746,2.751,2.760,2.763,2.768,2.777,2.785,2.794,
     +2.797,2.802,2.810,2.819,2.827,2.830,2.835,2.844,2.847,2.852,
     +2.860,2.868,2.876,2.876,2.885,2.893,2.896,2.896,2.901,2.909,
     +2.917,2.919,2.925,2.933,2.941,2.944,2.949,2.957,2.957,2.965,
     +3.032,3.035,3.038,3.114,3.130,3.147,3.207,3.218,3.253,3.296,
     +3.305,3.383,3.389,3.416,3.471,3.479,3.540,3.551,3.582,3.634,
     +3.683,3.784,3.867,3.867,3.911,3.937,4.029,4.087,4.144,4.204,
     +4.257,4.282,4.327,4.334,4.379,4.396,4.458,4.514,4.580,4.580,
     +4.645,4.715,4.773,4.860,4.912,4.931,5.001,5.065,5.140,5.140,
     +5.213,5.356,5.374,5.541,5.623,5.704,5.862,5.935,6.665,7.271,
     +7.672,7.733,8.066,8.329,8.485,9.125,9.186,9.576,9.979,15.1,
     +20.,30.,40.,50.,60.,70.,80.,90.,100.,120.,
     +150.,170.,200.,240.,270.,310.,350.,400.,500.,7000./
      data gpNr/
     +0.683,0.539,0.374,0.267,0.378,0.143,0.085,0.137,0.233,0.297,
     +0.249,0.333,0.364,0.365,0.408,0.460,0.476,0.494,0.472,0.431,
     +0.303,0.374,0.376,0.384,0.394,0.412,0.444,0.357,0.458,0.486,
     +0.553,0.576,0.577,0.559,0.580,0.545,0.530,0.531,0.568,0.590,
     +0.655,0.677,0.699,0.740,0.720,0.750,0.749,0.765,0.771,0.800,
     +0.803,0.799,0.817,0.825,0.831,0.830,0.844,0.846,0.839,0.852,
     +0.853,0.860,0.863,0.859,0.856,0.876,0.874,0.877,0.876,0.873,
     +0.881,0.883,0.881,0.883,0.886,0.889,0.895,0.901,0.889,0.870,
     +0.900,0.900,0.901,0.901,0.900,0.906,0.904,0.908,0.902,0.899,
     +0.909,0.906,0.912,0.908,0.910,0.910,0.912,0.911,0.908,0.914,
     +0.917,0.920,0.914,0.919,0.919,0.922,0.921,0.922,0.923,0.920,
     +0.925,0.919,0.924,0.917,0.923,0.921,0.923,0.929,0.925,0.927,
     +0.930,0.927,0.934,0.929,0.930,0.931,0.929,0.931,0.932,0.918,
     +0.931,0.932,0.931,0.931,0.937,0.935,0.937,0.933,0.940,0.934,
     +0.937,0.937,0.935,0.936,0.936,0.935,0.935,0.939,0.937,0.937,
     +0.935,0.941,0.939,0.942,0.937,0.941,0.942,0.937,0.945,0.939,
     +0.943,0.945,0.942,0.943,0.943,0.942,0.941,0.941,0.941,0.941,
     +0.945,0.948,0.947,0.944,0.943,0.946,0.940,0.946,0.950,0.945,
     +0.952,0.947,0.951,0.943,0.952,0.947,0.950,0.948,0.955,0.952,
     +0.946,0.951,0.955,0.952,0.951,0.952,0.953,0.953,0.953,0.951,
     +0.954,0.949,0.953,0.956,0.955,0.955,0.960,0.957,0.960,0.959,
     +0.959,0.963,0.960,0.962,0.963,0.962,0.962,0.964,0.966,0.966,
     +0.966,0.967,0.970,0.969,0.970,0.971,0.971,0.971,0.972,0.973,
     +0.972,0.974,0.973,0.975,0.974,0.975,0.976,0.977,0.977,0.975,
     +0.978,0.978,0.978,0.978,0.978,0.979,0.979,0.978,0.979,0.980,
     +0.981,0.980,0.980,0.982,0.982,0.982,0.983,0.984,0.986,0.987,
     +0.988,0.988,0.989,0.989,0.990,0.991,0.991,0.991,0.992,0.992,
     +0.992,0.993,0.993,0.993,0.993,0.994,0.994,0.994,0.994,0.995,
     +0.995,0.995,0.995,0.995,0.996,0.996,0.996,1.000,1.000,1.000/
c
      real M
      real Mp,Mmin
      parameter (alpha=1./137,pi=3.1415926,Mp=0.938,Mmin=Mp+0.1396)
      parameter (rMps=Mp**2)
c
      double precision rW1z,rW2z,rW1q,rW2q,rW2l,f
c
      if(iSF.lt.1.or.iSF.gt.5) then ! Suri & Yennie
         call SuriYennieSF(rS,  rQs, rW1, rW2)
         return
      endif
c
      M=sqrt(rS)
c
c   Select bin for interpolation
      if(M.gt.gpen(Ngp)) then
         Ien=Ngp
      elseif(M.lt.gpen(1)) then
         Ien=1
      else
         Ien=Isearch(M,Ngp,gpen)
      endif
c
c   Interpolation for gp cross section
      if(Ien.eq.1) then
         I1=1
         I2=2
      elseif(Ien.eq.Ngp) then
         I1=Ngp-1
         I2=Ngp
      else
         I1=Ien
         I2=Ien+1
      endif
      tn=(gpcs(I2)-gpcs(i1))/(gpen(i2)-gpen(i1))
      Sgp=gpcs(I1)+tn*(M-gpen(i1))
c
c    Suri & Yennie SFunctions
      call SuriYennieSF(rS,  rQs, rW1q, rW2q)
c
c   q^2 dependence of Sgp
      Qs=rQs
      if(iSF.eq.3.or.(iSF.ge.4.and.M.gt.1.5).or.
     &       (iSF.eq.2.or.iSF.ge.4).and.
     &              (Ien.gt.NgpNr.or.RLU().lt.gpNr(Ien))
     &                                                  ) then ! nonresonant
         if(iSF.ge.4) then
!?         Gm=QsDepNR(rS,rQs,iSF-3)
           Gm=QsDepNR(rS,rQs,2)
         else
           call SuriYennieSF(rS,1.d-7, rW1z, rW2z)
           Gm=rW1q/rW1z
         endif
         Ires=0
      else                                                     ! resonant
         if(Qs.ge.gmQ(Ngm)) then
            Gm=Gm0/exp(GmB*Qs)
         else
            Im=Isearch(Qs,Ngm,GmQ)
            tn=(GmV(Im+1)-GmV(Im))/(GmQ(Im+1)-GmQ(Im))
            Gm=GmV(Im)+tn*(Qs-GmQ(Im))
         endif
         Gm=Gm/3.  ! due to data normalization
         Ires=1
      endif
c
      Sgp=Sgp*Gm       ! cross section with some q^2 dependence
c
c   For W -> cross section
      S1=8*pi**2*alpha*Mp/(rS-Mp**2)*0.3893 ! mb/GeV
      S2=2*pi**2*alpha*
     &  ((rS-rMpS)**2+2.*(rS+rMpS)*rQs+rQs**2)
     &               /Mp/rQs/(rS-rMpS)*0.3893 ! mb/GeV
c
c   Ratio (\sigma_T+\sigma_L)/\sigma_T according to S & Y
      Ratio=(S2*rW2q)/(S1*rW1q)
c
      if(iSF.ge.4) then         ! Sgp = \sigma_T
         rW1=Sgp/S1
         rW2=Sgp*Ratio/S2
      else                      ! Sgp = \sigma_L+\sigma_T
         rW1=Sgp/Ratio/S1
         rW2=Sgp/S2
      endif
c
      end
*=======================================================================
* Array search
*=======================================================================
	function Isearch(t,Nb,Tb)
	real tb(Nb)
	i1=1
	i2=Nb
10	Isearch=(i1+i2)/2.
	if(Isearch.eq.i1) return
	if(t.ge.tb(Isearch)) goto 100
	i2=Isearch			! t < tb(Isearch)
	goto 10
100	i1=Isearch			! t >= tb(Isearch)
	goto 10
	end
*-----------------------------------------------------------------------
*  Suri & Yennie structure functions.
*
*  This is a piece of ZLPAIR code rewritten by A.Poluektov, BINP.
*
*  Alternative set of parameters mentioned in the paper of Suri & Yennie
*  is added as a comment.
*
*  Thanks to S.P.Baranov, O.Dinger, H.Shooshtari and J.A.M.Vermaseren.
*-----------------------------------------------------------------------
	subroutine SuriYennieSF(rS, rQs, rW1, rW2) 	
	implicit double precision (r)

	parameter (rMp = 0.9383,rMps = rMp**2)
	
	common /SuriYennieSFcb/ rC1,rC2,rD1,rMrhos,rCp,rBp

	data rC1/0.86926/,rC2/2.23422/,rD1/0.12549/  ! LPAIR default
	data rMrhos/0.585/,rCp/0.96/,rBp/0.63/

!	data rC1/0.6303/, rC2/2.3049/, rD1/0.04681/  ! alternative
!	data rMrhos/1.05/,rCp/1.23/,rBp/0.61/

	save /SuriYennieSFcb/ 

	rSMs = rS - rMps
	
	rXpr = rQs/(rQs + rS)
	rEn = rSMs + rQs
	rTau = rQs / (4.*rMps)
	rMQ = (rMrhos + rQs)
	
	rC = (rC1*rSMs*(rMrhos/rMQ)**2 + 
	1	(rC2*rMps*(1.-rXpr)**4)/
	2	(1.+rXpr*(rXpr*rCp-2.*rBp)))/rQs
!	2	(1.+rXpr*(rXpr*rCp+2.*rBp)))/rQs

	rD=(rTau*rC + 
	1	rD1*rSMs*rQs*rMrhos/rMps*(rSMs/rMQ/rEn)**2)/ 
	2	( 1.+(rEn**2)/(4.*rMps*rQs) )

	rW2 = 2.*rMp*rD
	rW1 = rC*rQs/2./rMp

	end
*----------------------------------------------------------------------
* q^2 dependence of the non-resonant gamma-p cross section
* Some fit of the know e-p data. Good only for Q^2 < 6 GeV^2
*----------------------------------------------------------------------
      function QsDepNR(rS,rQs,iFIT)
      double precision rS,rQs
c
      parameter (rMp=0.9383,rMpS=rMp**2)
c
c   A few fits
      dimension rQs0(2),rPower(2)
      data rQs0/0.36,0.65/
      data rPower/0.52,0.71/
      save rQs0,rPower
c
c   Fitting function
      QsDepNR=1./(1.+(rQs/rQs0(iFIT))**2)**rPower(iFIT)
c
      end
*----------------------------------------------------------------------
* For use in PERIPP routine of ZLPAIR
*----------------------------------------------------------------------
      subroutine forPERIPP(rS,rQs,iSF,rU1,rU2)
      implicit double precision (r)
      parameter (rMp = 0.9383,rMps = rMp**2)
      call SoftSF(rS, rQs, rW1, rW2, iSF)
      rU2=rW2/(2.*rMp)
      rU1=rW1/(rQs/2./rMp)
      end
