
drop table DIMSERVICENAME;
drop table SHOWHISTO;
drop table PAGE;
drop table PAGEFOLDER;
drop table ANASETTINGS;
drop table ANALYSIS;
drop table HCREATOR;
drop table HISTOGRAM;
drop table HISTOGRAMSET;
drop table DISPLAYOPTIONS;
drop table ALGORITHM;
drop table TASK;
drop table SUBSYSTEM;
drop table ERGOSUM;

drop sequence HistogramSet_ID;
drop sequence Analysis_ID;
drop sequence Displayoptions_ID;

CREATE OR REPLACE TYPE thresholds as TABLE OF FLOAT;
/
CREATE OR REPLACE TYPE parameters as TABLE OF VARCHAR2(15);
/
CREATE OR REPLACE TYPE intlist as VARRAY(20) OF int;
/
CREATE OR REPLACE TYPE analist is VARRAY(20) OF varchar2(30);
/
CREATE OR REPLACE TYPE flolist is VARRAY(20) OF float;
/
CREATE OR REPLACE TYPE hnalist is VARRAY(100) OF varchar2(130);
/
CREATE OR REPLACE TYPE DISPOPT AS OBJECT (
  LABEL_X varchar2(50),
  LABEL_Y varchar2(50),
  LABEL_Z varchar2(50),
  YMIN float,
  YMAX float,
  STATS smallint,
  FILLSTYLE smallint,
  FILLCOLOR smallint,
  LINESTYLE smallint,
  LINECOLOR smallint,
  LINEWIDTH smallint,
  DRAWOPTS varchar2(50),
  XMIN         FLOAT,
  XMAX         FLOAT,
  ZMIN         FLOAT,
  ZMAX         FLOAT,
  LOGX         NUMBER(1),
  LOGY         NUMBER(1),
  LOGZ         NUMBER(1),
  TIMAGE       VARCHAR2(5),
  REF          VARCHAR2(5),
  REFRESH      FLOAT,
  TIT_X_SIZE   FLOAT,
  TIT_X_OFFS   FLOAT,
  TIT_Y_SIZE   FLOAT,
  TIT_Y_OFFS   FLOAT,
  TIT_Z_SIZE   FLOAT,
  TIT_Z_OFFS   FLOAT,
  LAB_X_SIZE   FLOAT,
  LAB_X_OFFS   FLOAT,
  LAB_Y_SIZE   FLOAT,
  LAB_Y_OFFS   FLOAT,
  LAB_Z_SIZE   FLOAT,
  LAB_Z_OFFS   FLOAT,
  GRIDX        NUMBER(1),
  GRIDY        NUMBER(1),
  THETA        FLOAT,
  PHI          FLOAT
);
/

create table ERGOSUM (
 version number(5),
 apiversion varchar2(10),
 alglist number(8)
);

create table SUBSYSTEM (
 SSName varchar2(10)  constraint SS_pk primary key
	USING INDEX (create index SS_pk_ix on SUBSYSTEM(SSName) TABLESPACE INDX01)
);


create table TASK (
 TASKNAME varchar2(64)  constraint TK_PK primary key 
	USING INDEX (create index TK_PK_IX on Task(TaskName) TABLESPACE INDX01), 
 RunOnPhysics number(1),
 RunOnCalib number(1),
 RunOnEmpty number(1),
 SUBSYS1 VARCHAR2(10) CONSTRAINT TK_SS1 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SUBSYS2 VARCHAR2(10) CONSTRAINT TK_SS2 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SUBSYS3 VARCHAR2(10) CONSTRAINT TK_SS3 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SaveFrequency float,
 REFERENCE  varchar2(100)
);

CREATE INDEX TK_SS1_IX on TASK(SUBSYS1) TABLESPACE INDX01;
CREATE INDEX TK_SS2_IX on TASK(SUBSYS2) TABLESPACE INDX01;
CREATE INDEX TK_SS3_IX on TASK(SUBSYS3) TABLESPACE INDX01;



