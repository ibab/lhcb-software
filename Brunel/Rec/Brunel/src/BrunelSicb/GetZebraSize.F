      SUBROUTINE GETZEBRASIZE(size)
*****************************************************************
*                                                               *
*   Subroutine GETZEBRASIZE                                     *
*                                                               *
*   Returns ZEBRA size that will be used to initialise ZEBRA    *
*                                                               *                                                               *
*   Author: M.Cattaneo 20000622                                 *
*   modified by:                                                *
*                                                               *
*****************************************************************
      IMPLICIT NONE
#include "BrunelZebra.inc"
	INTEGER size

	size = NWGEAN

      RETURN
      END
