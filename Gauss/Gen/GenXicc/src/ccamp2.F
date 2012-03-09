
c---------------------------------
c---- amplitude square of c+c mechanism
c---------------------------------

      double precision function ccxicc6()
      implicit double precision (a-h,o-z) 
      implicit  integer (i-n)

c...user transformation.
	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/rconst/pi

      x1=-2*dotup(1,4)
      t1=-2*dotup(1,3)+pmxicc**2
      s1= 2*dotup(1,2)-0.5*pmxicc**2

      ccxicc6=(2048.0/81.0*pmxicc**(-1)*s1**(-2)*t1**
     . (-4)*pi**3*x1**(-4))*(pmxicc**2*(12.0*s1**4*t1**2*x1**2-s1**3*
     . t1**3*x1**2-s1**2*t1**4*x1**2+20.0*t1**4*x1**4)+4.0*pmxicc**4*
     . (2.0*s1**5*t1*x1-s1**4*t1**2*x1-s1**3*t1**3*x1+12.0*s1*t1**3*
     . x1**3)+pmxicc**6*(-2.0*s1**6-7.0*s1**5*t1-15.0*s1**4*t1**2-
     . 64.0*s1**3*t1**3-152.0*s1**2*t1**4-144.0*s1*t1**5-48.0*t1**6)+
     . 2.0*(2.0*s1**5*t1**2*x1**2+9.0*s1**4*t1**3*x1**2+9.0*s1**3*
     . t1**4*x1**2+10.0*s1*t1**4*x1**4))*fxicc**2

      return
      end

