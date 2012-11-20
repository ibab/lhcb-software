C --------------------------------------------------------------------
      SUBROUTINE dijet_KS_2TO2_MAP(VFLAV,nleg,KS_LABEL,KS_MAP)
c
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      character * 1 ks_label
      integer nleg,vflav(nleg),ks_map(nleg)
      integer v(nlegreal),vv(nlegreal)
      integer j,itmp
      do j=1,nleg
         if(j.gt.2) then
            v(j)= - vflav(j)
         else
            v(j)=vflav(j)
         endif
c set gluon smaller than any flavour
         if(v(j).eq.0) v(j)=-21
      enddo
c Find permutation to put them in descending order:
c gluons last, flavours in decreasing order
c CERNLIB routine:
      call dijet_SORTZV(v,ks_map,nleg,-1,1,0)
      do j=1,nleg
         vv(j)=v(ks_map(j))
      enddo
c classify processes
      if(vv(1).eq.-21) then
         ks_label='D'
      elseif(vv(3).eq.-21) then
         ks_label='C'
      elseif(vv(1).eq.vv(2)) then
         ks_label='B'
      else
         ks_label='A'
c Kunsz Soper have last two particles exchanged in this case (i.e. q Q -> -q -Q
c rather than q Q -> -Q -q (if q>Q)
         itmp=vv(3)
         vv(3)=vv(4)
         vv(4)=itmp
         itmp=ks_map(3)
         ks_map(3)=ks_map(4)
         ks_map(4)=itmp
      endif
      end

      subroutine dijet_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'

      integer nlegs
      parameter (nlegs=nlegborn)
      real* 8 p(0:3,nlegs)
      integer vflav(nlegs)
      real* 8 virtual
      integer mu

C     Define a real *8 value for nc in order
C     to avoid integer by integer division
      real*8 ncol
      parameter (ncol=nc)
      real*8 VC
      parameter(VC=ncol**2-1)

C     Abbreviation of (4.*pi*st_alpha)**2
      real*8 gs4

C     Kunszt and Soper's virtual functions (eqs. B17-B20).
      real*8   dijet_psitilde6ns_a,dijet_psitilde6ns_b,dijet_psitilde6ns_c,dijet_psitilde6ns_d
      external dijet_psitilde6ns_a,dijet_psitilde6ns_b,dijet_psitilde6ns_c,dijet_psitilde6ns_d

c     The process class according to Kunszt Soper
      character*1  ks_label
c     The map from POWHEG-BOX to Kunszt-Soper particles and momenta.
      integer ksmap(4),k
c     Kunszt-Soper momenta analogous to POWHEG-BOX.
      real*8  k1(0:3),k2(0:3),k3(0:3),k4(0:3)

C     Setting the QCD coupling squared.
      gs4 = (4.*pi*st_alpha)**2

c     Get the Kunszt Soper and Madgraph labels as well as the map
c     to Kunszt Soper conventions:
      call dijet_KS_2TO2_MAP(vflav,nlegborn,ks_label,ksmap)  
c     Assign Kunszt Soper momenta
      do mu=0,3
        p(mu,3) = -p(mu,3)        ! temprarily flip the outgoing momenta
        p(mu,4) = -p(mu,4)        !
        k1(mu)  =  p(mu,ksmap(1)) ! setting the Kunszt Soper momenta using
        k2(mu)  =  p(mu,ksmap(2)) ! the Kunszt Soper map.
        k3(mu)  =  p(mu,ksmap(3)) ! 
        k4(mu)  =  p(mu,ksmap(4)) !
        p(mu,3) = -p(mu,3)        ! restore the powheg outgoing momenta
        p(mu,4) = -p(mu,4)        !
      enddo      
c     Virtual contributions from Kunszt-Soper.
C     WARNING: the st_alpha/2*pi missing factor is provided later
C     on by the program
C --------------------------------------------------------------------
C     A-type: q + Q -> q + Q plus charge conjugations and crossings
C --------------------------------------------------------------------
      if(ks_label.eq.'A') then
         virtual = dijet_psitilde6ns_a(k1,k2,k3,k4)
C --------------------------------------------------------------------
C     B-type: q + q -> q + q plus charge conjugations
C --------------------------------------------------------------------
      elseif(ks_label.eq.'B') then
         virtual = dijet_psitilde6ns_b(k1,k2,k3,k4)
C --------------------------------------------------------------------
C     C-type: q + qb -> g + g plus charge conjugations & crossings
C --------------------------------------------------------------------
      elseif(ks_label.eq.'C') then
         virtual = dijet_psitilde6ns_c(k1,k2,k3,k4)
C --------------------------------------------------------------------
C     D-type: g + g -> g + g
C --------------------------------------------------------------------
      elseif(ks_label.eq.'D') then
         virtual = dijet_psitilde6ns_d(k1,k2,k3,k4)
      else
         write(*,*) 'dijet_setvirtual: could not identify flavour list!'
         call exit(1)
      endif
      virtual=virtual*gs4
      do k=1,2
         if(vflav(k).ne.0) then
c - sign for crossing fermion line
            virtual=-virtual/(2*ncol)
         else
            virtual=virtual/(2*Vc)
         endif
      enddo
      if(vflav(3).eq.vflav(4)) virtual=virtual/2.0d0
      end


C THE FOLLOWING FUNCTIONS ARE TAKEN FROM ELLIS-SEXTON CODE
C AND MODIFIED ACCORDING POWHEG STRUCTURE
C WARNING: THE CLASSIFICATION IN TERMS OF A,B,C,D
C PROCESSES IS ACCORDING TO KUNSZT-SOPER AND NOT 
C ACCORDING ELLIS-SEXTON  A,B,C,D FUNCTIONS
 
      function dijet_psitilde6ns_a(p1,p2,p3,p4)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      real *8  dijet_psitilde6ns_a
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      DOUBLE PRECISION dijet_THETA
      DOUBLE PRECISION LS,LT,LU,LMU,L2S,L2T,L2U
      DOUBLE PRECISION QES,MUUV
      DOUBLE PRECISION N,V,NFLAVOR
      EXTERNAL dijet_THETA
C     Invariant quantities
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
C     Renormalization scale 
      MUUV=sqrt(st_muren2)
C     In the POWHEG-BOX we always assume that QES=st_muren
      QES=sqrt(st_muren2)
C     SU(3) constants
      N=nc
      V=nc**2-1
C     NUMBER OF LIGHT FLAVOURS
      NFLAVOR=st_nlight
C  Here are the Ellis and Sexton log functions
C
      LS = DLOG(DABS(S/QES**2))
      LT = DLOG(DABS(T/QES**2))
      LU = DLOG(DABS(U/QES**2))
      LMU = DLOG(DABS(MUUV**2/QES**2))
      L2S = LS**2 - PI**2 * dijet_THETA(S.GT.0.0D0)
      L2T = LT**2 - PI**2 * dijet_THETA(T.GT.0.0D0)
      L2U = LU**2 - PI**2 * dijet_THETA(U.GT.0.0D0)
      
      dijet_psitilde6ns_a =
     >     V/(9.0D0*N*T**2)
     >     *(-  9*PI**2*(N**2-4 )*(S**2 - U**2)
     >     +  2*(72 + 13*N**2 - 10*N*NFLAVOR + 9*N**2*PI**2)
     >     *(S**2 + U**2)
     >     +  6*N*(11*N - 2*NFLAVOR)*(S**2 + U**2)*LMU
     >     - 36*T*U*LS
     >     +  3*(- 6*S**2 - 30*U**2 + 4*N*NFLAVOR*(S**2 + U**2)
     >     - N**2*(7*S**2 + 3*T**2 + U**2))*LT
     >     - 18*S*T*(N**2 -2)*LU
     >     - 36*(3*S**2 + U**2)*LS*LT
     >     - 18*(N**2 -2)*(S**2 + 3*U**2)*LT*LU
     >     + 18*(S**2 - U**2)*L2S
     >     +  9*(N**2*(S**2 + 3*U**2) + 2*(3*S**2 - U**2))*L2T
     >     -  9*(N**2 - 2)*(S**2 - U**2)*L2U)

      return
      end


      function dijet_psitilde6ns_b(p1,p2,p3,p4)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      real *8  dijet_psitilde6ns_b
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      DOUBLE PRECISION dijet_THETA
      DOUBLE PRECISION LS,LT,LU,LMU,L2S,L2T,L2U
      DOUBLE PRECISION QES,MUUV
      DOUBLE PRECISION N,V,NFLAVOR
      EXTERNAL dijet_THETA
C     Invariant quantities
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
C     Renormalization scale 
      MUUV=sqrt(st_muren2)
C     In the POWHEG-BOX we always assume that QES=st_muren
      QES=sqrt(st_muren2)
C     SU(3) constants
      N=nc
      V=nc**2-1
C     NUMBER OF LIGHT FLAVOURS
      NFLAVOR=st_nlight
C  Here are the Ellis and Sexton log functions
C
      LS = DLOG(DABS(S/QES**2))
      LT = DLOG(DABS(T/QES**2))
      LU = DLOG(DABS(U/QES**2))
      LMU = DLOG(DABS(MUUV**2/QES**2))
      L2S = LS**2 - PI**2 * dijet_THETA(S.GT.0.0D0)
      L2T = LT**2 - PI**2 * dijet_THETA(T.GT.0.0D0)
      L2U = LU**2 - PI**2 * dijet_THETA(U.GT.0.0D0)
      
      dijet_psitilde6ns_b =
     >     40*N*NFLAVOR*S**2*T*U - 4*N**2*S**2*T*U*(13 + 9*PI**2)
     >   - 9*T*U*(32*S**2 + PI**2*(S**2 + 2*T*U))
     >   + 36*N*(S**2*(4 + PI**2) * (T**2 + U**2)
     >   + (4 - PI**2)  *  (T**4 + U**4))
     >   + N**3*(S**2*(26 + 9*PI**2)*(T**2 + U**2)
     >   + (26 + 27*PI**2)*(T**4 + U**4))
     >   - 20*N**2*NFLAVOR*(T**4 + U**4 + S**2*(T**2 + U**2))
C
      dijet_psitilde6ns_b =  dijet_psitilde6ns_b
     >  + 6*N*(11*N - 2*NFLAVOR)
     >     *(-2*S**2*T*U + N*(T**4 + U**4 + S**2*(T**2 + U**2))) *LMU
     >  - 36*N*T*U*(T**2 + U**2)*LS
     >  - 6*U*(- 2*N**2*S**2*T + 2*N*NFLAVOR*S**2*T
     >         - 2*N**2*NFLAVOR*U*(S**2 + U**2)
     >         + 6*S*T*(T + 2*U) + N**3*(-3*T**3 + 2*T**2*U
     >         + 7*T*U**2 + 4*U**3)
     >         + 3*N*(2*T**3 + 3*T**2*U + 2*T*U**2 + 6*U**3)) *LT
     >  - 6*T*(- 2*N**2*S**2*U + 2*N*NFLAVOR*S**2*U
     >         - 2*N**2*NFLAVOR*T*(S**2 + T**2)
     >         + 6*S*U*(2*T + U)
     >         + N**3*(4*T**3 + 7*T**2*U + 2*T*U**2 - 3*U**3)
     >         + 3*N*(6*T**3 + 2*T**2*U + 3*T*U**2 + 2*U**3)) * LU
C
       dijet_psitilde6ns_b =  dijet_psitilde6ns_b
     >  - 36*U*(- S**2*T - N**2*S**2*T + N*U*(3*S**2 + U**2)) *LS*LT
     >  - 36*T*(- S**2*U - N**2*S**2*U + N*T*(3*S**2 + T**2)) *LS*LU
     >  - 18*(  2*N*(-2 + N**2)*(T**2 - T*U + U**2)
     >          *(2*T**2 + 3*T*U + 2*U**2)
     >        + T*U*(3*T**2 + 4*T*U + 3*U**2)) * LT*LU
     >  + 18*N*T*U*(S**2 + T**2 + U**2) * L2S
     >  + 9*U*(- 2*N**2*S**2*T - 4*N*(S**3 - S*T**2 - T**3)
     >         - T*(3*T**2 + 8*T*U + 3*U**2)
     >         - 2*N**3*(T**3 - T*U**2 - 2*U**3)) * L2T
     >  + 9*T*(- 2*N**2*S**2*U - 4*N*(S**3 - S*U**2 - U**3)
     >         - U*(3*T**2 + 8*T*U + 3*U**2)
     >         + 2*N**3*(2*T**3 + T**2*U - U**3)) * L2U
C
       dijet_psitilde6ns_b = V/(9*N**2*T**2*U**2) * dijet_psitilde6ns_b

      return
      end


      function dijet_psitilde6ns_c(p1,p2,p3,p4)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      real *8  dijet_psitilde6ns_c
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      DOUBLE PRECISION dijet_THETA
      DOUBLE PRECISION LS,LT,LU,LMU,L2S,L2T,L2U
      DOUBLE PRECISION QES,MUUV
      DOUBLE PRECISION N,V,NFLAVOR
      EXTERNAL dijet_THETA
C     Invariant quantities
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
C     Renormalization scale 
      MUUV=sqrt(st_muren2)
C     In the POWHEG-BOX we always assume that QES=st_muren
      QES=sqrt(st_muren2)
C     SU(3) constants
      N=nc
      V=nc**2-1
C     NUMBER OF LIGHT FLAVOURS
      NFLAVOR=st_nlight
C  Here are the Ellis and Sexton log functions
C
      LS = DLOG(DABS(S/QES**2))
      LT = DLOG(DABS(T/QES**2))
      LU = DLOG(DABS(U/QES**2))
      LMU = DLOG(DABS(MUUV**2/QES**2))
      L2S = LS**2 - PI**2 * dijet_THETA(S.GT.0.0D0)
      L2T = LT**2 - PI**2 * dijet_THETA(T.GT.0.0D0)
      L2U = LU**2 - PI**2 * dijet_THETA(U.GT.0.0D0)
      
      dijet_psitilde6ns_c =
     >   3*S**2*( -7*(T**2 + U**2) + PI**2*(3*S**2 -2*T*U) )
     >   + 3*N**2*( 14*(T**4 + U**4) + T*U*(13*S**2 + 4*T*U )
     >            + PI**2*(T**3*U + 2*T**2*U**2 + T*U**3)    )
     >   - 3*N**4*( 7*(T**4 + U**4)+ T*U*(S**2 + 10*T*U)
     >            - PI**2*(T - U)**2*(T**2 + U**2)  )
     >   + 2*N*(11*N - 2*NFLAVOR)*(-S**2 + N**2*(T**2 + U**2))
     >         *(T**2 + U**2) * LMU
     >   - 3*T*U*(  4*T**2 + 8*T*U + 4*U**2
     >         + N**2*(-1 + N**2)*(T**2 - 10*T*U + U**2) ) * LS
     >   + 3*S*U*(S + N**2*U)*(2*T + 3*U + N**2*(2*T - 3*U)) * LT
     >   + 3*S*T*(S + N**2*T)*(3*T + 2*U + N**2*(-3*T + 2*U)) * LU
C
       dijet_psitilde6ns_c = dijet_psitilde6ns_c 
     >   - 6*(  N**4*U*(U - T)*(2*T**2 + T*U + U**2)
     >     + S*(S - N**2*T)*(2*T**2 + 2*T*U + U**2)  ) * LS*LT
     >   - 6*(  N**4*T*(T - U)*(T**2 + T*U + 2*U**2)
     >   + S*(S - N**2*U)*(T**2 + 2*T*U + 2*U**2)  ) * LS*LU
     >   + 12*N**2*S**2*(T**2 + U**2) * LT*LU
     >   + 3*( 2*S**4 - 2*N**4*T*U*(T**2 + U**2)
     >     + N**2*(T**2 + T*U + 2*U**2)*(2*T**2 + T*U + U**2) ) *L2S
     >   - 3*S*(- N**4*U*(2*T**2 - T*U + U**2)
     >       + (N**2*T - S)*(2*T**2 + 2*T*U + U**2) ) *L2T
     >   - 3*S*(- N**4*T*(2*U**2 - T*U + T**2 )
     >       + (N**2*U - S)*( 2*U**2 + 2*T*U + T**2)) *L2U
C
       dijet_psitilde6ns_c = V/(3.0D0*N**2*S**2*T*U) * dijet_psitilde6ns_c

      return
      end


      function dijet_psitilde6ns_d(p1,p2,p3,p4)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      real *8  dijet_psitilde6ns_d
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      DOUBLE PRECISION dijet_THETA
      DOUBLE PRECISION LS,LT,LU,LMU,L2S,L2T,L2U
      DOUBLE PRECISION QES,MUUV
      DOUBLE PRECISION N,V,NFLAVOR
      EXTERNAL dijet_THETA
C     Invariant quantities
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
C     Renormalization scale 
      MUUV=sqrt(st_muren2)
C     In the POWHEG-BOX we always assume that QES=st_muren
      QES=sqrt(st_muren2)
C     SU(3) constants
      N=nc
      V=nc**2-1
C     NUMBER OF LIGHT FLAVOURS
      NFLAVOR=st_nlight
C  Here are the Ellis and Sexton log functions
C
      LS = DLOG(DABS(S/QES**2))
      LT = DLOG(DABS(T/QES**2))
      LU = DLOG(DABS(U/QES**2))
      LMU = DLOG(DABS(MUUV**2/QES**2))
      L2S = LS**2 - PI**2 * dijet_THETA(S.GT.0.0D0)
      L2T = LT**2 - PI**2 * dijet_THETA(T.GT.0.0D0)
      L2U = LU**2 - PI**2 * dijet_THETA(U.GT.0.0D0)
      
  
       dijet_psitilde6ns_d =
     >  2*N**2*NFLAVOR*( - (66 + 27*PI**2)*S**2*T**2*U**2
     >           + 40*(S**6 + T**6 + U**6))
     >   + 2*N**3*(  6*(125 - 27*PI**2)*S**2*T**2*U**2
     >         - 4*(67 - 9*PI**2)*(S**6 + T**6 + U**6))
     >   + 6*N**2*(11*N - 2*NFLAVOR)*(S**2 + T**2 + U**2)**3 *LMU
     >   + 6*N**2*T*U*(S**2 + T**2 + U**2)
     >       *(  NFLAVOR *(5*T**2 + 2*T*U + 5*U**2)
     >         - 2*N*(7*T**2 - 8*T*U + 7*U**2) ) *LS
C
       dijet_psitilde6ns_d = dijet_psitilde6ns_d 
     >   + 6*N**2*S*U*(S**2 + T**2 + U**2)
     >       *(  NFLAVOR *(5*S**2 + 2*S*U + 5*U**2)
     >         - 2*N*(7*S**2 - 8*S*U + 7*U**2) ) *LT
     >   + 6*N**2*S*T*(S**2 + T**2 + U**2)
     >       *(  NFLAVOR* (5*S**2 + 2*S*T + 5*T**2)
     >         - 2*N*(7*S**2 - 8*S*T + 7*T**2) ) *LU
     >   - 36*N**2*U**2*( NFLAVOR*S*T*(S**2 + T**2)
     >             + 2*N*(2*S**4 + 2*S**3*T + 3*S**2*T**2 + 2*S*T**3
     >             + 2*T**4))*LS*LT
C
       dijet_psitilde6ns_d = dijet_psitilde6ns_d 
     >   - 36*N**2*S**2*(  NFLAVOR*T*U*(T**2 + U**2)
     >             + 2*N*(2*T**4 + 2*T**3*U + 3*T**2*U**2 + 2*T*U**3
     >             + 2*U**4))*LT*LU
     >   - 36*N**2*T**2*(  NFLAVOR*S*U*(S**2 + U**2)
     >             + 2*N*(2*S**4 + 2*S**3*U + 3*S**2*U**2 + 2*S*U**3
     >             + 2*U**4))*LS*LU
     >   + 18*N**2*S**2*T*U*(4*N*(T**2 + U**2)
     >             - NFLAVOR*(T**2 + 3*T*U + U**2))*L2S
     >   + 18*N**2*S*T**2*U*(4*N*(S**2 + U**2)
     >             - NFLAVOR*(S**2 + 3*S*U + U**2))*L2T
     >   + 18*N**2*S*T*U**2*(4*N*(S**2 + T**2)
     >             - NFLAVOR*(S**2 + 3*S*T + T**2))*L2U
C
       dijet_psitilde6ns_d = V/(9.0D0*S**2*T**2*U**2) * dijet_psitilde6ns_d
      return
      end


      DOUBLE PRECISION FUNCTION dijet_THETA(BOOL)
C     
      LOGICAL BOOL
      IF (BOOL) THEN
         dijet_THETA = 1.0D0
      ELSE
         dijet_THETA = 0.0D0
      ENDIF
      RETURN
      END
C
