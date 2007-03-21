create or replace package OnlineHistDB as
 TYPE sourceh is VARRAY(8) of HCREATOR.SOURCEH1%TYPE; 
 TYPE histotlist is TABLE OF HISTOGRAM.HID%TYPE;
 TYPE floattlist is TABLE OF real;
 TYPE inttlist is TABLE OF int;
 procedure DeclareSubSystem(subsys varchar2);	
 procedure DeclareTask (Name varchar2, ss1 varchar2:='NONE', ss2 varchar2:='NONE', ss3 varchar2:='NONE',
	KRunOnPhysics number :=1, KRunOnCalib number :=0, KRunOnEmpty number :=0);
 procedure DeclareHistogram(tk IN varchar2,algo IN  varchar2 := '', title IN  varchar2:= '_BYSN_', mytype IN varchar2:= '');
 procedure DeclareHistByServiceName(ServiceName IN varchar2);
 function DeclareHistByAttributes(tk IN varchar2,algo IN  varchar2, title IN  varchar2,  mytype IN varchar2 ) 
   return varchar2 ;
 function SetDimServiceName(theHID IN  HISTOGRAM.HID%TYPE, theDSN IN DIMSERVICENAME.SN%TYPE) return number;
 procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2:=NULL);
 procedure DeclareCreatorAlgorithm(Name IN varchar2,Ninp IN number:=0,pars IN parameters,
			thetype IN varchar2 := 'H1D', doc IN varchar2:=NULL);

 function DeclareAnalysis(theSet IN HISTOGRAMSET.HSID%TYPE, Algo IN varchar2, warn IN thresholds, alr IN thresholds, instance IN integer:=1)
	return number;
 procedure SetSpecialAnalysis(theAna IN integer, theHisto IN varchar2,  warn IN thresholds, alr IN thresholds);


 function DeclareHistDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL,reset IN int :=0) return number;
 function DeclareHistoSetDisplayOptions(theSet IN HISTOGRAMSET.HSID%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL,reset IN int :=0) return number;
 function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage PAGE.PAGENAME%TYPE, TheInstance IN int := 1, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL,reset IN int :=0) return number;
 function GetDisplayOptions(theDOID IN int, KLABEL_X OUT varchar2 ,KLABEL_Y OUT varchar2 ,KLABEL_Z OUT varchar2 ,
			KYMIN OUT float ,KYMAX OUT float ,KSTATS OUT smallint , KFILLSTYLE OUT smallint ,
		        KFILLCOLOR  OUT smallint , KLINESTYLE OUT smallint ,KLINECOLOR OUT smallint ,KLINEWIDTH OUT smallint ,
			KDRAWOPTS OUT varchar2 )   return number;	
 function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN PAGE.PAGENAME%TYPE := NULL,TheInstance IN int := 1) return number;

 function GetHID(theHistoName IN varchar2,Subindex OUT HISTOGRAM.IHS%TYPE) return number;
 procedure GetAlgoNpar(theAlg IN varchar2, Npar OUT integer, Ninp OUT integer);
 procedure GetAlgoParname(theAlg IN varchar2, Ipar IN integer, name OUT varchar2);
 procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, Ipar IN integer, warn OUT float, alr OUT float);
 procedure GetHCSettings(theHID IN HISTOGRAM.HID%TYPE, Ipar IN integer, value OUT float);
 function GetHistoAnalysis(theHistoSet IN int, anaids OUT intlist, ananames OUT analist) return number;
 procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE := 0,
                                  theSources sourceh := NULL, thePars IN thresholds := thresholds());
 procedure DeclarePageFolder(theFolder IN varchar2);
 procedure DeclarePage(theName IN varchar2,theFolder IN varchar2,theDoc IN varchar2,hlist IN histotlist,
			Cx IN floattlist,Cy IN floattlist,Sx IN floattlist,Sy IN floattlist);
 function GetPage(theName IN varchar2,theFolder OUT varchar2,theDoc OUT varchar2) return number; 
 PROCEDURE DeletePage(theName IN varchar2);
 function GetHistogramData(theName IN varchar2, thePage IN varchar2,  theInstance IN int,
	theHid OUT varchar2,theHsid OUT int,
	theIhs OUT int,theNhs OUT int, theHstype OUT string, theHstitle OUT string, theSubtitle OUT string,
	theTask OUT string, theAlgo OUT string, theNanalysis OUT int,
	theDescr OUT string, theDoc OUT string, theIsanalysishist OUT int, theCreation OUT int,
        theObsolete OUT int, theDisplay out int, theHSDisplay out int, theSHDisplay out int,
	theDIMServiceName OUT varchar2)
	return number;
 function DeleteHistogramSet(theSet IN HISTOGRAMSET.HSID%TYPE) return number;
 function DeleteHistogram(theHID IN HISTOGRAM.HID%TYPE) return number;
 procedure mycommit(dbschema int := 0);
end OnlineHistDB;
/


----------------------------------------------
----------------------------------------------
create or replace  package body OnlineHistDB as


--------------------------
--PRIVATE FUNCTIONS:
-----------------------


function DeclareDisplayOptions(KLABEL_X IN varchar2 := NULL,KLABEL_Y IN varchar2 := NULL,KLABEL_Z IN varchar2 := NULL,
			KYMIN IN float,KYMAX IN float,KSTATS IN smallint, KFILLSTYLE IN smallint,
		        KFILLCOLOR  IN smallint, KLINESTYLE IN smallint,KLINECOLOR  IN smallint,KLINEWIDTH IN smallint,
			KDRAWOPTS IN varchar2,theDOID IN DISPLAYOPTIONS.DOID%TYPE := NULL,reset IN int := 0) return number is
 myid number := 0;
begin
if (theDOID is NULL) then
  insert into DISPLAYOPTIONS(DOID,LABEL_X,LABEL_Y,LABEL_Z,YMIN,YMAX,STATS,FILLSTYLE,FILLCOLOR,
		LINESTYLE,LINECOLOR,LINEWIDTH,DRAWOPTS) 
	   VALUES(DISPLAYOPTIONS_ID.NEXTVAL,KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,KFILLSTYLE,KFILLCOLOR,
		  KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS);
  SELECT DISPLAYOPTIONS_ID.CURRVAL into myid  from ERGOSUM;
