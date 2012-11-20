      function st_tch_dotp(p1,p2)
      implicit none
      real * 8 st_tch_dotp,p1(0:3),p2(0:3)
      st_tch_dotp = (p1(0)*p2(0) - p1(3)*p2(3)) - p1(1)*p2(1) - p1(2)*p2(2)
      end

      function st_tch_cdotp(p1,cp2)
      implicit none      
      complex * 16 st_tch_cdotp,cp2(0:3)
      real * 8 p1(0:3)
      st_tch_cdotp = (p1(0)*cp2(0) - p1(3)*cp2(3))
     #      - p1(1)*cp2(1) - p1(2)*cp2(2)
      end
      
      function st_tch_ccdotp(cp1,cp2)
      implicit none      
      complex * 16 st_tch_ccdotp,cp1(0:3),cp2(0:3)
      st_tch_ccdotp = (cp1(0)*cp2(0) - cp1(3)*cp2(3))
     #     - cp1(1)*cp2(1) - cp1(2)*cp2(2)
      end


C
C
C*********** polarisation vector (st_tch_POLVEC) ************
C*********** entry st_tch_HELVEC ****************************
C
C   Calculates the polarisation vector eps_mu(k,lambda) as given in
C   MAD/PH/402, Appendix A, Eqn A.12 and A.13
C   checked on May 24, 88
C   bug in st_tch_HELVEC for KT=0 corrected Nov.4, 1993
C
C   INPUT:
C
C            kbar      real*8  array(0:4)
C                        physical four momentum for boson and mass
C                        kbar(0:4) = [Ebar, kbarx, kbary, kbarz, mass]
C
C            sigmak      integer
C                        sign factor for vector boson
C                        = +1 outgoing
C                        = -1 incoming
C
C            lambda      integer
C                        rectangular polarization index as used
C                        in Eqn A.12. Allowed st_tch_values= 1,2,3.
C                        or vector boson helicity (allowed st_tch_values
C                        -1,0,1) when entry st_tch_HELVEC is used
C
C
C   OUTPUT:
C            epscar  real*8  array(0:3)
C                        polarisation vector epsilon_mu(lambda)
C                               in eq. A.12 (cartesian basis)
C
C            epshel  complex*16  array(0:3)
C                        polarisation vector epsilon_mu(lambda)
C                               in eq. A.12/13 (helicity basis)
C
C                  The polarization vector for helicity -1,1 or
C                  polarization index 1,2 has vanishing time component.
C                  For longitudinal polarisation, current conservation
C                  is used to eliminate the time component. This is
C                  only possible for MASSLESS FERMIONS. For massive
C                  fermions the polarization vectors defined in eq.A.12
C                  must be used!
C
      SUBROUTINE st_tch_POLVEC(KBAR,LAMBDA,EPSCAR)
C
C   arguments:
C
      real*8  KBAR(0:4)
      real*8  EPSCAR(0:3)
      complex*16 EPSHEL(0:3)
      INTEGER SIGMAK, LAMBDA
C
C   local variables:
C
      real*8  A1,A2,A3,B1,B2
      real*8  KT,KMAGNI,NORMAL
C
C   code:
C
C
C            Compute polarization vector:
C            Eqn A.12
C
      EPSCAR(0) = 0.0d0
      IF (LAMBDA .EQ. 1) THEN
          KT = SQRT(KBAR(1)**2+KBAR(2)**2)
          IF (KT .GT. 0) THEN
            IF (KBAR(4) .EQ. 0 ) THEN
                  KMAGNI = KBAR(0)
            ELSE
                  KMAGNI = SQRT(KBAR(1)**2+KBAR(2)**2+KBAR(3)**2)
            ENDIF
            NORMAL = 1.0d0 / (KMAGNI*KT)
            EPSCAR(1) = KBAR(1)*KBAR(3)*NORMAL
            EPSCAR(2) = KBAR(2)*KBAR(3)*NORMAL
            EPSCAR(3) =          -KT**2*NORMAL
            ELSE
            EPSCAR(1) = dsign(1.0d0, KBAR(3))
            EPSCAR(2) = 0.0d0
            EPSCAR(3) = 0.0d0
          ENDIF
      ELSE IF (LAMBDA .EQ. 2) THEN
          KT = SQRT(KBAR(1)**2+KBAR(2)**2)
          IF (KT .GT. 0) THEN
            EPSCAR(1) = -KBAR(2)/KT
            EPSCAR(2) =  KBAR(1)/KT
            EPSCAR(3) = 0.0d0
            ELSE
            EPSCAR(1) = 0.0d0
            EPSCAR(2) = 1.0d0
            EPSCAR(3) = 0.0d0
            ENDIF
      ELSE IF (LAMBDA .EQ. 3) THEN
            IF (KBAR(4) .EQ. 0) THEN
