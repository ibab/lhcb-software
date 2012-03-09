
c*****************************************************
c*** these functions are to derive the subtaction of 
c*** charm PDF.
c*****************************************************
c...quark distribution inside an on-shell gluon.
      double precision function frag(xmu,xi)
      implicit double precision (a-h, o-z)
	implicit integer(i-n)
	common/rconst/pi
	common/upalphas/alps

	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)

	xm=pmc
	
	ptogq =(1.0d0-2.0d0*xi+2.0d0*xi**2)/2.0d0

	frag=alps/(2.0d0*pi)*dlog(xmu**2/xm**2)*ptogq

	return
	end

c**************************************************
c...quark distribution inside an on-shell gluon.
      double precision function cut(xmu,xi,xksi)
      implicit double precision (a-h, o-z)
	implicit integer(i-n)

c...fixed to cteq6hq.
	if(dabs(xksi-xi).ge.1.0d-6) then
	 xpdf=ctq6pdf(0,xi/xksi,xmu)
	else
	 xpdf=ctq6pdf(0,0.99999d0,xmu)
	end if

	cut=frag(xmu,xksi)*xpdf/xksi

	return
	end

c********************************************************************
c********************************************************************

c...for transverse part of wave function
      subroutine qtrap(xmu,xi,func,a1,b1,s1)
	implicit double precision (a-h,o-z)
	implicit integer (i-n)

      external func
      parameter (eps=1.0d-6, jmax=10)
      
	olds=-1.0d30
      do j=1,jmax
	  call trapzd(xmu,xi,func,a1,b1,s1,j)
	  if (abs(s1-olds)<eps*abs(olds)) return
        if (s1==0.0d0.and.olds==0.0d0.and.j>6) return
        olds=s1
      end do

	return
      end

c********************************************************************
c********************************************************************

c...for transverse part of wave function
      subroutine trapzd(xmu,xi,func,a,b,s,n)
	implicit double precision (a-h,o-z)
	implicit integer (i-n)
      
	external func

      if (n==1) then
        s=0.5*(b-a)*(func(xmu,xi,a)+func(xmu,xi,b))
      else
        it=2**(n-2)
        tnm=it
        del=(b-a)/tnm
        x=a+0.5d0*del
        sum=0.0d0
        do j=1,it
           sum=sum+func(xmu,xi,x)
           x=x+del
        end do
        s=0.5*(s+(b-a)*sum/tnm)
      end if
      
	return
	end

