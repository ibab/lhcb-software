      subroutine dijet_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_math.h'

      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2,amp2mad,tmp
      integer nleg
      parameter (nleg=nlegreal)
      integer mu,ileg

      real *8 kr_mad(0:3,nleg)


      do ileg=1,5
         do mu=0,3
            kr_mad(mu,ileg)=p(mu,ileg)
         enddo
      enddo
c     restore exact masslessness of  incoming partons 
      kr_mad(0,1)=dabs(kr_mad(3,1))
      kr_mad(0,2)=dabs(kr_mad(3,2))

      call dijet_ALT_COMPREAL(kr_mad,rflav,amp2mad)

      amp2=amp2mad/(st_alpha/2./pi)

      return
      end



      SUBROUTINE dijet_ALT_COMPREAL(P,FLAV,AMP2)

      IMPLICIT NONE
C -   *************

      REAL*8 P(0:3,1:5)
      INTEGER FLAV(5)
      REAL*8 AMP2
      CHARACTER*1 KS_LABEL
      INTEGER KS_MAP(5),V(5),VV(5)
      INTEGER I,ITMP
      REAL*8  AVG,SYM

      DO I=1,5
         IF(I.GT.2) THEN
            V(I)=-FLAV(I)
         ELSE
            V(I)= FLAV(I)
         ENDIF
c Set gluon smaller than any flavour
         IF(V(I).EQ.0) V(I)=-21
      ENDDO
c Find permutation to put them in descending order:
c gluons last, flavours in decreasing order
c CERNLIB routine:
      CALL dijet_SORTZV(V,KS_MAP,5,-1,1,0)
      DO I=1,5
         VV(I)=V(KS_MAP(I))
      ENDDO
c Classify processes
      IF(VV(1).EQ.-21) THEN
         KS_LABEL='D'
      ELSEIF(VV(3).EQ.-21) THEN
         KS_LABEL='C'
      ELSEIF(VV(1).EQ.VV(2)) THEN
         KS_LABEL='B'
      ELSE
         KS_label='A'
c Kunszt Soper have last two particles exchanged in this case (i.e. q Q ->
c -q -Q rather than q Q -> -Q -q (if q>Q) )
         ITMP=VV(3)
         VV(3)=VV(4)
         VV(4)=ITMP
         ITMP=KS_MAP(3)
         KS_MAP(3)=KS_MAP(4)
         KS_MAP(4)=ITMP
      ENDIF

      AVG=1.0d0
      DO I=1,2
         IF(FLAV(I).NE.0) THEN
c - Sign for crossing fermion line:
            AVG = AVG*-6.
         ELSE
            AVG = AVG*16.
         ENDIF
      ENDDO
      IF((FLAV(3).EQ.FLAV(4)).AND.(FLAV(3).EQ.FLAV(5))) THEN
         SYM=6.0D0
      ELSEIF((FLAV(3).EQ.FLAV(4)).AND.(FLAV(3).NE.FLAV(5))) THEN
         SYM=2.0D0
      ELSEIF((FLAV(3).EQ.FLAV(5)).AND.(FLAV(3).NE.FLAV(4))) THEN
         SYM=2.0D0
      ELSEIF((FLAV(4).EQ.FLAV(5)).AND.(FLAV(3).NE.FLAV(4))) THEN
         SYM=2.0D0
      ELSE
         SYM=1.0D0
      ENDIF

c - KH: don't understand why I find I need this extra factor two 
      IF(KS_LABEL.EQ.'C') SYM=SYM/2.0D0

      DO I=1,5
         IF(KS_MAP(I).GT.2) KS_MAP(I)=-KS_MAP(I)
      ENDDO
      IF(KS_LABEL.EQ.'A') THEN
         CALL dijet_ES_A(P,AMP2,KS_MAP,AVG,SYM)
      ELSEIF(ks_label.EQ.'B') THEN
         CALL dijet_ES_B(P,AMP2,KS_MAP,AVG,SYM)
      ELSEIF(ks_label.EQ.'C') THEN
         CALL dijet_ES_C(P,AMP2,KS_MAP,AVG,SYM)
      ELSEIF(ks_label.EQ.'D') THEN
         CALL dijet_ES_D(P,AMP2,KS_MAP,AVG,SYM)
      ELSE
         WRITE(*,*) 'compreal: fatal error.',FLAV
         WRITE(*,*) 'Failed to determine real emission proc. Quitting.'
         CALL EXIT(1)
      ENDIF

      RETURN
      END



