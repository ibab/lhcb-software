from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True, DataType = 'MC09' )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT.dst']
        )
