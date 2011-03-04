#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'PiMinPT'              : 500       # MeV                                                                                                 
    ,'PiMinP'               : 5000      # MeV                                                                                                
    ,'PiMinTrackProb'       : 0.000001  # unitless                                                                                           
    ,'PiMinIPChi2'          : 25        # unitless                                                                                           
    ,'Pi0MinPT_M'           : 2500      # MeV                                                                                                
    ,'Pi0MinPT_R'           : 1500      # MeV                                                                                                
    ,'ResPi0MinMM'          : 100       # MeV  # no effect if bellow 105                                                                     
    ,'ResPi0MaxMM'          : 170       # MeV  # no effect if above  165                                                                     
    ,'ResPi0MinGamCL'       : 0.2       # unitless                                                                                           
    ,'BMinM'                : 4200      # MeV                                                                                                
    ,'BMaxM'                : 6400      # MeV                                                                                                
    ,'BMinPT_M'             : 3000      # MeV                                                                                                
    ,'BMinPT_R'             : 2500      # MeV                                                                                                
    ,'BMinVtxProb'          : 0.001     # unitless                                                                                           
    ,'BMaxIPChi2'           : 9         # unitless                                                                                           
    ,'BMinDIRA'             : 0.99995   # unitless                                                                                           
    ,'BMinVVDChi2'          : 64        # unitless                                                                                           
    ,'MergedLinePrescale'   : 1.        # unitless                                                                                           
    ,'MergedLinePostscale'  : 1.        # unitless                                                                                           
    ,'ResolvedLinePrescale' : 1.        # unitless                                                                                           
    ,'ResolvedLinePostscale': 1.        # unitless                                                                                           
    }


from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf as builder


def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