CREATE SEQUENCE Displayoptions_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table DISPLAYOPTIONS (
  DOID integer constraint DO_PK primary key 
        USING INDEX (create index DO_PK_IX on DISPLAYOPTIONS(DOID) TABLESPACE INDX01),
  OPT dispopt
);




CREATE SEQUENCE HistogramSet_ID START WITH 1
      NOMAXVALUE
      CACHE 50;   


create table HISTOGRAMSET (
 HSID integer  constraint HS_PK primary key 
	USING INDEX (create index HS_PK_IX on HISTOGRAMSET(HSID) TABLESPACE INDX01),
 NHS number(6) DEFAULT 1 NOT NULL ,
 HSTASK varchar2(64)  NOT NULL CONSTRAINT HS_TK references TASK(TASKNAME),
 HSALGO varchar2(64) NOT NULL,
 HSTITLE varchar2(100) NOT NULL,
 CONSTRAINT HS_UNQ UNIQUE (HSTASK,HSALGO,HSTITLE) USING INDEX TABLESPACE INDX01,
 HSTYPE varchar2(3) NOT NULL CONSTRAINT HS_TY_CK CHECK (HSTYPE IN ('H1D','H2D','P1D','P2D','CNT')),
 NANALYSIS number(2) DEFAULT 0,
 DESCR varchar2(4000),
 DOC varchar2(200),
 HSDISPLAY integer  CONSTRAINT HS_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL
);
CREATE INDEX HS_TIT_IX ON HISTOGRAMSET(HSTITLE) TABLESPACE INDX01;
CREATE INDEX HS_ALG_IX ON HISTOGRAMSET(HSALGO) TABLESPACE INDX01;
CREATE INDEX HS_TK_IX on HISTOGRAMSET(HSTASK) TABLESPACE INDX01;



CREATE INDEX HS_DISP_IX on HISTOGRAMSET(HSDISPLAY) TABLESPACE INDX01;


create table HISTOGRAM (
 HID varchar2(12)  constraint H_PK primary key 
	USING INDEX (create index H_PK_IX ON HISTOGRAM(HID) TABLESPACE INDX01),
 NAME varchar2(130)  NOT NULL,
 HSET integer NOT NULL CONSTRAINT H_SET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 IHS number(6) DEFAULT 1 NOT NULL ,
 SUBTITLE varchar2(50),
 CONSTRAINT H_UNQ UNIQUE (HSET,SUBTITLE) USING INDEX TABLESPACE INDX01,
 IsTest   number(1),
 IsAnalysisHist number(1) DEFAULT 0 NOT NULL,
 CREATION TIMESTAMP,
 OBSOLETENESS TIMESTAMP,
 DISPLAY integer  CONSTRAINT H_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL
);
CREATE INDEX H_SET_IX on HISTOGRAM(HSET) TABLESPACE INDX01;
CREATE INDEX H_SUB_IX on HISTOGRAM(SUBTITLE) TABLESPACE INDX01;
CREATE INDEX H_DISP_IX on HISTOGRAM(DISPLAY) TABLESPACE INDX01;
CREATE INDEX H_NAME_IX on HISTOGRAM(NAME) TABLESPACE INDX01;

create table DIMSERVICENAME (
 SN varchar2(130) constraint DSN_PK primary key 
	USING INDEX (create index DSN_PK_IX on DIMSERVICENAME(SN) TABLESPACE INDX01),
 PUBHISTO varchar2(12) constraint DSN_PH references HISTOGRAM(HID) ON DELETE CASCADE
);
CREATE INDEX DSN_PHIX on DIMSERVICENAME(PUBHISTO) TABLESPACE INDX01;



