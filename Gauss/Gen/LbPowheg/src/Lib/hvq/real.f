      subroutine hvq_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_math.h'
      include 'src/Lib/hvq/pwhg_st.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      real * 8 p(0:3,nlegreal),amp2
      integer rflav(nlegreal)
      character * 2 prc
      common/hvq_process/prc
      real * 8 s,q1q,xm2,tk,uk,q2q,w1h,w2h,x,y,cth2
      integer ifl
      real * 8 hvq_dotp,hvq_fppx
      external hvq_dotp,hvq_fppx
c compute mnr invariants
      s=2*hvq_dotp(p(0,1),p(0,2))
      q1q=-2*hvq_dotp(p(0,1),p(0,3))
      xm2=kn_masses(3)**2
      tk=-2*hvq_dotp(p(0,1),p(0,5))
      uk=-2*hvq_dotp(p(0,2),p(0,5))
      q2q=-2*hvq_dotp(p(0,2),p(0,4))
c     w1=-q1+q2-tk, w2=q1-q2-uk, w1(w2) = w1h(w2h) *s*(1-x)/2
      w1h=2*(q2q-q1q-tk)/(-tk-uk)
      w2h=2*(q1q-q2q-uk)/(-tk-uk)
      x=1+(tk+uk)/s
      y=(tk-uk)/(s*(1-x))