else
  if reset = 0 then
   UPDATE DISPLAYOPTIONS SET  LABEL_X=TNNS(KLABEL_X,LABEL_X),LABEL_Y=TNNS(KLABEL_Y,LABEL_Y),LABEL_Z=TNNS(KLABEL_Z,LABEL_Z),
		YMIN=TNNF(KYMIN,YMIN),YMAX=TNNF(KYMAX,YMAX),
		STATS=TNNI(KSTATS,STATS),FILLSTYLE=TNNI(KFILLSTYLE,FILLSTYLE),FILLCOLOR=TNNI(KFILLCOLOR,FILLCOLOR),
		LINESTYLE=TNNI(KLINESTYLE,LINESTYLE),LINECOLOR=TNNI(KLINECOLOR,LINECOLOR),LINEWIDTH=TNNI(KLINEWIDTH,LINEWIDTH),
		DRAWOPTS=TNNS(KDRAWOPTS,DRAWOPTS)
    WHERE DOID=theDOID;
  else
   UPDATE DISPLAYOPTIONS SET  LABEL_X=KLABEL_X,LABEL_Y=KLABEL_Y,LABEL_Z=KLABEL_Z,YMIN=KYMIN,YMAX=KYMAX,
		STATS=KSTATS,FILLSTYLE=KFILLSTYLE,FILLCOLOR=KFILLCOLOR,
		LINESTYLE=KLINESTYLE,LINECOLOR=KLINECOLOR,LINEWIDTH=KLINEWIDTH,DRAWOPTS=KDRAWOPTS
    WHERE DOID=theDOID;
  end if;
  myid := theDOID;
end if;
return myid;
end DeclareDisplayOptions;

---------------------------------------------------------------------


function myGetHID(theHistoSetName IN varchar2,theSubtitle IN varchar2 := NULL,Subindex OUT HISTOGRAM.IHS%TYPE) return number is
 tk TASK.TASKNAME%TYPE;
 algo HISTOGRAMSET.HSALGO%TYPE;
 title varchar2(300);
 cursor myh(Xsubtit HISTOGRAM.SUBTITLE%TYPE, Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask TASK.TASKNAME%TYPE) 
	is select HSID,IHS from VIEWHISTOGRAM where SUBTITLE=Xsubtit AND TITLE=Xtit AND ALGO=Xalg and TASK=Xtask;
 cursor myhs(Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask TASK.TASKNAME%TYPE) 
	is select HSID from VIEWHISTOGRAM where TITLE=Xtit AND ALGO=Xalg and TASK=Xtask;
 myhsid VIEWHISTOGRAM.HSTYPE%TYPE;
 myihs VIEWHISTOGRAM.IHS%TYPE;
begin
 tk := REGEXP_REPLACE(theHistoSetName,'^(.*)/.*/.*$','\1');
 algo := REGEXP_REPLACE(theHistoSetName,'^.*/(.*)/.*$','\1');
 title := REGEXP_REPLACE(theHistoSetName,'^.*/.*/(.*)$','\1');
 if (theSubtitle is NULL) then
  open myhs(title,algo,tk);
  fetch myhs into myhsid;
  if (myhs%NOTFOUND) then
   Subindex := 0;
   return 0;
  else
   Subindex := 1;
   return myhsid;
  end if;
 else
  open myh(theSubtitle,title,algo,tk);
  fetch myh into myhsid,myihs;
  if (myh%NOTFOUND) then
   Subindex := 0;
   return 0;
  else
   Subindex := myihs;
   return myhsid;
  end if;
 end if;
end myGetHID;

procedure CheckNullValueS( value IN OUT varchar2 ) is
begin
if (value is null) then
 value := '_unset_';
end if;
end CheckNullValueS;

procedure CheckNullValueF( value IN OUT float ) is
begin
if (value is null) then
 value := -999999.;
end if;
end CheckNullValueF;

procedure CheckNullValueI( value IN OUT int ) is
begin
if (value is null) then
 value := -99999;
end if;
end CheckNullValueI;

procedure UpdateDimServiceName(theHID IN  HISTOGRAM.HID%TYPE, ServiceName IN DIMSERVICENAME.SN%TYPE) is
begin
 DELETE FROM DIMSERVICENAME WHERE PUBHISTO=theHID;
 INSERT INTO DIMSERVICENAME(SN,PUBHISTO) VALUES(ServiceName,theHID);
end UpdateDimServiceName;

-----------------------

--------------------------
--PUBLIC FUNCTIONS:
-----------------------

-----------------------
function GetHID(theHistoName IN varchar2,Subindex OUT HISTOGRAM.IHS%TYPE) return number is
 cursor myh
	is select HSID,IHS from VIEWHISTOGRAM where NAME=theHistoName;
 myhsid VIEWHISTOGRAM.HSTYPE%TYPE;
 myihs VIEWHISTOGRAM.IHS%TYPE;
begin
 open myh;
 fetch myh into myhsid,myihs;
 if (myh%NOTFOUND) then
  Subindex := 0;
  return 0;
 else
  Subindex := myihs;
  return myhsid;
 end if;
end GetHID;

-----------------------
 procedure DeclareSubSystem(subsys varchar2) is
 cursor cs is  select SSName from SUBSYSTEM where SSName=subsys;
 myssname SUBSYSTEM.SSNAME%TYPE;
 begin
 open cs;
 fetch cs into  myssname;
 if cs%NOTFOUND then
   insert into SUBSYSTEM(SSNAME) VALUES(subsys);
 end if;
exception
 when OTHERS then 
  raise_application_error(-20050,SQLERRM);
 end DeclareSubSystem;

-----------------------
 procedure DeclareTask (Name varchar2, ss1 varchar2:='NONE', ss2 varchar2:='NONE', ss3 varchar2:='NONE',
	KRunOnPhysics number :=1, KRunOnCalib number :=0, KRunOnEmpty number :=0) is
 tn TASK.TASKNAME%TYPE;
 cursor ct is  select TASKNAME from TASK where TASKNAME=Name;
 t boolean;

 begin
 savepoint beforeDTwrite;
 open ct;
 fetch ct into  tn;

 if ct%NOTFOUND then
   insert into TASK(TASKNAME) VALUES(Name);
 end if;

 update TASK set RUNONPHYSICS=KRunOnPhysics,RUNONCALIB=KRunOnCalib,RUNONEMPTY=KRunOnEmpty where TASKNAME=Name;
 if (ss1 != 'NONE') then -- 'NONE' means do nothing
  if (ss1 != 'NULL') then
    DeclareSubSystem(ss1);
    update TASK set SUBSYS1=ss1 where TASKNAME=Name;
  else
    update TASK set SUBSYS1=NULL where TASKNAME=Name;
  end if;
  if (ss2 != 'NULL') then
    DeclareSubSystem(ss2);
    update TASK set SUBSYS2=ss2 where TASKNAME=Name;
  else
    update TASK set SUBSYS2=NULL where TASKNAME=Name;
  end if;
  if (ss3 != 'NULL') then
    DeclareSubSystem(ss3);
    update TASK set SUBSYS3=ss3 where TASKNAME=Name;
  else
    update TASK set SUBSYS3=NULL where TASKNAME=Name;
  end if;
 end if;