C ----------------------------------------------------------------- C
      SUBROUTINE dijet_ES_D(P1,RESULT,ES_MAP,AVG,SYM)
C - The squared, spin and colour summed matrix element for gg->ggg
C - , averaged over initial state colours and spins. The final state
C - symmetry factor 1/3! is included at the end. This derives from
C - formulae in Ellis & Sexton, NPB269:445,1986. 
C ----------------------------------------------------------------- C

      IMPLICIT NONE
C -   *************
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_math.h'
      REAL*8 P1(0:3,nlegreal),AVG,SYM
      REAL*8 FD_INVARIANT,FD_NONINVARIANT,dijet_FD_DENOM
      REAL*8 dijet_dotp
      EXTERNAL dijet_dotp
      REAL*8 S(5,5),RESULT
      INTEGER ES_MAP(5)
      INTEGER IXX,JXX,KXX,LXX,MXX
      REAL*8 GS6

C - Calculate the strong coupling constant prefactor
      GS6 = (4.*PI*ST_ALPHA)**3
C - First calculate the kinematic invariants once and for all (in
C - Ellis sextion conventions).
      DO IXX=1,5
         DO JXX=IXX,5 
            IF(IXX.EQ.JXX) THEN
               S(IXX,JXX) = 0.0
            ELSE
               S(IXX,JXX) = dijet_dotp(P1(0,ABS(ES_MAP(IXX))),
     $                           P1(0,ABS(ES_MAP(JXX))))
               IF((ES_MAP(IXX)*ES_MAP(JXX)).LT.0) S(IXX,JXX)=-S(IXX,JXX)
               S(JXX,IXX) = S(IXX,JXX)
            ENDIF
         ENDDO
      ENDDO
C - Then we calculate the Ellis Sexton D-function (Eq. 3.11) as a sum
C - over permutations of the arguments of the F^D function (Eq. 3.12).
C - We calculate the FD function as a product of two parts to save time:
C - a permuation invariant part FD_INVARIANT and a part which is not
C - invariant under permutations of the momenta FD_NONINVARIANT. The
C - latter is the denominator of the prefactor in Eq. 3.12 of the Ellis,
C - Sexton paper. 
      FD_INVARIANT = 4. * 8. * 3. * 3. * 3. 
     $     * 0.5 * ( S(1,2)**4 + S(1,3)**4 + S(1,4)**4 + S(1,5)**4
     $             + S(2,3)**4 + S(2,4)**4 + S(2,5)**4
     $             + S(3,4)**4 + S(3,5)**4
     $             + S(4,5)**4 )
C - Initialise non invariant part and loop over permutations 
      FD_NONINVARIANT = 1/(S(1,2)*S(1,3)*S(2,4)*S(3,5)*S(4,5))
     $                + 1/(S(1,2)*S(1,3)*S(2,5)*S(3,4)*S(4,5))
     $                + 1/(S(1,2)*S(1,4)*S(2,3)*S(3,5)*S(4,5))
     $                + 1/(S(1,2)*S(1,4)*S(2,5)*S(3,4)*S(3,5))
     $                + 1/(S(1,2)*S(1,5)*S(2,3)*S(3,4)*S(4,5))
     $                + 1/(S(1,2)*S(1,5)*S(2,4)*S(3,4)*S(3,5))
     $                + 1/(S(1,3)*S(1,4)*S(2,3)*S(2,5)*S(4,5))
     $                + 1/(S(1,3)*S(1,4)*S(2,4)*S(2,5)*S(3,5))
     $                + 1/(S(1,3)*S(1,5)*S(2,3)*S(2,4)*S(4,5))
     $                + 1/(S(1,3)*S(1,5)*S(2,4)*S(2,5)*S(3,4))
     $                + 1/(S(1,4)*S(1,5)*S(2,3)*S(2,4)*S(3,5))
     $                + 1/(S(1,4)*S(1,5)*S(2,3)*S(2,5)*S(3,4))

      FD_NONINVARIANT = FD_NONINVARIANT*10.0
