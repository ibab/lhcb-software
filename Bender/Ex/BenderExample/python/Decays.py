#!/usr/bin/env python
# $Id: Decays.py,v 1.1 2004-06-29 06:41:52 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run D*+ -> ( D0 -> K- pi+ ) pi+ selection with Bender
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig  as bender 

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class Decays(Algo):
    " My own analysis algorithm for selection members of D0->(K- pi+) decay "
    def analyse ( self ) :
        
        mc  = self.mctruth()
        trees = mc.find ( decay = "[D0 -> K- pi+]cc" )
        print " # of found decay trees          " , trees.size()
        for p in trees :
            print " MCID of the particle " , MCID( p )
            
        n1 = mc.find ( decay = "[D0 -> ^K- ^pi+]cc" )
        print " # of found decay members (all)    " , n1.size()
        for p in n1 :
            print " MCID of the particle " , MCID( p )
            
        n2 = mc.find ( decay = "[D0 -> ^K- pi+]cc" )
        print " # of found decay members (first)  " , n2.size()
        for p in n2 :
            print " MCID of the particle " , MCID( p )
            
        n3 = mc.find ( decay = "[D0 -> K- ^pi+]cc" )
        print " # of found decay members (second) " , n3.size()
        for p in n3 :
            print " MCID of the particle " , MCID( p )            

        n4 = mc.find ( decay = " K- : [D0 -> K- pi+]cc" )
        print " # of found decay members (K-) "     , n4.size()
        for p in n4 :
            print " MCID of the particle " , MCID( p )

        n5 = mc.find ( decay = " K+ : [D0 -> K- pi+]cc" )
        print " # of found decay members (K+) "     , n5.size()
        for p in n5 :
            print " MCID of the particle " , MCID( p )

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
                           'EventSelector.PrintFreq   =  50  ' ] )

# define input data channel B0 -> ( D*- -> D0bar(K+ pi-) pi- ) pi+  
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/415000.opts')

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================


# create analysis algorithm and add it to the list of
alg = Decays('Decays')
g.topAlg += [ 'Decays' ]
alg = gaudi.iProperty('Decays')
alg.OutputLevel = 5

# add 'similar' C++ algorithm from LoKiExample package
g.DLLs   += [ "LoKiExample"            ]
g.topAlg += [ 'LoKi_MCDecays/MCDecays' ]

# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'Decays.hbook'


# =============================================================================
# job execution 
# =============================================================================

g.run(20) 

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