exception
 when OTHERS then 
  ROLLBACK TO beforeDTwrite;
  raise_application_error(-20050,SQLERRM);
 end DeclareTask;
 
-----------------------
 

procedure DeclareHistogram(tk IN varchar2,algo IN  varchar2 := '', title IN  varchar2:= '_BYSN_', 
                           mytype IN varchar2:= '') is
 out HISTOGRAM.HID%TYPE;
 begin
 if (title = '_BYSN_') then
   DeclareHistByServiceName(tk);
 else
   out := DeclareHistByAttributes(tk,algo,title,mytype);
 end if;
end DeclareHistogram;
	  
-----------------------

procedure DeclareHistByServiceName(ServiceName IN varchar2) is
 mytype HISTOGRAMSET.HSTYPE%TYPE; 
 tk TASK.TASKNAME%TYPE;
 algo HISTOGRAMSET.HSALGO%TYPE;
 title varchar2(300);
 cursor mysn(Xsn DIMSERVICENAME.SN%TYPE) is select SN from DIMSERVICENAME where SN=Xsn;
 myfsn  DIMSERVICENAME.SN%TYPE;
 hid HISTOGRAM.HID%TYPE := '';

 begin
 open mysn(ServiceName);
 fetch mysn into myfsn;
 if mysn%NOTFOUND then -- if service name is known, do nothing
  -- decode service name
  mytype := REGEXP_SUBSTR(ServiceName,'^.{3}');
  tk := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]*_(.+)_[^/]*/.+$','\1');
  if (tk = ServiceName) then
     raise_application_error(-20051,'syntax error in Service Name '||ServiceName);
  end if;
  if (tk = 'Adder') then -- expect one more field for the taskname
     tk := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]*_Adder_[^/]*/([^/]+)/[^/]+/[^/]+$','\1');
     if (tk = ServiceName) then
       raise_application_error(-20051,'syntax error in Service Name '||ServiceName);
     end if;
     algo := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]+/[^/]+/([^/]+)/[^/]+$','\1');
     title := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]+/[^/]+/[^/]+/([^/]+)$','\1');
  else	
     algo := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]+/([^/]+)/[^/]+$','\1'); 
     title := REGEXP_REPLACE(ServiceName,'^.{3}/[^/]+/[^/]+/([^/]+)$','\1');
  end if;
  if (algo = ServiceName) then
    raise_application_error(-20051,'syntax error in Service Name '||ServiceName);
  end if;
  if (title = ServiceName) then
    raise_application_error(-20051,'syntax error in Service Name '||ServiceName);
  end if;
  hid := DeclareHistByAttributes(tk,algo,title,mytype);
  if (hid != '_none_') then 
   -- update service name (the dimension or the node or the task serial number could have changed)
   UpdateDimServiceName(hid,ServiceName);
  end if;
 end if;
end  DeclareHistByServiceName;
-----------------------


function DeclareHistByAttributes(tk IN varchar2,algo IN  varchar2, title IN  varchar2, mytype IN varchar2 ) 
  return varchar2 is
 hstitle HISTOGRAMSET.HSTITLE%TYPE;
 subtitle  HISTOGRAM.SUBTITLE%TYPE;
 cursor myh(Xsubtit HISTOGRAM.SUBTITLE%TYPE, Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask TASK.TASKNAME%TYPE) is 
	select HID,HSID,HSTYPE from VIEWHISTOGRAM where (SUBTITLE=Xsubtit or (Xsubtit is NULL and SUBTITLE is NULL)) 
	AND TITLE=Xtit AND ALGO=Xalg and TASK=Xtask;
 cursor myhs(Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask HISTOGRAMSET.HSTASK%TYPE) is 
	select HSID,NHS,NANALYSIS from HISTOGRAMSET where HSTITLE=Xtit AND HSALGO=Xalg and HSTASK=Xtask;
 cursor myhinset(Xhsid HISTOGRAMSET.HSID%TYPE) is select MAX(IHS) from HISTOGRAM WHERE HSET=Xhsid;
 myhid HISTOGRAM.HID%TYPE := '_none_';
 myhsid HISTOGRAMSET.HSID%TYPE;
 mynhs HISTOGRAMSET.NHS%TYPE;
 mynana HISTOGRAMSET.NANALYSIS%TYPE;
 myihs HISTOGRAM.IHS%TYPE;
 cursor myans(Xhisto HISTOGRAM.HID%TYPE) is select ANA,WARNINGS,ALARMS from ANASETTINGS where HISTO=Xhisto;
 myana ANASETTINGS.ANA%TYPE;
 mywn thresholds;
 myal thresholds;
 oldtype  VIEWHISTOGRAM.HSTYPE%TYPE := 'NON'; 

 begin
 savepoint beforeDHwrite;
 if (INSTR(title,'_$') != 0) then
   hstitle :=  REGEXP_REPLACE(title,'^(.*)_\$.*$','\1');
   subtitle :=  REGEXP_REPLACE(title,'^.*_\$(.*)$','\1');
 else
   hstitle := title;
   subtitle := NULL;
 end if;

  -- see if histogram exists
  --  DBMS_OUTPUT.PUT_LINE('histo e'' ___'||tk||'/'||algo||'/'||hstitle||'_$'||subtitle||'___');
 open myh(subtitle,hstitle,algo,tk);
 fetch myh into myhid,myhsid,oldtype;
 if myh%NOTFOUND then -- see if histogram set exists
  open myhs(hstitle,algo,tk);
  fetch myhs into myhsid,mynhs,mynana;
  if myhs%NOTFOUND then -- check that task exists and declare set
    DeclareTask(tk);
    -- create histogram set and histogram
    INSERT INTO HISTOGRAMSET(HSID,NHS,HSTASK,HSALGO,HSTITLE,HSTYPE) 
       VALUES(HistogramSet_ID.NEXTVAL,1,tk,algo,hstitle,mytype);
    INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION) 
       VALUES(HistogramSet_ID.CURRVAL||'/1',HistogramSet_ID.CURRVAL,1,subtitle,SYSTIMESTAMP);
    SELECT HistogramSet_ID.CURRVAL||'/1' INTO myhid from ERGOSUM;
  else -- histogram set exists, create histogram object and update number in set
    open myhinset(myhsid);
    fetch myhinset into myihs;
    if myhinset%NOTFOUND then
     raise_application_error(-20910,'Fatal DB inconsistency when creating histogram: contact DB manager');
    end if;
    mynhs := mynhs+1;
    myihs := myihs+1;
    myhid := myhsid||'/'||myihs;
    INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION) VALUES(myhid,myhsid,myihs,subtitle,SYSTIMESTAMP);
    UPDATE HISTOGRAMSET SET NHS=mynhs where HSID=myhsid;
    -- if analysis exists for set, create it for this histogram, masked by default, with same parameters than first histogram in set
    if (mynana >0) then
      for aset in myans(myhsid||'/1') LOOP
       INSERT INTO ANASETTINGS(ANA,HISTO,MASK,WARNINGS,ALARMS) VALUES(aset.ANA,myhid,1,aset.WARNINGS,aset.ALARMS);
      end LOOP;     
    end if;
  end if;	
 else -- histogram exists, only check dimension 
  if (mytype != oldtype) then
   UPDATE HISTOGRAMSET SET HSTYPE=mytype where HSID=myhsid;
  end if;
 end if;
 return myhid;
