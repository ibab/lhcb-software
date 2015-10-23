# -*- coding: utf-8 -*-
## @file
#  Set of Hlt1-lines for study of low multiplicity processes. 
#
#  @author Dan Johnson daniel.johnson@cern.ch
#=============================================================================
#=============================================================================
#__author__  = "Dan Johnson daniel.johnson@cern.ch"

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1LowMultLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'Prescale'                                : {"Hlt1LowMultPassThrough" : 0.01}
        , 'SpdMult'                                 :   100.   # dimensionless, Spd Multiplicity cut
        , 'VeloCut_Hadrons_MinVelo'                 :     2    # dimensionless
        , 'VeloCut_Hadrons_MaxVelo'                 :     8    # dimensionless
        , 'VeloCut_Hadrons_SpdMult'                 :    20    # dimensionless
        , 'VeloCut_Leptons_MinVelo'                 :     0    # dimensionless
        , 'VeloCut_Leptons_MaxVelo'                 :   100    # dimensionless
        , 'MaxVeloCut_MaxVelo'                      :     8    # dimensionless
        , 'MaxNVelo'                                :  1000    # dimensionless, 
        , 'MinNVelo'                                :     2    # dimensionless, 
        , 'TrChi2'                                  :     5.   # dimensionless, 
        , 'PT'                                      :   500.   # MeV
        , 'P'                                       :     0.   # MeV 
        , 'LowMultLineL0Dependency'                 : "( L0_CHANNEL_RE('.*lowMult') & ~(L0_CHANNEL_RE('Photon,lowMult')) )"
        , 'LowMultVeloCut_HadronsLineL0Dependency'  : "( L0_CHANNEL_RE('DiHadron,lowMult') )"
        , 'LowMultVeloCut_LeptonsLineL0Dependency'  : "( L0_CHANNEL_RE('Muon,lowMult') | L0_CHANNEL_RE('DiMuon,lowMult') | L0_CHANNEL_RE('Electron,lowMult') )" 
        , 'LowMultMaxVeloCutLineL0Dependency'       : "( L0_CHANNEL_RE('Photon,lowMult') | L0_CHANNEL_RE('DiEM,lowMult') )"
        , 'LowMultPassThroughLineL0Dependency'      : "L0_CHANNEL_RE('.*lowMult')"
        , 'NoBiasTriggerType'                       : 'LumiTrigger' # dimensionless
        , 'NoBiasBeamCrossingTypeVeto'              : 'BeamCrossing' # dimensionless
        }
    
    def preambulo( self ):
        ## define some "common" preambulo 
        from HltTracking.Hlt1Tracking import TrackCandidates, FitTrack
        prefix = "LowMult"
        Preambulo = [ FitTrack,
                      TrackCandidates( prefix ) ]
        return Preambulo
   
    def streamer_VeloCut_Hadrons( self, name ):
        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( %(VeloCut_Hadrons_MinVelo)s, CONTAINS('%(Velo)s'), %(VeloCut_Hadrons_MaxVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1'+name+'Decision', Code = code)
        return [veloFilter]
    
    def streamer_VeloCut_Leptons( self, name ):
        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( %(VeloCut_Leptons_MinVelo)s, CONTAINS('%(Velo)s'), %(VeloCut_Leptons_MaxVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1'+name+'Decision', Code = code)
        return [veloFilter]
    
    def streamer_MaxVeloCut( self, name ):
        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( 0, CONTAINS('%(Velo)s'), %(MaxVeloCut_MaxVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1'+name+'Decision', Code = code)
        return [veloFilter]

    def streamer( self ):
        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( %(MinNVelo)s, CONTAINS('%(Velo)s'), %(MaxNVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1LowMultNVeloFilter', Code = code)

        ## Streamer
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1LowMultStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo(),
            Code = """
            TrackCandidates
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) ) 
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK('Hlt1LowMultDecision')
            >> ~TC_EMPTY
            """ % props
            )
        return [veloFilter, unit]
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
        from HltTracking.HltSharedTracking import MinimalVelo
               
        Hlt1Line(
            'LowMult',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(SpdMult)s ) & ( %(LowMultLineL0Dependency)s ) )" % self.getProps(),   
            ##
            algos     = [ MinimalVelo ] + self.streamer()
            )
        Hlt1Line(
            'LowMultVeloCut_Hadrons',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(VeloCut_Hadrons_SpdMult)s ) & ( %(LowMultVeloCut_HadronsLineL0Dependency)s ) )" % self.getProps(),   
            ##
            algos     =  [ MinimalVelo ] + self.streamer_VeloCut_Hadrons('LowMultVeloCut_Hadrons')
            )
        Hlt1Line(
            'LowMultVeloCut_Leptons',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(SpdMult)s ) & ( %(LowMultVeloCut_LeptonsLineL0Dependency)s ) )" % self.getProps(),   
            ##
            algos     =  [ MinimalVelo ] + self.streamer_VeloCut_Leptons('LowMultVeloCut_Leptons')
            )
        Hlt1Line(
            'LowMultMaxVeloCut',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(SpdMult)s ) & ( %(LowMultMaxVeloCutLineL0Dependency)s ) )" % self.getProps(),
            ##
            algos     =  [ MinimalVelo ] + self.streamer_MaxVeloCut('LowMultMaxVeloCut')
            )
        Hlt1Line(
            'NoBiasNonBeamBeam',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            ODIN      = ('( (ODIN_TRGTYP == LHCb.ODIN.%(NoBiasTriggerType)s) ' % self.getProps()) +\
                        ('& ~(ODIN_BXTYP == LHCb.ODIN.%(NoBiasBeamCrossingTypeVeto)s) )' % self.getProps())
            )
        Hlt1Line(
            'LowMultPassThrough',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( %(LowMultPassThroughLineL0Dependency)s )" % self.getProps(),
            ##
            )
