#!/usr/bin/env python2.4
# =============================================================================
## The simple Bender-based example for Bs-> Jpsi phi selection
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The simple Bender-based example for Bs-> Jpsi phi selection """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

# =============================================================================
## import everything form bender 
from Bender.All import * 
# =============================================================================

# =============================================================================
## Simple class for Bs -> Jpsi Phi selection 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Bs2PsiPhi(AlgoMC) :
    """
    Simple class for Bs -> Psi Phi
    """
    
    ## standard constructor
    def __init__ ( self , name = 'Bs2PsiPhi' ) :
        """ standard constructor """ 
        return AlgoMC.__init__ ( self , name )

    ## standard mehtod for analyses
    def analyse( self ) :
        """ standard method for analyses """

        # get all primary vertices
        primaries = self.vselect( 'PVs' , ISPRIMARY )

        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)

        mips = MIPCHI2( primaries, self.geo() ) 
        
        finder  = self.mcFinder()
        mcbs  = finder.find ( '[ B_s0 -> (  J/psi(1S) ->  mu+  mu- {, gamma } ) (  phi(1020) ->  K+  K- ) ]cc' )
        mcpsi = finder.find ( '[ B_s0 -> ( ^J/psi(1S) ->  mu+  mu- {, gamma } ) (  phi(1020) ->  K+  K- ) ]cc' )
        mcphi = finder.find ( '[ B_s0 -> (  J/psi(1S) ->  mu+  mu- {, gamma } ) ( ^phi(1020) ->  K+  K- ) ]cc' ) 
        mcmu  = finder.find ( '[ B_s0 -> (  J/psi(1S) -> ^mu+ ^mu- {, gamma } ) (  phi(1020) ->  K+  K- ) ]cc' )
        mck   = finder.find ( '[ B_s0 -> (  J/psi(1S) ->  mu+  mu- {, gamma } ) (  phi(1020) -> ^K+ ^K- ) ]cc' )
        
        if mcbs  . empty() : return self.Warning ( 'No MC-Bs  is found!' , SUCCESS ) # RETURN
        if mcpsi . empty() : return self.Warning ( 'No MC-psi is found!' , SUCCESS ) # RETURN 
        if mcphi . empty() : return self.Warning ( 'No MC-phi is found!' , SUCCESS ) # RETURN 
        if mcmu  . empty() : return self.Warning ( 'No MC-mu  is found!' , SUCCESS ) # RETURN 
        if mck   . empty() : return self.Warning ( 'No MC-K   is found!' , SUCCESS ) # RETURN 
        
        matcher = self.mcTruth ( "Bs->PsiPhi")
        mcBs  = MCTRUTH ( matcher , mcbs   ) 
        mcPsi = MCTRUTH ( matcher , mcpsi  ) 
        mcPhi = MCTRUTH ( matcher , mcphi  ) 
        mcMu  = MCTRUTH ( matcher , mcmu   ) 
        mcK   = MCTRUTH ( matcher , mck    ) 

        ## select all muons
        muons = self.select ( 'muons' , 'mu+' == ABSID )
        if muons.empty() : return self.Warning ( "No RC-muons" , SUCCESS) # RETURN 
        
        self.select ( "mu+" , muons , 0 < Q )
        self.select ( "mu-" , muons , 0 > Q )
        
        ## select all kaons 
        kaons = self.select( 'kaons'  , 'K+'  == ABSID )
        if kaons.empty() : return self.Warning ( "No RC-kaons" , SUCCESS) # RETURN 
        
        self.select ( "K+"  , kaons , 0 < Q )
        self.select ( "K-"  , kaons , 0 > Q )
        
        ## make a loop over all dimuons
        dimuon = self.loop ( "mu+ mu-" , "J/psi(1S)" )
        for mm in dimuon :
            m12  = mm.mass(1,2) / 1000
            if 3.5 < m12 or 2.5 > m12  : continue
            chi2 = VCHI2( mm )
            if 0 > chi2  or 100 < chi2 : continue
            sc = mm.reFit()
            if sc.isFailure()          : continue
            self.plot ( m12 , " mu mu  mass ", 2.5 , 3.5 )
            mu1 = mm(1)
            mu2 = mm(2)
            if mcMu(mu1) and mcMu(mu2) and mcPsi( mm ) :
                self.plot ( m12 , " mu mu  mass ", 2.5 , 3.5 )
            mm.save("psi")

        ## get all selected psis 
        psi = self.selected("psi")
        if psi.empty() : return self.Warning ( "No RC-psi" , SUCCESS) # RETURN 
        
        ## loop over all dikaons 
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 1.1 < m12  : continue
            mass = M(phi)/1000
            if 0   > mass : continue 
            self.plot ( mass , "K+K- mass " , 1. , 1.1 ) 
            chi2 = VCHI2( phi )
            if 0 > chi2 or 49 < chi2 : continue
            self.plot ( mass , "K+K- mass chi2<49  " , 1. , 1.1 )
            k1 = phi(1)
            k2 = phi(2)
            if mcK(k1) and mcK(k2) and mcPhi(phi) : 
                self.plot ( mass , "K+K- mass mctruth  " , 1. , 1.1 ) 
                phi.save("phi")
                
        phi = self.selected("phi")
        if phi.empty() : return self.Warning ( "No RC-phi" , SUCCESS) # RETURN         

        ## loop over selected psi & phi
        bs = self.loop("psi phi", "B_s0")
        for b in bs :
            m12  = b.mass(1,2) / 1000 
            if 4 > m12 or    6 < m12  : continue                 # CONTINUE  
            chi2 = VCHI2 ( b )
            if 0 > chi2 or 100 < chi2 : continue                 # CONTINUE 
            m = M(b)/1000
            self.plot ( m , " psi phi mass " , 4 , 6 )
            psi = b(1)
            phi = b(2)
            if mcPsi( psi ) :
                self.plot ( m , " psi(MC) phi     mass    " , 4 , 6 )
            if mcPhi( phi ) :
                self.plot ( m , " psi     phi(MC) mass    " , 4 , 6 )
            if mcPsi( psi ) and mcPhi( phi ) :
                self.plot ( m , " psi(MC) phi(MC) mass    " , 4 , 6 )
            if mcBs ( b   ) :
                self.plot ( m , " psi     phi     mass MC " , 4 , 6 )
            if mcBs ( b ) and mcPsi ( psi ) and mcPhi ( phi ) :
                self.plot ( m , " psi(MC) phi(MC) mass MC " , 4 , 6 )

            # find proper primary vertex
            bp = b.particle()
            bv = b.vertex()
            if not bv or not bp : continue                       # CONTINUE 
            pv = LoKi.SelectVertex.selectMin (
                primaries , VIPCHI2 ( bp , self.geo() ) , VALL )
            if not pv :
                self.Warning('No proper primary vertex found!')
                continue                                         # CONTINUE 
            dira = DIRA( pv )
            self.plot ( dira( b ) , "Cosine Direction Angle" , 0.995 , 1.0 ) 
            
            b.save("Bs")
                
        Bs = self.selected("Bs")
        if not Bs.empty() :  self.setFilterPassed( True )        # IMPORTANT 
        
        return SUCCESS                                           # RETURN 
    
    
# =============================================================================
## configure the job
def configure ( **args ) :
    """ Configure the job """
    
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 

    ## read external configruation files
    gaudi.config (
        files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ,
        '$COMMONPARTICLESROOT/options/StandardMuons.opts' ] ,
        options = [
        "PoolDbCacheSvc.Catalog = %s "%input.catalog_CERN ]
        )
    
    ## I am old-fashioned person - I like HBOOK 
    if not 'HbookCnv' in gaudi.DLls : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = args.get('histos','Bs2PsiPhi.hbook')

    
    ## create local algorithm:
    alg = Bs2PsiPhi()

    gaudi.addAlgorithm( alg )

    alg=gaudi.algorithm('Bs2PsiPhi')
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Bs2PsiPhi.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseKaons' ,
        '/Event/Phys/StdLooseMuons' 
        ]
    
    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTSITE',None) and \
           os.environ.has_key('GAUDISITESVCROOT') :
        stager = gaudi.service('StagerSvc')
        stager.BlockSize    = 20
        stager.InitialStage =  5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]
            
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.FILEs ) 
    evtSel.PrintFreq = 100
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure()

    ## run the job
    gaudi.run(5000)


# =============================================================================
# The END 
# =============================================================================
