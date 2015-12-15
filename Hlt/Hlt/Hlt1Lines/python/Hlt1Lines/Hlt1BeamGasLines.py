# =============================================================================
## @@file
#  Configuration of BeamGas Lines
#  @@author Jaap Panman jaap.panman@cern.ch
#  @@date 2009-02-25
#  @@author Plamen Hopchev phopchev@cern.ch
#  @@date 2011-03-01
#  @@author Rosen Matev rosen.matev@cern.ch
#  @@date 2015-04-17
# =============================================================================
"""Script to configure BeamGas trigger lines"""
# =============================================================================
__author__  = "Jaap Panman jaap.panman@cern.ch"
__author__  = "Plamen Hopchev phopchev@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.25 $"
# =============================================================================

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import bindMembers
from HltLine.HltLine import Hlt1Line
from HltLine.HltDecodeRaw import DecodeVELO
from Configurables import LoKi__VoidFilter, LoKi__HltUnit as HltUnit
import Configurables

class Hlt1BeamGasLinesConf(HltLinesConfigurableUser) :

    __slots__ = {
        ### General behaviour settings
          'TrackingConf'             : 'FastVelo' # can be 'FastVelo' or 'PatVelo'
        , 'FitTracks'                : False  # fit (velo-only) tracks
        , 'PVFitter'                 : 'LSAdaptPV3DFitter'
        , 'PVSeeding'                : 'PVSeed3DTool'
        , 'SplitVertices'            : False  # implies creation of selections (stored in SelReports)
        , 'CreateGlobalSelection'    : False  # selections not saved even if True! (Apr 2015)
        , 'Turbo'                    : False  # store more information for objects in the SelReports
        , 'FullZVetoLumiTriggers'    : True   # veto lumi triggers for ForcedRecoFullZ line

        ### Container names, selection names, etc.
        , 'FastVeloBGHitManagerName' : 'FastVeloBeamGasHitManager'
        , 'RZTrackContName'          : 'Hlt1/Track/BeamGasRZ'
        , 'TrackContName'            : 'Hlt1/Track/BeamGasAll'
        , 'FittedTrackContName'      : 'Hlt1/Track/BeamGasAllFitted'
        , 'VertexContName'           : 'Hlt1/Vertex/BeamGasAll'
        , 'SplitVertexContName'      : 'Hlt1/Vertex/BeamGasAllSplit'
        , 'GlobalSelectionNames'     : { 'PV': 'BeamGasAllPV', 'Split': 'BeamGasAllSplit' }

        ### Constituents of the names of the HLT Lines
        , 'LineNameParts' : { 'ee' : ['BeamGas','NoBeam']
                            , 'be' : ['BeamGas','']
                            , 'bbenhanced'  : ['BeamGasCrossing','Enhanced']
                            , 'bbforced'    : ['BeamGasCrossing','ForcedReco']
                            , 'bbparasitic' : ['BeamGasCrossing','Parasitic']
                            , 'highrho'     : ['BeamGas', 'HighRhoVertices']
                            }

        ### ODIN rate limits for the high-rho lines
        , 'ODINLimitHighRho' : { 'HighRhoBB'    : 'SCALE(0.)'  #e.g. 'SCALE(0.1)' or 'RATE(50000.0)'
                               , 'HighRhoNonBB' : 'SCALE(1.0)'
                               }

        ### L0DU requirements
        , 'L0Filter' : { 'NoBeamBeam1'   : "L0_CHANNEL('B1gas')"
                       , 'NoBeamBeam2'   : "L0_CHANNEL('B2gas')"
                       , 'Beam1'         : "L0_CHANNEL('B1gas')"
                       , 'Beam2'         : "L0_CHANNEL('B2gas')"
                       , 'EnhancedBeam1' : "L0_CHANNEL('B1gas')"
                       , 'EnhancedBeam2' : "L0_CHANNEL('B2gas')"
                       , 'BB'            : "(L0_DATA('Spd(Mult)') > 5) | (L0_DATA('PUHits(Mult)') > 5)"
                       }

        ### Rate Limits of events passing the L0DU requirements
        , 'L0RateLimit' : { 'NoBeamBeam1'    : 10000.
                          , 'NoBeamBeam2'    : 10000.
                          , 'Beam1'          :  None  # was 5000.
                          , 'Beam2'          :  None  # was 5000.
                          , 'EnhancedBeam1'  :  5000.
                          , 'EnhancedBeam2'  :  5000.
                          , 'ForcedReco'     :  1000.
                          , 'ForcedRecoFullZ':  1000.
                          , 'Parasitic'      : 50000.
                          }

        ### z-ranges for Tracking
        , 'TrackingZMin'            : -2000.
        , 'TrackingZMax'            :  2000.

        ### z-ranges for Vertexing
        , 'Beam1VtxRangeLow'        : -2000.
        , 'Beam1VtxRangeUp'         :   700.
        , 'Beam2VtxRangeLow'        :  -200.
        , 'Beam2VtxRangeUp'         :  2000.
        , 'BGVtxExclRangeMin'       :  -300.      # These 2 options take effect
        , 'BGVtxExclRangeMax'       :   300.      # only for the bb Lines

        ### Proto-Vertexing Settings

        ### PV3D vertex cuts
        , 'VertexMinNTracks'          : 9  # strictly greater than
        , 'FullZVertexMinNTracks'     : 9  # strictly greater than
        , 'HighRhoVerticesMinNTracks' : 9  # strictly greater than
        , 'VertexMaxChi2PerDoF'       : False  # None doesn't work with the framework...
        , 'VertexCutSmallRho'       : '(VX_BEAMSPOTRHO( 6*mm ) < 4*mm)'
        , 'VertexCutHighRho'        : '(VX_BEAMSPOTRHO( 6*mm ) > 4*mm)'

        ### Global Event Cuts
        , 'UseGEC'                  : 'Loose' # 'None' / 'Loose' / 'Tight' (defined in Hlt1GECs.py)

        ### Input Prescales
        , 'Prescale'                : { 'Hlt1BeamGasNoBeamBeam1'             : 1.
                                      , 'Hlt1BeamGasNoBeamBeam2'             : 1.
                                      , 'Hlt1BeamGasBeam1'                   : 1.
                                      , 'Hlt1BeamGasBeam2'                   : 1.
                                      , 'Hlt1BeamGasCrossingEnhancedBeam1'   : 1.
                                      , 'Hlt1BeamGasCrossingEnhancedBeam2'   : 1.
                                      , 'Hlt1BeamGasCrossingForcedReco'      : 1.
                                      , 'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.
                                      , 'Hlt1BeamGasCrossingParasitic'       : 1.
                                      , 'Hlt1BeamGasHighRhoVertices'         : 1.
                                      }
        ### Output Postscales
        , 'Postscale'               : { 'Hlt1BeamGasNoBeamBeam1'             : 'RATE(5)'
                                      , 'Hlt1BeamGasNoBeamBeam2'             : 'RATE(5)'
                                      , 'Hlt1BeamGasBeam1'                   : 'RATE(20)'
                                      , 'Hlt1BeamGasBeam2'                   : 'RATE(20)'
                                      , 'Hlt1BeamGasCrossingEnhancedBeam1'   : 'RATE(20)'
                                      , 'Hlt1BeamGasCrossingEnhancedBeam2'   : 'RATE(20)'
                                      , 'Hlt1BeamGasCrossingForcedReco'      : 'RATE(5)'
                                      , 'Hlt1BeamGasCrossingForcedRecoFullZ' : 'RATE(5)'
                                      , 'Hlt1BeamGasCrossingParasitic'       : 'RATE(5)'
                                      , 'Hlt1BeamGasHighRhoVertices'         : 'RATE(5)'
                                      }
        }


    def _get_GEC(self):
        '''Get the GEC algorithm in a list.'''
        gec = self.getProp('UseGEC')
        if gec in ['Loose', 'Tight']:
            from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
            return [ Hlt1GECUnit( gec=gec ) ]
        elif not gec or gec == 'None':
            return []
        else:
            raise ValueError('Unrecognized GEC value ({})!'.format(gec))

    def _create_tracking_algos(self):
        '''Create the common velo tracking instance.'''

        tracking_conf = self.getProp("TrackingConf")
        tracking_output = self.getProp("TrackContName")

        algs = []
        if tracking_conf == 'FastVelo':
            from Configurables import FastVeloTracking
            algTr = FastVeloTracking('FastVeloTrackingBeamGas')
            algTr.HitManagerName = self.getProp("FastVeloBGHitManagerName")
            algTr.OutputTracksName = tracking_output
            algTr.ZVertexMin = self.getProp("TrackingZMin")
            algTr.ZVertexMax = self.getProp("TrackingZMax")
            algs.append(algTr)
        elif tracking_conf == 'PatVelo':
            from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool, Tf__PatVeloGeneralTracking
            patVeloR = Tf__PatVeloRTracking('PatVeloBeamGas_RTracking')
            patVeloR.OutputTracksName = self.getProp('RZTrackContName')
            patVeloR.ZVertexMin = self.getProp("TrackingZMin")
            patVeloR.ZVertexMax = self.getProp("TrackingZMax")
            patVeloSpace = Tf__PatVeloSpaceTracking('PatVeloBeamGas_SpaceTracking')
            patVeloSpace.InputTracksName  = patVeloR.OutputTracksName
            patVeloSpace.OutputTracksName = tracking_output
            patVeloSpace.addTool(Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool")
            patVeloSpace.PatVeloSpaceTool.MarkClustersUsed=True
            patVeloGeneral = Tf__PatVeloGeneralTracking('PatVeloBeamGas_GeneralTracking')
            patVeloGeneral.OutputTracksLocation = tracking_output
            patVeloGeneral.PointErrorMin = 2
            algs.extend( [patVeloR, patVeloSpace, patVeloGeneral] )
        else:
            raise ValueError("Unknown TrackingConf ({})!".format(tracking_conf))

        return algs

    def _create_velo_fitter(self):
        '''Create the common instance of the velo fitting algorithm.'''
        from HltTracking.HltSharedTracking import fittedVelo
        return fittedVelo(self.getProp("TrackContName"), self.getProp("FittedTrackContName"), name='VeloOnlyFitterHltBeamGas')

    def HelperBeamsZRange(self, whichBeam):
        ''' Function returning the z-range limits to be
            used by the [Proto]Vertexing alogirthms '''
        if not whichBeam:
            zMin = self.getProp('Beam1VtxRangeLow')
            zMax = self.getProp('Beam2VtxRangeUp')
        elif whichBeam in ['Beam1', 'Beam2']:
            zMin = self.getProp(whichBeam+'VtxRangeLow')
            zMax = self.getProp(whichBeam+'VtxRangeUp')
        else:
            raise ValueError("Value of whichBeam should be one of 'Beam1','Beam2','' !")
        return zMin, zMax

    def _create_proto_vertex_alg(self, lineName, InpTracksName, whichBeam=0):
        zMin, zMax = self.HelperBeamsZRange(whichBeam)
        from Configurables import BeamGasProtoVertex
        protoVtxAlgo = BeamGasProtoVertex( 'BGProtoVertex_' + lineName
                       , InputSelection     = 'TES:%s' % InpTracksName
                       , zTracksMin         = zMin
                       , zTracksMax         = zMax
                       , zTracksExcludeLumiRegionLow = self.getProp("BGVtxExclRangeMin") if ("Cross" in lineName) else 0.
                       , zTracksExcludeLumiRegionUp  = self.getProp("BGVtxExclRangeMax") if ("Cross" in lineName) else 0.
                       , MinimumNumberOfTracks = self.getProp("VertexMinNTracks")
                       )
        return protoVtxAlgo

    def _configure_pvtool(self, pvalg):
        fitter_name = self.getProp("PVFitter")
        seeding_name = self.getProp("PVSeeding")

        pvalg.addTool(Configurables.PVOfflineTool, 'PVOfflineTool')
        pvOfflineTool = pvalg.PVOfflineTool

        pvOfflineTool.PVFitterName = fitter_name + "/Fitter"
        pvOfflineTool.addTool(getattr(Configurables, fitter_name), "Fitter")
        fitter = pvOfflineTool.Fitter
        if fitter_name == "LSAdaptPV3DFitter":
            fitter.UseFittedTracks = self.getProp("FitTracks")
            # The following option improves consistensy of MC resolution and
            # the resolution from the difference of split vertices.
            # (TrackErrorScaleFactor = 2 is equivalent to trackMaxChi2 = 9*2^2)
            fitter.TrackErrorScaleFactor = 2. # default 1.
        if fitter_name in ["LSAdaptPV3DFitter", "LSAdaptPVFitter"]:
            fitter.MinTracks = 5 # default 5
            fitter.Iterations = 50 # default 20/50
            fitter.maxChi2 = 1600. # default 400.
        else:
            log.warning("You are using an unsupported PV fitter ({})".format(fitter_name))

        pvOfflineTool.PVSeedingName = seeding_name + "/Seeding"
        pvOfflineTool.addTool(getattr(Configurables, seeding_name), "Seeding")
        if seeding_name == 'PVSeed3DTool':
            pvOfflineTool.Seeding.TrackPairMaxDistance = 2. # default 0.3
            pvOfflineTool.Seeding.zMaxSpread = 10. # default 3.
            pvOfflineTool.Seeding.MinCloseTracks = 1 # default 4
        else:
            log.warning("You are using an unsupported PV seeding ({})".format(seeding_name))

        return pvOfflineTool

    def _create_pvalg(self):
        from Configurables import PatPV3D

        pvalg = PatPV3D('PatPV3DHltBeamGas')
        pvalg.OutputVerticesName = self.getProp("VertexContName")
        pvOfflineTool = self._configure_pvtool(pvalg)
        if self.getProp("FitTracks"):
            pvOfflineTool.InputTracks = [self.getProp("FittedTrackContName")]
        else:
            pvOfflineTool.InputTracks = [self.getProp("TrackContName")]

        return pvalg

    def _create_pvsplit(self, lineName=None):
        from Configurables import PVSplit, PVOfflineTool

        if not lineName:  # global instance, runnging on all PVs
            pvSplitAlg = PVSplit("PVSplitHltBeamGas")
            pvSplitAlg.InputVerticesLocation = self.getProp("VertexContName")
            pvSplitAlg.OutputVerticesLocation = self.getProp("SplitVertexContName")
        else:  # per-line instance, running on selected PVs
            pvSplitAlg = PVSplit("PVSplitHlt" + lineName)
            pvSplitAlg.InputVerticesLocation = 'Hlt1/Vertex/' + lineName
            pvSplitAlg.OutputVerticesLocation = 'Hlt1/Vertex/' + lineName + 'Split'
        pvSplitAlg.RandomShuffle = True
        pvSplitAlg.SplitMethod = "MiddlePerVeloHalf"
        pvSplitAlg.UsePVForSeed = True
        pvSplitAlg.CopyPV = True

        pvOfflineTool = self._configure_pvtool(pvSplitAlg)
        # Custom fitter/seeding settings for splitting:
        pvOfflineTool.Fitter.MinTracks = 3  # default 5

        return pvSplitAlg

    def _create_global_selections(self):
        tes_to_sel = "VSOURCE('{}') >> VX_SINK('{}') >> ~VEMPTY"
        sel_names = self.getProp("GlobalSelectionNames")

        storePVs = HltUnit('CreateBeamGasPVSelection',
	        Code = tes_to_sel.format(self.getProp("VertexContName"), sel_names["PV"])
        )
        algs = [storePVs]

        if self.getProp("SplitVertices"):
            storeSVs = HltUnit('CreateBeamGasSplitSelection',
                Code = tes_to_sel.format(self.getProp("SplitVertexContName"), sel_names["Split"])
            )
            algs.append(storeSVs)

        return algs

    def _create_line_selection(self, lineName):
        decname = 'Hlt1{}Decision'.format(lineName)
        source = 'Hlt1/Vertex/' + lineName + ('Split' if self.getProp("SplitVertices") else '')
        code = "VSOURCE('{}') >> VX_SINK('{}') >> ~VEMPTY".format(source, decname)
        return HltUnit(decname, Code = code)

    def _create_reco_algos(self):
        algs = []

        algTracking = self._create_tracking_algos()
        algs.extend(algTracking)

        if self.getProp("FitTracks"):
            algs += self._create_velo_fitter()

        algs.append(self._create_pvalg())

        if self.getProp("CreateGlobalSelection") and self.getProp("SplitVertices"):
            algs.append(self._create_pvsplit())

        return algs

    def _pv_filter_predicate(self, lineName):
        ''' Create filters for the vertices produced by PV3D '''
        ### To Do:
        #!!! Add a cut on the track asymmetry? --> this can help us in the case of pp collisions in ee, be or eb crossings
        #!!! Add a cut on z_vtx to get rid of Offest Collisions at +/- 75 cm

        if   'Beam1' in lineName: zMin, zMax = self.HelperBeamsZRange('Beam1')
        elif 'Beam2' in lineName: zMin, zMax = self.HelperBeamsZRange('Beam2')
        else:                     zMin, zMax = self.HelperBeamsZRange('')

        ### One object cuts ###
        vtxCut_zPos           = "in_range(%s*mm, VZ, %s*mm)" %(zMin, zMax)
        vtxCut_maxChi2PerDoF  = "(VCHI2PDOF < %s)"% self.getProp("VertexMaxChi2PerDoF") if self.getProp("VertexMaxChi2PerDoF") else ""
        prop = 'FullZVertexMinNTracks' if 'FullZ' in lineName else 'VertexMinNTracks'
        vtxCut_minNTracks     = "(NTRACKS > %s)"  % self.getProp(prop) if self.getProp(prop) else ""
        vtxCut_smallRho = self.getProp('VertexCutSmallRho')
        listVertexCuts = [ vtxCut_zPos, vtxCut_maxChi2PerDoF, vtxCut_minNTracks, vtxCut_smallRho ]

        #in case of a bb line add the cut that vetoes the Lumi Region
        vtxCut_excludeLumiReg = "~in_range(%s*mm, VZ, %s*mm)" %(self.getProp("BGVtxExclRangeMin"), self.getProp("BGVtxExclRangeMax"))
        if "Cross" in lineName:
            if "FullZ" not in lineName: listVertexCuts.append(vtxCut_excludeLumiReg)

        ### The special case for the high-rho vertices Line
        if "HighRho" in lineName:
            vtxCut_minNTracks = "(NTRACKS > %s)"  % self.getProp("HighRhoVerticesMinNTracks") if self.getProp("HighRhoVerticesMinNTracks") else ""
            vtxCut_highRho    = self.getProp('VertexCutHighRho')
            listVertexCuts = [ vtxCut_zPos, vtxCut_maxChi2PerDoF, vtxCut_minNTracks, vtxCut_highRho ]

        return " & ".join([ c for c in listVertexCuts if c ])

    def _create_main_algos(self, lineName):
        split = self.getProp('SplitVertices')

        algs = []

        algs.extend(self._create_reco_algos())

        if self.getProp('CreateGlobalSelection'):
            algs.append(self._create_global_selections())

        # vertex filtering code
        predicate = self._pv_filter_predicate(lineName)  # the actual cuts
        code = "VSOURCE('{}', {})".format(self.getProp("VertexContName"), predicate)

        if not split:
            code += " >> ~VEMPTY"
            filter_alg = LoKi__VoidFilter(
                'Hlt1{}Decision'.format(lineName),
                Preambulo = ['from LoKiPhys.decorators import *','from LoKiCore.functions import *'],
                Code = code
            )
        else:
            code += " >> RV_SINKTES('{}') >> ~VEMPTY".format('Hlt1/Vertex/' + lineName)
            filter_alg = HltUnit(
                'Hlt1{}Filter'.format(lineName),
                Preambulo = ['from LoKiPhys.decorators import *','from LoKiCore.functions import *'],
                Code = code
            )
        algs.append(filter_alg)

        if split:
            algs.append(self._create_pvsplit(lineName))
            algs.append(self._create_line_selection(lineName))

        return algs

    ############################################################################
    ### Function creating the ee, be, lonely and forced lines
    ############################################################################
    def __create_nonParasitic_line__(self, whichLine, whichBeam) :
        ''' Function to create the Lines for 'ee'(2), 'be'(1), 'eb'(1),
            'lonely bb'(2) and 'forced bb'(2) crossings. Algorithms:
            1) FastVeloTracking with enlarged z-window (the same algorithm for all lines)
            2) PV3D (the same algorithm for all lines)
            3) Loki-Filter the produced vertices (Algo name should be <name of line> + Decision)
            In Algos 2) and 3) need to exclude lumi region for the 'lonely bb' and 'forced bb' cases
        '''

        nameParts = self.getProp("LineNameParts")[whichLine]
        nameKey   = nameParts[1]+whichBeam
        lineName  = nameParts[0]+nameKey

        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        if 'ForcedReco' in nameParts[1]:
            l0du = self.getProp('L0Filter')['BB']
            odin = '(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            #reject lumi events for the FullZ line
            if whichBeam == 'FullZ' and self.getProp('FullZVetoLumiTriggers'):
                odin += ' & (ODIN_TRGTYP != LHCb.ODIN.LumiTrigger)'
        elif 'HighRho' in nameParts[1]:
            l0du = self.getProp('L0Filter')['BB']
            reqBB    = '(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            reqNonBB = '((ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2))'
            rateLimBB    = self.getProp('ODINLimitHighRho')['HighRhoBB']
            rateLimNonBB = self.getProp('ODINLimitHighRho')['HighRhoNonBB']
            odin = '(scale(%s, %s) | scale(%s, %s))' %(reqBB, rateLimBB, reqNonBB, rateLimNonBB)
        else:
            l0du = self.getProp('L0Filter')[nameKey] if (nameKey in self.getProp('L0Filter').keys()) else None
            if 'NoBeam' == nameParts[1]:
                odin = '(ODIN_BXTYP == LHCb.ODIN.NoBeam)'
            elif '' == nameParts[1]:
                odin = '(ODIN_BXTYP == LHCb.ODIN.'+whichBeam+')'
            elif 'Enhanced' == nameParts[1]:
                odin = '(ODIN_TRGTYP == LHCb.ODIN.BeamGasTrigger) & (ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            else:
                print "\n\nFATAL ERROR - Can't set ODIN Filter\n\n"

        gec_algs = self._get_GEC()
        main_algs = self._create_main_algos(lineName)

        return Hlt1Line( lineName
                       , priority = None
                       , prescale = self.prescale
                       , ODIN  = odin
                       , L0DU  = 'scale({}, RATE({}))'.format(l0du, L0RateLimit) if L0RateLimit else l0du
                       , algos = gec_algs + [ DecodeVELO ] + main_algs
                       , postscale = self.postscale
                       , Turbo = self.getProp("Turbo")
                   )

    ############################################################################
    ### Function creating the parastic bb line
    ############################################################################
    def __create_parasitic_line__(self):
        ''' Function to create the Parasitic line for bb crossings. Algorithms:
            1) Check if Velo tracks have already been reconstructed
            2) Run the proto-vertexing algorithm (excluding the lumi region)
            3) Run Full Fast Velo tracking (the same instance as in the non-parasitic lines)
            4) PV3D (the same instance as in the non-parasitic lines)
            5) Filter the produced vertices
        '''

        nameParts = ['BeamGasCrossing', 'Parasitic']
        nameKey   = nameParts[1]
        lineName  = nameParts[0]+nameKey

        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        l0du = self.getProp('L0Filter')['BB']
        odin = "(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)"

        ### Algorithm to check for existing Velo tracks
        from HltTracking.HltSharedTracking import MinimalVelo
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckTracks = VoidFilter('Hlt1BeamGasCheckVeloTracks' , Code = "CONTAINS('%s') > 0" % MinimalVelo.outputSelection() )

        ### ProtoVertexing algorithm
        algProtoVertex = self._create_proto_vertex_alg(lineName, MinimalVelo.outputSelection())

        gec_algs = self._get_GEC()
        main_algs = self._create_main_algos(lineName)

        return Hlt1Line( lineName
                       , priority = 200
                       , prescale = self.prescale
                       , ODIN  = odin
                       , L0DU  = 'scale({}, RATE({}))'.format(l0du, L0RateLimit) if L0RateLimit else l0du
                       , algos = gec_algs + [ DecodeVELO, algCheckTracks, algProtoVertex ] + main_algs
                       , postscale = self.postscale
                       , Turbo = self.getProp("Turbo")
                       )

        return TheLine

    ############################################################################
    ### Create the BeamGas Hlt Lines
    ############################################################################
    def __apply_configuration__(self) :
        self.__create_nonParasitic_line__('ee', 'Beam1') #line name = 'Hlt1BeamGasNoBeamBeam1'
        self.__create_nonParasitic_line__('ee', 'Beam2')
        self.__create_nonParasitic_line__('be', 'Beam1') #line name = 'Hlt1BeamGasBeam1'
        self.__create_nonParasitic_line__('be', 'Beam2')
        self.__create_nonParasitic_line__('bbenhanced', 'Beam1') #line name = 'Hlt1BeamGasCrossingEnhancedBeam1'
        self.__create_nonParasitic_line__('bbenhanced', 'Beam2')
        self.__create_nonParasitic_line__('bbforced', '') #line name = 'Hlt1BeamGasCrossingForcedReco'
        self.__create_nonParasitic_line__('bbforced', 'FullZ') #line name = 'Hlt1BeamGasCrossingForcedRecoFullZ'
        self.__create_nonParasitic_line__('highrho', '') #line name = Hlt1BeamGasHighRhoVertices
        self.__create_parasitic_line__() #line name = 'Hlt1BeamGasCrossingParasitic'