C
C                  Mass = 0 not allowed for longitudinal polarization
C
                  WRITE(*,*) 'Mass = 0 for Lambda = 3 in st_tch_POLVEC'
                  EPSCAR(1) = 0.0d0
                  EPSCAR(2) = 0.0d0
                  EPSCAR(3) = 0.0d0
                  RETURN
            END IF
            KMAGNI = SQRT(KBAR(1)**2+KBAR(2)**2+KBAR(3)**2)
            NORMAL = KBAR(4)/(KBAR(0)*KMAGNI)
            EPSCAR(1) = KBAR(1)*NORMAL
            EPSCAR(2) = KBAR(2)*NORMAL
            EPSCAR(3) = KBAR(3)*NORMAL
      ELSE
C
C                  Unrecognised value of Lambda
C
            WRITE (*,*) 'Invalid Lambda in st_tch_POLVEC: Lambda = ',LAMBDA
                  EPSCAR(1) = 0.0d0
                  EPSCAR(2) = 0.0d0
                  EPSCAR(3) = 0.0d0
            END IF
      RETURN
C
      ENTRY st_tch_HELVEC(KBAR,SIGMAK,LAMBDA,EPSHEL)
C
C   code:
C
C
C            Compute polarization vector:
C            Use Eqn A.12 and combine according to A.13
C            For outgoing bosons (sigmak=+1) the complex
C            conjugate polarisation vector is returned.
C
      EPSHEL(0) = 0.0d0
      IF (ABS(LAMBDA) .EQ. 1) THEN
          KT = SQRT(2D0 *(KBAR(1)**2+KBAR(2)**2))
            IF (KT .GT. 0) THEN
            IF (KBAR(4) .EQ. 0 ) THEN
                  KMAGNI = KBAR(0)
            ELSE
                  KMAGNI = SQRT(KBAR(1)**2+KBAR(2)**2+KBAR(3)**2)
            ENDIF
            NORMAL = -LAMBDA/(KMAGNI*KT)
            A1 = KBAR(1)*KBAR(3)*NORMAL
            A2 = KBAR(2)*KBAR(3)*NORMAL
            A3 =    -KT**2 * 0.5d0 *NORMAL
            B1 = -KBAR(2)/KT
            B2 =  KBAR(1)/KT
            EPSHEL(1) = dcmplx(A1, SIGMAK * B1)
            EPSHEL(2) = dcmplx(A2, SIGMAK * B2)
            EPSHEL(3) = A3
          ELSE
            NORMAL = -LAMBDA / sqrt( 2D0 )
            EPSHEL(1) = dsign(1.0d0, KBAR(3)) * NORMAL
            EPSHEL(2) = dcmplx(0.0d0, -LAMBDA * SIGMAK * NORMAL)
            EPSHEL(3) = 0.0d0
          ENDIF
      ELSE IF (LAMBDA .EQ. 0) THEN
            IF (KBAR(4) .EQ. 0) THEN
C
C                  Mass = 0 not allowed for longitudinal polarization
C
                  WRITE(*,*) 'Mass = 0 for Lambda = 0 in st_tch_HELVEC'
                  EPSHEL(1) = 0.0d0
                  EPSHEL(2) = 0.0d0
                  EPSHEL(3) = 0.0d0
                  RETURN
                  END IF
            KMAGNI = SQRT(KBAR(1)**2+KBAR(2)**2+KBAR(3)**2)
            NORMAL = KBAR(4)/(KBAR(0)*KMAGNI)
            EPSHEL(1) = KBAR(1)*NORMAL
            EPSHEL(2) = KBAR(2)*NORMAL
            EPSHEL(3) = KBAR(3)*NORMAL
      ELSE
C
C                  Unrecognised value of Lambda
C
            WRITE (*,*) 'Invalid Lambda in st_tch_HELVEC: Lambda = ',LAMBDA
                  EPSHEL(1) = 0.0d0
                  EPSHEL(2) = 0.0d0
                  EPSHEL(3) = 0.0d0
            END IF
      RETURN
      END




