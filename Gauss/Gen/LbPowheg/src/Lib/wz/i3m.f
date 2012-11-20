      double complex function wz_I3m(s1,s2,s3)
C     This is the function wz_I3m, a massless triangle with all three external 
C     lines offshell defined in BDK
C     %\cite{Bern:1997sc}
C     \bibitem{Bern:1997sc}
C     Z.~Bern, L.~J.~Dixon and D.~A.~Kosower,
C     %``One-loop amplitudes for e+ e- to four partons,''
C     Nucl.\ Phys.\ B {\bf 513}, 3 (1998)
C     [arXiv:hep-ph/9708239].
C     %%CITATION = HEP-PH 9708239;%%
C     defined in their equation II.9
C     \int da_1 da_2 da_3 /(-a_1*a_2*s1-a_2*a_3*s2-a_3*a_1*s3)
       implicit none
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/pwhg_math.h'
      double precision two,pisq,one
      parameter (two=2d0,pisq=pi**2,one=1d0)
      double precision s1,s2,s3,smax,smid,smin,del3,rtdel3
      double precision wz_I3m1a,flag
      double complex wz_I3m1b

      smax=max(s1,s2,s3)
      smin=min(s1,s2,s3)
      smid=s1+s2+s3-smax-smin
      del3=s1**2+s2**2+s3**2-two*(s1*s2+s2*s3+s3*s1)      

      if (del3 .gt. 0) then
      rtdel3=sqrt(del3)
         if (smax .lt. 0) then
c---case all negative
             flag=0d0
             wz_I3m=wz_I3m1b(smax,smid,smin,rtdel3,flag)
         elseif (smin .gt. 0) then
c---case all positive
             flag=0d0
             wz_I3m=-wz_I3m1b(-smin,-smid,-smax,rtdel3,flag)
         elseif ((smid .lt. 0) .and. (smin .lt. 0)) then
c---case two negative and one positive
             flag=+1d0
             wz_I3m=wz_I3m1b(smin,smid,smax,rtdel3,flag)
         elseif ((smax .gt. 0).and.(smid .gt. 0)) then
c---case two positive and one negative
             flag=-1d0
             wz_I3m=-wz_I3m1b(-smax,-smid,-smin,rtdel3,flag)
         endif
      elseif (del3 .lt. 0) then 
      rtdel3=sqrt(-del3)
         if (smax .lt. 0) then
c---case all negative
             wz_I3m=+dcmplx(wz_I3m1a(+s1,+s2,+s3,rtdel3))
         elseif (smin .gt. 0) then
c---case all positive
             wz_I3m=-dcmplx(wz_I3m1a(-s1,-s2,-s3,rtdel3))  
          endif
      endif

      return
      end     


      double precision function wz_I3m1a(s1,s2,s3,rtmdel)
      implicit none
C     symmetric form of Lu and Perez
C     %\cite{Lu:1992ny}
c     \bibitem{Lu:1992ny}
c     H.~J.~Lu and C.~A.~Perez,
c     %``Massless one loop scalar three point integral and associated Clausen,
c     %Glaisher and L functions,''
c     SLAC-PUB-5809
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/pwhg_math.h'
      double precision two,pisq,one
      parameter (two=2d0,pisq=pi**2,one=1d0)
      double precision s1,s2,s3,d1,d2,d3,rtmdel,arg1,arg2,arg3,wz_DCLAUS

      d1=s1-s2-s3
      d2=s2-s3-s1
      d3=s3-s1-s2
      
      arg1=two*datan(rtmdel/d1)
      arg2=two*datan(rtmdel/d2)
      arg3=two*datan(rtmdel/d3)
      wz_I3m1a=two/rtmdel*(wz_DCLAUS(arg1)+wz_DCLAUS(arg2)+wz_DCLAUS(arg3))

      end

      
      double complex function wz_I3m1b(s1,s2,s3,rtdel,flag)
      implicit none
C     form of Ussyukina and Davydychev
C %\cite{Usyukina:1994iw}
C \bibitem{Usyukina:1994iw}
C   N.~I.~Usyukina and A.~I.~Davydychev,
C   %``New results for two loop off-shell three point diagrams,''
C  Phys.\ Lett.\ B {\bf 332}, 159 (1994)
C  [arXiv:hep-ph/9402223].
C  %%CITATION = HEP-PH 9402223;%%

!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/pwhg_math.h'
      double precision two,pisq,one
      double complex impi
      parameter (two=2d0,pisq=pi**2,one=1d0,impi=(0d0,pi))
      double precision s1,s2,s3,d3,temp,wz_DDILOG,wz_xlog,ylog,rat
      double precision x,y,rho,rtdel,argx,argy,argdlx,argdly,flag
      d3=s3-s1-s2
      x=s1/s3
      y=s2/s3
      rat=0.5d0*(d3+rtdel)/s3
      if (abs(rat) .lt. 1d-3) rat=2d0*s1*s2/(s3*(d3-rtdel))
      rho=1d0/rat
      argx=rho*x
      argy=rho*y
      argdlx=-argx
      argdly=-argy

      if ((argdlx .gt. 1d0) .or. (argdly .gt. 1d0)) then
      write(6,*) 'problems with call of wz_I3m1b'
      write(6,*) 'argdlx',argdlx
      write(6,*) 'argdly',argdly
      stop
      endif

      wz_xlog=log(abs(argx))
      ylog=log(abs(argy))
      temp=wz_xlog*ylog+pisq/3d0+(ylog-wz_xlog)*log((one+argy)/(one+argx))
     & +two*(wz_DDILOG(argdlx)+wz_DDILOG(argdly))
      wz_I3m1b=Dcmplx(temp-abs(flag)*pisq)+impi*Dcmplx(flag*(wz_xlog+ylog))
      wz_I3m1b=-wz_I3m1b/Dcmplx(rtdel)
      end





