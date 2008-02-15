create or replace package  routingtable_pck AUTHID CURRENT_USER is
	function createNodeLink_tab(sysIDlist number,recreate_table number,bidi_needed number default 0) return number;
	function createRT_all(devname number,path_necessary number default 0,round_trip number default 10,recreate_table number default 1) return number;
	function Insert_firsttab(devname number,rt_needed number,path_necessary number) return number;
	function createRT_all_devname(devname varchar2,path_necessary number default 0,round_trip number default 10,recreate_tab number default 1) return number; 
	function createPathTab(devfrom number,rt_needed number, round_trip number default 10) return number;
	function createRoutingTable_SP(devfrom number) return number ;
	function createTD_all_devname(devname varchar2,path_necessary number default 0,round_trip number default 10,recreate_tab number default 1) return number;
	function createTD_all(devname number,path_necessary number default 0,round_trip number default 10,recreate_tab number default 1) return number;
	function createTabDestin(devfrom number) return number;	
	function CheckExistenceSeq(seqname varchar2) return number;
	function UpdatePathUsed(systemname number) return number;
	function CreatePathTab_Host( devfrom number,rt_needed number, round_trip number default 10) return number;
	procedure createMicroNodeLink_tab(motherboardidList in out number ,recreate_table number,rescode in out number, cpntid in out number,cpntname varchar2 default 'none');
	function createMicroPathTab(devfrom number, round_trip number default 10) return number;
	function CreateMicroPathTab_Host( devfrom_bis number, round_trip number default 10) return number;
	function InsertIntoMicroPathCnptType( portid_from number,cpnttype varchar2,cpnttype_given number) return number;
	function InsertIntoMicroPathFromPortID( portid_from number,portid number, cpntname varchar2) return number;
	function InsertIntoMicroPathUsingID( cpntid number,portid number) return number;
	function DeleteLinksInPaths(pfromid number,ptoid number) return number;
	
	function checkUpdatePathTable(daq_sysid in number, tfc_sysid number) return number ;
	function generKeyDTab(systemname in number) return number;
	function CheckPathTabCompleteAndUpdate(systemname in number)  return varchar2 ;
end routingtable_pck;
/