c p = physical momentum  p[0] = E > 0
c ferm_type = fermion (+1) or antifermion (-1)
c gives st_tch_ket(1..2,-1:1) for massless spinors
c  -1 : 1 helicity
      subroutine st_tch_ket(p,ferm_type,psi)
      implicit none
      real * 8 p(0:3)
      integer ferm_type
      complex * 16 psi(1:2,-1:1)
      real * 8 E,px,py,pz,Eppz,st_tch_norm,sqrt_Eppz
      real * 8 tiny
      parameter (tiny=1d-20)
      
      E  = p(0)
      px = p(1)
      py = p(2)
      pz = p(3)    


      if ( pz.gt.0.d0 ) then
         Eppz = E + pz
      else
         Eppz = (px**2 + py**2)/(E - pz)
      endif

       
      if (ferm_type.eq.1) then         
c     fermion => u(p)
         if ( Eppz.le.tiny*E ) then
c     p along negative z-axis 
            st_tch_norm = sqrt(2*E)
            psi(1,-1) = -st_tch_norm
            psi(2,-1) = 0d0
            psi(1,1)  = 0d0
            psi(2,1)  = st_tch_norm
         else
            sqrt_Eppz = sqrt(Eppz)
            st_tch_norm = 1d0/sqrt_Eppz
            psi(1,-1) = dcmplx(-px*st_tch_norm,py*st_tch_norm)
            psi(2,-1) = sqrt_Eppz
            psi(1,1)  = sqrt_Eppz
            psi(2,1)  = dcmplx(px*st_tch_norm,py*st_tch_norm)
         endif
      else
c     antifermion => v(p)
         if ( Eppz.le.tiny*E ) then
c     p along negative z-axis 
            st_tch_norm = sqrt(2*E)
            psi(1,1) = 0d0
            psi(2,1) = -st_tch_norm
            psi(1,-1)  = st_tch_norm
            psi(2,-1)  = 0d0
         else
            sqrt_Eppz = sqrt(Eppz)
            st_tch_norm = 1d0/sqrt_Eppz
            psi(1,1) = -sqrt_Eppz
            psi(2,1) = dcmplx(-px*st_tch_norm,-py*st_tch_norm)
            psi(1,-1)  = dcmplx(px*st_tch_norm,-py*st_tch_norm)
            psi(2,-1)  = -sqrt_Eppz
         endif
      endif
      end


c p = physical momentum  p[0] = E > 0
c ferm_type = fermion (+1) or antifermion (-1)
c gives st_tch_ket(1..2,-1:1) for massless spinors
c  -1 : 1 helicity
      subroutine st_tch_ket_limit(p,ferm_type,psi)
      implicit none
      real * 8 p(0:3)
      integer ferm_type
      complex * 16 psi(1:2,-1:1)
      real * 8 E,px,py,pz,Eppz,st_tch_norm,sqrt_Eppz
      real * 8 tiny
      parameter (tiny=1d-10)
      
      E  = p(0)
      px = p(1)
      py = p(2)
      pz = p(3)    

      if (ferm_type.eq.1) then         
c         st_tch_norm = sqrt(2*E)
c         psi(1,-1) = -st_tch_norm
c         psi(2,-1) = 0d0
c         psi(1,1)  = 0d0
c         psi(2,1)  = st_tch_norm

         psi(1,-1) = dcmplx(-sqrt(E),sqrt(E))
         psi(2,-1) = 0d0
         psi(1,1)  = 0d0
         psi(2,1)  = dcmplx(sqrt(E),sqrt(E))
      else
         st_tch_norm = sqrt(2*E)
         psi(1,1) = 0d0
         psi(2,1) = -st_tch_norm
         psi(1,-1)  = st_tch_norm
         psi(2,-1)  = 0d0
      endif
      end





c p = physical momentum  p[0] = E > 0
c ferm_type = fermion (+1) or antifermion (-1)
c gives st_tch_bra(1..2,-1:1) for massless spinors
c  -1 : 1 helicity
      subroutine st_tch_bra(p,ferm_type,psi)
      implicit none
      real * 8 p(0:3)
      integer ferm_type
      complex * 16 psi(1:2,-1:1)
      real * 8 E,px,py,pz,Eppz,st_tch_norm,sqrt_Eppz
      
      real * 8 tiny
      parameter (tiny=1d-20)
      
      E  = p(0)
      px = p(1)
      py = p(2)
      pz = p(3)

      if ( pz.gt.0.d0 ) then
         Eppz = E + pz
      else
         Eppz = (px**2 + py**2)/(E - pz)
      endif

       
      if (ferm_type.eq.1) then         
