from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT-2009.dst']
        )
