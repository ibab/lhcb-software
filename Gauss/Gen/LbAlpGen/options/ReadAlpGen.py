from Configurables import Generation
Generation().SampleGenerationTool = "Special"

from Configurables import ReadAlpGen, Special
Generation().addTool( Special )
Generation().Special.ProductionTool = "ReadAlpGen"
Generation().Special.addTool( ReadAlpGen )

Generation().Special.CutTool = "" 

Generation().Special.ReadAlpGen.FileLabel = "Zj0"

Generation().PileUpTool = "FixedNInteractions" 

