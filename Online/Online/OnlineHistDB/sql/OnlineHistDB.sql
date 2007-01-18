create or replace package OnlineHistDB as
 TYPE sourceh is VARRAY(8) of HCREATOR.SOURCEH1%TYPE; 
 TYPE histolist is TABLE OF HISTOGRAM.HID%TYPE;
 TYPE floatlist is TABLE OF real;
 procedure DeclareSubSystem(subsys varchar2);	
 procedure DeclareTask (Name varchar2, ss1 varchar2:='NONE', ss2 varchar2:='NONE', ss3 varchar2:='NONE',
	KRunOnPhysics number :=1, KRunOnCalib number :=0, KRunOnEmpty number :=0);
 procedure DeclareHistByServiceName(ServiceName IN varchar2);
 procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2:=NULL);
 procedure DeclareCreatorAlgorithm(Name varchar2,Ninp number:=0,doc varchar2:=NULL);

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
 function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage PAGE.PAGENAME%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL,reset IN int :=0) return number;
 function GetDisplayOptions(theDOID IN int, KLABEL_X OUT varchar2 ,KLABEL_Y OUT varchar2 ,KLABEL_Z OUT varchar2 ,
			KYMIN OUT float ,KYMAX OUT float ,KSTATS OUT smallint , KFILLSTYLE OUT smallint ,
		        KFILLCOLOR  OUT smallint , KLINESTYLE OUT smallint ,KLINECOLOR OUT smallint ,KLINEWIDTH OUT smallint ,
			KDRAWOPTS OUT varchar2 )   return number;	
 function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN PAGE.PAGENAME%TYPE := NULL) return number;

 function GetHID(theHistoName IN varchar2,Subindex OUT HISTOGRAM.IHS%TYPE) return number;
 procedure GetAlgoNpar(theAlg IN varchar2, Npar OUT integer);
 procedure GetAlgoParname(theAlg IN varchar2, Ipar IN integer, name OUT varchar2);
 procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, Ipar IN integer, warn OUT float, alr OUT float);
 function GetHistoAnalysis(theHistoSet IN int, anaids OUT intlist, ananames OUT analist) return number;
 procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE := 0,theSources sourceh := NULL);
 procedure DeclarePageFolder(theFolder IN varchar2);
 procedure DeclarePage(theName IN varchar2,theFolder IN varchar2,theDoc IN varchar2,hlist IN histolist,
			Cx IN floatlist,Cy IN floatlist,Sx IN floatlist,Sy IN floatlist);
 function GetPage(theName IN varchar2,theFolder OUT varchar2,theDoc OUT varchar2) return number; 
 PROCEDURE DeletePage(theName IN varchar2);
 function GetHistogramData(theName IN varchar2, thePage IN varchar2, theHid OUT varchar2,theHsid OUT int,
	theIhs OUT int,theNhs OUT int, theHstype OUT string, theHstitle OUT string, theSubtitle OUT string,
	theTask OUT string, theAlgo OUT string, theNanalysis OUT int,
	theDescr OUT string, theDoc OUT string, theIsanalysishist OUT int, theCreation OUT int,
        theObsolete OUT int, theDisplay out int, theHSDisplay out int, theSHDisplay out int,
	theDIMServiceName OUT varchar2)
	return number;

 procedure mycommit;
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


