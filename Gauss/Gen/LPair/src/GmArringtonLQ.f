* Published in Phys.Rev.C76:035201,2007.
* e-Print: nucl-th/0612079
      function GmArringtonLQ(q2)
      real b(5,3)
      data b/3.173,-0.314,-1.165,5.619,-1.087,
     &       3.224,-0.313,-0.868,4.278,-1.102,
     &       3.205,-0.318,-1.228,5.619,-1.116
     &     /
      save b
      integer mFF
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
c
      n=mFF/10
      if(n.lt.1.or.n.gt.3) n=3
      GmArringtonLQ=1./(1.+b(1,n)*q2/(1+b(2,n)*q2/(1.+b(3,n)*q2/
     &     (1.+b(4,n)*q2/(1+b(5,n)*q2)))))
      if(First.ne.0) then
         print *,'Gm according to Phys Rev C76(2007)035201'
         print *,'  b',(b(i,n),i=1,5)
         First=0
      endif
      end