exception
 when OTHERS then 
  ROLLBACK TO beforeDHwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistByAttributes;
-----------------------

function SetDimServiceName(theHID IN  HISTOGRAM.HID%TYPE, theDSN IN DIMSERVICENAME.SN%TYPE) return number is
out int :=1;
cursor myh is select HID from HISTOGRAM where HID=theHID;
myhid HISTOGRAM.HID%TYPE;
begin
 open myh;
 fetch myh into myhid;
 if myh%NOTFOUND then
  out := 0;
 else
   UpdateDimServiceName(theHID,theDSN);
 end if;
 return out;
end SetDimServiceName;
-----------------------

procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2:=NULL) is
 cursor al is  select ALGNAME from ALGORITHM where ALGNAME=Name;	
 algo ALGORITHM.ALGNAME%TYPE;
 nin integer := pars.COUNT;
begin
 savepoint beforeCKAwrite;
 open al;
 fetch al into  algo;
 if al%NOTFOUND then
   insert into ALGORITHM(ALGNAME,ALGTYPE,NPARS,ALGPARS) VALUES(Name,'CHECK',nin,pars);
 else
   update ALGORITHM set ALGTYPE='CHECK',NPARS=nin,ALGPARS=pars where ALGNAME=Name;
 end if;
 if (LENGTH(doc) > 0 ) then
    update ALGORITHM set ALGDOC=doc where ALGNAME=Name;
 end if;
EXCEPTION
 when OTHERS then 
  rollback to beforeCKAwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareCheckAlgorithm;

-----------------------
procedure DeclareCreatorAlgorithm(Name IN varchar2,Ninp IN number:=0,pars IN parameters,
		thetype IN varchar2 := 'H1D', doc IN varchar2:=NULL) is
 cursor al is  select ALGNAME from ALGORITHM where ALGNAME=Name;	
 algo ALGORITHM.ALGNAME%TYPE;
 nin integer := pars.COUNT;
 myhctype HISTOGRAMSET.HSTYPE%TYPE;
begin
 savepoint beforeCRAwrite;
 myhctype := thetype;  -- generates exception if thetype is not a correct HSTYPE
 open al;
 fetch al into  algo;
 if al%NOTFOUND then
   insert into ALGORITHM(ALGNAME,ALGTYPE,NINPUT,HCTYPE) VALUES(Name,'HCREATOR',Ninp,thetype);
 else
   update ALGORITHM set ALGTYPE='HCREATOR',NINPUT=Ninp,HCTYPE=thetype where ALGNAME=Name;
 end if;
 if (nin > 0) then
    update ALGORITHM set NPARS=nin,ALGPARS=pars where ALGNAME=Name;
 end if;
 if (LENGTH(doc) > 0 ) then
    update ALGORITHM set ALGDOC=doc where ALGNAME=Name;
 end if;
EXCEPTION
 when OTHERS then 
  rollback to beforeCRAwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareCreatorAlgorithm;

-----------------------

function DeclareAnalysis(theSet IN HISTOGRAMSET.HSID%TYPE, Algo IN varchar2, warn IN thresholds, alr IN thresholds, 
                         instance IN integer :=1) 
	return number is
 cursor hs is  select NHS from HISTOGRAMSET where HSID=theSet;
 mynh HISTOGRAMSET.NHS%TYPE :=0;
 cursor al is  select ALGTYPE,NPARS from ALGORITHM where ALGNAME=Algo;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algon ALGORITHM.NPARS%TYPE;
 cursor ana is select AID from ANALYSIS where HSET=theSet and ALGORITHM=Algo order by AID;
 myid ANALYSIS.AID%TYPE;
 hid HISTOGRAM.HID%TYPE;
 ii int := 0;
begin
 SAVEPOINT beforeDAwrite;
 -- check that required histogram set exists
 open hs;
 fetch hs into  mynh;
 if hs%NOTFOUND then
    raise_application_error(-20001,'Histogram set '||theSet||' does not exist');
 end if; 

 open ana;
 while (ii < instance) LOOP
  fetch ana into myid;
  if (ana%NOTFOUND) then
   ii := 9999;
  else
   ii := ii+1;
  end if;
 end LOOP;
 if (ana%NOTFOUND) then -- create new analysis
   open al;
   fetch al into algotype,algon;
   if (al%NOTFOUND or algotype!='CHECK') then
     raise_application_error(-20002,'Algorithm '||Algo||' does not exist or is not a check histogram');
   else 
    if (warn.COUNT != algon or alr.COUNT != algon) then
        raise_application_error(-20003,'Algorithm '||Algo||' requires '||algon||' parameters');
    else
     INSERT INTO ANALYSIS(AID,HSET,ALGORITHM) VALUES(Analysis_ID.NEXTVAL,theSet,Algo);
     UPDATE  HISTOGRAMSET set NANALYSIS=NANALYSIS+1 where HSID=theSet;
     for i IN 1..mynh LOOP
      hid := theSet||'/'||i;
       INSERT INTO ANASETTINGS(ANA,HISTO,WARNINGS,ALARMS) VALUES(Analysis_ID.CURRVAL,hid,warn,alr);
       SELECT Analysis_ID.CURRVAL into myid  from ERGOSUM;
     end LOOP;
    end if;
   end if;
 else
  -- analysis already exists: update parameters
   UPDATE ANASETTINGS SET WARNINGS=warn,ALARMS=alr where ANA=myid and REGEXP_REPLACE(HISTO,'^(.*)/.*$','\1')=theSet;
 end if;
 return myid;
