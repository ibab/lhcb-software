!-----------------------------------------------------------------------------
! Package     : Tutorial/Analysis
! Responsible : P. Koppenburg
! Purpose     : Analysis tutorial
!-----------------------------------------------------------------------------
 
  1) You are welcome to write the code yourself, following the tutorial
  
  2) To get it to work quickly, go through the following:
     
    > DaVinciEnv v12r3   (or more if available)
    > cd cmt/
    > cmt config
    > source setup.csh
    > cp ../solution/TutorialAlgorithm.* ../src/
    > make
    > DaVinci ../options/DVTutorial_1.opts
  
  3) To also have the Phi, the Bs and the efficiency:   

    > DaVinci ../options/DVTutorial_2.opts

  4) A solution of the "exercise" that extends the
     TutorialAlgorithm to also accept phi's can be used with

    > cp ../solution2/TutorialAlgorithm.* ../src/
    > make
    > DaVinci ../options/DVTutorial_3.opts

