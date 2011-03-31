# =============================================================================
## @file
#  Helper module for decoration on LHCb::Inpector class
#  @see LHCb::Inspector
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-03
#
#                    $Revision$
#  Last modification $Date$
#                 by $    $
#
# =============================================================================
"""
Helper module for decoration on LHCb::Inpector class

>>> import LHCbKernel.Inspector

>>> from GaudiPython.Bindings import gbl as cpp  # get the global namespace
>>> Inspector = cpp.LHCb.Inspector               # get the LHCb::Inspector class


>>> i = Inspector( [ 'InputSelection' , 'OutputSelection'] ) # create the instance

>>> from GaudiPython.Bindings import AppMgr
>>> gaudi = AppMgr()                         # Application Manager

Inspect the certain algorithm:

>>> alg = gaudi.algorithm( 'AlgName')  # get the algorithm
>>> i << alg                           # use the inspector

Inspect the certain service:

>>> svc = gaudi.service ( 'SvcName' )  # get the service
>>> i << svc                           # use the inspector

Inspect the tool

>>> tool = gaudi.tool('ToolName' )     # get the tool
>>> i << tool

Inspect the generic component:


>>> cmp = gaudi.property('ComponentName' ) # get the component
>>> i << cmp                               # inspect the component

Inspect the whole application

>>> i << gaudi                             # inspect the application


"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.BElyaev@nikhef.nl"
__version__ = "$Revision$"
__date__    = " 2008-08-03"
## NB: nothing to be imported
__all__     = ()                                   # NB: nothing to be imported
# =============================================================================

from GaudiPython.Bindings import gbl as _cpp

_Inspector = _cpp.LHCb.Inspector

## helper function for inspection of variosu objects
def _new_shift_ ( self , obj ) :
    """
    Helper function for inspection of various objects:

    >>> obj = ...   # get the object
    >>> ins << obj  # inspect it uinsg the inspector

    """
    iobj = None
    if   hasattr ( obj , '_isvc' ) and obj._isvc : iobj = obj._isvc
    elif hasattr ( obj , '_ialg' ) and obj._ialg : iobj = obj._ialg
    elif hasattr ( obj , '_ip'   ) and obj._ip   : iobj = obj._ip

    if not iobj :
        raise AttributeError, ' cannot find proper valid interface for %s'%obj

    lines = self.inspect ( iobj ).split ('\n')
    for line in lines : print line

    return self

## extend the documentation
_new_shift_.__doc__ += '\n' + _cpp.LHCb.Inspector.inspect.__doc__


# backup old init
_old_init_ = _Inspector.__init__

## new (python-oritnted) constructor for LHCb::Inspector
def _new_init_ ( self    ,
                 props   ,
                 members = ( 'Members' , 'TopAlg' ) ) :
    """

    New (python-oriented) constructor:

    >>> int = Inspector ( ['OutputLevel', 'InputSelection'] )


    """
    if type(props) is str : props = (props,)
    if type(props) is list or type(props) is tuple :
        _vct = _cpp.std.vector('std::string')()
        for p in props : _vct.push_back ( p )
        props = _vct

    if type(members) is str : members = (members,)
    if type(members) is list or type(members) is tuple :
        _vct = _cpp.std.vector('std::string')()
        for p in members : _vct.push_back ( p )
        members = _vct
    return _old_init_ ( self , props , members )

## extend the documentation
_new_init_.__doc__ += '\n' + _old_init_.__doc__

_Inspector.__init__   = _new_init_
_Inspector.__lshift__ = _new_shift_


# =============================================================================
if '__main__' == __name__ :

    print 120*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__
    print 120*'*'


# =============================================================================
# The END
# =============================================================================
