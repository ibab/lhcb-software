* \mu_p excluded
*  E.~J.~Brash, A.~Kozlov, S.~Li and G.~M.~Huber,
*  %``New empirical fits to the proton electromagnetic form factors,''
*  Phys.\ Rev.\  C {\bf 65} (2002) 051001
      real function GmPhysRevC65_2002_051001(q2)
      implicit none
      real q2
      integer mFF
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
      real Q
      Q=sqrt(abs(q2))
      GmPhysRevC65_2002_051001=1./(
     &     1.+Q*(0.116+Q*(2.874+Q*(0.241+Q*(1.006+Q*0.345)))))
      if(First.ne.0) then
         print *,'Gm according to Phys Rev C65(2002)051001'
         First=0
      endif
      end

