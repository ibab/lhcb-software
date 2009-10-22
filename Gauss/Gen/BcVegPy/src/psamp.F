CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C...SUBROUTINES FOR gg ->Bc+b+\bar{c}:  Bc in p-wave STATES.         C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C...       BC SHOULD BE IN COLOR-SINGLET AND COLOR-OCTET STATES.     C
C Copyright (c) Z.X ZHANG, Chafik Driouich, Paula Eerola and X.G. Wu C
C reference: Phys.Rev. D70,114019(2004);                             C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C**********************************************************
C******** TO GET SQUARE AMPLITUDE FOR 1P1   ***************
C**********************************************************
      DOUBLE PRECISION FUNCTION amps20_1p1()
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N)
      COMMON/CCCC/c36,c35,c34,c33,c32,c31,c30,c29 
     . ,c28,c27,c26,c25,c24,c23,c22,c21,c20,c19,c18,c17 
     . ,c16,c15,c14,c13,c12,c11,c10,c9,c8,c7,c6,c5,c4,c3
     . ,c2,c1s2,c1s1
      include 'inclamp.f'
C...COLOR FLOW.
 	COMMON/COLFLOW/AMP2CF(10),SMATVAL
	DIMENSION TEMPAMP(3)

	DO II=1,10
	   AMP2CF(II)=0.0D0
	END DO

      DO 200 m=1,32 
        DO 400 n=1,3 
          c(m,n)=0.0D0
400     continue 
200   continue

      call genppp()

      call amp1s1_1p1(c1s1) 
      call amp1s2_1p1(c1s2) 
      call amp2_1p1(c2)
      call amp3_1p1(c3)
      call amp4_1p1(c4) 
      call amp5_1p1(c5) 
      call amp6_1p1(c6) 
      call amp7_1p1(c7) 
      call amp8_1p1(c8) 
      call amp9_1p1(c9) 
      call amp10_1p1(c10) 
      call amp11_1p1(c11) 
      call amp12_1p1(c12) 
      call amp13_1p1(c13) 
      call amp14_1p1(c14) 
      call amp15_1p1(c15) 
      call amp16_1p1(c16) 
      call amp17_1p1(c17) 
      call amp18_1p1(c18) 
      call amp19_1p1(c19) 
      call amp20_1p1(c20) 
      call amp21_1p1(c21) 
      call amp22_1p1(c22) 
      call amp23_1p1(c23) 
      call amp24_1p1(c24) 
      call amp25_1p1(c25) 
      call amp26_1p1(c26) 
      call amp27_1p1(c27) 
      call amp28_1p1(c28) 
      call amp29_1p1(c29) 
      call amp30_1p1(c30) 
      call amp31_1p1(c31) 
      call amp32_1p1(c32) 
      call amp33_1p1(c33) 
      call amp34_1p1(c34) 
      call amp35_1p1(c35) 
      call amp36_1p1(c36) 
      
	amps20_1p1=0.0D0

      DO 600 N=1,3
         call genpppn(N)
         TEMPAMP(N)=ams1_1p1(N)
         amps20_1p1=amps20_1p1+TEMPAMP(N)
600   continue

	nreduce=4
      amps20_1p1=nreduce*amps20_1p1

C... COLOR-FLOW.
C... NOTE HERE: SINCE THE CROSS-TERMS IN THE COLOR-FLOW ARE NEGLECTED,
C... THE TOTAL SQUARE OF AMPLITUDE DOES NOT EQUAL TO THE SUM OF THE
C... SQUARE OF THE PARTIAL AMPLITUDE (PERTAIN TO A CERTAIN COLOR-FLOW).

	 AMP2CF(1)=6.0D0/65.0D0*TEMPAMP(1)+11.0D0/78.0D0*TEMPAMP(2)
       AMP2CF(2)=6.0D0/65.0D0*TEMPAMP(1)+2.0D0/429.0D0*TEMPAMP(2)
     &           +3.0D0/22.0D0*TEMPAMP(3)
	 AMP2CF(3)=27.0D0/130.0D0*TEMPAMP(1)+3.0D0/286.0D0*TEMPAMP(2)
     &           +1.0D0/66.0D0*TEMPAMP(3)
C...SMATVAL NEED NOT TO BE NORMALIZED TO THE WHOLE AMPLITUDE, 
C...SINCE WE ONLY NEED THE RELATIVE PROBABILITY FOR EACH COLOR-FLOW.
	 SMATVAL=AMP2CF(1)+AMP2CF(2)+AMP2CF(3)

