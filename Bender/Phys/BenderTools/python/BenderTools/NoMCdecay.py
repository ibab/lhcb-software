#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/NoMCdecays.py
#
#  Simple script to address "MCDecayTuple does not contain all events" problem
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
# @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-09-25
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
""" Simple script to address ``MCDecayTuple does not contain all events'' problem

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

   no-mc-decays [options] -d GENERUCDECAY
                          -z DECAYINQUESTION file1 [ file2 [...


"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2013-09-25 "
__version__ = " $Revision$ "
__all__     = ( 'noDecays' , ) 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.NoMCdecay' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## look for ``no-decays''-events
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date  2012-09025
def noDecays ( config  ) :
    """
    Look for ``no-decays'' events 
    """
    if not config.Simulation  :
        logger.warning ( 'Redefine "Simulation" to be true')
        
    if config.nEvents < 0 :
        logger.info ( 'Redefine "nEvents" to be 10000' ) 
        config.nEvents = 10000 
    if config.OutputLevel < 4 : config.OutputLevel = 4

    #
    ## configure it!
    #
    from BenderTools.DstExplorer import configure 
    configure ( config )
    
    from Bender.MainMC  import cpp,appMgr,get,run,MCDECTREE,SUCCESS, FAILURE  
    
    ## instantiate the application manager 
    gaudi  = appMgr ()
    
    evtSel = gaudi.evtSel()
    
    evtSvc = gaudi.evtSvc()
    detSvc = gaudi.detSvc()
        
    ## initialize and read the first event
    
    run ( 1 )
    mc = get('/Event/MC/Particles')
    
    ievent = 0 

    decay1 = MCDECTREE ( config.GenericDecay )
    if not decay1.valid() :
        raise TypeError ( 'Invalid decay descriptor "%s"' % config.GenericDecay )
    
    decay2 = MCDECTREE ( config.DecayInQuestion )
    if not decay2.valid() :
        raise TypeError ( 'Invalid decay descriptor "%s"' % config.DecayInQuestion ) 

    cnts = [ cpp.StatEntity() , cpp.StatEntity() , cpp.StatEntity() ] 

    decays = set() 

    ## helper function to process the events 
    def process ( evnts = 100 ) :
        """
        helper function to process the events 
        """
        if evnts < 0 : evnts = 100
        
        ievent = 0
        
        while ievent < evnts : 
            
            ievent += 1
            
            mc    = get ( '/Event/MC/Particles' )
            if not mc :
                logger.error('No MC-container found: do you run over simulated data?')
                return FAILURE 
        
            decs1 = get ( '/Event/MC/Particles' , decay1 )
            decs2 = get ( '/Event/MC/Particles' , decay2 )
            
            l1 = len ( decs1 )
            l2 = len ( decs2 )

            
            cnts[0] += l1
            cnts[1] += l2
            
            if l1 : cnts[2] +=   ( 0 < l2 )
            
            if l1 and not l2 :

                for d in decs1 :
                    decays.add( d.decay() ) 
                    
                if config.Quiet :
                    for d in decs1 : print d.decay()
                else : print decs1
        
            run ( 1 )
                
        logger.info ( 100*'*')
        logger.info ( "Generic decays     found     %s " % cnts[0] )
        logger.info ( "Decays-in-question found(1)  %s " % cnts[1] )
        logger.info ( "Decays-in-question found(2)  %s " % cnts[2] )
        if decays : 
            logger.info ( 100*'*')
            logger.info ( "Problematic decays are: " )
            for d in decays : print '   %s' % d
        logger.info ( 100*'*')

        
        return SUCCESS

    ## run it! 
    return process ( config.nEvents )


# =============================================================================
if '__main__' == __name__ :

    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 
    
    
    
# =============================================================================
# The END 
# =============================================================================
