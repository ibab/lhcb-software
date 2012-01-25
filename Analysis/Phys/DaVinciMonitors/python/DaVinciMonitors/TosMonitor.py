##############################################################################
# $Id: Jpsi2MuPi.py,v 1.2 2009-05-29 17:30:19 pkoppenb Exp $
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
def oneTosMonitor(name="None",input=None,calo=False):

    from PhysSelPython.Wrappers import Selection, SelectionSequence    
    from Configurables import TisTosParticleTagger, ParticleMonitor, PrintDecayTree
    Tosser = TisTosParticleTagger(name+"Tosser")
    Tosser.TisTosSpecs = { name+"Decision%TOS" : 0 }
    Tosser.ProjectTracksToCalo = calo 
    Tosser.CaloClustForCharged = calo

    s1 = Selection("TOSPresel"+name,
                   Algorithm = Tosser,
                   RequiredSelections = [ input ] )

    tools = [ "MomentumPlotTool" ]
    if ('Muon' in name) : tools += [  "MuonPlotTool" ]
    plotter = ParticleMonitor(name+"TosMoni" , PlotTools = tools )
    s2 = Selection("TOSSel"+name,
                   Algorithm = plotter,
                   RequiredSelections = [ s1 ] )
    ss = SelectionSequence("TOSSeq"+name, TopSelection = s2 )
    return ss.sequence()

def tosMonitors():
    """
    TisTos Monitors
    """
    from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseElectrons
    seqs = []
    seqs.extend( [ oneTosMonitor( "Hlt1TrackAllL0", StdLoosePions ) ] )
    seqs.extend( [ oneTosMonitor( "Hlt1TrackMuon", StdLooseMuons ) ] )
    seqs.extend( [ oneTosMonitor( "L0Hadron", StdLoosePions, calo = True ) ] )
    seqs.extend( [ oneTosMonitor( "L0Muon", StdLooseMuons ) ] )
    seqs.extend( [ oneTosMonitor( "L0Electron", StdLooseElectrons, calo = True ) ] )
    print seqs
    
    return seqs

