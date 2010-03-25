#!/usr/bin/env python
# =============================================================================
# =============================================================================


print 'Hello!!!'

if '__main__' == __name__ :

    import ROOT 
    from KaliCalo.Utils import POOLCatalogParser
    import os
    os.system ( ' ls -al ' )
    parser = POOLCatalogParser()

    lfns = parser.lfns()

    pfns = parser.pfns()

    print 'LFNs: ' , lfns
    print 'PFNS: ' , pfns

    ## try to do something useful: 

    import KaliCalo.Kali as Kali 
    
    from   KaliCalo.Pi0FillHistos    import fillDataBase
    from   GaudiKernel.SystemOfUnits import GeV
    
    lambdas = Kali.LambdaMap()
    
    histos, lambdas , badfiles = fillDataBase (
        lambdas                 ,
        pfns                    , 
        dbase_name = 'kali_zdb' , 
        Unit = GeV 
        )


# =============================================================================
# The END 
# =============================================================================



