## import everything from bender 
from   Bender.Awesome      import *
from   LoKiCore.math       import * 
from   ROOT                import Double
from   PartProp.Nodes      import * 
from   LoKiPhys.decorators import *
# =============================================================================
import LHCbMath.Types
from   DaVinciTools import Tools as DVTools
from   GaudiKernel.PhysicalConstants import c_light
# =============================================================================
## @class Candidate
#  Class used to store a particle and whether it was selected by the prescaled,
#  detached or both selections.
#  @author Roel Aaij roel.aaij@cern.ch
#  @date 2010-11-01
class Candidate( object ):
    def __init__( self, p, dec ):
        self._particle = p
        self._dec = dec

    def particle( self ):
        return self._particle

    def prescaled( self ):
        return self._dec[ 0 ]

    def detached( self ):
        return self._dec[ 1 ]

## @class ReaderAlgo_PreDet
#  Algorithm to read prescaled and detached selection candidates and store info
#  in an nTuple.
#  @author Roel Aaij roel.aaij@cern.ch
#  @date 2010-11-01
class ReaderAlgo_PreDet( Algo ):
    """
    Algorithm to get numbers related to proper time acceptance due to
    trigger lines
    """
    ## initialize the algorithm 
    def initialize ( self ) :
        """
        Initialize the algorithm 
        """

        ## initialize the base 
        sc = Algo.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc

        ## Create the TisTosTool
        self._tisTosTool = self.tool( cpp.ITriggerTisTos, 'TriggerTisTos' )
        self._tisTosTool.setOfflineInput()

        self._L0TisTosTool = self.tool( cpp.ITriggerTisTos, 'L0TriggerTisTos' )
        self._L0TisTosTool.setOfflineInput()

        self._tesTisTosTool = self.tool( cpp.ITriggerTisTos, 'TESTisTos' )
        self._tesTisTosTool.setOfflineInput()

        ## Get the DistanceCalculator
        self._distanceCalculator = self.tool( cpp.IDistanceCalculator, 'LoKi::DistanceCalculator' )

        ## get an instance of PropertimeFitter
        self._properTimeFitter = self.tool( cpp.ILifetimeFitter, 'PropertimeFitter' )
 
        ## Angles tool
        if self.Configuration.angles():
            self._angleTool  = self.tool( cpp.IP2VVPartAngleCalculator,
                                          self.Configuration.angleTool() ) 

        self._execStage = self.Configuration.execStage()

        self._tisTos = self.Configuration.tistos()[ 'Hlt' ]
        self._L0TisTos = self.Configuration.tistos()[ 'L0' ]

        self._cut = self.Configuration.cut()
        self._selection = self.Configuration.selection()

        self._prescaledLocation = None
        self._detachedLocation = None
        for location in self.InputLocations:
            if location.find( "Prescaled" ) != -1:
                self._prescaledLocation = self.RootInTES + location
            elif location.find( "Detached" ) != -1:
                self._detachedLocation = self.RootInTES + location
        if not self._prescaledLocation:
            self.Error( "Prescaled particle location must be specified" )
            return FAILURE
        elif not self._detachedLocation:
            self.Error( "Detached particle location must be specified" )
            return FAILURE
        
        return SUCCESS
    
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        
        """
        ## NTuple
        tuple  = self.nTuple( 'mDSTReader' )        

        ## get recontructed B mesons
        candidates = self.select ( 'B' , eval( self._cut % self._selection ) )

        if not candidates:
            return SUCCESS 

        candidates = self.uniqueCandidates( candidates )
        
        candidate = None
        relPV = None
        best = self._selection[ 'maxBChi2Ndof' ]
        for c in candidates:
            _B = c.particle()
            _relPV = self.bestVertex( _B )
            if not _relPV:
                continue
            chi2 = DTF_CHI2NDOF( True )( _B )            
            if chi2 < best:
                candidate = c
                relPV = _relPV
                best = chi2

        if not candidate:
            return SUCCESS
        B = candidate.particle()

        ## Which stripping selection selected the particle
        tuple.column( "pre", candidate.prescaled() )
        tuple.column( "det", candidate.detached() )

        ## Propertime fit
        tau   = Double(-99999999.)
        error = Double(999999999.)
        chi2 =  Double(999999999.)
        self._properTimeFitter.fit( relPV, B, tau, error, chi2 )

        # Event info
        odin = self.get( self.RootInTES + 'DAQ/ODIN' )
        runNum = odin.runNumber()
        evtNum = odin.eventNumber()
        tck = odin.triggerConfigurationKey()

        tuple.column( 'runNumber', runNum )
        tuple.column( 'eventNumber', evtNum )
        tuple.column( 'tck', tck )

        # Use DecayTreeFitter to get the B mass with J/psi constraint
        tuple.column( "B_Mass", DTF_FUN( M , False, 'J/psi(1S)' )( B ) )
        tuple.column( "B_Pt", PT( B ) )
        tuple.column( "B_P", P( B ) ) 
        tuple.column( "B_DLS", BPVDLS( B ) )
        tuple.column( "B_tau", tau )
        tuple.column( "B_tauerror", error )
        tuple.column( "B_fitchi2", chi2 )
        tuple.column( "Jpsi_Mass", DTF_FUN( CHILD( M,  1 ), False )( B ) )
        tuple.column( "Jpsi_Pt", DTF_FUN( CHILD( PT,  1 ), False )( B ) )
        tuple.column( "Jpsi_P", DTF_FUN( CHILD( P,  1 ), False )( B ) )
        tuple.column( "mu_pt_max", MAXTREE( PT, ABSID == "mu+" )( B ) )

        # DTF chi2/nDoF
        # Fit tree without J/psi mass constraint for the lifetime, but with
        tuple.column( 'dtf_chi2', best )
        tuple.column( 'dtf_tau', DTF_CTAU( 0, True )( B ) / c_light )
        tuple.column( 'dtf_tauerror', DTF_CTAUERR( 0, True )( B ) )
        tuple.column( 'dtf_B_DLS', DTF_FUN( DLS( relPV ) , True )( B ) )

        # extra vars
        for name, desc in self.Configuration.extraVars().iteritems():
            fun = eval( desc )
            tuple.column( name, fun( B ) )

        # tis/tos
        for name, dec in self._tisTos.iteritems():
            tisTos = self.tisTosSignal( B, dec )
            tuple.column( '%s_dec' % name, tisTos.decision() )
            tuple.column( '%s_tis' % name, tisTos.tis() )
            tuple.column( '%s_tos' % name, tisTos.tos() )

        # L0 tis/tos
        for name, dec in self._L0TisTos.iteritems():
            L0TisTos = self.L0TisTosSignal( B, dec )
            tuple.column( '%s_dec' % name, L0TisTos.decision() )
            tuple.column( '%s_tis' % name, L0TisTos.tis() )
            tuple.column( '%s_tos' % name, L0TisTos.tos() )

        # rejection stages
        decReports = self.get( self.RootInTES + 'Hlt/DecReports' )
        for name, dec in self._execStage.iteritems():
            decReport = decReports.decReport( dec )
            tuple.column( "%s_stage" % name, decReport.executionStage() )

        # angles
        if self.Configuration.angles():
            thetaL = Double(-999.)
            thetaK = Double(-999.)
            phi    = Double(-999.)
            self._angleTool.calculateAngles( B, thetaL, thetaK, phi )

            tuple.column( "thetaL", thetaL )
            tuple.column( "thetaK", thetaK )
            tuple.column( "phi", phi )

            theta_tr = Double(-999.)
            phi_tr    = Double(-999.)
            theta_v = Double(-999.)
            self._angleTool.calculateTransversityAngles( B, theta_tr, phi_tr, theta_v )

            tuple.column( "theta_tr", theta_tr )
            tuple.column( "phi_tr", phi_tr )
            tuple.column( "theta_v", theta_v )

        tuple.write()

        self.setFilterPassed ( True )

        return SUCCESS

    # convenience function
    def tisTosSignal( self, signal, line ):
        self._tisTosTool.setOfflineInput( signal )
        _tisTos = self._tisTosTool.triggerTisTos( line )
        if self.OutputLevel <= 2:
            print self._tisTosTool.analysisReportTrigger()
        return _tisTos

    # convenience function
    def L0TisTosSignal( self, signal, line ):
        self._L0TisTosTool.setOfflineInput( signal )
        _L0TisTos = self._L0TisTosTool.triggerTisTos( line )
        if self.OutputLevel <= 2:
            print self._L0TisTosTool.analysisReportTrigger()
        return _L0TisTos

    def uniqueCandidates( self, particles ):
        ## Preload TES
        self._tesTisTosTool.triggerSelectionNames( self.RootInTES + ".*" )

        ## split particles by stripping selection
        pre = []
        det = []
        for p in particles:
            if INTES( self._prescaledLocation + "/Particles" )( p ):
                pre.append( p )
            else:
                det.append( p )

        ## Slight optimisation for the case of a single candidate
        if particles.size() == 1:
            return [ Candidate( p, ( 1, 0 ) ) for p in pre ] + [ Candidate( p, ( 0, 1 ) ) for p in det ]

        unique = []
        ## Process prescaled particles
        self._tesTisTosTool.setTriggerInput( self._detachedLocation + "/Particles" )
        for p in pre:
            self._tesTisTosTool.setOfflineInput( p )
            if self._tesTisTosTool.tosTrigger():
                unique.append( Candidate( p, ( 1, 1 ) ) )
            else:
                unique.append( Candidate( p, ( 1, 0 ) ) )
            if self.OutputLevel <= 2:
                print self._tesTisTosTool.analysisReportTrigger()

        ## Process detached particles
        self._tesTisTosTool.setTriggerInput( self._prescaledLocation + "/Particles" )
        for p in det:
            self._tesTisTosTool.setOfflineInput( p )
            if not self._tesTisTosTool.tosTrigger():
                unique.append( Candidate( p, ( 0, 1 ) ) )
            if self.OutputLevel <= 2:
                print self._tesTisTosTool.analysisReportTrigger()

        for c in unique:
            print "unique: %d %d" % ( c.prescaled(), c.detached() )

        return unique
