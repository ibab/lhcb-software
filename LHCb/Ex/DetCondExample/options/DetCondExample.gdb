# $Id: DetCondExample.gdb,v 1.2 2001-09-17 13:49:10 andreav Exp $
delete breakpoints
break 'PopulateDB::initialize(void)'
break 'EvtClock::initialize(void)'
break 'TestConditionsDBDataSvc::initialize(void)'
break 'TestSmartDataPtr::initialize(void)'
break 'EvtClock::execute(void)'
break 'EvtClock::finalize(void)'
run