c     fermion => u(p)
         if ( Eppz.le.tiny*E ) then
c     p along negative z-axis 
            st_tch_norm = sqrt(2*E)
            psi(1,-1) = -st_tch_norm
            psi(2,-1) = 0d0
            psi(1,1)  = 0d0
            psi(2,1)  = st_tch_norm
         else
            sqrt_Eppz = sqrt(Eppz)
            st_tch_norm = 1d0/sqrt_Eppz            
            psi(1,-1) = dcmplx(-px*st_tch_norm,-py*st_tch_norm)
            psi(2,-1) = sqrt_Eppz
            psi(1,1)  = sqrt_Eppz
            psi(2,1)  = dcmplx(px*st_tch_norm,-py*st_tch_norm)
         endif
      else
c     antifermion => v(p)
         if ( Eppz.le.tiny*E ) then
c     p along negative z-axis 
            st_tch_norm = sqrt(2*E)
            psi(1,1) = 0d0
            psi(2,1) = -st_tch_norm
            psi(1,-1)  = st_tch_norm
            psi(2,-1)  = 0d0
         else
            sqrt_Eppz = sqrt(Eppz)
            st_tch_norm = 1d0/sqrt_Eppz
            psi(1,1) = -sqrt_Eppz
            psi(2,1) = dcmplx(-px*st_tch_norm,py*st_tch_norm)
            psi(1,-1)  = dcmplx(px*st_tch_norm,py*st_tch_norm)
            psi(2,-1)  = -sqrt_Eppz
         endif
      endif
      end







c                           ( ris(1))
c gives p_slash |st_tch_ket,hel> = (       )
c                           ( ris(2))
      subroutine st_tch_slash_ket(p,st_tch_ket,hel,ris)
      implicit none
      complex * 16 p(0:3)
      complex * 16 st_tch_ket(1:2)
      integer hel
      complex * 16 ris(1:2)
      complex * 16 pslash(1:2,1:2)
      complex * 16 I
      parameter (I=(0,1))

      pslash(1,1) = p(0) - hel*p(3)
      pslash(1,2) = -hel*(p(1) - I*p(2))
      pslash(2,1) = -hel*(p(1) + I*p(2))
      pslash(2,2) = p(0) + hel*p(3)
      ris(1) = pslash(1,1)*st_tch_ket(1)+pslash(1,2)*st_tch_ket(2)
      ris(2) = pslash(2,1)*st_tch_ket(1)+pslash(2,2)*st_tch_ket(2)
      end


c gives <st_tch_bra,hel | p_slash = ( ris(1), ris(2) )      

      subroutine st_tch_slash_bra(p,st_tch_bra,hel,ris)
      implicit none
      complex * 16 p(0:3)
      complex * 16 st_tch_bra(1:2)
      integer hel
      complex * 16 ris(1:2)
      complex * 16 pslash(1:2,1:2)
      complex * 16 I
      parameter (I=(0,1))

      pslash(1,1) = p(0) - hel*p(3)
      pslash(1,2) = -hel*(p(1) - I*p(2))
      pslash(2,1) = -hel*(p(1) + I*p(2))
      pslash(2,2) = p(0) + hel*p(3)
      ris(1) = pslash(1,1)*st_tch_bra(1)+pslash(2,1)*st_tch_bra(2)
      ris(2) = pslash(1,2)*st_tch_bra(1)+pslash(2,2)*st_tch_bra(2)
      end


c <st_tch_bra, helbra| slash_p_1 slash_p_2 ... slash_p_n |st_tch_ket, helket>
c where list_mom = [p_1, p_2,...., p_n]
c helket = -(-1)^n*helbra, otherwise the result is zero!!!
      subroutine st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,list_mom,n,ris)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer helket
      integer n
      complex * 16 list_mom(0:3,n)
c      dimension list_mom(0:3,n)
      complex * 16 ris

      complex * 16 b(2),k(2),new_ket(2)
      complex * 16 I
      parameter (I=(0,1))
      integer helbra,ii,hel
      
      helbra = -(-1)**n*helket
      hel = helket
      b(1) = st_tch_bra(1,helbra)
      b(2) = st_tch_bra(2,helbra)
      k(1) = st_tch_ket(1,helket)
      k(2) = st_tch_ket(2,helket)
      
      do ii=n,1,-1
         call st_tch_slash_ket(list_mom(0,ii),k,hel,new_ket)
         k(1) = new_ket(1)
         k(2) = new_ket(2)
         hel = -hel
      enddo
      ris = b(1)*k(1)+b(2)*k(2)     
      end



