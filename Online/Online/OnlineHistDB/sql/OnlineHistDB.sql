create or replace package OnlineHistDB AUTHID CURRENT_USER as
 TYPE sourceh is VARRAY(8) of HCREATOR.SOURCEH1%TYPE; 
 TYPE histotlist is TABLE OF HISTOGRAM.HID%TYPE;
 TYPE floattlist is TABLE OF real;
 TYPE inttlist is TABLE OF int;
 procedure DeclareSubSystem(subsys varchar2);	
 procedure DeclareTask (Name varchar2, ss1 varchar2:='NONE', ss2 varchar2:='NONE', ss3 varchar2:='NONE',
	KRunOnPhysics number :=1, KRunOnCalib number :=0, KRunOnEmpty number :=0, SFreq float := NULL, 
	Ref  varchar2:='NONE' );
 function RenameTask(oldName IN varchar2, newName IN varchar2) return number;
 function RenameAlgo(Task IN varchar2, oldName IN varchar2, newName IN varchar2) return number;
 procedure DeclareHistogram(tk IN varchar2,algo IN  varchar2 := '', title IN  varchar2:= '_BYSN_', mytype  VIEWHISTOGRAM.HSTYPE%TYPE := '');
 procedure DeclareHistByServiceName(ServiceName IN varchar2);
 function DeclareHistByAttributes(tk IN varchar2,algo IN  varchar2, title IN  varchar2,  thetype  VIEWHISTOGRAM.HSTYPE%TYPE ) 
   return varchar2 ; 
 function RenameHistogramSet(oldTask IN varchar2, oldAlgo IN varchar2, oldTitle IN varchar2,
                newTask IN varchar2, newAlgo IN varchar2, newTitle IN varchar2) return number;
 function RenameHistogram(oldIdentifier IN varchar2, 
                newTask IN varchar2, newAlgo IN varchar2, newTitle IN varchar2) return number;
 function SetDimServiceName(theHID IN  HISTOGRAM.HID%TYPE, theDSN IN DIMSERVICENAME.SN%TYPE) return number;
 procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2:=NULL, nin integer :=0, defVals thresholds);
 procedure DeclareCreatorAlgorithm(Name IN varchar2,Ninp IN number:=0,pars IN parameters,
			thetype IN varchar2 := 'H1D', doc IN varchar2:=NULL, thegetset in ALGORITHM.GETSET%TYPE := 0,
			defVals thresholds);

 function DeclareAnalysis(theSet IN HISTOGRAMSET.HSID%TYPE, Algo IN varchar2, warn IN thresholds, alr IN thresholds, 
	instance IN integer:=1, inputs IN thresholds:=thresholds()) return number;
 procedure SetSpecialAnalysis(theAna IN integer, theHisto IN varchar2,  warn IN thresholds, alr IN thresholds,
	inputs IN thresholds:=thresholds());


 function DeclareHistDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, theOptions IN dispopt) return number;
 function DeclareHistoSetDisplayOptions(theSet IN HISTOGRAMSET.HSID%TYPE, theOptions IN dispopt) return number;
 function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage varchar2, TheInstance IN int := 1, 
                        theOptions IN dispopt) return number;
 function GetDisplayOptions(theDOID IN int, theOptions OUT dispopt)  return number;
 procedure GET_DISPLAYOPTIONS(theDOID IN int
  ,LABEL_X OUT VARCHAR2,LABEL_Y OUT VARCHAR2,LABEL_Z OUT VARCHAR2,YMIN OUT FLOAT,YMAX OUT FLOAT,STATS OUT INT
  ,FILLSTYLE OUT INT,FILLCOLOR OUT INT,LINESTYLE OUT INT,LINECOLOR OUT INT,LINEWIDTH OUT INT,DRAWOPTS OUT VARCHAR2
  ,XMIN OUT FLOAT,XMAX OUT FLOAT,ZMIN OUT FLOAT,ZMAX OUT FLOAT,LOGX OUT INT,LOGY OUT INT
  ,LOGZ OUT INT,TIMAGE OUT VARCHAR2,REF OUT VARCHAR2,REFRESH OUT FLOAT,TIT_X_SIZE OUT FLOAT,TIT_X_OFFS OUT FLOAT
  ,TIT_Y_SIZE OUT FLOAT,TIT_Y_OFFS OUT FLOAT,TIT_Z_SIZE OUT FLOAT,TIT_Z_OFFS OUT FLOAT,LAB_X_SIZE OUT FLOAT,LAB_X_OFFS OUT FLOAT
  ,LAB_Y_SIZE OUT FLOAT,LAB_Y_OFFS OUT FLOAT,LAB_Z_SIZE OUT FLOAT,LAB_Z_OFFS OUT FLOAT,GRIDX OUT INT,GRIDY OUT INT
  ,THETA OUT FLOAT,PHI OUT FLOAT,CNTPLOT OUT VARCHAR2,DRAWPATTERN OUT VARCHAR2,STAT_X_SIZE OUT FLOAT,STAT_X_OFFS OUT FLOAT
  ,STAT_Y_SIZE OUT FLOAT,STAT_Y_OFFS OUT FLOAT,HTIT_X_SIZE OUT FLOAT,HTIT_X_OFFS OUT FLOAT,HTIT_Y_SIZE OUT FLOAT,HTIT_Y_OFFS OUT FLOAT
  ,NDIVX OUT INT,NDIVY OUT INT,MARKERSIZE OUT INT,MARKERCOLOR OUT INT,MARKERSTYLE OUT INT);

 function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN varchar2 := NULL,TheInstance IN int := 1) return number;
 function GetHID(theHistoName IN varchar2,Subindex OUT HISTOGRAM.IHS%TYPE) return number;
 function GetName(theHID IN varchar2) return varchar2;
 procedure GetAlgoNpar(theAlg IN varchar2, Npar OUT integer, Ninp OUT integer);
 procedure GetAlgoParname(theAlg IN varchar2, Ipar IN integer, name OUT varchar2, defVal OUT float);
 procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, Ipar IN integer, warn OUT float, alr OUT float);
 procedure GetAnaInput(theAna IN integer, theHisto IN varchar2, Ipar IN integer, input OUT float);
 procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, warn OUT thresholds, alr OUT thresholds,
	mask OUT int, inputs OUT thresholds);
 procedure GetHCSettings(theHID IN HISTOGRAM.HID%TYPE, Ipar IN integer, value OUT float);
 function GetHistoAnalysis(theHistoSet IN int, anaids OUT intlist, ananames OUT analist) return number;
 procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE := 0,
                                  theSources sourceh := NULL, thePars IN thresholds := thresholds(),theName OUT varchar2);
 procedure GetAnaHistDirections(theHCID IN varchar2, Alg OUT varchar2, Sset OUT int, Sources out hnalist, Pars out flolist);
 function PagenameSyntax(theName IN varchar2,Folder OUT varchar2) return varchar2;
 function DeclarePageFolder(theFolder IN varchar2) return varchar2;
 function DeclarePage(theFullName IN varchar2,theDoc IN varchar2,hlist IN histotlist,
			Cx IN floattlist,Cy IN floattlist,Sx IN floattlist,Sy IN floattlist,
                        theName OUT varchar2, theFolder OUT varchar2) return varchar2;
 function GetPage(thePage IN varchar2,theFolder OUT varchar2,theDoc OUT varchar2) return number; 
 function DeletePageFolder(thePageFolder IN varchar2) return number;
 function DeletePage(thePage IN varchar2) return number;
 function RenamePage(oldName IN varchar2, newName IN varchar2, newFolder OUT varchar2) return varchar2;
 function GetHistogramData(theName IN varchar2, thePage IN varchar2,  theInstance IN int,
	theHid OUT varchar2,theHsid OUT int,
	theIhs OUT int,theNhs OUT int, theHstype OUT varchar2, theHstitle OUT varchar2, theSubtitle OUT varchar2,
	theTask OUT varchar2, theAlgo OUT varchar2, theNanalysis OUT int,
	theDescr OUT varchar2, theDoc OUT varchar2, theIsanalysishist OUT int, theCreation OUT int,
        theObsolete OUT int, theDisplay out int, theHSDisplay out int, theSHDisplay out int,
	theDIMServiceName OUT varchar2, theLabels OUT parameters)
	return number;
 procedure DeclareBinLabels(theSet IN HISTOGRAMSET.HSID%TYPE, theHID IN HISTOGRAM.HID%TYPE, labels parameters);
 function DeleteHistogramSet(theSet IN HISTOGRAMSET.HSID%TYPE) return number;
 function DeleteHistogram(theHID IN HISTOGRAM.HID%TYPE) return number;
 procedure TaskCleanup;
 procedure GetTotalCounts(nHist OUT int, nPages OUT int, nPageFolders OUT int);
 procedure CheckSchema(dbschema IN int := 0); 
 procedure CheckSchema(dbschema IN int := 0, algListID OUT int);
 procedure CheckSchema(dbschema IN int := 0, algListID OUT int, writePerm OUT int);
 procedure mycommit(dbschema int := 0);
