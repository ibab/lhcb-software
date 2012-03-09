
c****************************************************************
c...taken from BCVEGPY with changes to make color-matrix elements.
c...more explain can be found in [Comput.Phys.Commun.159,192(2004)].
c...helicity amplitude of free quark part: g+g->\bar{c}+c+c+\bar{c}.

	subroutine freehelicity
	implicit double precision (a-h,o-z)
	implicit integer (i-n)

	double complex bfun,colmat,inpup,sppup,yup,xup,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
      common/tryup/bfun(9,4,64),yup(42),xup(2),idp,idq0,idk1,idk2,
     &	idb1,idb2,idc1,idc2
	common/counter/ixiccstate,nev
	common/ggiter/iggiter

      dimension propup(14,4)
    
      idq0=8
	idp=3

      do 700, ii=1,4
	 if (ii .eq. 1) then
	  idk1=1
	  idk2=2
	  idb1=4
	  idb2=6
	  idc1=7
	  idc2=5
	 end if

	 if (ii .eq. 2) then
	  idk1=2
	  idk2=1
	  idb1=4
	  idb2=6
	  idc1=7
	  idc2=5
       end if

       if (ii .eq. 3) then
	  idk1=1
	  idk2=2
	  idb1=7
	  idb2=5
	  idc1=4
	  idc2=6
       end if

       if (ii .eq. 4) then
	  idk1=2
	  idk2=1
	  idb1=7
	  idb2=5
	  idc1=4
	  idc2=6
       end if

c...define some basic spinnor products that are needed in helicity amplitude.
	 yup(1)=sppup(idk1,idc2,idq0)
	 yup(2)=sppup(idk1,idb2,idq0)
	 yup(3)=sppup(idk1,idq0,idb2)
	 yup(4)=sppup(idk1,idq0,idc2)
	 yup(5)=sppup(idk1,idq0,idc1)
	 yup(6)=sppup(idk1,idq0,idb1)
	 yup(7)=sppup(idk1,idk2,idq0)
	 yup(8)=sppup(idk1,idq0,idk2)
	 yup(9)=sppup(idk1,idc1,idq0)

	 yup(10)=sppup(idk2,idb1,idq0)
	 yup(11)=sppup(idk2,idc2,idq0)
	 yup(12)=sppup(idk2,idk1,idq0)
	 yup(13)=sppup(idk2,idq0,idb2)
	 yup(14)=sppup(idk2,idq0,idc2)
	 yup(15)=sppup(idk2,idq0,idc1)
	 yup(16)=sppup(idk2,idb2,idq0)
	 yup(17)=sppup(idk2,idq0,idb1)
	 yup(18)=sppup(idk2,idc1,idq0)

	 yup(19)=sppup(idq0,idk2,idq0)
	 yup(20)=sppup(idq0,idb1,idc1)
	 yup(21)=sppup(idq0,idk1,idb1)
	 yup(22)=sppup(idq0,idc2,idb1)
	 yup(23)=sppup(idq0,idb1,idc2)
	 yup(24)=sppup(idq0,idb2,idc2)
	 yup(25)=sppup(idq0,idb1,idk1)
	 yup(26)=sppup(idq0,idk2,idb1)
	 yup(27)=sppup(idq0,idc1,idq0)
	 yup(28)=sppup(idq0,idc2,idq0)
	 yup(29)=sppup(idq0,idb1,idq0)
	 yup(30)=sppup(idq0,idb2,idq0)
	 yup(31)=sppup(idq0,idk1,idq0)
	 yup(32)=sppup(idq0,idc1,idb1)
	 yup(33)=sppup(idq0,idk2,idc2)

	 yup(34)=sppup(idc2,idq0,idc1)
	 yup(35)=sppup(idb2,idq0,idb1)
	 yup(36)=sppup(idb2,idc2,idq0)
	 yup(37)=sppup(idb1,idq0,idc2)
	 yup(38)=sppup(idq0,idk1,idb2)
	 yup(39)=sppup(idq0,idb2,idc1)
	 yup(40)=sppup(idq0,idk2,idc1)
	 yup(41)=sppup(idq0,idk2,idb2)
	 yup(42)=sppup(idq0,idc1,idb2)
	 xup(1)=inpup(idq0,idk1)*inpup(idq0,idk2)
	 xup(2)=dconjg(xup(1))
       
	 if (iggiter.eq.1) then
	   if (ii.eq.1) call bfirst
	   if (ii.eq.2) call bsecond
	   if (ii.eq.3) call bthird
	   if (ii.eq.4) call bfourth
	 end if