C this subroutine adds the emission of a gluon/photon to a fermionic line 
c that has only ONE intermediate propagator!!!  This means that, BEFORE 
c emitting the gluon/phot, the expression of the line is
c <st_tch_bra, helbra| slash_curr_vertex_1, slash_p1, slash_curr_vertex_2|st_tch_ket, helket>
c where list_mom = [slash_curr_vertex_1, slash_p1, slash_curr_vertex_2]
c helket = -(-1)^n*helbra, otherwise the result is zero!!!
c NB!!! the momentum of the intermediate propagator is the momentum with 
c NO emission. On the other hand, the external momenta are such that 
c momentum conservation is satisfied!!
c You have to provide the momentum of the emitted gluon/photon (pg) 
c and its pol vector
c This procedure attach the right propagators too, when the gluon/photon 
c is emitted 
      subroutine st_tch_bra_slash_ket_gluon
     #     (st_tch_bra,st_tch_ket,helket,pbra,pket,pg,eps,list_mom,ris)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer helket
      real * 8 pbra(0:3),pket(0:3),pg(0:3),eps(0:3)
      complex * 16 list_mom(0:3,3)
c      dimension list_mom(0:3,n)
      complex * 16 ris

      complex * 16 ris1,ris2,ris3
c      complex * 16 I
c      parameter (I=(0,1))
      complex*16 local_list(0:3,5)
      integer mu
      complex * 16 st_tch_ccdotp
c emissione dalla gamba esterna st_tch_bra
      do mu=0,3
         local_list(mu,1) = eps(mu)
         local_list(mu,2) = pg(mu)+pbra(mu)
         local_list(mu,3) = list_mom(mu,1)
         local_list(mu,4) = list_mom(mu,2)
         local_list(mu,5) = list_mom(mu,3)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris1)

      ris1 = ris1 / st_tch_ccdotp(local_list(0,2),local_list(0,2))/
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))



c emissione dal propagatore interno
      do mu=0,3
         local_list(mu,1) = list_mom(mu,1)
         local_list(mu,2) = list_mom(mu,2)-pg(mu)
         local_list(mu,3) = eps(mu)
         local_list(mu,4) = list_mom(mu,2)
         local_list(mu,5) = list_mom(mu,3)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris2)
      ris2 = ris2 / st_tch_ccdotp(local_list(0,2),local_list(0,2)) /
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))


c emissione dalla gamba esterna st_tch_ket
      do mu=0,3
         local_list(mu,1) = list_mom(mu,1)
         local_list(mu,2) = list_mom(mu,2)-pg(mu)
         local_list(mu,3) = list_mom(mu,3)
         local_list(mu,4) = pket(mu)-pg(mu)
         local_list(mu,5) = eps(mu)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris3)
      ris3 = ris3 / st_tch_ccdotp(local_list(0,2),local_list(0,2))/
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))
      
      ris = ris1+ris2+ris3
      end

c this subroutine adds the emission of a couple of gluon/photon to a fermionic line 
c with a single attached current, preserving colour ordering of gluon emissions.
c It sums three graphs and attaches the right propagators too
C NO TRIPLE GLUON VERTEX HERE!!!
      subroutine st_tch_bra_slash_ket_g1_g2
     #     (st_tch_bra,st_tch_ket,helket,pbra,pket,pg1,eps1,pg2,eps2,jcurr,ris)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer helket
      real * 8 pbra(0:3),pket(0:3),pg1(0:3),eps1(0:3)
      real *8  pg2(0:3),eps2(0:3)
      complex * 16 jcurr(0:3)
      complex * 16 ris

      complex * 16 ris1,ris2,ris3
c      complex * 16 I
c      parameter (I=(0,1))
      complex*16 local_list(0:3,5)
      integer mu
      complex * 16 st_tch_ccdotp

