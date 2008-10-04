#!/usr/bin/env python 
# =============================================================================
# $Id: Bs2Jpsiphi_mm_data.py,v 1.1 2008-10-04 16:14:47 ibelyaev Exp $
# =============================================================================
## @file
# 
#  Helper python module to define the input data for LoKi example
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-10-04
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaebv@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $,  version $Revision: 1.1 $ "
# =============================================================================
__all__ = (
    'Files'  , ## useful for iEventSelector.open input (GaudiPython) 
    'Inputs' , ## useful for EventSelector ( Input = ... ) (Configurables)
    'LFNs'     ## the list of proper LFNs (for Ganga&Dirac?)
    ) 
# =============================================================================
import os, logging 

Files_MCSELECTED = (
    'Bs2PsiPhi.dst'   ,
    'Bs2PsiPhi_1.dst' ,
    'Bs2PsiPhi_2.dst' , 
    'Bs2PsiPhi_3.dst' 
    )
CERN_LFN_Path = "LFN:/lhcb/production/DC06/phys-v2-lumi2/00001657/DST/0000/"
LFNs = [ CERN_LFN_Path+'00001657_00000%03d_5.dst'%i for i in range(1,171) ] 


Vanya_Local_Paths = (
    "C:/data"                  , ## my Win32 laptop VANYADELL
    "/data/bfys/vanya/JpsiPhi"   ## NIKHEF local disks 
    )

Vanya_AFS_Path    = '/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA'
Vanya_Castor_Path = "/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples"


## check the presence of "interesting files" 
def checkFiles ( files                     ,
                 local = Vanya_Local_Paths ) :
    """
    Check the presence of the interesting files 
    """
    Files = []
    
    for f in files :

        found = False 
        for lp in local : 
            fname = lp + '/' + f
            if os.path.exists ( fname ) :
                Files.append ( "PFN:" + fname )
                found = True 
                continue     ## CONTINUE
        if found : continue  ## CONTINUE 
        ## 2) check the AFS directory
        fname = Vanya_AFS_Path + '/'  + f
        if os.path.exists ( fname ) :
            Files.append ( "PFN:" + fname )
            continue                              ## CONTINUE
        ## 3) check the CASTOR 
        if 'CASTOR_HOME' not in os.environ : continue
        fname = Vanya_Castor_Path + '/' + f
        import popen2
        cout,cin,cerr = popen2.popen3 ( ' nsls -l '  + fname ) 
        # count lines:
        lo = 0
        for l in cout :  lo+=1 
        le = 0
        for l in cerr :  le+=1
        if 1 == lo and 0 == le :
            Files.append ( 'PFN:castor:' + fname )
            ## try to pre-stage it 
            cout,cin,cerr = popen2.popen3 ( 'stager_get -M ' + fname )
            for l in cerr : print l
            
    return Files

## transform the list of files into the list of "Input"-configurtaion strings 
def getInputs ( files ) :
    ## transform the list of files into the list of "Input"-configurtaion strings 
    return [ "DATAFILE='%s' TYP='POOL_ROOTTREE' OPT='READ'"%f for f in files ] 

# =============================================================================



Files = checkFiles ( Files_MCSELECTED )
if not Files : Files = LFNs     
Inputs = getInputs ( Files )   

# =============================================================================
if '__main__' == __name__ :  ## just for pure testing purposes 
    print 'Files: '
    for f in Files  : print '\t' , f 
    print 'Inputs: '
    for i in Inputs : print '\t' , i 
    
# =============================================================================
# The END 
# =============================================================================



