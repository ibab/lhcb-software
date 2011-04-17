C $Id: Pythia.inc,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C =====================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
C =====================================================================
C  Purpose : 
C
C  Author  : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created : 2006-10-22
C =====================================================================

CCC #include "pythia_cms.inc"
C...PYTHIA commonblocks for most purposes
C$$      
C...The event record.
      INTEGER N,NPAD,K
      DOUBLE PRECISION P,V,P1X,P1Y,P1Z,P2X,P2Y,P2Z
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      
C...Parameters.
      INTEGER MSTU,MSTJ
      DOUBLE PRECISION PARU,PARJ
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      
C...Particle properties + some flavour parameters.
      INTEGER KCHG
      DOUBLE PRECISION PMAS,PARF,VCKM  
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      
C...Decay information.
      INTEGER MDCY,MDME,KFDP
      DOUBLE PRECISION BRAT
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      
C...Random number generator information.
      INTEGER MRPY
      DOUBLE PRECISION RRPY
      COMMON/PYDATR/MRPY(6),RRPY(100)
      
C...Selection of hard scattering subprocesses.
      INTEGER MSEL,MSELPD,MSUB,KFIN
      DOUBLE PRECISION CKIN
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      
C...Parameters. 
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

      INTEGER MINT
      DOUBLE  PRECISION VINT 
      COMMON/PYINT1/MINT(400),VINT(400)
           
      
C...EXTERNAL statement links PYDATA on most machines.
      EXTERNAL PYDATA      
      
      INTEGER  PYCOMP 
      EXTERNAL PYCOMP 
      
      DOUBLE  PRECISION PYP
      EXTERNAL PYP 

C =====================================================================
C The END 
C =====================================================================
      
