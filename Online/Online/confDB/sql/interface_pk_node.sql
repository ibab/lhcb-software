create or replace package  routingtable_pck  AUTHID CURRENT_USER is
	function createNodeLink_tab(sysname varchar2) return number;
	function createRT_all(sysname varchar2,devname number,path_necessary number default 0, round_trip number default 10) return number;
	function createPathTab(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number;
	function createRoutingTable_SP(sysname varchar2,devfrom number) return number ;
	function createPathTab_nolkpairs(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number;
	function createTD_all(sysname varchar2,devname number,path_necessary number default 0,round_trip number default 10) return number;
	function CreatePathTab_Host_nolkpairs(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number;
	function createTabDestin(sysname varchar2,devfrom number) return number;	
	function CheckExistenceSeq(seqname varchar2) return number;
	function UpdatePathUsed(systemname varchar2) return number;
	function CheckExistenceTabTemp(tabname varchar2) return number;
	function Insert_firsttab(sysname varchar2,devname number,rt_needed number,path_necessary number) return number;
	function InsertRT_tab(sysname varchar2,devname number) return number;
	function InsertDestin_tab(sysname varchar2,devname number) return number;
	function CreatePathTab_Host(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number;
	function createRT_all_devname(sysname varchar2,devname varchar2,path_necessary number default 0,round_trip number default 10) return number; 
	function createTD_all_devname(sysname varchar2,devname varchar2,path_necessary number default 0,round_trip number default 10) return number;
end routingtable_pck;
/
