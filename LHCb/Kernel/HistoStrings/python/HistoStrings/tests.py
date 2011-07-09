#!/usr/bin/env python
# =============================================================================
## @file HistoStrings/tests.py
#  Helper module to test histogram  <--> string conversions 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
# =============================================================================
"""

Helper module to test  histogram <--> string conversion

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $"
## nothing is exported 
__all__     = ()  # nothing is exported 
# =============================================================================

import ROOT 

import HistoStrings.Histos


def test1D() :
    
    
    h1 = ROOT.TH1D('h1','my title', 5 , 0 , 1 )
    h1.Fill ( 0.1 )
    h1.Fill ( 0.4 )
    h1.Fill ( 0.5 )
    
    print h1.toString()
    h1str = str(eval(h1.toString()))
    print h1.fromString ( h1.toString())
    print "From string: "+h1str
    print h1.toString()


    print h1.toXml()
    print h1.fromXml ( h1.toXml() )
    


def test2D() :    
    
    h2 = ROOT.TH2D('h2','my title', 5 , 0 , 1 , 5 , 0 , 2 )
    h2.Fill ( 0.1 , 0.1 )
    h2.Fill ( 0.4 , 0.4 )
    h2.Fill ( 0.5 , 0.5 )
    
    print h2.toString()
    print h2.fromString ( h2.toString() )


    h3 = ROOT.TH2D('h3','my title 3 ', 10 , 0 , 1 , 10 , 0 , 2 )

    print h2.toXml()
    print "From Xml:"
    print h2.fromXml ( h2.toXml() )

    h3.fromString(str(eval(h2.toString())))
    print "From string:"
    print h3.toString()
    


# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__

    test1D()
    test2D()
    
# =============================================================================
