touch config.tar
gaudirun.py $APPCONFIGROOT/options/DaVinci/DV-Stripping24-Stripping.py /afs/cern.ch/lhcb/software/releases/DBASE/AppConfig/v3r239/options/DaVinci/DV-RawEventJuggler-0_3-to-4_2.py $APPCONFIGROOT/options/DaVinci/DataType-2015.py $STRIPPINGSELECTIONSROOT/tests/data/Reco15a_Run164668.py $APPCONFIGROOT/options/DaVinci/InputType-RDST.py DaVinci-Nevents.py tck.py | tee log
