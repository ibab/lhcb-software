#!/usr/bin/env python
# $Id: MCJPsiKstar.py,v 1.1 2004-03-16 12:06:28 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to check MC for B -> J/psi K* channel 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-03-16 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 

g.JobOptionsType = 'NONE'
g.OutputLevel = 3
g.config()

global h1 
# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class MCJPsiKstar(Algo):
    " My own analysis algorithm for MC checks for B0 -> J/psi K* channel "
    def analyse ( self ) :
        
        mc  = self.mctruth()
        Bmc = mc.find ( decay = "[B0 -> J/psi(1S) K*(892)0]cc" )
        print Bmc.size()
        lst = Seq2List  ( Bmc )
        print lst 
        tup = Seq2Tuple ( Bmc )
        print tup 
        
        return SUCCESS 
    
# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ] ,
               options = [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                           'HcalPIDmu.OutputLevel     =   5  ' ,
                           'EcalPIDe.OutputLevel      =   5  ' ,
                           'HcalPIDe.OutputLevel      =   5  ' ,
                           'BremPIDe.OutputLevel      =   5  ' ,
                           'PrsPIDe.OutputLevel       =   5  ' ,
                           'NeutralPP2MC.OutputLevel  =   5  ' ,
                           'EventSelector.PrintFreq   =  50  ' ] )

# define input data channel B0 -> J/Psi K*  
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/411500.opts')

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

# create analysis algorithm and add it to the list of
alg = MCJPsiKstar('MC')

g.topAlg += [ 'MC' ]

alg = gaudi.iProperty('MC')
alg.OutputLevel = 5
alg.NTupleLUN    = 'MC'

desktop                 = g.property('Dstar.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged"]

# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'mc.hbook'

nsvc = gaudi.iProperty( 'NTupleSvc' )
nsvc.Output =[ "DSTAR DATAFILE='mc_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

# =============================================================================
# job execution 
# =============================================================================


g.initialize()


## g.run(100)  ## crash !!
g._evtpro.executeRun(10)

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
