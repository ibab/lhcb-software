C $Id: GaussUserProcess.inc,v 1.2 2007-06-29 13:37:19 ibelyaev Exp $
C----------------------------------------------------------------------
C  Purpose : 
C
C  @author  : 
C  @date    : 2006-10-03
C----------------------------------------------------------------------
      INTEGER PYTHIAGAUSSPROCESS
      COMMON /GAUSSUSERPROCESS/ PYTHIAGAUSSPROCESS
C*    "Standard Pythia Action" - read the event in "old" LHA format
      INTEGER LHAREADPROCESS
      PARAMETER(LHAREADPROCESS=0)
C*    Use Acer_MC
      INTEGER ACERMCPROCESS
      PARAMETER(ACERMCPROCESS=1)
C*    Use BcVegPy 
      INTEGER BCVEGPYPROCESS
      PARAMETER(BCVEGPYPROCESS=2)
C*    Read the event in "new" (xml-like) LHE format
      INTEGER LHEREADPROCESS
      PARAMETER(LHEREADPROCESS=3)
C*    Use Hidden Valley production
      INTEGER HIDVALLEYPROCESS
      PARAMETER(HIDVALLEYPROCESS=4)
C*    Read ALPGEN partonic event 
      INTEGER ALPGENREADPROCESS
      PARAMETER(ALPGENREADPROCESS=5)
C----------------------------------------------------------------------
C The END 
C----------------------------------------------------------------------
