from OfflineSelections import OfflineSelection

class AllJpsiPhiSelection( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "JpsiPhi" )
        self._angles = True
        self._angleTool = "Bs2JpsiPhiAngleCalculator"
        
        self._cut = "ALL"

        self._selection = { }
       
        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool

class JpsiSelection( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Jpsi" )
        self._angles = False
        
        self._cut = "( ( '%(MotherName)s' == ABSID                                       ) \
        & ( in_range( %(minMass)s * MeV, M, %(maxMass)s * MeV )                          ) \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                      > %(minDLLmupi)s       ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                  < %(maxTrChi2DoFMu)s   ) \
        & ( MINTREE( PT, 'mu+' == ABSID )                         > %(minMuPt)s          ) \
        & ( VFASPF( VCHI2PDOF )                                   < %(maxVtxChi2DoF)s    ) )"
       
        self._selection = { "MotherName"       :  "J/psi(1S)",
                            "minMass"          :  3017,
                            "maxMass"          :  3277,
                            "maxTrChi2DoFMu"   :     4,
                            "minMuPt"          :   500,
                            "minDLLmupi"       :     0,
                            "maxVtxChi2DoF"    :    11 }
       
        if selection:
            self._selection.update( selection )