c     cth2 is not used by hvq_FGG,hvq_FQG,hvq_FQQ, unless we are very near the collinear limit
      cth2=0
      if(rflav(1).eq.0.and.rflav(2).eq.0) then
         prc='gg'
         ifl=0
      elseif(rflav(1).gt.0.and.rflav(2).lt.0) then
         prc='qq'
         ifl=1
      elseif(rflav(1).lt.0.and.rflav(2).gt.0) then
         prc='qq'
         ifl=-1
      elseif(rflav(1).gt.0.and.rflav(2).eq.0) then
         prc='qg'
         ifl=1
      elseif(rflav(1).lt.0.and.rflav(2).eq.0) then
         prc='qg'
         ifl=-1
      elseif(rflav(1).eq.0.and.rflav(2).gt.0) then
         prc='gq'
         ifl=-1
      elseif(rflav(1).eq.0.and.rflav(2).lt.0) then
         prc='gq'
         ifl=1
      endif
      amp2= hvq_fppx(ifl,s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)/(4*tk*uk)
      amp2=amp2 * (4*pi*st_alpha)**3 /(st_alpha/(2*pi)) * 2 * s
      end

      function hvq_fppx(ifl,s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
c front-end to hvq_fpp in hvqcrsx.f that deals with the cases:
c prc=gq,
c ifl=-1, i.e. light quark -> light antiquark
c Thus
c prc ifl
c gg  0     -> gluon gluon
c qq  1     -> quark antiquark
c qq -1     -> antiquark quark
c qg  1     -> quark gluon
c qg -1     -> antiquark gluon
c gq -1     -> gluon quark
c gq  1     -> gluon antiquark
      implicit none
      real * 8 hvq_fppx,s,x,y,xm2,q1q,q2q,w1h,w2h,cth2
      integer ifl
      character * 2 prc
      common/hvq_process/prc
      real * 8 hvq_fpp
      external hvq_fpp
      if(prc.eq.'gg') then
         if(ifl.eq.0) then
            continue
         else
            goto 998
         endif
      elseif(prc.eq.'qq') then
         if(ifl.eq.1) then
            continue
         elseif(ifl.eq.-1) then
            call hvq_exchk1k2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
         else
            goto 998
         endif
      elseif(prc.eq.'qg') then
         if(ifl.eq.1) then
            continue
         elseif(ifl.eq.-1) then
            call hvq_exchk1k2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
         else
            goto 998
         endif
      elseif(prc.eq.'gq') then
         prc='qg'
         call hvq_exchp1p2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
         if(ifl.eq.-1) then
            continue
         elseif(ifl.eq.1) then
            call hvq_exchk1k2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
         else
            goto 998
         endif
      else
         goto 998
      endif
      hvq_fppx=hvq_fpp(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
      goto 999
 998  write(*,*) 'hvq_fppx called with inconsistent prc,jfl=',prc,ifl
      call exit(-1)
 999  end



      subroutine hvq_exchk1k2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
c exchange of momenta of heavy quarks
      implicit none
      real * 8 s,x,y,xm2,q1q,q2q,w1h,w2h,cth2
      real * 8 tk,uk,tmp
      tk=-0.5d0*s*(1-x)*(1-y)
      uk=-0.5d0*s*(1-x)*(1+y)
c exchange
      q1q=-s-tk-q1q
      q2q=-s-uk-q2q
      tmp=w1h
      w1h=w2h
      w2h=tmp
      cth2=-cth2
      end

      subroutine hvq_exchp1p2(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
c exchange of momenta of incoming partons
      implicit none
      real * 8 s,x,y,xm2,q1q,q2q,w1h,w2h,cth2
      real * 8 tk,uk,tmp
      tk=-0.5d0*s*(1-x)*(1-y)
      uk=-0.5d0*s*(1-x)*(1+y)
      tmp=q1q
      q1q=-s-uk-q2q
      q2q=-s-tk-tmp
      y=-y
c What happens to cth2?  theta2-> 2 pi - theta2, no change
      end




c NR06:  Normalization: (NB: 0 < th2 < pi)
c NR06:   d sigma = hvq_FGG/qq/qg * f(x1)*f(x2) * gs^6/(16 pi^2 64 pi^2)
c NR06:   1/(1-x)_rho (1/(1-y)_+ + 1/(1-y)_-) betax/s dcosth1 dth2 dy dx dx1 dx2
c NR06:  NB: hvq_FQG -> q(p1)+g(p2)
      function hvq_fpp(s0,x,y,xm20,q1q0,q2q0,w1h,w2h,cth2)
      implicit real * 8 (a-h,o-z)
      character * 2 prc
      common/hvq_process/prc
      s=1
      xm2=xm20/s0
      q1q=q1q0/s0
      q2q=q2q0/s0
      if(prc.eq.'gg') then
         hvq_fpp = hvq_FGG(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
      elseif(prc.eq.'qg') then
         hvq_fpp = hvq_FQG(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
      elseif(prc.eq.'qq') then
         hvq_fpp = hvq_FQQ(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)
      else
         write(*,*) 'hvq_fpp: non existent subprocess',prc
         stop
      endif
      end


       FUNCTION hvq_FGG(S,X,Y,XM2,Q1Q,Q2Q,W1H,W2H,CTH2)
c
c      d sigma_gg (f) = N g^6 / (64 pi^2 s) beta_x d costh1 d th2 dy dx
c                    1/(1-x)_rho ( 1/(1-y)_+ + 1/(1+y)_+ ) hvq_FGG
c
c           N = 1 / (4 pi)^2
c
       IMPLICIT REAL* 8(A-H,O-Z)
       REAL* 8 N
       TINY = 0
       V = 8
       N = 3
       N2 = N*N
       CF = 4.D0/3.D0
       CA = 3
       TK = - (1-X)*(1-Y)*S/2
       UK = - (1-X)*(1+Y)*S/2
       IF(1-X.LT.TINY)THEN
          Q2C=-S-Q2Q
          P13 = -Q1Q/2
          P23 = -Q2C/2
          P12 = S/2
          BORN = 1/(2*V*N)*(V/(P13*P23)-2*N**2/P12**2)*
     #           (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
          XXX  = N**2/(4*V)*( (P12+2*XM2)/P23-(P12+2*XM2)/P13
     #           +XM2**2/P13**2-XM2**2/P23**2+2*(P23-P13)/P12 )
          YYY  = 1/(4*V*N**2)*
     #           (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
     #          *(1/(P13*P23)+2*N**2/P12**2)
C         --------------------------------------------------------------
C         Fattori iconali moltiplicati per 4*tk*uk
C         P1.K = -TK/2
C         P2.K = -UK/2
C         P3.K = W1/2
C         P4.K = W2/2
C
          P14 = P23
          P24 = P13
C
          E13 = 16*(1+Y)*P13/W1H
          E14 = 16*(1+Y)*P14/W2H
          E23 = 16*(1-Y)*P23/W1H
          E24 = 16*(1-Y)*P24/W2H
          E12 = 16*P12
          E33 = 16*(1-Y)*(1+Y)* XM2/W1H**2
          E44 = 16*(1-Y)*(1+Y)* XM2/W2H**2
          E34 = 16*(1-Y)*(1+Y)* (S/2-XM2)/(W1H*W2H)
          SUM = BORN*( CF*(E13+E14+E23+E24-2*E12-E33-E44) +2*N*E12 )
     #        +        XXX *(E14+E23-E13-E24)
     #        +        YYY *(2*E12+2*E34-E13-E24-E14-E23)
          hvq_FGG = 1/(2*S)*SUM
       ELSEIF(1-Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = - X*Q1Q/2
          P23 = -Q2C/2
          P12 = S*X/2
          BX = SQRT(1-4*XM2/(S*X))
          CTH1 = (P23-P13)/P12/BX
          AZIDEP =
     #    -512*BX**2*(CTH1-1)*(CTH1+1)*V*(2*V+BX**2*CTH1**2*N2-N2)
     #    *(X-1)**2*XM2/((BX*CTH1-1)**2*(BX*CTH1+1)**2*X**2)
          AZIDEP = AZIDEP * (2*CTH2**2-1)/2 /(4*64)
          BORN = 1/(2*V*N)*(V/(P13*P23)-2*N**2/P12**2)*
     #           (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
          SUM = - BORN*(8*UK)*2*CA*(X/(1-X)+(1-X)/X+X*(1-X))
     #    + AZIDEP
          hvq_FGG = 1/(2*X*S)*SUM
       ELSEIF(1+Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = -Q1Q/2
          P23 = -X*Q2C/2
          P12 = S*X/2
          BX = SQRT(1-4*XM2/(S*X))
          CTH1 = (P23-P13)/P12/BX
          AZIDEP =
     #    -512*BX**2*(CTH1-1)*(CTH1+1)*V*(2*V+BX**2*CTH1**2*N2-N2)
     #    *(X-1)**2*XM2/((BX*CTH1-1)**2*(BX*CTH1+1)**2*X**2)
          AZIDEP = AZIDEP * (2*CTH2**2-1)/2 /(4*64)
          BORN = 1/(2*V*N)*(V/(P13*P23)-2*N**2/P12**2)*
     #           (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
          SUM = - BORN*(8*TK)*2*CA*(X/(1-X)+(1-X)/X+X*(1-X))
     #          + AZIDEP
          hvq_FGG = 1/(2*X*S)*SUM
       ELSE
       S2 = S+TK+UK
       Q1C=-S-TK-Q1Q
       Q2C=-S-UK-Q2Q
       W1 =Q2Q-Q1Q-TK
       W2 =Q1Q-Q2Q-UK
C
       P12 = (S2 - 2*XM2)/2
       P13 = W1/2
       P14 = Q1Q/2
       P15 = Q2C/2
       P23 = W2/2
       P24 = Q1C/2
       P25 = Q2Q/2
       P34 = TK/2
       P35 = UK/2
       P45 = S/2
      ANS = hvq_FBB1(XM2,P12,P13,P14,P15,P23,P24,P25,P34,P35,P45)
      ANS = 4*TK*UK*ANS/(2*S*4*64)
      hvq_FGG = ANS
      ENDIF
      RETURN
      END


       FUNCTION hvq_FQQ(S,X,Y,XM2,Q1Q,Q2Q,W1H,W2H,CTH2)
c
c      q(p1) + q_barra(p2) --> Q(k1) + Q_barra(k2) + g(k)
c
c      d sigma_qq (f) = N g^6 / (64 pi^2 s) beta_x d costh1 d th2 dy dx
c                       1/(1-x)_rho ( 1/(1-y)_+ + 1/(1+y)_+ ) hvq_FQQ
c
c           N = 1 / (4 pi)^2
c
       IMPLICIT REAL* 8(A-H,O-Z)
       REAL* 8 N
       TINY = 0
       V = 8
       N = 3
       N2 = N*N
       CF = 4.D0/3.D0
       CA = 3
       TK = - (1-X)*(1-Y)*S/2
       UK = - (1-X)*(1+Y)*S/2
       IF(1-X.LT.TINY)THEN
          Q2C=-S-Q2Q
          P13 = -Q1Q/2
          P23 = -Q2C/2
          P12 = S/2
          BORN = V/(2*N2)*( (P13**2+P23**2)/P12**2 + XM2/P12 )
C
C         Fattori iconali moltiplicati per 4*tk*uk
C         P1.K = -TK/2
C         P2.K = -UK/2
C         P3.K = W1/2
C         P4.K = W2/2
C
          P14 = P23
          P24 = P13
C
          E13 = 16*(1+Y)*P13/W1H
          E14 = 16*(1+Y)*P14/W2H
          E23 = 16*(1-Y)*P23/W1H
          E24 = 16*(1-Y)*P24/W2H
          E12 = 16*P12
          E33 = 16*(1-Y)*(1+Y)* XM2/W1H**2
          E44 = 16*(1-Y)*(1+Y)* XM2/W2H**2
          E34 = 16*(1-Y)*(1+Y)* (S/2-XM2)/(W1H*W2H)
          SUM = BORN*( CF*(2*E13+2*E24-E33-E44)
     #        +           (2*E14+2*E23-E13-E24-E12-E34)/N )
          hvq_FQQ = 1/(2*S)*SUM
       ELSEIF(1-Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = - X*Q1Q/2
          P23 = -Q2C/2
          P12 = S*X/2
          BORN = V/(2*N2)*( (P13**2+P23**2)/P12**2 + XM2/P12 )
          SUM = - BORN*(8*UK)*CF*(1+X**2)/(1-X)
          hvq_FQQ = 1/(2*X*S)*SUM
       ELSEIF(1+Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = -Q1Q/2
          P23 = -X*Q2C/2
          P12 = S*X/2
          BORN = V/(2*N2)*( (P13**2+P23**2)/P12**2 + XM2/P12 )
          SUM = - BORN*(8*TK)*CF*(1+X**2)/(1-X)
          hvq_FQQ = 1/(2*X*S)*SUM
       ELSE
       S2 = S+TK+UK
       Q1C=-S-TK-Q1Q
       Q2C=-S-UK-Q2Q
       W1 =Q2Q-Q1Q-TK
       W2 =Q1Q-Q2Q-UK
C
       P12 = S/2
       P13 = Q1Q/2
       P14 = Q1C/2
       P15 = TK/2
       P23 = Q2C/2
       P24 = Q2Q/2
       P25 = UK/2
       P34 = (S2-2*XM2)/2
       P35 = W1/2
       P45 = W2/2
      ans = hvq_Q4G1M(XM2,P34,P24,P14,P45,P23,P13,P35,P12,P25,P15)
      ANS = 4*TK*UK*ANS/(2*S*4*9)
      hvq_FQQ = ANS
      ENDIF
      RETURN
      END

       FUNCTION hvq_FQG(S,X,Y,XM2,Q1Q,Q2Q,W1H,W2H,CTH2)
c
c      q(p1) + g(p2) --> Q(k1) + Q_barra(k2) + g(k)
c
c      d sigma_qg (f) = N g^6 / (64 pi^2 s) beta_x d costh1 d th2 dy dx
c                       1/(1-x)_rho ( 1/(1-y)_+ + 1/(1+y)_+ ) hvq_FQG
c
c           N = 1 / (4 pi)^2
c
       IMPLICIT REAL* 8(A-H,O-Z)
       REAL* 8 N
       TINY = 0
       V = 8
       N = 3
       N2 = N*N
       TF = .5D0
       CF = 4.D0/3.D0
       CA = 3
       TK = - (1-X)*(1-Y)*S/2
       UK = - (1-X)*(1+Y)*S/2
       IF(1-X.LT.TINY)THEN
          hvq_FQG = 0
       ELSEIF(1-Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = - X*Q1Q/2
          P23 = -Q2C/2
          P12 = S*X/2
          BX = SQRT(1-4*XM2/(S*X))
          CTH1 = (P23-P13)/P12/BX
          AZIDEP =
     #    -512*BX**2*(CTH1-1)*(CTH1+1)*V*(2*V+BX**2*CTH1**2*N2-N2)
     #    *(X-1)**2*XM2/((BX*CTH1-1)**2*(BX*CTH1+1)**2*X**2)
          AZIDEP = AZIDEP * (2*CTH2**2-1)/2 /(4*64)
          AZIDEP = AZIDEP * CF / CA
          BORN = 1/(2*V*N)*(V/(P13*P23)-2*N**2/P12**2)*
     #           (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
          SUM = - BORN*(8*UK)*CF*(1+(1-X)**2)/X
     #    + AZIDEP
          hvq_FQG = 1/(2*X*S)*SUM
       ELSEIF(1+Y.LT.TINY)THEN
          Q2C = -S-UK-Q2Q
          P13 = -Q1Q/2
          P23 = -X*Q2C/2
          P12 = S*X/2
          BORN = V/(2*N2)*( (P13**2+P23**2)/P12**2 + XM2/P12 )
          SUM = - BORN*(8*TK)*TF*(X**2+(1-X)**2)
          hvq_FQG = 1/(2*X*S)*SUM
       ELSE
       S2 = S+TK+UK
       Q1C=-S-TK-Q1Q
       Q2C=-S-UK-Q2Q
       W1 =Q2Q-Q1Q-TK
       W2 =Q1Q-Q2Q-UK
C
       P12 = S/2
       P13 = Q1Q/2
       P14 = Q1C/2
       P15 = TK/2
       P23 = Q2C/2
       P24 = Q2Q/2
       P25 = UK/2
       P34 = (S2-2*XM2)/2
       P35 = W1/2
       P45 = W2/2
      ANS = hvq_Q4G1M(XM2,P34,P45,P14,P24,P35,P13,P23,P15,P25,P12)
      ANS = - 4*TK*UK*ANS/(2*S*4*3*8)
      hvq_FQG = ANS
      ENDIF
      RETURN
      END



c
c       This function subroutine, hvq_Q4G1M, calculates the
c       invariant matrix element squared for the hvq_process:
c
c       Q(-p1) + Qbar(-p2) --> q(p3) + qbar(p4) + g(p5)
C
c       summed over initial and final spins and colors including
c       masses for the incoming quark-antiquark pair.
c       The final state quark-antiquark pair are massless.
c       No averaging is performed for initial spins or colors.
c
c
c       1.      p1...p5 are the outgoing four momenta of the
c               partons, satisfying
c                       p1 + p2 + p3 + p4 + p5 = 0
c
c       2.      xm2 is the mass^2 of the heavy Quark and AntiQuark lines
c               with
c                       p1**2 = p2**2 = xm2
c               where
c                       pi**2 = pi(4)**2-pi(1)**2-pi(2)**2-pi(3)**2
c
c Abbiamo:
c             p12 = p1(4)*p2(4)-p1(1)*p2(1)-p1(2)*p2(2)-p1(3)*p2(3)
c             etc...
c
c Per il processo
c       Q(-p1) + Qbar(-p2) --> q(p3) + qbar(p4) + g(p5)
c chiamare
c       hvq_Q4G1M(XM2,P12,P13,P14,P15,P23,P24,P25,P34,P35,P45)*g**6/4/N**2
c
c Per
c       q(-p1) + qbar(-p2) --> Q(p3) + Qbar(p4) + g(p5)
c che equivarrebbe a
c       Q(-p4)+Qbar(-p3) --> q(p2)+qbar(p1)+g(p5)
c chiamare
c       hvq_Q4G1M(XM2,P34,P24,P14,P45,P23,P13,P35,P12,P25,P15)*g**6/4/N**2
c
c

      FUNCTION  hvq_Q4G1M(XM2,P12,P13,P14,P15,P23,P24,P25,P34,P35,P45)
      REAL*8  DL1, DL2, DL3, DL4
      REAL*8  P12, P13, P14, P15, P23, P24, P25, P34, P35, P45
      REAL*8  hvq_Q4G1M, XM2, RES, S, XN, XV
      PARAMETER         (XN = 3.D0)

      XV  = XN**2 - 1
      S = 2*(P12+XM2)

      DL1 = P13/P25-2*P35/S
      DL2 = P14/P25-2*P45/S
      DL3 = P23/P15-2*P35/S
      DL4 = P24/P15-2*P45/S


      RES =
     & +(P13**2+P23**2+P14**2+P24**2+XM2*(P12+P34+XM2))/2/S/P34
     & *(4*XV**2/XN*(P13/P15/P35+P24/P25/P45)
     & +4*XV/XN*(2*P14/P15/P45+2*P23/P25/P35
     & -P13/P15/P35-P24/P25/P45-P12/P15/P25-P34/P35/P45) )

      RES = RES
     & -XV*(XN**2-4)/XN
     & *2*XM2/S/P34*((P13-P14)/P25-(P23-P24)/P15)
     & +4*XV**2/XN*XM2*( (P35**2+P45**2)/P35/P45/S**2
     & -0.5*(1/P15+1/P25+1/P35+1/P45)/S
     & -0.25*(1/P15+1/P25+XM2/P15**2+XM2/P25**2+4/S)/P34
     & -(DL1**2+DL2**2+DL3**2+DL4**2)/4/P34**2)
     & -2*XV/XN
     & *XM2/S/P34*(1+2*P34/S+XM2/P15+XM2/P25+(P35**2+P45**2)/P15/P25
     & +(P13-P14)*(DL1-DL2)/P34+(P23-P24)*(DL3-DL4)/P34 )

      hvq_Q4G1M = RES


      RETURN
      END



      FUNCTION hvq_FBB1(XM2,P12,P13,P14,P15,P23,P24,P25,P34,P35,P45)
      IMPLICIT REAL * 8 (A-Z)
      n = 3
      n2 = n**2
      v = n2-1
      s = 2*xm2+2*p12
      ans = 0
      tmp0 = v**3*xm2*((xm2**2+4*p24*xm2-2*p24*p25)/(p13**2*p24**2)+(p34
     1   *s+2*p15*p25-p14*p24)/(p13*p14*p23*p24))/n2/2+v*xm2*(xm2**2/(
     2   p13*p14*p23*p25)-(5*p13*s+6*p13*p25+8*p13*p23-4*p13**2)/(p13*p1
     3   4*p23*p24)/4)/n2
      tmp0 = v**2*xm2*(-4*xm2**2/(p13*p14*p25**2)+2*s*xm2/(p13*p14*p23*p
     1   25)+(-(p23+p13)*p45+p23**2+p13**2)/(p13*p14*p23*p25))/n2/4+((
     2   -p13*s+p15*p25+p23**2+p15**2+p13**2)/(p13*p15*p24*p25*p34)+(p24
     3   +p15)/(p13*p14*p23*p25))*v*xm2**2-v*((2*(p45+p35)*xm2+p25**2+p1
     4   5**2)/(p14*p23*p34)/2+p13*(2*(p35+p34)*xm2+p23**2+p13**2)/(p1
     5   4*p15*p25*p34))+(n2+1)*p12*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(n
     6   2*p13*p14*p23*p24)/4+v**2*xm2*((p45+p34)*xm2/(p13*p25*p34*p45
     7   )+2*xm2/(p15**2*p34)+(p13/p25-p23/p15)**2/p34**2)+tmp0
      ans = -2*n2*(s**2/4+p45**2+p34**2)*v*xm2**2/(p13*p25*p34*p45*s)+
     1   n2*p14*p24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p25*p34*p45*s
     2   )+2*n2*p23*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(p25*p34*p45*s)-(n
     3   2+1)*v*xm2*(4*p12*xm2+s**2+2*p23*p24+2*p13*p14)/(n2*p13*p14*p23
     4   *p24)/8+n2*v*((p34-2*xm2)/(p13*p24*s)-s/(p15*p25*p34)/4)*xm
     5   2+v*((p12-3*xm2)/(p15*p25*p34)+(5*p13*p25+3*p13*p23-2*p13**2)/(
     6   p13*p14*p23*p24))*xm2+2*n2*(-s*(s+2*p34)/8+4*p15*p25+p14*p24+
     7   p13*p23)*v*xm2/(p15*p25*p34*s)+4*n2*(2*p23**2/(p15*p34**2*s)+(p
     8   45**2+p35**2+p34**2)/(p34**2*s**2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p23*xm2-2*p23*p25)/(p14**2*p23**2)+(p34
     1   *s+2*p15*p25-p13*p23)/(p13*p14*p23*p24))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p14*p24*p25)-(5*p14*s+6*p14*p25+8*p14*p2
     1   4-4*p14**2)/(p13*p14*p23*p24)/4)/n2+v**2*xm2*(-4*xm2**2/(p13*
     2   p14*p25**2)+2*s*xm2/(p13*p14*p24*p25)+(-(p24+p14)*p35+p24**2+p1
     3   4**2)/(p13*p14*p24*p25))/n2/4+((-p14*s+p15*p25+p24**2+p15**2+
     4   p14**2)/(p14*p15*p23*p25*p34)+(p23+p15)/(p13*p14*p24*p25))*v*xm
     5   2**2-v*((2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p24*p34)/2+p14*(
     6   2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p15*p25*p34))+(n2+1)*p12*v*
     7   (2*(p45+p35)*xm2+p25**2+p15**2)/(n2*p13*p14*p23*p24)/4+2*n2*p
     8   24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p25*p34*p35*s)+tmp0
      ans = -2*n2*(s**2/4+p35**2+p34**2)*v*xm2**2/(p14*p25*p34*p35*s)+
     1   v**2*xm2*((p35+p34)*xm2/(p14*p25*p34*p35)+2*xm2/(p15**2*p34)+(p
     2   14/p25-p24/p15)**2/p34**2)+n2*p13*p23*v*(2*(p35+p34)*xm2+p23**2
     3   +p13**2)/(p14*p25*p34*p35*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23
     4   *p24+2*p13*p14)/(n2*p13*p14*p23*p24)/8+n2*v*((p34-2*xm2)/(p14
     5   *p23*s)-s/(p15*p25*p34)/4)*xm2+v*((p12-3*xm2)/(p15*p25*p34)+(
     6   5*p14*p25+3*p14*p24-2*p14**2)/(p13*p14*p23*p24))*xm2+2*n2*(-s*(
     7   s+2*p34)/8+4*p15*p25+p14*p24+p13*p23)*v*xm2/(p15*p25*p34*s)+4
     8   *n2*(2*p24**2/(p15*p34**2*s)+(p45**2+p35**2+p34**2)/(p34**2*s**
     9   2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p25*xm2-2*p23*p25)/(p14**2*p25**2)+(p45
     1   *s-p15*p25+2*p13*p23)/(p14*p15*p24*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p14*p15*p23*p24)-(5*p14*s+8*p14*p24+6*p14*p2
     1   3-4*p14**2)/(p14*p15*p24*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p14*
     2   p15*p23**2)+2*s*xm2/(p14*p15*p23*p24)+(-(p24+p14)*p35+p24**2+p1
     3   4**2)/(p14*p15*p23*p24))/n2/4-2*n2*(s**2/4+p45**2+p35**2)*v
     4   *xm2**2/(p14*p23*p35*p45*s)+((-p14*s+p24**2+p13*p23+p14**2+p13*
     5   *2)/(p13*p14*p23*p25*p45)+(p25+p13)/(p14*p15*p23*p24))*v*xm2**2
     6   -v*(p14*(2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p15*p23*p45)+(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p15*p24*p45)/2)+v**2*xm2*((p45+p3
     8   5)*xm2/(p14*p23*p35*p45)+2*xm2/(p13**2*p45)+(p14/p23-p24/p13)**
     9   2/p45**2)+n2*p15*p25*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(p14*p23
     :   *p35*p45*s)+tmp0
      ans = 2*n2*p24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p23*p35*p45*s)+(
     1   n2+1)*p12*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(n2*p14*p15*p24*p25
     2   )/4-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p24*p25+2*p14*p15)/(n2*p14
     3   *p15*p24*p25)/8+n2*v*((p45-2*xm2)/(p14*p25*s)-s/(p13*p23*p45)
     4   /4)*xm2+v*((p12-3*xm2)/(p13*p23*p45)+(3*p14*p24+5*p14*p23-2*p
     5   14**2)/(p14*p15*p24*p25))*xm2+2*n2*(-s*(s+2*p45)/8+p15*p25+p1
     6   4*p24+4*p13*p23)*v*xm2/(p13*p23*p45*s)+4*n2*(2*p24**2/(p13*p45*
     7   *2*s)+(p45**2+p35**2+p34**2)/(p45**2*s**2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p24*xm2-2*p23*p24)/(p15**2*p24**2)+(p45
     1   *s-p14*p24+2*p13*p23)/(p14*p15*p24*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p14*p15*p23*p25)-(5*p15*s+8*p15*p25+6*p15*p2
     1   3-4*p15**2)/(p14*p15*p24*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p14*
     2   p15*p23**2)+2*s*xm2/(p14*p15*p23*p25)+(-(p25+p15)*p34+p25**2+p1
     3   5**2)/(p14*p15*p23*p25))/n2/4-2*n2*(s**2/4+p45**2+p34**2)*v
     4   *xm2**2/(p15*p23*p34*p45*s)+((-p15*s+p25**2+p13*p23+p15**2+p13*
     5   *2)/(p13*p15*p23*p24*p45)+(p24+p13)/(p14*p15*p23*p25))*v*xm2**2
     6   -v*(p15*(2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p14*p23*p45)+(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p14*p25*p45)/2)+2*n2*p25*v*(2*(p4
     8   5+p35)*xm2+p25**2+p15**2)/(p23*p34*p45*s)+v**2*xm2*((p45+p34)*x
     9   m2/(p15*p23*p34*p45)+2*xm2/(p13**2*p45)+(p15/p23-p25/p13)**2/p4
     :   5**2)+tmp0
      ans = n2*p14*p24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p23*p34*p4
     1   5*s)+(n2+1)*p12*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(n2*p14*p15*p
     2   24*p25)/4-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p24*p25+2*p14*p15)/(
     3   n2*p14*p15*p24*p25)/8+n2*v*((p45-2*xm2)/(p15*p24*s)-s/(p13*p2
     4   3*p45)/4)*xm2+v*((p12-3*xm2)/(p13*p23*p45)+(3*p15*p25+5*p15*p
     5   23-2*p15**2)/(p14*p15*p24*p25))*xm2+2*n2*(-s*(s+2*p45)/8+p15*
     6   p25+p14*p24+4*p13*p23)*v*xm2/(p13*p23*p45*s)+4*n2*(2*p25**2/(p1
     7   3*p45**2*s)+(p45**2+p35**2+p34**2)/(p45**2*s**2))*v*xm2+tmp0+an
     8   s
      tmp0 = v**3*xm2*((xm2**2+4*p23*xm2-2*p23*p24)/(p15**2*p23**2)+(p35
     1   *s+2*p14*p24-p13*p23)/(p13*p15*p23*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p15*p24*p25)-(5*p15*s+8*p15*p25+6*p15*p2
     1   4-4*p15**2)/(p13*p15*p23*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p13*
     2   p15*p24**2)+2*s*xm2/(p13*p15*p24*p25)+(-(p25+p15)*p34+p25**2+p1
     3   5**2)/(p13*p15*p24*p25))/n2/4+((-p15*s+p25**2+p14*p24+p15**2+
     4   p14**2)/(p14*p15*p23*p24*p35)+(p23+p14)/(p13*p15*p24*p25))*v*xm
     5   2**2-v*(p15*(2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p14*p24*p35)+(
     6   2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p25*p35)/2)+2*n2*p25*v*(2
     7   *(p45+p35)*xm2+p25**2+p15**2)/(p24*p34*p35*s)+(n2+1)*p12*v*(2*(
     8   p45+p34)*xm2+p24**2+p14**2)/(n2*p13*p15*p23*p25)/4+tmp0
      ans = -2*n2*(s**2/4+p35**2+p34**2)*v*xm2**2/(p15*p24*p34*p35*s)+
     1   v**2*xm2*((p35+p34)*xm2/(p15*p24*p34*p35)+2*xm2/(p14**2*p35)+(p
     2   15/p24-p25/p14)**2/p35**2)+n2*p13*p23*v*(2*(p35+p34)*xm2+p23**2
     3   +p13**2)/(p15*p24*p34*p35*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23
     4   *p25+2*p13*p15)/(n2*p13*p15*p23*p25)/8+n2*v*((p35-2*xm2)/(p15
     5   *p23*s)-s/(p14*p24*p35)/4)*xm2+v*((p12-3*xm2)/(p14*p24*p35)+(
     6   3*p15*p25+5*p15*p24-2*p15**2)/(p13*p15*p23*p25))*xm2+2*n2*(-s*(
     7   s+2*p35)/8+p15*p25+4*p14*p24+p13*p23)*v*xm2/(p14*p24*p35*s)+4
     8   *n2*(2*p25**2/(p14*p35**2*s)+(p45**2+p35**2+p34**2)/(p35**2*s**
     9   2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p25*xm2-2*p24*p25)/(p13**2*p25**2)+(p35
     1   *s-p15*p25+2*p14*p24)/(p13*p15*p23*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p15*p23*p24)-(5*p13*s+6*p13*p24+8*p13*p2
     1   3-4*p13**2)/(p13*p15*p23*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p13*
     2   p15*p24**2)+2*s*xm2/(p13*p15*p23*p24)+(-(p23+p13)*p45+p23**2+p1
     3   3**2)/(p13*p15*p23*p24))/n2/4-2*n2*(s**2/4+p45**2+p35**2)*v
     4   *xm2**2/(p13*p24*p35*p45*s)+((-p13*s+p14*p24+p23**2+p14**2+p13*
     5   *2)/(p13*p14*p24*p25*p35)+(p25+p14)/(p13*p15*p23*p24))*v*xm2**2
     6   -v*((2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p23*p35)/2+p13*(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p14*p15*p24*p35))+v**2*xm2*((p45+p3
     8   5)*xm2/(p13*p24*p35*p45)+2*xm2/(p14**2*p35)+(p13/p24-p23/p14)**
     9   2/p35**2)+n2*p15*p25*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p24
     :   *p35*p45*s)+tmp0
      ans = (n2+1)*p12*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(n2*p13*p15*p23
     1   *p25)/4+2*n2*p23*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(p24*p35*p
     2   45*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23*p25+2*p13*p15)/(n2*p13
     3   *p15*p23*p25)/8+n2*v*((p35-2*xm2)/(p13*p25*s)-s/(p14*p24*p35)
     4   /4)*xm2+v*((p12-3*xm2)/(p14*p24*p35)+(5*p13*p24+3*p13*p23-2*p
     5   13**2)/(p13*p15*p23*p25))*xm2+2*n2*(-s*(s+2*p35)/8+p15*p25+4*
     6   p14*p24+p13*p23)*v*xm2/(p14*p24*p35*s)+4*n2*(2*p23**2/(p14*p35*
     7   *2*s)+(p45**2+p35**2+p34**2)/(p35**2*s**2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p14*xm2-2*p14*p15)/(p14**2*p23**2)+(p34
     1   *s+2*p15*p25-p14*p24)/(p13*p14*p23*p24))/n2/2+v*xm2*(xm2**2/(
     2   p13*p15*p23*p24)-(5*p23*s-4*p23**2+6*p15*p23+8*p13*p23)/(p13*p1
     3   4*p23*p24)/4)/n2
      tmp0 = v**2*xm2*(-4*xm2**2/(p15**2*p23*p24)+2*s*xm2/(p13*p15*p23*p
     1   24)+(-(p23+p13)*p45+p23**2+p13**2)/(p13*p15*p23*p24))/n2/4+((
     2   -p23*s+p25**2+p15*p25+p23**2+p13**2)/(p14*p15*p23*p25*p34)+(p25
     3   +p14)/(p13*p15*p23*p24))*v*xm2**2-v*((2*(p45+p35)*xm2+p25**2+p1
     4   5**2)/(p13*p24*p34)/2+p23*(2*(p35+p34)*xm2+p23**2+p13**2)/(p1
     5   5*p24*p25*p34))+(n2+1)*p12*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(n
     6   2*p13*p14*p23*p24)/4+v**2*xm2*((p45+p34)*xm2/(p15*p23*p34*p45
     7   )+2*xm2/(p25**2*p34)+(p23/p15-p13/p25)**2/p34**2)+tmp0
      ans = -2*n2*(s**2/4+p45**2+p34**2)*v*xm2**2/(p15*p23*p34*p45*s)+
     1   n2*p14*p24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p23*p34*p45*s
     2   )+2*n2*p13*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(p15*p34*p45*s)-(n
     3   2+1)*v*xm2*(4*p12*xm2+s**2+2*p23*p24+2*p13*p14)/(n2*p13*p14*p23
     4   *p24)/8+n2*v*((p34-2*xm2)/(p14*p23*s)-s/(p15*p25*p34)/4)*xm
     5   2+v*((p12-3*xm2)/(p15*p25*p34)+(-2*p23**2+5*p15*p23+3*p13*p23)/
     6   (p13*p14*p23*p24))*xm2+2*n2*(-s*(s+2*p34)/8+4*p15*p25+p14*p24
     7   +p13*p23)*v*xm2/(p15*p25*p34*s)+4*n2*(2*p13**2/(p25*p34**2*s)+(
     8   p45**2+p35**2+p34**2)/(p34**2*s**2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p13*xm2-2*p13*p15)/(p13**2*p24**2)+(p34
     1   *s+2*p15*p25-p13*p23)/(p13*p14*p23*p24))/n2/2
      tmp0 = v*xm2*(xm2**2/(p14*p15*p23*p24)-(5*p24*s-4*p24**2+6*p15*p24
     1   +8*p14*p24)/(p13*p14*p23*p24)/4)/n2+v**2*xm2*(-4*xm2**2/(p15*
     2   *2*p23*p24)+2*s*xm2/(p14*p15*p23*p24)+(-(p24+p14)*p35+p24**2+p1
     3   4**2)/(p14*p15*p23*p24))/n2/4+((-p24*s+p25**2+p15*p25+p24**2+
     4   p14**2)/(p13*p15*p24*p25*p34)+(p25+p13)/(p14*p15*p23*p24))*v*xm
     5   2**2-v*((2*(p45+p35)*xm2+p25**2+p15**2)/(p14*p23*p34)/2+p24*(
     6   2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p23*p25*p34))+(n2+1)*p12*v*
     7   (2*(p45+p35)*xm2+p25**2+p15**2)/(n2*p13*p14*p23*p24)/4+2*n2*p
     8   14*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p34*p35*s)+tmp0
      ans = -2*n2*(s**2/4+p35**2+p34**2)*v*xm2**2/(p15*p24*p34*p35*s)+
     1   v**2*xm2*((p35+p34)*xm2/(p15*p24*p34*p35)+2*xm2/(p25**2*p34)+(p
     2   24/p15-p14/p25)**2/p34**2)+n2*p13*p23*v*(2*(p35+p34)*xm2+p23**2
     3   +p13**2)/(p15*p24*p34*p35*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23
     4   *p24+2*p13*p14)/(n2*p13*p14*p23*p24)/8+n2*v*((p34-2*xm2)/(p13
     5   *p24*s)-s/(p15*p25*p34)/4)*xm2+v*((p12-3*xm2)/(p15*p25*p34)+(
     6   -2*p24**2+5*p15*p24+3*p14*p24)/(p13*p14*p23*p24))*xm2+2*n2*(-s*
     7   (s+2*p34)/8+4*p15*p25+p14*p24+p13*p23)*v*xm2/(p15*p25*p34*s)+
     8   4*n2*(2*p14**2/(p25*p34**2*s)+(p45**2+p35**2+p34**2)/(p34**2*s*
     9   *2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p15*xm2-2*p13*p15)/(p15**2*p24**2)+(p45
     1   *s-p15*p25+2*p13*p23)/(p14*p15*p24*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p14*p24*p25)-(5*p24*s-4*p24**2+8*p14*p24
     1   +6*p13*p24)/(p14*p15*p24*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p13*
     2   *2*p24*p25)+2*s*xm2/(p13*p14*p24*p25)+(-(p24+p14)*p35+p24**2+p1
     3   4**2)/(p13*p14*p24*p25))/n2/4-2*n2*(s**2/4+p45**2+p35**2)*v
     4   *xm2**2/(p13*p24*p35*p45*s)+((-p24*s+p24**2+p23**2+p13*p23+p14*
     5   *2)/(p13*p15*p23*p24*p45)+(p23+p15)/(p13*p14*p24*p25))*v*xm2**2
     6   -v*(p24*(2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p23*p25*p45)+(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p14*p25*p45)/2)+v**2*xm2*((p45+p3
     8   5)*xm2/(p13*p24*p35*p45)+2*xm2/(p23**2*p45)+(p24/p13-p14/p23)**
     9   2/p45**2)+n2*p15*p25*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p24
     :   *p35*p45*s)+tmp0
      ans = 2*n2*p14*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p35*p45*s)+(
     1   n2+1)*p12*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(n2*p14*p15*p24*p25
     2   )/4-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p24*p25+2*p14*p15)/(n2*p14
     3   *p15*p24*p25)/8+n2*v*((p45-2*xm2)/(p15*p24*s)-s/(p13*p23*p45)
     4   /4)*xm2+v*((p12-3*xm2)/(p13*p23*p45)+(-2*p24**2+3*p14*p24+5*p
     5   13*p24)/(p14*p15*p24*p25))*xm2+2*n2*(-s*(s+2*p45)/8+p15*p25+p
     6   14*p24+4*p13*p23)*v*xm2/(p13*p23*p45*s)+4*n2*(2*p14**2/(p23*p45
     7   **2*s)+(p45**2+p35**2+p34**2)/(p45**2*s**2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p14*xm2-2*p13*p14)/(p14**2*p25**2)+(p45
     1   *s-p14*p24+2*p13*p23)/(p14*p15*p24*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p15*p24*p25)-(5*p25*s-4*p25**2+8*p15*p25
     1   +6*p13*p25)/(p14*p15*p24*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p13*
     2   *2*p24*p25)+2*s*xm2/(p13*p15*p24*p25)+(-(p25+p15)*p34+p25**2+p1
     3   5**2)/(p13*p15*p24*p25))/n2/4-2*n2*(s**2/4+p45**2+p34**2)*v
     4   *xm2**2/(p13*p25*p34*p45*s)+((-p25*s+p25**2+p23**2+p13*p23+p15*
     5   *2)/(p13*p14*p23*p25*p45)+(p23+p14)/(p13*p15*p24*p25))*v*xm2**2
     6   -v*(p25*(2*(p45+p35)*xm2+p25**2+p15**2)/(p13*p23*p24*p45)+(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p15*p24*p45)/2)+2*n2*p15*v*(2*(p4
     8   5+p35)*xm2+p25**2+p15**2)/(p13*p34*p45*s)+v**2*xm2*((p45+p34)*x
     9   m2/(p13*p25*p34*p45)+2*xm2/(p23**2*p45)+(p25/p13-p15/p23)**2/p4
     :   5**2)+tmp0
      ans = n2*p14*p24*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p25*p34*p4
     1   5*s)+(n2+1)*p12*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(n2*p14*p15*p
     2   24*p25)/4-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p24*p25+2*p14*p15)/(
     3   n2*p14*p15*p24*p25)/8+n2*v*((p45-2*xm2)/(p14*p25*s)-s/(p13*p2
     4   3*p45)/4)*xm2+v*((p12-3*xm2)/(p13*p23*p45)+(-2*p25**2+3*p15*p
     5   25+5*p13*p25)/(p14*p15*p24*p25))*xm2+2*n2*(-s*(s+2*p45)/8+p15
     6   *p25+p14*p24+4*p13*p23)*v*xm2/(p13*p23*p45*s)+4*n2*(2*p15**2/(p
     7   23*p45**2*s)+(p45**2+p35**2+p34**2)/(p45**2*s**2))*v*xm2+tmp0+a
     8   ns
      tmp0 = v**3*xm2*((xm2**2+4*p13*xm2-2*p13*p14)/(p13**2*p25**2)+(p35
     1   *s+2*p14*p24-p13*p23)/(p13*p15*p23*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p14*p15*p23*p25)-(5*p25*s-4*p25**2+8*p15*p25
     1   +6*p14*p25)/(p13*p15*p23*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p14*
     2   *2*p23*p25)+2*s*xm2/(p14*p15*p23*p25)+(-(p25+p15)*p34+p25**2+p1
     3   5**2)/(p14*p15*p23*p25))/n2/4+((-p25*s+p25**2+p24**2+p14*p24+
     4   p15**2)/(p13*p14*p24*p25*p35)+(p24+p13)/(p14*p15*p23*p25))*v*xm
     5   2**2-v*(p25*(2*(p45+p35)*xm2+p25**2+p15**2)/(p14*p23*p24*p35)+(
     6   2*(p45+p34)*xm2+p24**2+p14**2)/(p15*p23*p35)/2)+2*n2*p15*v*(2
     7   *(p45+p35)*xm2+p25**2+p15**2)/(p14*p34*p35*s)+(n2+1)*p12*v*(2*(
     8   p45+p34)*xm2+p24**2+p14**2)/(n2*p13*p15*p23*p25)/4+tmp0
      ans = -2*n2*(s**2/4+p35**2+p34**2)*v*xm2**2/(p14*p25*p34*p35*s)+
     1   v**2*xm2*((p35+p34)*xm2/(p14*p25*p34*p35)+2*xm2/(p24**2*p35)+(p
     2   25/p14-p15/p24)**2/p35**2)+n2*p13*p23*v*(2*(p35+p34)*xm2+p23**2
     3   +p13**2)/(p14*p25*p34*p35*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23
     4   *p25+2*p13*p15)/(n2*p13*p15*p23*p25)/8+n2*v*((p35-2*xm2)/(p13
     5   *p25*s)-s/(p14*p24*p35)/4)*xm2+v*((p12-3*xm2)/(p14*p24*p35)+(
     6   -2*p25**2+3*p15*p25+5*p14*p25)/(p13*p15*p23*p25))*xm2+2*n2*(-s*
     7   (s+2*p35)/8+p15*p25+4*p14*p24+p13*p23)*v*xm2/(p14*p24*p35*s)+
     8   4*n2*(2*p15**2/(p24*p35**2*s)+(p45**2+p35**2+p34**2)/(p35**2*s*
     9   *2))*v*xm2+tmp0+ans
      tmp0 = v**3*xm2*((xm2**2+4*p15*xm2-2*p14*p15)/(p15**2*p23**2)+(p35
     1   *s-p15*p25+2*p14*p24)/(p13*p15*p23*p25))/n2/2
      tmp0 = v*xm2*(xm2**2/(p13*p14*p23*p25)-(5*p23*s-4*p23**2+6*p14*p23
     1   +8*p13*p23)/(p13*p15*p23*p25)/4)/n2+v**2*xm2*(-4*xm2**2/(p14*
     2   *2*p23*p25)+2*s*xm2/(p13*p14*p23*p25)+(-(p23+p13)*p45+p23**2+p1
     3   3**2)/(p13*p14*p23*p25))/n2/4-2*n2*(s**2/4+p45**2+p35**2)*v
     4   *xm2**2/(p14*p23*p35*p45*s)+((-p23*s+p24**2+p14*p24+p23**2+p13*
     5   *2)/(p14*p15*p23*p24*p35)+(p24+p15)/(p13*p14*p23*p25))*v*xm2**2
     6   -v*((2*(p45+p34)*xm2+p24**2+p14**2)/(p13*p25*p35)/2+p23*(2*(p
     7   35+p34)*xm2+p23**2+p13**2)/(p14*p24*p25*p35))+v**2*xm2*((p45+p3
     8   5)*xm2/(p14*p23*p35*p45)+2*xm2/(p24**2*p35)+(p23/p14-p13/p24)**
     9   2/p35**2)+n2*p15*p25*v*(2*(p45+p35)*xm2+p25**2+p15**2)/(p14*p23
     :   *p35*p45*s)+tmp0
      ans = (n2+1)*p12*v*(2*(p45+p34)*xm2+p24**2+p14**2)/(n2*p13*p15*p23
     1   *p25)/4+2*n2*p13*v*(2*(p35+p34)*xm2+p23**2+p13**2)/(p14*p35*p
     2   45*s)-(n2+1)*v*xm2*(4*p12*xm2+s**2+2*p23*p25+2*p13*p15)/(n2*p13
     3   *p15*p23*p25)/8+n2*v*((p35-2*xm2)/(p15*p23*s)-s/(p14*p24*p35)
     4   /4)*xm2+v*((p12-3*xm2)/(p14*p24*p35)+(-2*p23**2+5*p14*p23+3*p
     5   13*p23)/(p13*p15*p23*p25))*xm2+2*n2*(-s*(s+2*p35)/8+p15*p25+4
     6   *p14*p24+p13*p23)*v*xm2/(p14*p24*p35*s)+4*n2*(2*p13**2/(p24*p35
     7   **2*s)+(p45**2+p35**2+p34**2)/(p35**2*s**2))*v*xm2+tmp0+ans
      hvq_FBB1 = ans
      return
      end


