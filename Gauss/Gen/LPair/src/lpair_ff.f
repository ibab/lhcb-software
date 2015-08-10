* lpair_ff.f ==========================================================
* By A. Shamov
* Ge,Gm control in LPAIR
*    call lpair_ff(ff)
*       integer ff - form-factor option:
*                  ff = 0 - standard  Ge=Gm/mu=1/(1+q^2/0.71)^2
*                       DO NOT AGREE WELL WITH EXPERIMENTAL DATA
*                     > 0 - parameterizations:
*                       1 - Phys.Rev.C76(2007)035205
*                           SEEMS TO BE THE BEST
*                      41 ! the spread of results gives an
*                      51 ! estimate of uncertainties due to
*                      61 ! statistic and systematic errors
*                      71 ! (experimental data and corrections
*                         ! applied by Arrington et al.)
*                       2 - Phys.Rev.C65(2002)051001
*                           UNREASONABLE PARAMETERIZATION
*                              (1-G_E(Q2^2) \propto \sqrt(Q^2)
*                           AT LOW Q^2 !!!
*                       3 - Nucl.Phys.A596(1996)367
*                       4 - Phys.Rev.C76(2007)035201
*                           SEEMS TO BE BEST FOR LOW Q2^2
*                      14 ! use spread of results to estimate
*                      24 ! uncertainties due to corrections
*                      34 ! applied by Arrington et al.
*                       60
*                       ... - dipole Ge=Gm/mu=1/(1+q^2/(ff/100.))^2
*                       80
*                     < 0 - special:
*                      -99 - dipole Ge=Gm/mu=1/(1+q^2/0.71)^2
*                      -98 - Ge=1, Gm=0
*                      -97 - Ge=0, Gm=1
*                      -96 - Ge=1, Gm=1
*=====================================================================
      subroutine lpair_ff(ff)
      implicit none
*--------------------------------------------------------------------      
      integer ff
c
      integer mFF
      common /mGmGe/ mFF
*--------------------------------------------------------------------
c
      mFF=ff
c
      print *
      print *,'LPAIR_FF: form-factor option',mFF
      print *
c
      end
