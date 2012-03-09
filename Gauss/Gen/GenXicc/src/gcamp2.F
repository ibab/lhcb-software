
c-------------------------------------------
c---- amplitude for g+c mechanism
c-------------------------------------------

      double precision function gcxicc6() 
      implicit double precision (a-h,o-z) 
      implicit  integer (i-n) 
c...user transformation.
	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/rconst/pi

      x1=-2*dotup(1,4)-0.5*pmxicc**2
      t1=-2*dotup(1,3)+pmxicc**2
      s1= 2*dotup(1,2)

      gcxicc6=(2048.0/81.0*pmxicc**(-1)*s1**(-4)*t1**(-4)*pi**3
     . *x1**(-2))*(pmxicc**2*(-20.0*s1**4*t1**4-s1**3*t1**3*x1**2
     . -12.0*s1**2*t1**2*x1**4)+4.0*pmxicc**4*(-12.0*s1**3*t1**3*x1
     . -s1**2*t1**2*x1**3-2.0*s1*t1*x1**5)+pmxicc**6*(-48.0*s1**3*
     . t1**3+8.0*s1**2*t1**2*x1**2-7.0*s1*t1*x1**4+2.0*x1**6)+
     . 2.0*(-10.0*s1**4*t1**4*x1+9.0*s1**3*t1**3*x1**3-
     . 2.0*s1**2*t1**2*x1**5))*fxicc**2

      return
      end

