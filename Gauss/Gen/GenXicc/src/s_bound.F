
c********************************************************************

c...helicity amplitude of the bound state part: c+c->\xicc.
c...for helicity: (++), (--), (+-), (-+)
      subroutine bundhelicity(ixicc)
      implicit double precision (a-h,o-z)
	implicit integer (i-n)

	double complex colmat,bundamp,polsppup,inpup
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
      common/pol/polar(4,3)
	common/rconst/pi

c...wavezero is the value of the wave function at origin.
      wavezero=fxicc/dsqrt(4.0d0*pi)

c...bundamp(1)->++, bundamp(2)->--, bundamp(3)->+-, bundamp(4)->-+.
c...this is for (1s0)-state.
      if(ixicc.eq.2. or. ixicc.eq.4) then
	 bundamp(1)=dcmplx(-wavezero*dsqrt(pmxicc)/(2.0d0*dsqrt(pmb*pmc)))
	 bundamp(2)=dcmplx(+wavezero*dsqrt(pmxicc)/(2.0d0*dsqrt(pmb*pmc)))
	 bundamp(3)=dcmplx(0.0d0)
	 bundamp(4)=dcmplx(0.0d0)
	 return
	end if

c-------------------------------------

c...the following is for (3s1)-state.
c...the expression of polarization vectors depend on the gauge choice.
c...one way of constructing polarization vector of 3s1 state: all
c...satisfies: polar(i)**2=-1, polar(i).polar(j)=0 (i.ne.j),
c...polar(i).pmomup(3)=0; in addition, we choose: polar(1,1)=0.0d0
c...polar(2,1)=0.0d0, and polar(1,3)=0.0d0.
      if(ixicc.eq.1 .or. ixicc.eq.3) then
	 polar(1,1)=0.0d0
	 polar(2,1)=0.0d0
	 polar(3,1)=pmomup(4,3)/dsqrt(pmomup(4,3)**2-pmomup(3,3)**2)
	 polar(4,1)=pmomup(3,3)/dsqrt(pmomup(4,3)**2-pmomup(3,3)**2)

       if(pmomup(3,3).lt.0.0d0) then
	  polar(1,2)=-dsqrt(pmomup(4,3)**2-pmomup(2,3)**2-pmomup(3,3)**2)
     &	   /pmomup(5,3)
	  polar(2,2)=-pmomup(1,3)*pmomup(2,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	  polar(3,2)=-pmomup(1,3)*pmomup(3,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	  polar(4,2)=-pmomup(1,3)*pmomup(4,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	 else
	  polar(1,2)=dsqrt(pmomup(4,3)**2-pmomup(2,3)**2-pmomup(3,3)**2)
     &	   /pmomup(5,3)
	  polar(2,2)=pmomup(1,3)*pmomup(2,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	  polar(3,2)=pmomup(1,3)*pmomup(3,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	  polar(4,2)=pmomup(1,3)*pmomup(4,3)/dsqrt(pmomup(4,3)**2
     &	  -pmomup(2,3)**2-pmomup(3,3)**2)/pmomup(5,3)
	 end if

	 polar(1,3)=0.0d0
	 polar(2,3)=dsqrt(pmomup(4,3)**2-pmomup(3,3)**2)
     &   /dsqrt(pmomup(4,3)**2-pmomup(2,3)**2-pmomup(3,3)**2)
	 polar(3,3)=pmomup(2,3)*pmomup(3,3)/dsqrt(pmomup(4,3)**2-
     &	  pmomup(3,3)**2)/dsqrt(pmomup(4,3)**2-pmomup(2,3)**2
     &	  -pmomup(3,3)**2)
	 polar(4,3)=pmomup(2,3)*pmomup(4,3)/dsqrt(pmomup(4,3)**2-
     &	  pmomup(3,3)**2)/dsqrt(pmomup(4,3)**2-pmomup(2,3)**2
     &	  -pmomup(3,3)**2)

c...four bound state matrix elements of 3s1.
	 bundamp(1)=dcmplx(0.0d0)
	 do i=1,3
	  bundamp(1)=bundamp(1)+wavezero*dsqrt(pmxicc)/(2.0d0*dsqrt(pmb*
     &	pmc))*pmxicc*(polar(4,i)*pmomup(4,8)-polar(1,i)*pmomup(1,8)-
     &    polar(2,i)*pmomup(2,8)-polar(3,i)*pmomup(3,8))/dotup(3,8)
	 end do
	 bundamp(2)=dconjg(bundamp(1))

       bundamp(3)=dcmplx(0.0d0)
	 do i=1,3
        bundamp(3)=bundamp(3)+wavezero*dsqrt(pmxicc)/(2.0d0*dsqrt(pmb*
     &	pmc))*polsppup(i)/(2.0d0*dotup(3,8))
       end do
   	 bundamp(4)=-dconjg(bundamp(3))
	end if
      
	return
	end