exception
 when OTHERS then 
  ROLLBACK TO beforeDAwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareAnalysis;

-----------------------

function DeclareHistoSetDisplayOptions(theSet IN HISTOGRAMSET.HSID%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL, reset IN int :=0) return number is
 cursor checko is select HSDISPLAY from HISTOGRAMSET where HSID=theSet; 
 mydoid DISPLAYOPTIONS.DOID%TYPE;
 mySetDisp HISTOGRAMSET.HSDISPLAY%TYPE := NULL;
begin
 savepoint beforeDHSDOwrite;
 open checko;
 fetch checko into mySetDisp;
 if (checko%NOTFOUND) then
  raise_application_error(-20010,'Histogram Set '||theSet||' does not exist!');
 end if;
 mydoid := DeclareDisplayOptions(KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,
		KFILLSTYLE,KFILLCOLOR,KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS,mySetDisp,reset);
 if (mySetDisp is NULL) then
  update HISTOGRAMSET set HSDISPLAY=mydoid where HSID=theSet;
 end if;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHSDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistoSetDisplayOptions;

-----------------------

function DeclareHistDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL, reset IN int :=0) return number is
 cursor checko is select HS.HSID,HS.NHS,H.DISPLAY from HISTOGRAMSET HS,HISTOGRAM H where H.HID=theHID and H.HSET=HS.HSID; 
 mydoid DISPLAYOPTIONS.DOID%TYPE;
 myHSID HISTOGRAMSET.HSID%TYPE;
 myNhs HISTOGRAMSET.NHS%TYPE;
 myDisp HISTOGRAM.DISPLAY%TYPE := NULL;
begin
 savepoint beforeDHDOwrite;
 open checko;
 fetch checko into myHSID,myNhs,myDisp;
 if (checko%NOTFOUND) then
  raise_application_error(-20010,'Histogram '||theHID||' does not exist!');
 end if;
 if (myNhs = 1) then -- take the set and never mind
  mydoid := DeclareHistoSetDisplayOptions(myHSID,KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,
	KFILLSTYLE,KFILLCOLOR,KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS,reset);
 else 
  mydoid := DeclareDisplayOptions(KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,
			KFILLSTYLE,KFILLCOLOR,KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS,myDisp,reset);
  if (myDisp is NULL) then
    update HISTOGRAM set DISPLAY=mydoid where HID=theHID;
  end if; 
 end if;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistDisplayOptions;


-----------------------

function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage PAGE.PAGENAME%TYPE, TheInstance IN int := 1, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL, reset IN int :=0) return number is
 cursor checko is select SDISPLAY from SHOWHISTO where HISTO=theHID and PAGE=thePage and INSTANCE=TheInstance; 
 myDisp HISTOGRAM.DISPLAY%TYPE := NULL;
 mydoid DISPLAYOPTIONS.DOID%TYPE;
begin
 savepoint beforeDHPDOwrite;
 open checko;
 fetch checko into myDisp;
 if (checko%NOTFOUND) then
  raise_application_error(-20010,'Histogram '||theHID||' in  Page '||thePage||' not found');
 end if;
 mydoid := DeclareDisplayOptions(KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,
		KFILLSTYLE,KFILLCOLOR,KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS,myDisp,reset);
 if (myDisp is NULL) then
   update SHOWHISTO set SDISPLAY=mydoid where HISTO=theHID and PAGE=thePage and INSTANCE=TheInstance;
 end if;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHPDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistoPageDisplayOptions;

-----------------------
function GetDisplayOptions(theDOID IN int, KLABEL_X OUT varchar2 ,KLABEL_Y OUT varchar2 ,KLABEL_Z OUT varchar2 ,
			KYMIN OUT float ,KYMAX OUT float ,KSTATS OUT smallint , KFILLSTYLE OUT smallint ,
		        KFILLCOLOR  OUT smallint , KLINESTYLE OUT smallint ,KLINECOLOR OUT smallint ,KLINEWIDTH OUT smallint ,
			KDRAWOPTS OUT varchar2 )   return number is
cursor mydo is select LABEL_X,LABEL_Y,LABEL_Z,YMIN,YMAX,STATS,FILLSTYLE,FILLCOLOR,LINESTYLE,LINECOLOR,LINEWIDTH,DRAWOPTS
	from DISPLAYOPTIONS where DOID=theDOID;
begin
open mydo;
fetch mydo into KLABEL_X,KLABEL_Y,KLABEL_Z,KYMIN,KYMAX,KSTATS,
		KFILLSTYLE,KFILLCOLOR,KLINESTYLE,KLINECOLOR,KLINEWIDTH,KDRAWOPTS;
if (mydo%NOTFOUND) then
 return 0;
else
 CheckNullValueS(KLABEL_X);
 CheckNullValueS(KLABEL_Y);
 CheckNullValueS(KLABEL_Z);
 CheckNullValueF(KYMIN);
 CheckNullValueF(KYMAX);
 CheckNullValueI(KSTATS);
 CheckNullValueI(KFILLSTYLE);
 CheckNullValueI(KFILLCOLOR);
 CheckNullValueI(KLINESTYLE);
 CheckNullValueI(KLINECOLOR);
 CheckNullValueI(KLINEWIDTH);
 CheckNullValueS(KDRAWOPTS);
 return 1;
end if;
end GetDisplayOptions;
-----------------------

function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN PAGE.PAGENAME%TYPE := NULL,
	TheInstance IN int := 1) return number is
 cursor checkp is select SDISPLAY from SHOWHISTO where HISTO=theHID and PAGE=thePage and INSTANCE=TheInstance; 
 cursor checkh is select H.DISPLAY,HS.HSDISPLAY from HISTOGRAMSET HS,HISTOGRAM H where H.HID=theHID and H.HSET=HS.HSID; 
 doid int := NULL;
 disp int;
 hsdisp int;
