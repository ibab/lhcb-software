# =============================================================================
# $Id: bendervcuts.py,v 1.1 2006-10-11 14:45:12 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is helper module for decoration of LoKi vertex functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
#  This is helper module for decoration of LoKi vertex  functions/cuts 
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_V      = _LoKi.Vertices

# =============================================================================
_RCV = 'const LHCb::VertexBase*'
# =============================================================================


## @see LoKi::Cuts::VTRUE
VTRUE     = _LoKi.BooleanConstant( _RCV )(True)
## @see LoKi::Cuts::VFALSE
VFALSE    = _LoKi.BooleanConstant( _RCV )(False)
## @see LoKi::Cuts::VALL
VALL      = VTRUE
## @see LoKi::Cuts::VNONE
VNONE     = VFALSE
## @see LoKi::Cuts::VONE
VONE      = _LoKi.Constant     ( _RCV )(1.0)
## @see LoKi::Cuts::VZERO
VZERO     = _LoKi.Constant     ( _RCV )(0.0)
## @see LoKi::Cuts::VMIN
VMIN      = _LoKi.Min          ( _RCV )
## @see LoKi::Cuts::VMIN
VMAX      = _LoKi.Max          ( _RCV )
## @see LoKi::Cuts::VSWITCH
VSWITCH   = _LoKi.Switch       ( _RCV )
## @see LoKi::Cuts::VSSWITCH
VSSWITCH  = _LoKi.SimpleSwitch ( _RCV )
## @see LoKi::Cuts::VVALID
VVALID    = _LoKi.Valid        ( _RCV )()
## @see LoKi::Cuts::VSAME
VSAME     = _LoKi.TheSame      ( _RCV )
## @see LoKi::Cuts::PRIMARY
PRIMARY   = _LoKi.Vertices.IsPrimary   ()
## @see LoKi::Cuts::ISPRIMARY
ISPRIMARY =  PRIMARY
## @see LoKi::Cuts::TECHNIQUE
TECHNIQUE = _LoKi.Vertices.Technique   ()
## @see LoKi::Cuts::VTECHNIQUE
VTECHNIQUE = TECHNIQUE
## @see LoKi::Cuts::VCHI2
VCHI2     = _LoKi.Vertices.VertexChi2  ()
## @see LoKi::Cuts::VDOF
VDOF      = _LoKi.Vertices.VertexDoF  ()
## @see LoKi::Cuts::VXNDOF
VXNDOF    =  VDOF 
## @see LoKi::Cuts::VX 
VX        = _LoKi.Vertices.PositionX  ()
## @see LoKi::Cuts::VY
VY        = _LoKi.Vertices.PositionY  ()
## @see LoKi::Cuts::VZ 
VZ        = _LoKi.Vertices.PositionZ  ()
## @see LoKi::Cuts::VHASINFO
VHASINFO  = _LoKi.Vertices.HasInfo
## @see LoKi::Cuts::HASINFO
HASINFO   =  VHASINFO 
## @see LoKi::Cuts::VINFO
VINFO     = _LoKi.Vertices.Info
## @see LoKi::Cuts::NTRACKS
NTRACKS   = _LoKi.Vertices.NumberOfTracks   ()
## @see LoKi::Cuts::NPRONGS 
NPRONGS   = _LoKi.Vertices.NumberOfOutgoing ()
## @see LoKi::Cuts::VVDIST
VVDIST    = _LoKi.Vertices.VertexDistance 
## @see LoKi::Cuts::VVD
VVD       = VVDIST
## @see LoKi::Cuts::VVDSIGN
VVDSIGN   = _LoKi.Vertices.VertexSignedDistance 
## @see LoKi::Cuts::VVDCHI2 
VVDCHI2   = _LoKi.Vertices.VertexChi2Distance 
## @see LoKi::Cuts::MINVVD
MINVVD    = _LoKi.Vertices.MinVertexDistance 
## @see LoKi::Cuts::MINVVDCHI2
MINVVDCHI2 = _LoKi.Vertices.MinVertexChi2Distance 


if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
