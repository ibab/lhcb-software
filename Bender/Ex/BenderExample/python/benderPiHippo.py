#!/usr/bin/env python
# =============================================================================
# $Id: benderPiHippo.py,v 1.1 2004-02-13 08:50:16 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file   simple helper python script to load hippoPlotter from PI
# @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
# @date   2004-02-11
# =============================================================================

import hippoPlotter

import AidaProxy

def _plotH1D_(self, h1, opt1, opt2 ) :
    cloud = AidaProxy.Cloud1D( "TMPCloud" + h1.title() , h1.title() )
    for i in range(0,h1.axis().bins() ) :
        for j in range(0,h1.binEntries(i) ) : 
            cloud.fill( h1.binMean(i) )
    return self.plot( cloud , opt1 , opt2 )
            
hippoPlotter.HippoPlotter.plotH1D = _plotH1D_

plotter = hippoPlotter.HippoPlotter()
canvas  = plotter 

# =============================================================================
# The END 
# =============================================================================
