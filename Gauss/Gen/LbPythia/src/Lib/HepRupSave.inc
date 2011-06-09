C $Id: $
C----------------------------------------------------------------------
C  Purpose : 
C
C  Author  : Vanya Belyaev
C  Created : 2011-06-09
C----------------------------------------------------------------------
      INTEGER          
     &  SIDBMUP , 
     &  SPDFGUP , 
     &  SPDFSUP , 
     &  SIDWTUP , 
     &  SNPRUP  , 
     &  SLPRUP

      DOUBLE PRECISION 
     &  SEBMUP  , 
     &  SXSECUP , 
     &  SXERRUP ,
     &  SXMAXUP
      
      COMMON / HepRupSave /
     &  SIDBMUP ( 2 ) ,
     &  SEBMUP  ( 2 ) ,
     &  SPDFGUP ( 2 ) ,
     &  SPDFSUP ( 2 ) ,
     &  SIDWTUP ,
     &  SNPRUP  ,
     &  SXSECUP ( MAXPUP ) ,
     &  SXERRUP ( MAXPUP ) ,
     &  SXMAXUP ( MAXPUP ) ,
     &  SLPRUP  ( MAXPUP )
      
C----------------------------------------------------------------------
C    The END 
C----------------------------------------------------------------------