C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES.<--------> DOUBLE PRECISION
	IF(amps20_1p1.LT.1.0D-16) amps20_1p1=1.0D-16
	DO II=1,3
	   IF(AMP2CF(II).LT.1.0D-16) AMP2CF(II)=1.0D-16
	END DO
	IF(SMATVAL.LT.1.0D-16) SMATVAL=1.0D-16
      
	return
      end

C**********************************************************************
      DOUBLE PRECISION FUNCTION amps2_1p1() 
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
      DIMENSION ep1(4,2:3), ep2(4,2:3), ep3(4,2:4)

      amps2_1p1=0.d0

      call genpolar(pp(1,1),ep1)
      call genpolar(pp(1,2),ep2)
      call genpolar3(pp(1,3),ep3,hbcm)

C...THE SAME FOR SUBPROCESS AND TEVATRON AND LHC
      DO 400 nb1=2,2 
        DO 401 nb2=2,3 
         DO 402 nc3=2,4
          do i=1,4
            pp(I,6)=ep1(I,nb1)
            pp(I,7)=ep2(I,nb2)
            pp(I,8)=ep3(I,nc3)
          enddo
          amps2_1p1=amps2_1p1+2.0D0*amps20_1p1()
402      continue
401     continue
400   continue

      return
      end

C**********************************************************
C******** TO GET SQUARE AMPLITUDE FOR 3P0   ***************
C**********************************************************
      DOUBLE PRECISION FUNCTION amps20_3p0()
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/CCCC/c36,c35,c34,c33,c32,c31,c30,c29 
     . ,c28,c27,c26,c25,c24,c23,c22,c21,c20,c19,c18,c17 
     . ,c16,c15,c14,c13,c12,c11,c10,c9,c8,c7,c6,c5,c4,c3 
     . ,c2,c1s2,c1s1 
      include 'inclamp.f'
C...COLOR FLOW.
 	COMMON/COLFLOW/AMP2CF(10),SMATVAL
	DIMENSION TEMPAMP(3)

	DO II=1,10
	   AMP2CF(II)=0.0D0
	END DO

       DO 200 m=1,16 
        DO 400 n=1,3 
           c(m,n)=0.d0 
400     continue 
200    continue
      
	 call genppp()

	 call amp1s1_3p0(c1s1) 
       call amp1s2_3p0(c1s2)
       call amp2_3p0(c2)
       call amp3_3p0(c3) 
       call amp4_3p0(c4) 
       call amp5_3p0(c5) 
       call amp6_3p0(c6) 
       call amp7_3p0(c7) 
       call amp8_3p0(c8) 
       call amp9_3p0(c9) 
       call amp10_3p0(c10) 
       call amp11_3p0(c11) 
       call amp12_3p0(c12) 
       call amp13_3p0(c13) 
       call amp14_3p0(c14) 
       call amp15_3p0(c15) 
       call amp16_3p0(c16) 
       call amp17_3p0(c17) 
       call amp18_3p0(c18) 
       call amp19_3p0(c19) 
       call amp20_3p0(c20) 
       call amp21_3p0(c21) 
       call amp22_3p0(c22) 
       call amp23_3p0(c23) 
       call amp24_3p0(c24) 
       call amp25_3p0(c25) 
       call amp26_3p0(c26) 
       call amp27_3p0(c27) 
       call amp28_3p0(c28) 
       call amp29_3p0(c29) 
       call amp30_3p0(c30) 
       call amp31_3p0(c31) 
       call amp32_3p0(c32) 
       call amp33_3p0(c33) 
       call amp34_3p0(c34) 
       call amp35_3p0(c35) 
       call amp36_3p0(c36)
      
	 amps20_3p0=0.0D0

       DO n=1,3
         TEMPAMP(N)=ams1_3p0(n)
	   amps20_3p0=amps20_3p0+TEMPAMP(N)
       ENDDO

       nreduce=4
       amps20_3p0=nreduce*amps20_3p0