function GetHID(theHistoSetName IN varchar2,theSubtitle IN varchar2 := NULL,Subindex OUT HISTOGRAM.IHS%TYPE) return number is
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
end GetHID;

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
 procedure DeclareHistByServiceName(ServiceName varchar2) is
 dimension int; -- still missing a convention for profile histograms
 newdim  VIEWHISTOGRAM.HSTYPE%TYPE;
 tk TASK.TASKNAME%TYPE;
 algo HISTOGRAMSET.HSALGO%TYPE;
 title varchar2(300);
 hstitle HISTOGRAMSET.HSTITLE%TYPE;
 subtitle  HISTOGRAM.SUBTITLE%TYPE;
 cursor myh(Xsubtit HISTOGRAM.SUBTITLE%TYPE, Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask TASK.TASKNAME%TYPE) is 
	select HID,HSID,HSTYPE from VIEWHISTOGRAM where (SUBTITLE=Xsubtit or (Xsubtit is NULL and SUBTITLE is NULL)) 
	AND TITLE=Xtit AND ALGO=Xalg and TASK=Xtask;
 myhstype VIEWHISTOGRAM.HSTYPE%TYPE;
 cursor myhs(Xtit HISTOGRAMSET.HSTITLE%TYPE, Xalg HISTOGRAMSET.HSALGO%TYPE, Xtask HISTOGRAMSET.HSTASK%TYPE) is 
	select HSID,NHS,NANALYSIS from HISTOGRAMSET where HSTITLE=Xtit AND HSALGO=Xalg and HSTASK=Xtask;
 myhid HISTOGRAM.HID%TYPE;
 myhsid HISTOGRAMSET.HSID%TYPE;
 mynhs HISTOGRAMSET.NHS%TYPE;
 mynana HISTOGRAMSET.NANALYSIS%TYPE;
 cursor mysn(Xsn DIMSERVICENAME.SN%TYPE) is select SN from DIMSERVICENAME where SN=Xsn;
 myfsn  DIMSERVICENAME.SN%TYPE;
 cursor myans(Xhisto HISTOGRAM.HID%TYPE) is select ANA,WARNINGS,ALARMS from ANASETTINGS where HISTO=Xhisto;
 myana ANASETTINGS.ANA%TYPE;
 mywn thresholds;
 myal thresholds;

 begin
 open mysn(ServiceName);
 fetch mysn into myfsn;
 if mysn%NOTFOUND then -- if service name is known, do nothing
  savepoint beforeDHwrite;
  -- decode service name
  dimension := REGEXP_REPLACE(ServiceName,'^H(.)D/.*$','\1');
  tk := REGEXP_REPLACE(ServiceName,'^H.D/.*_(.*)_.*/.*/.*$','\1');
  algo := REGEXP_REPLACE(ServiceName,'^H.D/.*/(.*)/.*$','\1');
  title := REGEXP_REPLACE(ServiceName,'^H.D/.*/.*/(.*)$','\1');
  if (INSTR(title,'_$$_') != 0) then
    hstitle :=  REGEXP_REPLACE(title,'^(.*)_\$\$_.*$','\1');
    subtitle :=  REGEXP_REPLACE(title,'^.*_\$\$_(.*)$','\1');
  else
    hstitle := title;
    subtitle := NULL;
  end if;

  -- see if histogram exists
   --  DBMS_OUTPUT.PUT_LINE('histo e'' ___'||tk||'/'||algo||'/'||hstitle||'_$$_'||subtitle||'___');
  open myh(subtitle,hstitle,algo,tk);
  fetch myh into myhid,myhsid,myhstype;
  if myh%NOTFOUND then -- see if histogram set exists
   open myhs(hstitle,algo,tk);
   fetch myhs into myhsid,mynhs,mynana;
   if myhs%NOTFOUND then -- check that task exists
      DeclareTask(tk);
      -- create histogram set and histogram
      INSERT INTO HISTOGRAMSET(HSID,NHS,HSTASK,HSALGO,HSTITLE,HSTYPE) 
	VALUES(HistogramSet_ID.NEXTVAL,1,tk,algo,hstitle,dimension||'D');
      INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION) 
	VALUES(HistogramSet_ID.CURRVAL||'/1',HistogramSet_ID.CURRVAL,1,subtitle,SYSTIMESTAMP);
      SELECT HistogramSet_ID.CURRVAL||'/1' INTO myhid from ERGOSUM;
   else -- histogram set exists, create histogram object and update number in set
    mynhs := mynhs+1;
    myhid := myhsid||'/'||mynhs;
    INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION) VALUES(myhid,myhsid,mynhs,subtitle,SYSTIMESTAMP);
    UPDATE HISTOGRAMSET SET NHS=mynhs where HSID=myhsid;
    -- if analysis exist for set, create it for this histogram, masked by default, with same parameters than first histogram in set
    if (mynana >0) then
      for aset in myans(myhsid||'/1') LOOP
       INSERT INTO ANASETTINGS(ANA,HISTO,MASK,WARNINGS,ALARMS) VALUES(aset.ANA,myhid,1,aset.WARNINGS,aset.ALARMS);
      end LOOP;     
    end if;
   end if;	
  else -- histogram exists, only check dimension 
   newdim:=dimension||'D'; -- convention for profile histogram ?? 
   if (newdim != myhstype) then
    UPDATE HISTOGRAMSET SET HSTYPE=newdim where HSID=myhsid;
   end if;
  end if;
  -- update service name (the dimension or the node or the task serial number could have changed)
  DELETE FROM DIMSERVICENAME WHERE PUBHISTO=myhid;
  INSERT INTO DIMSERVICENAME VALUES(ServiceName,myhid);
 end if;