end OnlineHistDB;
/


----------------------------------------------
----------------------------------------------
create or replace  package body OnlineHistDB as


--------------------------
--PRIVATE FUNCTIONS:
-----------------------


function DeclareDisplayOptions(theOptions IN dispopt, theDOID IN DISPLAYOPTIONS.DOID%TYPE := NULL) return number is
 myid number := 0;
begin
 if (theDOID is NULL) then
  insert into DISPLAYOPTIONS(DOID, OPT) 
	   VALUES(DISPLAYOPTIONS_ID.NEXTVAL, theOptions);
  SELECT DISPLAYOPTIONS_ID.CURRVAL into myid  from ERGOSUM;
 else
  UPDATE DISPLAYOPTIONS SET  OPT=theOptions    WHERE DOID=theDOID;
  myid := theDOID;
 end if;
 return myid;
end DeclareDisplayOptions;

---------------------------------------------------------------------

procedure UpdateDimServiceName(theHID IN  HISTOGRAM.HID%TYPE, ServiceName IN DIMSERVICENAME.SN%TYPE) is
begin
 DELETE FROM DIMSERVICENAME WHERE PUBHISTO=theHID;
 INSERT INTO DIMSERVICENAME(SN,PUBHISTO) VALUES(ServiceName,theHID);
end UpdateDimServiceName;

-----------------------

function HistogramPages(theHID IN HISTOGRAM.HID%TYPE, PageNames OUT varchar2) return number is
 npages int :=0;
 cursor vsh is SELECT PAGE from SHOWHISTO where showhisto.HISTO =theHID;
 mypage varchar2(351);
begin
 PageNames := ' ';
 open vsh;
 LOOP	
  fetch vsh into mypage;
  EXIT WHEN vsh%NOTFOUND;
  npages := npages+1;
  if (LENGTH(PageNames) < (800-351)) then
    PageNames := PageNames || mypage || ' , ';
  end if;
 end LOOP;
 close vsh;
 return npages;
end HistogramPages;

--------------------------
--PUBLIC FUNCTIONS:
-----------------------

-----------------------
function GetHID(theHistoName IN varchar2,Subindex OUT HISTOGRAM.IHS%TYPE) return number is
 cursor myh
	is select HSID,IHS from VIEWHISTOGRAM where NAME=theHistoName;
 myhsid VIEWHISTOGRAM.HSID%TYPE;
 myihs VIEWHISTOGRAM.IHS%TYPE;
 out VIEWHISTOGRAM.HSID%TYPE := 0;
begin
 open myh;
 fetch myh into myhsid,myihs;
 if (myh%NOTFOUND) then
  Subindex := 0;
 else
  Subindex := myihs;
  out := myhsid;
 end if;
 close myh;
 return out;
end GetHID;
-----------------------

function GetName(theHID IN varchar2) return varchar2 is
 cursor myn is select NAME from HISTOGRAM where HID=theHID;
 out varchar2(130);
begin
 open myn;
 fetch myn into out;
 close myn;
 return out;
end GetName;

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
 close cs;
exception
 when OTHERS then 
  raise_application_error(-20050,SQLERRM);
 end DeclareSubSystem;

-----------------------
 procedure DeclareTask (Name varchar2, ss1 varchar2:='NONE', ss2 varchar2:='NONE', ss3 varchar2:='NONE',
	KRunOnPhysics number :=1, KRunOnCalib number :=0, KRunOnEmpty number :=0 , SFreq float := NULL, 
	Ref  varchar2:='NONE' ) is
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
 close ct;

 update TASK set RUNONPHYSICS=KRunOnPhysics,RUNONCALIB=KRunOnCalib,RUNONEMPTY=KRunOnEmpty,SaveFrequency=SFreq where TASKNAME=Name;
 if (ss1 != 'NONE') then -- 'NONE' means do nothing
  if (ss1 != 'NULL') then
    DeclareSubSystem(ss1);
    update TASK set SUBSYS1=ss1 where TASKNAME=Name;
  else
    update TASK set SUBSYS1=NULL where TASKNAME=Name;
  end if;
 end if;
 if (ss2 != 'NONE') then 
  if (ss2 != 'NULL') then
    DeclareSubSystem(ss2);
    update TASK set SUBSYS2=ss2 where TASKNAME=Name;
  else
    update TASK set SUBSYS2=NULL where TASKNAME=Name;
  end if;
 end if;
 if (ss3 != 'NONE') then 
  if (ss3 != 'NULL') then
    DeclareSubSystem(ss3);
    update TASK set SUBSYS3=ss3 where TASKNAME=Name;
  else
    update TASK set SUBSYS3=NULL where TASKNAME=Name;
  end if;
 end if;
 if (Ref != 'NONE') then 
  update TASK set REFERENCE=Ref  where TASKNAME=Name;
 end if;
exception
 when OTHERS then 
  ROLLBACK TO beforeDTwrite;
  raise_application_error(-20050,SQLERRM);
 end DeclareTask;
 
-----------------------
function RenameTask(oldName IN varchar2, newName IN varchar2) return number is
 oldRegTask varchar2(100) := '^'||oldName||'/';
 newRegTask varchar2(100) := newName||'/';
 begin
  savepoint beforeRNTwrite;
  DeclareTask(newName);
  UPDATE TASK set (RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SUBSYS1,SUBSYS2,SUBSYS3,SAVEFREQUENCY,REFERENCE) =
      (select RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SUBSYS1,SUBSYS2,SUBSYS3,SAVEFREQUENCY,REFERENCE from task where
       TASKNAME=oldName) where TASKNAME=newName;
  UPDATE HISTOGRAMSET set HSTASK=newName where HSTASK=oldName;
  UPDATE HISTOGRAM set NAME=REGEXP_REPLACE(NAME,oldRegTask,newRegTask) where HSET in 
      (SELECT HSID from HISTOGRAMSET where HSTASK=newName);
  delete from TASK where TASKNAME=oldName;
  if (SQL%ROWCOUNT = 0) then
   ROLLBACK TO beforeRNTwrite;
  end if;
  return SQL%ROWCOUNT;
 exception
 when OTHERS then 
  ROLLBACK TO beforeRNTwrite;
  raise_application_error(-20050,SQLERRM);
end RenameTask;
-----------------------

function RenameAlgo(Task IN varchar2, oldName IN varchar2, newName IN varchar2) return number is
 oldRegTask varchar2(100) := '/'||oldName||'/';
 newRegTask varchar2(100) := '/'||newName||'/';
 begin
  savepoint beforeRNAwrite;
  UPDATE HISTOGRAMSET set HSALGO=newName where HSALGO=oldName and HSTASK=Task;
  UPDATE HISTOGRAM set NAME=REGEXP_REPLACE(NAME,oldRegTask,newRegTask) where HSET in 
      (SELECT HSID from HISTOGRAMSET where HSALGO=newName and HSTASK=Task);
  if (SQL%ROWCOUNT = 0) then
   ROLLBACK TO beforeRNAwrite;
  end if;
  return SQL%ROWCOUNT;
 exception
 when OTHERS then 
  ROLLBACK TO beforeRNAwrite;
  raise_application_error(-20050,SQLERRM);
end RenameAlgo;
-----------------------


