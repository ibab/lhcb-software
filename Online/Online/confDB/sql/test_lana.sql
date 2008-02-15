select * from lhcb_lg_devices where devicename like 'X%' and rownum<4;
--insert into lhcb_lg_links (linkway,node_from,node_to, link_weight, lktype,lkused,system_name)
--select distinct 1,f.deviceid,e.deviceid,t.link_weight,t.link_type,1, t.system_name from lhcb_macroscopic_connectivity t , lhcb_port_properties f ,lhcb_port_properties e
--where t.portidfrom=f.portid and t.portidto=e.portid and (mod(t.system_name,:sysIDlist)=0 or mod(:sysIDlist,t.system_name)=0) and t.lkused=1' using sysIDlist,sysIDlist;
--	select t.nodeid_end1,l.devicename,t.pathused,t.routingpathused from lhcb_routingtable t, lhcb_lg_devices l where t.routingpathused=1 and t.nodeid_start0=13314 and t.nodeid_end1=l.deviceid;
--select t.linkid, t.lkused,t.last_update,v.port_type,v.port_nbr,l.devicename,p.devicename from lhcb_macroscopic_connectivity t,lhcb_lg_devices l,lhcb_port_properties v,lhcb_lg_devices p,lhcb_port_properties m where l.deviceid=14792 and l.deviceid=v.deviceid and t.portidto=v.portid and t.portidfrom=m.portid and m.deviceid=p.deviceid;
select max(last_update) from lhcb_routingtable ;
--select * from lhcb_routingtable where nodeid_end1=14792;
--select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g,lhcb_macroscopic_connectivity x where (mod(x.system_name,1)=0 or mod(1,x.system_name)=0) and f.portid=x.portidfrom and x.portidto=g.portid group by f.deviceid,g.deviceid having sum(x.lkused)=0;
--select mod(1,5) from dual;
--commit;
  --update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.pathused from lhcb_pdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0);
--update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.pfromid0 or p.portidto=r.pfromid0) where r.pathused=1;
--update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.ptoid1 or p.portidto=r.ptoid1) where r.pathused=1;
--update lhcb_routingtable f set f.routingpathused=1 where f.pathused=1  and f.pathid||':'||f.pfromid0=(select  min(t.pathid||':'||t.pfromid0) from lhcb_routingtable t where t.nodeid_start0=f.nodeid_start0 and t.ptoid1=f.ptoid1 ) ;
  --commit;
--select  min(pathid||':'||pfromid0)||'?'||t.ptoid1||'?'||t.nodeid_start0 from lhcb_routingtable t group by t.nodeid_start0,t.ptoid1
 --select * from user_jobs
 --select * from lhcb_pdetails    where pathused=0;
 --select * from lhcb_routingtable where nodeid_end1=14792;
  -- update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.pathused from lhcb_pdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0);
  --update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.pfromid0 or p.portidto=r.pfromid0) where r.pathused=1;
--update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.ptoid1 or p.portidto=r.ptoid1) where r.pathused=1;

--update lhcb_routingtable f set f.routingpathused=0;
--update lhcb_routingtable f set f.routingpathused=1 where f.pathused=1  and f.pathid||':'||f.pfromid0=(select  min(t.pathid||':'||t.pfromid0) from lhcb_routingtable t where t.nodeid_start0=f.nodeid_start0 and t.ptoid1=f.ptoid1 ) ;

--select * from lhcb_routingtable where nodeid_end1=14792;
  --commit;
 -- select * from lhcb_port_properties t,lhcb_lg_devices r where r.deviceid=t.deviceid and t.deviceid=13313 and t.portid in (select portidfrom from lhcb_macroscopic_connectivity);
--select count(*) from lhcb_macroscopic_connectivity where bidirectional_link_used=1 and portidfrom in (select portid from lhcb_port_properties where deviceid=13313);
--select ceil(max(t.last_update) - min(f.last_update)) from lhcb_macroscopic_connectivity t,lhcb_pdetails f ;
select max(last_update)  from lhcb_macroscopic_connectivity;
--select * from lhcb_pdetails where node1=13313;
--select count(*) from lhcb_routingtable where nodeid_start0=13313 and routingpathused=1;
   -- select * from lhcb_pdetails where node1=13314;
