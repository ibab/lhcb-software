!-----------------------------------------------------------------------------
! Package     : Tutorial/Analysis
! Responsible : P. Koppenburg
! Purpose     : Analysis tutorial
!-----------------------------------------------------------------------------
 
The exercises are to be found in the exercise/ directory
  
  - exercise 1 asks you to write a basic DVAlgorithm
  - exercise 2 asks you to write a basic DVAlgorithm doing J/psi -> mu mu
  - exercise 3 extends this algorithm to Phi->KK
  - exercise 4 completes the Bs->J/PsiPhi chain using generic code
  - exercise 5 shows some more nice utility algorithms
  - exercise 6 lets you look for tagging, trigger and association to MC truth

For those who want to cheat:
     
    > SetupProject 
    > cd cmt/
    > cmt config
    > source setup.csh
    > cp ../solutions/DaVinci1/*.{cpp,h} ../src/
    > make
    > gaudirun.py ../solutions/DaVinci1/DVTutorial_1.py
  
