* \mu_p excluded
*  E.~J.~Brash, A.~Kozlov, S.~Li and G.~M.~Huber,
*  %``New empirical fits to the proton electromagnetic form factors,''
*  Phys.\ Rev.\  C {\bf 65} (2002) 051001
      real function GePhysRevC65_2002_051001(q2)
      implicit none
      real q2
      real GmPhysRevC65_2002_051001
      integer mFF
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
      real r
      r=1.-0.130*(q2-0.04)
      if(r.lt.0) then
         GePhysRevC65_2002_051001=0
      else
         if(r.gt.1.) r=1.
         GePhysRevC65_2002_051001=r*GmPhysRevC65_2002_051001(q2)
      endif
      if(First.ne.0) then
         print *,'Ge according to Phys Rev C65(2002)051001'
         First=0
      endif
      end

