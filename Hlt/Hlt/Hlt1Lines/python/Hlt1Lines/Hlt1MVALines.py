# =============================================================================
## @@file
#  Configuration of One Track Lines
#  @@author Vladimir Gligorov vladimir.gligorov@@cern.ch
#  @@date 2010-08-16
# =============================================================================
"""
 script to configure One Track trigger lines
"""
# =============================================================================
__author__  = "Vladimir Gligorov vladimir.gligorov@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.10 $"
# =============================================================================

from GaudiKernel.SystemOfUnits import GeV, MeV, mm

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1MVALinesConf( HltLinesConfigurableUser ) :
    #--------------------------------
    #
    # V. Gligorov
    __slots__ = {'DoTiming'          : False,
                 'TrackMVA' :    {'MinPT'     :  1000.  * MeV,
                                  'MaxPT'     : 25000.  * MeV,
                                  'MinIPChi2' :     7.4,
                                  'Param1'    :     0.5,
                                  'Param2'    :     1.0,
                                  'Param3'    :     1.0,
                                  'GEC'       : 'Loose'},
                 'TwoTrackMVA' : {'P'         :  3000. * MeV,
                                  'PT'        :   500. * MeV,
                                  'IPChi2'    :     4.,
                                  'MinMCOR'   :  1000. * MeV,
                                  'MaxMCOR'   :   1e9  * MeV,
                                  'MinETA'    :     2.,
                                  'MaxETA'    :     5.,
                                  'DOCA'      :  1000. * mm,
                                  'V0PT'      :  2000. * MeV,
                                  'VxChi2'    :    10.,
                                  'MvaVars'   : {'pt'     : 'PT',
                                                 'chi2'   : 'VFASPF(VCHI2)',
                                                 'ipchi2' : 'BPVIPCHI2()',
                                                 'fdchi2' : 'BPVVDCHI2',
                                                 'mcor'   : 'BPVCORRM',
                                                 'minpt'  : 'MINTREE(PT, ISBASIC)',
                                                 'nlt16'  : 'NINTREE(ISBASIC & (BPVIPCHI2() < 16))'},
                                  'Threshold' :     0.4,
                                  'File'      : '$PARAMFILESROOT/data/Hlt1TwoTrackMVA.mx',
                                  'GEC'       : 'Loose'},
                 'L0Channels'  : {'TrackMVA'    : 'L0_DECISION_PHYSICS',
                                  'TwoTrackMVA' : 'L0_DECISION_PHYSICS'},
                 'Priorities'  : {'TrackMVA'    : 20,
                                  'TwoTrackMVA' : 21}
                }

    def localise_props(self, name):
        from copy import deepcopy
        props = deepcopy(self.getProps()[name])
        # get the list of options belonging to this prefix
        return props
    
    def oneTrackVars(self):
        """Return all variables required for the BDT
        Variable names MUST correspond exactly to what is needed by classifier (xml)
        If they are unknown: they are in the xml, and they will be shown in stdout
        """
        mvaVars = {
            'pt' : 'PT',
            'ipchi2' : 'BPVIPCHI2()'
        }
        return mvaVars

    def twoTrackVars(self, nickname):
        """Return all variables required for the BDT
        Variable names MUST correspond exactly to what is needed by classifier (xml)
        If they are unknown: they are in the xml, and they will be shown in stdout
        """
        props = self.getProps()
        mvaVars = props[nickname]['MvaVars']
        daughterVars = props[nickname].get('DaughterVars', {})

        # Add all daughter variables to output
        for child, daughterNum in enumerate(range(1, 3)):
            for var, fun in daughterVars.iteritems():
                mvaVars.update({var : fun.format(daughterNum)})
        return mvaVars
    
    def matrixNetClassifier(self, unit, name, varFun):
        from MVADictHelpers import addMatrixnetclassifierValue
        return addMatrixnetclassifierValue(unit, self.getProp(name)['File'], varFun(),
                                           name + 'MatrixNetTool')
    def oneTrackStreamer(self, name, props, inputParticles) :
        from copy import deepcopy
        props = deepcopy(props)
        props['input'] = inputParticles.outputSelection()
        props['name']  = name

        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%sUnit' % name,
            PVSelection = "PV3D",
            Monitor = True,
            Code = """
            SELECTION('%(input)s')
            >> (((PT > %(MaxPT)s) & (BPVIPCHI2() > %(MinIPChi2)s))
                | ( in_range( %(MinPT)s, PT, %(MaxPT)s) &
                   (log(BPVIPCHI2()) > (%(Param1)s / ((PT / GeV - %(Param2)s) ** 2)  + 
                                       (%(Param3)s / %(MaxPT)s) * (%(MaxPT)s   - PT) +
                                        math.log(%(MinIPChi2)s)))))
            >> SINK('Hlt1%(name)sDecision')
            >> ~TC_EMPTY
            """ % props,
            Preambulo = self.mvaPreambulo()
        )
                
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( props['GEC'] ), PV3D('Hlt1'), inputParticles, unit ]    

    def twoTrackStreamer(self, name, props, inputParticles) :
        from copy import deepcopy
        props = deepcopy(props)
        props['input'] = inputParticles.outputSelection()
        props['name']  = name
        preambulo = self.mvaPreambulo()
        preambulo += [ "from LoKiArrayFunctors.decorators import APT, ACUTDOCA",
                      ("%(name)sCombinationConf = LoKi.Hlt1.Hlt1CombinerConf(strings(['K*(892)0 -> pi- pi+', '[K*(892)0 -> pi+ pi+]cc'])," +
                       "((APT > %(PT)s) & (ACUTDOCA(%(DOCA)s,'LoKi::DistanceCalculator'))), ALL)") % props]

        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%sUnit' % name,
            PVSelection = "PV3D",
            Monitor = True,
            Preambulo = preambulo
            )
        tool = self.matrixNetClassifier(unit, name, lambda : self.twoTrackVars(name))

        props['tool'] = tool.getFullName()
        code = """
        TC_HLT1COMBINER('', %(name)sCombinationConf,
                        '%(input)s', ((PT > %(V0PT)s * MeV) & (P  > %(P)s * MeV) & \
                                      (BPVIPCHI2() > %(IPChi2)s)))
        >> tee(monitor(TC_SIZE > 0, '# V0', LoKi.Monitoring.ContextSvc))
        >> tee(monitor(TC_SIZE    , 'nV0s', LoKi.Monitoring.ContextSvc))
        >> ( HASVERTEX & (VFASPF(VCHI2) < %(VxChi2)s)      & \
            (in_range(%(MinETA)s,  BPVETA,    %(MaxETA)s))  & \
            (in_range(%(MinMCOR)s, BPVCORRM, %(MaxMCOR)s)) & \
            (VALUE('%(tool)s') > %(Threshold)s))
        >> SINK ('Hlt1%(name)sDecision')
        >> ~TC_EMPTY
        """ % props
        unit.Code = code
        unit.PreloadTools = [tool.getFullName()]

        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from HltTracking.HltPVs import PV3D

        return [Hlt1GECUnit(props['GEC']), PV3D('Hlt1'), inputParticles, unit]

    def __l0du(self, nickname):
        import collections
        l0 = self.getProp( 'L0Channels' )[nickname]
        if isinstance(l0, collections.Iterable) and not isinstance(l0, basestring):
            return "|".join(["L0_CHANNEL('%s')" % chan for chan in l0])
        else:
            return l0

    def __odin(self, nickname):
        if 'ODINTriggers' not in self.getProps():
            return None
        odin = self.getProp( 'ODINTriggers' )[nickname]
        import collections
        if isinstance(odin, collections.Iterable) and not isinstance(odin, basestring):
            return "|".join(["(ODIN_TRGTYP == %s)" % chan for chan in odin])
        else:
            return odin

    def build_line( self, name, streamer, inputParticles ):
        from HltLine.HltLine import Hlt1Line
        algos = [ unit for unit in streamer( name, self.localise_props( name ), inputParticles ) ]
        priorities = self.getProp( "Priorities" )
        priority = priorities[ name ] if name in priorities else None
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.postscale,
            priority  = priority,
            ODIN = self.__odin(name),
            L0DU = self.__l0du(name),
            algos = algos)

    def mvaPreambulo(self):
        return ["import math"]
        
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line
        priorities = self.getProp( "Priorities" )
        doTiming = self.getProp( 'DoTiming' )

        from Hlt1SharedParticles import Hlt1SharedParticles
        sharedParticles = Hlt1SharedParticles()
        pionUnit = sharedParticles.pionUnit()

        to_build = {'TrackMVA'    : self.oneTrackStreamer,
                    'TwoTrackMVA' : self.twoTrackStreamer}

        for line, streamer in to_build.iteritems():
            self.build_line(line, streamer, pionUnit)
