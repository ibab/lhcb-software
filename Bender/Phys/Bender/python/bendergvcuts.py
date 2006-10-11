# =============================================================================
# $Id: bendergvcuts.py,v 1.1 2006-10-11 14:45:08 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is helper module for decoration of LoKi Gen vertex functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
#  This is helper module for decoration of LoKi Gen vertex functions/cuts 
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_GENV   = _LoKi.GenVertices

# =============================================================================
_GV = 'const HepMC::GenVertex*'
# =============================================================================

## @see LoKi::Cuts::GVTRUE
GVTRUE     = _LoKi.BooleanConstant( _GV )(True)
## @see LoKi::Cuts::GVFALSE
GVFALSE    = _LoKi.BooleanConstant( _GV )(False)
## @see LoKi::Cuts::GVALL
GVALL      = GVTRUE
## @see LoKi::Cuts::GVNONE
GVNONE     = GVFALSE
## @see LoKi::Cuts::GVONE
GVONE      = _LoKi.Constant ( _GV )(1.0)
## @see LoKi::Cuts::GVZERO
GVZERO     = _LoKi.Constant ( _GV )(0.0)
## @see LoKi::Cuts::GVMIN
GVMIN      = _LoKi.Min          ( _GV )
## @see LoKi::Cuts::GVMAX
GVMAX      = _LoKi.Max          ( _GV )
## @see LoKi::Cuts::GVSWITCH
GVSWITCH   = _LoKi.Switch       ( _GV )
## @see LoKi::Cuts::GVSSWITCH
GVSSWITCH  = _LoKi.SimpleSwitch ( _GV )
## @see LoKi::Cuts::GVVALID
GVVALID    = _LoKi.Valid        ( _GV )()
## @see LoKi::Cuts::GVSAME
GVSAME     = _LoKi.TheSame      ( _GV )
## @see LoKi::Cuts::GVBAR
GVBAR      = _LoKi.GenVertices.BarCode()
## @see LoKi::Cuts::GVBARCODE 
GVBARCODE  = GVBAR
## @see LoKi::Cuts::GVTIME
GVTIME     = _LoKi.GenVertices.PositionT()
## @see LoKi::Cuts::GVCOUNT
GVCOUNT    = _LoKi.GenVertices.CountIF
## @see LoKi::Cuts::GVSUM
GVSUM      = _LoKi.GenVertices.SumIF 


if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