c      FD_NONINVARIANT = 0.
c      DO IXX=1,5
c         DO JXX=1,5
c            IF(JXX.EQ.IXX) CYCLE
c            DO KXX=1,5
c               IF((KXX.EQ.IXX).OR.
c     $            (KXX.EQ.JXX)) CYCLE
c               DO LXX=1,5
c                  IF((LXX.EQ.IXX).OR.
c     $               (LXX.EQ.JXX).OR.
c     $               (LXX.EQ.KXX)) CYCLE
c                  DO MXX=1,5
c                     IF((MXX.EQ.IXX).OR.
c     $                  (MXX.EQ.JXX).OR.
c     $                  (MXX.EQ.KXX).OR.
c     $                  (MXX.EQ.LXX)) CYCLE
c                     FD_NONINVARIANT = FD_NONINVARIANT
c     $                               + dijet_FD_DENOM(IXX,JXX,KXX,LXX,MXX,S)
c                  ENDDO
c               ENDDO
c            ENDDO
c         ENDDO
c      ENDDO
C - On the next line we obtain the D function (Eq.3.11).
      RESULT=GS6/10.*FD_INVARIANT*FD_NONINVARIANT
C - To get the matrix element squared we need to divide by the
C - spin colour averaging factor 4V^2 (see Table 7).
      RESULT=RESULT/AVG
C - We also need to include another symmetry factor of 3! to get
C - agreement with the MadGraph (and, hopefully, reality).
      RESULT=RESULT/SYM

      END
       
       
      FUNCTION dijet_FD_DENOM(I,J,K,L,M,S)
      IMPLICIT NONE
      REAL*8 dijet_FD_DENOM
      INTEGER I,J,K,L,M
      REAL*8 S(5,5)
      dijet_FD_DENOM = 1.0/(S(I,J)*S(J,K)*S(K,L)*S(L,M)*S(M,I))
      RETURN
      END
       


C ----------------------------------------------------------------- C
      SUBROUTINE dijet_ES_C(P1,RESULT,ES_MAP,AVG,SYM)
C - The squared, spin and colour summed matrix element for gu->ugg
C - , averaged over initial state colours and spins. This comes from
C - formulae in Ellis & Sexton, NPB269:445,1986. Checked agreement
C - with Madgraph matrix subroutine. One instance occurred prior to
C - "POWHEG:initialization" with fractional differences at the level
C - of 1/10^7 but after then in the grid search phase and all
C - integrations the fractional difference was better than 1*10^7.
C ----------------------------------------------------------------- C

      IMPLICIT NONE
C -   *************
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_math.h'
      REAL*8 P1(0:3,nlegreal),AVG,SYM
      REAL*8 dijet_dotp
      EXTERNAL dijet_dotp
      REAL*8 S(5,5),dijet_FC_FN,RESULT
      INTEGER ES_MAP(5)
      INTEGER IXX,JXX,LXX,MXX
      REAL*8 GS6
C - Calculate the strong coupling constant prefactor
      GS6 = (4.*PI*ST_ALPHA)**3
C - First calculate the kinematic invariants once and for all (in
C - Ellis sextion conventions).
      DO IXX=1,5
         DO JXX=IXX,5 
            IF(IXX.EQ.JXX) THEN
               S(IXX,JXX) = 0.0
            ELSE
               S(IXX,JXX) = dijet_dotp(P1(0,ABS(ES_MAP(IXX))),
     $                           P1(0,ABS(ES_MAP(JXX))))
               IF((ES_MAP(IXX)*ES_MAP(JXX)).LT.0) S(IXX,JXX)=-S(IXX,JXX)
               S(JXX,IXX) = S(IXX,JXX)
            ENDIF
         ENDDO
      ENDDO
