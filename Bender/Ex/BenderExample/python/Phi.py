#!/usr/bin/env python
# =============================================================================
# $Id: Phi.py,v 1.7 2004-03-17 10:18:18 ibelyaev Exp $
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
from   bendermodule  import *
import benderconfig  as bender 
import benderPreLoad as preload

global h1 
# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Phi(Algo):
    " My own analysis algorithm for selection of phi -> K+ K- "
    def analyse ( self ) :
        # get kaons 
        kplus  = self.select( tag="K+" , cuts =  321 == ID )
        kminus = self.select( tag="K-" , cuts = -321 == ID )
        # create loop
        phis   = self.loop( formula = "K+ K-" , pid = "phi(1020)" )
        # loop over KK combinations 
        for phi in phis:
            if phi.mass(1,2) > 1050 * MeV : continue
            global h1
            h1 = self.plot( title = "K+ K- mass"      ,
                            value = M ( phi ) / MeV   ,
                            low   = 1000              ,
                            high  = 1050              )
            phi.save('phi')
            
        return SUCCESS 
        

# =============================================================================
# Generic job configuration & input data 
# =============================================================================

bender.config( files   = [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ] ,
               options = [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                           'HcalPIDmu.OutputLevel     =   5  ' ,
                           'EcalPIDe.OutputLevel      =   5  ' ,
                           'HcalPIDe.OutputLevel      =   5  ' ,
                           'BremPIDe.OutputLevel      =   5  ' ,
                           'PrsPIDe.OutputLevel       =   5  ' ,
                           'EventSelector.PrintFreq   =  10  ' ] )

# input data 
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/615300.opts')

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

# preload algorithm(s)
g.topAlg += ['LoKiPreLoad/Hadrons']
preload.Hadrons( Particles = [ 'kaon' , 'pion'] )

# create analysis algorithm and add it to the list of
phi = Phi('Phi')

g.topAlg += [ 'Phi' ]

phi = gaudi.iProperty('Phi')
phi.OutputLevel = 5

desktop                 = g.property('Phi.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Hadrons"]

# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'phi.hbook'


# =============================================================================
# job execution 
# =============================================================================

g.run(500) 

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
