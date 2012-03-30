
c---------------------------------------------------
c*** squark of the matrix elements for gg mechanism
c---------------------------------------------------
c...note: ixiccstate=3 and 4 are for hadronic production of (xibc)

	subroutine xsection(totcross)
	implicit double precision (a-h,o-z)
	implicit integer (i-n)

	double complex fincol(6,16),colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
      common/counter/ixiccstate,nev,xmaxwgt
	common/mixevnt/imix,imixtype
	common/ggiter/iggiter
c...transfer the matrix elements' information: h1=h3*cmfactor
	common/valmatrix/cmfactor
	common/mtypeofxi/mgenxi

c----------------------
	totcross =0.0d0
	iggiter  =1

c**************************************************
c...change momenta into light-like. those light-like momenta
c...will be used in inner scalar product and spinner product.
c...the transition is based on reference momentum pmomup(8).
c**************************************************
      do i=1,2
	  do j=1,5
	     pmomzero(j,i)=pmomup(j,i)
        end do
	end do

      do 411, i=3,8
	 if (pmomup(5,i) .gt. 0.0d0) then
	   do k=1,4
	    pmomzero(k,i)=pmomup(k,i)-pmomup(k,8)*pmomup(5,i)**2/
     &	    (2.0d0*dotup(8,i))
	   end do
	   pmomzero(5,i)=0.0d0
	 else
	   do j=1,5
	     pmomzero(j,i)=pmomup(j,i)
         end do
       end if
411	continue


c...for mixing events of ixiccstate=1, 2
	if(imix .eq. 1) then
	  ixiccstate=1
	  if(mgenxi.eq.2 .and. imixtype.eq.2) ixiccstate=3
	end if

3000	continue

c**************************************************
c...get helicity matrix elements for free quark part
c**************************************************
	call freehelicity

c**************************************************
c...color factors for the hadronic production.
c**************************************************

c-----------------------
c--- color-anti-triplet.
	if(ixiccstate.eq.1 .or. ixiccstate.eq.4) then
	  c11  = 4.0d0/3.0d0
	  c12  =-1.0d0/6.0d0
	  c13  = 2.0d0/3.0d0
	  c14  =-1.0d0/12.0d0
	  c15  = 5.0d0/12.0d0
	  c16  =-1.0d0/3.0d0
	  c22  = 4.0d0/3.0d0
	  c23  =-1.0d0/12.0d0
	  c24  = 2.0d0/3.0d0
	  c25  =-1.0d0/3.0d0
	  c26  = 5.0d0/12.0d0
	  c33  = 4.0d0/3.0d0
	  c34  =-5.0d0/12.0d0
	  c35  = 1.0d0/12.0d0
	  c36  =-2.0d0/3.0d0
	  c44  = 4.0d0/3.0d0
	  c45  =-2.0d0/3.0d0
	  c46  = 1.0d0/12.0d0
	  c55  = 4.0d0/3.0d0
	  c56  =-1.0d0/6.0d0
	  c66  = 4.0d0/3.0d0
	end if

c---------------------
c--- color six state.
      if(ixiccstate.eq.2 .or. ixiccstate.eq.3) then
	  c11  = 8.0d0/3.0d0
	  c12  =-1.0d0/3.0d0
	  c13  = 2.0d0/3.0d0
	  c14  =-1.0d0/12.0d0
	  c15  = 11.0d0/12.0d0
	  c16  = 1.0d0/6.0d0
	  c22  = 8.0d0/3.0d0
	  c23  =-1.0d0/12.0d0
	  c24  = 2.0d0/3.0d0
	  c25  = 1.0d0/6.0d0
	  c26  = 11.0d0/12.0d0
	  c33  = 8.0d0/3.0d0
	  c34  = 11.0d0/12.0d0
	  c35  =-1.0d0/12.0d0
	  c36  = 2.0d0/3.0d0
	  c44  = 8.0d0/3.0d0
	  c45  = 2.0d0/3.0d0
	  c46  =-1.0d0/12.0d0
	  c55  = 8.0d0/3.0d0
	  c56  =-1.0d0/3.0d0
	  c66  = 8.0d0/3.0d0
	end if

c*****************************************************
c...get helicity matrix elements for bound state part
c*****************************************************
	call bundhelicity(ixiccstate)

c----------------------------------------------------
c...get subsets matrix elements of the whole process
	do jj=1,8
	  fincol(1,jj)=colmat(1,jj)*bundamp(4)+colmat(1,33-jj)*bundamp(1)
     &	  +colmat(1,jj+32)*bundamp(2)+colmat(1,65-jj)*bundamp(3)
	  fincol(2,jj)=colmat(2,jj)*bundamp(4)+colmat(2,33-jj)*bundamp(1)
     &	  +colmat(2,jj+32)*bundamp(2)+colmat(2,65-jj)*bundamp(3)
	  fincol(3,jj)=colmat(3,jj)*bundamp(4)+colmat(3,33-jj)*bundamp(1)
     &	  +colmat(3,jj+32)*bundamp(2)+colmat(3,65-jj)*bundamp(3)
	  fincol(4,jj)=colmat(4,jj)*bundamp(4)+colmat(4,33-jj)*bundamp(1)
     &	  +colmat(4,jj+32)*bundamp(2)+colmat(4,65-jj)*bundamp(3)
	  fincol(5,jj)=colmat(5,jj)*bundamp(4)+colmat(5,33-jj)*bundamp(1)
     &	  +colmat(5,jj+32)*bundamp(2)+colmat(5,65-jj)*bundamp(3)
	  fincol(6,jj)=colmat(6,jj)*bundamp(4)+colmat(6,33-jj)*bundamp(1)
     &	  +colmat(6,jj+32)*bundamp(2)+colmat(6,65-jj)*bundamp(3)
      end do
