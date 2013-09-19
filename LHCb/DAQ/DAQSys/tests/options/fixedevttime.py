from Configurables import EventClockSvc, FakeEventTime
clock=EventClockSvc()
clock.EventTimeDecoder="FakeEventTime"
clock.addTool(FakeEventTime,name="FakeEventTime")
clock.FakeEventTime.StartTime="1346336270000000000" #2012 08 30
