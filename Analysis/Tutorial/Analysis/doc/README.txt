!-----------------------------------------------------------------------------
! Package     : Tutorial/Analysis
! Responsible : P. Koppenburg
! Purpose     : Analysis tutorial
!-----------------------------------------------------------------------------
 
The exercises are to be found in the exercise/ directory
  
  - exercise 1 asks you to write a basic DVAlgorithm
  - exercise 2 asks you to write a basic DVAlgorithm doing J/psi -> mu mu

  - exercise 2 extends this algorithm to Phi->KK
  - exercise 3 completes the Bs->J/PsiPhi chain using generic code
  - exercise 4 shows some more nice utility algorithms
  - exercise 5 lets you look for tagging and trigger
  - exercise 6 is about association to MC truth

For those who want to cheat:
     
    > DaVinciEnv v17r5   (or more if available)
    > cd cmt/
    > cmt config
    > source setup.csh
    > cp ../solutions/DaVinci1/*.{cpp,h} ../src/
    > make
    > DaVinci ../solutions/DaVinci1/DVTutorial_1.opts
  
