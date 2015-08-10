* \mu_p excluded
      real function GeNuclPhysA596_1996_367(q2)
      real Mp
      parameter (Mp=0.93827231)
      integer First
      data First/1/
      save First
      common /NuclPhysA596_1996_367/ tau,F1,F2,q2prev
*!      save /NuclPhysA596_1996_367/
      real log1,log2,log3,d1,d2,d3
      if(q2.ne.q2prev) then
         q2prev=q2
         tau=q2/4./Mp**2
         log1=log((9.733+q2)/0.350)**2.148
         d1=0.611+q2
         d2=1.039+q2
         d3=2.560+q2
         Fs1=(9.464/d1-9.054/d2-0.410/d3)/log1
         Fs2=(-1.549/d1+1.985/d2-0.436/d3)/log1
         log2=log((9.733-0.500)/0.350)**2.148
         log3=log((9.733-0.400)/0.350)**2.148
         d1=2.103+q2
         d2=2.734+q2
         d3=2.835+q2
         Fv1=((1.032*log2+0.088*log3/(1.+q2/0.318)**2)/2./(1.+q2/0.550)-
     &        38.885/d1+425.007/d2-389.742/d3)/log1
         Fv2=((5.782*log2+0.391*log3/(1.+q2/0.142))/2./(1.+q2/0.536)-
     &        73.535/d1+83.211/d2-29.467/d3)/log1
         F1=Fv1+Fs1
         F2=Fv2+Fs2
      endif
      GeNuclPhysA596_1996_367=F1-tau*F2
      if(First.ne.0) then
         print *,'Ge according to Nucl.Phys. A596(1996)367'
         First=0
      endif
      end

