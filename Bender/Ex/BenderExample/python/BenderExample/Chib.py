#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file BenderExample/Chib.py
#  The simple Bender-based example to look for Chi_b-peak 
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-15
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example to look for Chi_b-peak 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = " 2012-02-15 " 
__version__ = " Version $Revision$"
# =============================================================================
## import everything from bender 
from   Bender.Main               import *
from   GaudiKernel.SystemOfUnits import MeV , GeV, mm 
import BenderTools.TisTos 
# =============================================================================
## Simple class to look for Chi_b-peak 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Chib(Algo) :
    """
    Simple class to look for Chi_b-peak 
    """
    ## the standard initialization 
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize        ( self )
        if sc.isFailure() : return sc
        
        triggers = {} 
        triggers[ 'Ups'] = {}
        
        lines         = {}
        lines ['Ups'] = {}
        lines ['Ups'] ['L0TOS'  ] = 'L0(Muon|DiMuon).*Decision'
        lines ['Ups'] ['L0TIS'  ] = 'L0(Hadron|Muon|DiMuon|Photon|Electron)Decision'
        lines ['Ups'] ['Hlt1TOS'] = 'Hlt1(DiMuon|SingleMuonHighPT|MuonTrack).*Decision'
        lines ['Ups'] ['Hlt1TIS'] = 'Hlt1TrackAll.*Decision'
        lines ['Ups'] ['Hlt2TOS'] = 'Hlt2(DiMuon|SingleMuonHighPT).*Decision'
        lines ['Ups'] ['Hlt2TIS'] = 'Hlt2(Charm|Topo|Single|Express|Inc|Tri).*Decision'
        
        sc = self.tisTos_initialize ( triggers , lines )
        if sc.isFailure() : return sc
        
        return SUCCESS
    
    ## standard mehtod for analysis
    def analyse( self ) :
        """
        The standard method for analysis
        """
        ## select dimuons
        dimuons  = self.select  ( 'dimion' , 'Meson -> mu+ mu-' )
        if dimuons.empty()  :
            return self.Warning ( "No dimuons are found!" , SUCCESS )
        
        ## select heavy dimuons
        upsilons = self.select  ( 'ups'    ,
                                  dimuons  ,
                                  ( 8 * GeV <= M ) & ( M < 12 * GeV ) ) 
        if upsilons.empty() :
            return self.Warning ( "No heavy dimuons  are found!" , SUCCESS )
        
        ## collect TisTos-statistics 
        for y in upsilons :
            self.decisions      ( y , self.triggers[ 'Ups' ] ) 
            self.plot ( M ( y ) / GeV , 'm(dimu)' , 8 , 12 , 150 )
            
        ## get all photons 
        allg     = self.select  ( "gamma_all" , 'gamma' == ID )
        if allg.empty() :
            return self.Warning ( "No photons are found!" , SUCCESS )
        
        ## select 300 MeV photons
        gamma   = self.select   ( 'gamma' , allg , PT > 300 * MeV )
        if gamma.empty() :
            return self.Warning ( "No good photons are found!" , SUCCESS )

        ## book n-=tuple
        tup = self.nTuple('Chib')
        
        chi2_dtf = DTF_CHI2NDOF(           True )
        deltaM   = DTF_FUN     ( M - M1  , True )
        mups     = DTF_FUN     ( M       , True )
        minDLLmu = MINTREE     ( ISBASIC , PIDmu - PIDpi )
        kullback = MINTREE     ( ISBASIC & HASTRACK , CLONEDIST ) 
        
        dm_1s = ADMASS('Upsilon(1S)')
        dm_2s = ADMASS('Upsilon(2S)')
        dm_3s = ADMASS('Upsilon(3S)')
        ## make a loop
        chibs = self.loop ( 'ups gamma' , 'chi_b1(1P)' )
        for chib in chibs :

            ## delta mass 
            dm = chib.mass(1,2) - chib.mass(1)
            if dm > 2 * GeV : continue
            
            ups = chib ( 1 )
            gam = chib ( 2 )
            if not ups : continue
            if not gam : continue
            
            mu1 = ups.child(1)
            mu2 = ups.child(2)

            if not mu1 : continue
            if not mu2 : continue

            p = chib.particle()
            if not p        : continue 
            
            tup.column ( 'dm'     , dm           / GeV )
            tup.column ( 'm'      , chib.mass(1) / GeV )
            
            tup.column ( 'c2_dtf' , chi2_dtf ( chib )  )
            tup.column ( 'mu_dtf' , mups     ( ups  )  / GeV )
            tup.column ( 'm_dtf'  , mups     ( chib )  / GeV )
            tup.column ( 'dm_dtf' , deltaM   ( chib )  / GeV )
            
            tup.column ( 'pt_g'   , PT ( gam ) / GeV ) 
            tup.column ( 'cl_g'   , CL ( gam )       ) 
            
            pi0veto = LoKi.Photons.pi0Veto ( gam , allg , 25 * MeV , -1 )
            tup.column ( 'pi0veto'  , pi0veto , 0 , 1 )
            
            
            tup.column ( 'p_mu1'   ,  P  ( mu1 ) / GeV ) 
            tup.column ( 'p_mu2'   ,  P  ( mu2 ) / GeV ) 
            tup.column ( 'pt_mu1'  ,  PT ( mu1 ) / GeV ) 
            tup.column ( 'pt_mu2'  ,  PT ( mu2 ) / GeV ) 
            tup.column ( 'pt_ups'  ,  PT ( ups ) / GeV ) 

            tup.column ( 'dm_1s'   ,  dm_1s ( ups ) / GeV )
            tup.column ( 'dm_2s'   ,  dm_2s ( ups ) / GeV )
            tup.column ( 'dm_3s'   ,  dm_3s ( ups ) / GeV )
            
            tup.column ( 'dll_min' , minDLLmu ( ups ) )
            tup.column ( 'kl_dist' , kullback ( ups ) )
            
            ## fill TisTos info
            self.tisTos ( ups               ,
                          tup               ,
                          'Ups_'            ,
                          self.lines['Ups'] ) 
            
            tup.write()

            chib.save ( 'chib' )

            
        ok = self.selected ('chib')
        
        self.setFilterPassed( 0 < len ( ok )  )
        
        return SUCCESS
    
    ## finalization 
    def finalize ( self ) :
        """
        Standard finalization 
        """
        #
        Algo.tisTos_finalize ( self )
        return Algo.finalize ( self )

