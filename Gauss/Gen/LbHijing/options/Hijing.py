from Configurables import Generation, MinimumBias

Generation().SampleGenerationTool = "MinimumBias"
Generation().addTool( MinimumBias ) 
Generation().MinimumBias.ProductionTool = "HijingProduction"