procedure DeclareHistogram(tk IN varchar2,algo IN  varchar2 := '', title IN  varchar2:= '_BYSN_', 
                           mytype  VIEWHISTOGRAM.HSTYPE%TYPE := '') is
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
 tk varchar2(200);
 algo varchar2(200);
 title varchar2(200);
 cursor mysn(Xsn DIMSERVICENAME.SN%TYPE) is select SN from DIMSERVICENAME where SN=Xsn;
 myfsn  DIMSERVICENAME.SN%TYPE;
 hid HISTOGRAM.HID%TYPE := '';

 begin
 open mysn(ServiceName);
 fetch mysn into myfsn;
 if mysn%NOTFOUND then -- if service name is known, do nothing
  -- decode service name
  mytype := REGEXP_SUBSTR(ServiceName,'^.{3}');
  if  (mytype = 'Mon') then -- support for MonObjects
   mytype := REGEXP_REPLACE(ServiceName,'^Mon([^/]*)/.+$','\1');
  end if;
  tk := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]*_([^/]+)_[^/]*/.+$','\1');
  if (tk = ServiceName) then -- try without farm node
   tk := REGEXP_REPLACE(ServiceName,'^[^/]*/([^/]+)/.+$','\1');

  end if;
  if (tk = ServiceName) then
     raise_application_error(-20051,'syntax error in Service Name '||ServiceName);
  end if;
  if (tk = 'Adder') then -- expect one more field for the taskname
     tk := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]*_Adder_[^/]*/([^/]+)/.+$','\1');
     if (tk = ServiceName) then
       raise_application_error(-20052,'syntax error in Service Name '||ServiceName);
     end if;
     algo := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]+/[^/]+/([^/]+)/.+$','\1');
     title := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]+/[^/]+/[^/]+/(.+)$','\1');
  else	
     algo := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]+/([^/]+)/.+$','\1'); 
     title := REGEXP_REPLACE(ServiceName,'^[^/]*/[^/]+/[^/]+/(.+)$','\1');
  end if;
  if (algo = ServiceName) then
    raise_application_error(-20053,'syntax error in Service Name '||ServiceName);
  end if;
  if (title = ServiceName) then
    raise_application_error(-20054,'syntax error in Service Name '||ServiceName);
  end if;
  hid := DeclareHistByAttributes(tk,algo,title,mytype);
  if (hid != '_none_') then 
   -- update service name (the dimension or the node or the task serial number could have changed)
   UpdateDimServiceName(hid,ServiceName);
  end if;
 end if;
 close mysn;
end  DeclareHistByServiceName;
-----------------------


function DeclareHistByAttributes(tk IN varchar2,algo IN  varchar2, title IN  varchar2, thetype  VIEWHISTOGRAM.HSTYPE%TYPE ) 
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
 mytype  VIEWHISTOGRAM.HSTYPE%TYPE := thetype;

 begin
 savepoint beforeDHwrite;
 if (INSTR(title,Set_Separator()) != 0) then
   hstitle :=  REGEXP_REPLACE(title,'^(.*)_\$.*$','\1');
   subtitle :=  REGEXP_REPLACE(title,'^.*_\$(.*)$','\1');
 else
   hstitle := title;
   subtitle := NULL;
 end if;
 -- backw. compat. hack
 if (mytype = 'HPD') then
  mytype := 'P1D';
 end if;
 -- support for MonObjects
 if (mytype = 'H1F') then
  mytype := 'H1D';
 end if;
 if (mytype = 'H2F') then
  mytype := 'H2D';
 end if;
 if (mytype = 'P1') then
  mytype := 'P1D';
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
    INSERT INTO HISTOGRAM(HID,NAME,HSET,IHS,SUBTITLE,CREATION) 
       VALUES(HistogramSet_ID.CURRVAL||'/1',tk||'/'||algo||'/'||hstitle||SUBTITSTRING(subtitle),
              HistogramSet_ID.CURRVAL,1,subtitle,SYSTIMESTAMP);
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
    INSERT INTO HISTOGRAM(HID,NAME,HSET,IHS,SUBTITLE,CREATION) 
           VALUES(myhid,tk||'/'||algo||'/'||hstitle||SUBTITSTRING(subtitle),
                  myhsid,myihs,subtitle,SYSTIMESTAMP);
    UPDATE HISTOGRAMSET SET NHS=mynhs where HSID=myhsid;
    -- if analysis exists for set, create it for this histogram, masked by default, with same parameters than first histogram in set
    if (mynana >0) then
      for aset in myans(myhsid||'/1') LOOP
       INSERT INTO ANASETTINGS(ANA,HISTO,MASK,WARNINGS,ALARMS) VALUES(aset.ANA,myhid,1,aset.WARNINGS,aset.ALARMS);
      end LOOP;     
    end if;
    close myhinset;
  end if;	
  close myhs;
 else -- histogram exists, only check dimension 
  if (mytype != oldtype) then
   UPDATE HISTOGRAMSET SET HSTYPE=mytype where HSID=myhsid;
  end if;
 end if;
 close myh;
 return myhid;
exception
 when OTHERS then 
  ROLLBACK TO beforeDHwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistByAttributes;

-----------------------
function RenameHistogramSet(oldTask IN varchar2, oldAlgo IN varchar2, oldTitle IN varchar2,
                newTask IN varchar2, newAlgo IN varchar2, newTitle IN varchar2) return number is
 cursor myhs is select HSID from HISTOGRAMSET where HSTASK = oldTask and HSALGO = oldAlgo and
					HSTITLE = oldTitle;
 cursor myh(Xset HISTOGRAM.HSET%TYPE) is select NAME,SUBTITLE from HISTOGRAM where HSET=Xset;
 myhsid HISTOGRAMSET.HSID%TYPE;
 myname HISTOGRAM.NAME%TYPE;
 mysub HISTOGRAM.SUBTITLE%TYPE;
begin
 open myhs;
 fetch myhs into myhsid;
 if myhs%NOTFOUND then
  return 0;
 else
  open myh(myhsid);
  fetch myh into myname,mysub;
  return RenameHistogram(myname, newTask, newAlgo, newTitle||SUBTITSTRING(mysub));
 end if;
exception
 when OTHERS then 
  return 0;
end RenameHistogramSet;
-----------------------

function RenameHistogram(oldIdentifier IN varchar2, 
                newTask IN varchar2, newAlgo IN varchar2, newTitle IN varchar2) return number is
 cursor myh(Xid HISTOGRAM.NAME%TYPE) is select HID,HSET,ISANALYSISHIST,HSTASK,HSALGO,HSTITLE from 
     HISTOGRAM h, HISTOGRAMSET hs where h.NAME=Xid and h.HSET = hs.HSID;
 cursor myh2(Xid HISTOGRAM.NAME%TYPE) is select HID from HISTOGRAM where NAME=Xid ;
 myhid HISTOGRAM.HID%TYPE;
 myhset HISTOGRAM.HSET%TYPE;
 myah HISTOGRAM.ISANALYSISHIST%TYPE;
 myhid2 HISTOGRAM.HID%TYPE;
 myhstask HISTOGRAMSET.HSTASK%TYPE;
 myhsalgo HISTOGRAMSET.HSALGO%TYPE;
 myhstitle HISTOGRAMSET.HSTITLE%TYPE;
 newhstitle HISTOGRAMSET.HSTITLE%TYPE;
 newsubtitle HISTOGRAM.SUBTITLE%TYPE;
 newname HISTOGRAM.NAME%TYPE;
begin
 savepoint beforeRHwrite;
 open myh(oldIdentifier);
 fetch myh into myhid,myhset,myah,myhstask,myhsalgo,myhstitle;
 if myh%NOTFOUND then
  return 0;
 else
  if (myah = 1) then
   raise_application_error(-20050,'you can''t change the name of an analysis histogram!');
  end if;
  close myh;
  newname := newTask||'/'||newAlgo||'/'||newTitle; 
  open myh2(newname);
  fetch myh2 into  myhid2;
  if myh2%FOUND then
   raise_application_error(-20050,'the new histogram name '||newname||' exists already!');
  end if;
  if (INSTR(newTitle,Set_Separator()) != 0) then
   newhstitle :=  REGEXP_REPLACE(newTitle,'^(.*)_\$.*$','\1');
   newsubtitle :=  REGEXP_REPLACE(newTitle,'^.*_\$(.*)$','\1');
  else
   newhstitle := newTitle;
   newsubtitle := NULL;
  end if;
  if ( newTask != myhstask or newAlgo != myhsalgo or newhstitle != myhstitle) then
   -- if histogramset has changed, update it together with all his HISTOGRAM children
   if( newTask != myhstask) then
    DeclareTask(newTask);
    UPDATE TASK set (RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SUBSYS1,SUBSYS2,SUBSYS3,SAVEFREQUENCY,REFERENCE) =
      (select RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SUBSYS1,SUBSYS2,SUBSYS3,SAVEFREQUENCY,REFERENCE from task where
       TASKNAME=myhstask) where TASKNAME=newTask;
   end if;
   UPDATE HISTOGRAMSET set HSTASK=newTask, HSALGO=newAlgo, HSTITLE=newhstitle where HSID = myhset;
   UPDATE HISTOGRAM set NAME=newTask||'/'||newAlgo||'/'||newhstitle||SUBTITSTRING(SUBTITLE) where HSET = myhset;
  end if;
   -- eventually update the HISTOGRAM
  UPDATE HISTOGRAM set NAME=newname, SUBTITLE = newsubtitle where HID = myhid;
 end if;
 return 1;