c--------------------------------------------------------------------
c...needed basic propagators. propup(n,1) from cb and cc sets. other
c...propagators can be obtained by interchange gluons and quarks,
c...which can be achieved by interchanging the identity number of the 
c...particles (idk1,idk2,...). here correspondence of diagram labels:
c...mcb1,mcb2,..., to original feynman diagrams are shown in paper.
c...propup(1,1)<-mcb1,propup(2,1)<-mcb2,
c...propup(3,1)<-mcb3,propup(4,1)<-mcb4
       if (ii.eq.1 .or. ii.eq.2) then
        propup(1,ii)=1/(-2.0d0*dotup(idb1,idk2))/(2.0d0*dotup(idc1,idc1)
     &	 +2.0d0*dotup(idc2,idc1)-2.0d0*dotup(idk1,idc2)-2.0d0*
     &	 dotup(idk1,idc1))/(-2.0d0*dotup(idk1,idc2))
	  propup(2,ii)=1/(-2.0d0*dotup(idb1,idk2))/(2.0d0*dotup(idc1,idc1)
     &	 +2.0d0*dotup(idc2,idc1)-2.0d0*dotup(idk1,idc2)-2.0d0*
     &	 dotup(idk1,idc1))/(-2.0d0*dotup(idc1,idk1))
	  propup(3,ii)=1/(-2.0d0*dotup(idb2,idk2))/(2.0d0*dotup(idc1,idc1)
     &	 +2.0d0*dotup(idc2,idc1)-2.0d0*dotup(idk1,idc2)-
     &     2.0d0*dotup(idk1,idc1))/(-2.0d0*dotup(idc2,idk1))
	  propup(4,ii)=1/(-2.0d0*dotup(idb2,idk2))/(2.0d0*dotup(idc1,idc1)
     &	 +2.0d0*dotup(idc2,idc1)-2.0d0*dotup(idk1,idc2)-
     &	 2.0d0*dotup(idk1,idc1))/(-2.0d0*dotup(idc1,idk1))
	 end if