C... COLOR-FLOW.
C... NOTE HERE: SINCE THE CROSS-TERMS IN THE COLOR-FLOW ARE NEGLECTED,
C... THE TOTAL SQUARE OF AMPLITUDE DOES NOT EQUAL TO THE SUM OF THE
C... SQUARE OF THE PARTIAL AMPLITUDE (PERTAIN TO A CERTAIN COLOR-FLOW).

	 AMP2CF(1)=6.0D0/65.0D0*TEMPAMP(1)+11.0D0/78.0D0*TEMPAMP(2)
       AMP2CF(2)=6.0D0/65.0D0*TEMPAMP(1)+2.0D0/429.0D0*TEMPAMP(2)
     &           +3.0D0/22.0D0*TEMPAMP(3)
	 AMP2CF(3)=27.0D0/130.0D0*TEMPAMP(1)+3.0D0/286.0D0*TEMPAMP(2)
     &           +1.0D0/66.0D0*TEMPAMP(3)
C...SMATVAL NEED NOT TO BE NORMALIZED TO THE WHOLE AMPLITUDE, 
C...SINCE WE ONLY NEED THE RELATIVE PROBABILITY FOR EACH COLOR-FLOW.
	 SMATVAL=AMP2CF(1)+AMP2CF(2)+AMP2CF(3)

C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES. <------> DOUBLE PRECISION
	IF(amps20_3p0.LT.1.0D-16) amps20_3p0=1.0D-16
	DO II=1,3
	   IF(AMP2CF(II).LT.1.0D-16) AMP2CF(II)=1.0D-16
	END DO
	IF(SMATVAL.LT.1.0D-16) SMATVAL=1.0D-16
      
	return
      end

C********************************** 
      DOUBLE PRECISION FUNCTION amps2_3p0()
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
      DIMENSION ep1(4,2:3), ep2(4,2:3)
      
	amps2_3p0=0.D0

      call genpolar(pp(1,1),ep1)
      call genpolar(pp(1,2),ep2)

C...THE SAME FOR SUBPROCESS AND TEVATRON AND LHC
      DO 400 nb1=2,2
        DO 401 nb2=2,3
         do i=1,4
           pp(I,6)=ep1(I,nb1)
           pp(I,7)=ep2(I,nb2)
         enddo
         amps2_3p0=amps2_3p0+2.0D0*amps20_3p0()
401     continue
400   continue

      RETURN
      END
C**********************************************************
C******** TO GET SQUARE AMPLITUDE FOR 3P1   ***************
C**********************************************************
      DOUBLE PRECISION FUNCTION amps20_3p1()
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/CCCC/c36,c35,c34,c33,c32,c31,c30,c29 
     . ,c28,c27,c26,c25,c24,c23,c22,c21,c20,c19,c18,c17 
     . ,c16,c15,c14,c13,c12,c11,c10,c9,c8,c7,c6,c5,c4,c3 
     . ,c2,c1s2,c1s1 
      include 'inclamp.f'
C...COLOR FLOW.
 	COMMON/COLFLOW/AMP2CF(10),SMATVAL
	DIMENSION TEMPAMP(3)

	DO II=1,10
	   AMP2CF(II)=0.0D0
	END DO

      Do 200 m=1,48 
       Do 400 n=1,3 
         c(m,n)=0.0d0 
400    continue 
200   continue 
      
	call genppp()

      call amp1s1_3p1(c1s1) 
      call amp1s2_3p1(c1s2) 
      call amp2_3p1(c2) 
      call amp3_3p1(c3) 
      call amp4_3p1(c4) 
      call amp5_3p1(c5) 
      call amp6_3p1(c6) 
      call amp7_3p1(c7) 
      call amp8_3p1(c8) 
      call amp9_3p1(c9) 
      call amp10_3p1(c10) 
      call amp11_3p1(c11) 
      call amp12_3p1(c12) 
      call amp13_3p1(c13) 
      call amp14_3p1(c14) 
      call amp15_3p1(c15) 
      call amp16_3p1(c16) 
      call amp17_3p1(c17) 
      call amp18_3p1(c18) 
      call amp19_3p1(c19) 
      call amp20_3p1(c20) 
      call amp21_3p1(c21) 
      call amp22_3p1(c22) 
      call amp23_3p1(c23) 
      call amp24_3p1(c24) 
      call amp25_3p1(c25) 
      call amp26_3p1(c26) 
      call amp27_3p1(c27) 
      call amp28_3p1(c28) 
      call amp29_3p1(c29) 
      call amp30_3p1(c30) 
      call amp31_3p1(c31) 
      call amp32_3p1(c32) 
      call amp33_3p1(c33) 
      call amp34_3p1(c34) 
      call amp35_3p1(c35) 
      call amp36_3p1(c36) 
      
	amps20_3p1=0.0d0

      do 600 n=1,3
         call genpppn(n)
         tempamp(n)=ams1_3p1(n)
	   amps20_3p1=amps20_3p1+tempamp(n)
  600 continue

      nreduce=4
      amps20_3p1=nreduce*amps20_3p1