exception
 when OTHERS then 
  ROLLBACK TO beforeRHwrite;
  raise_application_error(-20050,SQLERRM);
end RenameHistogram;
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
 close myh;
 return out;
end SetDimServiceName;
-----------------------

procedure DeclareCheckAlgorithm(Name varchar2,pars parameters,doc varchar2:=NULL, nin integer := 0,
				defVals thresholds) is
-- pars must contain the names of output parameters, followed by input parameters
 cursor al is  select ALGNAME from ALGORITHM where ALGNAME=Name;	
 algo ALGORITHM.ALGNAME%TYPE;
 np integer := pars.COUNT - nin;
begin
 savepoint beforeCKAwrite;
 open al;
 fetch al into  algo;
 if al%NOTFOUND then
   insert into ALGORITHM(ALGNAME,ALGTYPE,NINPUT,NPARS,ALGPARS,PARDEFVAL) VALUES(Name,'CHECK',nin,np,pars,defVals);
 else
   update ALGORITHM set ALGTYPE='CHECK',NPARS=np,NINPUT=nin,ALGPARS=pars,PARDEFVAL=defVals where ALGNAME=Name;
 end if;
 close al;
 if (LENGTH(doc) > 0 ) then
    update ALGORITHM set ALGDOC=doc where ALGNAME=Name;
 end if;
EXCEPTION
 when OTHERS then 
  rollback to beforeCKAwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareCheckAlgorithm;

-----------------------
procedure DeclareCreatorAlgorithm(Name IN varchar2,Ninp IN number:=0,
	        pars IN parameters, thetype IN varchar2 := 'H1D', doc IN varchar2:=NULL, 
                thegetset in ALGORITHM.GETSET%TYPE := 0, defVals thresholds) is
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
   insert into ALGORITHM(ALGNAME,ALGTYPE,NINPUT,HCTYPE,GETSET) VALUES(Name,'HCREATOR',Ninp,thetype,thegetset);
 else
   update ALGORITHM set ALGTYPE='HCREATOR',NINPUT=Ninp,HCTYPE=thetype where ALGNAME=Name;
 end if;
 close al;
 if (nin > 0) then
    update ALGORITHM set NPARS=nin,ALGPARS=pars,PARDEFVAL=defVals where ALGNAME=Name;
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
                         instance IN integer :=1, inputs IN thresholds:=thresholds()) 
	return number is
 cursor hs is  select NHS from HISTOGRAMSET where HSID=theSet;
 mynh HISTOGRAMSET.NHS%TYPE :=0;
 cursor al is  select ALGTYPE,NPARS,NINPUT from ALGORITHM where ALGNAME=Algo;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algon ALGORITHM.NPARS%TYPE;
 algonin ALGORITHM.NINPUT%TYPE;
 cursor ana is select AID from ANALYSIS where HSET=theSet and ALGORITHM=Algo order by AID;
 myid ANALYSIS.AID%TYPE := 0;
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
 close hs;

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
   fetch al into algotype,algon,algonin;
   if (al%NOTFOUND or algotype!='CHECK') then
     raise_application_error(-20002,'Algorithm '||Algo||' does not exist or is not a check histogram');
   else 
    if (warn.COUNT != algon or alr.COUNT != algon or inputs.COUNT > algonin ) then
        raise_application_error(-20003,'Bad number of parameters for Algorithm '||Algo);
    else
     INSERT INTO ANALYSIS(AID,HSET,ALGORITHM) VALUES(Analysis_ID.NEXTVAL,theSet,Algo);
     UPDATE  HISTOGRAMSET set NANALYSIS=NANALYSIS+1 where HSID=theSet;
     for i IN 1..mynh LOOP
      hid := theSet||'/'||i;
       INSERT INTO ANASETTINGS(ANA,HISTO,WARNINGS,ALARMS,INPUTPARS) VALUES(Analysis_ID.CURRVAL,hid,warn,alr,inputs);
       SELECT Analysis_ID.CURRVAL into myid  from ERGOSUM;
     end LOOP;
    end if;
   end if;
   close al;
 else
  -- analysis already exists: update parameters
   UPDATE ANASETTINGS SET WARNINGS=warn,ALARMS=alr,INPUTPARS=inputs where ANA=myid and REGEXP_REPLACE(HISTO,'^(.*)/.*$','\1')=theSet;
 end if;
 close ana;
 return myid;
exception
 when OTHERS then 
  ROLLBACK TO beforeDAwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareAnalysis;

-----------------------
procedure SetSpecialAnalysis(theAna IN integer, theHisto IN varchar2,  warn IN thresholds, alr IN thresholds,
	inputs IN thresholds:=thresholds()) is
 cursor anaset(Xh HISTOGRAM.HID%TYPE) is select MASK from ANASETTINGS where ANA=theAna and HISTO=Xh;
 mk ANASETTINGS.MASK%TYPE;
begin
 savepoint beforeSSAwrite;
 open anaset(theHisto);
 fetch anaset into mk;
 if(anaset%NOTFOUND) then -- try with parent
    open anaset(REGEXP_REPLACE(theHisto,'/.*$','/1'));
    fetch anaset into mk;
    if(anaset%NOTFOUND) then
     raise_application_error(-20005,'Cannot find Analysis '||theAna||' for histogram'|| theHisto);
    else
     -- histogram has been added to set after analysis declaration: create new ANASETTINGS entry
     INSERT INTO ANASETTINGS(ANA,HISTO,WARNINGS,ALARMS,INPUTPARS) VALUES(theAna,theHisto,warn,alr,inputs);
    end if;
 else
  UPDATE ANASETTINGS SET WARNINGS=warn,ALARMS=alr,INPUTPARS=inputs WHERE ANA=theAna and HISTO=theHisto;
 end if;
 close anaset;
EXCEPTION
 when others then
  ROLLBACK TO beforeSSAwrite;
  raise_application_error(-20050,SQLERRM);
end SetSpecialAnalysis;

-----------------------

function DeclareHistoSetDisplayOptions(theSet IN HISTOGRAMSET.HSID%TYPE,theOptions IN dispopt) return number is
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
 mydoid := DeclareDisplayOptions(theOptions ,mySetDisp);
 if (mySetDisp is NULL) then
  update HISTOGRAMSET set HSDISPLAY=mydoid where HSID=theSet;
 end if;
 close checko;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHSDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistoSetDisplayOptions;

-----------------------

function DeclareHistDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, theOptions IN dispopt) return number is
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
 mydoid := DeclareDisplayOptions(theOptions ,myDisp);
 if (myDisp is NULL) then
   update HISTOGRAM set DISPLAY=mydoid where HID=theHID;
 end if; 
 close checko;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistDisplayOptions;


-----------------------

function DeclareHistoPageDisplayOptions(theHID IN HISTOGRAM.HID%TYPE, thePage varchar2, TheInstance IN int := 1, 
                        theOptions IN dispopt) return number is
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
 mydoid := DeclareDisplayOptions(theOptions,myDisp);
 if (myDisp is NULL) then
   update SHOWHISTO set SDISPLAY=mydoid where HISTO=theHID and PAGE=thePage and INSTANCE=TheInstance;
 end if;
 close checko;
 return mydoid;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHPDOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareHistoPageDisplayOptions;

