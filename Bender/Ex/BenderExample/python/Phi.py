#!/usr/bin/env python
# =============================================================================
# $Id: Phi.py,v 1.2 2004-01-22 22:35:43 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Python based phi -> K+ K- "analysis"
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 

g.JobOptionsType = 'NONE'
g.OutputLevel = 3
g.config()

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Phi(Algo):
    " My own analysis algorithm for selection of phi -> K+ K- "
    def __init__(self, name = 'Phi' ) :
        Algo.__init__( self, name )
        
    def analyse ( self ) :
        # get kaons 
        kplus  = self.select( tag="K+" , cuts =  321 == ID )
        kminus = self.select( tag="K-" , cuts = -321 == ID )
        # create loop
        phis   = self.loop( formula = "K+ K-" , pid = "phi(1020)" )
        # loop over KK combinations 
        for phi in phis:
            if phi.mass(1,2) > 1050 * MeV : continue
            self.plot( title = "K+ K- mass"      ,
                       value = M ( phi ) / MeV   ,
                       low   = 1000              ,
                       high  = 1050              )
            
        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ '$DAVINCIROOT/options/DaVinci.opts' ] ,
               options = [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                           'HcalPIDmu.OutputLevel     =   5  ' ,
                           'EcalPIDe.OutputLevel      =   5  ' ,
                           'HcalPIDe.OutputLevel      =   5  ' ,
                           'BremPIDe.OutputLevel      =   5  ' ,
                           'PrsPIDe.OutputLevel       =   5  ' ,
                           'EventSelector.PrintFreq   =  10  ' ] )

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

# preload algorithm(s)
g.topAlg += [ "LoKiPreLoad/Charged"]

# configure the preload algorithm(s) 
preload  = g.property('Charged')
preload.OutputLevel = 4

desktop                   = g.property('Charged.PhysDesktop')
desktop.ParticleMakerType = "CombinedParticleMaker"
desktop.OutputLocation    = "/Event/Phys/Charged"

maker                    = g.property('Charged.PhysDesktop.CombinedParticleMaker')
maker.ExclusiveSelection = 0
maker.Particles          = [ "kaon" , "pion" ]
maker.KaonSelection      = [ "det='RICH' k-pi='-5.0'   k-p='-5.0' "]
maker.PionSelection      = [ "det='RICH' pi-k='-5.0' "]

# create analysis algorithm and add it to the list of
phi = Phi('Phi')

g.topAlg += [ 'Phi' ]

phi = gaudi.iProperty('Phi')
phi.OutputLevel = 5

desktop                 = g.property('Phi.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged"]
desktop.OutputLocation  =   "/Event/Phys/Phi"

# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'phi.hbook'

# input data 
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/615300.opts')

# =============================================================================
# job execution 
# =============================================================================

g.initialize()

## g.run(100)  ## crash !!
g._evtpro.executeRun(100)

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2003/12/07 16:14:30  ibelyaev
#  update for Bender application and project
# 
# =============================================================================
