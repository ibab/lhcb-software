#
class Selection :
    def __init__(self, mainSequence, mainSelector, mainOptions, dataFiles) :
        self.mainSequence = mainSequence
        self.mainSelector = mainSelector
        self.mainLocation = 'Phys/'+self.mainSelector
        self.mainOptions  = mainOptions
        self.dataFiles    = dataFiles
    def clone(self) :
        return Selection (self.mainSequence,self.mainSelector, self.mainOptions, self.dataFiles)
