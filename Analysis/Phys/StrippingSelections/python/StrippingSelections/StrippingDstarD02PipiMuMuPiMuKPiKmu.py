# $Id: StrippingDstarD02PipiMuMuPiMuKPiKmu.py,v 1.1 2009-12-23 11:56:40 poluekt Exp $

__author__ = ['Walter Bonivento', 'Francesco Dettori']
__date__ = '14 December 2010'
__version__ = '$Revision: 1.1 $'

'''
Analysis selections for D*+ -> pi+ D0(-> mu mu').
This implementation with the Selection python tools was composed in imitation
of Bs2JpsiPhi.py revision 1.4.
'''
__all__ = ('name', 'SelD0MuMu','SelD0PiPi'
           'SelDstarWithD02Mu','SelDstarWithD0PiPiForDstarWithD02MuMu',\
           'SeqDstarWithD02MuMu', 'SeqDstarWithD0PiPiForDstarWithD02MuMu',\
           'lineDstarWithD0PiPiForDstarWithD02MuMu','lineDstarWithD0PiMuForDstarWithD02MuMu')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, bindMembers, StrippingMember
from CommonParticles.Utils import DefaultTrackingCuts
name = "DstarWithD02MuMu"

## Make sure the input particles are declared.  They should already be
##   subscribed to the Data on Demand service.
#from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons
from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdLooseMuons import StdLooseMuons
from CommonParticles.StdLoosePions import StdLoosePions
from CommonParticles.StdLooseD02HH import StdLooseD02KPi, StdLooseD02KK, StdLooseD02PiPi, StdLooseD02KPiDCS

#_stdNoPIDsKaons = DataOnDemand("stdNoPIDsKaons", Location = "Phys/StdNoPIDsKaons")
_stdNoPIDsPions = DataOnDemand("stdNoPIDsPions", Location = "Phys/StdNoPIDsPions")
_stdLooseD02RSKPi = DataOnDemand("stdLooseD02RSKPi", Location = "Phys/StdLooseD02KPi")
_stdLooseD02PiPi = DataOnDemand("stdLooseD02PiPi", Location = "Phys/StdLooseD02PiPi")
_stdLooseMuons = DataOnDemand("stdLooseMuons", Location = "Phys/StdLooseMuons")
_stdLoosePions = DataOnDemand("stdLooseMuons", Location = "Phys/StdLoosePions")
_stdLooseUpPions = DataOnDemand('SelStdLooseUpPions',Location='Phys/StdNoPIDsUpPions')
DefaultTrackingCuts().Types = ["Long","Upstream"]

## make the D0 out of 2 muons
_D0MuMu = CombineParticles('D0MuMu')
_D0MuMu.DecayDescriptor = "[D0 -> mu+ mu-]cc"
_D0MuMu.DaughtersCuts =   {  'mu+' : "(PT>800*MeV) & (P>3000*MeV) & (BPVIPCHI2()>1)"
                               ,'mu-' : "(PT>800*MeV) & (P>3000*MeV) & (BPVIPCHI2()>1)"   }   
_D0MuMu.CombinationCut = "(ADAMASS('D0')<150*MeV)"
_D0MuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<14)  & (BPVDIRA>0.999)" ## & (PT>250*MeV)"
SelD0MuMu = Selection("SelD0MuMu", Algorithm = _D0MuMu, RequiredSelections = [_stdLooseMuons] )

## make the D0 out of 2 Pions
_D0PiPi = CombineParticles('D0PiPi')
_D0PiPi.DecayDescriptor = "[D0 -> pi+ pi-]cc"
_D0PiPi.DaughtersCuts =   {  'pi+' : "(PT>800*MeV) & (P>3000*MeV) & (BPVIPCHI2()>1)"
                               ,'pi-' : "(PT>800*MeV) & (P>3000*MeV) & (BPVIPCHI2()>1)"   }
_D0PiPi.CombinationCut = "(ADAMASS('D0')<75*MeV) "
_D0PiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<14)  & (BPVDIRA>0.999) &  (M > 1815*MeV)" ## & (PT>250*MeV) "
SelD02PiPiForDstarWithD02MuMu = Selection("SelD02PiPiForDstarWithD0MuMu", Algorithm = _D0PiPi, RequiredSelections = [_stdLoosePions] )

