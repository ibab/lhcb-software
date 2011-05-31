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
## @class ReaderAlgo
#  Algorithm to read DST and store variables in an nTuple
#  @author Roel Aaij roel.aaij@cern.ch
#  @date 2010-07-15
class ReaderAlgo( Algo ):
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

        ## Get the DistanceCalculator
        self._distanceCalculator = self.tool( cpp.IDistanceCalculator, 'LoKi::DistanceCalculator' )

        ## get an instance of PropertimeFitter
        self._properTimeFitter = self.tool( cpp.ILifetimeFitter, 'PropertimeFitter' )

        self._tisTos = self.Configuration.tistos()[ 'Hlt' ]
        self._L0TisTos = self.Configuration.tistos()[ 'L0' ]

        self._cut = self.Configuration.cut()
        self._selection = self.Configuration.selection()

        self._execStage = self.Configuration.execStage()

        ## Angles tool
        if self.Configuration.angles():
            self._angleTool  = self.tool( cpp.IP2VVPartAngleCalculator,
                                          self.Configuration.angleTool() ) 

        return SUCCESS
    
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        
        """
        ## NTuple
        tuple = self.nTuple( 'Reader_%s' % self.Configuration.channel() )        

        ## Check for PVs
        PVs = self.get( self.RootInTES + self.InputPrimaryVertices )
        if not PVs or PVs.size() == 0:
            return SUCCESS

        ## get recontructed B+ mesons
        Bs = self.select ( 'B' , eval( self._cut % self._selection ) )
                            
        if not Bs or Bs.size() == 0:
            return SUCCESS 

        for B in Bs:
            relPV = self.bestVertex( B )
            if not relPV: continue

            # proper time fit
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

            tuple.column( "nPVs", PVs.size() )
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
            # Fit tree without J/psi mass constraint for the lifetime, but with PV
            

            tuple.column( 'dtf_chi2', DTF_CHI2NDOF( True )( B ) )
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
                
            for name, dec in self._L0TisTos.iteritems():
                L0TisTos = self.L0TisTosSignal( B, dec )
                tuple.column( '%s_dec' % name, L0TisTos.decision() )
                tuple.column( '%s_tis' % name, L0TisTos.tis() )
                tuple.column( '%s_tos' % name, L0TisTos.tos() )

            tuple.column( "cda", COSPOL( 'mu+' == ID, 'J/psi(1S)' == ID )( B ) )

            decReports = self.get( self.RootInTES + 'Hlt/DecReports' )
            for name, dec in self._execStage.iteritems():
                if not decReports.hasDecisionName( dec ): continue
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
        return self._tisTosTool.triggerTisTos( line )

    # convenience function
    def L0TisTosSignal( self, signal, line ):
        self._L0TisTosTool.setOfflineInput( signal )
        return self._L0TisTosTool.triggerTisTos( line )

