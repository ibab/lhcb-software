from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class Commissioning_25ns_August2015(object) :
    """
    Threshold settings for Hlt2 Commissioning lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author M. Vesterinen
    @date 2015-07-023
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
        lines = ['Hlt2PassThrough','Hlt2Lumi','Hlt2DebugEvent',
                 'Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent']
        
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Commissioning.Lines     import CommissioningLines            
        d.update({CommissioningLines :
                      {'Prescale'    : {'Hlt2PassThrough' : 0.0001,
                                        'Hlt2Forward'     : 0.00001,
                                        'Hlt2DebugEvent'  : 0.000001},
                       'Postscale'   : {'Hlt2ErrorEvent'  : 'RATE(0.01)'},
                       # do not want debug events on lumi-exclusive Hlt1 events...
                       'DebugEvent'  : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')"},
                       'ErrorEvent'  : {'Priority' : 254,
                                        'VoidFilter' : '',
                                        'HLT2' : "HLT_COUNT_ERRORBITS_RE('^Hlt2.*',0xffff) > 0"},
                       'PassThrough' : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')",
                                        'VoidFilter' : ''},
                       'NoBiasPassThrough' : {'HLT1' : "HLT_PASS('Hlt1NoBiasPrescaledDecision')",
                                              'VoidFilter' : ''},
                       'Transparent' : {'HLT1' : "HLT_PASS_RE('^Hlt1(ODIN.*|L0.*|MB.*|BeamGas.*|Velo.*|NZS.*|Incident|Tell1Error|ErrorEvent)Decision$')",
                                        'VoidFilter' : ''},
                       'Lumi'        : {'HLT1' : "HLT_PASS_SUBSTR('Hlt1Lumi')",
                                        'VoidFilter' : ''},
                       'KS0_DD'      : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')",
                                        'VoidFilter' : ''},
                       'KS0_LL'      : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')",
                                        'VoidFilter' : ''},
                       'Turbo'       : ['KS0_DD', 'KS0_LL']
                       }}
                 )
        return d
    


