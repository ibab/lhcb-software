from Gaudi.Configuration import *
from Configurables import GaudiSequencer

class Selection :
    def __init__(self, algo, requiredSelections ) :
        self.algo = algo
        self.requiredSelections = []
        for sel in requiredSelections :
            if type(sel) == str :
                self.dealWithStringSelection(sel) # Std should not do anything
                self.algo.InputLocations += [sel]
            else :
                self.algo.InputLocations += [sel.name()]
                self.requiredSelections.append(sel)
        print "Required Selection Algorithms: ", self.requiredSelections
    def algorithm(self) :
        return self.algo
    def dealWithStringSelection(self, selection) :
        print "Selection.dealWithStringSelection(", selection, "): DO NOTHING"
    def name(self) :
        return self.algo.name()

class SelectionSequence :
    def __init__(self, TopSelection ) :
        self.algo = TopSelection.algorithm()
        self.seq = GaudiSequencer("Seq"+self.algo.name())
        self.seq.Members += [TopSelection.algorithm()]
        self.buildSelectionList( TopSelection.requiredSelections )
        self.seq.Members.reverse()
    def sequence(self) :
        return self.seq
    def name(self) :
        return self.seq.name()
    def topAlgName(self) :
        return self.algo.name()
    def buildSelectionList(self, selections) :
        for sel in selections :
            print "Adding Algo ", sel.name(), " to ", self.seq.name()
            self.seq.Members += [sel.algorithm()]
            self.buildSelectionList( sel.requiredSelections )