c nice factorized form, obtained with macsyma real.mac performing
c the summation over permutations symbolically.
      result = 4*(
     1   s(1,5)*s(2,5)*(s(2,5)**2+s(1,5)**2)
     2  +s(1,4)*s(2,4)*(s(2,4)**2+s(1,4)**2)
     3  +s(1,3)*s(2,3)*(s(1,3)**2+s(2,3)**2)   )*
     4 (-s(1,2)*s(1,3)*s(2,4)*s(3,5)*s(4,5)
     5  -s(1,2)*s(1,4)*s(2,3)*s(3,5)*s(4,5)
     6  -s(1,2)*s(1,3)*s(2,5)*s(3,4)*s(4,5)
     7  -s(1,2)*s(1,5)*s(2,3)*s(3,4)*s(4,5)
     8  -s(1,2)*s(1,4)*s(2,5)*s(3,4)*s(3,5)
     9  -s(1,2)*s(1,5)*s(2,4)*s(3,4)*s(3,5)
     1  +9*s(1,3)*s(1,4)*s(2,3)*s(2,5)*s(4,5)
     2  +9*s(1,3)*s(1,5)*s(2,3)*s(2,4)*s(4,5)
     3  +9*s(1,3)*s(1,4)*s(2,4)*s(2,5)*s(3,5)
     4  +9*s(1,4)*s(1,5)*s(2,3)*s(2,4)*s(3,5)
     5  +9*s(1,3)*s(1,5)*s(2,4)*s(2,5)*s(3,4)
     6  +9*s(1,4)*s(1,5)*s(2,3)*s(2,5)*s(3,4)
     7 +10*s(1,2)**2*s(3,4)*s(3,5)*s(4,5)/9)
     8  /( s(1,2)*s(1,3)*s(1,4)*s(1,5)*s(2,3)*s(2,4)
     9    *s(2,5)*s(3,4)*s(3,5)*s(4,5));
C - Then we calculate the Ellis Sexton C-function (Eq. 3.6) as a sum
C - over permutations of the arguments of the F^C function (Eq. 3.7)
C - for the gluons only.
c      RESULT=0.
c      DO IXX=3,5
c         DO LXX=3,5
c            IF(LXX.EQ.IXX) CYCLE
c            DO MXX=3,5
c               IF((MXX.EQ.IXX).OR.
c     $            (MXX.EQ.LXX)) CYCLE
c               RESULT = RESULT + dijet_FC_FN(1,2,IXX,LXX,MXX,S)
c            ENDDO
c         ENDDO
c      ENDDO
C - On the next line we obtain the C function (Eq.3.6).
      RESULT=GS6*RESULT
C - To get the matrix element squared we need to divide by the
C - spin colour averaging factor 4V^2 (see Table 7).
      RESULT=RESULT/AVG
C - We also need to include another symmetry factor of 3! to get
C - agreement with the MadGraph (and, hopefully, reality).
      RESULT=RESULT/SYM

      END


      FUNCTION dijet_FC_FN(I1,I2,I3,I4,I5,S)
      IMPLICIT NONE
      REAL*8 dijet_FC_FN
      INTEGER I1,I2,I3,I4,I5
      REAL*8 S(5,5),AP,AM,BP,BM,CP,CM
      REAL*8 N_GLUONS,N_COLOURS

      N_GLUONS  = 8.
      N_COLOURS = 3.

      AP = (S(I1,I3)+S(I2,I3))/2.
      AM = (S(I1,I3)-S(I2,I3))/2.
      BP = (S(I1,I4)+S(I2,I4))/2.
      BM = (S(I1,I4)-S(I2,I4))/2.
      CP = (S(I1,I5)+S(I2,I5))/2.
      CM = (S(I1,I5)-S(I2,I5))/2.
      
      dijet_FC_FN = N_GLUONS/N_COLOURS/N_COLOURS 
     $      * ( 
     $          -N_COLOURS*N_COLOURS*(AP*BP-AM*BM)/S(I3,I4)
     $          * ( 2.*(AP*AP+AM*AM)/S(I1,I4)/S(I2,I4)/S(I1,I5)/S(I2,I5)
     $            +    (CP*CP+CM*CM)/S(I1,I3)/S(I2,I3)/S(I1,I4)/S(I2,I4)
     $            )
     $          + N_COLOURS**4*(AP*BP-AM*BM)/S(I1,I2)/S(I4,I5)/S(I3,I5)
     $          * ( 2.*(BP*BP+BM*BM)/S(I1,I3)/S(I2,I3)
     $            + (CP**4-CM**4)/S(I1,I4)/S(I2,I4)/S(I1,I3)/S(I2,I3)
     $            )
     $          + (N_COLOURS*N_COLOURS+1.)*0.25*S(I1,I2)
     $          * 2.*(CP*CP+CM*CM)/S(I1,I3)/S(I2,I3)/S(I1,I4)/S(I2,I4)  
     $        )
      RETURN
      END


