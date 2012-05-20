#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/Lam0_Akshay.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for Lambda0 -> p pi- peak on real data
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
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date   2010-06-21
#
#  Last modification $Date$
#                 by $Author$
# ===========================================================================================
"""

A little bit modified version of original script by Alexander KOZLINSKY and
Thomas BAUER to look for Lambda0 -> p pi- peak on real data

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# ===========================================================================================
__author__   = " Alexander KOZLINSKY, Thomas BAUER & Vanya BELYAEV "
__date__     = " 2010-06-21 "
__version__  = " Verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.Main         import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light

# =============================================================================
## @class Lam0
#  Simple algorithm to study Lambda0 -> p pi-
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Lam0(Algo) :
    """
    Simple algorithm to study Lam0 -> p pi-
    """
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """

        ## get the list of all reconstructe dvertices 
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )

        ## get the list of all protons 
        protons = self.select ( 'proton' , 'p+'  == ABSID )
        if protons.empty() : 
            return self.Warning('No protons are found', SUCCESS )

        ## get the list of all pions 
        pions   = self.select ( 'pion'   , 'pi+' == ABSID )        
        if pions  .empty() : 
            return self.Warning('No pions   are found', SUCCESS )


        lambdas = self.loop ('proton pion' , 'Lambda0')

        for lam in lambdas : 
            
            m12 = lam.mass(1, 2) / GeV
            if m12 > 1.5 : continue
            
            if m12 < 1.25 :
                self.plot ( m12 , 'proton pion mass 1', 1.05 , 1.25 ) 

            ## get the proton as the firss  daughter particle : 
            proton   = lam(1)
            
            ## get the pion   as the second daughter particle :
            pion     = lam(2)

            ## get the charge 
            qProton = Q ( proton )
            qPion   = Q ( pion   )
            
            # check the charge:
            if not -0.5 < ( qProton + qPion ) < 0.5 : continue
            
            if m12 < 1.25 :
                self.plot ( m12 , 'p pi mass, correct sign', 1.05 , 1.25 ) 
            
            ## fit the common vertex & check chi2 
            vchi2 = VCHI2(lam)
            if not 0 <= vchi2 < 100 : continue
            
            if m12 < 1.25 :
                self.plot ( m12 , 'p pi mass, chi2(vx)<100', 1.05 , 1.25 ) 
            
            ## get the correct mass 
            m = M(lam) / GeV;
            if m > 1.250           : continue
          
            self.plot ( m , 'p pi mass, correct mass', 1.05 , 1.25 ) 

            bpv = self.bestVertex ( lam.particle() )
            if not bpv :
                self.Warning('No primary vertex has been found')
                continue
            
            lam.save('lam0')
            

        lams = self.selected('lam0')
        
        self.setFilterPassed ( not lams.empty() )
          
        return SUCCESS 

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )
  
# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Job configuration 
    """
    
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci   
    davinci = DaVinci (
        DataType      = '2010' ,
        PrintFreq     = 1000   ,
        HistogramFile = 'Lam0_Akshay_Histos.root' ,
        TupleFile     = 'Lam0_Akshay_Tuples.root' , 
        Lumi          = False 
        )
    
    from Configurables import CondDB
    CondDB( IgnoreHeartBeat = True ) 
    
    from StandardParticles import ( StdLoosePions        ,
                                    StdNoPIDsDownPions   ,
                                    StdLooseProtons      ,
                                    StdNoPIDsDownProtons )    
    InputParticles = [
        StdLoosePions        .outputLocation () ,
        StdNoPIDsDownPions   .outputLocation () ,
        StdLooseProtons      .outputLocation () ,
        StdNoPIDsDownProtons .outputLocation () 
        ]

    #
    ## define the input data:
    setData ( datafiles , catalogs , castor )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Lam0(
        'Lam0'             ,   ## Algorithm name
        Inputs    = InputParticles
        )
    
    userSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence',True)
    userSeq.Members += [ alg.name() ] 
    
    return SUCCESS 

# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    ## minbias-stream 
    inputdata = [
        ## min-bias stream 
        '/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000%03d_1.minibias.dst' % n for n in range ( 1 , 658 )
        ]
    
    configure ( inputdata , castor = True ) 
    
    run ( 500 )


# =============================================================================
# The END 
# =============================================================================
