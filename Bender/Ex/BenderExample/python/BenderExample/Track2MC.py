#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/TrackMC.py
# The simple Bender-based example to illustaret MC-truth access to tracks
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date 2010-04-30
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================
"""
The simple Bender-based example to illustaret MC-truth access to tracks

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-04-30 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision$ "
# =============================================================================
## import everything form bender
from Bender.MainMC import *

# =============================================================================
## Simple class for access MC-truth for tracks 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Track2MC(AlgoMC) :
    """
    Simple class for access MC-truth for tracks 
    """
    ## standard constructor
    def __init__ ( self , name = 'TracksMC' , **args ) :
        """
        Standard constructor
        """ 
        AlgoMC.__init__ ( self , name , **args )

    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        TracksLocation = 'Rec/Track/Best'
        
        tracks = self.get( TracksLocation )

        print '# tarcks ', tracks.size()

        ## get the relation table
        table = self.get( 'Relations/Rec/Track/Default' )

        print '# #tracks/links : ', tracks.size() , table.relations().size() 
        
        t2mc = table
        mc2t = table.inverse()

        for t in tracks :

            ## get all related MC-partiles: 
            mcps = table.relations ( t )
            if not mcps.empty() :
                ## print number of related Mc-particles, ID of the first one and track key 
                mcp = mcps[0]._to() 
                print ' #links  ' , mcps.size() , mcp.pname() , mcp.key() , t.key () 

            
        # get all MC-particles 
        mcparticles = self.mcselect('all', MCALL )

        for mcp in mcparticles :

            trks = mc2t.relations ( mcp )
            if not trks.empty() :
                # print number of tracks, partile name and track key  
                trk = trks[0]._to()
                print ' #tracks ', trks.size() , mcp.pname() , mcp.key() , trk.key(), trks[0].weight() 
                        
        
        ## 
        self.setFilterPassed( True ) 
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
        return AlgoMC.finalize ( self )
    
# =============================================================================
def configure ( datafiles , catalogs  = [] , castor = False ) :
    """
    Perform the real configuration of the job
    """
    
    ##
    ## Static Configuration (``Configurables'')
    ##    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2012' ,
        Simulation    = True   , 
        HistogramFile = 'Track2MC_Histos.root'  
        )

    ## configure  Track <--> MC relation table  
    import LoKiPhysMC.Track2MC_Configuration
    import LoKiMC.MC
    
    ## define the input data 
    setData ( datafiles , catalogs , castor = True ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
        
    ## create local algorithm:
    alg = Track2MC(
        HistoPrint     = True                ,  ## print histos 
        InputLocations = [ 'StdTightKaons' ] ,  ## input particles 
        PP2MCs         = []
        )
    
    ## gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
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
  
    configure ( [
        '/lhcb/MC/DEV/ALLSTREAMS.DST/00018392/0000/00018392_00000016_1.allstreams.dst'
        ] , castor = True
                )
    
                
    
    run(10) 


# =============================================================================
# The END 
# =============================================================================
