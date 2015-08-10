* Published in Phys.Rev.C76:035201,2007.
* e-Print: nucl-th/0612079
*   mode 1 - Coulomb distorsion only
*        2 - full TPE corrections
*        3 - no corrections
      function GeArringtonLQ(q2)
      real b(5,3)
      data b/3.440,-0.178,-1.212,1.176,-0.284,
     &       3.478,-0.140,-1.311,1.128,-0.233,
     &       3.366,-0.189,-1.263,1.351,-0.301
     &     /
      save b
      integer mFF
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
      n=mFF/10
      if(n.lt.1.or.n.gt.3) n=3
      GeArringtonLQ=1./(1.+b(1,n)*q2/(1+b(2,n)*q2/(1.+b(3,n)*q2/
     &     (1.+b(4,n)*q2/(1+b(5,n)*q2)))))
      if(First.ne.0) then
         print *,'Ge according to Phys Rev C76(2007)035201'
         print *,'  b',(b(i,n),i=1,5)
         First=0
      endif
      end