C ----------------------------------------------------------------- C
      SUBROUTINE dijet_ES_A(P1,RESULT,ES_MAP,AVG,SYM)
C - The squared, spin and colour summed matrix element for qq'->qq'g
C - , averaged over initial state colours and spins. This comes from
C - formulae in Ellis Marchesini and Webber NPB286:643,1987. Agreement
C - with MadGraph was very satisfactory.
C ----------------------------------------------------------------- C

      IMPLICIT NONE
C -   *************
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_math.h'
      REAL*8 P1(0:3,nlegreal),AVG,SYM
      REAL*8 dijet_dotp
      EXTERNAL dijet_dotp
      REAL*8 S(5,5),dijet_FA_FN,RESULT
      INTEGER ES_MAP(5)
      INTEGER IXX,JXX
      REAL*8 GS6
C - Calculate the strong coupling constant prefactor
      GS6 = (4.*PI*ST_ALPHA)**3
C - First calculate the kinematic invariants once and for all (in
C - Ellis sextion conventions).
      DO IXX=1,5
         DO JXX=IXX,5 
            IF(IXX.EQ.JXX) THEN
               S(IXX,JXX) = 0.0
            ELSE
               S(IXX,JXX) = dijet_dotp(P1(0,ABS(ES_MAP(IXX))),
     $                           P1(0,ABS(ES_MAP(JXX))))
               IF((ES_MAP(IXX)*ES_MAP(JXX)).LT.0) S(IXX,JXX)=-S(IXX,JXX)
               S(JXX,IXX) = S(IXX,JXX)
            ENDIF
         ENDDO
      ENDDO
C - Then we calculate the Ellis Marchesini Webber Sum|M^(A)|^2 function
      RESULT = dijet_FA_FN(1,2,3,4,5,S)
C - On the next line we obtain the A function (Eq.3.4).
      RESULT=GS6*RESULT
C - To get the matrix element squared we need to divide by the
C - spin colour averaging factor (see Table 7).
      RESULT=RESULT/AVG
C - We may also need to include another symmetry factor to get
C - agreement with the MadGraph (and, hopefully, reality).
      RESULT=RESULT/SYM

      END

      FUNCTION dijet_FA_FN(I1,I2,I3,I4,I5,S)
      IMPLICIT NONE
      REAL*8 dijet_FA_FN
      INTEGER I1,I2,I3,I4,I5
      REAL*8 S(5,5)
      REAL*8 CF,CA

      CF = 4d0/3
      CA = 3

      dijet_FA_FN = CF/CA*( S(I1,I2)**2+S(I3,I4)**2
     $              + S(I1,I4)**2+S(I2,I3)**2
     $              )
     $              / 2 / S(I1,I3) / S(I2,I4)
     $             *( 
     $                (2*CF-1/CA)*(   S(I1,I4)/(S(I1,I5)*S(I4,I5))
     $                              + S(I2,I3)/(S(I2,I5)*S(I3,I5))
     $                              )
     $              + 1/CA*(
     $                        2*S(I1,I2)/(S(I1,I5)*S(I2,I5))
     $                      + 2*S(I3,I4)/(S(I3,I5)*S(I4,I5))
     $                      -   S(I1,I3)/(S(I1,I5)*S(I3,I5))
     $                      -   S(I2,I4)/(S(I2,I5)*S(I4,I5))
     $                      )
     $              )

      dijet_FA_FN = dijet_FA_FN*36    ! Removes implicit 2*3*2*3 spin / colour avg
                          ! factor which goes in later in ES_A.
      RETURN
      END


