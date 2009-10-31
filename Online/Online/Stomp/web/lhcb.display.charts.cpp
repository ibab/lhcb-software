Charts = new Object();

Charts.create = function(area,attrs)              {  return new EJSC.Chart(area,attrs);}

Charts.createAreaSeries = function(handler,attrs) {  return new EJSC.AreaSeries(handler,attrs);    }
 
Charts.createLineSeries = function(handler,attrs) {  return new EJSC.LineSeries(handler,attrs);    }
 
Charts.createArrayDataHandler = function(array) {
  return new EJSC.ArrayDataHandler(array);
}

Charts.dateFormatter = function(attrs) {  return new EJSC.DateFormatter(attrs); }

Charts.define = function(area) {
  area.Chart = Charts;
  area.createChart = Charts.create;
  area.dateFormatter = Charts.dateFormatter
  area.createAreaSeries = Charts.createAreaSeries;
  area.createLineSeries = Charts.createLineSeries;
  area.createArrayDataHandler = Charts.createArrayDataHandler;
}

Charts.createChartArea = function(name) {
  Charts.body = document.getElementById('body_element');
  if ( !Charts.area ) Charts.area = new Object();
  Charts.area[name] = document.getElementById('chart');
  Charts.area[name].caption = document.getElementById('caption');
  Charts.define(Charts.area[name]);
  // document.write('Chartarea defined:'+name+'  '+Charts.area[name]);
  return Charts.area[name];
}