begin
 if (thePage is not NULL) then
  open checkp;
  fetch checkp into doid;
 end if;
 if (doid is NULL) then
  open checkh;
  fetch checkh into disp,hsdisp;
  if (checkh%FOUND) then
   if (disp > 0) then
    doid := disp;
   else
    if (hsdisp > 0) then
     doid := hsdisp;
    end if;
   end if;
  end if;
 end if;
 if (doid is NULL) then
   doid := 0;
 end if;
 return doid;
end GetBestDO;

-----------------------
procedure SetSpecialAnalysis(theAna IN integer, theHisto IN varchar2,  warn IN thresholds, alr IN thresholds) is
 cursor anaset(Xh HISTOGRAM.HID%TYPE) is select MASK from ANASETTINGS where ANA=theAna and HISTO=Xh;
 mk ANASETTINGS.MASK%TYPE;
begin
 open anaset(theHisto);
 fetch anaset into mk;
 if(anaset%NOTFOUND) then -- try with parent
    open anaset(REGEXP_REPLACE(theHisto,'/.*$','/1'));
    fetch anaset into mk;
    if(anaset%NOTFOUND) then
     raise_application_error(-20005,'Cannot find Analysis '||theAna||' for histogram'|| theHisto);
    else
     -- histogram has been added to set after analysis declaration: create new ANASETTINGS entry
     INSERT INTO ANASETTINGS(ANA,HISTO,WARNINGS,ALARMS) VALUES(theAna,theHisto,warn,alr);
    end if;
 else
  UPDATE ANASETTINGS SET WARNINGS=warn,ALARMS=alr WHERE ANA=theAna and HISTO=theHisto;
 end if;
 
end SetSpecialAnalysis;


-----------------------

procedure GetAlgoNpar(theAlg IN varchar2, Npar OUT integer, Ninp OUT integer) is
 cursor np is select NPARS,NINPUT from ALGORITHM where ALGNAME=theAlg;
begin
 open np;
 fetch np into Npar,Ninp;
 if (np%NOTFOUND) then
     Npar:=-1;
     Ninp:=-1;
     raise_application_error(-20006,'Cannot find Algorithm '||theAlg);
 end if;
end GetAlgoNpar;

-----------------------

procedure GetAlgoParname(theAlg IN varchar2, Ipar IN integer, name OUT varchar2) is
 cursor np is select ALGPARS from ALGORITHM where ALGNAME=theAlg;
 mypars parameters;
begin
 open np;
 fetch np into mypars;
 if (np%NOTFOUND) then
     name := 'Unknown';
     raise_application_error(-20006,'Cannot find Algorithm '||theAlg);
 end if;
 name := mypars(Ipar);
end GetAlgoParname;

-----------------------
function GetHistoAnalysis(theHistoSet IN int, anaids OUT intlist, ananames OUT analist) return number is
 cursor myhs is select NANALYSIS from HISTOGRAMSET where HSID=theHistoSet;
 cursor mya is select AID,ALGORITHM from ANALYSIS where HSET=theHistoSet order by AID;
 myna HISTOGRAMSET.NANALYSIS%TYPE;
 na int := 0;
 myaid ANALYSIS.AID%TYPE;
 myalg ANALYSIS.ALGORITHM%TYPE;
begin
 anaids := intlist();
 ananames := analist();
 open myhs;
 fetch myhs into myna;
 if (myhs%NOTFOUND) then
  raise_application_error(-20010,'Histogram Set '||theHistoSet||' not found');
 end if;
 open mya;
 LOOP	
  fetch mya into myaid,myalg;
  EXIT WHEN mya%NOTFOUND;
  na := na+1;
  anaids.EXTEND;
  anaids(na) := myaid;
  ananames.EXTEND;
  ananames(na) := myalg;
 end LOOP;
 if (na != myna) then
  raise_application_error(-20910,'Fatal DB inconsistency for Histogram Set '||theHistoSet||': contact DB manager');
 end if;
 return na;
end GetHistoAnalysis;

-----------------------
procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, Ipar IN integer, warn OUT float, alr OUT float) is
 cursor anaset is  select WARNINGS,ALARMS FROM ANASETTINGS WHERE ANA=theAna and HISTO=theHisto;
 myw thresholds;
 mya thresholds;
begin
 open anaset;
 fetch anaset into myw,mya;
 if(anaset%NOTFOUND) then
     warn := -999;
     alr := -999;
     raise_application_error(-20005,'Cannot find Analysis '||theAna||' for histogram'|| theHisto);
 end if;
 warn := myw(Ipar);
 alr := mya(Ipar);
EXCEPTION
 when others then
  raise_application_error(-20050,SQLERRM);
end GetAnaSettings;
-----------------------

procedure GetHCSettings(theHID IN HISTOGRAM.HID%TYPE, Ipar IN integer, value OUT float) is
 cursor hcp is  select HCPARS FROM HCREATOR WHERE HCID=theHID;
 myp thresholds;
begin
 open hcp;
 fetch hcp into myp;
 if(hcp%NOTFOUND) then
     value := -999;
     raise_application_error(-20005,'Cannot find Histogram Creator '||theHID);
 end if;
 value := myp(Ipar);
EXCEPTION
 when others then
  raise_application_error(-20050,SQLERRM);
end GetHCSettings;
-------------------------

procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE,
			           theSources sourceh, thePars IN thresholds := thresholds()) is
 cursor histo is select HSID from HISTOGRAMSET where HSTITLE=theTitle AND HSTASK='ANALYSIS' AND HSALGO=theAlg;
 myhsid HISTOGRAMSET.HSID%TYPE;
 cursor al(xAlg ALGORITHM.ALGNAME%TYPE) is  select ALGTYPE,NINPUT,NPARS,HCTYPE from ALGORITHM where ALGNAME=xAlg;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algonh ALGORITHM.NINPUT%TYPE;
 algonp ALGORITHM.NPARS%TYPE;
 command varchar2(500);
 value varchar2(500);
 myhctype HISTOGRAMSET.HSTYPE%TYPE := 'H1D';
 myhcid HCREATOR.HCID%TYPE;
