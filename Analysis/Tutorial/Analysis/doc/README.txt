 !-----------------------------------------------------------------------------
! Package     : Tutorial/Analysis
! Responsible : P. Koppenburg
! Purpose     : Analysis tutorial
!-----------------------------------------------------------------------------
 
  1) You are welcome to write the codee yourself, following the tutorial
  
  2) To get it to work quickly, go through the following:
     
    > cd $DAVINCIROOT/cmt/
    # add  to requirements:
         use Analysis v4 Tutorial  
    > cmt config
    > source setup.csh
    > cd $ANALYSISROOT
    > cp solution/TutorialAlgorithm.* src/
    > cd cmt
    > make
    > DaVinci ../options/DVTutorial_1.opts
  
  3) To alos have the Phi, the Bs and the efficiency:   

    > DaVinci ../options/DVTutorial_2.opts