C... COLOR-FLOW.
C... NOTE HERE: SINCE THE CROSS-TERMS IN THE COLOR-FLOW ARE NEGLECTED,
C... THE TOTAL SQUARE OF AMPLITUDE DOES NOT EQUAL TO THE SUM OF THE
C... SQUARE OF THE PARTIAL AMPLITUDE (PERTAIN TO A CERTAIN COLOR-FLOW).

	 AMP2CF(1)=6.0D0/65.0D0*TEMPAMP(1)+11.0D0/78.0D0*TEMPAMP(2)
       AMP2CF(2)=6.0D0/65.0D0*TEMPAMP(1)+2.0D0/429.0D0*TEMPAMP(2)
     &           +3.0D0/22.0D0*TEMPAMP(3)
	 AMP2CF(3)=27.0D0/130.0D0*TEMPAMP(1)+3.0D0/286.0D0*TEMPAMP(2)
     &           +1.0D0/66.0D0*TEMPAMP(3)
C...SMATVAL NEED NOT TO BE NORMALIZED TO THE WHOLE AMPLITUDE, 
C...SINCE WE ONLY NEED THE RELATIVE PROBABILITY FOR EACH COLOR-FLOW.
	 SMATVAL=AMP2CF(1)+AMP2CF(2)+AMP2CF(3)

C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES. <----->DOUBLE PRECISION
	IF(amps20_3p1.LT.1.0D-16) amps20_3p1=1.0D-16
	DO II=1,3
	   IF(AMP2CF(II).LT.1.0D-16) AMP2CF(II)=1.0D-16
	END DO
	IF(SMATVAL.LT.1.0D-16) SMATVAL=1.0D-16

      return
      end

C******************************** 
      DOUBLE PRECISION FUNCTION amps2_3p1() 
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
      DIMENSION ep1(4,2:3), ep2(4,2:3), ep3(4,2:4) 

      amps2_3p1=0.0D0

      call genpolar(pp(1,1),ep1)
      call genpolar(pp(1,2),ep2)
      call genpolar3(pp(1,3),ep3,hbcm)

C...THE SAME FOR SUBPROCESS AND TEVATRON AND LHC
	DO 400 nb1=2,2 
        DO 401 nb2=2,3 
         DO 402 nc3=2,4 
           do i=1,4
             pp(I,6)=ep1(I,nb1)
             pp(I,7)=ep2(I,nb2)
             pp(I,8)=ep3(I,nc3)
           enddo
           amps2_3p1=amps2_3p1+2.0D0*amps20_3p1()
402      continue
401     continue
400   continue

      return
      end

C**********************************************************
C******** TO GET SQUARE AMPLITUDE FOR 3P2   ***************
C**********************************************************
      DOUBLE PRECISION FUNCTION amps20_3p2() 
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/CCCC/c36,c35,c34,c33,c32,c31,c30,c29 
     . ,c28,c27,c26,c25,c24,c23,c22,c21,c20,c19,c18,c17 
     . ,c16,c15,c14,c13,c12,c11,c10,c9,c8,c7,c6,c5,c4,c3 
     . ,c2,c1s2,c1s1 
      include 'inclamp.f'
C...COLOR FLOW.
 	COMMON/COLFLOW/AMP2CF(10),SMATVAL
	DIMENSION TEMPAMP(3)

	DO II=1,10
	   AMP2CF(II)=0.0D0
	END DO

      Do 200 m=1,94
        Do 400 n=1,3
          c(m,n)=0.0D0
