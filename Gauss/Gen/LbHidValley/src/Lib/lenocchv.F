*
* $Id: lenocchv.F,v 1.1 2007-01-26 14:54:11 ibelyaev Exp $
*
* CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
*
* $Log: not supported by cvs2svn $
*
*
      FUNCTION LENOCCHV (CHV)

C-    Find last non-blank character in CHV

      CHARACTER    CHV*(*)

      N = LEN(CHV)

      DO 17  JJ= N,1,-1
      IF (CHV(JJ:JJ).NE.' ') GO TO 99
   17 CONTINUE
      JJ = 0

   99 LENOCCHV = JJ
      RETURN
      END
