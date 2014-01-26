#!/usr/bin/env python 
# =============================================================================
# -*- coding: utf-8 -*-
# =============================================================================
## @file BenderTools/EvtTag.py
#  The helper Python module for creationof tag-collections 
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
#  @date   2011-12-17
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
The helper Python module for creation of tag-collections 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = "2011-12-17"
__version__ = '$Revision$'
__all__     = ()
# =============================================================================
from   GaudiPython.GaudiAlgs import TupleAlgo, SUCCESS 
# ============================================================================
## @class EvtTags
#  simple class/template to build event tag collections 
#  @date   2012-12-17
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
class EvtTags(TupleAlgo) :
    """
    Simple class to perform quick check for TisTos
    """
    ## the only one esential method: 
    def execute ( self ) :
        """
        The only one essential method
        """
        
        etc = self.evtCol('Tags','Tags')
        
        etc.column ( "one"   , 1 )
        etc.column ( "two"   , 2 )
        etc.column ( "three" , 3 )
        
        head    = self.get('/Event')
        address = head.registry().address()
        
        etc.column ( 'Address' , address )
        
        etc.write  ()
        
        return SUCCESS

_seqs = {} 
# ========================================================
## prepare the tags 
def etcSeq ( name  = "GoodEvent" ,
             fname = None        ) : 
    
    from GaudiPython.Bindings import AppMgr

    _g = AppMgr()

    ## configure the service (if needed)
    if   'TagCollectionSvc/EvtTupleSvc' in _g.ExtSvc : pass 
    elif 'EvtTupleSvc'                  in _g.ExtSvc : pass
    else :
        _g.ExtSvc += [ 'TagCollectionSvc/EvtTupleSvc' ]

    if not fname :
        fname = "%s.etc" % name

    tagName = '%sETC'                           % name
    wrtName = 'EvtCollectionStream/%sEtcWriter' % name  
    seqName = 'GaudiSequencer/%sEtcSeq'         % name  

    tags = EvtTags (
        tagName                     ,
        EvtColLUN       = 'GOODTAGS' , 
        EvtColsProduce  = True      , 
        EvtColsPrint    = True      , 
        NTupleProduce   = False     , 
        HistoProduce    = False
        )
    
    #_g.OutStream += [
    # 'GaudiSequencer/%s' % _seqname 
    # ]
    
    writer = _g.algorithm ( wrtName , True )
    writer.ItemList    = [ '/NTUPLES/GOODTAGS/%s/Tags' % tags.name() ]
    writer.EvtDataSvc  =   "EvtTupleSvc" ;
    
    seq = _g.algorithm ( seqName , True )
    seq.Members = [
        tags.name   () ,
        writer.name () 
        ]
    
    return seq     

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

# =============================================================================
# The END 
# =============================================================================