exception
 when OTHERS then 
  ROLLBACK TO beforeDHwrite;
  raise_application_error(-20050,SQLERRM);
 end DeclareHistByServiceName;

-----------------------
procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2) is
 cursor al is  select ALGNAME from ALGORITHM where ALGNAME=Name;	
 algo ALGORITHM.ALGNAME%TYPE;
 nin integer := pars.COUNT;
begin
 open al;
 fetch al into  algo;
 if al%NOTFOUND then
   insert into ALGORITHM(ALGNAME,ALGTYPE,NINPUT,ALGPARS) VALUES(Name,'CHECK',nin,pars);
 else
   update ALGORITHM set ALGTYPE='CHECK',NINPUT=nin,ALGPARS=pars where ALGNAME=Name;
 end if;
 if (LENGTH(doc) > 0 ) then
    update ALGORITHM set ALGDOC=doc where ALGNAME=Name;
 end if;
end DeclareCheckAlgorithm;

-----------------------
procedure DeclareCreatorAlgorithm(Name varchar2,Ninp number,doc varchar2) is
 cursor al is  select ALGNAME from ALGORITHM where ALGNAME=Name;	
 algo ALGORITHM.ALGNAME%TYPE;
begin
 open al;
 fetch al into  algo;
 if al%NOTFOUND then
   insert into ALGORITHM(ALGNAME,ALGTYPE,NINPUT) VALUES(Name,'HCREATOR',Ninp);
 else
   update ALGORITHM set ALGTYPE='HCREATOR',NINPUT=Ninp where ALGNAME=Name;
 end if;
 if (LENGTH(doc) > 0 ) then
    update ALGORITHM set ALGDOC=doc where ALGNAME=Name;
 end if;
end DeclareCreatorAlgorithm;

-----------------------

function DeclareAnalysis(theSet IN HISTOGRAMSET.HSID%TYPE, Algo IN varchar2, warn IN thresholds, alr IN thresholds, instance IN integer :=1) 
	return number is
 cursor hs is  select NHS from HISTOGRAMSET where HSID=theSet;
 mynh HISTOGRAMSET.NHS%TYPE :=0;
 cursor al is  select ALGTYPE,NINPUT from ALGORITHM where ALGNAME=Algo;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algon ALGORITHM.NINPUT%TYPE;
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
  DBMS_OUTPUT.PUT_LINE(SQLERRM);
  return 0;
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

