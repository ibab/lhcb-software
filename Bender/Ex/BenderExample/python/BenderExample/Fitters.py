#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/Fitters.py
#  The simple Bender-based example to compare variosu fitters
#   for Bs-> Jpsi phi selection
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

The simple Bender-based example to compare various fitters
for Bs-> Jpsi phi selection

This file is a part of BENDER project:

``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:

  ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-07-10 "
__version__ = " Version $Revision$ "
# =============================================================================
import LHCbMath.Types 
## import everything from bender 
from Bender.All                    import *
from GaudiKernel.SystemOfUnits     import GeV
from GaudiKernel.PhysicalConstants import c_light 
import math

Gaudi = cpp.Gaudi
from LoKiCore.functions import strings

# =============================================================================
## Simple class for Bs -> Jpsi Phi selection 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-07-10
class Fitters1(AlgoMC) :
    """
    Simple class for Bs -> Psi Phi
    """
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # get all primary vertices
        primaries = self.vselect( 'PVs' , ISPRIMARY )

        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        
        mcbs  = self.mcselect ( 'mcbs'  , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mcpsi = self.mcselect ( 'mcpsi' , '[ B_s0 -> ^(  J/psi(1S) =>  mu+  mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mcphi = self.mcselect ( 'mcphi' , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- ) ^(  phi(1020) =>  K+  K- ) ]CC' )
        mcmu  = self.mcselect ( 'mcmu'  , '[ B_s0 ->  (  J/psi(1S) => ^mu+ ^mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mck   = self.mcselect ( 'mck'   , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- )  (  phi(1020) => ^K+ ^K- ) ]CC' )
        
        if mcbs  . empty() : return self.Warning ( 'No MC-Bs  is found!' , SUCCESS ) # RETURN
        if mcpsi . empty() : return self.Warning ( 'No MC-psi is found!' , SUCCESS ) # RETURN 
        if mcphi . empty() : return self.Warning ( 'No MC-phi is found!' , SUCCESS ) # RETURN 
        if mcmu  . empty() : return self.Warning ( 'No MC-mu  is found!' , SUCCESS ) # RETURN 
        if mck   . empty() : return self.Warning ( 'No MC-K   is found!' , SUCCESS ) # RETURN 

        if 1 != mcbs.size() : return SUCCESS

        matcher = self.mcTruth ( "Bs->PsiPhi")
        mcBs  = MCTRUTH ( matcher , mcbs   ) 
        mcPsi = MCTRUTH ( matcher , mcpsi  ) 
        mcPhi = MCTRUTH ( matcher , mcphi  ) 
        mcMu  = MCTRUTH ( matcher , mcmu   ) 
        mcK   = MCTRUTH ( matcher , mck    ) 
        
        ## select all muons
        muons = self.select ( 'muons' , ( 'mu+' == ABSID ) & mcMu )
        if 2 > muons.size () : return self.Warning ( "No RC-muons" , SUCCESS) # RETURN 
        
        self.select ( "mu+" , muons , 0 < Q )
        self.select ( "mu-" , muons , 0 > Q )
        
        ## select all kaons 
        kaons = self.select( 'kaons'  , ( 'K+'  == ABSID ) & mcK )
        if 2 > kaons.size () : return self.Warning ( "No RC-kaons" , SUCCESS) # RETURN 
        
        self.select ( "K+"  , kaons , 0 < Q )
        self.select ( "K-"  , kaons , 0 > Q )
        
        bs_Xie = self.loop ( "mu+ mu- K+ K-" , "B_s0" , self.particleCombiner('Offline') ) 
        for bs in bs_Xie :
            
            m = bs.mass() / GeV
            if not 5.0 < m < 6.0    : continue
            
            chi2 = VCHI2( bs )
            if not 0   < chi2  < 36     : continue

            if not mcBs ( bs )          : continue
            
            bs.save('Bs_Xie') 


        bs_LoKi = self.loop ( "mu+ mu- K+ K-" , "B_s0" , self.particleCombiner('LoKi') ) 
        for bs in bs_LoKi :
            
            m = bs.mass() / GeV
            if not 5.0 < m < 6.0    : continue
            
            chi2 = VCHI2( bs )
            if not 0   < chi2  < 36     : continue

            if not mcBs ( bs )          : continue
            
            bs.save('Bs_LoKi')
            


        ## copare two particle combiners:
            
        Bs_Xie  = self.selected ( 'Bs_Xie'  )
        Bs_LoKi = self.selected ( 'Bs_LoKi' )

        if Bs_Xie.size() != Bs_LoKi.size() :
            return self.Warning( 'Different number of Bs !'     , SUCCESS )
        if Bs_Xie.empty()  : return SUCCESS 
        if 1 != Bs_Xie.size()              :
            return self.Warning( 'Number of Bs is not equal 1 ' , SUCCESS )

        
        Bs_1  = Bs_Xie  [0]
        Bs_2  = Bs_LoKi [0]
        mc_Bs = mcbs    [0]        

        self.compareBs ( Bs_1 , Bs_2 , mc_Bs )
                
        self.setFilterPassed ( True )
        
        return SUCCESS                                           # RETURN 


    ## compare the results of two fitters:
    def compareBs ( self , Bs_1 , Bs_2 , mc_Bs ) :  
        """
        compare the results of two fitters 
        """
        
        vx_1 = Bs_1.endVertex()
        vx_2 = Bs_2.endVertex()
        
        self.plot ( M (Bs_1) - M (Bs_2) , 'mass-difference'    , -10    , 10     , 200 )
        self.plot ( PT(Bs_1) - PT(Bs_2) , 'PT-difference'      , -10    , 10     , 200 )
        self.plot ( VZ(vx_1) - VZ(vx_2) , 'delta_z difference' , -0.005 , 0.005  , 200 )
        self.plot ( VX(vx_1) - VX(vx_2) , 'delta_x difference' , -0.001 , 0.001  , 200 )
        self.plot ( VY(vx_1) - VY(vx_2) , 'delta_y difference' , -0.001 , 0.001  , 200 )
        
        pv_1 = self.bestVertex( Bs_1 )
        pv_2 = self.bestVertex( Bs_2 )

        ctau1  = LTIME( self.lifetimeFitter() , pv_1 ) * c_light 
        ctau2  = LTIME( self.lifetimeFitter() , pv_2 ) * c_light 

        ## lifetime
        mctau = MCCTAU ( mc_Bs ) 


        ct1 = ctau1( Bs_1 )
        ct2 = ctau2( Bs_2 )

        
        self.plot ( ct1 - mctau , 'sigma(ctau) : the first  fitter' , -0.05  , 0.05  , 200 )
        self.plot ( ct2 - mctau , 'sigma(ctau) : the second fitter' , -0.05  , 0.05  , 200 )
        self.plot ( ct1 - ct2   , 'ctau(1) - ctau(2)'               , -0.001 , 0.001 , 200 )

##         ## try with Wouter's fitter: 
##         ctau_A = DTF_CTAU ( 0 , False )
##         ctau_B = DTF_CTAU ( 0 , True  )
##         ctau_C = DTF_CTAU ( 0 , False , strings( 'J/psi(1S)' ) )
##         ctau_D = DTF_CTAU ( 0 , True  , strings( 'J/psi(1S)' ) )
##         ctau_E = DTF_CTAU ( 0 , False , strings( 'J/psi(1S)' , 'B_s0' ) )
##         ctau_F = DTF_CTAU ( 0 , True  , strings( 'J/psi(1S)' , 'B_s0' ) )
        
##         self.plot ( ctau_A ( Bs_2 )  - mctau , 'sigma(ctau_A)'  , -0.05  , 0.05  , 200 )
##         self.plot ( ctau_B ( Bs_2 )  - mctau , 'sigma(ctau_B)'  , -0.05  , 0.05  , 200 )
##         self.plot ( ctau_C ( Bs_2 )  - mctau , 'sigma(ctau_C)'  , -0.05  , 0.05  , 200 )
##         self.plot ( ctau_D ( Bs_2 )  - mctau , 'sigma(ctau_D)'  , -0.05  , 0.05  , 200 )
##         self.plot ( ctau_E ( Bs_2 )  - mctau , 'sigma(ctau_E)'  , -0.05  , 0.05  , 200 )
##         self.plot ( ctau_F ( Bs_2 )  - mctau , 'sigma(ctau_F)'  , -0.05  , 0.05  , 200 )
        
        ## check the covariance matrices for the vertices:
        cov1 = vx_1.covMatrix()
        cov2 = vx_2.covMatrix()

        ## get the 1/3 of trace that is 1/3 of sum of eigenvalues 
        tr1 = ( cov1(0,0) + cov1(1,1) + cov1(2,2) ) / 3.0 
        tr2 = ( cov2(0,0) + cov2(1,1) + cov2(2,2) ) / 3.0 
        
        self.plot ( math.sqrt( tr1 ) - math.sqrt( tr2)  ,
                    'sqrt(1/3*trace1)-sqrt(1/3*trace2)' , -0.0025 , 0.0025 , 200 )

        # check difference in vertex-matrices 
        for  i in range( 0 , 3 ) :
            # relative difference in diagonal elements 
            dd = ( cov1(i,i)-cov2(i,i) ) / ( cov1(i,i)+cov2(i,i) ) 
            self.plot ( dd , 'relative difference for diagonal coefficients (vertex)' , -0.05, 0.05 , 200 ) 
            for j in range ( i + 1 , 3 ) :
                # difference in correlation coefficients 
                c1 = cov1(i,j) / math.sqrt ( cov1(i,i)*cov1(j,j) )
                c2 = cov2(i,j) / math.sqrt ( cov2(i,i)*cov2(j,j) ) 
                self.plot ( c1 - c2  , 'difference for correlation coefficients (vertex)' , -0.02 , 0.02 , 200 ) 
        
        # check the difference in momentum-matrices
        mcov1 = Gaudi.Math.SymMatrix4x4 ( Bs_1.momCovMatrix() ) 
        mcov2 = Gaudi.Math.SymMatrix4x4 ( Bs_2.momCovMatrix() ) 
        
        # print 'mtrx1 \n', mcov1
        # print 'mtrx2 \n', mcov2

        for  i in range( 0 , 4 ) :
            # relative difference in diagonal elements
            dd = ( mcov1(i,i) - mcov2(i,i) ) / ( mcov1(i,i) + mcov2(i,i) ) 
            self.plot ( dd , 'relative difference for diagonal coefficients (momentum)' , -0.01 , 0.01 , 200 ) 
            for j in range ( i + 1 , 4 ) :
                # difference in correlation coefficients
                c1 = mcov1(i,j) / math.sqrt ( abs ( mcov1(i,i) * mcov1(j,j) ) )
                c2 = mcov2(i,j) / math.sqrt ( abs ( mcov2(i,i) * mcov2(j,j) ) ) 
                self.plot ( c1 - c2 , 'difference for correlation coefficients (momentum)' , -0.01 , 0.01 , 200 ) 
        
        ## momentum-position correlations
        mpcov1 = Bs_1.posMomCovMatrix()
        mpcov2 = Bs_2.posMomCovMatrix()
        
        # print 'mtrxp1 \n', mpcov1
        # print 'mtrxp2 \n', mpcov2

        for i in range ( 0 , 4 ) :
            for j in range ( 0 , 3 ) :
                ## difference
                c1 = mpcov1(i,j) / math.sqrt( abs ( mcov1(i,i) * cov1(j,j) ) )
                c2 = mpcov2(i,j) / math.sqrt( abs ( mcov2(i,i) * cov2(j,j) ) )
                self.plot ( c1 - c2 , 'difference for non-diagonal corrrelations (momentum-vertex)' , -0.01 , 0.01 , 200 ) 

    
    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """

        print 'Offline Combiner     is : ', self.particleCombiner ( 'Offline' )
        print 'LoKi    Combiner     is : ', self.particleCombiner ( 'LoKi'    ) 
        print 'LoKi    ReFitter     is : ', self.particleReFitter ( 'LoKi'    ) 
        print 'LoKi    VertexFitter is : ', self.vertexFitter     ( 'LoKi'    )
        
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return AlgoMC.finalize ( self )


# =============================================================================
## Simple class for Bs -> Jpsi Phi selection 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-07-10
class Fitters2(Fitters1) :
    """
    Simple class for Bs -> Psi Phi
    """
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # get all primary vertices
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        
        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        
        mcbs  = self.mcselect ( 'mcbs'  , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mcpsi = self.mcselect ( 'mcpsi' , '[ B_s0 -> ^(  J/psi(1S) =>  mu+  mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mcphi = self.mcselect ( 'mcphi' , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- ) ^(  phi(1020) =>  K+  K- ) ]CC' )
        mcmu  = self.mcselect ( 'mcmu'  , '[ B_s0 ->  (  J/psi(1S) => ^mu+ ^mu- )  (  phi(1020) =>  K+  K- ) ]CC' )
        mck   = self.mcselect ( 'mck'   , '[ B_s0 ->  (  J/psi(1S) =>  mu+  mu- )  (  phi(1020) => ^K+ ^K- ) ]CC' )
        
        if mcbs  . empty() : return self.Warning ( 'No MC-Bs  is found!' , SUCCESS ) # RETURN
        if mcpsi . empty() : return self.Warning ( 'No MC-psi is found!' , SUCCESS ) # RETURN 
        if mcphi . empty() : return self.Warning ( 'No MC-phi is found!' , SUCCESS ) # RETURN 
        if mcmu  . empty() : return self.Warning ( 'No MC-mu  is found!' , SUCCESS ) # RETURN 
        if mck   . empty() : return self.Warning ( 'No MC-K   is found!' , SUCCESS ) # RETURN 
        
        if 1 != mcbs.size() : return SUCCESS
        
        matcher = self.mcTruth ( "Bs->PsiPhi")
        mcBs  = MCTRUTH ( matcher , mcbs   ) 
        mcPsi = MCTRUTH ( matcher , mcpsi  ) 
        mcPhi = MCTRUTH ( matcher , mcphi  ) 
        mcMu  = MCTRUTH ( matcher , mcmu   ) 
        mcK   = MCTRUTH ( matcher , mck    ) 
        
        ## select all muons
        muons = self.select ( 'muons' , ( 'mu+' == ABSID ) & mcMu )
        if 2 > muons.size () : return self.Warning ( "No RC-muons" , SUCCESS) # RETURN 
        
        self.select ( "mu+" , muons , 0 < Q )
        self.select ( "mu-" , muons , 0 > Q )
        
        ## create J/psi
        psis = self.loop ( 'mu+ mu-' , 'J/psi(1S)' )
        for psi in psis :
            
            m = psi.mass() / GeV
            if not 2.9 < m < 3.2       : continue
            
            chi2 = VCHI2( psi )
            if not 0   < chi2  < 36     : continue

            if not mcPsi ( psi )        : continue 
            
            psi.save('psi')


        psis = self.selected('psi')
        if psis.empty() : return SUCCESS

        ## select all kaons 
        kaons = self.select( 'kaons'  , ( 'K+'  == ABSID ) & mcK )
        if 2 > kaons.size () : return self.Warning ( "No RC-kaons" , SUCCESS) # RETURN 
        
        self.select ( "K+"  , kaons , 0 < Q )
        self.select ( "K-"  , kaons , 0 > Q )


        ## create phis
        phis = self.loop ( 'K+ K-' , 'phi(1020)' )
        for phi in phis :
            
            m = phi.mass() / GeV
            if not  m < 1.1             : continue
            
            chi2 = VCHI2( phi )
            if not 0   < chi2  < 36     : continue

            if not mcPhi ( phi )        : continue 
            
            phi.save('phi')


        
        bs_Xie = self.loop ( "psi phi" , "B_s0" , self.particleCombiner('Offline') ) 
        for bs in bs_Xie :
            
            m = bs.mass() / GeV
            if not 5.0 < m < 6.0      : continue
            
            chi2 = VCHI2( bs )
            if not 0   < chi2  < 36     : continue

            if not mcBs ( bs )          : continue
            
            bs.save('Bs_Xie') 


        #bs_LoKi = self.loop ( "psi phi" , "B_s0" , self.particleCombiner('LoKi') ) 
        #bs_LoKi = self.loop ( "mu+ mu- phi" , "B_s0" , self.particleCombiner('LoKi') ) 
        bs_LoKi = self.loop ( "mu+ mu- K- K+" , "B_s0" , self.particleCombiner('LoKi') ) 
        for bs in bs_LoKi :
            
            m = bs.mass() / GeV
            if not 5.0 < m < 6.0    : continue
            
            chi2 = VCHI2( bs )
            if not 0   < chi2  < 36     : continue

            if not mcBs ( bs )          : continue
            
            bs.save('Bs_LoKi')
            

        ## copare two particle combiners:
            
        Bs_Xie  = self.selected ( 'Bs_Xie'  )
        Bs_LoKi = self.selected ( 'Bs_LoKi' )

        if Bs_Xie.size() != Bs_LoKi.size() :
            return self.Warning( 'Different number of Bs !'     , SUCCESS )
        if Bs_Xie.empty()  : return SUCCESS 
        if 1 != Bs_Xie.size()              :
            return self.Warning( 'Number of Bs is not equal 1 ' , SUCCESS )
        
        
        Bs_1  = Bs_Xie  [0]
        Bs_2  = Bs_LoKi [0]
        mc_Bs = mcbs    [0]        

        self.compareBs ( Bs_1 , Bs_2 , mc_Bs )
                
        self.setFilterPassed ( True )
        
        return SUCCESS                                           # RETURN 

    
# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
    ##
    ## Static Configuration 
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2010'  ,
        Simulation    = True    ,
        HistogramFile = 'Fitters_Histos.root'  
        )
    
    from StandardParticles import StdNoPIDsKaons, StdNoPIDsPions, StdNoPIDsMuons
    InputParticles = [
        StdNoPIDsKaons  . outputLocation () ,
        StdNoPIDsPions  . outputLocation () ,
        StdNoPIDsMuons  . outputLocation () ,
        ]

    ## define input data 
    setData  ( datafiles , catalogs , castor )
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    alg1 = Fitters1(
        'Fitters1'            , 
        ## print histos 
        HistoPrint = True     ,
        ## MC-relations
        PP2MCs     = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        Inputs     = InputParticles  
        )
    
    ## create local algorithm:
    alg2 = Fitters2(
        'Fitters2'            , 
        ## print histos 
        HistoPrint = True     ,
        ## MC-relations
        PP2MCs     = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        Inputs     = InputParticles 
        )
    
    userSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence',True)
    userSeq.Members += [ alg1.name() , alg2.name() ]
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
        
    ## configure the job:
    inputdata = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008919/0000/00008919_00000%03d_1.allstreams.dst' % i for i in range ( 1 , 90 ) 
        ]
    
    configure( inputdata , castor = True )  

    ## run the job
    run(500)

# =============================================================================
# The END 
# =============================================================================