c emission  <st_tch_bra| 1,2,j |st_tch_ket>
      do mu=0,3
         local_list(mu,1) = eps1(mu)
         local_list(mu,2) = pg1(mu)+pbra(mu)
         local_list(mu,3) = eps2(mu)
         local_list(mu,4) = pg1(mu)+pg2(mu)+pbra(mu)
         local_list(mu,5) = jcurr(mu)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris1)

      ris1 = ris1 / st_tch_ccdotp(local_list(0,2),local_list(0,2))/
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))



c emission <st_tch_bra|1,j,2|st_tch_ket> 
      do mu=0,3
         local_list(mu,1) = eps1(mu)
         local_list(mu,2) = pg1(mu)+pbra(mu)
         local_list(mu,3) = jcurr(mu)
         local_list(mu,4) = pket(mu)-pg2(mu)
         local_list(mu,5) = eps2(mu)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris2)
      ris2 = ris2 / st_tch_ccdotp(local_list(0,2),local_list(0,2)) /
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))


c emission <st_tch_bra|j,1,2|st_tch_ket>

      do mu=0,3
         local_list(mu,1) = jcurr(mu)
         local_list(mu,2) = pket(mu)-pg1(mu)-pg2(mu)
         local_list(mu,3) = eps1(mu)
         local_list(mu,4) = pket(mu)-pg2(mu)
         local_list(mu,5) = eps2(mu)
      enddo
      call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,helket,local_list,5,ris3)
      ris3 = ris3 / st_tch_ccdotp(local_list(0,2),local_list(0,2))/
     #     st_tch_ccdotp(local_list(0,4),local_list(0,4))
      
      ris = ris1+ris2+ris3
      end



c <st_tch_bra, helbra| slash_p_1 slash_p_2 ... slash_p_n |st_tch_ket, helket>
c where list_mom = [p_1, p_2,...., p_n]
c helket = -(-1)^n*helbra, otherwise the result is zero!!!
      subroutine st_tch_bra_slash_ket1(st_tch_bra,st_tch_ket,helbra,list_mom,n,ris)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer helket
      integer n
      complex * 16 list_mom(0:3,n)
c      dimension list_mom(0:3,n)
      complex * 16 ris

      complex * 16 b(2),k(2),new_bra(2)
      complex * 16 I
      parameter (I=(0,1))
      integer helbra,ii,hel
      
      helket = -(-1)**n*helbra
      hel = helbra
      b(1) = st_tch_bra(1,helbra)
      b(2) = st_tch_bra(2,helbra)
      k(1) = st_tch_ket(1,helket)
      k(2) = st_tch_ket(2,helket)
      
      do ii=1,n
         call st_tch_slash_bra(list_mom(0,ii),b,hel,new_bra)
         b(1) = new_bra(1)
         b(2) = new_bra(2)
         hel = -hel
      enddo
      ris = b(1)*k(1)+b(2)*k(2)     
      end



c the helicity of the st_tch_bra and the st_tch_ket are the same, otherwise this is zero!
c sig^mu = <st_tch_bra, helbra| gamma^mu |st_tch_ket, helket>
      subroutine st_tch_bra_gamma_ket(st_tch_bra,st_tch_ket,hel,sig)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      complex * 16 b(2),k(2)
      integer hel
      complex * 16 sig(0:3)
      complex * 16 I
      parameter (I=(0,1))

      b(1) = st_tch_bra(1,hel)
      b(2) = st_tch_bra(2,hel)
      k(1) = st_tch_ket(1,hel)
      k(2) = st_tch_ket(2,hel)

      sig(0) = b(1)*k(1)+k(2)*b(2)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c PLEASE NOTE the sign of the next components!!!!
c in fact  slash_p = gamma_0*p_0 - vec{gamma} dot vec(p)
c here we have just all the indexes up or down ==> the minus sign on the
c previous equation IS NOT there!!!
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      sig(1) = hel*(b(1)*k(2)+k(1)*b(2))
      sig(2) = hel*(-I*b(1)*k(2)+I*k(1)*b(2))
      sig(3) = hel*(b(1)*k(1)-k(2)*b(2))
      end


