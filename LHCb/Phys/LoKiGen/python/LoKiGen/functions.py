#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.17 2010-05-30 17:09:55 ibelyaev Exp $
# =============================================================================
## @file LoKiGen/functions.py
#  The full set of useful objects from LoKiGen library 
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-12-01
# =============================================================================
"""
The full set of useful objects from LoKiGen library

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "2007-12-01"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.17 $ "
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

## needed since there is no autoloading of HepMC dictionaries:
import sys,PyCintex

if sys.platform == 'win32' : PyCintex.loadDict (    "HepMCRflx" )  
else                       : PyCintex.loadDict ( "libHepMCRflx" )  
    
# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
HepMC    = cpp.HepMC
Gaudi    = cpp.Gaudi 

_GP = 'const HepMC::GenParticle*'
_GV = 'const HepMC::GenVertex*'

_c1 = std.vector( _GP )
_c2 = std.vector( _GV )

## ranges
GRange   = Gaudi.NamedRange_ ( _c1     )
GVRange  = Gaudi.NamedRange_ ( _c2     )
## range-lists: 
GRangeL  = LoKi.RangeList_  (  GRange )
GVRangeL = LoKi.RangeList_  ( GVRange )

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::GFunc
GFunc  = LoKi.Functor                ( _GP  + ',double' ) 
## @see LoKi::Types::GCuts
GCuts  = LoKi.Functor                ( _GP  + ',bool'   ) 
## @see LoKi::Types::GFun
GFun   = LoKi.FunctorFromFunctor     ( _GP  + ',double' ) 
## @see LoKi::Types::GCut
GCut   = LoKi.FunctorFromFunctor     ( _GP  + ',bool'   ) 
## @see LoKi::Types::GFunc  
GVFunc = LoKi.Functor                ( _GV  + ',double' ) 
## @see LoKi::Types::GCuts
GVCuts = LoKi.Functor                ( _GV  + ',bool'   ) 
## @see LoKi::Types::GFun
GVFun  = LoKi.FunctorFromFunctor     ( _GV  + ',double' ) 
## @see LoKi::Types::GCut
GVCut  = LoKi.FunctorFromFunctor     ( _GV  +  ',bool'  ) 

# =============================================================================
## All concrete types 
# =============================================================================


## @see LoKi::Cuts::G3Q
G3Q      = LoKi.GenParticles.ThreeCharge() 
## @see LoKi::Cuts::GABSID
GABSID    = LoKi.GenParticles.AbsIdentifier()
## @see LoKi::Cuts::GALL
GALL      = LoKi.Constant ( _GP + ',bool' ) ( True ) 
## @see LoKi::Cuts::GANCESTOR
GANCESTOR = LoKi.GenParticles.IsAnAncestor
## @see LoKi::Cuts::GBAR
GBAR      = LoKi.GenParticles.BarCode ()
## @see LoKi::Cuts::GBARCODE 
GBARCODE  = LoKi.GenParticles.BarCode ()
## @see LoKi::Cuts::GBARYON
GBARYON  = LoKi.GenParticles.IsBaryon() 
## @see LoKi::Cuts::GBEAUTY
##GBEAUTY   = GQUARK( LHCb.ParticleID.bottom  )
GBEAUTY   = LoKi.GenParticles.HasQuark ( 5 )
## @see LoKi::Cuts::GCHARGED
GCHARGED  = LoKi.GenParticles.IsCharged ()
## @see LoKi::Cuts::GCHARM
##GCHARM  = GQUARK( LHCb.ParticleID.charm  )
GCHARM    = LoKi.GenParticles.HasQuark ( 4 )
## @see LoKi::Cuts::GCTAU
GCTAU     = LoKi.GenParticles.ProperLifeTime ()
## @see LoKi::Cuts::GDECNODE 
GDECNODE  = LoKi.GenParticles.DecNode
## @see LoKi::Cuts::GDECTREE 
GDECTREE  = LoKi.GenParticles.DecTree
## @see LoKi::Cuts::GDELTAR2
GDELTAR2  = LoKi.GenParticles.DeltaR2   
## @see LoKi::Cuts::GDETA
GDETA     = LoKi.GenParticles.DeltaEta  
## @see LoKi::Cuts::GDPHI
GDPHI     = LoKi.GenParticles.DeltaPhi  
## @see LoKi::Cuts::GDR2
GDR2      = LoKi.GenParticles.DeltaR2   
## @see LoKi::Cuts::GE
GE        = LoKi.GenParticles.Energy  ()
## @see LoKi::Cuts::GETA
GETA      = LoKi.GenParticles.PseudoRapidity ()
## @see LoKi::Cuts::GFALSE
GFALSE    = LoKi.Constant( _GP + ',bool' )(False)
## @see LoKi::Cuts::GFAEVX
GFAEVX   = LoKi.GenParticles.AdapterToEndVertex
## @see LoKi::Cuts::GFAPVX
GFAPVX   = LoKi.GenParticles.AdapterToProductionVertex
## @see LoKi::Cuts::GFROMTREE
GFROMTREE = LoKi.GenParticles.FromHepMCTree
## @see LoKi::Cuts::GHADRON
GHADRON  = LoKi.GenParticles.IsHadron   () 
## @see LoKi::Cuts::GID
GID      = LoKi.GenParticles.Identifier ()
## @see LoKi::Cuts::GINTREE
GINTREE  = LoKi.GenParticles.InTree
## @see LoKi::Cuts::GLEPTON
GLEPTON   = LoKi.GenParticles.IsLepton  () 
## @see LoKi::Cuts::GM
GM       = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GM0
GM0      = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GMASS
GMASS    = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GMESON
GMESON   = LoKi.GenParticles.IsMeson    () 
## @see LoKi::Cuts::GMOMDIST
GMOMDIST  = LoKi.GenParticles.MomentumDistance
## @see LoKi::Cuts::GNEUTRAL
GNEUTRAL  = LoKi.GenParticles.IsNeutral () 
## @see LoKi::Cuts::GNINTREE
GNINTREE  = LoKi.GenParticles.NInTree
## @see LoKi::Cuts::GNLT
GNLT     = LoKi.GenParticles.NominalLifeTime () 
## @see LoKi::Cuts::GNONE
GNONE    = LoKi.Constant( _GP + ',bool' )(False)
## @see LoKi::Cuts::GNUCLEUS
GNUCLEUS = LoKi.GenParticles.IsNucleus () 
## @see LoKi::Cuts::GONE
GONE      = LoKi.Constant ( _GP + ',double' )( 1.0 )
## @see LoKi::Cust::GOSCILLATED
GOSCILLATED = LoKi.GenParticles.Oscillated() 
## @see LoKi::Cust::GOSCILLATED1
GOSCILLATED1 = LoKi.GenParticles.Oscillated1() 
## @see LoKi::Cust::GOSCILLATED2
GOSCILLATED2 = LoKi.GenParticles.Oscillated2() 
## @see LoKi::Cuts::GP
GP       = LoKi.GenParticles.Momentum () 
## @see LoKi::Cuts::GPHI
GPHI      = LoKi.GenParticles.Phi     ()
## @see LoKi::Cuts::GPT
GPT      = LoKi.GenParticles.TransverseMomentum     () 
## @see LoKi::Cuts::GPT0
GPT0     = LoKi.GenParticles.TransverseMomentum     () 
## @see LoKi::Cuts::GPTDIR
GPTDIR    = LoKi.GenParticles.TransverseMomentumRel
## @see LoKi::Cuts::GPTREL
GPTREL    = LoKi.GenParticles.TransverseMomentumRel
## @see LoKi::Cuts::GPX
GPX      = LoKi.GenParticles.MomentumX () 
## @see LoKi::Cuts::GPY
GPY      = LoKi.GenParticles.MomentumY () 
## @see LoKi::Cuts::GPZ
GPZ      = LoKi.GenParticles.MomentumZ () 
## @see LoKi::Cuts::GQUARK
GQUARK    = LoKi.GenParticles.HasQuark
## @see LoKi::Cuts::GSAME
GSAME     = LoKi.TheSame      ( _GP )
## @see LoKi::Cuts::GSTATUS
GSTATUS   = LoKi.GenParticles.Status ( )
## @see LoKi::Cuts::GSTRANGE
## GSTRANGE  = LoKi.GenParticles.HasQuark ( LHCb.ParticleID.strange )
GSTRANGE  = LoKi.GenParticles.HasQuark ( 3 )
## @see LoKi::Cuts::GTHETA
GTHETA    = LoKi.GenParticles.Theta ()
## @see LoKi::Cuts::GTIME
GTIME    = LoKi.GenParticles.ProperLifeTime () 
## @see LoKi::Cuts::GTOP
## GTOP  = LoKi.GenParticles.HasQuark ( LHCb.ParticleID.top )
GTOP     = LoKi.GenParticles.HasQuark ( 6 )
## @see LoKi::Cuts::GTRUE
GTRUE    = LoKi.Constant ( _GP , bool )( True )
## @see LoKi::Cuts::GVALID
GVALID   = LoKi.Valid           ( _GP ) ()
## @see LoKi::Cuts::GVEV
GVEV     = LoKi.GenParticles.ValidEndVertex()
## @see LoKi::Cuts::GZERO
GZERO    = LoKi.Constant       ( _GP + ',double' )( 0.0 )    

## @see LoKi::Cuts::GY 
GY       = LoKi.GenParticles.Rapidity  ()
## @see LoKi::Cuts::GY0 
GY0      = LoKi.GenParticles.Rapidity0 ()

## @see LoKi::Cuts::GVTRUE
GVTRUE     = LoKi.Constant( _GV , bool )(True)
## @see LoKi::Cuts::GVFALSE
GVFALSE    = LoKi.Constant( _GV , bool )(False)
## @see LoKi::Cuts::GVALL
GVALL      = GVTRUE
## @see LoKi::Cuts::GVNONE
GVNONE     = GVFALSE
## @see LoKi::Cuts::GVONE
GVONE      = LoKi.Constant ( _GV + ',double' )(1.0)
## @see LoKi::Cuts::GVZERO
GVZERO     = LoKi.Constant ( _GV + ',double' )(0.0)
## @see LoKi::Cuts::GVVALID
GVVALID    = LoKi.Valid        ( _GV )()
## @see LoKi::Cuts::GVSAME
GVSAME     = LoKi.TheSame      ( _GV )
## @see LoKi::Cuts::GVBAR
GVBAR      = LoKi.GenVertices.BarCode()
## @see LoKi::Cuts::GVBARCODE 
GVBARCODE  = GVBAR
## @see LoKi::Cuts::GVTIME
GVTIME     = LoKi.GenVertices.PositionT()
## @see LoKi::Cuts::GVCOUNT
GVCOUNT    = LoKi.GenVertices.CountIF
## @see LoKi::Cuts::GVSUM
GVSUM      = LoKi.GenVertices.SumIF 



## functional part
#_vgp      = std.vector ( _GP    )
#_vgv      = std.vector ( _GV    )
#_vd       = std.vector ('double')
_vgp      = 'std::vector<const HepMC::GenParticle*>'
_vgv      = 'std::vector<const HepMC::GenVertex*>'
_vd       = 'std::vector<double>'

#
GMaps      = LoKi.Functor             ( _vgp , _vd       )
GMap       = LoKi.FunctorFromFunctor  ( _vgp , _vd       )
GPipes     = LoKi.Functor             ( _vgp , _vgp      )
GPipe      = LoKi.FunctorFromFunctor  ( _vgp , _vgp      )
GFunVals   = LoKi.Functor             ( _vgp , 'double'  )
GFunVal    = LoKi.FunctorFromFunctor  ( _vgp , 'double'  )
GCutVals   = LoKi.Functor             ( _vgp , bool      )
GCutVal    = LoKi.FunctorFromFunctor  ( _vgp , bool      )
GElements  = LoKi.Functor             ( _vgp , _GP       ) 
GElement   = LoKi.FunctorFromFunctor  ( _vgp , _GP       )
GSources   = LoKi.Functor             ('void', _vgp      )
GSource    = LoKi.FunctorFromFunctor  ('void', _vgp      )

#
GVMaps     = LoKi.Functor             ( _vgv , _vd       )
GVMap      = LoKi.FunctorFromFunctor  ( _vgv , _vd       )
GVPipes    = LoKi.Functor             ( _vgv , _vgv      )
GVPipe     = LoKi.FunctorFromFunctor  ( _vgv , _vgv      )
GVFunVals  = LoKi.Functor             ( _vgv , 'double'  )
GVFunVal   = LoKi.FunctorFromFunctor  ( _vgv , 'double'  )
GVCutVals  = LoKi.Functor             ( _vgv , bool      )
GVcutVal   = LoKi.FunctorFromFunctor  ( _vgv , bool      )
GVElements = LoKi.Functor             ( _vgv , _GV       ) 
GVElement  = LoKi.FunctorFromFunctor  ( _vgv , _GV       ) 


GSOURCE    = LoKi.GenParticles.SourceTES

GEMPTY      = LoKi.Functors.Empty ( _GP ) ()
GVEMPTY     = LoKi.Functors.Empty ( _GV ) ()
GSIZE       = LoKi.Functors.Size  ( _GP ) ()
GVSIZE      = LoKi.Functors.Size  ( _GV ) () 

# =============================================================================
if '__main__' == __name__ :
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 
        
# =============================================================================
# The END
# =============================================================================
