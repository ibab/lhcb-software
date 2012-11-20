      subroutine dijet_init_couplings
      implicit none
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_physpar.h'
      logical  verbose
      parameter(verbose=.true.)
      real *8  dijet_pwhg_alphas
      external dijet_pwhg_alphas

C - Quark masses to produce on-shell quarks in the LH file
C - ... as in Herwig,
C      physpar_mq(1)=0.32     ! up
C      physpar_mq(2)=0.32     ! down
C      physpar_mq(3)=0.50     ! strange
C      physpar_mq(4)=1.55     ! charm
C      physpar_mq(5)=4.95     ! bottom
C      physpar_mq(6)=174.3000 ! top (!)
C - ... or as in Pythia,
      physpar_mq(1)=0.33     ! up
      physpar_mq(2)=0.33     ! down
      physpar_mq(3)=0.50     ! strange
      physpar_mq(4)=1.50     ! charm
      physpar_mq(5)=4.80     ! bottom
      physpar_mq(6)=176.0256 ! top (!)

      if(verbose) then
         write(*,*) ' '
         write(*,*) '--------------------------------------'
         write(*,*) 'POWHEG: alpha_S information'
         write(*,*) 'lambda_QCD     ',st_lambda5MSB
         write(*,'(1X,A,f7.3,A,f15.7)') 'alpha_s(',91.2d0,')',
     $               dijet_pwhg_alphas(91.2d0**2,st_lambda5MSB,st_nlight)
         write(*,*) '--------------------------------------'
      endif

      end


