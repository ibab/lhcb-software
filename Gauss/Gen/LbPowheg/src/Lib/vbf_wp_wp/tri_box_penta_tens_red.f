c     This subroutine computes ALL the virtual corrections along the 
c     quark line depicted below and returns it as a Lorentz vector. Only 
c     TWO contractions with external currents (among jmu1, jmu2, jmu3) are done
c     
c
c      q,jmu1            t,jmu3
c        |                 |
c       \|/               \|/
c        |                 |
c -->-------------------------->---
c   p            |             p'
c               /|\
c                |
c              l,jmu2
c
c     p^2 = p'^2 = 0
c     
c     The notation for the external momenta is
c     
c     p + q + l + t = p' = pp
c     musq = mu^2 = common external scale (renormalization scale, eventually)
c     
c  
c     Only TWO external currents are passed to the subroutine
c     
c     Carlo Oleari    13/6/2005
c
      subroutine vbf_wp_wp_J_virtual_tri_box_pent(
c     vbf_wp_wp_bra(p'), vbf_wp_wp_ket(p) and helicity (of the vbf_wp_wp_ket fermion)
     #     brapp,ketp,hel,
c     momenta and squared renormalization scale
     #     p,q,l,t,pp,musq,
c     input currents (any couple among [J1,J2,J3]). ORDER IS IMPORTANT!!
c     They can be  jmu1,jmu2  or  jmu1,jmu3   or   jmu2,jmu3
     #     jal,jbe,
c     uncontracted external current. It can be 1, 2 or 3
     #     num_ret_current,
c     returned current
     #     Jret)

      implicit none
      complex * 16 brapp(2,-1:1),ketp(2,-1:1)
      integer hel
      real * 8 p(0:3),pp(0:3),q(0:3),l(0:3),t(0:3)
      real * 8 musq
      complex * 16 jal(0:3),jbe(0:3)
      integer num_ret_current
      complex * 16 jret(0:3)

      complex*16 jmu1(0:3),jmu2(0:3),jmu3(0:3)
      real * 8 psq, qsq, lsq, tsq,
     #     pq, pl, pt, ql, qt, lt, ppp, ppq, ppl
      real * 8 pq_sq,pql_sq,pqlt_sq,ql_sq,qlt_sq,lt_sq
      complex * 16 vbf_wp_wp_D04m_fin,vbf_wp_wp_D03m_fin,vbf_wp_wp_D02m_fin,vbf_wp_wp_C0fin,vbf_wp_wp_B0fin
      complex*16 E0
      complex * 16 B0_12,B0_13,B0_14,B0_15,B0_23,B0_24,B0_25,B0_34,
     #     B0_35,B0_45
      complex * 16 C0_345,C0_245,C0_235,C0_234,C0_145,C0_135,C0_134,
     #     C0_125,C0_124,C0_123
      complex * 16 Cij_345(2,4),Cij_245(2,4),Cij_235(2,4),Cij_234(2,4),
     #     Cij_145(2,4),Cij_135(2,4),Cij_134(2,4),Cij_125(2,4),
     #     Cij_124(2,4),Cij_123(2,4)
      complex * 16 D0_2345,D0_1345,D0_1245,D0_1235,D0_1234
      complex * 16 Dij_2345(3,13),Dij_1345(3,13),Dij_1245(3,13),
     #     Dij_1235(3,13),Dij_1234(3,13)
      complex * 16 E0_12345, Eij_12345(4,46)
      common/vbf_wp_wp_E_functions/E0_12345, Eij_12345
      common/vbf_wp_wp_D_functions/D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,
     #     Dij_2345,Dij_1345,Dij_1245,Dij_1235,Dij_1234
      common/vbf_wp_wp_C_functions/
     #     C0_345,C0_245,C0_235,C0_234,C0_145,
     #     C0_135,C0_134,C0_125,C0_124,C0_123,
     #     Cij_345,Cij_245,Cij_235,Cij_234,Cij_145,
     #     Cij_135,Cij_134,Cij_125,Cij_124,Cij_123
      common/vbf_wp_wp_B_functions/B0_13,B0_14,B0_24,B0_34,B0_23,B0_45,B0_35
      integer compon
      complex * 16 tri_box,vbf_wp_wp_virt_tri_box,pent,vbf_wp_wp_virt_pent

      logical debug
c      parameter (debug=.true.)
      parameter (debug=.false.)

      complex * 16 jref(0:3,0:3)
      integer mu
      real * 8 vbf_wp_wp_dotp

      psq = 0d0                 ! vbf_wp_wp_dotp(p,p)
      qsq = vbf_wp_wp_dotp(q,q)
      lsq = vbf_wp_wp_dotp(l,l)
      tsq = vbf_wp_wp_dotp(t,t)

      pq = vbf_wp_wp_dotp(p,q)
      pl = vbf_wp_wp_dotp(p,l)
      ql = vbf_wp_wp_dotp(q,l)
      qt = vbf_wp_wp_dotp(q,t)
      lt = vbf_wp_wp_dotp(l,t)

      pt = vbf_wp_wp_dotp(p,t)

      ppp = vbf_wp_wp_dotp(p,pp)
      ppq = vbf_wp_wp_dotp(pp,q)
      ppl = vbf_wp_wp_dotp(pp,l)

c (p+q)^2
      pq_sq = qsq+2*pq+psq
c (p+q+l)^2
      pql_sq = lsq+2*ql+2*pl+qsq+2*pq+psq
c (p+q+l+t)^2
      pqlt_sq =  0d0 ! tsq+2*lt+2*qt+2*pt+lsq+2*ql+2*pl+qsq+2*pq+psq
c (q+l)^2
      ql_sq = lsq+2*ql+qsq  
c (q+l+t)^2
      qlt_sq = tsq+2*lt+2*qt+lsq+2*ql+qsq
c (l+t)^2
      lt_sq = tsq+2*lt+lsq

      if (debug) then
         write(*,*) 'more prod scal'
         write(*,*) pq_sq,pql_sq,pqlt_sq,ql_sq,qlt_sq,lt_sq
      endif

c vbf_wp_wp_B0(p)
      B0_12 = 0d0 ! vbf_wp_wp_B0fin(psq,musq)
c vbf_wp_wp_B0(p+q)
      B0_13 = vbf_wp_wp_B0fin(pq_sq,musq)
c vbf_wp_wp_B0(p+q+l)
      B0_14 = vbf_wp_wp_B0fin(pql_sq,musq)
c vbf_wp_wp_B0(p+q+l+t)
      B0_15 = vbf_wp_wp_B0fin(pqlt_sq,musq)
c vbf_wp_wp_B0(q)
      B0_23 = vbf_wp_wp_B0fin(qsq,musq)
c vbf_wp_wp_B0(q+l)
      B0_24 = vbf_wp_wp_B0fin(ql_sq,musq)
c vbf_wp_wp_B0(q+l+t)
      B0_25 = vbf_wp_wp_B0fin(qlt_sq,musq)
c vbf_wp_wp_B0(l)
      B0_34 = vbf_wp_wp_B0fin(lsq,musq)
c vbf_wp_wp_B0(l+t)
      B0_35 = vbf_wp_wp_B0fin(lt_sq,musq)
c vbf_wp_wp_B0(t)
      B0_45 = vbf_wp_wp_B0fin(tsq,musq)

c vbf_wp_wp_C0(l,t)
      C0_345 = vbf_wp_wp_C0fin(lsq,tsq,lt_sq,musq)
c vbf_wp_wp_C0(q+l,t)
      C0_245 = vbf_wp_wp_C0fin(ql_sq,tsq,qlt_sq,musq)
c vbf_wp_wp_C0(q,l+t)
      C0_235 = vbf_wp_wp_C0fin(qsq,lt_sq,qlt_sq,musq)
c vbf_wp_wp_C0(q,l)
      C0_234 = vbf_wp_wp_C0fin(qsq,lsq,ql_sq,musq)
c vbf_wp_wp_C0(p+q+l,t)
      C0_145 = vbf_wp_wp_C0fin(pql_sq,tsq,pqlt_sq,musq)
c vbf_wp_wp_C0(p+q,l+t)
      C0_135 = vbf_wp_wp_C0fin(pq_sq,lt_sq,pqlt_sq,musq)
c vbf_wp_wp_C0(p+q,l)
      C0_134 = vbf_wp_wp_C0fin(pq_sq,lsq,pql_sq,musq)
c vbf_wp_wp_C0(p,q+l+t)
      C0_125 = vbf_wp_wp_C0fin(psq,qlt_sq,pqlt_sq,musq)
c vbf_wp_wp_C0(p,q+l)
      C0_124 = vbf_wp_wp_C0fin(psq,ql_sq,pql_sq,musq)
c vbf_wp_wp_C0(p,q)
      C0_123 = vbf_wp_wp_C0fin(psq,qsq,pq_sq,musq)

c Cij_345
      call vbf_wp_wp_tens_red3(lsq,tsq,lt_sq, B0_45, B0_35, B0_34, 
     #     C0_345, Cij_345)
c Cij_245
      call vbf_wp_wp_tens_red3(ql_sq,tsq,qlt_sq, B0_45, B0_25, B0_24, 
     #     C0_245, Cij_245)
c Cij_235
      call vbf_wp_wp_tens_red3(qsq,lt_sq,qlt_sq, B0_35, B0_25, B0_23, 
     #     C0_235, Cij_235)
c Cij_234
      call vbf_wp_wp_tens_red3(qsq,lsq,ql_sq, B0_34, B0_24, B0_23, 
     #     C0_234, Cij_234)
c Cij_145
      call vbf_wp_wp_tens_red3(pql_sq,tsq,pqlt_sq, B0_45, B0_15, B0_14, 
     #     C0_145, Cij_145)
c Cij_135
      call vbf_wp_wp_tens_red3(pq_sq,lt_sq,pqlt_sq, B0_35, B0_15, B0_13, 
     #     C0_135, Cij_135)
c Cij_134
      call vbf_wp_wp_tens_red3(pq_sq,lsq,pql_sq, B0_34, B0_14, B0_13, 
     #     C0_134, Cij_134)
c Cij_125
      call vbf_wp_wp_tens_red3(psq,qlt_sq,pqlt_sq, B0_25, B0_15, B0_12, 
     #     C0_125, Cij_125)
c Cij_124
      call vbf_wp_wp_tens_red3(psq,ql_sq,pql_sq, B0_24, B0_14, B0_12, 
     #     C0_124, Cij_124)
c Cij_123
      call vbf_wp_wp_tens_red3(psq,qsq,pq_sq, B0_23, B0_13, B0_12, 
     #     C0_123, Cij_123)

c      call vbf_wp_wp_print_Cij(C0_123,Cij_123)
c      call vbf_wp_wp_print_Cij(C0_245,Cij_245)

c D0_2345 = D00(q,l,t)
c      D0_2345 = D0(qsq,lsq,tsq,ql,qt,lt)
c the following two expressions are equivalent
      D0_2345 = vbf_wp_wp_D04m_fin(lt_sq,ql_sq,qlt_sq,qsq,lsq,tsq,musq)
c      write(*,*) 'D0_2345',D0_2345
c      D0_2345 = vbf_wp_wp_D04m_fin(ql_sq,lt_sq,qsq,lsq,tsq,qlt_sq,musq)
c      write(*,*) 'D0_2345',D0_2345
      
c D0_1345 = D00(p+q,l,t)         
c      D0_1345 = D0(pq_sq,lsq,tsq,ql+pl,qt+pt,lt)
      D0_1345 = vbf_wp_wp_D03m_fin(lt_sq,pql_sq,pq_sq,lsq,tsq,musq)
c      write(*,*) 'D0_1345',D0_1345

c D0_1245 = D00(p,q+l,t) 
c      D0_1245 = D0(psq,ql_sq,tsq,pl+pq,pt,lt+qt)
      D0_1245 = vbf_wp_wp_D02m_fin(qlt_sq,pql_sq,ql_sq,tsq,musq)
c      write(*,*) 'D0_1245',D0_1245

c D0_1235 = D00(p,q,l+t)  
c      D0_1235 = D0(psq,qsq,lt_sq,pq,pt+pl,qt+ql)
      D0_1235 = vbf_wp_wp_D02m_fin(qlt_sq,pq_sq,qsq,lt_sq,musq)
c      write(*,*) 'D0_1235',D0_1235

c D0_1234 = D00(p,q,l)
c      D0_1234 = D0(psq,qsq,lsq,pq,pl,ql)
      D0_1234 = vbf_wp_wp_D03m_fin(ql_sq,pq_sq,pql_sq,lsq,qsq,musq)
c      write(*,*) 'D0_1234',D0_1234

c Dij_2345
      call vbf_wp_wp_tens_red4(qsq,lsq,tsq,ql,qt,lt,
     #     C0_345, C0_245, C0_235, C0_234,   
     #     Cij_345, Cij_245, Cij_235, Cij_234,   
     #     D0_2345, 
     #     Dij_2345)

c Dij_1345
      call vbf_wp_wp_tens_red4(pq_sq,lsq,tsq,ql+pl,qt+pt,lt,
     #     C0_345, C0_145, C0_135, C0_134,   
     #     Cij_345, Cij_145, Cij_135, Cij_134,   
     #     D0_1345, 
     #     Dij_1345)

c Dij_1245
      call vbf_wp_wp_tens_red4(psq,ql_sq,tsq,pl+pq,pt,lt+qt,
     #     C0_245, C0_145, C0_125, C0_124,   
     #     Cij_245, Cij_145, Cij_125, Cij_124,   
     #     D0_1245, 
     #     Dij_1245)

c Dij_1235
      call vbf_wp_wp_tens_red4(psq,qsq,lt_sq,pq,pt+pl,qt+ql,
     #     C0_235, C0_135, C0_125, C0_123,   
     #     Cij_235, Cij_135, Cij_125, Cij_123,   
     #     D0_1235, 
     #     Dij_1235)

c Dij_1234
      call vbf_wp_wp_tens_red4(psq,qsq,lsq,pq,pl,ql,
     #     C0_234, C0_134, C0_124, C0_123,   
     #     Cij_234, Cij_134, Cij_124, Cij_123,   
     #     D0_1234, 
     #     Dij_1234)

c      call vbf_wp_wp_print_Dij(D0_1234,Dij_1234)
c      call vbf_wp_wp_print_Dij(D0_2345,Dij_2345)      
c      call vbf_wp_wp_print_Dij(D0_1345,Dij_1345)      
c      call vbf_wp_wp_print_Dij(D0_1245,Dij_1245)      
c      call vbf_wp_wp_print_Dij(D0_1235,Dij_1235)      

c E0_12345 = E0(p,q,l,t)
      E0_12345 = E0(psq, qsq, lsq, tsq,
     #     pq, pl, pt, ql, qt, lt, 
     #     D0_2345,D0_1345,D0_1245,D0_1235,D0_1234)
c      write(*,*) 'E0',E0_12345

c Eij_12345
      call vbf_wp_wp_tens_red5(psq, qsq, lsq, tsq,
     #     pq, pl, pt, ql, qt, lt, 
     #     D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,
     #     Dij_2345,Dij_1345,Dij_1245,Dij_1235,Dij_1234,
     #     E0_12345, 
     #     Eij_12345)

      if (debug) then
         write(*,*) 'E0_12345' 
         write(*,*) E0_12345
         write(*,*) 'D0_2345,D0_1345,D0_1245,D0_1235,D0_1234'
         write(*,*) D0_2345,D0_1345,D0_1245,D0_1235,D0_1234
         write(*,*) 'C0_345,C0_245,C0_235,C0_234,C0_145,'//
     #        'C0_135,C0_134,C0_125,C0_124,C0_123'
         write(*,*) C0_345,C0_245,C0_235,C0_234,C0_145,
     #        C0_135,C0_134,C0_125,C0_124,C0_123
         write(*,*) 'B0_12,B0_13,B0_14,B0_15,B0_23,B0_24,B0_25,B0_34,'//
     #        'B0_35,B0_45'
         write(*,*) B0_12,B0_13,B0_14,B0_15,B0_23,B0_24,B0_25,B0_34,
     #        B0_35,B0_45
      endif
      
c      call vbf_wp_wp_print_Cij(C0_123,Cij_123)
c      call vbf_wp_wp_print_Dij(D0_2345,Dij_2345)
c      call vbf_wp_wp_print_Eij(E0_12345,Eij_12345)            
c      write(*,*) qsq,lsq,pq,pl,ppp,ppq,ppl,ql
      
      jref(0,0)=1d0 
      jref(0,1)=0d0
      jref(0,2)=0d0
      jref(0,3)=0d0
      
      jref(1,0)=0d0
      jref(1,1)=-1d0
      jref(1,2)=0d0
      jref(1,3)=0d0
      
      jref(2,0)=0d0
      jref(2,1)=0d0
      jref(2,2)=-1d0
      jref(2,3)=0d0

      jref(3,0)=0d0
      jref(3,1)=0d0
      jref(3,2)=0d0
      jref(3,3)=-1d0
      
      if  (num_ret_current.eq.1) then
         do mu=0,3
            jmu2(mu)=jal(mu)
            jmu3(mu)=jbe(mu)
         enddo
         do compon=0,3
            do mu=0,3
               jmu1(mu) = jref(compon,mu)
            enddo
            call vbf_wp_wp_compute_Jprod_gammaprod(
     #           brapp,ketp,hel,
     #           p,q,l,t,pp,
     #           jmu1,jmu2,jmu3)
            tri_box = vbf_wp_wp_virt_tri_box(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            pent    =    vbf_wp_wp_virt_pent(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            Jret(compon) = tri_box + pent
         enddo
      elseif  (num_ret_current.eq.2) then
         do mu=0,3
            jmu1(mu)=jal(mu)
            jmu3(mu)=jbe(mu)
         enddo
         do compon=0,3
            do mu=0,3
               jmu2(mu) = jref(compon,mu)
            enddo
            call vbf_wp_wp_compute_Jprod_gammaprod(
     #           brapp,ketp,hel,
     #           p,q,l,t,pp,
     #           jmu1,jmu2,jmu3)
            tri_box = vbf_wp_wp_virt_tri_box(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            pent    =    vbf_wp_wp_virt_pent(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            Jret(compon) = tri_box + pent
         enddo
      elseif  (num_ret_current.eq.3) then
         do mu=0,3
            jmu1(mu)=jal(mu)
            jmu2(mu)=jbe(mu)
         enddo
         do compon=0,3
            do mu=0,3
               jmu3(mu) = jref(compon,mu)
            enddo
            call vbf_wp_wp_compute_Jprod_gammaprod(
     #           brapp,ketp,hel,
     #           p,q,l,t,pp,
     #           jmu1,jmu2,jmu3)
            tri_box = vbf_wp_wp_virt_tri_box(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            pent    =    vbf_wp_wp_virt_pent(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
            Jret(compon) = tri_box + pent
         enddo
      else
         write(*,*) 'WRONG returned current',num_ret_current
         stop
      endif
      end


c     Compute J.p_i, where J's are the external currents, and all the 
c     <vbf_wp_wp_bra|...|vbf_wp_wp_ket> gamma prods
c     The returned values are in the common blocks
c
c  Carlo Oleari    13/6/2005
c
      subroutine vbf_wp_wp_compute_Jprod_gammaprod(
c     vbf_wp_wp_bra(p'), vbf_wp_wp_ket(p) and helicity 
     #     vbf_wp_wp_bra,vbf_wp_wp_ket,hel,
c     momenta
     #     p,q,l,t,pp,
c     input currents
     #     jmu1,jmu2,jmu3)
      implicit none
      complex * 16 vbf_wp_wp_bra(2,-1:1),vbf_wp_wp_ket(2,-1:1)
      integer hel
      real * 8 p(0:3),pp(0:3),q(0:3),l(0:3),t(0:3)
      complex*16 jmu1(0:3),jmu2(0:3),jmu3(0:3)

      complex * 16 cq(0:3),cl(0:3)
      complex * 16 dpmu1,dpmu2,dpmu3,dqmu1,dqmu2,dqmu3
      complex * 16 dlmu1,dlmu2,dlmu3,dppmu1,dppmu2,dppmu3
      complex * 16 dmu1mu2,dmu1mu3,dmu2mu3
      common/vbf_wp_wp_dotp_pol_vec/dpmu1,dpmu2,dpmu3,dqmu1,dqmu2,dqmu3,
     #     dlmu1,dlmu2,dlmu3,dppmu1,dppmu2,dppmu3,
     #     dmu1mu2,dmu1mu3,dmu2mu3
      complex * 16 gp_mu3_l_mu2_q_mu1,gp_l,gp_mu2,gp_mu3_l_q
      complex * 16 gp_mu2_q_mu1,gp_mu3_mu2_q,gp_q,gp_mu3_l_mu2
      complex * 16 gp_l_mu2_q,gp_mu3_mu2_mu1,gp_mu3_l_mu1,gp_mu3
      complex * 16 gp_mu1,gp_l_q_mu1,gp_mu3_q_mu1,gp_l_mu2_mu1
      common/vbf_wp_wp_gammaprods/gp_mu3_l_mu2_q_mu1,gp_l,gp_mu2,gp_mu3_l_q,
     #     gp_mu2_q_mu1,gp_mu3_mu2_q,gp_q,gp_mu3_l_mu2,
     #     gp_l_mu2_q,gp_mu3_mu2_mu1,gp_mu3_l_mu1,gp_mu3,
     #     gp_mu1,gp_l_q_mu1,gp_mu3_q_mu1,gp_l_mu2_mu1
c      
      complex * 16 local_mom(0:3,5), vbf_wp_wp_cdotp, vbf_wp_wp_ccdotp
      integer al
      
      logical debug
c      parameter (debug=.true.)
      parameter (debug=.false.)

      dpmu1 = vbf_wp_wp_cdotp(p,jmu1)
      dpmu2 = vbf_wp_wp_cdotp(p,jmu2)
      dpmu3 = vbf_wp_wp_cdotp(p,jmu3)

      dqmu1 = vbf_wp_wp_cdotp(q,jmu1)
      dqmu2 = vbf_wp_wp_cdotp(q,jmu2)
      dqmu3 = vbf_wp_wp_cdotp(q,jmu3)

      dlmu1 = vbf_wp_wp_cdotp(l,jmu1)
      dlmu2 = vbf_wp_wp_cdotp(l,jmu2)
      dlmu3 = vbf_wp_wp_cdotp(l,jmu3)

      dppmu1 = vbf_wp_wp_cdotp(pp,jmu1)
      dppmu2 = vbf_wp_wp_cdotp(pp,jmu2)
      dppmu3 = vbf_wp_wp_cdotp(pp,jmu3)

      dmu1mu2 = vbf_wp_wp_ccdotp(jmu1,jmu2)
      dmu1mu3 = vbf_wp_wp_ccdotp(jmu1,jmu3)
      dmu2mu3 = vbf_wp_wp_ccdotp(jmu2,jmu3)
      
      do al=0,3
         cl(al) = l(al)
         cq(al) = q(al)
      enddo

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = cl(al)
         local_mom(al,3) = jmu2(al)
         local_mom(al,4) = cq(al)
         local_mom(al,5) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,5,gp_mu3_l_mu2_q_mu1)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = cl(al)
         local_mom(al,3) = cq(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_l_q)

      do al=0,3
         local_mom(al,1) = jmu2(al)
         local_mom(al,2) = cq(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu2_q_mu1)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = jmu2(al)
         local_mom(al,3) = cq(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_mu2_q)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = cl(al)
         local_mom(al,3) = jmu2(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_l_mu2)

      do al=0,3
         local_mom(al,1) = cl(al)
         local_mom(al,2) = jmu2(al)
         local_mom(al,3) = cq(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_l_mu2_q)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = jmu2(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_mu2_mu1)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = cl(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_l_mu1)

      do al=0,3
         local_mom(al,1) = cl(al)
         local_mom(al,2) = cq(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_l_q_mu1)

      do al=0,3
         local_mom(al,1) = jmu3(al)
         local_mom(al,2) = cq(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_mu3_q_mu1)

      do al=0,3
         local_mom(al,1) = cl(al)
         local_mom(al,2) = jmu2(al)
         local_mom(al,3) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,3,gp_l_mu2_mu1)

      do al=0,3
         local_mom(al,1) = cl(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,1,gp_l)

      do al=0,3
         local_mom(al,1) = jmu2(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,1,gp_mu2)

      do al=0,3
         local_mom(al,1) = cq(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,1,gp_q)

      do al=0,3
         local_mom(al,1) = jmu3(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,1,gp_mu3)

      do al=0,3
         local_mom(al,1) = jmu1(al)
      enddo
      call vbf_wp_wp_bra_slash_ket(vbf_wp_wp_bra,vbf_wp_wp_ket,hel,local_mom,1,gp_mu1)

      if (debug) then
         write(*,*) 'gamma prods'
         write(*,*) gp_l,gp_q,gp_mu2,gp_mu3,gp_mu3_l_mu2,gp_mu3_l_q,
     #        gp_mu3_mu2_q,gp_mu3_l_mu2,gp_l_mu2_q
         
         write(*,*) 
         write(*,*) gp_mu3_l_mu2_q_mu1,gp_mu2_q_mu1,gp_mu3_mu2_mu1,
     #        gp_mu3_l_mu1,gp_l_q_mu1,gp_l_mu2_mu1,gp_mu3_q_mu1,gp_mu1
      endif

      if (debug) then
         write(*,*) gp_mu3_l_mu2_q_mu1,gp_l,gp_mu2,gp_mu3_l_q,
     #        gp_mu2_q_mu1,gp_mu3_mu2_q,gp_q,gp_mu3_l_mu2,
     #        gp_l_mu2_q,gp_mu3_mu2_mu1,gp_mu3_l_mu1,gp_mu3,
     #        gp_mu1,gp_l_q_mu1,gp_mu3_q_mu1,gp_l_mu2_mu1
      endif
      end



c     Given casus and qqaux, the subroutine returns the 
c     right ordered q, l and t
c     See subroutine for details
c
c     Carlo Oleari    13/6/2005
c
      subroutine vbf_wp_wp_from_qqaux_to_qlt(casus,qqaux,q,l,t)
      implicit none
      integer casus
      real * 8 qqaux(0:3,3)
      real * 8 q(0:3),l(0:3),t(0:3)
      integer al

      if (casus.eq.1) then
         do al=0,3
            q(al) = qqaux(al,1)
            l(al) = qqaux(al,2)
            t(al) = qqaux(al,3)
         enddo
      elseif (casus.eq.2)then
         do al=0,3
            q(al) = qqaux(al,1)
            t(al) = qqaux(al,2)
            l(al) = qqaux(al,3)
         enddo
      elseif (casus.eq.3)then
         do al=0,3
            l(al) = qqaux(al,1)
            t(al) = qqaux(al,2)
            q(al) = qqaux(al,3)
         enddo
      endif
      end

c     Returns the azimuthal angle between the vector and the y axis
c     in the xy plane
      function vbf_wp_wp_azimuth_vec_y(p)
      implicit none
      real * 8 vbf_wp_wp_azimuth_vec_y,p(0:3)
      real * 8 vbf_wp_wp_azimuth_vec_x,phi_x
      real * 8 pi
      parameter (pi=3.141592653589793238462643383279502884197D0)
      phi_x = vbf_wp_wp_azimuth_vec_x(p)      
      if ((-pi.lt.phi_x).and.(phi_x.lt.-pi/2)) then
         vbf_wp_wp_azimuth_vec_y = 3*pi/2 + phi_x 
      else         
         vbf_wp_wp_azimuth_vec_y = phi_x - pi/2
      endif
      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

c     returns the azimuthal angle between the vector and the x axis
      function vbf_wp_wp_azimuth_vec_x(p)
      implicit none
      real * 8 vbf_wp_wp_azimuth_vec_x,p(0:3)
      real * 8 phi
      real * 8 pi
      parameter (pi=3.141592653589793238462643383279502884197D0)      
      real * 8 tiny
      parameter (tiny = 1d-13)
      
      if ((abs(p(1)).lt.tiny).and.(abs(p(2)).lt.tiny)) then
         vbf_wp_wp_azimuth_vec_x = 0d0
         return
      endif

      if (abs(p(1)).lt.tiny) then
         if (p(2).gt.0d0) then
            phi = pi/2
         else
            phi = -pi/2
         endif
      else
         phi = atan(p(2)/p(1))
         if (p(1).lt.0d0) then
            if (phi.gt.0d0) then               
               phi = phi - pi
            else
               phi = phi + pi
            endif
         endif         
      endif
      vbf_wp_wp_azimuth_vec_x = phi
      end



      subroutine vbf_wp_wp_rotatez(vbf_wp_wp_theta,p,pp)
      implicit none
      real * 8 p(0:3),pp(0:3),vbf_wp_wp_theta
      pp(0) = p(0)
      pp(1) = p(1)*cos(vbf_wp_wp_theta) - p(2)*sin(vbf_wp_wp_theta)
      pp(2) = p(1)*sin(vbf_wp_wp_theta) + p(2)*cos(vbf_wp_wp_theta)
      pp(3) = p(3)
      end
      
      subroutine vbf_wp_wp_rotatex(vbf_wp_wp_theta,p,pp)
      implicit none
      real * 8 p(0:3),pp(0:3),vbf_wp_wp_theta
      pp(0) = p(0)
      pp(1) = p(1)
      pp(2) = p(2)*cos(vbf_wp_wp_theta) - p(3)*sin(vbf_wp_wp_theta)
      pp(3) = p(2)*sin(vbf_wp_wp_theta) + p(3)*cos(vbf_wp_wp_theta)
      end
      
      subroutine vbf_wp_wp_rotatey(vbf_wp_wp_theta,p,pp)
      implicit none
      real * 8 p(0:3),pp(0:3),vbf_wp_wp_theta
      pp(0) = p(0)
      pp(1) = p(1)*cos(vbf_wp_wp_theta) + p(3)*sin(vbf_wp_wp_theta)
      pp(2) = p(2)      
      pp(3) = -p(1)*sin(vbf_wp_wp_theta) + p(3)*cos(vbf_wp_wp_theta)
      end
      

      subroutine vbf_wp_wp_crotatez(vbf_wp_wp_theta,p,pp)
      implicit none
      real * 8 vbf_wp_wp_theta
      complex * 16 p(0:3),pp(0:3)
      pp(0) = p(0)
      pp(1) = p(1)*cos(vbf_wp_wp_theta) - p(2)*sin(vbf_wp_wp_theta)
      pp(2) = p(1)*sin(vbf_wp_wp_theta) + p(2)*cos(vbf_wp_wp_theta)
      pp(3) = p(3)
      end
      
      subroutine vbf_wp_wp_crotatex(vbf_wp_wp_theta,p,pp)
      implicit none
      real * 8 vbf_wp_wp_theta
      complex * 16 p(0:3),pp(0:3)
      pp(0) = p(0)
      pp(1) = p(1)
      pp(2) = p(2)*cos(vbf_wp_wp_theta) - p(3)*sin(vbf_wp_wp_theta)
      pp(3) = p(2)*sin(vbf_wp_wp_theta) + p(3)*cos(vbf_wp_wp_theta)
      end
      

      function vbf_wp_wp_theta(x)
      implicit none
      real * 8 vbf_wp_wp_theta,x
      if (x.gt.0d0) then
         vbf_wp_wp_theta = 1d0
      else
         vbf_wp_wp_theta = 0d0
      endif
      end

      function vbf_wp_wp_sgn(x)
      implicit none
      real * 8 vbf_wp_wp_sgn,x
      if (x.gt.0d0) then
         vbf_wp_wp_sgn = +1d0
      else
         vbf_wp_wp_sgn = -1d0
      endif
      end


CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCC                SCALAR INTEGRALS                CCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

c    ***********   vbf_wp_wp_D02m_fin(s,t,m3sq,m4sq,musq)   *************
c
c Scalar box with MASSLESS PROPAGATORS and with external kinematics: 
c
c                     k
c     p1 ->-----------<-------------<-- p4
c             |                 |
c             |                 |
c             |                 |  
c     p2 ->-------------------------<-- p3
c
c
c    s = (p1+p2)^2
c    t = (p2+p3)^2
c    p1^2 = 0, p2^2 = 0, p3^2 = m3sq <>0, p4^2 = m4sq <>0
c    musq = mu^2 = reference dimensional scale 
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2/(k+p1+p2+p3)^2 = 
c          N_ep * vbf_wp_wp_D02m_fin(s,t,m3sq,m4sq,musq);
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)

      function vbf_wp_wp_D02m_fin(s,t,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D02m_fin
      real * 8 s,t,m3sq,m4sq,musq
      real * 8 ms,mt,mm3sq,mm4sq
      complex * 16 lnms,lnmt,lnmm3sq,lnmm4sq,lnsot
      complex * 16 li2arg1,li2arg2
      real * 8 arg1,arg2
      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      complex * 16 ris
      real * 8 prefactor, vbf_wp_wp_theta, vbf_wp_wp_DILOG, vbf_wp_wp_im_part
      
      if (musq.lt.0d0) then
         write(*,*) 
     #        'POSSIBLE ERROR IN vbf_wp_wp_D02m_fin: SCALE MUSQ LESS THAN ZERO!!'
      endif

      prefactor = 1d0/(s*t)
      ms = -s/musq
      mt = -t/musq
      mm3sq = - m3sq/musq
      mm4sq = - m4sq/musq
      
      lnms = log(abs(ms)) - ipi*vbf_wp_wp_theta(-ms)
      lnmt = log(abs(mt)) - ipi*vbf_wp_wp_theta(-mt)
      lnmm3sq = log(abs(mm3sq)) - ipi*vbf_wp_wp_theta(-mm3sq)
      lnmm4sq = log(abs(mm4sq)) - ipi*vbf_wp_wp_theta(-mm4sq)
      lnsot = log(abs(s/t)) + ipi * vbf_wp_wp_im_part(s,t)

      arg1 = 1-m3sq/t
      arg2 = 1-m4sq/t

c     (m3sq/t.lt.0d0)
      if (arg1.gt.1d0) then
         li2arg1 = -vbf_wp_wp_DILOG(1d0/arg1) - log(arg1)**2/2+pi2/3 
     #        - ipi*log(arg1)*vbf_wp_wp_im_part(m3sq,t)
      else
         li2arg1 = vbf_wp_wp_DILOG(arg1)
      endif

c     (m4sq/t.lt.0d0)
      if (arg2.gt.1d0)  then
         li2arg2 = -vbf_wp_wp_DILOG(1d0/arg2) - log(arg2)**2/2+pi2/3 
     #        - ipi*log(arg2)*vbf_wp_wp_im_part(m4sq,t)
      else
         li2arg2 = vbf_wp_wp_DILOG(arg2)
      endif

      ris = lnms**2/2+lnmt**2-lnmm3sq**2/2-lnmm4sq**2/2
     #     +lnmm3sq*lnmm4sq-(lnmm4sq+lnmm3sq-lnms)*lnms
     #     -2*li2arg1-2*li2arg2-lnsot**2 - pi2/6
        
      vbf_wp_wp_D02m_fin = prefactor * ris
      end




c    ***********   vbf_wp_wp_D03m_fin(s,t,m2sq,m3sq,m4sq,musq)   *************
c
c Scalar box with MASSLESS PROPAGATORS and with external kinematics: 
c
c
c                     k
c     p1 ->-----------<-------------<-- p4
c             |                 |
c             |                 |
c             |                 |  
c     p2 ->-------------------------<-- p3
c
c
c    s = (p1+p2)^2
c    t = (p2+p3)^2
c    p1^2 = 0, p2^2 = m2sq <>0, p3^2 = m3sq <>0, p4^2 = m4sq <>0
c    musq = mu^2 = reference dimensional scale 
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2/(k+p1+p2+p3)^2 =  
c          N_ep * vbf_wp_wp_D03m_fin(s,t,m2sq,m3sq,m4sq,musq);
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)

      function vbf_wp_wp_D03m_fin(s,t,m2sq,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D03m_fin
      real * 8 s,t,m2sq,m3sq,m4sq,musq
      complex * 16 vbf_wp_wp_D03m_fin_BDK,vbf_wp_wp_D03m_fin_DN
      vbf_wp_wp_D03m_fin = vbf_wp_wp_D03m_fin_BDK(s,t,m2sq,m3sq,m4sq,musq)
c      vbf_wp_wp_D03m_fin = vbf_wp_wp_D03m_fin_DN(s,t,m2sq,m3sq,m4sq,musq)
      
      end




      function vbf_wp_wp_D03m_fin_BDK(s,t,m2sq,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D03m_fin_BDK
      real * 8 s,t,m2sq,m3sq,m4sq,musq
      real * 8 ms,mt,mm2sq,mm3sq,mm4sq,sot
      complex * 16 lnms,lnmt,lnmm2sq,lnmm3sq,lnmm4sq,lnsot,lnarg3
      complex * 16 li2arg1,li2arg2,li2arg3
      real * 8 arg1,arg2,arg3
      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      complex * 16 ris
      real * 8 prefactor, vbf_wp_wp_theta, vbf_wp_wp_DILOG, vbf_wp_wp_sgn, vbf_wp_wp_im_part
      real * 8 im_a, im_b,im_ab, vbf_wp_wp_eta3

      if (musq.lt.0d0) then
         write(*,*) 
     #        'POSSIBLE ERROR IN vbf_wp_wp_D03m_fin: SCALE MUSQ LESS THAN ZERO!!'
      endif


c  1/(ms*mt-mm2s*mm4s)*
c  (1/2*ln(ms)^2+1/2*ln(mt)^2-1/2*ln(mm2s)^2-1/2*ln(mm4s)^2-ln(ms/mt)^2
c  +ln(mm2s)*ln(mm3s)-(ln(mm3s)+ln(mm2s)-ln(mt))*ln(mt)
c  +ln(mm3s)*ln(mm4s)-(ln(mm4s)+ln(mm3s)-ln(ms))*ln(ms)
c  -2*vbf_wp_wp_li2(1-mm2s/ms)
c  -2*vbf_wp_wp_li2(1-mm4s/mt)
c  +2*vbf_wp_wp_li2(1-mm2s*mm4s/ms/mt))

      prefactor = 1d0/(s*t-m2sq*m4sq)
      ms = -s/musq
      mt = -t/musq
      mm2sq = - m2sq/musq
      mm3sq = - m3sq/musq
      mm4sq = - m4sq/musq
      sot = s/t
      
      lnms = log(abs(ms)) - ipi*vbf_wp_wp_theta(-ms)
      lnmt = log(abs(mt)) - ipi*vbf_wp_wp_theta(-mt)
      lnmm2sq = log(abs(mm2sq)) - ipi*vbf_wp_wp_theta(-mm2sq)
      lnmm3sq = log(abs(mm3sq)) - ipi*vbf_wp_wp_theta(-mm3sq)
      lnmm4sq = log(abs(mm4sq)) - ipi*vbf_wp_wp_theta(-mm4sq)
      lnsot = log(abs(sot)) + ipi * vbf_wp_wp_im_part(s,t)

     
      arg1 = 1-m2sq/s
      arg2 = 1-m4sq/t
      arg3 = 1-m2sq*m4sq/s/t
      

c     m2sq/s < 0 
      if (arg1.gt.1d0) then
         li2arg1 = -vbf_wp_wp_DILOG(1d0/arg1) - log(arg1)**2/2+pi2/3 
     #        - ipi*log(arg1)*vbf_wp_wp_im_part(m2sq,s)
      else
         li2arg1 = vbf_wp_wp_DILOG(arg1)
      endif

c     m4sq/t < 0
      if (arg2.gt.1d0) then
         li2arg2 = -vbf_wp_wp_DILOG(1d0/arg2) - log(arg2)**2/2+pi2/3 
     #        - ipi*log(arg2)*vbf_wp_wp_im_part(m4sq,t)
      else
         li2arg2 = vbf_wp_wp_DILOG(arg2)
      endif


      im_a = m2sq/s*(1d0/m2sq-1d0/s)
      im_b = m4sq/t*(1d0/m4sq-1d0/t)
      im_ab = m2sq*m4sq/s/t*(1d0/m2sq-1d0/s+1d0/m4sq-1d0/t)


c     m2sq*m4sq/(s*t)  < 0
      if (arg3.gt.1d0) then
         li2arg3 = -vbf_wp_wp_DILOG(1d0/arg3) - log(arg3)**2/2+pi2/3 
     #        - ipi*log(arg3)*vbf_wp_wp_sgn(im_ab)
      else
         li2arg3 = vbf_wp_wp_DILOG(arg3)
      endif

      if (arg3.lt.0d0) then
         lnarg3 = log(abs(arg3)) - ipi*vbf_wp_wp_sgn(im_ab)
      else
         lnarg3 = log(arg3)
      endif
      
      li2arg3 = li2arg3  + 2*ipi*vbf_wp_wp_eta3(im_a,im_b,im_ab)*lnarg3

      
      ris = lnms**2/2+lnmt**2/2-lnmm2sq**2/2-lnmm4sq**2/2
     #     +lnmm2sq*lnmm3sq
     #     -(lnmm3sq+lnmm2sq-lnmt)*lnmt+lnmm3sq*lnmm4sq
     #     -(lnmm4sq+lnmm3sq-lnms)*lnms      
      
      ris = ris - 2*li2arg1 - 2*li2arg2 + 2*li2arg3 
     #     - lnsot**2
        
      vbf_wp_wp_D03m_fin_BDK = prefactor * ris
      end

c from hep-ph/0006249 by Duplancic and Nizic
      function vbf_wp_wp_D03m_fin_DN(s,t,m2sq,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D03m_fin_DN
      real * 8 s,t,m2sq,m3sq,m4sq,musq
      real * 8 ms,mt,mm2sq,mm3sq,mm4sq
      complex * 16 lnms,lnmt,lnmm2sq,lnmm3sq,lnmm4sq
      complex * 16 li2m2ot,li2m4os,li2arg3
      real * 8 m2ot,m4os,arg3
      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      complex * 16 ris, li2arg(4)
      real * 8 prefactor, vbf_wp_wp_theta, vbf_wp_wp_DILOG, vbf_wp_wp_sgn, vbf_wp_wp_im_part, vbf_wp_wp_sgnratio

      real * 8 f3m, arg(4),argom, sign
      integer i

      if (musq.lt.0d0) then
         write(*,*) 
     #        'POSSIBLE ERROR IN vbf_wp_wp_D03m_fin: SCALE MUSQ LESS THAN ZERO!!'
      endif

      prefactor = 1d0/(s*t-m2sq*m4sq)
      ms = -s/musq
      mt = -t/musq
      mm2sq = - m2sq/musq
      mm3sq = - m3sq/musq
      mm4sq = - m4sq/musq
      
      lnms = log(abs(ms)) - ipi*vbf_wp_wp_theta(-ms)
      lnmt = log(abs(mt)) - ipi*vbf_wp_wp_theta(-mt)
      lnmm2sq = log(abs(mm2sq)) - ipi*vbf_wp_wp_theta(-mm2sq)
      lnmm3sq = log(abs(mm3sq)) - ipi*vbf_wp_wp_theta(-mm3sq)
      lnmm4sq = log(abs(mm4sq)) - ipi*vbf_wp_wp_theta(-mm4sq)

     
      m2ot = 1-m2sq/t
      m4os = 1-m4sq/s
      

c     m2sq/t < 0 
      if (m2ot.gt.1d0) then
         li2m2ot = -vbf_wp_wp_DILOG(1d0/m2ot) - log(m2ot)**2/2+pi2/3 
c     #        - ipi*log(m2ot)*vbf_wp_wp_im_part(m2sq,t)
     #        - ipi*log(m2ot)*vbf_wp_wp_sgnratio(m2sq,t)
      else
         li2m2ot = vbf_wp_wp_DILOG(m2ot)
      endif

c     m4sq/s < 0
      if (m4os.gt.1d0) then
         li2m4os = -vbf_wp_wp_DILOG(1d0/m4os) - log(m4os)**2/2+pi2/3 
c     #        - ipi*log(m4os)*vbf_wp_wp_im_part(m4sq,s)
     #        - ipi*log(m4os)*vbf_wp_wp_sgnratio(m4sq,s)
      else
         li2m4os = vbf_wp_wp_DILOG(m4os)
      endif

      
      arg(1) = s
      arg(2) = t
      arg(3) = m2sq
      arg(4) = m4sq
      f3m = (s+t-m2sq-m4sq)/(s*t-m2sq*m4sq)

      
      if (f3m.gt.0d0) then
         sign = -1d0
      else
         sign = +1d0
      endif


      do i=1,4
         argom = 1-arg(i)*f3m            
         if (argom.gt.1d0) then
            li2arg(i) = -vbf_wp_wp_DILOG(1d0/argom) - log(argom)**2/2+pi2/3 
     #           + ipi*log(argom)*sign
         else
            li2arg(i) = vbf_wp_wp_DILOG(argom)
         endif
      enddo
      
      ris = lnms**2/2+lnmt**2/2-lnmm2sq**2/2-lnmm4sq**2/2
     #     +lnmm2sq*lnmm3sq
     #     -(lnmm3sq+lnmm2sq-lnmt)*lnmt+lnmm3sq*lnmm4sq
     #     -(lnmm4sq+lnmm3sq-lnms)*lnms      
      
      ris = ris + 2 * (li2m2ot + li2m4os + li2arg(1) + li2arg(2) 
     #     - li2arg(3) - li2arg(4))
        
      vbf_wp_wp_D03m_fin_DN = prefactor * ris
      end




c    ***********   vbf_wp_wp_D04m_fin(s,t,m1sq,m2sq,m3sq,m4sq,musq)   *************
c
c Scalar box with MASSLESS PROPAGATORS and with external kinematics: 
c
c
c                     k
c     p1 ->-----------<-------------<-- p4
c             |                 |
c             |                 |
c             |                 |  
c     p2 ->-------------------------<-- p3
c
c
c    s = (p1+p2)^2
c    t = (p2+p3)^2
c    p1^2 = m1sq <>0, p2^2 = m2sq <>0, p3^2 = m3sq <>0, p4^2 = m4sq <>0
c    musq = mu^2 = reference dimensional scale  (NOT used here, since 
c                 this box is finite 
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2/(k+p1+p2+p3)^2 =  
c          N_ep * vbf_wp_wp_D04m_fin(s,t,m1sq,m2sq,m3sq,m4sq,musq);
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)
c

c	m2 ... arbitrary, but positive -> set m2 = 1d0
c
c     D0 with 4 nonzero external masses
c     and zero internal propagators      
c     use Eq.(41) from Denner, Nierste, Scharf		     
c

      function vbf_wp_wp_D04m_fin(s,t,m1sq,m2sq,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D04m_fin
      real * 8 s,t,m1sq,m2sq,m3sq,m4sq,musq

      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      integer k

      real * 8  vbf_wp_wp_DILOG, vbf_wp_wp_eta3, vbf_wp_wp_theta, vbf_wp_wp_sgn      
      complex * 16 ln_mx(2),lnk12,lnk13,lnk14,lnk23,total

      real * 8 m2,k12,k13,k14,k23,k24,k34
      real * 8 a,b,c,d,sqrtdelta,x(2)
      real * 8 k34ok13,im_k34ok13,k24ok12,im_k24ok12
      real * 8 im_x(2),im_k34ok13_x(2),im_k24ok12_x(2),arg,logarg
      complex * 16 eta_k34ok13(2),eta_k24ok12(2)
      complex * 16 lik34ok13(2), logk34ok13(2)
      complex * 16 lik24ok12(2), logk24ok12(2)
      complex * 16 D04m_fin_real

      m2 = 1d0                  ! > 0 ALWAYS!
      
      k12 = -m1sq/m2
      k13 = -s/m2
      k14 = -m4sq/m2
      k23 = -m2sq/m2
      k24 = -t/m2
      k34 = -m3sq/m2
      

      a = k24*k34
      b = k13*k24+k12*k34-k14*k23
      c = k12*k13
      d = k23
      

      if ((b**2-4*a*c).lt.0d0) then
         write(*,*) 'ERROR: delta less than zero!!'
         write(*,*) 'RETURN ZERO'
c use BDK version of D04m, since in this case it's REAL!!!
c         vbf_wp_wp_D04m_fin = D04m_fin_real(s,t,m1sq,m2sq,m3sq,m4sq,musq)
         vbf_wp_wp_D04m_fin = 0d0
c         write(*,*) 's,t,m1sq,m2sq,m3sq,m4sq'
c         write(*,*) s,t,m1sq,m2sq,m3sq,m4sq
         return         
c         stop
      else
         sqrtdelta = sqrt(b**2-4*a*c)
      endif


      x(1) = (-b + sqrtdelta)/(2d0*a)
      x(2) = (-b - sqrtdelta)/(2d0*a)

      k34ok13 = k34/k13
      im_k34ok13 = (1d0-m3sq/s)/s
      
      k24ok12 = k24/k12
      im_k24ok12 = (1d0-t/m1sq)/m1sq

      im_x(1) = m2sq/sqrtdelta
      im_x(2) = -im_x(1)


      do k=1,2
         im_k34ok13_x(k) = im_k34ok13*x(k) + k34ok13*im_x(k)
         im_k24ok12_x(k) = im_k24ok12*x(k) + k24ok12*im_x(k)
         eta_k34ok13(k) = 
     #        2*ipi*vbf_wp_wp_eta3(-im_x(k),im_k34ok13,-im_k34ok13_x(k))
         eta_k24ok12(k) = 2*ipi*
     #        vbf_wp_wp_eta3(-im_x(k),im_k24ok12,-im_k24ok12_x(k))
         ln_mx(k) = log(abs(x(k))) - vbf_wp_wp_theta(x(k))*vbf_wp_wp_sgn(im_x(k))*ipi
      enddo

      lnk12 = log(abs(k12))-ipi*vbf_wp_wp_theta(-k12)
      lnk13 = log(abs(k13))-ipi*vbf_wp_wp_theta(-k13)
      lnk14 = log(abs(k14))-ipi*vbf_wp_wp_theta(-k14)
      lnk23 = log(abs(k23))-ipi*vbf_wp_wp_theta(-k23)

c      write(*,*) lnk12,lnk13,lnk14,lnk23

      do k=1,2
         arg = 1d0 + k34ok13*x(k)
         if (arg.gt.1d0) then
c     complex vbf_wp_wp_DILOG
            logarg = log(arg)
            lik34ok13(k) = -vbf_wp_wp_DILOG(1/arg) - 0.5d0 * logarg**2 + pi2/3
     #           + ipi * vbf_wp_wp_sgn(im_k34ok13_x(k)) * logarg
         else
            lik34ok13(k) = vbf_wp_wp_DILOG(arg)
         endif
         logk34ok13(k) = log(abs(arg)) 
     #        + ipi*vbf_wp_wp_theta(-arg) * vbf_wp_wp_sgn(im_k34ok13_x(k))
c         write(*,*) lik34ok13(k),logk34ok13(k)
      enddo


      do k=1,2
         arg = 1d0 + k24ok12*x(k)
         if (arg.gt.1d0) then
c     complex vbf_wp_wp_DILOG
            logarg = log(arg)
            lik24ok12(k) = -vbf_wp_wp_DILOG(1/arg) - 0.5d0 * logarg**2 + pi2/3
     #           + ipi * vbf_wp_wp_sgn(im_k24ok12_x(k)) * logarg
         else
            lik24ok12(k) = vbf_wp_wp_DILOG(arg)
         endif
         logk24ok12(k) = log(abs(arg)) 
     #        + ipi*vbf_wp_wp_theta(-arg) * vbf_wp_wp_sgn(im_k24ok12_x(k))
      enddo

      total = 0d0
      do k=1,2
         total = total + (-1)**k*( - 0.5d0 *ln_mx(k)**2 
     #        - lik34ok13(k) - eta_k34ok13(k) * logk34ok13(k)
     #        - lik24ok12(k) - eta_k24ok12(k) * logk24ok12(k))
      enddo
      
      total = total + (lnk12+lnk13-lnk14-lnk23)*(-ln_mx(1)+ln_mx(2))

      vbf_wp_wp_D04m_fin = total/a/m2**2/(x(1)-x(2))
      
      end


      function E0(psq, qsq, lsq, tsq,
     #     pq, pl, pt, ql, qt, lt, 
     #     D0_2345,D0_1345,D0_1245,D0_1235,D0_1234)
      implicit none
      complex * 16 E0
      real * 8 psq, qsq, lsq, tsq, pq, pl, pt, ql, qt, lt
      real * 8 q1s, q2s, ppp, pq1, ppq1, pq2, ppq2, q1q2, pps
      real * 8 det, coeff(1:5)
      complex * 16 D0_2345,D0_1345,D0_1245,D0_1235,D0_1234, J(1:5)
      integer i
      real * 8 tiny
      logical lpr
      common /vbf_wp_wp_e0print/lpr

      tiny = 1d-4

      if (abs(psq).gt.tiny) then
         write(*,*) 'p^2 =',psq
         write(*,*) 'E0 computed only with p^2 and pp^2 = 0'
         stop
      endif

      q1s  = qsq
      q2s  = lsq
      ppp  = psq+pq+pl+pt
      pq1  = pq
      ppq1 = pq + qsq+ql+qt
      pq2  = pl
      ppq2 = pl + ql + lsq + lt
      q1q2 = ql
      pps = lsq+2*lt+2*ql+2*pl+tsq+2*qt+2*pt+qsq+2*pq+psq
      if (abs(pps).gt.tiny) then
         write(*,*) 'pp^2 =',pps
         write(*,*) 'E0 computed only with p^2 and pp^2 = 0'
         lpr = .true.
c         stop
      endif


      det = -8*ppp*(4*q1s*pq2**2*ppq1+q1s**2*q2s*ppp-4*pq1*pq2*q1s*ppq2-
     #4*pq1*pq2*ppq1*q2s+4*q1s*pq2*q1q2*ppp+4*q1s*pq2*q2s*ppp-4*pq1*q2s*
     #q1q2*ppp+4*q1s*q2s*pq1*ppp-8*pq1*pq2*ppq1*q1q2-8*pq1*q1q2*pq2*ppp+
     #4*q1s*pq1*q1q2*ppq2+2*q1s*pq1*q2s*ppq2-8*pq1*q2s*ppq1*q1q2+2*q2s*p
     #q2*q1s*ppq1+4*q1s*pq2*ppq1*q1q2+2*q1s*q1q2*q2s*ppp-2*q1s**2*pq2*pp
     #q2+4*pq1**2*q2s*ppq2+8*pq1**2*q1q2*ppq2+4*q1s*pq2**2*ppp-8*q1q2**2
     #*pq1*ppp+4*pq1**2*q2s*ppp+q1s*q2s**2*ppp-8*q1q2**2*pq1*ppq1-2*pq1*
     #q2s**2*ppq1)


      coeff(1)=(-16*pq1**2*q2s*ppp-8*q1s*pq1*ppq2**2+16*q1q2**2*ppq1*ppp
     #-8*pq1**2*q2s*ppq2-16*pq1**2*q1q2*ppq2-16*q1q2*pq2*ppq1**2-8*q1s*p
     #q2**2*ppq1-16*q1s*pq2**2*ppp+16*q1q2**2*pq1*ppp+16*q1q2**2*ppp**2-
     #8*q2s*pq2*ppq1**2-16*q1s*q2s*ppp**2+8*pq1*pq2*q1s*ppq2+8*pq1*pq2*p
     #pq1*q2s-8*q1s*pq2*q1q2*ppp-8*q1s*pq2*q2s*ppp+8*pq1*q2s*q1q2*ppp-8*
     #q1s*q2s*pq1*ppp+8*q1s*pq2*ppq1*ppq2-8*q1s*q1q2*ppq2*ppp-8*q1s*q2s*
     #ppq1*ppp-8*q1s*q2s*ppq2*ppp+8*pq1*q2s*ppq1*ppq2+8*q1q2*q2s*ppq1*pp
     #p+16*pq1*pq2*ppq1*q1q2+16*q1s*pq2*ppq2*ppp+16*pq1*q2s*ppq1*ppp+32*
     #pq1*q1q2*pq2*ppp-16*pq1*q1q2*ppq2*ppp-16*q1q2*pq2*ppq1*ppp+16*pq1*
     #q1q2*ppq1*ppq2)/det


      coeff(2)=(16*pq1*q2s*ppp**2+8*q2s*pq2*ppq1*ppp+16*pq2**2*ppq1**2-1
     #6*pq1*pq2*ppq2*ppp-16*q1q2*pq2*ppp**2+16*pq1**2*ppq2**2-32*pq1*pq2
     #*ppq1*ppq2+16*pq1*q2s*ppq1*ppp-8*q1q2*q2s*ppp**2+8*q1s*q2s*ppp**2+
     #16*pq2**2*ppq1*ppp+8*pq1*q2s*ppq2*ppp-16*q1q2**2*ppp**2+8*q1s*pq1*
     #ppq2**2-16*q1q2**2*ppq1*ppp+8*q2s*pq2*ppq1**2-8*q1s*pq2*ppq1*ppq2-
     #16*pq1*q1q2*ppq1*ppq2+8*q1s*q1q2*ppq2*ppp+16*q1q2*pq2*ppq1**2+8*q1
     #s*q2s*ppq1*ppp+8*q1s*q2s*ppq2*ppp-8*pq1*q2s*ppq1*ppq2-8*q1q2*q2s*p
     #pq1*ppp)/det


      coeff(3)=8*ppp*(-2*pq1*q1q2*ppp+2*q1s*pq2*ppq2-2*pq1*q2s*ppq1-4*pq
     #1*q1q2*ppq1-2*pq1*q1q2*ppq2-2*pq1*pq2*ppq1+q1s*q1q2*ppp+2*pq1**2*p
     #pq2+2*q1s*pq2*ppp+q1s*pq1*ppq2+q1s*pq2*ppq1-pq1*q2s*ppq2-q2s*pq2*p
     #pq1+2*pq1*pq2*ppq2+2*q1q2**2*ppp+q1q2*q2s*ppp-2*pq2**2*ppq1-2*pq1*
     #q2s*ppp-2*q1q2*pq2*ppq1+2*q1q2*pq2*ppp)/det


      coeff(4)=-8*ppp*(-2*pq1*q1q2*ppp-2*pq1*q2s*ppq1-4*pq1*q1q2*ppq1-2*
     #pq1*pq2*ppq1+q1s*q2s*ppp+q1s*q1q2*ppp+2*pq1**2*ppq2+2*q1s*pq2*ppp+
     #q1s*pq1*ppq2+q1s*pq2*ppq1)/det


      coeff(5)=(16*pq1**2*q1q2*ppq2+8*pq1**2*q2s*ppq2+16*q1s*pq2**2*ppp-
     #8*pq1*pq2*q1s*ppq2-16*pq1*pq2*ppq1*q1q2-8*pq1*pq2*ppq1*q2s+8*q1s*p
     #q2*q1q2*ppp+8*q1s*pq2*q2s*ppp-8*pq1*q2s*q1q2*ppp-32*pq1*q1q2*pq2*p
     #pp-16*q1q2**2*pq1*ppp+8*q1s*pq2**2*ppq1+16*pq1**2*q2s*ppp+8*q1s*q2
     #s*pq1*ppp)/det


c D0(q1,q2,pp-p-q1-q2) 
      J(1) = D0_2345
c D0(p+q1,q2,pp-p-q1-q2)
      J(2) = D0_1345
c D0(p,q1+q2,pp-p-q1-q2)
      J(3) = D0_1245
c D0(p,q1,pp-p-q1)
      J(4) = D0_1235
c D0(p,q1,q2)
      J(5) = D0_1234
      
      E0 = 0d0
      do i=1,5
         E0 = E0 + J(i)*coeff(i)
      enddo
      end



CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


c this function compute the imaginary part of a/b, where:
c (a+I*ep)/(b+I*ep) = a/b + I*ep*a/b*(1/a-1/b)
c  and return the sign of this imaginary part
      function vbf_wp_wp_sgnratio(a,b)
      implicit none
      real * 8 a,b,vbf_wp_wp_sgnratio
      if ((1d0/b-a/b**2).gt.0d0) then
         vbf_wp_wp_sgnratio = +1d0
      else
         vbf_wp_wp_sgnratio = -1d0          
      endif

      end


c     imaginary part of a/b  
c     IF a/b > 0 then returns 0
c     IF a/b < 0 then
c        if a < 0 then return +1
c        else return -1
      function vbf_wp_wp_im_part(a,b)
      implicit none
      real * 8 a,b,vbf_wp_wp_im_part
      if ((a/b).gt.0d0) then
         vbf_wp_wp_im_part = 0d0
      else
         if (a.lt.0d0) then
            vbf_wp_wp_im_part = +1d0
         else
            vbf_wp_wp_im_part = -1d0
         endif
      endif
      
      end


c     compute  Li(1-a/b) 
c     IF a/b < 0 then, it attaches a POSITIVE +I*ep to a or b, 
c     according to which of them is negative
      function vbf_wp_wp_dilog_1_m_aob(a,b)
      implicit none
      complex * 16 vbf_wp_wp_dilog_1_m_aob
      real * 8 a,b
      real * 8 arg
      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)
      real * 8  vbf_wp_wp_DILOG, sign

      arg = 1d0-a/b
      if (arg.gt.1d0) then
         if (a.lt.0d0) then
            sign = +1d0
         else
            sign = -1d0
         endif
         vbf_wp_wp_dilog_1_m_aob = -vbf_wp_wp_DILOG(1d0/arg) - log(arg)**2/2+pi2/3 
     #        - ipi*log(arg)*sign
      else
         vbf_wp_wp_dilog_1_m_aob = vbf_wp_wp_DILOG(arg)
      endif
      
      end


      function vbf_wp_wp_eta3(im_a,im_b,im_ab)
      implicit none
      real * 8 vbf_wp_wp_eta3,im_a,im_b,im_ab
      real * 8 vbf_wp_wp_theta
      vbf_wp_wp_eta3 = vbf_wp_wp_theta(-im_a)*vbf_wp_wp_theta(-im_b)*vbf_wp_wp_theta(im_ab)
     #     - vbf_wp_wp_theta(im_a)*vbf_wp_wp_theta(im_b)*vbf_wp_wp_theta(-im_ab)
      end


c     Parabolic interpolation thru the points (x1,y1), (x2,y2), (x3,y3)
c     It return the ordinate of point with abscissa x
      function vbf_wp_wp_interp_parab(x1,y1,x2,y2,x3,y3,x)
      implicit none
      real * 8 x1,y1,x2,y2,x3,y3,x,vbf_wp_wp_interp_parab
      real * 8 a,b,c
      a = (x1*y3-x3*y1-x2*y3-x1*y2+x2*y1+x3*y2)/(x2-x3)/(-x3+x1)/(x1-x2)
      b = -(x1**2*y3-x1**2*y2-y1*x3**2+y2*x3**2-x2**2*y3+y1*x2**2)/(x2-x
     #3)/(-x3+x1)/(x1-x2)
      c = (-y1*x2*x3**2+x2**2*x3*y1+x1**2*x2*y3-x1**2*x3*y2+y2*x1*x3**2-
     #x2**2*x1*y3)/(x2-x3)/(-x3+x1)/(x1-x2)
      vbf_wp_wp_interp_parab = a*x**2 + b*x + c
      end

c determinant of
c     q1(0)   q1(1)  q1(2)  q1(3)
c     q2(0)   q2(1)  ....
c     ...
c     q4(0)   ...           q4(3)
      function vbf_wp_wp_det_planar(q1,q2,q3,q4)
      implicit none
      real * 8 vbf_wp_wp_det_planar,q1(0:3),q2(0:3),q3(0:3),q4(0:3)
      real * 8 s1
      s1 = q1(0)*q2(1)*q3(2)*q4(3)-q1(0)*q2(1)*q3(3)*q4(2)-q1(0)*q3(1)*q
     #2(2)*q4(3)+q1(0)*q3(1)*q2(3)*q4(2)+q1(0)*q4(1)*q2(2)*q3(3)-q1(0)*q
     #4(1)*q2(3)*q3(2)-q2(0)*q1(1)*q3(2)*q4(3)+q2(0)*q1(1)*q3(3)*q4(2)+q
     #2(0)*q3(1)*q1(2)*q4(3)-q2(0)*q3(1)*q1(3)*q4(2)-q2(0)*q4(1)*q1(2)*q
     #3(3)+q2(0)*q4(1)*q1(3)*q3(2)
      vbf_wp_wp_det_planar = s1+q3(0)*q1(1)*q2(2)*q4(3)-q3(0)*q1(1)*q2(3)*q4(2)-q3
     #(0)*q2(1)*q1(2)*q4(3)+q3(0)*q2(1)*q1(3)*q4(2)+q3(0)*q4(1)*q1(2)*q2
     #(3)-q3(0)*q4(1)*q1(3)*q2(2)-q4(0)*q1(1)*q2(2)*q3(3)+q4(0)*q1(1)*q2
     #(3)*q3(2)+q4(0)*q2(1)*q1(2)*q3(3)-q4(0)*q2(1)*q1(3)*q3(2)-q4(0)*q3
     #(1)*q1(2)*q2(3)+q4(0)*q3(1)*q1(3)*q2(2)
      end

c     expression that appears in the denominator of the scalar pentagon, 
c     once it's written in terms of the 5 boxes
      function vbf_wp_wp_E0_determ(q1,q2,q3,q4)
      implicit none
      real * 8 vbf_wp_wp_E0_determ,q1(0:3),q2(0:3),q3(0:3),q4(0:3)
      real * 8 s1,s2,determin,q1s,q2s,q3s,q4s,q12,q13,q14,q23,q24,q34
      real * 8 vbf_wp_wp_dotp

      q1s = vbf_wp_wp_dotp(q1,q1)
      q2s = vbf_wp_wp_dotp(q2,q2)
      q3s = vbf_wp_wp_dotp(q3,q3)
      q4s = vbf_wp_wp_dotp(q4,q4)
      q12 = vbf_wp_wp_dotp(q1,q2)
      q13 = vbf_wp_wp_dotp(q1,q3)
      q14 = vbf_wp_wp_dotp(q1,q4)
      q23 = vbf_wp_wp_dotp(q2,q3)
      q24 = vbf_wp_wp_dotp(q2,q4)
      q34 = vbf_wp_wp_dotp(q3,q4)

      s2 = -8*q2s*q4s**2*q13**2-8*q1s*q3s**2*q24**2-8*q3s*q14**2*q2s**2-
     #32*q4s*q12**2*q23**2-32*q2s*q34**2*q13**2-8*q2s*q34**2*q1s**2-8*q1
     #s*q4s**2*q23**2-8*q4s*q1s**2*q23**2-8*q3s*q4s**2*q12**2-8*q4s*q2s*
     #*2*q13**2-32*q1s*q34**2*q23**2-32*q3s*q12**2*q24**2+16*q1s*q2s*q3s
     #*q4s*q14-8*q2s*q3s**2*q14**2-8*q3s*q1s**2*q24**2-8*q3s**2*q4s*q12*
     #*2-8*q1s*q34**2*q2s**2+64*q23*q34**2*q12**2+8*q2s*q3s*q4s*q1s**2+3
     #2*q1s*q23*q34**2*q12-32*q1s*q2s*q34**2*q13+8*q1s*q2s*q3s*q4s**2+8*
     #q1s*q3s*q4s*q2s**2
      s1 = 8*q1s*q2s*q3s**2*q4s-32*q1s*q4s*q34*q23**2-16*q1s*q2s*q34**2*
     #q12-32*q1s*q2s*q34**2*q23+16*q23*q34*q1s**2*q24-16*q3s*q4s*q34*q12
     #**2-32*q23*q3s*q4s*q12**2-32*q24*q3s*q4s*q12**2-32*q24*q3s*q34*q12
     #**2+64*q23*q34*q12**2*q24-32*q1s*q4s*q12*q23**2-32*q1s*q34*q14*q23
     #**2-16*q1s*q4s*q14*q23**2-32*q1s*q3s*q12*q24**2-16*q1s*q3s*q13*q24
     #**2+64*q12*q23*q34**2*q13-32*q12*q2s*q34**2*q13+16*q12*q24*q3s**2*
     #q14+16*q12*q23*q4s**2*q13-64*q12*q34*q14*q23**2-32*q12*q4s*q14*q23
     #**2-32*q12*q3s*q13*q24**2+32*q23*q4s*q34*q12**2-32*q2s*q4s*q34*q13
     #**2+s2
      s2 = s1-32*q2s*q34*q13**2*q24-16*q2s*q4s*q13**2*q24+16*q13*q34*q14
     #*q2s**2-16*q2s*q3s*q14**2*q23+16*q1s*q2s*q3s*q4s*q12+16*q1s*q2s*q3
     #s*q4s*q34+16*q1s*q2s*q3s*q4s*q24+16*q1s*q2s*q3s*q4s*q13+16*q1s*q2s
     #*q3s*q4s*q23-16*q1s*q2s*q4s*q34*q13-16*q1s*q2s*q4s*q34*q23+16*q1s*
     #q2s*q34*q3s*q14+16*q1s*q2s*q34*q3s*q24+16*q1s*q23*q3s*q4s*q24+16*q
     #1s*q23*q4s*q34*q12+32*q1s*q23*q34*q3s*q24-16*q1s*q23*q3s*q4s*q12-1
     #6*q1s*q24*q3s*q4s*q12-16*q1s*q24*q3s*q34*q12-16*q1s*q2s*q34*q13*q2
     #4-16*q1s*q2s*q34*q14*q23+16*q1s*q23*q4s*q2s*q13+64*q1s*q23*q34*q12
     #*q24
      determin = 32*q1s*q23*q34*q13*q24+16*q1s*q23*q4s*q13*q24+16*q1s*q2
     #4*q3s*q14*q2s+16*q1s*q24*q3s*q14*q23+16*q12*q2s*q3s*q4s*q13-16*q12
     #*q2s*q4s*q34*q13+16*q12*q2s*q34*q3s*q14-16*q12*q23*q3s*q4s*q14+64*
     #q12*q23*q4s*q34*q13-32*q12*q23*q34*q3s*q14-32*q12*q24*q3s*q34*q13-
     #16*q12*q24*q3s*q4s*q13-32*q12*q2s*q34*q13*q24-32*q12*q2s*q34*q14*q
     #23+32*q12*q23*q4s*q2s*q13+64*q12*q23*q34*q13*q24+32*q12*q23*q4s*q1
     #3*q24+32*q12*q24*q3s*q14*q2s+32*q12*q24*q3s*q14*q23+16*q12*q2s*q3s
     #*q4s*q14+16*q13*q2s*q3s*q4s*q14+32*q13*q2s*q34*q3s*q14+32*q13*q2s*
     #q34*q14*q23+16*q13*q2s*q4s*q14*q23+16*q13*q24*q3s*q14*q2s+s2
      vbf_wp_wp_E0_determ = determin
      end

      subroutine vbf_wp_wp_print_Cij(vbf_wp_wp_C0,Cij)
      implicit none
      complex * 16 vbf_wp_wp_C0,Cij(2,4)
      integer indx_C(2),indx_D(3),indx_E(4)
      common/vbf_wp_wp_indexes/indx_C,indx_D,indx_E
      integer i1,i2
      write(*,*) 'vbf_wp_wp_C0 =',vbf_wp_wp_C0
      do i1=1,2
         do i2=1,indx_C(i1)
            write(*,*) 'Cij(',i1,i2,') =',Cij(i1,i2)
         enddo
      enddo
      end


      subroutine vbf_wp_wp_print_Dij(D0,Dij)
      implicit none
      complex * 16 D0,Dij(3,13)
      integer indx_C(2),indx_D(3),indx_E(4)
      common/vbf_wp_wp_indexes/indx_C,indx_D,indx_E
      integer i1,i2
      write(*,*) 'D0 =',D0
      do i1=1,3
         do i2=1,indx_D(i1)
            write(*,*) 'Dij(',i1,i2,') =',Dij(i1,i2)
         enddo
      enddo
      end

      subroutine vbf_wp_wp_print_Eij(E0,Eij)
      implicit none
      complex * 16 E0,Eij(4,46)
      integer indx_C(2),indx_D(3),indx_E(4)
      common/vbf_wp_wp_indexes/indx_C,indx_D,indx_E
      integer i1,i2
      write(*,*) 'E0 =',E0
      do i1=1,4
         do i2=1,indx_E(i1)
            write(*,*) 'Eij(',i1,i2,') =',Eij(i1,i2)
         enddo
      enddo
      end


      function vbf_wp_wp_rndsign()
      implicit none
      integer vbf_wp_wp_rndsign
      integer num,num2     
      COMMON/vbf_wp_wp_SEED/num,num2
      real * 8 vbf_wp_wp_random2
      if (vbf_wp_wp_random2(NUM,num2).lt.0.5d0) then
         vbf_wp_wp_rndsign = -1
      else
         vbf_wp_wp_rndsign = 1
      endif
      end



c  vbf_wp_wp_random vbf_wp_wp_B0, vbf_wp_wp_C0, D0 and E0 functions  (for debug purpose only)
      function vbf_wp_wp_B0(psq,musq)
      implicit none
      complex * 16 vbf_wp_wp_B0
      real * 8 psq,musq
c      complex * 16 vbf_wp_wp_crand_num      
      if (psq.eq.0d0) then
         vbf_wp_wp_B0 = 0d0
      else
         vbf_wp_wp_B0 = psq + 1d0/psq*(0,1)
      endif
c      vbf_wp_wp_B0 = 0d0
      vbf_wp_wp_B0 = (1d0,3d0)
      end

      function vbf_wp_wp_C0(psq,qsq,pq_sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_C0
      real * 8 psq,qsq,pq_sq,musq
c      complex * 16 vbf_wp_wp_crand_num      
c      vbf_wp_wp_C0 = psq+qsq+pq_sq*(0,1)
c      vbf_wp_wp_C0 = 0d0
      vbf_wp_wp_C0 = (5d0,7d0)
      end


      function D0(psq,qsq,lsq,pq,pl,ql)
      implicit none
      complex * 16 D0
      real * 8 psq,qsq,lsq,pq,pl,ql
c      complex * 16 vbf_wp_wp_crand_num      
c      D0 = psq+qsq+lsq+(pq+pl+ql)*(0,1)
c      D0 = 0d0
      D0 = (11d0,13d0)
      end


c version of vbf_wp_wp_D04m_fin taken from Bern-Dixon and Kosower.
c Analytic continuation is NOT traightforward!!
c
c    ***********   vbf_wp_wp_D04m_fin(s,t,m1sq,m2sq,m3sq,m4sq,musq)   *************
c
c Scalar box with MASSLESS PROPAGATORS and with external kinematics: 
c
c                     
c     p2 ->-------------------------<-- p3
c             |                 |
c             |                 |
c             |       k         |  
c     p1 ->-----------<-------------<-- p4
c
c
c    s = (p1+p2)^2
c    t = (p2+p3)^2
c    p1^2 = m1sq <>0, p2^2 = m2sq <>0, p3^2 = m3sq <>0, p4^2 = m4sq <>0
c    musq = mu^2 = reference dimensional scale  (NOT used here, since 
c                 this box is finite 
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2/(k+p1+p2+p3)^2 =  
c          N_ep * vbf_wp_wp_D04m_fin(s,t,m1sq,m2sq,m3sq,m4sq,musq);
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)
c



      function vbf_wp_wp_D04m_fin_wrong(s,t,m1sq,m2sq,m3sq,m4sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D04m_fin_wrong
      real * 8 s,t,m1sq,m2sq,m3sq,m4sq,musq

      real * 8 arg1,arg2,arg3,arg4,arg5,l1,l2,argr,r,l1tmp

      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      real * 8  vbf_wp_wp_DILOG
            
c      if (musq.lt.0d0) then
c         write(*,*) 'POSSIBLE ERROR: SCALE MUSQ LESS THAN ZERO!!'
c      endif

c     the formula contains log(l1) so, to simplify the treatment of 
c     analytic continuation, we relabel the external invariants so 
c     that l1 > 0
c     
      
      l1 = m1sq*m3sq/s/t
      l2 = m2sq*m4sq/s/t
      
      if (l1.lt.0d0) then
c     exchange s and t, and relabel the external masses starting from m2, 
c     so that:
c     s <-> t,  m2sq -> m1sq,  m3sq -> m2sq,  m4sq -> m3sq,  m1sq -> m4sq 
         l1tmp = l1
         l1 = l2
         l2 = l1tmp
      endif

      argr = 1-2*l1-2*l2+(l1-l2)**2

      if (argr.le.0d0) then
         write(*,*) 'ERROR: argr < 0 in vbf_wp_wp_D04m_fin_wrong'
         stop
      endif

      r = sqrt(argr)
      arg1 = (1-l1+l2+r)/2
      arg2 = (1-l1+l2-r)/2
      arg3 = -(1-l1-l2-r)/(2*l1)
      arg4 = -(1-l1-l2+r)/(2*l1)
      arg5 = (1+l1-l2+r)/(1+l1-l2-r)

      write(*,*) 'r,l1,l2,arg1,arg2,arg3,arg4,arg5'
      write(*,*) r,l1,l2,arg1,arg2,arg3,arg4,arg5
      
      if ((l1.lt.0d0).or.(arg1.gt.1d0).or.(arg2.gt.1d0).or.
     #     (arg3.gt.1d0).or.(arg4.gt.1d0).or.(arg5.lt.0d0)) then
         write(*,*) 'ERROR in one of the arguments in vbf_wp_wp_D04m_fin_wrong'
         write(*,*) 's,t,m1sq,m2sq,m3sq,m4sq'
         write(*,*) s,t,m1sq,m2sq,m3sq,m4sq
         write(*,*) 'l1,arg1,arg2,arg3,arg4,arg5'
         write(*,*) l1,arg1,arg2,arg3,arg4,arg5
         stop
      endif
      
      vbf_wp_wp_D04m_fin_wrong = (vbf_wp_wp_DILOG(arg1)-vbf_wp_wp_DILOG(arg2)+vbf_wp_wp_DILOG(arg3)-vbf_wp_wp_DILOG(arg4)
     #     + log(l1/l2**2)*log(arg5)/2)/r/s/t

      end


      subroutine vbf_wp_wp_test_anal_cont(s,t,m2sq,m3sq,m4sq)
      implicit none
      real * 8 s,t,m2sq,m3sq,m4sq
      real * 8 ms,mt,mm2sq,mm3sq,mm4sq,sot
      complex * 16 lnms,lnmt,lnmm2sq,lnmm3sq,lnmm4sq,lnsot
      complex * 16 li2arg1,li2arg2,li2arg3
      real * 8 arg1,arg2,arg3
      complex * 16 ipi
      parameter (ipi=(0d0,3.14159265358979323846264338328d0))
      real * 8 pi,pi2
      parameter (pi=3.14159265358979323846264338328d0,
     #     pi2 = 9.86960440108935861883449099988d0)

      complex * 16 ris1,ris2
      real * 8 prefactor, vbf_wp_wp_theta, vbf_wp_wp_DILOG, vbf_wp_wp_sgn, vbf_wp_wp_im_part, f3m


     
      arg1 = 1-m2sq/s
      arg2 = 1-m4sq/t
      arg3 = 1-m2sq*m4sq/s/t

      lnsot = log(s/t)
      

c     m2sq/s < 0 
      if (arg1.gt.1d0) then
         li2arg1 = -vbf_wp_wp_DILOG(1d0/arg1) - log(arg1)**2/2+pi2/3 
     #        - ipi*log(arg1)*vbf_wp_wp_im_part(m2sq,s)
      else
         li2arg1 = vbf_wp_wp_DILOG(arg1)
      endif

c     m4sq/t < 0
      if (arg2.gt.1d0) then
         li2arg2 = -vbf_wp_wp_DILOG(1d0/arg2) - log(arg2)**2/2+pi2/3 
     #        - ipi*log(arg2)*vbf_wp_wp_im_part(m4sq,t)
      else
         li2arg2 = vbf_wp_wp_DILOG(arg2)
      endif

c     m2sq*m4sq/(s*t)  < 0
      if (arg3.gt.1d0) then
         li2arg3 = -vbf_wp_wp_DILOG(1d0/arg3) - log(arg3)**2/2+pi2/3 
     #        - ipi*log(arg3)*vbf_wp_wp_im_part(m2sq*m4sq,s*t)
      else
         li2arg3 = vbf_wp_wp_DILOG(arg3)
      endif
      
      ris1 =  - 2*li2arg1 - 2*li2arg2 + 2*li2arg3 
     #     - lnsot**2
        
      f3m = (s+t-m2sq-m4sq)/(s*t-m2sq*m4sq)

      ris2 = 2*(vbf_wp_wp_DILOG(1-m2sq/t) + vbf_wp_wp_DILOG(1-m4sq/s) 
     #     + vbf_wp_wp_DILOG(1-s*f3m) + vbf_wp_wp_DILOG(1-t*f3m)  
     #     - vbf_wp_wp_DILOG(1-m2sq*f3m) - vbf_wp_wp_DILOG(1-m4sq*f3m))

      write(*,*) ris1
      write(*,*) ris2

      end





      subroutine vbf_wp_wp_test_D0
      implicit none

      complex * 16 vbf_wp_wp_D0t,vbf_wp_wp_D04m_fin,D04,ris1,ris2,ris3,ris4,
     #     vbf_wp_wp_D03m_fin,vbf_wp_wp_D0fin, vbf_wp_wp_D02m_fin,
     #     vbf_wp_wp_D03m_fin_DN, vbf_wp_wp_D03m_fin_BDK
      real * 8 invs,invt,m1sq,m2sq,m3sq,m4sq,m
      
      real * 8 energ,musq,ratioimag,ratioreal

      integer num,num2     
      COMMON/vbf_wp_wp_SEED/num,num2
      real * 8 vbf_wp_wp_random2
      integer vbf_wp_wp_rndsign
      integer i
      
      energ = 100
      m = 0.0001d0

      do i=1,1000
         invs = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         invt = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         m1sq = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         m2sq = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         m3sq = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         m4sq = energ*vbf_wp_wp_random2(NUM,num2)*vbf_wp_wp_rndsign()
         musq = energ*vbf_wp_wp_random2(NUM,num2)  ! > 0 always


c     TEST D04m
         goto 727
         ris1 = vbf_wp_wp_D04m_fin(invs,invt,m1sq,m2sq,m3sq,m4sq,musq)
         ris2 = vbf_wp_wp_D0t(m1sq,m2sq,m3sq,m4sq,invt,invs)
         ris3 = D04(m1sq,m2sq,m3sq,m4sq,invs,invt,0d0,m,m,m)
         write(*,*) ris1
         write(*,*) ris2
         write(*,*) ris3
 727     continue


c     TEST D03m
c         goto 456
         ris1 = vbf_wp_wp_D03m_fin_DN(invs,invt,m2sq,m3sq,m4sq,musq)
         ris2 = vbf_wp_wp_D03m_fin_BDK(invs,invt,m2sq,m3sq,m4sq,musq)

         if ((abs(dimag(ris1)).lt.1d-14).and.
     #        (abs(dimag(ris2)).lt.1d-14)) then
            ratioimag = 1d0
         else
            ratioimag = dimag(ris1)/dimag(ris2)
         endif
         
         ratioreal = dreal(ris1)/dreal(ris2)
         
         if ((abs(ratioimag-1d0).gt.1d-10).or.
     #        (abs(ratioreal-1d0).gt.1d-10)) then
            write(*,*) '=========>',ratioreal,ratioimag
            write(*,*) ris1
            write(*,*) ris2
            write(*,*) ''
         endif

 456     continue


      enddo


      end


c generate n ligh-like momenta
c  p1 + p2 = p3 + p4 + ... + pn
      subroutine vbf_wp_wp_fill_momenta(n,p)
      implicit none
      integer n
      real * 8 softness
      real * 8 p(0:3,n)
      
      real * 8 E(n),th(n),ph(n),Etot
      integer mu
      real * 8 vbf_wp_wp_random2
      integer num,num2     
      COMMON/vbf_wp_wp_SEED/num,num2
      double precision pi
      parameter (pi=3.141592653589793238462643383279502884197D0)
      real * 8 Emax(n) 
      integer i
      real * 8 vbf_wp_wp_dotp
      
      do i=3,n
         Emax(i) = 2000.d0         
         E(i)  = Emax(i)*vbf_wp_wp_random2(NUM,num2)
         th(i) = pi*vbf_wp_wp_random2(NUM,num2)
         ph(i) = 2*pi*vbf_wp_wp_random2(NUM,num2)
      enddo


      th(1) = 0d0
      ph(1) = 0d0

      th(2) = pi
      ph(2) = 0d0

      do i=3,n
         p(0,i) = E(i)
         p(1,i) = E(i)*sin(th(i))*cos(ph(i))
         p(2,i) = E(i)*sin(th(i))*sin(ph(i))
         p(3,i) = E(i)*cos(th(i))
      enddo

c  compute px, py and pz of the (n-1)th vector: Sum P_vec_i = 0            
      do mu=1,3
         p(mu,n-1) = 0d0
         do i=3,n
            if (i.ne.n-1) then 
               p(mu,n-1) = p(mu,n-1) - p(mu,i)
            endif
         enddo
      enddo

C p(n-1)^2=0 ==>
      p(0,n-1) = sqrt(p(1,n-1)**2 + p(2,n-1)**2 + p(3,n-1)**2)

           
c  conservation p1 + p2 = p3 + p4 + ... + pn
      Etot = 0d0
      do i=3,n
         Etot = Etot + p(0,i)
      enddo
      
      E(1) = Etot/2
      E(2) = Etot/2    
      
      p(0,1) = E(1)
      p(1,1) = 0.d0
      p(2,1) = 0.d0
      p(3,1) = E(1)

      p(0,2) = E(2)
      p(1,2) = 0.d0
      p(2,2) = 0.d0
      p(3,2) = -E(2)


      end


      
      function vbf_wp_wp_born(
c     vbf_wp_wp_bra(p'), vbf_wp_wp_ket(p) and helicity 
     #     brapp,ketp,hel,
c     momenta and squared renormalization scale
     #     p,q,l,t,pp,
c     input currents 
     #     jmu1,jmu2,jmu3)

      implicit none
      complex * 16 vbf_wp_wp_born
      complex * 16 brapp(2,-1:1),ketp(2,-1:1)
      integer hel
      real * 8 p(0:3),pp(0:3),q(0:3),l(0:3),t(0:3)
      complex * 16 jmu1(0:3),jmu2(0:3),jmu3(0:3)

      complex * 16 vbf_wp_wp_tree

      real * 8 psq, qsq, lsq, tsq,
     #     pq, pl, pt, ql, qt, lt, ppp, ppq, ppl

      real * 8 vbf_wp_wp_dotp

      psq = vbf_wp_wp_dotp(p,p)
      qsq = vbf_wp_wp_dotp(q,q)
      lsq = vbf_wp_wp_dotp(l,l)
      tsq = vbf_wp_wp_dotp(t,t)

      pq = vbf_wp_wp_dotp(p,q)
      pl = vbf_wp_wp_dotp(p,l)
      ql = vbf_wp_wp_dotp(q,l)
      qt = vbf_wp_wp_dotp(q,t)
      lt = vbf_wp_wp_dotp(l,t)

      pt = vbf_wp_wp_dotp(p,t)

      ppp = vbf_wp_wp_dotp(p,pp)
      ppq = vbf_wp_wp_dotp(pp,q)
      ppl = vbf_wp_wp_dotp(pp,l)
      
      call vbf_wp_wp_compute_Jprod_gammaprod(
     #     brapp,ketp,hel,
     #     p,q,l,t,pp,
     #     jmu1,jmu2,jmu3)
      
      vbf_wp_wp_born = vbf_wp_wp_tree(qsq,lsq,pq,pl,ppp,ppq,ppl,ql)
      
      end


      function vbf_wp_wp_tree(q1s,q2s,pq1,pq2,ppp,ppq1,ppq2,q1q2)
      implicit none
      complex * 16 vbf_wp_wp_tree
      real * 8 q1s,q2s,pq1,pq2,ppp,ppq1,ppq2,q1q2
      complex * 16 dpmu1,dpmu2,dpmu3,dqmu1,dqmu2,dqmu3
      complex * 16 dlmu1,dlmu2,dlmu3,dppmu1,dppmu2,dppmu3
      complex * 16 dmu1mu2,dmu1mu3,dmu2mu3
      common/vbf_wp_wp_dotp_pol_vec/dpmu1,dpmu2,dpmu3,dqmu1,dqmu2,dqmu3,
     #     dlmu1,dlmu2,dlmu3,dppmu1,dppmu2,dppmu3,
     #     dmu1mu2,dmu1mu3,dmu2mu3
      complex * 16 gp_mu3_l_mu2_q_mu1,gp_l,gp_mu2,gp_mu3_l_q
      complex * 16 gp_mu2_q_mu1,gp_mu3_mu2_q,gp_q,gp_mu3_l_mu2
      complex * 16 gp_l_mu2_q,gp_mu3_mu2_mu1,gp_mu3_l_mu1,gp_mu3
      complex * 16 gp_mu1,gp_l_q_mu1,gp_mu3_q_mu1,gp_l_mu2_mu1
      common/vbf_wp_wp_gammaprods/gp_mu3_l_mu2_q_mu1,gp_l,gp_mu2,gp_mu3_l_q,
     #     gp_mu2_q_mu1,gp_mu3_mu2_q,gp_q,gp_mu3_l_mu2,
     #     gp_l_mu2_q,gp_mu3_mu2_mu1,gp_mu3_l_mu1,gp_mu3,
     #     gp_mu1,gp_l_q_mu1,gp_mu3_q_mu1,gp_l_mu2_mu1
      complex * 16 t1,t2,t3,t4,t5,t6,t7,t8,t9,t10
      complex * 16 t11,t12,t13,t14,t15,t16,t17
      t2 = 2*pq1
      t5 = 1/(q1s+2*q1q2+t2+q2s+2*pq2)
      t7 = dqmu2+dpmu2
      t10 = 1/(q1s+t2)
      t11 = t5*t10
      vbf_wp_wp_tree = -t5*gp_mu3_mu2_mu1+4*dpmu1*t7*t11*gp_mu3+t11*gp_mu3_l_mu2_q
     #_mu1+2*t7*t5*t10*gp_mu3_q_mu1+2*dpmu1*t5*t10*gp_mu3_l_mu2
      end

c     returns the azimuthal angle between the vector and the y axis
c     in the yz plane
      function vbf_wp_wp_azimuth_vec_y_yz(p)
      implicit none
      real * 8 vbf_wp_wp_azimuth_vec_y_yz,p(0:3)
      real * 8 phi
      real * 8 pi
      parameter (pi=3.141592653589793238462643383279502884197D0)      
      real * 8 tiny
      parameter (tiny = 1d-13)
      
      if ((abs(p(2)).lt.tiny).and.(abs(p(3)).lt.tiny)) then
         vbf_wp_wp_azimuth_vec_y_yz = 0d0
         return
      endif

      if (abs(p(2)).lt.tiny) then
         if (p(3).gt.0d0) then
            phi = pi/2
         else
            phi = -pi/2
         endif
      else
         phi = atan(p(3)/p(2))
         if (p(2).lt.0d0) then
            if (phi.gt.0d0) then               
               phi = phi - pi
            else
               phi = phi + pi
            endif
         endif         
      endif
      vbf_wp_wp_azimuth_vec_y_yz = phi
      end



