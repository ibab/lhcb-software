#!/usr/bin/env python
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
#                   $Revision: 162137 $
# Last modification $Date: 2013-09-05 15:18:57 +0200 (Thu, 05 Sep 2013) $
#                by $Author: ibelyaev $
# =============================================================================
"""

Simple script to address ``MCDecayTuple does not contain all events'' problem

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
__version__ = " $Revision: 162137 $ "
__usage__   = """
   no-mc-decays [options] -d GENERICDECAY
                          -z DECAYINQUESTION file1 [ file2 [ ....
   """
__all__     = ( 'noDecays' , ) 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger(__name__)
# =============================================================================

# =============================================================================
## look for ``no-decays''-events
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date  2012-09025
def noDecays ( usage = __usage__ , vers = __version__ ) :
    """
    Look for ``no-decays'' events 
    """
    
    from BenderTools.Parser import makeParser
    parser = makeParser  ( usage , vers  )
    
    parser.add_option (
        '-d'                      ,
        '--decay'                 ,
        type    = 'str'           ,
        dest    = 'GenericDecay'  ,
        help    = 'The decay descriptor for generic decay: script will look for decays that have this generic pattern ' ,
        )
    
    parser.add_option (
        '-z'                        ,
        '--miss'                    ,
        type    = 'str'             ,
        dest    = 'DecayInQuestion' ,
        help    = 'The decay descriptor for "decay-in-question": script will look for decays that have NOT this missing pattern' ,
        )
    
    parser.add_option (
        '-n'                          ,
        '--events'                    ,
        dest    = 'nEvents'           ,
        type    = 'int'               , 
        help    = "Number of events to process " ,
        default = 1000     
        )

    ##
    options , arguments = parser.parse_args()
    
    ## Files must be specfied are mandatory!
    if not arguments : parser.error ( 'No input files are specified' ) 

    if not options.Simulation  : logger.warning ( 'Redefine "Simulation" to be true')
    
    if options.nEvents < 0 :
        logger.info ( 'Redefine "nEvents" to be 10000' ) 
        options.nEvents = 10000 
    if options.OutputLevel < 4 : options.OutputLevel = 4

    #
    ## configure it!
    #
    from BenderTools.DstExplorer import configure 
    configure ( options , arguments )
    
    from Bender.MainMC  import cpp,appMgr,get,run,MCDECTREE,SUCCESS 
    
    ## instantiate the application manager 
    gaudi  = appMgr ()
    
    evtSel = gaudi.evtSel()
    
    evtSvc = gaudi.evtSvc()
    detSvc = gaudi.detSvc()
        
    ## initialize and read the first event
    
    run ( 1 )
    mc = get('/Event/MC/Particles')
    
    ievent = 0 

    decay1 = MCDECTREE ( options.GenericDecay )
    if not decay1.valid() :
        raise TypeError ( 'Invalid decay descriptor "%s"' % options.GenericDecay )
    
    decay2 = MCDECTREE ( options.DecayInQuestion )
    if not decay2.valid() :
        raise TypeError ( 'Invalid decay descriptor "%s"' % options.DecayInQuestion ) 

    cnts = [ cpp.StatEntity() , cpp.StatEntity() , cpp.StatEntity() ] 

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
                logger.warning ('No MC-container found')
                break
        
            decs1 = get ( '/Event/MC/Particles' , decay1 )
            decs2 = get ( '/Event/MC/Particles' , decay2 )
            
            l1 = len ( decs1 )
            l2 = len ( decs2 )

            
            cnts[0] += l1
            cnts[1] += l2
            
            if l1 : cnts[2] +=   ( 0 < l2 )
            
            if l1 and not l2 :
                
                if options.Quiet :
                    for d in decs1 : print d.decay()
                else : print decs1
        
            run ( 1 )
                
        print 120*'*'
        logger.info ( "Generic decays     found     %s " % cnts[0] )
        logger.info ( "Decays-in-question found(1)  %s " % cnts[1] )
        logger.info ( "Decays-in-question found(2)  %s " % cnts[2] )
        print 120*'*'

        return SUCCESS

    ## run it! 
    return process ( options.nEvents )


# =============================================================================
if '__main__' == __name__ :

    
    print 120*'*'
    
    print  __doc__ 
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__
    
    print 120*'*'
    
    noDecays ( __usage__ , __version__ )
    
    
# =============================================================================
# The END 
# =============================================================================