# =============================================================================
## configure the job
def configure ( datafiles , catalogs  = [] , castor = False ) :
    """
    Configure the job
    """
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    
    from PhysConf.Filters import LoKi_Filters
    
    ## "/Event/Dimuon/Phys/FullDSTDiMuonDiMuonHighMassLine"
    fltrs = LoKi_Filters (
        STRIP_Code = """
        HLT_PASS_RE ( 'Stripping.*DiMuonHighMass.*Decision' )
        """ ,
        VOID_Code  = """
        0 < CONTAINS ('/Event/Dimuon/Phys/FullDSTDiMuonDiMuonHighMassLine/Particles') 
        """
        )
    
    filters = fltrs.filters('Filters')
    filters.reverse() 
    
    davinci = DaVinci (
        DataType      = '2011' ,
        PrintFreq     = 1000   ,
        EventPreFilters = filters ,
        EvtMax        = -1     ,
        ##
        HistogramFile = 'Chib_Histos.root' ,
        TupleFile     = 'Chib_Tuples.root' ,
        ##
        Lumi          = True ,
        ##
        )
    
##     from Configurables import CondDB
##     CondDB ( UseLatestTags = ["2011"] , 
##              UseOracle     = True     )
    
    from Configurables import Gaudi__IODataManager as IODataManager
    IODataManager().AgeLimit = 2 
    
    # come back to Bender 
    setData ( datafiles , catalogs , castor )
    
    gaudi = appMgr()
    
    alg = Chib (
        'Chib'                      ,   ## Algorithm name ,
        # input particles 
        Inputs = [ '/Event/Dimuon/Phys/FullDSTDiMuonDiMuonHighMassLine/Particles' , 
                   '/Event/Phys/StdLooseAllPhotons/Particles'                     ] ,
        # take care about the proper particle combiner
        ParticleCombiners = { '' : 'LoKi::VertexFitter'} 
        )
    #
    mainSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence', True )
    mainSeq.Members += [ alg.name() ]
    
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
 

    data = [
        '/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000%03d_1.dimuon.dst' % i for i in range(1,120)
        ] + [
        '/lhcb/LHCb/Collision11/DIMUON.DST/00012575/0000/00012575_00000%03d_1.dimuon.dst' % i for i in range(1,254)
        ] 
    
    configure ( data , castor = True )
    
    run ( 500 )
    
    gaudi = appMgr()

    
# =============================================================================
# The END 
# =============================================================================