begin
 SAVEPOINT beforeHCwrite;
  -- check algorithm
  open al(theAlg);
  fetch al into algotype,algonh,algonp,myhctype;
  if (al%NOTFOUND or algotype!='HCREATOR') then
    raise_application_error(-20002,'Algorithm '||TheAlg||' does not exist or is not a creator histogram');
  end if;
 -- CHECK IF THE HISTOGRAM IS ALREADY DEFINED
 open histo;
 fetch histo into myhsid;
 if (histo%NOTFOUND) then
  -- check that input histograms and parameters are correct
  if (algonh = 0) then
    if (theSet = 0) then
      raise_application_error(-20012,'Algorithm '||TheAlg||' requires an Histogram Set as input');
    end if;
  else
    if (theSources.COUNT != algonh) then
       raise_application_error(-20003,'Algorithm '||TheAlg||' requires '||algonh||' input histograms and you provided '||theSources.COUNT);
    end if;
  end if;
  if (algonp != thePars.COUNT) then
    raise_application_error(-20003,'Algorithm '||TheAlg||' requires '||algonp||' parameters');
  end if;
  -- create histogram entry
  INSERT INTO HISTOGRAMSET(HSID,NHS,HSTASK,HSALGO,HSTITLE,HSTYPE) VALUES(HistogramSet_ID.NEXTVAL,1,'ANALYSIS',theAlg,theTitle,myhctype);
  INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION,ISANALYSISHIST) VALUES(HistogramSet_ID.CURRVAL||'/1',HistogramSet_ID.CURRVAL,1,'',SYSTIMESTAMP,1);
  -- create HCREATOR entry  
  INSERT INTO HCREATOR(HCID,ALGORITHM) VALUES(HistogramSet_ID.CURRVAL||'/1',theAlg); 
  select HistogramSet_ID.CURRVAL into myhsid from ERGOSUM;
 end if; -- end of new histogram creation
 -- update sources and parameters
 myhcid := myhsid||'/1';
 UPDATE HCREATOR set HCPARS=thePars where HCID=myhcid;
 if (algonh = 0) then
  UPDATE HCREATOR set SOURCESET=theSet where HCID=myhcid;
 else
  command := 'UPDATE HCREATOR SET ALGORITHM='''||theAlg||'''';
  for i IN 1..algonh LOOP
   command := command||',SOURCEH'||i||'='''||theSources(i)||'''';
  end LOOP;
  command := command||' where HCID='''||myhcid||'''';
  EXECUTE IMMEDIATE command;
 end if;
exception
 when OTHERS then  
  ROLLBACK TO beforeHCwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareAnalysisHistogram;

-----------------------

procedure DeclarePageFolder(theFolder IN varchar2) is 
cursor mypf(xFolder PAGEFOLDER.PAGEFOLDERNAME%TYPE) is select PAGEFOLDERNAME from PAGEFOLDER where PAGEFOLDERNAME=xFolder;
theSon PAGEFOLDER.PAGEFOLDERNAME%TYPE;
theFather PAGEFOLDER.PAGEFOLDERNAME%TYPE;
conti int := 1;
begin
savepoint beforePFOwrite;
theSon := theFolder;
while ( conti = 1 ) LOOP -- there is a parent
 open mypf(theSon);
 fetch mypf into theFather;
 if (mypf%NOTFOUND) then
  insert into PAGEFOLDER(PAGEFOLDERNAME) VALUES(theSon);
  theFather := REGEXP_REPLACE(theSon,'^([^/]+)/.*$','\1');
  if (theFather != theSon) then
    update PAGEFOLDER set Parent=theFather where PAGEFOLDERNAME=theSon;
    theSon := theFather;
  else
   conti := 0;
  end if;
 else
  conti := 0;
 end if;
 close mypf;
end LOOP;
EXCEPTION
 when others then
  ROLLBACK TO beforePFOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclarePageFolder;


-----------------------
procedure DeclarePage(theName IN varchar2,theFolder IN varchar2,theDoc IN varchar2,hlist IN histotlist,
			Cx IN floattlist,Cy IN floattlist,Sx IN floattlist,Sy IN floattlist) is
 CURSOR cpg is select PAGENAME from PAGE where PAGENAME=theName;
 myPName  PAGE.PAGENAME%TYPE;
 Nin int;
 condition varchar2(500);
 CURSOR sh(Xpage PAGE.PAGENAME%TYPE,Xhisto SHOWHISTO.HISTO%TYPE,Xinstance SHOWHISTO.INSTANCE%TYPE) is 
	select PAGE from SHOWHISTO where PAGE=Xpage and HISTO=Xhisto and INSTANCE=Xinstance;
 inst int;
 instlist inttlist := inttlist();
begin
 savepoint beforeDPwrite;
 Nin := hlist.COUNT;
 if (Cx.COUNT != Nin or Cy.COUNT != Nin or Sx.COUNT != Nin or Sy.COUNT != Nin) then
   raise_application_error(-20003,'incomplete input variables');
 end if;
 DeclarePageFolder(theFolder);
 -- see if some histogram is present more than once and assign "instance" order parameter 
 instlist.EXTEND(Nin);
 for i IN 1..Nin LOOP
  inst :=0;
  for j IN 1..i LOOP
   if hlist(i) = hlist(j) then
     inst := inst +1;
   end if;
  end LOOP;
  instlist(i) := inst;
 end LOOP;

 open cpg;
 fetch cpg into myPName;
 if (cpg%NOTFOUND) then -- new page
  INSERT INTO PAGE(PAGENAME,FOLDER,NHISTO,PAGEDOC) VALUES(theName,theFolder,Nin,theDoc);
 else  -- update PAGE entry
  UPDATE PAGE SET  NHISTO=Nin,PAGEDOC=theDoc,FOLDER=theFolder where PAGENAME=theName;
  -- remove all showhisto entry related to this page that are no more in the definition
  condition := ' WHERE PAGE='''||theName||'''';
  for i IN 1..Nin LOOP
   condition := condition||' AND ( HISTO != '''||hlist(i)||''' OR INSTANCE != '||instlist(i)||' )';
  end LOOP; 
  --    first, remove DISPLAYOPTIONS that may have been set for the SHOWHISTO to be deleted, then remove SHOWHISTO entries
  EXECUTE IMMEDIATE
    'DELETE FROM DISPLAYOPTIONS WHERE DOID IN (SELECT SDISPLAY FROM SHOWHISTO '||condition||')';
  EXECUTE IMMEDIATE
    'DELETE FROM SHOWHISTO '||condition;
 end if;
 -- now create/update SHOWHISTO entries
 for i IN 1..Nin LOOP
   open sh(theName,hlist(i),instlist(i));
   fetch sh into myPName;
   if (sh%NOTFOUND) then
    INSERT INTO SHOWHISTO(PAGE,HISTO,INSTANCE,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y) 
	VALUES(theName,hlist(i),instlist(i),Cx(i),Cy(i),Sx(i),Sy(i)); 
   else
    UPDATE SHOWHISTO SET CENTER_X=Cx(i),CENTER_Y=Cy(i),SIZE_X=Sx(i),SIZE_Y=Sy(i) WHERE 
	PAGE=theName AND HISTO=hlist(i) AND INSTANCE=instlist(i);
   end if;
   close sh;
 end LOOP;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDPwrite;
  raise_application_error(-20050,SQLERRM);
end DeclarePage;
-----------------------

function GetPage(theName IN varchar2,theFolder OUT varchar2,theDoc OUT varchar2) return number is
 CURSOR cpg is select NHISTO,FOLDER,PAGEDOC from PAGE where PAGENAME=theName;
 myNh  PAGE.NHISTO%TYPE;
begin
 open cpg;
 fetch cpg into myNh,theFolder,theDoc;
 if (cpg%NOTFOUND) then 
   myNh := -1;
 end if;  
 return  myNh;
end GetPage;
-----------------------

PROCEDURE DeletePage(theName IN varchar2) is
begin
 delete from displayoptions where doid in (select sdisplay from showhisto where page=theName);
 delete from showhisto where page=theName;
 delete from page where PAGENAME=theName;
end DeletePage;
-----------------------

function GetHistogramData(theName IN varchar2, thePage IN varchar2, theInstance IN int , 
	theHid OUT varchar2,theHsid OUT int,
	theIhs OUT int,theNhs OUT int, theHstype OUT string, theHstitle OUT string, theSubtitle OUT string,
	theTask OUT string, theAlgo OUT string, theNanalysis OUT int,
	theDescr OUT string, theDoc OUT string, theIsanalysishist OUT int, theCreation OUT int,
        theObsolete OUT int, theDisplay OUT int, theHSDisplay OUT int, theSHDisplay OUT int,
	theDIMServiceName OUT varchar2) 
	return number is
cursor vh is select HID,HSID,IHS,NHS,HSTYPE,SUBTITLE,TITLE,ALGO,TASK FROM VIEWHISTOGRAM
       WHERE NAME=theName;
cursor myh(Xhid HISTOGRAM.HID%TYPE) is SELECT HS.NANALYSIS,HS.DESCR,HS.DOC,HS.HSDISPLAY,H.ISANALYSISHIST,
	(NEW_TIME(H.CREATION,'EST','GMT')-TO_DATE('01-JAN-1970','DD-MON-YYYY'))*(86400),
	(NEW_TIME(H.OBSOLETENESS,'EST','GMT')-TO_DATE('01-JAN-1970','DD-MON-YYYY'))*(86400),
	H.DISPLAY FROM HISTOGRAMSET HS,HISTOGRAM H WHERE H.HID=Xhid AND H.HSET=HS.HSID;
cursor mysh(Xhid HISTOGRAM.HID%TYPE) is SELECT SDISPLAY FROM SHOWHISTO 
	WHERE PAGE=thePage AND HISTO=Xhid AND INSTANCE=theInstance;
cursor mysn(Xhid HISTOGRAM.HID%TYPE) is SELECT SN from DIMSERVICENAME where PUBHISTO=Xhid;
begin
open vh;
fetch vh into theHid,theHsid,theIhs,theNhs,theHstype,theSubtitle,theHstitle,theAlgo,theTask;
if (vh%NOTFOUND) then
 raise_application_error(-20050,'Histogram '||theName||' not found');
 return 0;
end if;
open myh(theHid);
fetch myh into theNanalysis,theDescr,theDoc,theHSDisplay,theIsanalysishist,theCreation,theObsolete,theDisplay;
theDIMServiceName := 'Unknown';
open mysn(theHid);
fetch mysn into theDIMServiceName;
if (thePage != '_NONE_') then
 open mysh(theHid);
 fetch mysh into theSHDisplay;
 if (mysh%NOTFOUND) then
   theSHDisplay := 0;
 end if;   
end if;
return 1;
end GetHistogramData;
-----------------------

function DeleteHistogramSet(theSet IN HISTOGRAMSET.HSID%TYPE) return number is
begin
 savepoint beforeDHSdelete;
 delete from DISPLAYOPTIONS where DOID in (select HSDISPLAY from histogramset where hsid=theSet);
 delete from DISPLAYOPTIONS where DOID in (select DISPLAY from histogram where HSET=theSet);
 delete from DISPLAYOPTIONS where DOID in (select SDISPLAY from showhisto,histogram 
                       where showhisto.HISTO = histogram.HID and histogram.HSET=theSet);
 delete from histogramset where hsid=theSet;
 return SQL%ROWCOUNT; -- returns the number of deleted objects (0 or 1)
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHSdelete;
  raise_application_error(-20050,SQLERRM); 
end DeleteHistogramSet;
-----------------------

function DeleteHistogram(theHID IN HISTOGRAM.HID%TYPE) return number is
 cursor vh is SELECT HSID,NHS,IHS from VIEWHISTOGRAM WHERE HID=theHID;
 myhsid HISTOGRAMSET.HSID%TYPE;
 mynhs HISTOGRAMSET.NHS%TYPE;
 myihs HISTOGRAM.IHS%TYPE;
begin
 savepoint beforeDHdelete;
 open vh;
 fetch vh into myhsid,mynhs,myihs;
 if (vh%NOTFOUND) then
  return 0;
 else
  delete from DISPLAYOPTIONS where DOID in (select DISPLAY from histogram where HID=theHID);
  delete from DISPLAYOPTIONS where DOID in (select SDISPLAY from showhisto where showhisto.HISTO =theHID);
  if (mynhs = 1) then
   return DeleteHistogramSet(myhsid);
  else
   update HISTOGRAMSET set NHS=NHS-1 where HSID=myhsid;
   delete from histogram where HID=theHID;
   return SQL%ROWCOUNT; -- returns the number of deleted objects (0 or 1)
  end if;
 end if;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHdelete;
  raise_application_error(-20050,SQLERRM); 
end DeleteHistogram;

-----------------------
procedure mycommit(dbschema int := 0) is
 curschema ERGOSUM.version%TYPE;
 curapi ERGOSUM.apiversion%TYPE;
 begin
 SELECT version,apiversion into curschema,curapi  from ERGOSUM;
 if (dbschema != curschema) then
   raise_application_error(-20999,'
-----------------------------------------------------------------------------
You are using an HistDB API version incompatible with the current DB schema. 
No changes can be committed. Please update to OnlineHistDB '||curapi||' 
-----------------------------------------------------------------------------
');
 else 
  commit;
 end if;
end mycommit;

end OnlineHistDB; -- end of package
/




  


