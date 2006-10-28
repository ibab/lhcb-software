C $Id: HidValley.inc,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C =====================================================================
C CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
C =====================================================================
C $Log: not supported by cvs2svn $
C =====================================================================
C  @file 
C  Purpose : local common block for "Hidded Valley" production, 
C            The lines comes from HV0.403 code by Matt Strassler
C            <strasslr@noether.phys.washington.edu>
C
C  @author: 
C  @date: 2006-10-02
C =====================================================================
      DOUBLE PRECISION ZPMASS,PIMASS,PIZLIF,PIPLIF
      COMMON/HVDATA/ZPMASS,PIMASS,PIZLIF,PIPLIF
      DOUBLE  PRECISION QEE,QPX,QPY,QPZ,QMS
      INTEGER IQTYP 
      COMMON/QDATA1/QEE(5),QPX(5),QPY(5),QPZ(5),QMS(5),IQTYP(5)
      DOUBLE PRECISION QQVX,QQVY,QQVZ,QQTHT,QQPHI
      COMMON/QDATA2/QQVX,QQVY,QQVZ,QQTHT,QQPHI
      INTEGER NDIM
      PARAMETER(NDIM=50) 
      INTEGER NTYP,NCNTR
      DOUBLE PRECISION NEE,NPX,NPY,NPZ
      COMMON/QDATA3/NCNTR,
     &  NTYP(NDIM),NEE(NDIM),NPX(NDIM),NPY(NDIM),NPZ(NDIM)    
C === technical parameters =============================================
      INTEGER SUCCESS 
      PARAMETER (SUCCESS=0)
      INTEGER FAILURE 
      PARAMETER (FAILURE=1)
C === v-pions =========================================================
      INTEGER  IDPIZ,IDPIP
      PARAMETER(IDPIZ=35)
      PARAMETER(IDPIP=36)
C =====================================================================

C =====================================================================
C The END 
C =====================================================================
      