C ----------------------------------------------------------------- C
      SUBROUTINE dijet_ES_B(P1,RESULT,ES_MAP,AVG,SYM)
C - The squared, spin and colour summed matrix element for qq->qqg
C - , averaged over initial state colours and spins. This comes from
C - formulae in Ellis Marchesini and Webber NPB286:643,1987. Agreement
C - with MadGraph was very satisfactory.
C ----------------------------------------------------------------- C

      IMPLICIT NONE
C -   *************
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_math.h'
      REAL*8 P1(0:3,nlegreal),AVG,SYM
      REAL*8 dijet_dotp
      EXTERNAL dijet_dotp
      REAL*8 S(5,5),dijet_FB_FN,RESULT
      INTEGER ES_MAP(5)
      INTEGER IXX,JXX
      REAL*8 GS6
C - Calculate the strong coupling constant prefactor
      GS6 = (4.*PI*ST_ALPHA)**3
C - First calculate the kinematic invariants once and for all (in
C - Ellis sextion conventions).
      DO IXX=1,5
         DO JXX=IXX,5 
            IF(IXX.EQ.JXX) THEN
               S(IXX,JXX) = 0.0
            ELSE
               S(IXX,JXX) = dijet_dotp(P1(0,ABS(ES_MAP(IXX))),
     $                           P1(0,ABS(ES_MAP(JXX))))
               IF((ES_MAP(IXX)*ES_MAP(JXX)).LT.0) S(IXX,JXX)=-S(IXX,JXX)
               S(JXX,IXX) = S(IXX,JXX)
            ENDIF
         ENDDO
      ENDDO
C - Then we calculate the Ellis Sexton B-function (Eq. 3.5/g_S^6).
      RESULT = dijet_FB_FN(1,2,3,4,5,S)
C - On the next line we obtain the A function (Eq.3.4).
      RESULT=GS6*RESULT
C - To get the matrix element squared we need to divide by the
C - spin colour averaging factor (see Table 7).
      RESULT=RESULT/AVG
C - We may also need to include another symmetry factor to get
C - agreement with the MadGraph (and, hopefully, reality).
      RESULT=RESULT/SYM

      END

      FUNCTION dijet_FB_FN(I1,I2,I3,I4,I5,S)
      IMPLICIT NONE
      REAL*8 dijet_FB_FN,dijet_FA_FN
      INTEGER I1,I2,I3,I4,I5
      REAL*8 S(5,5)
      REAL*8 CF,CA

      CF = 4d0/3
      CA = 3

      dijet_FB_FN = 
     $    -CF/CA/CA*(   ( S(I1,I2)**2 + S(I3,I4)**2 )
     $                * (   S(I1,I2)*S(I3,I4)/(S(I1,I3)*S(I2,I4)
     $                                        *S(I2,I3)*S(I1,I4))
     $                  -1/(S(I2,I3)*S(I1,I4))
     $                  -1/(S(I1,I3)*S(I2,I4))
     $                  )
     $              ) / 2
     $             *( 
     $                2*(CF+1/CA) * ( S(I1,I2)/(S(I1,I5)*S(I2,I5))
     $                              + S(I3,I4)/(S(I3,I5)*S(I4,I5))
     $                              )
     $              + 1/CA*(
     $                      - S(I1,I3)/(S(I1,I5)*S(I3,I5))
     $                      - S(I1,I4)/(S(I1,I5)*S(I4,I5))
     $                      - S(I2,I3)/(S(I2,I5)*S(I3,I5))
     $                      - S(I2,I4)/(S(I2,I5)*S(I4,I5))
     $                      )
     $              )*36

      dijet_FB_FN = dijet_FB_FN
     $      + dijet_FA_FN(I1,I2,I3,I4,I5,S)
     $      + dijet_FA_FN(I1,I2,I4,I3,I5,S)

      RETURN
      END
