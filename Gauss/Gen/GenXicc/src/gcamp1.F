
c---------------------------------
c---- amplitude square of g+c mechanism
c---------------------------------

      double precision function gcxicc3()
      implicit double precision (a-h,o-z) 
      implicit  integer (i-n)

c...user transformation.
	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/rconst/pi

      x1=-2*dotup(1,4)-pmxicc**2*0.5
      t1=-2*dotup(1,3)+pmxicc**2
      s1= 2*dotup(1,2)

	gcxicc3=(2048.0/81.0*pmxicc**(-1)*s1**(-4)*t1**(-4)
     . *pi**3*x1**(-2))*(4.0*pmxicc**2*(10.0*s1**4*t1**4-4.0*s1**3*t1
     . **3*x1**2+11.0*s1**2*t1**2*x1**4+4.0*s1*t1*x1**6)+4.0*pmxicc**
     . 4*(-17.0*s1**3*t1**3*x1+28.0*s1**2*t1**2*x1**3-20.0*s1*t1*x1**
     . 5)+3.0*pmxicc**6*(-12.0*s1**3*t1**3-5.0*s1**2*t1**2*x1**2-14.0
     . *s1*t1*x1**4+4.0*x1**6)+8.0*(-2.0*s1**4*t1**4*x1+11.0*s1**3*t1
     . **3*x1**3-9.0*s1**2*t1**2*x1**5))*(fxicc**2)
      return
      end