function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage PAGE.PAGENAME%TYPE, 
                        KLABEL_X varchar2 := NULL,KLABEL_Y varchar2 := NULL,KLABEL_Z varchar2 := NULL,
			KYMIN IN float := NULL,KYMAX IN float := NULL,KSTATS IN smallint := NULL, KFILLSTYLE IN smallint := NULL,
		        KFILLCOLOR  IN smallint := NULL, KLINESTYLE IN smallint := NULL,KLINECOLOR IN smallint := NULL,KLINEWIDTH IN smallint := NULL,
			KDRAWOPTS IN varchar2 := NULL, reset IN int :=0) return number is
 cursor checko is select SDISPLAY from SHOWHISTO where HISTO=theHID and PAGE=thePage; 
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
   update SHOWHISTO set SDISPLAY=mydoid where HISTO=theHID and PAGE=thePage;
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

function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN PAGE.PAGENAME%TYPE := NULL) return number is
 cursor checkp is select SDISPLAY from SHOWHISTO where HISTO=theHID and PAGE=thePage; 
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

procedure GetAlgoNpar(theAlg IN varchar2, Npar OUT integer) is
 cursor np is select NINPUT from ALGORITHM where ALGNAME=theAlg;
begin
 open np;
 fetch np into Npar;
 if (np%NOTFOUND) then
     Npar:=-1;
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
 cursor mya is select AID,ALGORITHM from ANALYSIS where HSET=theHistoSet order by AID;
 na int := 0;
 myaid ANALYSIS.AID%TYPE;
 myalg ANALYSIS.ALGORITHM%TYPE;
begin
 anaids := intlist();
 ananames := analist();
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
end GetAnaSettings;
-----------------------

procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE,theSources sourceh) is
 cursor histo is select HSID from HISTOGRAMSET where HSTITLE=theTitle AND HSTASK='ANALYSIS' AND HSALGO=theAlg;
 myhsid HISTOGRAMSET.HSID%TYPE;
 cursor al is  select ALGTYPE,NINPUT from ALGORITHM where ALGNAME=TheAlg;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algon ALGORITHM.NINPUT%TYPE;
 command varchar2(500);
 value varchar2(500);