-----------------------
function GetDisplayOptions(theDOID IN int, theOptions OUT dispopt)   return number is
cursor mydo is select OPT from DISPLAYOPTIONS where DOID=theDOID;
out int;
begin
 open mydo;
 fetch mydo into theOptions;
 if (mydo%NOTFOUND) then
  out :=  0;
 else
  out := 1;
 end if;
 close mydo;
 return out;
end GetDisplayOptions;
-----------------------
-- this is needed for the web interface since PHP4 doesn't support objects
-- produced automatically by  autodispopt.pl

procedure GET_DISPLAYOPTIONS(theDOID IN int
,LABEL_X OUT VARCHAR2,LABEL_Y OUT VARCHAR2,LABEL_Z OUT VARCHAR2,YMIN OUT FLOAT,YMAX OUT FLOAT,STATS OUT INT
,FILLSTYLE OUT INT,FILLCOLOR OUT INT,LINESTYLE OUT INT,LINECOLOR OUT INT,LINEWIDTH OUT INT,DRAWOPTS OUT VARCHAR2
,XMIN OUT FLOAT,XMAX OUT FLOAT,ZMIN OUT FLOAT,ZMAX OUT FLOAT,LOGX OUT INT,LOGY OUT INT
,LOGZ OUT INT,TIMAGE OUT VARCHAR2,REF OUT VARCHAR2,REFRESH OUT FLOAT,TIT_X_SIZE OUT FLOAT,TIT_X_OFFS OUT FLOAT
,TIT_Y_SIZE OUT FLOAT,TIT_Y_OFFS OUT FLOAT,TIT_Z_SIZE OUT FLOAT,TIT_Z_OFFS OUT FLOAT,LAB_X_SIZE OUT FLOAT,LAB_X_OFFS OUT FLOAT
,LAB_Y_SIZE OUT FLOAT,LAB_Y_OFFS OUT FLOAT,LAB_Z_SIZE OUT FLOAT,LAB_Z_OFFS OUT FLOAT,GRIDX OUT INT,GRIDY OUT INT
,THETA OUT FLOAT,PHI OUT FLOAT,CNTPLOT OUT VARCHAR2,DRAWPATTERN OUT VARCHAR2,STAT_X_SIZE OUT FLOAT,STAT_X_OFFS OUT FLOAT
,STAT_Y_SIZE OUT FLOAT,STAT_Y_OFFS OUT FLOAT,HTIT_X_SIZE OUT FLOAT,HTIT_X_OFFS OUT FLOAT,HTIT_Y_SIZE OUT FLOAT,HTIT_Y_OFFS OUT FLOAT
,NDIVX OUT INT,NDIVY OUT INT,MARKERSIZE OUT INT,MARKERCOLOR OUT INT,MARKERSTYLE OUT INT) is
 mydo dispopt; 
begin
 SELECT OPT INTO mydo FROM DISPLAYOPTIONS WHERE DOID = theDOID;
 LABEL_X := mydo.LABEL_X;
 LABEL_Y := mydo.LABEL_Y;
 LABEL_Z := mydo.LABEL_Z;
 YMIN := mydo.YMIN;
 YMAX := mydo.YMAX;
 STATS := mydo.STATS;
 FILLSTYLE := mydo.FILLSTYLE;
 FILLCOLOR := mydo.FILLCOLOR;
 LINESTYLE := mydo.LINESTYLE;
 LINECOLOR := mydo.LINECOLOR;
 LINEWIDTH := mydo.LINEWIDTH;
 DRAWOPTS := mydo.DRAWOPTS;
 XMIN := mydo.XMIN;
 XMAX := mydo.XMAX;
 ZMIN := mydo.ZMIN;
 ZMAX := mydo.ZMAX;
 LOGX := mydo.LOGX;
 LOGY := mydo.LOGY;
 LOGZ := mydo.LOGZ;
 TIMAGE := mydo.TIMAGE;
 REF := mydo.REF;
 REFRESH := mydo.REFRESH;
 TIT_X_SIZE := mydo.TIT_X_SIZE;
 TIT_X_OFFS := mydo.TIT_X_OFFS;
 TIT_Y_SIZE := mydo.TIT_Y_SIZE;
 TIT_Y_OFFS := mydo.TIT_Y_OFFS;
 TIT_Z_SIZE := mydo.TIT_Z_SIZE;
 TIT_Z_OFFS := mydo.TIT_Z_OFFS;
 LAB_X_SIZE := mydo.LAB_X_SIZE;
 LAB_X_OFFS := mydo.LAB_X_OFFS;
 LAB_Y_SIZE := mydo.LAB_Y_SIZE;
 LAB_Y_OFFS := mydo.LAB_Y_OFFS;
 LAB_Z_SIZE := mydo.LAB_Z_SIZE;
 LAB_Z_OFFS := mydo.LAB_Z_OFFS;
 GRIDX := mydo.GRIDX;
 GRIDY := mydo.GRIDY;
 THETA := mydo.THETA;
 PHI := mydo.PHI;
 CNTPLOT := mydo.CNTPLOT;
 DRAWPATTERN := mydo.DRAWPATTERN;
 STAT_X_SIZE := mydo.STAT_X_SIZE;
 STAT_X_OFFS := mydo.STAT_X_OFFS;
 STAT_Y_SIZE := mydo.STAT_Y_SIZE;
 STAT_Y_OFFS := mydo.STAT_Y_OFFS;
 HTIT_X_SIZE := mydo.HTIT_X_SIZE;
 HTIT_X_OFFS := mydo.HTIT_X_OFFS;
 HTIT_Y_SIZE := mydo.HTIT_Y_SIZE;
 HTIT_Y_OFFS := mydo.HTIT_Y_OFFS;
 NDIVX := mydo.NDIVX;
 NDIVY := mydo.NDIVY;
 MARKERSIZE := mydo.MARKERSIZE;
 MARKERCOLOR := mydo.MARKERCOLOR;
 MARKERSTYLE := mydo.MARKERSTYLE;
end GET_DISPLAYOPTIONS;


-------------------------------


