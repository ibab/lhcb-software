create or replace package  pathqueries_pck  AUTHID CURRENT_USER is
	function checkUpdatePathTable(daq_sysid in number, tfc_sysid number) return number ;
	function generKeyDTab(systemname in number) return number;
	
	function CheckPathTabCompleteAndUpdate(systemname in number)  return varchar2 ;
	
end pathqueries_pck;
/