c...propup(5,1)->mcc1,propup(6,1)->mcc3,propup(7,1)->mcc5
	 propup(5,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(-2.0d0*dotup(idc2,idk2))/(2.0d0*dotup(idk1,idk2)-
     &	 2.0d0*dotup(idk1,idc2)-2.0d0*dotup(idc2,idk2))
	 propup(6,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(-2.0d0*dotup(idc2,idk2))/(-2.0d0*dotup(idc1,idk1))
	 propup(7,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(-2.0d0*dotup(idc1,idk1))/(2.0d0*dotup(idk1,idk2)
     &     -2.0d0*dotup(idc1,idk1)-2.0d0*dotup(idc1,idk2))

c...propup(8,1)->mcc7, propup(9,1)->mcc8
	 if (ii.eq.1 .or. ii.eq.3) then
	   propup(8,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(2.0d0*dotup(idk1,idk2))/(2.0d0*dotup(idk1,idk2)
     &     -2.0d0*dotup(idk1,idc2)-2.0d0*dotup(idc2,idk2))
	   propup(9,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(2.0d0*dotup(idk1,idk2))/(2.0d0*dotup(idk1,idk2)
     &     -2.0d0*dotup(idc1,idk1)-2.0d0*dotup(idc1,idk2))
	 end if

c...propup(10,1)->mco1, propup(11,1)->mco2
	 propup(10,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(-2.0d0*dotup(idc2,idk1))/(2.0d0*dotup(idb1,idb1)+2.0d0*
     &   dotup(idb1,idb2)-2.0d0*dotup(idb1,idk2)-2.0d0*dotup(idb2,idk2))
	 propup(11,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(-2.0d0*dotup(idc1,idk1))/(2.0d0*dotup(idb1,idb1)+2.0d0*
     &   dotup(idb1,idb2)-2.0d0*dotup(idb1,idk2)-2.0d0*dotup(idb2,idk2))

c...propup(12,1)->moo1
	 if (ii.eq.1 .or. ii.eq.2) then
	  propup(12,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(2.0d0*dotup(idc1,idc1)+2.0d0*dotup(idc2,idc1))
     &     /(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2)-
     &     2.0d0*dotup(idb1,idk1)-2.0d0*dotup(idb2,idk1))
	 end if

c...propup(13,1)->moo3, propup(14,1)->moo4
	 if (ii.eq.1) then
	  propup(13,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(2.0d0*dotup(idk1,idk2))/(2.0d0*dotup(idc1,idc1)+
     &     2.0d0*dotup(idc2,idc1))
	  propup(14,ii)=1/(2.0d0*dotup(idb1,idb1)+2.0d0*dotup(idb1,idb2))
     &	 /(2.0d0*dotup(idc1,idc1)+2.0d0*dotup(idc2,idc1))
	 end if

700   continue
      	
c...kk=1-64 stands for the helicity of the amplitude.

c*****************************************************
c--- matrix elemets for different color structure.
c--- six independent color basis, c.f. PRD73,094022(2006).
c*****************************************************

c...cbar3
	if(ixiccstate.eq.1 .or. ixiccstate.eq.4) then
	  do kk=1,64
	   colmat(1,kk) =((-2*bfun(7,1,kk)*propup(6,1) - 
     -    6*bfun(8,3,kk)*propup(7,3) +8*(bfun(6,2,kk) - 
     -      (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(5,1) - dotup(5,2)))*propup(8,1) - 
     -    8*bfun(6,1,kk)*(propup(5,1) + propup(8,1)) + 
     -    2*(bfun(8,2,kk) -(4*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -          bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2)))*
     -     propup(9,1) - 2*bfun(8,1,kk)*(propup(7,1) + propup(9,1)) - 
     -    6*(bfun(8,3,kk)-bfun(8,4,kk)+(4*bfun(5,3,kk)-4*bfun(5,4,kk)+
     -     bfun(9,3,kk) - bfun(9,4,kk))*
     -        (dotup(1,2) - dotup(4,1) - dotup(4,2)))*propup(9,3) + 
     -    6*(bfun(3,2,kk)-2*bfun(5,2,kk)*dotup(5,2))* propup(10,2) + 
     -    6*(bfun(2,3,kk) - bfun(4,3,kk) - 
     -       2*bfun(5,3,kk)*dotup(4,1))*propup(11,3) + 
     -    6*(bfun(2,2,kk) + bfun(3,2,kk) - bfun(4,2,kk) - 
     -       2*bfun(5,2,kk)* (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2)))*propup(12,1) - 6*bfun(1,2,kk)*
     -     (propup(1,2) + propup(10,2) + propup(12,1)) - 
     -    6*(bfun(6,1,kk) - bfun(6,2,kk) - bfun(8,1,kk) + 
     -       bfun(8,2,kk) +  4*bfun(5,2,kk)*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2)) - 
     -       (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(5,1) + dotup(5,2) - dotup(7,1) - dotup(7,2)) + 
     -       4*bfun(5,1,kk)*(-dotup(1,2)+ dotup(7,1) + dotup(7,2)))*
     -     propup(13,1) +  3*(-4*bfun(5,1,kk) + 2*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(2,kk) =((-2*(bfun(7,2,kk)*propup(6,2) + 
     -       bfun(8,2,kk)*propup(7,2) +3*bfun(8,4,kk)*propup(7,4)) + 
     -    8*(bfun(6,1,kk) + (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(5,1) - dotup(5,2)))*
     -     propup(8,1) -  8*bfun(6,2,kk)*(propup(5,2) + propup(8,1)) + 
     -    2*(bfun(8,1,kk) - bfun(8,2,kk) + (4*bfun(5,1,kk) -
     -        4*bfun(5,2,kk) +    bfun(9,1,kk) - bfun(9,2,kk))*
     -     (dotup(1,2) - dotup(7,1) - dotup(7,2)))*propup(9,1) + 
     -    6*(bfun(8,3,kk) - bfun(8,4,kk) + (4*bfun(5,3,kk) -
     -        4*bfun(5,4,kk) +  bfun(9,3,kk) - bfun(9,4,kk))*
     -    (dotup(1,2) - dotup(4,1) - dotup(4,2)))*propup(9,3) + 
     -    6*(bfun(3,1,kk) - 2*bfun(5,1,kk)*dotup(5,1))* propup(10,1)+ 
     -    6*(bfun(2,4,kk) - bfun(4,4,kk) - 
     -       2*bfun(5,4,kk)*dotup(4,2))*propup(11,4) + 
     -    6*(bfun(2,1,kk)+bfun(3,1,kk)-bfun(4,1,kk) -2*bfun(5,1,kk)*
     -     (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -          dotup(7,1)))*propup(12,2) - 6*bfun(1,1,kk)*
     -     (propup(1,1) + propup(10,1) + propup(12,2)) + 
     -    6*(bfun(6,1,kk) - bfun(6,2,kk) - bfun(8,1,kk) + 
     -       bfun(8,2,kk) +  4*bfun(5,2,kk)*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2)) - 
     -       (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -          dotup(7,2)) + 4*bfun(5,1,kk)*
     -        (-dotup(1,2) + dotup(7,1) + dotup(7,2)))*
     -     propup(13,1) +  3*(2*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(3,kk) =((bfun(2,2,kk)*propup(2,2) + 
     -    bfun(4,2,kk)*propup(4,2) + 
     -    bfun(3,2,kk)*(4*propup(3,2) + 3*propup(10,2)) + 
     -    3*(bfun(2,2,kk) + bfun(3,2,kk) - bfun(4,2,kk) - 
     -       2*bfun(5,2,kk)*(dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2)))*propup(12,1) + 
     -    bfun(1,2,kk)*(propup(1,2) -3*(propup(10,2)+propup(12,1)))- 
     -    3*(-(bfun(7,1,kk)*propup(6,1)) -bfun(7,4,kk)*propup(6,4) + 
     -       2*bfun(5,2,kk)*dotup(5,2)*propup(10,2) + 
     -       (bfun(1,3,kk) - bfun(3,3,kk) + 
     -          2*bfun(5,3,kk)*dotup(6,1))*propup(10,3) + 
     -       (-bfun(2,4,kk) + bfun(4,4,kk) + 
     -          2*bfun(5,4,kk)*dotup(4,2))*propup(11,4) + 
     -       bfun(1,1,kk)*propup(12,2)) + 
     -    3*(bfun(3,1,kk)*propup(12,2) - 
     -     bfun(4,1,kk)*(propup(11,1) + propup(12,2)) +  bfun(2,1,kk)*
     -     (propup(2,1) + propup(11,1)+propup(12,2))+2*bfun(5,1,kk)*
     -        (-(dotup(7,1)*propup(11,1)) - 
     -          (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -             dotup(7,1))*propup(12,2)) - 
     -       (bfun(5,1,kk) + bfun(5,2,kk) - bfun(9,1,kk) - 
     -          bfun(9,2,kk))*propup(14,1)))/3.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(4,kk) =((bfun(2,1,kk)*propup(2,1) + 
     -    4*bfun(3,1,kk)*propup(3,1) +bfun(4,1,kk)*propup(4,1) + 
     -    3*bfun(2,2,kk)* (propup(2,2) + propup(11,2) + propup(12,1)) + 
     -    3*(bfun(7,2,kk)*propup(6,2) +bfun(7,3,kk)*propup(6,3) + 
     -       (-bfun(1,4,kk) + bfun(3,4,kk) - 
     -          2*bfun(5,4,kk)*dotup(6,2))*propup(10,4) - 
     -       (bfun(4,2,kk) + 2*bfun(5,2,kk)*dotup(7,2))*
     -        propup(11,2) + (bfun(2,3,kk) - bfun(4,3,kk) - 
     -          2*bfun(5,3,kk)*dotup(4,1))*propup(11,3) + 
     -       (-bfun(1,2,kk) + bfun(3,2,kk))*propup(12,1) - 
     -       (bfun(4,2,kk) +  2*bfun(5,2,kk)*
     -           (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -             dotup(7,2)))*propup(12,1)) + 
     -    bfun(1,1,kk)*(propup(1,1) - 
     -       3*(propup(10,1) + propup(12,2))) + 
     -    3*((bfun(2,1,kk) - bfun(4,1,kk))*propup(12,2) + 
     -       bfun(3,1,kk)*(propup(10,1) + propup(12,2)) + 
     -       (-bfun(5,2,kk) + bfun(9,1,kk) + bfun(9,2,kk))*
     -    propup(14,1) - bfun(5,1,kk)* (2*dotup(5,1)*propup(10,1) + 
     -          2*(dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -        dotup(7,1))*propup(12,2) + propup(14,1))))/3.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(5,kk) =((2*(bfun(7,3,kk)*propup(6,3) + 
     -       bfun(8,3,kk)*propup(7,3) -  4*(bfun(6,4,kk) - 
     -          (bfun(9,3,kk) - bfun(9,4,kk))*
     -           (dotup(1,2) - dotup(6,1) - dotup(6,2)))*
     -     propup(8,3) +4*bfun(6,3,kk)*(propup(5,3) + propup(8,3)) + 
     -       3*(4*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -          bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2))*
     -        propup(9,1) + bfun(8,3,kk)*propup(9,3) - 
     -       bfun(8,4,kk)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(1,2)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(1,2)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(1,2)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(1,2)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(4,1)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(4,1)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(4,1)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(4,1)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(4,2)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(4,2)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(4,2)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(4,2)*propup(9,3) + 
     -       6*bfun(5,4,kk)*dotup(6,2)*propup(10,4) - 
     -       3*(bfun(8,2,kk)*propup(9,1) + 
     -          (-bfun(1,4,kk) + bfun(3,4,kk))*
     -           propup(10,4) + bfun(2,1,kk)*propup(11,1))
     -        - 3*(-bfun(1,1,kk) + bfun(2,1,kk) + 
     -          bfun(3,1,kk))*propup(12,2) - 
     -       3*(bfun(6,1,kk) - bfun(6,2,kk) + bfun(8,2,kk))*
     -        propup(13,1) +3*bfun(8,1,kk)*
     -        (propup(7,1) + propup(9,1) + propup(13,1)) + 
     -    3*(bfun(4,1,kk)*(propup(4,1)+propup(11,1) + propup(12,2))
     -      + 2*bfun(5,1,kk)*(dotup(7,1)*propup(11,1) + 
     -             (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -                dotup(7,1))*propup(12,2)) + (4*bfun(5,1,kk)*
     -              (dotup(1,2) - dotup(7,1) - dotup(7,2))
     -              + (bfun(9,1,kk) - bfun(9,2,kk))*
     -              (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -                dotup(7,2)) +  4*bfun(5,2,kk)*
     -              (-dotup(1,2) + dotup(7,1) + dotup(7,2)))
     -     *propup(13,1))) -3*(2*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(6,kk) =((6*bfun(4,2,kk)*(propup(4,2) + propup(11,2) + 
     -       propup(12,1))+12*bfun(5,2,kk)*(dotup(7,2)*propup(11,2) + 
     -       (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2))*propup(12,1)) +6*(4*bfun(5,2,kk)*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2)) - 
     -       (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(5,1) + dotup(5,2) - dotup(7,1) - dotup(7,2)) + 
     -       4*bfun(5,1,kk)*(dotup(7,1) + dotup(7,2)))* propup(13,1) + 
     -    2*(bfun(7,4,kk)*propup(6,4) + bfun(8,4,kk)*propup(7,4) - 
     -       4*(bfun(6,3,kk) +(bfun(9,3,kk) - bfun(9,4,kk))*
     -           (dotup(1,2) - dotup(6,1) - dotup(6,2)))*propup(8,3) + 
     -       4*bfun(6,4,kk)*(propup(5,4) + propup(8,3)) - 
     -       3*(4*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -          bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2))*
     -        propup(9,1) - bfun(8,3,kk)*propup(9,3) + 
     -       bfun(8,4,kk)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(1,2)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(1,2)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(1,2)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(1,2)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(4,1)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(4,1)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(4,1)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(4,1)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(4,2)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(4,2)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(4,2)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(4,2)*propup(9,3) + 
     -       6*bfun(5,3,kk)*dotup(6,1)*propup(10,3) - 
     -       12*bfun(5,1,kk)*dotup(1,2)*propup(13,1) - 
     -       3*bfun(8,1,kk)*(propup(9,1) + propup(13,1)) + 
     -       3*bfun(8,2,kk)*(propup(7,2)+propup(9,1)+propup(13,1))+ 
     -       3*((bfun(1,3,kk) - bfun(3,3,kk))*propup(10,3) + 
     -       (bfun(1,2,kk)-bfun(3,2,kk))*propup(12,1)-bfun(2,2,kk)*
     -           (propup(11,2) + propup(12,1)) + 
     -          (bfun(6,1,kk) - bfun(6,2,kk))*propup(13,1)))
     -      - 3*(-4*bfun(5,1,kk) + 2*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))
	 end do
	end if

c--------------------------------------------------------------------

c...c6
	if(ixiccstate.eq.2 .or. ixiccstate.eq.3) then
	  do kk=1,64
	   colmat(1,kk) =((2*(-(bfun(7,1,kk)*propup(6,1)) + 
     -       2*(bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(5,1) - dotup(5,2))*
     -        propup(8,1) + bfun(8,2,kk)*propup(9,1) - 
     -       4*bfun(5,1,kk)*dotup(1,2)*propup(9,1) + 
     -       4*bfun(5,2,kk)*dotup(1,2)*propup(9,1) - 
     -       bfun(9,1,kk)*dotup(1,2)*propup(9,1) + 
     -       bfun(9,2,kk)*dotup(1,2)*propup(9,1) + 
     -       4*bfun(5,1,kk)*dotup(7,1)*propup(9,1) - 
     -       4*bfun(5,2,kk)*dotup(7,1)*propup(9,1) + 
     -       bfun(9,1,kk)*dotup(7,1)*propup(9,1) - 
     -       bfun(9,2,kk)*dotup(7,1)*propup(9,1) + 
     -       4*bfun(5,1,kk)*dotup(7,2)*propup(9,1) - 
     -       4*bfun(5,2,kk)*dotup(7,2)*propup(9,1) + 
     -       bfun(9,1,kk)*dotup(7,2)*propup(9,1) - 
     -       bfun(9,2,kk)*dotup(7,2)*propup(9,1) - 
     -       3*bfun(8,4,kk)*propup(9,3) + 
     -       12*bfun(5,3,kk)*dotup(1,2)*propup(9,3) - 
     -       12*bfun(5,4,kk)*dotup(1,2)*propup(9,3) + 
     -       3*bfun(9,3,kk)*dotup(1,2)*propup(9,3) - 
     -       3*bfun(9,4,kk)*dotup(1,2)*propup(9,3) - 
     -       12*bfun(5,3,kk)*dotup(4,1)*propup(9,3) + 
     -       12*bfun(5,4,kk)*dotup(4,1)*propup(9,3) - 
     -       3*bfun(9,3,kk)*dotup(4,1)*propup(9,3) + 
     -       3*bfun(9,4,kk)*dotup(4,1)*propup(9,3) - 
     -       12*bfun(5,3,kk)*dotup(4,2)*propup(9,3) + 
     -       12*bfun(5,4,kk)*dotup(4,2)*propup(9,3) - 
     -       3*bfun(9,3,kk)*dotup(4,2)*propup(9,3) + 
     -       3*bfun(9,4,kk)*dotup(4,2)*propup(9,3) + 
     -       3*bfun(8,3,kk)*(propup(7,3) + propup(9,3)) + 
     -       3*(-bfun(2,3,kk) + bfun(4,3,kk))*
     -        propup(11,3) + 
     -       6*bfun(5,3,kk)*dotup(4,1)*propup(11,3) + 
     -       3*(bfun(8,2,kk) - 4*bfun(5,1,kk)*dotup(1,2))*
     -        propup(13,1) - bfun(6,2,kk)*
     -        (2*propup(8,1) + 3*propup(13,1)) + 
     -       bfun(6,1,kk)*(2*(propup(5,1) + propup(8,1)) + 
     -          3*propup(13,1)) - bfun(8,1,kk)*
     -        (propup(7,1) + propup(9,1) + 3*propup(13,1))
     -       + 3*((-bfun(2,2,kk) + bfun(4,2,kk))* propup(12,1) - 
     -          bfun(3,2,kk)* (propup(10,2) + propup(12,1)) + 
     -      bfun(1,2,kk)*(propup(1,2) + propup(10,2) + propup(12,1))
     -            + 2*bfun(5,2,kk)*  (dotup(5,2)*propup(10,2) + 
     -             (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2))*propup(12,1)) + (4*bfun(5,2,kk)*
     -              (dotup(1,2) - dotup(7,1) - dotup(7,2))
     -              - (bfun(9,1,kk) - bfun(9,2,kk))*
     -              (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -      dotup(7,2)) +4*bfun(5,1,kk)*(dotup(7,1) + dotup(7,2)))
     -            *propup(13,1))) - 
     -    3*(-4*bfun(5,1,kk) + 2*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(2,kk) =((2*(-(bfun(7,2,kk)*propup(6,2)) - 
     -       2*bfun(6,1,kk)*propup(8,1) - 
     -       2*bfun(9,1,kk)*dotup(1,2)*propup(8,1) + 
     -       2*bfun(9,2,kk)*dotup(1,2)*propup(8,1) + 
     -       2*bfun(9,1,kk)*dotup(5,1)*propup(8,1) - 
     -       2*bfun(9,2,kk)*dotup(5,1)*propup(8,1) + 
     -       2*bfun(9,1,kk)*dotup(5,2)*propup(8,1) - 
     -       2*bfun(9,2,kk)*dotup(5,2)*propup(8,1) + 
     -       bfun(8,1,kk)*propup(9,1) + 
     -       4*bfun(5,1,kk)*dotup(1,2)*propup(9,1) - 
     -       4*bfun(5,2,kk)*dotup(1,2)*propup(9,1) + 
     -       bfun(9,1,kk)*dotup(1,2)*propup(9,1) - 
     -       bfun(9,2,kk)*dotup(1,2)*propup(9,1) - 
     -       4*bfun(5,1,kk)*dotup(7,1)*propup(9,1) + 
     -       4*bfun(5,2,kk)*dotup(7,1)*propup(9,1) - 
     -       bfun(9,1,kk)*dotup(7,1)*propup(9,1) + 
     -       bfun(9,2,kk)*dotup(7,1)*propup(9,1) - 
     -       4*bfun(5,1,kk)*dotup(7,2)*propup(9,1) + 
     -       4*bfun(5,2,kk)*dotup(7,2)*propup(9,1) - 
     -       bfun(9,1,kk)*dotup(7,2)*propup(9,1) + 
     -       bfun(9,2,kk)*dotup(7,2)*propup(9,1) - 
     -       3*bfun(8,3,kk)*propup(9,3) + 
     -       3*(-bfun(2,4,kk) + bfun(4,4,kk))* propup(11,4) + 
     -       3*(-((4*bfun(5,3,kk) - 4*bfun(5,4,kk) + 
     -               bfun(9,3,kk) - bfun(9,4,kk))*
     -       (dotup(1,2) - dotup(4,1) - dotup(4,2))* propup(9,3))+ 
     -          bfun(8,4,kk)*(propup(7,4) + propup(9,3)) + 
     -          2*bfun(5,4,kk)*dotup(4,2)*propup(11,4)) - 
     -       3*(bfun(6,1,kk) - bfun(8,1,kk))*propup(13,1) + 
     -       bfun(6,2,kk)*(2*(propup(5,2) + propup(8,1)) + 
     -          3*propup(13,1)) - bfun(8,2,kk)*
     -        (propup(7,2) + propup(9,1) + 3*propup(13,1))
     -       + 3*((-bfun(2,1,kk) + bfun(4,1,kk))*propup(12,2) - 
     -          bfun(3,1,kk)* (propup(10,1) + propup(12,2)) + 
     -      bfun(1,1,kk)*(propup(1,1) + propup(10,1) + propup(12,2))
     -            + 2*bfun(5,1,kk)*(dotup(5,1)*propup(10,1) + 
     -             (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -                dotup(7,1))*propup(12,2)) +(4*bfun(5,1,kk)*
     -              (dotup(1,2) - dotup(7,1) - dotup(7,2))
     -              + (bfun(9,1,kk) - bfun(9,2,kk))*
     -              (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -                dotup(7,2)) + 4*bfun(5,2,kk)*
     -              (-dotup(1,2) + dotup(7,1) + dotup(7,2)))
     -      *propup(13,1))) -3*(2*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(3,kk) =((-(bfun(2,2,kk)*propup(2,2)) - 
     -    bfun(4,2,kk)*propup(4,2) + 
     -    bfun(3,2,kk)*(2*propup(3,2) + 3*propup(10,2)) + 
     -    3*(bfun(2,2,kk) + bfun(3,2,kk) - bfun(4,2,kk) - 
     -       2*bfun(5,2,kk)*
     -        (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2)))*propup(12,1) - 
     -    bfun(1,2,kk)*(propup(1,2) + 
     -       3*(propup(10,2) + propup(12,1))) - 
     -    3*(-(bfun(7,1,kk)*propup(6,1)) - 
     -       bfun(7,4,kk)*propup(6,4) + 
     -       2*bfun(5,2,kk)*dotup(5,2)*propup(10,2) + 
     -       (bfun(1,3,kk) - bfun(3,3,kk) + 
     -          2*bfun(5,3,kk)*dotup(6,1))*propup(10,3) + 
     -       (-bfun(2,4,kk) + bfun(4,4,kk) + 
     -          2*bfun(5,4,kk)*dotup(4,2))*propup(11,4) + 
     -       bfun(1,1,kk)*propup(12,2)) + 
     -    3*(bfun(3,1,kk)*propup(12,2) - 
     -       bfun(4,1,kk)*(propup(11,1) + propup(12,2)) + 
     -       bfun(2,1,kk)*
     -        (propup(2,1) + propup(11,1) + propup(12,2)) + 
     -       2*bfun(5,1,kk)*
     -        (-(dotup(7,1)*propup(11,1)) - 
     -          (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -             dotup(7,1))*propup(12,2)) - 
     -       (bfun(5,1,kk) + bfun(5,2,kk) - bfun(9,1,kk) - 
     -          bfun(9,2,kk))*propup(14,1)))/3.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(4,kk) =((-(bfun(2,1,kk)*propup(2,1)) + 
     -    2*bfun(3,1,kk)*propup(3,1) - 
     -    bfun(4,1,kk)*propup(4,1) + 
     -    3*bfun(2,2,kk)*
     -     (propup(2,2) + propup(11,2) + propup(12,1)) + 
     -    3*(bfun(7,2,kk)*propup(6,2) + 
     -       bfun(7,3,kk)*propup(6,3) + 
     -       (-bfun(1,4,kk) + bfun(3,4,kk) - 
     -          2*bfun(5,4,kk)*dotup(6,2))*propup(10,4) - 
     -       (bfun(4,2,kk) + 2*bfun(5,2,kk)*dotup(7,2))*
     -        propup(11,2) + 
     -       (bfun(2,3,kk) - bfun(4,3,kk) - 
     -          2*bfun(5,3,kk)*dotup(4,1))*propup(11,3) + 
     -       (-bfun(1,2,kk) + bfun(3,2,kk))*propup(12,1) - 
     -       (bfun(4,2,kk) + 
     -          2*bfun(5,2,kk)*
     -           (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -             dotup(7,2)))*propup(12,1)) - 
     -    bfun(1,1,kk)*(propup(1,1) + 
     -       3*(propup(10,1) + propup(12,2))) + 
     -    3*((bfun(2,1,kk) - bfun(4,1,kk))*propup(12,2) + 
     -       bfun(3,1,kk)*(propup(10,1) + propup(12,2)) + 
     -       (-bfun(5,2,kk) + bfun(9,1,kk) + bfun(9,2,kk))*
     -        propup(14,1) - 
     -       bfun(5,1,kk)*
     -        (2*dotup(5,1)*propup(10,1) + 
     -          2*(dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -          dotup(7,1))*propup(12,2) + propup(14,1))))/3.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(5,kk) =((2*(-(bfun(7,3,kk)*propup(6,3)) - 
     -       bfun(8,3,kk)*propup(7,3) - 
     -       2*(bfun(6,4,kk) - 
     -          (bfun(9,3,kk) - bfun(9,4,kk))*
     -           (dotup(1,2) - dotup(6,1) - dotup(6,2)))*
     -        propup(8,3) + 
     -       2*bfun(6,3,kk)*(propup(5,3) + propup(8,3)) + 
     -       3*(4*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -          bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2))*
     -        propup(9,1) - bfun(8,3,kk)*propup(9,3) + 
     -       bfun(8,4,kk)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(1,2)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(1,2)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(1,2)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(1,2)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(4,1)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(4,1)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(4,1)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(4,1)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(4,2)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(4,2)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(4,2)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(4,2)*propup(9,3) + 
     -       6*bfun(5,4,kk)*dotup(6,2)*propup(10,4) - 
     -       3*(bfun(8,2,kk)*propup(9,1) + 
     -          (-bfun(1,4,kk) + bfun(3,4,kk))*
     -           propup(10,4) + bfun(2,1,kk)*propup(11,1))
     -        - 3*(-bfun(1,1,kk) + bfun(2,1,kk) + 
     -          bfun(3,1,kk))*propup(12,2) - 
     -       3*(bfun(6,1,kk) - bfun(6,2,kk) + bfun(8,2,kk))*
     -        propup(13,1) + 
     -       3*bfun(8,1,kk)*
     -        (propup(7,1) + propup(9,1) + propup(13,1)) + 
     -       3*(bfun(4,1,kk)*
     -           (propup(4,1) + propup(11,1) + propup(12,2))
     -            + 2*bfun(5,1,kk)*
     -           (dotup(7,1)*propup(11,1) + 
     -             (dotup(4,4) + dotup(4,6) + dotup(5,1) + 
     -                dotup(7,1))*propup(12,2)) + 
     -          (4*bfun(5,1,kk)*
     -              (dotup(1,2) - dotup(7,1) - dotup(7,2))
     -              + (bfun(9,1,kk) - bfun(9,2,kk))*
     -              (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -                dotup(7,2)) + 
     -             4*bfun(5,2,kk)*
     -              (-dotup(1,2) + dotup(7,1) + dotup(7,2)))
     -            *propup(13,1))) - 
     -    3*(2*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))

	   colmat(6,kk) =((6*bfun(4,2,kk)*(propup(4,2) + propup(11,2) + 
     -       propup(12,1)) + 
     -    12*bfun(5,2,kk)*
     -     (dotup(7,2)*propup(11,2) + 
     -       (dotup(4,4) + dotup(4,6) + dotup(5,2) + 
     -          dotup(7,2))*propup(12,1)) + 
     -    6*(4*bfun(5,2,kk)*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2)) - 
     -       (bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(5,1) + dotup(5,2) - dotup(7,1) - 
     -          dotup(7,2)) + 
     -       4*bfun(5,1,kk)*(dotup(7,1) + dotup(7,2)))*
     -     propup(13,1) + 
     -    2*(-(bfun(7,4,kk)*propup(6,4)) - 
     -       bfun(8,4,kk)*propup(7,4) - 
     -       2*(bfun(6,3,kk) + 
     -          (bfun(9,3,kk) - bfun(9,4,kk))*
     -           (dotup(1,2) - dotup(6,1) - dotup(6,2)))*
     -        propup(8,3) + 
     -       2*bfun(6,4,kk)*(propup(5,4) + propup(8,3)) - 
     -       3*(4*bfun(5,1,kk) - 4*bfun(5,2,kk) + 
     -          bfun(9,1,kk) - bfun(9,2,kk))*
     -        (dotup(1,2) - dotup(7,1) - dotup(7,2))*
     -        propup(9,1) + bfun(8,3,kk)*propup(9,3) - 
     -       bfun(8,4,kk)*propup(9,3) + 
     -       4*bfun(5,3,kk)*dotup(1,2)*propup(9,3) - 
     -       4*bfun(5,4,kk)*dotup(1,2)*propup(9,3) + 
     -       bfun(9,3,kk)*dotup(1,2)*propup(9,3) - 
     -       bfun(9,4,kk)*dotup(1,2)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(4,1)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(4,1)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(4,1)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(4,1)*propup(9,3) - 
     -       4*bfun(5,3,kk)*dotup(4,2)*propup(9,3) + 
     -       4*bfun(5,4,kk)*dotup(4,2)*propup(9,3) - 
     -       bfun(9,3,kk)*dotup(4,2)*propup(9,3) + 
     -       bfun(9,4,kk)*dotup(4,2)*propup(9,3) + 
     -       6*bfun(5,3,kk)*dotup(6,1)*propup(10,3) - 
     -       12*bfun(5,1,kk)*dotup(1,2)*propup(13,1) - 
     -       3*bfun(8,1,kk)*(propup(9,1) + propup(13,1)) + 
     -       3*bfun(8,2,kk)*
     -        (propup(7,2) + propup(9,1) + propup(13,1)) + 
     -       3*((bfun(1,3,kk) - bfun(3,3,kk))*
     -           propup(10,3) +(bfun(1,2,kk) - bfun(3,2,kk))*
     -           propup(12,1) -bfun(2,2,kk)*
     -           (propup(11,2) + propup(12,1)) + 
     -          (bfun(6,1,kk) - bfun(6,2,kk))*propup(13,1)))
     -      - 3*(-4*bfun(5,1,kk) + 2*bfun(5,2,kk) + 
     -       bfun(9,1,kk) + bfun(9,2,kk))*propup(14,1))/6.)
     &    /dsqrt(dotup(8,6)*dotup(8,7)*dotup(8,4)*dotup(8,5))
	 end do
	end if

      return
	end