function GetBestDO(theHID IN HISTOGRAM.HID%TYPE, thePage IN varchar2 := NULL,
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
  close checkp;
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
  close checkh;
 end if;
 if (doid is NULL) then
   doid := 0;
 end if;
 return doid;
end GetBestDO;


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
 close np;
end GetAlgoNpar;

-----------------------

procedure GetAlgoParname(theAlg IN varchar2, Ipar IN integer, name OUT varchar2, defVal OUT float) is
 cursor np is select ALGPARS,PARDEFVAL from ALGORITHM where ALGNAME=theAlg;
 mypars parameters;
 mydefval thresholds;
begin
 name := 'Unknown';
 defVal := NULL;
 open np;
 fetch np into mypars,mydefval;
 if (np%NOTFOUND) then
     raise_application_error(-20006,'Cannot find Algorithm '||theAlg);
 end if;
 if (Ipar>0 and Ipar <= mypars.COUNT) then
  name := mypars(Ipar);
  if (mydefval is not NULL) then
   defVal := mydefval(Ipar);
  end if;
 end if;
 close np;
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
 close myhs;
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
 close mya;
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
 close anaset;
EXCEPTION
 when others then
  raise_application_error(-20050,SQLERRM);
end GetAnaSettings;
-----------------------
procedure GetAnaInput(theAna IN integer, theHisto IN varchar2, Ipar IN integer, input OUT float) is
 cursor anaset is  select INPUTPARS FROM ANASETTINGS WHERE ANA=theAna and HISTO=theHisto;
 myinp thresholds;
begin
 open anaset;
 fetch anaset into myinp;
 if(anaset%NOTFOUND) then
     input := -999;
     raise_application_error(-20005,'Cannot find Analysis '||theAna||' for histogram'|| theHisto);
 end if;
 input := myinp(Ipar);
 close anaset;
EXCEPTION
 when others then
  raise_application_error(-20050,SQLERRM);
end GetAnaInput;

------------------------------------------------------------------------------------------------------------

procedure GetAnaSettings(theAna IN integer, theHisto IN varchar2, warn OUT thresholds, alr OUT thresholds,
	mask OUT int , inputs OUT thresholds) is
 cursor anaset is  select WARNINGS,ALARMS,MASK,INPUTPARS FROM ANASETTINGS WHERE ANA=theAna and HISTO=theHisto;
begin
 open anaset;
 fetch anaset into warn,alr,mask,inputs;
 if(anaset%NOTFOUND) then
     warn := thresholds();
     alr := thresholds();
     mask := 0;
     raise_application_error(-20005,'Cannot find Analysis '||theAna||' for histogram'|| theHisto);
 end if;
 close anaset;
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
 close hcp;
EXCEPTION
 when others then
  raise_application_error(-20050,SQLERRM);
end GetHCSettings;
-------------------------

procedure DeclareAnalysisHistogram(theAlg IN varchar2,theTitle IN varchar2,theSet IN HISTOGRAMSET.HSID%TYPE := 0,
			           theSources sourceh, thePars IN thresholds := thresholds(),
	                           theName OUT varchar2) is
 myhid HISTOGRAM.HID%TYPE;
 myhcid HCREATOR.HCID%TYPE;
 cursor hc(xHID HISTOGRAM.HID%TYPE) is select HCID from HCREATOR where HCID=xHID;
 cursor al(xAlg ALGORITHM.ALGNAME%TYPE) is  select ALGTYPE,NINPUT,NPARS,HCTYPE,GETSET from ALGORITHM where ALGNAME=xAlg;	
 algotype ALGORITHM.ALGTYPE%TYPE;
 algonh ALGORITHM.NINPUT%TYPE;
 algonp ALGORITHM.NPARS%TYPE;
 algogs ALGORITHM.GETSET%TYPE;
 command varchar2(500);
 value varchar2(500);
 myhctype HISTOGRAMSET.HSTYPE%TYPE := 'H1D';
 anatk HISTOGRAMSET.HSTASK%TYPE := 'ANALYSIS';
begin
 SAVEPOINT beforeHCwrite;
 -- check algorithm
 open al(theAlg);
 fetch al into algotype,algonh,algonp,myhctype,algogs;
 if (al%NOTFOUND or algotype!='HCREATOR') then
   raise_application_error(-20002,'Algorithm '||TheAlg||' does not exist or is not a creator histogram');
 end if;
 close al;
 -- check that input histograms and parameters are correct
 if (algonh = 0) then
   if (theSet = 0) then
     raise_application_error(-20012,'Algorithm '||TheAlg||' requires an Histogram Set as input');
   end if;
 else
   if (theSources.COUNT > algonh) then
      raise_application_error(-20003,'Algorithm '||TheAlg||' requires '||algonh||' input histograms and you provided '||theSources.COUNT);
   end if;
 end if;
 if ( thePars.COUNT > algonp ) then
   raise_application_error(-20003,'Algorithm '||TheAlg||' requires '||algonp||' parameters and you provided '||thePars.COUNT);
 end if;
 if (myhctype = 'SAM') then -- take type and task from source
  if ( theSources.COUNT > 0) then
    select HSTYPE,TASK into myhctype,anatk from VIEWHISTOGRAM where HID=theSources(1);
  else
    select HSTYPE,TASK into myhctype,anatk from VIEWHISTOGRAM where HID=theSet||'/1';
  end if;
 else  -- take task from source
  if (theSources.COUNT >0) then
    select TASK into anatk from VIEWHISTOGRAM where HID=theSources(1);
  else
    select TASK into anatk from VIEWHISTOGRAM where HID=theSet||'/1';
  end if;
 end if;
 if ( REGEXP_SUBSTR(anatk,'_ANALYSIS') is NULL) then
  anatk :=  anatk || '_ANALYSIS';
 end if;

 myhid := DeclareHistByAttributes( tk => anatk, algo => theAlg, title => theTitle, thetype => myhctype);
 update histogram set ISANALYSISHIST=1 where HID=myhid;
 open hc(myhid);
 fetch hc into myhcid;
 if (hc%NOTFOUND) then
  -- create HCREATOR entry  
  INSERT INTO HCREATOR(HCID,ALGORITHM) VALUES(myhid,theAlg); 
 end if;

 UPDATE HCREATOR set HCPARS=thePars where HCID=myhid;
 if ( algogs = 1 and theSet > 0 ) then
  UPDATE HCREATOR set SOURCESET=theSet where HCID=myhid;
 end if;
 if (algonh > 0) then
  command := 'UPDATE HCREATOR SET ALGORITHM='''||theAlg||'''';
  for i IN 1..algonh LOOP
   if (theSources.COUNT >= i) then
    command := command||',SOURCEH'||i||'='''||theSources(i)||'''';
   end if;
  end LOOP;
  command := command||' where HCID='''||myhid||'''';
  EXECUTE IMMEDIATE command;
 end if;
 close hc;
 theName := anatk||'/'||theAlg||'/'||theTitle;
exception
 when OTHERS then  
  ROLLBACK TO beforeHCwrite;
  raise_application_error(-20050,SQLERRM);
end DeclareAnalysisHistogram;

-----------------------
procedure GetAnaHistDirections(theHCID IN varchar2, Alg OUT varchar2, Sset OUT int, Sources out hnalist, Pars out flolist) is
 cursor myHC is select ALGORITHM,SOURCEH1,SOURCEH2,SOURCEH3,SOURCEH4,SOURCEH5,SOURCEH6,SOURCEH7,SOURCEH8,SOURCESET,HCPARS
    from HCREATOR where HCID=theHCID;
 cursor myhh(Xset int) is select NAME from VIEWHISTOGRAM where HSID=Xset;
 Npar int;
 Ninp int;
 myalg HCREATOR.ALGORITHM%TYPE;
 sh sourceh := sourceh();
 mypars thresholds ;
 i int;
begin
 sh.EXTEND(8);
 open myHC;
 fetch myHC into myalg,sh(1),sh(2),sh(3),sh(4),sh(5),sh(6),sh(7),sh(8),Sset,mypars;
 if(myHC%NOTFOUND) then
  Alg := '';
  Sset := 0;
  Sources := hnalist();
  Pars := flolist();
   raise_application_error(-20005,'Histogram '||theHCID||' not an analysis histogram');
 else
  Alg := myalg;
  GetAlgoNpar(myalg, Npar, Ninp);
  Sources := hnalist();
  if (Ninp = 0) then -- get sources from set
   if (Sset is not NULL) then
    select count(*) into Ninp from VIEWHISTOGRAM where HSID=Sset;
    Sources.EXTEND(Ninp);
    i := 1;
    for hset in myhh(Sset) LOOP
     if (i <= 100) then 
       Sources(i) := hset.NAME ;
       i := i+1;
     end if;
    end LOOP; 
   end if;
  else
   for i IN 1..Ninp LOOP
    if( sh(i) is not NULL ) then
     Sources.EXTEND(1);
     Sources(i) := GetName(sh(i));
    end if;
   end LOOP; 
  end if;

  Pars := flolist();
  Pars.EXTEND(Npar);
  for i IN 1..Npar LOOP
   Pars(i) := mypars(i);
  end LOOP; 
 end if;
 close myHC;
end GetAnaHistDirections;


-----------------------
function PagenameSyntax(theName IN varchar2,Folder OUT varchar2) return varchar2 is
outName varchar2(350) := theName;
begin
 Folder := '';
 if (outName != '/') then 
  outName := REGEXP_REPLACE(outName,'^ *([^ ]+) *$','\1'); -- remove spaces
  outName := REGEXP_REPLACE(outName,'/+$',''); -- remove trailing slashes
  outName := REGEXP_REPLACE(outName,'/+','/'); -- remove multiple slashes
  if (SUBSTR(outName,1,1) != '/') then -- check the root
   outName := '/'||outName;
  end if;
  -- extract folder name
  Folder := REGEXP_REPLACE(outName,'^(/.+)/[^/]+$','\1');
  if (Folder = outName) then
   Folder := '/';
  end if;
 end if;
 return outName;
end PagenameSyntax;

-----------------------

function DeclarePageFolder(theFolder IN varchar2) return varchar2 is 
cursor mypf(xFolder PAGEFOLDER.PAGEFOLDERNAME%TYPE) is select PAGEFOLDERNAME from PAGEFOLDER where PAGEFOLDERNAME=xFolder;
theSon PAGEFOLDER.PAGEFOLDERNAME%TYPE;
theFather PAGEFOLDER.PAGEFOLDERNAME%TYPE;
outFolder PAGEFOLDER.PAGEFOLDERNAME%TYPE;
conti int := 1;
begin
 savepoint beforePFOwrite;
 outFolder := PagenameSyntax(theFolder,theFather);
 theSon := outFolder;
 while ( conti = 1 ) LOOP -- there is a folder to check
  open mypf(theSon);
  fetch mypf into theFather;
  if (mypf%NOTFOUND) then
   insert into PAGEFOLDER(PAGEFOLDERNAME) VALUES(theSon);
   theFather := REGEXP_REPLACE(theSon,'^(/.+)/[^/]+$','\1');
   if (theFather != theSon) then -- there is a parent
     update PAGEFOLDER set Parent=theFather where PAGEFOLDERNAME=theSon;
     theSon := theFather; 
   else
    if (theSon != '/') then
     update PAGEFOLDER set Parent='/' where PAGEFOLDERNAME=theSon;
    end if;
    conti := 0;
   end if;
  else
   conti := 0; -- pagefolder already in DB
  end if;
  close mypf;
 end LOOP;
 return outFolder;
EXCEPTION
 when others then
  ROLLBACK TO beforePFOwrite;
  raise_application_error(-20050,SQLERRM);
end DeclarePageFolder;


-----------------------
function DeclarePage(theFullName IN varchar2,theDoc IN varchar2,hlist IN histotlist,
		      Cx IN floattlist,Cy IN floattlist,Sx IN floattlist,Sy IN floattlist,
                      theName OUT varchar2, theFolder OUT varchar2) return varchar2 is
 CURSOR cpg(Xpage PAGE.PAGENAME%TYPE) is select PAGENAME from PAGE where PAGENAME=Xpage;
 myPName  PAGE.PAGENAME%TYPE;
 outName varchar2(350);
 Nin int;
 condition varchar2(10000);
 CURSOR sh(Xpage PAGE.PAGENAME%TYPE,Xhisto SHOWHISTO.HISTO%TYPE,Xinstance SHOWHISTO.INSTANCE%TYPE) is 
	select PAGE from SHOWHISTO where PAGE=Xpage and HISTO=Xhisto and INSTANCE=Xinstance;
 inst int;
 instlist inttlist := inttlist();
 xFolder PAGEFOLDER.PAGEFOLDERNAME%TYPE;
begin
 savepoint beforeDPwrite;
 outName := PagenameSyntax(theFullName,xFolder);
 xFolder := DeclarePageFolder(xFolder);
 theFolder := xFolder;
 theName := outName;
 Nin := hlist.COUNT;
 if (Cx.COUNT != Nin or Cy.COUNT != Nin or Sx.COUNT != Nin or Sy.COUNT != Nin) then
   raise_application_error(-20003,'incomplete input variables');
 end if;
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

 open cpg(theName);
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
 close cpg;
 -- now create/update SHOWHISTO entries
 for i IN 1..Nin LOOP
   open sh(theName,hlist(i),instlist(i));
   fetch sh into myPName;
   if (sh%NOTFOUND) then
   -- DBMS_OUTPUT.PUT_LINE('inserting '||theName||' '||theFolder||' '||hlist(i)||' '||instlist(i));
    INSERT INTO SHOWHISTO(PAGE,PAGEFOLDER,HISTO,INSTANCE,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y) 
	VALUES(theName,theFolder,hlist(i),instlist(i),Cx(i),Cy(i),Sx(i),Sy(i)); 
   else
    UPDATE SHOWHISTO SET CENTER_X=Cx(i),CENTER_Y=Cy(i),SIZE_X=Sx(i),SIZE_Y=Sy(i) WHERE 
	PAGE=theName AND HISTO=hlist(i) AND INSTANCE=instlist(i);
   end if;
   close sh;
 end LOOP;
 theName := REGEXP_REPLACE(outName,'^.*/([^/]+)$','\1');
 return outName;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDPwrite;
  raise_application_error(-20050,SQLERRM);
end DeclarePage;
-----------------------

function GetPage(thePage IN varchar2,theFolder OUT varchar2,theDoc OUT varchar2) return number is
 CURSOR cpg is select NHISTO,FOLDER,PAGEDOC from PAGE where PAGENAME=thePage;
 myNh  PAGE.NHISTO%TYPE;
begin
 open cpg;
 fetch cpg into myNh,theFolder,theDoc;
 if (cpg%NOTFOUND) then 
   myNh := -1;
 end if;  
 close cpg;
 return  myNh;
end GetPage;
-----------------------

function DeletePageFolder(thePageFolder IN varchar2) return number is
 cursor sons is select PAGEFOLDERNAME from PAGEFOLDER where PARENT=thePageFolder;
 cursor pages is select PAGENAME from PAGE where FOLDER=thePageFolder;
 myFold PAGEFOLDER.PAGEFOLDERNAME%TYPE;
begin
 savepoint beforePAGEFOLDERdelete;
 open sons;
 fetch sons into myFold;
 if (sons%NOTFOUND) then
   for pset in pages LOOP
     if ( DeletePage(pset.PAGENAME) = 0) then
       raise_application_error(-20050,'Cannot delete Page '||
	  pset.PAGENAME);
     end if;
   end LOOP; 
  delete from PAGEFOLDER where PAGEFOLDERNAME=thePageFolder;
  return SQL%ROWCOUNT; -- returns the number of deleted objects (0 or 1)	
 else 
  raise_application_error(-20050,'Pagefolder '||thePageFolder||' contains folder '||myFold||' and cannot be deleted');
 end if;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforePAGEFOLDERdelete;
  raise_application_error(-20050,SQLERRM);  
end DeletePageFolder;
-----------------------

function DeletePage(thePage IN varchar2) return number is
 myFolder PAGEFOLDER.PAGEFOLDERNAME%TYPE;
 myPage PAGE.PAGENAME%TYPE;
begin
 savepoint beforePAGEdelete;
 myPage := PagenameSyntax(thePage, myFolder);
 delete from displayoptions where doid in (select sdisplay from showhisto where PAGE=myPage);
 delete from showhisto where PAGE=myPage;
 delete from page where PAGENAME=myPage;
 return SQL%ROWCOUNT; -- returns the number of deleted objects (0 or 1)	
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforePAGEdelete;
  raise_application_error(-20050,SQLERRM);  
end DeletePage;
-----------------------
function RenamePage(oldName IN varchar2, newName IN varchar2, newFolder OUT varchar2) return varchar2 is
 cursor oldp is select  NHISTO,PAGEDOC from PAGE where PAGENAME=oldName;
 nh int;
 doc PAGE.PAGEDOC%TYPE;
 fold PAGE.FOLDER%TYPE;
 out PAGE.PAGENAME%TYPE;
begin
 savepoint beforePAGErename;
-- check old page is there
 open oldp;
 fetch oldp into nh,doc;
 if (oldp%FOUND) then
  out := PagenameSyntax(newName, fold);
  newFolder := fold;
  fold := DeclarePageFolder(fold);
  INSERT INTO PAGE(PAGENAME,FOLDER,NHISTO,PAGEDOC) VALUES(out,fold,nh,doc);
  UPDATE SHOWHISTO SET PAGE=out,PAGEFOLDER=fold where PAGE=oldName;
  DELETE FROM PAGE where PAGENAME=oldName;
 else
  out := oldName;
  newFolder :='';
 end if;
 return out;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforePAGErename;
  raise_application_error(-20050,SQLERRM); 
end RenamePage;


-----------------------

function GetHistogramData(theName IN varchar2, thePage IN varchar2, theInstance IN int , 
	theHid OUT varchar2,theHsid OUT int,
	theIhs OUT int,theNhs OUT int, theHstype OUT varchar2, theHstitle OUT varchar2, theSubtitle OUT varchar2,
	theTask OUT varchar2, theAlgo OUT varchar2, theNanalysis OUT int,
	theDescr OUT varchar2, theDoc OUT varchar2, theIsanalysishist OUT int, theCreation OUT int,
        theObsolete OUT int, theDisplay OUT int, theHSDisplay OUT int, theSHDisplay OUT int,
	theDIMServiceName OUT varchar2, theLabels OUT parameters) 
	return number is
cursor myh(XName HISTOGRAM.NAME%TYPE) is SELECT HS.HSID,HS.NHS,HS.HSTYPE,HS.HSTITLE,HS.HSALGO,HS.HSTASK,
        HS.NANALYSIS,HS.DESCR,HS.DOC,HS.HSDISPLAY,
        H.HID,H.IHS,H.SUBTITLE,H.ISANALYSISHIST,
	(NEW_TIME(H.CREATION,'EST','GMT')-TO_DATE('01-JAN-1970','DD-MON-YYYY'))*(86400),
	(NEW_TIME(H.OBSOLETENESS,'EST','GMT')-TO_DATE('01-JAN-1970','DD-MON-YYYY'))*(86400),
	H.DISPLAY,H.BINLABELS FROM HISTOGRAMSET HS,HISTOGRAM H WHERE H.NAME=XName AND H.HSET=HS.HSID;
cursor mysh(Xhid HISTOGRAM.HID%TYPE) is SELECT SDISPLAY FROM SHOWHISTO 
	WHERE PAGE=thePage AND HISTO=Xhid AND INSTANCE=theInstance;
cursor mysn(Xhid HISTOGRAM.HID%TYPE) is SELECT SN from DIMSERVICENAME where PUBHISTO=Xhid;
out number := 1;
begin
 open myh(theName);
 fetch myh into theHsid,theNhs,theHstype,theHstitle,theAlgo,theTask,theNanalysis,theDescr,
   theDoc,theHSDisplay,theHid,theIhs,theSubtitle,theIsanalysishist,theCreation,theObsolete,theDisplay,
   theLabels;
 if (myh%NOTFOUND) then
  close myh;
  raise_application_error(-20050,'Histogram '||theName||' not found');
  out := 0;
 end if;
 theDIMServiceName := 'Unknown';
 close myh;
 open mysn(theHid);
 fetch mysn into theDIMServiceName;
 close mysn;
 if (thePage != '_NONE_') then
  open mysh(theHid);
  fetch mysh into theSHDisplay;
  if (mysh%NOTFOUND) then
    theSHDisplay := 0;
  end if;
  close mysh;   
 end if;
 return out;
end GetHistogramData;
-----------------------
procedure DeclareBinLabels(theSet IN HISTOGRAMSET.HSID%TYPE, theHID IN HISTOGRAM.HID%TYPE, labels parameters) is
begin
if (labels.COUNT >0) then
 if (theHID is not NULL) then
  UPDATE HISTOGRAM SET BINLABELS=labels where HID=theHID;
 else
  UPDATE HISTOGRAM SET BINLABELS=labels where HSET=theSet;
 end if;

end if;
end DeclareBinLabels;

-----------------------
function DeleteHistogramSet(theSet IN HISTOGRAMSET.HSID%TYPE) return number is
 cursor vh is SELECT HID from HISTOGRAM where HSET=theSet;
 myhid HISTOGRAM.HID%TYPE;
 pagenames varchar2(800);
 np int;
 nd int;
begin
 savepoint beforeDHSdelete;
 open vh;
 LOOP	
  fetch vh into myhid;
  EXIT WHEN vh%NOTFOUND;
  np := HistogramPages(myhid, PageNames); -- check if histogram is on a page
  if (np > 0) then
   raise_application_error(-20005,'Histogram '||GetName(myhid)||' is on pages --'||PageNames||'-- and cannot be removed');
  end if;
 end LOOP;
 close vh;
 delete from DISPLAYOPTIONS where DOID in (select HSDISPLAY from histogramset where hsid=theSet);
 delete from DISPLAYOPTIONS where DOID in (select DISPLAY from histogram where HSET=theSet);
 delete from histogramset where hsid=theSet;
 nd := SQL%ROWCOUNT;
 -- in case an analysis hist was deleted in cascade
 delete from histogram where isanalysishist=1 and hid not in (select HCID from hcreator);
 return nd; -- returns the number of deleted objects (0 or 1)
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHSdelete;
  raise_application_error(-20050,SQLERRM); 
end DeleteHistogramSet;
-----------------------

function DeleteHistogram(theHID IN HISTOGRAM.HID%TYPE) return number is
 cursor vh is SELECT HSID,NHS,IHS from VIEWHISTOGRAM WHERE HID=theHID;
 np int;
 pagenames varchar2(800);
 myhsid HISTOGRAMSET.HSID%TYPE;
 mynhs HISTOGRAMSET.NHS%TYPE;
 myihs HISTOGRAM.IHS%TYPE;
 nd int;
begin
 savepoint beforeDHdelete;
 open vh;
 fetch vh into myhsid,mynhs,myihs;
 if (vh%NOTFOUND) then
  return 0;
 else
  np := HistogramPages(theHID, PageNames); -- check if histogram is on a page
  if (np > 0) then
   raise_application_error(-20005,'Histogram '||GetName(theHID)||' is on pages --'||PageNames||'-- and cannot be removed');
  end if;
  delete from DISPLAYOPTIONS where DOID in (select DISPLAY from histogram where HID=theHID);
  if (mynhs = 1) then
   return DeleteHistogramSet(myhsid);
  else
   update HISTOGRAMSET set NHS=NHS-1 where HSID=myhsid;
   delete from histogram where HID=theHID;
   nd := SQL%ROWCOUNT;
   -- in case an analysis hist was deleted in cascade
   delete from histogram where isanalysishist=1 and hid not in (select HCID from hcreator);
   return nd; -- returns the number of deleted objects (0 or 1)
  end if;
 end if;
 close vh;
EXCEPTION
 when OTHERS then
  ROLLBACK TO beforeDHdelete;
  raise_application_error(-20050,SQLERRM); 
end DeleteHistogram;

-----------------------------------

procedure TaskCleanup is
 n int;
begin
 for tset in (select TASKNAME from TASK) LOOP
  select COUNT(*) into n from HISTOGRAMSET where HSTASK = tset.TASKNAME;
  if (n = 0) then
    delete from task where TASKNAME = tset.TASKNAME;
  end if;
 end LOOP;
end TaskCleanup;

-----------------------------------

procedure GetTotalCounts(nHist OUT int, nPages OUT int, nPageFolders OUT int) is
begin
 select COUNT(*) into nHist from HISTOGRAM;
 select COUNT(*) into nPages from PAGE;
 select COUNT(*) into nPageFolders from PAGEFOLDER;
end GetTotalCounts;

-----------------------------------
procedure CheckSchema(dbschema IN int := 0) is -- for backward compatibility
 algListID int;
begin
 CheckSchema(dbschema, algListID);
end CheckSchema;

-----------------------
procedure CheckSchema(dbschema IN int := 0, algListID OUT int) is
wP int;
begin
 CheckSchema(dbschema, algListID, wP);
end CheckSchema;

----------------------
procedure CheckSchema(dbschema IN int := 0, algListID OUT int, writePerm OUT int) is
 curschema ERGOSUM.version%TYPE;
 curapi ERGOSUM.apiversion%TYPE;
 curalglist ERGOSUM.ALGLIST%TYPE;
 insuf_privs exception;
 pragma exception_init(insuf_privs, -1031);
begin
 SELECT version,apiversion,alglist into curschema,curapi,curalglist  from ERGOSUM;
 algListID := curalglist;
 if (dbschema < curschema) then
   rollback;
   raise_application_error(-20999,'
-----------------------------------------------------------------------------
You are using an HistDB API version incompatible with the current DB schema. 
No changes can be committed. Please update to OnlineHistDB '||curapi||' 
-----------------------------------------------------------------------------
');
 else 
  if (dbschema > curschema) then
   rollback;
   raise_application_error(-20999,'
-----------------------------------------------------------------------------
You are using an HistDB API version too recent for the current DB schema. 
No changes can be committed. Please downgrade to OnlineHistDB version '||curapi||' 
-----------------------------------------------------------------------------
');
  end if;
 end if;
 -- check write permission
 EXECUTE IMMEDIATE 'update ERGOSUM set ALGLIST=ALGLIST';
 writePerm := 1;
 commit;
EXCEPTION
when insuf_privs then 
 writePerm := 0;
when OTHERS then
 writePerm := 0;
 RAISE;
end CheckSchema;

-----------------------
procedure mycommit(dbschema int := 0) is
alglist ERGOSUM.ALGLIST%TYPE;
begin
 CheckSchema(dbschema,alglist);
 commit;
end mycommit;

end OnlineHistDB; -- end of package
/




  