c-----------------
	do jj=17,24
	 fincol(1,jj-8)=colmat(1,jj)*bundamp(3)+colmat(1,33-jj)*bundamp(2)
     &	  +colmat(1,jj+32)*bundamp(1)+colmat(1,65-jj)*bundamp(4)
	 fincol(2,jj-8)=colmat(2,jj)*bundamp(3)+colmat(2,33-jj)*bundamp(2)
     &	  +colmat(2,jj+32)*bundamp(1)+colmat(2,65-jj)*bundamp(4)
	 fincol(3,jj-8)=colmat(3,jj)*bundamp(3)+colmat(3,33-jj)*bundamp(2)
     &	  +colmat(3,jj+32)*bundamp(1)+colmat(3,65-jj)*bundamp(4)
	 fincol(4,jj-8)=colmat(4,jj)*bundamp(3)+colmat(4,33-jj)*bundamp(2)
     &	  +colmat(4,jj+32)*bundamp(1)+colmat(4,65-jj)*bundamp(4)
	 fincol(5,jj-8)=colmat(5,jj)*bundamp(3)+colmat(5,33-jj)*bundamp(2)
     &	  +colmat(5,jj+32)*bundamp(1)+colmat(5,65-jj)*bundamp(4)
	 fincol(6,jj-8)=colmat(6,jj)*bundamp(3)+colmat(6,33-jj)*bundamp(2)
     &	  +colmat(6,jj+32)*bundamp(1)+colmat(6,65-jj)*bundamp(4)
      end do

c**************************************************
c... square of the amplitude.
c**************************************************

	do jj=1,16
	  totcross=totcross+c11*dble(abs(fincol(1,jj))**2)+2.0d0*c12*
     &	dreal(fincol(1,jj)*dconjg(fincol(2,jj)))+2.0d0*c13*
     &	dreal(fincol(1,jj)*dconjg(fincol(3,jj)))+2.0d0*c14*
     &	dreal(fincol(1,jj)*dconjg(fincol(4,jj)))+2.0d0*c15*
     &	dreal(fincol(1,jj)*dconjg(fincol(5,jj)))+2.0d0*c16*
     &	dreal(fincol(1,jj)*dconjg(fincol(6,jj)))+c22*dble(
     &    abs(fincol(2,jj))**2)+2.0d0*c23*dreal(fincol(2,jj)*
     &	dconjg(fincol(3,jj)))+2.0d0*c24*dreal(fincol(2,jj)*
     &	dconjg(fincol(4,jj)))+2.0d0*c25*dreal(fincol(2,jj)*
     &	dconjg(fincol(5,jj)))+2.0d0*c26*dreal(fincol(2,jj)*
     &	dconjg(fincol(6,jj)))+c33*dble(abs(fincol(3,jj))**2)+
     &    2.0d0*c34*dreal(fincol(3,jj)*dconjg(fincol(4,jj)))+
     &    2.0d0*c35*dreal(fincol(3,jj)*dconjg(fincol(5,jj)))+
     &    2.0d0*c36*dreal(fincol(3,jj)*dconjg(fincol(6,jj)))+
     &    c44*dble(abs(fincol(4,jj))**2)+2.0d0*c45*dreal(fincol(4,jj)
     &	*dconjg(fincol(5,jj)))+2.0d0*c46*dreal(fincol(4,jj)*
     &	dconjg(fincol(6,jj)))+c55*dble(abs(fincol(5,jj))**2)+
     &    2.0d0*c56*dreal(fincol(5,jj)*dconjg(fincol(6,jj)))+
     &    c66*dble(abs(fincol(6,jj))**2)
	end do

	if(ixiccstate.eq.2 .or. ixiccstate.eq.3 .or. ixiccstate.eq.4)then
	  totcross=totcross*cmfactor
	end if

c... mixing events
	if(imix.eq.1) then
	  ixiccstate=ixiccstate+1
	  iggiter=2
	  if(mgenxi.eq.2 .and. imixtype.eq.2) then
	   if(ixiccstate.lt.5) goto 3000
	  else
	   if(ixiccstate.lt.3) goto 3000
	  end if
	end if

C**************************************************
C...FOR (CC) OR (BB) PRODUCTION.
C**************************************************
c...this factor comes from the identity of the final
c...particles \bar{c} and \bar{c}. However it can be 
c...compensated by the bound state: 2^2/(2!*2!)=1, 
c...where 2^2 is from double of 36 Feynmans; one 1/2! 
c...from bound state and one 1/2! for identity of 
c...\bar{c} and \bar{c} or \bar{b} and \bar{b}. So
c...the following should be commented out.
c**************************************************

c      totcross=totcross/2.0D0

	return
	end

