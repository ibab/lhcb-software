!-----------------------------------------------------------------------------
! Package     : Tutorial/Analysis
! Responsible : P. Koppenburg
! Purpose     : Analysis tutorial
!-----------------------------------------------------------------------------
 
The exercises are to be found in the same directory.
  
  - exercise 1 asks you to write a basic DVAlgorithm doing J/psi -> mu mu
  - exercise 2 extends this algorithm to Phi->KK
  - exercise 3 completes the Bs->J/psiPhi chain using generic code
  - exercise 4 is about efficiencies and correlations
  - exercise 5 lets you look for tagging and trigger
  - exercise 6 is about MC truth

For those who want to cheat:
     
    > DaVinciEnv v12r14   (or more if available)
    > cd cmt/
    > cmt config
    > source setup.csh
    > cp ../solution1/TutorialAlgorithm.* ../src/
    > make
    > DaVinci ../solution1/DVTutorial_1.opts
  