create table ALGORITHM (
 ALGNAME varchar2(30) constraint AL_PK primary key 
	USING INDEX (create index AL_PK_IX ON ALGORITHM(ALGNAME) TABLESPACE INDX01),
 ALGTYPE varchar2(8)  NOT NULL CONSTRAINT AL_TY_CK CHECK ( ALGTYPE IN ('HCREATOR','CHECK')),
 NINPUT smallint DEFAULT 0,
 NPARS smallint DEFAULT 0,
 ALGPARS parameters,
 ALGDOC varchar2(1000),
 HCTYPE varchar2(3) default NULL
)NESTED TABLE ALGPARS STORE AS ALGPARS_STORE ;


create table HCREATOR (
 HCID varchar2(12)  NOT NULL constraint HC_ID references HISTOGRAM(HID) ON DELETE CASCADE,
 ALGORITHM varchar2(30) NOT NULL references ALGORITHM(ALGNAME),
 SOURCEH1 varchar2(12) CONSTRAINT HC_SH1 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH2 varchar2(12) CONSTRAINT HC_SH2 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH3 varchar2(12) CONSTRAINT HC_SH3 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH4 varchar2(12) CONSTRAINT HC_SH4 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH5 varchar2(12) CONSTRAINT HC_SH5 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH6 varchar2(12) CONSTRAINT HC_SH6 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH7 varchar2(12) CONSTRAINT HC_SH7 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH8 varchar2(12) CONSTRAINT HC_SH8 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCESET integer CONSTRAINT HC_SSET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 HCPARS thresholds
) NESTED TABLE HCPARS STORE AS HCPARS_STORE ;

CREATE INDEX HC_ID_IX on HCREATOR(HCID) TABLESPACE INDX01;
CREATE INDEX HC_AL_IX on HCREATOR(ALGORITHM) TABLESPACE INDX01;
CREATE INDEX HC_SH1_IX on HCREATOR(SOURCEH1) TABLESPACE INDX01;
CREATE INDEX HC_SH2_IX on HCREATOR(SOURCEH2) TABLESPACE INDX01;
CREATE INDEX HC_SH3_IX on HCREATOR(SOURCEH3) TABLESPACE INDX01;
CREATE INDEX HC_SH4_IX on HCREATOR(SOURCEH4) TABLESPACE INDX01;
CREATE INDEX HC_SH5_IX on HCREATOR(SOURCEH5) TABLESPACE INDX01;
CREATE INDEX HC_SH6_IX on HCREATOR(SOURCEH6) TABLESPACE INDX01;
CREATE INDEX HC_SH7_IX on HCREATOR(SOURCEH7) TABLESPACE INDX01;
CREATE INDEX HC_SH8_IX on HCREATOR(SOURCEH8) TABLESPACE INDX01;
CREATE INDEX HC_SSET_IX on HCREATOR(SOURCESET) TABLESPACE INDX01;

CREATE SEQUENCE Analysis_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table ANALYSIS (
 AID integer  constraint A_PK primary key 
	USING INDEX (create index A_PK_IX ON ANALYSIS(AID) TABLESPACE INDX01),
 HSET integer NOT NULL CONSTRAINT A_HSET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 ALGORITHM varchar2(30) NOT NULL references ALGORITHM(ALGNAME)
);
CREATE INDEX A_HSET_IX on ANALYSIS(HSET) TABLESPACE INDX01;
CREATE INDEX A_AL_IX on ANALYSIS(ALGORITHM) TABLESPACE INDX01;


create table ANASETTINGS (
 ANA integer NOT NULL CONSTRAINT ASET_ANA references ANALYSIS(AID) ON DELETE CASCADE,
 HISTO  varchar2(12) NOT NULL CONSTRAINT ASET_HISTO references HISTOGRAM(HID) ON DELETE CASCADE,
 CONSTRAINT ASET_UNQ UNIQUE (ANA,HISTO) USING INDEX TABLESPACE INDX01,
 MASK number(1) DEFAULT 0 NOT NULL ,
 WARNINGS thresholds,
 ALARMS thresholds
)NESTED TABLE WARNINGS STORE AS WARNING_STORE, NESTED TABLE ALARMS STORE AS ALARM_STORE ;
CREATE INDEX ASET_ANA_IX on ANASETTINGS(ANA) TABLESPACE INDX01;
CREATE INDEX ASET_HISTO_IX on ANASETTINGS(HISTO) TABLESPACE INDX01;

