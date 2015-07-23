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

class Hlt1CEPLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'SpdMult'    :   100.   # dimensionless, Spd Multiplicy cut
        , 'MaxNVelo'   :  1000    # dimensionless, 
        , 'MinNVelo'   :     2    # dimensionless, 
        , 'TrChi2'     :     5.   # dimensionless, 
        , 'PT'         :   200.   # MeV
        , 'P'          :  1000.   # MeV 
        , 'VeloCutLineL0Dependency' : '.*DiHadron,lowMult'
        , 'NoVeloCutLineL0Dependency':'.*DiHadron,lowMult'
        , 'NoBiasTriggerType' : 'LumiTrigger' # dimensionless
        , 'NoBiasBeamCrossingTypeVeto' : 'BeamCrossing' # dimensionless
        }
    
    def preambulo( self ):
        ## define some "common" preambulo 
        from HltTracking.Hlt1Tracking import TrackCandidates, FitTrack
        prefix = "CEP"
        Preambulo = [ FitTrack,
                      TrackCandidates( prefix ) ]
        return Preambulo
   
    def streamer_veloCutsOnly( self, name ):
        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( %(MinNVelo)s, CONTAINS('%(Velo)s'), %(MaxNVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1'+name+'Decision', Code = code)

        return [veloFilter]
    
    def streamer( self ):

        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "in_range( %(MinNVelo)s, CONTAINS('%(Velo)s'), %(MaxNVelo)s ) "% props
        veloFilter = VoidFilter('Hlt1CEPNVeloFilter', Code = code)

        ## Streamer
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1CEPStreamer',
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
            >>  SINK('Hlt1CEPDecision')
            >> ~TC_EMPTY
            """ % props
            )
        return [veloFilter, unit]
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
               
        Hlt1Line(
            'CEP',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(SpdMult)s ) & ( L0_CHANNEL_RE('%(NoVeloCutLineL0Dependency)s') ) )" % self.getProps(),   
            ##
            algos     = self.streamer()
            )
        from HltTracking.HltSharedTracking import MinimalVelo
        Hlt1Line(
            'CEPVeloCut',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( ( L0_DATA('Spd(Mult)') < %(SpdMult)s ) & ( L0_CHANNEL_RE('%(VeloCutLineL0Dependency)s') ) )" % self.getProps(),   
            ##
            algos     =  [ MinimalVelo ] + self.streamer_veloCutsOnly('CEPVeloCut')
            )
        Hlt1Line(
            'NoBiasNonBeamBeam',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            ODIN      = ('( (ODIN_TRGTYP == LHCb.ODIN.%(NoBiasTriggerType)s) ' % self.getProps()) +\
                        ('& ~(ODIN_BXTYP == LHCb.ODIN.%(NoBiasBeamCrossingTypeVeto)s) )' % self.getProps())
            )
