c leptonic tensors:
      double complex wel, wve, wmu, wvu, we, wu
      double complex xxew,xxuw,wet,wut
      double precision qe, qu, qww
      common /vbf_wp_wp_weuspinor/ wel(6), wve(6), wmu(6), wvu(6),
     1                   we(6), wu(6), qe(0:4), qu(0:4), qww(0:4),
     2			 wet(6),wut(6), xxew,xxuw

c  leptonic tensors
      double complex wppwpp,wmmwmm, 
     #		       awen,zwen,waen,wzen,
     #		       awmu,zwmu,wamu,wzmu     
      logical lfp,lfm
      common /vbf_wp_wp_tenswpm/ wppwpp(0:3,0:3,3),wmmwmm(0:3,0:3,3),
     #		       awen(0:3,0:3,2,3),zwen(0:3,0:3,2,3),
     #		       waen(0:3,0:3,2,3),wzen(0:3,0:3,2,3),
     #		       awmu(0:3,0:3,2,3),zwmu(0:3,0:3,2,3),
     #		       wamu(0:3,0:3,2,3),wzmu(0:3,0:3,2,3),	
     #		       lfp(4),lfm(4)
      integer is1,is3