-- for backw. comp.
CREATE or replace FUNCTION PAGEFULLNAME(name VARCHAR2, folder VARCHAR2) RETURN VARCHAR2 AS
BEGIN
   return name;
END;
/

create table PAGEFOLDER (
 PageFolderName varchar2(300) constraint PGF_pk primary key
	USING INDEX (create index PGF_pk_ix on PAGEFOLDER(PageFolderName) TABLESPACE INDX01),
 Parent varchar2(300) default NULL 
);

create table PAGE (
 PageName varchar2(350)  constraint PG_pk primary key
       USING INDEX (create index PG_pk_ix on PAGE(PageName) TABLESPACE INDX01),
 Folder varchar2(300) NOT NULL CONSTRAINT PG_FD references PAGEFOLDER(PageFolderName),
 NHisto integer,
 PageDoc  varchar2(100)
);
CREATE INDEX PG_FD_IX on PAGE(Folder) TABLESPACE INDX01; 

create table SHOWHISTO (
 PAGE varchar2(350) NOT NULL CONSTRAINT SHH_PAGE references PAGE(PageName) ON DELETE CASCADE,
 PAGEFOLDER varchar2(300) NOT NULL,
 HISTO  varchar2(12) NOT NULL CONSTRAINT SHH_HISTO references HISTOGRAM(HID),
 INSTANCE smallint DEFAULT 1 NOT NULL,
 CONSTRAINT SHH_UNQ UNIQUE (PAGE,HISTO,INSTANCE) USING INDEX TABLESPACE INDX01,
 CENTER_X real NOT NULL,
 CENTER_Y real NOT NULL,
 SIZE_X real NOT NULL,
 SIZE_Y real NOT NULL,
 IOVERLAP smallint,
 SDISPLAY integer  CONSTRAINT SH_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL
);
CREATE INDEX SHH_PAGE_IX on SHOWHISTO(PAGE) TABLESPACE INDX01;
CREATE INDEX SHH_HISTO_IX on SHOWHISTO(HISTO) TABLESPACE INDX01;

CREATE or replace FUNCTION SET_SEPARATOR RETURN VARCHAR2 AS
BEGIN
 RETURN '_$';
END;
/

CREATE or replace FUNCTION SUBTITSTRING(subtit VARCHAR2) RETURN VARCHAR2 AS
BEGIN
   IF subtit is null then
   RETURN '';
   ELSE RETURN SET_SEPARATOR()||subtit;
   END IF;
END;
/


create or replace view VIEWHISTOGRAM(NAME,HID,HSID,IHS,NHS,HSTYPE,SUBTITLE,TITLE,ALGO,TASK) as
  SELECT NAME,HID,HSID,IHS,NHS,HSTYPE,SUBTITLE,HSTITLE,HSALGO,HSTASK
  from HISTOGRAM H, HISTOGRAMSET HS where H.HSET = HS.HSID;


create or replace function TNNS(fir IN varchar2, sec IN varchar2) return varchar2 as
-- returns first argument if not null, second otherwise
begin
 if fir is NULL then
  return sec;
 else
  return fir;
 end if;
end;
/
create or replace function TNNF(fir IN float, sec IN float) return float as
begin
 if fir is NULL then
  return sec;
 else
  return fir;
 end if;
end;
/
create or replace function TNNI(fir IN int, sec IN int) return int as
begin
 if fir is NULL then
  return sec;
 else
  return fir;
 end if;
end;
/
@subsystems;