c this procedure attach a gluon/photon with polarization vector eps along 
c all the fermionic line  (NO COLOR STRUCTURE!!)
c the helicity of the st_tch_bra and the st_tch_ket are the same, otherwise this is zero!
c sig^mu = 1/st_tch_dotp(pbra+pglu,pbra+pglu) * 
c   <st_tch_bra, pbra, hel| eps_slash, (pbra+pglu)_slash, gamma^mu |st_tch_ket, pket, hel>+
c          1/st_tch_dotp(pket-pglu,pket-pglu) * 
c   <st_tch_bra, pbra, hel| gamma^mu, (pket-pglu)_slash, eps_slash |st_tch_ket, pket, hel>

      subroutine st_tch_bra_gamma_ket_gluon(st_tch_bra,st_tch_ket,hel,pbra,pket,pglu,eps,sig)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer hel
      real * 8 pbra(0:3),pket(0:3),pglu(0:3),eps(0:3)
      complex * 16 sig(0:3)
      complex * 16 I
      parameter (I=(0,1))
      complex*16 list_mom(0:3,3)
      real * 8 gamma(0:3),prop1,prop2,st_tch_dotp
      integer mu,nu
      complex * 16 J1(0:3),J2(0:3)


      do mu=0,3
         list_mom(mu,1) = eps(mu)
         list_mom(mu,2) = pglu(mu)+pbra(mu)
      enddo         

      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,3) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J1(mu))
      enddo



      do mu=0,3
         list_mom(mu,2) = pket(mu)-pglu(mu)
         list_mom(mu,3) = eps(mu)
      enddo         

      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,1) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J2(mu))
      enddo


      prop1 = 2*st_tch_dotp(pglu,pbra)
      prop2 = -2*st_tch_dotp(pglu,pket)

      do mu=0,3
         sig(mu) = J1(mu)/prop1 + J2(mu)/prop2
      enddo
       
      end


c This subroutine attaches a couple of gluon with polarization vector eps1, eps2 
c via a triple gluon vertex along  a fermionic line  (NO COLOR STRUCTURE!!) 
c with a single vertex inserted
      subroutine st_tch_bra_gamma_ket_3gv(st_tch_bra,st_tch_ket,hel,pbra,pket,p1,eps1,
     #                  p2,eps2,sig)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer hel
      real * 8 pbra(0:3),pket(0:3),p1(0:3),p2(0:3),
     #            eps1(0:3),eps2(0:3)
      complex * 16 sig(0:3)
      complex * 16 I
      parameter (I=(0,1))
      complex*16 list_mom(0:3,3)
      real * 8 gamma(0:3),prop1,prop2,st_tch_dotp
      integer mu,nu
      complex * 16 J1(0:3),J2(0:3)
      real *8 coeff1,coeff2,coeff3
      real *8 ptemp(0:3),jtemp(0:3)

      coeff1=2d0*st_tch_dotp(p1,eps2)+st_tch_dotp(p2,eps2)
      coeff2=-2d0*st_tch_dotp(p2,eps1)-st_tch_dotp(p1,eps1)
      coeff3=st_tch_dotp(eps1,eps2)

      do mu=0,3
         jtemp(mu)=coeff1*eps1(mu)+coeff2*eps2(mu)+
     #             coeff3*(p2(mu)-p1(mu))
         ptemp(mu)=p1(mu)+p2(mu)
      enddo   

      do mu=0,3
         list_mom(mu,1) = jtemp(mu)
         list_mom(mu,2) = p1(mu)+p2(mu)+pbra(mu)
      enddo         
      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,3) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J1(mu))
      enddo

      do mu=0,3
         list_mom(mu,2) = pket(mu)-p1(mu)-p2(mu)
         list_mom(mu,3) = jtemp(mu)
      enddo         

      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,1) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J2(mu))
      enddo

      prop1 = 2*st_tch_dotp(p1,p2)*(st_tch_dotp(ptemp,ptemp)+2*st_tch_dotp(pbra,ptemp))
      prop2 = 2*st_tch_dotp(p1,p2)*(st_tch_dotp(ptemp,ptemp)-2*st_tch_dotp(ptemp,pket))

      do mu=0,3
         sig(mu) = J1(mu)/prop1 + J2(mu)/prop2
      enddo
       
      end


