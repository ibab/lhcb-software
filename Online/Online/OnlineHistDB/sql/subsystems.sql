insert into SUBSYSTEM(SSName) VALUES('L0');
insert into SUBSYSTEM(SSName) VALUES('TELL1');
insert into SUBSYSTEM(SSName) VALUES('HLT');
insert into SUBSYSTEM(SSName) VALUES('VELO');
insert into SUBSYSTEM(SSName) VALUES('TT');
insert into SUBSYSTEM(SSName) VALUES('OT');
insert into SUBSYSTEM(SSName) VALUES('IT');
insert into SUBSYSTEM(SSName) VALUES('RICH1');
insert into SUBSYSTEM(SSName) VALUES('RICH2');
insert into SUBSYSTEM(SSName) VALUES('RICH');
insert into SUBSYSTEM(SSName) VALUES('SPD');
insert into SUBSYSTEM(SSName) VALUES('ECAL');
insert into SUBSYSTEM(SSName) VALUES('HCAL');
insert into SUBSYSTEM(SSName) VALUES('MUON');
insert into SUBSYSTEM(SSName) VALUES('DAQ');
insert into SUBSYSTEM(SSName) VALUES('BEAM');
insert into SUBSYSTEM(SSName) VALUES('RECO');

insert into ERGOSUM(version,apiversion,alglist) VALUES(16,'v8r0',0);
insert into PAGEFOLDER VALUES('/',NULL);


insert into CONDITIONS(IBIT,TEXT,DIMSERVICE) 
  VALUES(0,'LHC in PHYSICS state','LHCbStatus/LHC');
insert into CONDITIONS(IBIT,TEXT,DIMSERVICE) 
  VALUES(1,'VELO closed','LHCbStatus/VELO');
insert into CONDITIONS(IBIT,TEXT,DIMSERVICE) 
  VALUES(2,'HV ready (global state)','LHCbStatus/HV');
