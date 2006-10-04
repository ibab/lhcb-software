C $Id: GaussUserProcess.inc,v 1.1 2006-10-04 14:16:50 ibelyaev Exp $
C----------------------------------------------------------------------
C CVS tag $Name: not supported by cvs2svn $m version $Revision: 1.1 $
C----------------------------------------------------------------------
C $Log: not supported by cvs2svn $
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
C----------------------------------------------------------------------
C The END 
C----------------------------------------------------------------------