c this procedure attaches a current along 
c all the fermionic line  (NO COLOR STRUCTURE!!)
c the helicity of the st_tch_bra and the st_tch_ket are the same, otherwise this is zero!
c sig^mu = 1/st_tch_dotp(pbra+pglu,pbra+pcurr) * 
c   <st_tch_bra, pbra, hel| j_slash, (pbra+pcurr)_slash, gamma^mu |st_tch_ket, pket, hel>+
c          1/st_tch_dotp(pket-pcurr,pket-pcurr) * 
c   <st_tch_bra, pbra, hel| gamma^mu, (pket-pcurr)_slash, j_slash |st_tch_ket, pket, hel>

      subroutine st_tch_bra_gamma_ket_curr(st_tch_bra,st_tch_ket,hel,pbra,
     # pket,pcurr,jcurr,sig)
      implicit none
      complex * 16 st_tch_bra(2,-1:1),st_tch_ket(2,-1:1)
      integer hel
      real * 8 pbra(0:3),pket(0:3),pcurr(0:3)
      complex *16 jcurr(0:3)
      complex * 16 sig(0:3)
      complex * 16 I
      parameter (I=(0,1))
      complex*16 list_mom(0:3,3)
      real * 8 gamma(0:3),prop1,prop2,st_tch_dotp
      integer mu,nu
      complex * 16 J1(0:3),J2(0:3)


      do mu=0,3
         list_mom(mu,1) = jcurr(mu)
         list_mom(mu,2) = pcurr(mu)+pbra(mu)
      enddo         
      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,3) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J1(mu))
      enddo



      do mu=0,3
         list_mom(mu,2) = pket(mu)-pcurr(mu)
         list_mom(mu,3) = jcurr(mu)
      enddo         

      do mu=0,3
         do nu=0,3
            gamma(nu)=0.d0
         enddo
         if (mu.eq.0) then
            gamma(mu) = 1.d0
         else
            gamma(mu) = -1.d0
         endif
         do nu=0,3
            list_mom(nu,1) = gamma(nu)
         enddo 
         call st_tch_bra_slash_ket(st_tch_bra,st_tch_ket,hel,list_mom,3,J2(mu))
      enddo


      prop1 = st_tch_dotp(pcurr,pcurr)*(st_tch_dotp(pcurr,pcurr)+2*st_tch_dotp(pcurr,pbra))
      prop2 = st_tch_dotp(pcurr,pcurr)*(st_tch_dotp(pcurr,pcurr)-2*st_tch_dotp(pcurr,pket))

      do mu=0,3
         sig(mu) = J1(mu)/prop1 + J2(mu)/prop2
      enddo
       
      end



      subroutine st_tch_exchange_momenta(p1,p2)
      implicit none
      real * 8 p1(0:3),p2(0:3)
      real * 8 tmp(0:3)
      integer mu
      do mu=0,3
         tmp(mu) = p1(mu)
         p1(mu) = p2(mu)
         p2(mu) = tmp(mu)
      enddo
      end


c return a st_tch_random number between 0 and 1
      function st_tch_rand_num()
      implicit none
      real * 8 st_tch_rand_num
      real * 8 st_tch_random2
      integer num,num2
      COMMON/st_tch_SEED/num,num2
c set st_tch_random seeds
      data NUM/12345/,NUM2/67890/
      st_tch_rand_num = st_tch_random2(num,num2)
      end
      
      subroutine st_tch_exchange_mom(p,i,j,dim,pnew)
      implicit none
      real * 8 p(0:3,*),pnew(0:3,*)
      integer i,j,dim
      integer mu,k
      real * 8 tmp(0:3)
      do k=1,dim
         do mu=0,3
            pnew(mu,k) = p(mu,k)
         enddo
      enddo

      do mu=0,3
         tmp(mu) = p(mu,j)
         pnew(mu,j) = p(mu,i)
         pnew(mu,i) = tmp(mu)
      enddo
      end
      

c*-- Author :    F. James, modified by Mike Seymour
C-----------------------------------------------------------------------
      FUNCTION st_tch_random2(iseed1,iseed2)
C     MAIN st_tch_random NUMBER GENERATOR
C     USES METHOD OF l'Ecuyer, (VIA F.JAMES, COMP PHYS COMM 60(1990)329)
C-----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION st_tch_random2
      INTEGER ISEED1,iseed2,K,IZ
      K=ISEED1/53668
      ISEED1=40014*(ISEED1-K*53668)-K*12211
      IF (ISEED1.LT.0) ISEED1=ISEED1+2147483563
      K=ISEED2/52774
      ISEED2=40692*(ISEED2-K*52774)-K*3791
      IF (ISEED2.LT.0) ISEED2=ISEED2+2147483399
      IZ=ISEED1-ISEED2
      IF (IZ.LT.1) IZ=IZ+2147483562
      st_tch_random2=DBLE(IZ)/2147483589
c      st_tch_random2=DBLE(IZ)*4.656613001013252D-10
C--->                (4.656613001013252D-10 = 1.D0/2147483589)
      end



