C $Id: GaussUserProcess.inc,v 1.1 2007-01-12 15:21:17 ranjard Exp $
C----------------------------------------------------------------------
C CVS tag $Name: not supported by cvs2svn $m version $Revision: 1.1 $
C----------------------------------------------------------------------
C $Log: not supported by cvs2svn $
C Revision 1.2  2006/10/25 11:29:45  ibelyaev
C  fixed to allow usage of Hidden Valley generator
C
C Revision 1.1  2006/10/04 14:16:50  ibelyaev
C  possibility to read LHE files + maney minor improvements
C
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
C----------------------------------------------------------------------
C The END 
C----------------------------------------------------------------------
