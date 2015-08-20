from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CommissioningLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale'    : {'Hlt2PassThrough' : 0.0001,
                                  'Hlt2Forward'     : 0.0001,
                                  'Hlt2DebugEvent'  : 0.0001},
                 'Postscale'   : {'Hlt2ErrorEvent'  : 'RATE(1)'},
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
                 'SMOGPhysics' : {'HLT1' : "HLT_PASS_RE('^Hlt1.*Decision$')" ,
                                  'VoidFilter' : ''},
                 'Lumi'        : {'HLT1' : "HLT_PASS_SUBSTR('Hlt1Lumi')",
                                  'VoidFilter' : ''},
                 'KS0_DD'      : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')",
                                  'VoidFilter' : ''},
                 'KS0_LL'      : {'HLT1' : "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')",
                                  'VoidFilter' : ''},
                 'Turbo'       : ['KS0_DD', 'KS0_LL']
                }

    def __apply_configuration__(self):
        from Stages import CopyTracks, IncidentGenerator
        from Inputs import KS0_DD, KS0_LL
        stages = {'Forward'     : [CopyTracks()],
                  'DebugEvent'  : [IncidentGenerator()],
                  'ErrorEvent'  : [],
                  'PassThrough' : [],
                  'NoBiasPassThrough' : [],
                  'Transparent' : [],
                  'SMOGPhysics' : [],
                  'Lumi'        : [],
                  'KS0_DD'      : [KS0_DD],
                  'KS0_LL'      : [KS0_LL]}

        from HltLine.HltLine import Hlt2Line
        for name, algos in self.algorithms(stages).iteritems():
            localProps = self.getProps().get(name, {})
            doturbo = False
            linename = name
            if name in self.getProps()['Turbo']:
                doturbo = True
                linename += 'Turbo'
            Hlt2Line(linename, prescale = self.prescale, postscale = self.postscale,
                     algos      = algos,
                     HLT1       = localProps.get('HLT1', None),
                     VoidFilter = localProps.get('VoidFilter', None),
                     priority   = localProps.get('Priority', None),
                     Turbo = doturbo)
