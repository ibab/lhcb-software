#!/usr/bin/env python

import KaliCalo.Kali as Kali

histos  = Kali.HistoMap  ()
lambdas = Kali.LambdaMap ()

mydir ='/afs/cern.ch/user/i/ibelyaev/cmtuser/DaVinci_v25r2p1/Phys/KaliCalo/cmt/'

histos.read(mydir + 'histos_db.gz')

from KaliCalo.Analyse import analyse

analyse ( histos , lambdas )

lambdas.save ( 'lambdas_db.gz')