400     continue 
200   continue 

      call genppp() 
      call amp1s1_3p2(c1s1) 
      call amp1s2_3p2(c1s2) 
      call amp2_3p2(c2) 
      call amp3_3p2(c3) 
      call amp4_3p2(c4) 
      call amp5_3p2(c5) 
      call amp6_3p2(c6) 
      call amp7_3p2(c7) 
      call amp8_3p2(c8) 
      call amp9_3p2(c9) 
      call amp10_3p2(c10) 
      call amp11_3p2(c11) 
      call amp12_3p2(c12) 
      call amp13_3p2(c13) 
      call amp14_3p2(c14) 
      call amp15_3p2(c15) 
      call amp16_3p2(c16) 
      call amp17_3p2(c17) 
      call amp18_3p2(c18) 
      call amp19_3p2(c19) 
      call amp20_3p2(c20) 
      call amp21_3p2(c21) 
      call amp22_3p2(c22) 
      call amp23_3p2(c23) 
      call amp24_3p2(c24) 
      call amp25_3p2(c25) 
      call amp26_3p2(c26) 
      call amp27_3p2(c27) 
      call amp28_3p2(c28) 
      call amp29_3p2(c29) 
      call amp30_3p2(c30) 
      call amp31_3p2(c31) 
      call amp32_3p2(c32) 
      call amp33_3p2(c33) 
      call amp34_3p2(c34) 
      call amp35_3p2(c35) 
      call amp36_3p2(c36)

	amps20_3p2=0.0D0

      DO n=1,3
         call genpppn(n)
	   TEMPAMP(N)=ams1_3p2(n)
	   amps20_3p2=amps20_3p2+TEMPAMP(N)
      ENDDO

	nreduce=4
      amps20_3p2=nreduce*amps20_3p2

C... COLOR-FLOW.
C... NOTE HERE: SINCE THE CROSS-TERMS IN THE COLOR-FLOW ARE NEGLECTED,
C... THE TOTAL SQUARE OF AMPLITUDE DOES NOT EQUAL TO THE SUM OF THE
C... SQUARE OF THE PARTIAL AMPLITUDE (PERTAIN TO A CERTAIN COLOR-FLOW).

	 AMP2CF(1)=6.0D0/65.0D0*TEMPAMP(1)+11.0D0/78.0D0*TEMPAMP(2)
       AMP2CF(2)=6.0D0/65.0D0*TEMPAMP(1)+2.0D0/429.0D0*TEMPAMP(2)
     &           +3.0D0/22.0D0*TEMPAMP(3)
	 AMP2CF(3)=27.0D0/130.0D0*TEMPAMP(1)+3.0D0/286.0D0*TEMPAMP(2)
     &           +1.0D0/66.0D0*TEMPAMP(3)
C...SMATVAL NEED NOT TO BE NORMALIZED TO THE WHOLE AMPLITUDE, 
C...SINCE WE ONLY NEED THE RELATIVE PROBABILITY FOR EACH COLOR-FLOW.
	 SMATVAL=AMP2CF(1)+AMP2CF(2)+AMP2CF(3)

C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES. <------>DOUBLE PRECISION
	IF(amps20_3p2.LT.1.0D-16) amps20_3p2=1.0D-16
	DO II=1,3
	   IF(AMP2CF(II).LT.1.0D-16) AMP2CF(II)=1.0D-16
	END DO
	IF(SMATVAL.LT.1.0D-16) SMATVAL=1.0D-16

      return
      end
c******************************************* 
      DOUBLE PRECISION FUNCTION amps2_3p2() 
      IMPLICIT DOUBLE PRECISION (A-H,O-Z) 
      IMPLICIT INTEGER (I-N) 
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
      DIMENSION ep1(4,2:3), ep2(4,2:3), ept3(4,4,2:6) 
      
	amps2_3p2=0.0d0

      call genpolar(pp(1,1),ep1)
      call genpolar(pp(1,2),ep2)
      call gentensor(pp(1,3),ept3,hbcm)

C...THE SAME FOR SUBPROCESS AND TEVATRON AND LHC
	DO 400 nb1=2,2
        DO 401 nb2=2,3 
         DO 402 nt3=2,6 
          do i=1,4
           pp(I,6)=ep1(I,nb1)
           pp(I,7)=ep2(I,nb2)
          enddo
          call def_p_eq_p_dot_tp(8,4,ept3,nt3)
          call def_p_eq_p_dot_tp(9,5,ept3,nt3)
          call def_p_eq_p_dot_tp(10,1,ept3,nt3)
          call def_p_eq_p_dot_tp(11,2,ept3,nt3)
          call def_p_eq_p_dot_tp(12,7,ept3,nt3)
          call def_p_eq_p_dot_tp(13,6,ept3,nt3)
          amps2_3p2=amps2_3p2+2.0D0*amps20_3p2()
402      continue
401     continue
400   continue

      RETURN
	END