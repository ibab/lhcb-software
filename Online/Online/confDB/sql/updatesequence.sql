create or replace procedure UpdateSequence(seq_name varchar2)  AUTHID CURRENT_USER   is

rescreate number:=0;
v_error_message varchar2(4000);
begin

execute immediate 'select '||seq_name||'.nextval from dual' into rescreate;
rescreate:=-rescreate;

execute immediate 'alter sequence '||seq_name||' increment by '||rescreate;

execute immediate 'select '||seq_name||'.nextval from dual' into rescreate;

execute immediate 'alter sequence '||seq_name||' increment by 1';

exception
when NO_DATA_FOUND then
raise_application_error( sqlcode, 'UpdateSequence: no data found');
when OTHERS then
v_error_message:=sqlerrm;
raise_application_error( sqlcode, 'UpdateSequence:'||v_error_message);
end UpdateSequence;
/

