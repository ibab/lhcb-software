# Stripping Lines for Study of SameSign Background to DY->MuMu
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# J.Anderson
#
# MuMuSS1 (3.2-5GeV): StdAllLooseMuons & P>10GeV & pT>1.5GeV (10% PRESCALE)
# MuMuSS2 (5-10GeV):  StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS3 (10-20GeV): StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS4 (>20GeV):   StdAllLooseMuons & P>10GeV & pT>3GeV

__all__ = 'default_config', 'MuMuSSConf'

__author__ = (
  None, ## Current
  'J. Anderson', ## previous
)

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, SimpleSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
  'NAME'        : 'MuMuSS',
  'BUILDERTYPE' : 'MuMuSSConf',
  'WGs'         : [ 'QEE'],
  'STREAMS'     : [ 'EW' ],
  'CONFIG'      : {
    'MuMuSSLine1Prescale' : 0.1,
    'MuMuSSLine2Prescale' : 1.0,
    'MuMuSSLine3Prescale' : 1.0,
    'MuMuSSLine4Prescale' : 1.0,
    'MuMuSSLine1MinMass'  : 3.2  * GeV,
    'MuMuSSLine1MaxMass'  :  5.  * GeV,
    'MuMuSSLine2MinMass'  :  5.  * GeV,
    'MuMuSSLine2MaxMass'  : 10.  * GeV,
    'MuMuSSLine3MinMass'  : 10.  * GeV,
    'MuMuSSLine3MaxMass'  : 20.  * GeV,
    'MuMuSSLine4MinMass'  : 20.  * GeV,
    'p'     : 10. * GeV,
    'pT1'   : 1.5 * GeV,
    'pT2'   :  3. * GeV,
  },
}


class MuMuSSConf( LineBuilder ) :

  __configuration_keys__ = default_config['CONFIG'].keys()
    
  def __init__( self, name, config ) :

    LineBuilder.__init__( self, name, config )

    # Define the cuts

    cut1 = '(P>%(p)s) & (PT>%(pT1)s)'%config
    cut2 = '(P>%(p)s) & (PT>%(pT2)s)'%config

    MuMuSS1MassCut = '(MM>%(MuMuSSLine1MinMass)s) & (MM<%(MuMuSSLine1MaxMass)s)'%config
    MuMuSS2MassCut = '(MM>%(MuMuSSLine2MinMass)s) & (MM<%(MuMuSSLine2MaxMass)s)'%config
    MuMuSS3MassCut = '(MM>%(MuMuSSLine3MinMass)s) & (MM<%(MuMuSSLine3MaxMass)s)'%config
    MuMuSS4MassCut = '(MM>%(MuMuSSLine4MinMass)s)'%config


    ## MuMuSS1
    sel = makeSSCombination( name + 'MuMuSS1', cut1, MuMuSS1MassCut )
    self.registerLine(StrippingLine( name + 'Line1',
      prescale  = config[ 'MuMuSSLine1Prescale' ],
      selection = sel,
    ))

    ## MuMuSS2
    sel = makeSSCombination( name + 'MuMuSS2', cut2, MuMuSS2MassCut )
    self.registerLine(StrippingLine( name + 'Line2',
      prescale  = config[ 'MuMuSSLine2Prescale' ],
      selection = sel,
    ))

    ## MuMuSS3
    sel = makeSSCombination( name + 'MuMuSS3', cut2, MuMuSS3MassCut )
    self.registerLine(StrippingLine( name + 'Line3',
      prescale  = config[ 'MuMuSSLine3Prescale' ],
      selection = sel,
    ))

    ## MuMuSS4
    sel = makeSSCombination( name + 'MuMuSS4', cut2, MuMuSS4MassCut )     
    self.registerLine(StrippingLine( name + 'Line4',
      prescale  = config[ 'MuMuSSLine4Prescale' ],
      selection = sel,
    ))
      

def makeSSCombination( name, dcut, mcut ) :
  return SimpleSelection( name, CombineParticles, [StdAllLooseMuons],
    DecayDescriptor    = '[Z0 -> mu- mu-]cc',
    DaughtersCuts      = { 'mu-' : dcut, 'mu+' : dcut },
    MotherCut          = mcut,
    WriteP2PVRelations = False,
  )
