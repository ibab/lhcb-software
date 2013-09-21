"""
Selection making tagging particles from individual taggers.

"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ('TaggingParticlesSelectionBuilder')

from Configurables import (CombinedParticleMaker,
                           ProtoParticleMUONFilter,
                           ProtoParticleCALOFilter,
                           FilterDesktop)

from PhysSelPython.Wrappers import Selection, MergedSelection, SelectionSequence

class TaggingParticlesSelectionBuilder(object) :

    """
    Class to build a Selection containing all the necessary required
    algorithms to produce particles for flavour tagging. Creates Muons,
    Pions and Electrons and selects them in OR mode. These are passed to
    a FilterDesktop, which places them in outputLocation().
    Example:
    tagSelBuilder = TaggingParticlesSelectionBuilder('Tagging')
    # check locaiton of output:
    tagSelBuilder.selectionSequence.outputLocation()
    
    """
    
    def __init__(self, name='Tagging') :
        
        mName   = name + "Muons"
        eName   = name + "Electrons"
        pName   = name + "Pions"
        tagName = name + 'Particles'

        _taggerMuons = CombinedParticleMaker(mName)
        _taggerMuons.Particle = 'Muon'
        _taggerMuons.addTool(ProtoParticleMUONFilter, name="Muon")
        _taggerMuons.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]

        _taggerElectrons = CombinedParticleMaker(eName)
        _taggerElectrons.Particle = 'Electron'
        _taggerElectrons.addTool(ProtoParticleCALOFilter, name="Electron")
        _taggerElectrons.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'4.0'" ]

        _taggerPions = CombinedParticleMaker(pName)
        _taggerPions.Particle = 'Pion'

        _taggingParticles = FilterDesktop(tagName)
        _taggingParticles.Code = "(P>2.0*GeV)"    

        taggerMuons = Selection(mName, Algorithm = _taggerMuons)
        
        taggerElectrons = Selection(eName, Algorithm = _taggerElectrons)

        taggerPions = Selection(pName, Algorithm = _taggerPions)
        
        allTaggers = MergedSelection(name + 'All',
                                     RequiredSelections = [taggerPions,
                                                           taggerMuons,
                                                           taggerElectrons])

        self.selection = Selection(tagName,
                                   Algorithm = _taggingParticles,
                                   RequiredSelections = [ allTaggers] )

