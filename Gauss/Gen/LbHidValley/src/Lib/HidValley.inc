C $Id: HidValley.inc,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C =====================================================================
C CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
C =====================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C
C =====================================================================
C  @file 
C  Purpose : local common block for "Hidded Valley" production, 
C            The lines comes from HV0.403 code by Matt Strassler
C            <strasslr@noether.phys.washington.edu>
C
C  Matt Strassler <strasslr@noether.phys.washington.edu>:
C
C  "This program simulates q qbar -> Z' -> v-quarks, followed by
C  v-showering and v-hadronization to v-pions.  The current program
C  makes various approximations and is not suitable for precision 
C  studies, but should be good at the factor-of-two level.
C  In particular, the v-hadronization code uses Pythias QCD
C  hadronization code, scaled up in energy, and this is not a perfect 
C  simulation of the real model. Also, the Z' charges are not precisely 
C  those discussed in ref [1]; but in any case the model of [1] was 
C  just an example, and many other models exist.  Updates to this 
C  program will gradually correct these issues."
C
C  @author: 
C  @date: 2006-10-02
C =====================================================================
      DOUBLE PRECISION ZPMASS,PIMASS,PIZLIF,PIPLIF,BEAMENE
      COMMON/HVDATA/ZPMASS,PIMASS,PIZLIF,PIPLIF,BEAMENE
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
      
