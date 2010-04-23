#!/usr/bin/env python
# =============================================================================
# $Id: Graig.py,v 1.1 2010-04-23 15:07:47 ibelyaev Exp $ 
# =============================================================================
# @file
# Simple examples to read Strip-DST for  2k+10 data
# @author Greig Cowan
# @date   2010-04-18
# =============================================================================
"""
Simple examples to read StripDST / 2k+10 data
"""
# =============================================================================
__author__  = " Greig Cowan "
__date__    = " 2010-04-18 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
## import everything from bender
import ROOT 
import LHCbMath.Types

from   Bender.All                    import *
from   GaudiKernel.SystemOfUnits     import GeV
from   GaudiKernel.PhysicalConstants import c_light
from   LoKiCore.functions            import * 
import LoKiPhys.Phys


class analyseDiMuon(Algo) :
    
    ## standard constructor
    def __init__ ( self , name = 'analyseDiMuon' , **args ) :
        """
        Standard constructor from algorithm name and properties/attributes
        """ 
        Algo.__init__ ( self , name , **args )
        self.ievent = 0
        self.count  = 0
        
        
    ## standard method for analyses
    def analyse ( self ) :
        """
        Standard method for analyses
        """
        
        # get all particles in this event
        all = self.select('all' , ALL )
       
        # loop on them and print the decay structure for first 20 events 
        self.ievent += 1 
        if self.ievent < 20 :
           for p in all : print p.decay()
                              
        
        # loop over reconstructed candidates:
        for a in all:
            #pv = self.getRelatedPV ( a )
            pv = self.bestPV ( a )
            if not pv: continue
            mass = M ( a ) / GeV 
            self.plot  ( mass , ' mass of di-muon ' , 0 , 5 )            
            
        self.setFilterPassed ( not all.empty() )
        
        return SUCCESS                                           # RETURN 

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
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = '2010'
        ) 
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'DiMuon_Histos_.root') 
    
    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
 
    ## create local algorithm:
    SELECTION = 'DiMuon_For_Early_Bd2KstarMuMu'
    alg = analyseDiMuon (
        PropertiesPrint  = True      , 
        HistoPrint       = True      ,
        NTupleLUN        = "DIMUON"  ,
        InputLocations   =  [ '/Event/Strip/Phys/' + SELECTION ] ,
        )
        
    ## finally inform Application Manager about our algorithm
    gaudi.setAlgorithms( ['UnpackRecVertex',
        alg 
        ] )
    
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
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120
    
    ## configure the job:
    
    configure ( '/castor/cern.ch/grid/lhcb/data/2010/DST/00006312/0000/00006312_00000419_4.Dimuon.dst')
    
    ## run the job
    
    run (-1)