'''
## Filter the standard D0s
_D02RSKPiFilter = FilterDesktop("D02RSKPiFilterFor"+name)
#_D02RSKPiFilter.InputLocations = [ _stdLooseD02RSKPi.Location ]
_D02RSKPiFilter.Code = " (MINTREE('K+'==ABSID,PT)>800*MeV)" \
                       "& (MINTREE('pi+'==ABSID,PT)>800*MeV)" \
                       "& (MINTREE('K+'==ABSID,P)>3*GeV)" \
                       "& (MINTREE('pi+'==ABSID,P)>3*GeV)" \
	               "& (MINTREE('K+'==ABSID,BPVIPCHI2())>1)" \
                       "& (MINTREE('pi+'==ABSID,BPVIPCHI2())>1)" \
                       "& (ADMASS('D0')<75*MeV)" \
                       "& (VFASPF(VCHI2/VDOF)<14)  & (BPVDIRA>0.999)"


SelD02RSKPiPiForDstarWithD02MuMu= Selection ("D02RSKPiPiFilterFor"+name,
                                           Algorithm = _D02RSKPiFilter,
                                           RequiredSelections = [  _stdLooseD02RSKPi ] )


_D02PiPiFilter  = _D02RSKPiFilter.clone("D02PiPiFilterFor"+name)
#_D02PiPiFilter.InputLocations = [ _stdLooseD02PiPi.Location ]
'''

#SelD02PiPiForDstarWithD02MuMu= Selection ("D02PiPiFilterFor"+name,
#                                           Algorithm = _D02PiPiFilter,
#                                           RequiredSelections = [  _stdLooseD02PiPi ] )


_DstarWithD02MuMu = CombineParticles( 'DstarWithD02MuMu'
                          , DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
                          , DaughtersCuts = { 'pi+' : '(PT>150*MeV) &  (BPVIPCHI2()<10)' , \
	                                      'D0'  : '(PT>1.8*GeV) & (BPVVDCHI2>16) & (BPVIPCHI2()<10)' }
                          , CombinationCut = "(ADAMASS('D*(2010)+')<600*MeV)"
                          , MotherCut = "(VFASPF(VCHI2)<14) & (M-MAXTREE('D0'==ABSID,M)<(145.5+15)*MeV)"
                         )
_DstarWithD0PiPiForDstarWithD02MuMu = CombineParticles( 'DstarWithD0PiPiForDstarWithD02MuMu'
                          , DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
                          , DaughtersCuts = { 'pi+' : '(PT>150*MeV) &  (BPVIPCHI2()<10)' , \
                                              'D0'  : '(PT>1.8*GeV) & (BPVVDCHI2>16) & (BPVIPCHI2()<10)' }
                          , CombinationCut = "(ADAMASS('D*(2010)+')<600*MeV)"
                          , MotherCut = "(VFASPF(VCHI2)<14) & (M-MAXTREE('D0'==ABSID,M)<(145.5+15)*MeV)"
                         )


SelDstarWithD02MuMu = Selection("SelDstarWithD02MuMu", \
Algorithm = _DstarWithD02MuMu, RequiredSelections = [SelD0MuMu,_stdNoPIDsPions,_stdLooseUpPions] )
SeqDstarWithD02MuMu  = SelectionSequence('SeqDstarWithD02MuMu',TopSelection  = SelDstarWithD02MuMu)

SelDstarWithD0PiPiForDstarWithD02MuMu = Selection("SelDstarWithD0PiPiForDstarWithD02MuMu", \
                                       Algorithm = _DstarWithD0PiPiForDstarWithD02MuMu, \
                                       RequiredSelections = [SelD02PiPiForDstarWithD02MuMu,_stdNoPIDsPions,_stdLooseUpPions] )
SeqDstarWithD0PiPiForDstarWithD02MuMu  = SelectionSequence('SeqDstarWithD0PiPiForDstarWithD02MuMu',  \
                                        TopSelection  = SelDstarWithD0PiPiForDstarWithD02MuMu)



##########################################################
# Create signal and control channels stripping lines #
##########################################################
lineDstarWithD02MuMu = StrippingLine('DstarWithD02MuMu'
               , prescale = 1
               , algos = [ SeqDstarWithD02MuMu ]
               )
lineDstarWithD0PiPiForDstarWithD02MuMu = StrippingLine('DstarWithD0PiPiForDstarWithD02MuMu'
               , prescale = 1
               , algos = [ SeqDstarWithD0PiPiForDstarWithD02MuMu ]
               )

