#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/CheckTrg.py
#
#  Trivial Bender-based script to check trigger decisions for certain lines 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# Usage:
#
# @code
#
#    CheckTrg [options] line file1 [ file2 [ file3 [ file4 ....'
# 
#  @endcode 
#
#  @date   2010-09-10
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

Trivial Bender-based script to check trigger decisions for certain lines 

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    CheckTrg [options] line file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2010-09-10"
__version__ = '$Revision$'
__all__     = (
    'configChkTrg' ,
    'chkTrg'      
    ) 
# =============================================================================
import ROOT 
ROOT.PyConfig.IgnoreCommandLineOptions = True
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## postpone the massive import from Bender 
def chkTrg  ( ) :    
    """
    Create the algorithnm for trigger checks 
    """
    from   Bender.Main          import Algo, SUCCESS, PALL  
    import BenderTools.TisTos   ## add methods for TisTos 
    # =============================================================================
    ## @class CheckTrg
    #  simple class to perform quick check for TisTos 
    #  @date   2010-09-10
    #  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    class CheckTrg(Algo) :
        """
        Simple class to perform quick check for TisTos
        """
        ## the standard initialization 
        def initialize ( self ) :
            """
            Initialization
            """
            sc = Algo.initialize        ( self )
            if sc.isFailure() : return sc
            
            sc = self.tisTos_initialize (      )
            if sc.isFailure() : return sc
            
            return SUCCESS
    
        ## the only one esential method: 
        def analyse  ( self ) :
            """
            The only one essential method
            """
            particles = self.select ( 'all' , PALL )
            
            for p in particles :
                
                pname = p.pname() 
                if not self.triggers.has_key ( pname ) : self.triggers[ pname ] = {}
                self.decisions ( p , self.triggers[ pname ] ) 

            self.setFilterPassed ( not particles.empty() )


            return SUCCESS 

        ## finalization 
        def finalize ( self ) :
            """
            Standard finalization 
            """
            #
            Algo.tisTos_finalize ( self )
            return Algo.finalize ( self )

    return CheckTrg


# =============================================================================
## run the actual machinery machinery
def configChkTrg ( config ) :
    """
    """
    
    # basic configuration 
    from BenderTools.DstExplorer import configure
    configure ( config )

    root = config.RootInTES 
    line = config.teslocation
    
    if root :
        if   0 == line.find ( root + '/') :
            line = line.replace( root + '/' , '' )
            logger.info('Line name is adjusted to be %s' % line )
        elif 0 != config.teslocation.find ( '/Event/' ) :
            tmpl = '/Event/' + line 
            if   0 == tmpl.find ( root ) :
                line = tmpl.replace( root , '' )
                logger.info('Line name is adjusted to be %s' % line  )
                
    if 0 == line.find('/Event/') :
        line = line.replace( '/Event/' , '' )
        logger.info('Line name is adjusted to be %s' % line )

    if       config.Simulation and 0 != line.find ( 'AllStreams/Phys/' ) :
        logger.warning('Suspicious line name %s' % line )
    elif not config.Simulation and 0 != line.find ( 'Phys/' ) :
        logger.warning('Suspicious line name %s' % line )
        
    if not 0 < line.rfind ('/Particles') :
        line = line + '/Particles'
        logger.info('Line name is adjusted to be %s' % line )

    # 
    config.teslocation = line 
    ##
    ## add filters:
    #
    from PhysConf.Filters import LoKi_Filters
    if root : location = root + '/' + line 
    else    : location = '/Event/' + line 
    logger.info("Use filter for non-empty location:'%s'"               % location )
    fltrs = LoKi_Filters ( VOID_Code  = " 0.5 < CONTAINS ('%s',True) " % location )
    from Configurables       import DaVinci
    dv = DaVinci (
        EventPreFilters = fltrs.filters('Filters')  ,
        UserAlgorithms  = [ 'CheckTrg' ]
        )
        
    from BenderTools.Utils import silence, totalSilence
    silence()
    
    if config.Quiet or  5<=config.OutputLevel :
        totalSilence()
    
    
# =============================================================================
if '__main__' == __name__ :

        logger.info ( 100*'*')
        logger.info ( __doc__ ) 
        logger.info ( 100*'*')
        logger.info ( ' Author  : %s ' % __author__   ) 
        logger.info ( ' Version : %s ' % __version__  ) 
        logger.info ( ' Date    : %s ' % __date__     )
        logger.info ( 100*'*')
    
# =============================================================================
# The END 
# =============================================================================