begin
 SAVEPOINT beforeHCwrite;
 -- CHECK IF THE HISTOGRAM IS ALREADY DEFINED
 open histo;
 fetch histo into myhsid;
 if (histo%NOTFOUND) then
  open al;
  fetch al into algotype,algon;
  -- check algorithm
  if (al%NOTFOUND or algotype!='HCREATOR') then
    raise_application_error(-20002,'Algorithm '||TheAlg||' does not exist or is not a creator histogram');
  end if;
  -- check input parameters are correct
  if (algon = 0) then
    if (theSet = 0) then
      raise_application_error(-20012,'Algorithm '||TheAlg||' requires an Histogram Set as input');
    end if;
  else
    if (theSources.COUNT != algon) then
       raise_application_error(-20003,'Algorithm '||TheAlg||' requires '||algon||' input histograms');
    end if;
  end if;
  -- create histogram entry
  INSERT INTO HISTOGRAMSET(HSID,NHS,HSTASK,HSALGO,HSTITLE,HSTYPE) VALUES(HistogramSet_ID.NEXTVAL,1,'ANALYSIS',theAlg,theTitle,'1D');
  INSERT INTO HISTOGRAM(HID,HSET,IHS,SUBTITLE,CREATION,ISANALYSISHIST) VALUES(HistogramSet_ID.CURRVAL||'/1',HistogramSet_ID.CURRVAL,1,'',SYSTIMESTAMP,1);
  -- create HCREATOR entry
  if (algon = 0) then
   INSERT INTO HCREATOR(HCID,ALGORITHM,SOURCESET) VALUES(HistogramSet_ID.CURRVAL||'/1',theAlg,theSet); 
  else
   command := 'INSERT INTO HCREATOR(HCID,ALGORITHM';
   value := 'VALUES(HistogramSet_ID.CURRVAL||''/1'','''||theAlg||'''';
   for i IN 1..algon LOOP
    command := command||',SOURCEH'||i;
    value := value||','''||theSources(i)||'''';
   end LOOP;
   EXECUTE IMMEDIATE command||') '||value||')';
  end if;
 end if;
 exception
 when OTHERS then  
  ROLLBACK TO beforeHCwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareAnalysisHistogram;

-----------------------

procedure DeclarePageFolder(theFolder IN varchar2) is
cursor mypf is select PAGEFOLDERNAME from PAGEFOLDER where PAGEFOLDERNAME=theFolder;
mypfname PAGEFOLDER.PAGEFOLDERNAME%TYPE;
begin
open mypf;
fetch mypf into mypfname;
if (mypf%NOTFOUND) then
 insert into PAGEFOLDER(PAGEFOLDERNAME) VALUES(theFolder);
end if;
end DeclarePageFolder;


-----------------------
procedure DeclarePage(theName IN varchar2,theFolder IN varchar2,theDoc IN varchar2,hlist IN histolist,
			Cx IN floatlist,Cy IN floatlist,Sx IN floatlist,Sy IN floatlist) is
 CURSOR cpg is select PAGENAME from PAGE where PAGENAME=theName;
 myPName  PAGE.PAGENAME%TYPE;
 Nin int;
 condition varchar2(500);
 CURSOR sh(Xpage PAGE.PAGENAME%TYPE,Xhisto SHOWHISTO.HISTO%TYPE) is 
	select PAGE from SHOWHISTO where PAGE=Xpage and HISTO=Xhisto;
begin
 savepoint beforeDPwrite;
 Nin := hlist.COUNT;
 if (Nin<1 or Cx.COUNT != Nin or Cy.COUNT != Nin or Sx.COUNT != Nin or Sy.COUNT != Nin) then
   raise_application_error(-20003,'incomplete input variables');
 end if;
 DeclarePageFolder(theFolder);
 open cpg;
 fetch cpg into myPName;
 if (cpg%NOTFOUND) then -- new page
  INSERT INTO PAGE(PAGENAME,FOLDER,NHISTO,PAGEDOC) VALUES(theName,theFolder,Nin,theDoc);
 else  -- update PAGE entry
  UPDATE PAGE SET  NHISTO=Nin,PAGEDOC=theDoc,FOLDER=theFolder where PAGENAME=theName;
  -- remove all showhisto entry related to this page that are no more in the definition
  condition := ' WHERE PAGE='''||theName||'''';
  for i IN 1..Nin LOOP
   condition := condition||' AND HISTO != '''||hlist(i)||'''';
  end LOOP; 
  --    first, remove DISPLAYOPTIONS that may have been set for the SHOWHISTO to be deleted, then remove SHOWHISTO entries
  EXECUTE IMMEDIATE
    'DELETE FROM DISPLAYOPTIONS WHERE DOID IN (SELECT SDISPLAY FROM SHOWHISTO '||condition||')';
  EXECUTE IMMEDIATE
    'DELETE FROM SHOWHISTO '||condition;
 end if;
 -- now create/update SHOWHISTO entries
 for i IN 1..Nin LOOP
   open sh(theName,hlist(i));
   fetch sh into myPName;
   if (sh%NOTFOUND) then
    INSERT INTO SHOWHISTO(PAGE,HISTO,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y) VALUES(theName,hlist(i),Cx(i),Cy(i),Sx(i),Sy(i)); 
   else
    UPDATE SHOWHISTO SET CENTER_X=Cx(i),CENTER_Y=Cy(i),SIZE_X=Sx(i),SIZE_Y=Sy(i) WHERE PAGE=theName AND HISTO=hlist(i);
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

function GetHistogramData(theName IN varchar2, thePage IN varchar2, theHid OUT varchar2,theHsid OUT int,
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
cursor mysh(Xhid HISTOGRAM.HID%TYPE) is SELECT SDISPLAY FROM SHOWHISTO WHERE PAGE=thePage AND HISTO=Xhid;
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
procedure mycommit is
 begin
 commit;
end mycommit;

end OnlineHistDB; -- end of package
/




  


