C $Id: xsectionPsiStar.F,v 1.2 2010-03-09 11:11:30 robbep Exp $
C Function to compute the cross-section for psi(3770) by N. Brook
C
C
      Double Precision Function xsectionPsiStar(as,sqm3,sh,th)
      Implicit Double Precision (A-H,O-Z)
*     as = 0.2
*     sqm3 = 3.770**2
*     sh = rand()*14000
*     th = rand()*14000
      pi = dacos(-1.D0)
      am3 = dsqrt(sqm3)
      nc = 3
      aj = 1.d0
      rppd2 = .637D0
      csme = 15.D0*(2.D0*aj+1.D0)*nc/4.D0/pi*rppd2
      cf= 16.D0*as**3*pi/81.D0/am3**3/((sqm3-sh)*(sqm3-th)*(sh+th))**5
      xtra = pi/sh**2
      c20 = (102.D0*sh**3+302.D0*sh**2*th+
     >       302.D0*sh*th**2+102.D0*th**3)*sqm3**10
      c18 = (286.D0*sh**4+1732.D0*sh**3*th+2844.D0*sh**2*th**2
     1       +1732.D0*sh*th**3+286.D0*th**4)*sqm3**9
      c16 = (275.D0*sh**5+3840.D0*sh**4*th+10289.D0*sh**3*th**2
     1       +10289.D0*sh**2*th**3+3840.D0*sh*th**4+
     2        275.D0*th**5)*sqm3**8
      c14 = (227.D0*sh**6+5004.D0*sh**5*th+19569.D0*sh**4*th**2+
     >       29536.D0*sh**3*th**3
     1       +19569.D0*sh**2*th**4+5004.D0*sh*th**5+
     >       227.D0*th**6)*sqm3**7
      c12 = (410.D0*sh**7+5137.D0*sh**6*th+23585.D0*sh**5*th**2+
     >       47908.D0*sh**4*th**3
     1       +47908.D0*sh**3*th**4+23585.D0*sh**2*th**5+
     >       5137.D0*sh*th**6
     2       +410.D0*th**7)*sqm3**6
      c10 = (470.D0*sh**8+4220.D0*sh**7*th+19534.D0*sh**6*th**2+
     >       47528.D0*sh**5*th**3
     1       +63536.D0*sh**4*th**4+47528.D0*sh**3*th**5+
     >        19534.D0*sh**2*th**6+
     2       +4220.D0*sh*th**7+470.D0*th**8)*sqm3**5
      c08 = (245.D0*sh**9+2190.D0*sh**8*th+10358.D0*sh**7*th**2+
     >       28602.D0*sh**6*th**3
     1       +47093.D0*sh**5*th**4+47093.D0*sh**4*th**5+
     >       28602.D0*sh**3*th**6
     2       +10358.D0*sh**2*th**7+2190.D0*sh*th**8+
     >       245.D0*th**9)*sqm3**4
      c06 = (49.D0*sh**10+580.D0*sh**9*th+2822.D0*sh**8*th**2+
     >       8984.D0*sh**7*th**3
     1       +17653.D0*sh**6*th**4+21968.D0*sh**5*th**5+
     >       17653.D0*sh**4*th**6
     2       +8984.D0*sh**3*th**7+2822.D0*sh**2*th**8+
     >       580.D0*sh*th**9
     3       +49.D0*th**10)*sqm3**3
      c04 = (67.D0*sh**10*th+210.D0*sh**9*th**2+774.D0*sh**8*th**3
     1       +2006.D0*sh**7*th**4+3147.D0*sh**6*th**5+
     >       3147.D0*sh**5*th**6
     2       +2006.D0*sh**4*th**7+774.D0*sh**3*th**8+210.D0*sh**2*th**9
     3       +67.D0*sh*th**10)*sqm3**2
      c02 = (25.D0*sh**10*th**2+100.D0*sh**9*th**3+220.D0*sh**8*th**4
     1       +340.D0*sh**7*th**5+390.D0*sh**6*th**6+340.D0*sh**5*th**7
     2       +220.D0*sh**4*th**8+100.D0*sh**3*th**9+
     >        25.D0*sh**2*th**10)*sqm3
      c00 = 5.D0*sh**10*th**3+25.D0*sh**9*th**4+60.D0*sh**8*th**5+
     >      90.D0*sh**7*th**6
     1      +90.D0*sh**6*th**7+60.D0*sh**5*th**8+25.D0*sh**4*th**9+
     >      5.D0*sh**3*th**10
      coeff = cf*(c20-c18+c16-c14+c12-c10+c08-c06+c04-c02+c00)
*     Write(6,*)'c20-c00:',c20,c18,c16,c14,c12,c10,c08,c06,c04,c02,c00
*     Write(6,*) 'Coeff: ',xtra*csme*coeff,cf
      xsectionPsiStar = csme*coeff
*     Write(6,*) 'xsection: ',xsection
      End
