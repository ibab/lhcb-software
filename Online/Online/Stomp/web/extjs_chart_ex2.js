/*!
 * Ext JS Library 3.1.1
 * Copyright(c) 2006-2010 Ext JS, LLC
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
function generateData()  {
    var data = [];
    for(var i = 0; i < 12; ++i){
        data.push([i+13400, (Math.floor(Math.random() *  11) + 1) * 100]);
    }
    return data;
}

Ext.onReady(function(){
    var store = new Ext.data.ArrayStore({
        fields: ['run', 'files'],
        data: generateData()
    });
    
    new Ext.Panel({
        width: 700,
        height: 400,
        renderTo: document.body,
        title: 'Column Chart with Reload - Hits per Month',
        tbar: [{
            text: 'Load new data set',
            handler: function(){
                store.loadData(generateData());
            }
        }],
        items: {
            xtype: 'columnchart',
            store: store,
            yField: 'files',
	    url: 'ExtJs/resources/charts.swf',
            xField: 'run',
            xAxis: new Ext.chart.CategoryAxis({
                title: 'Run'
            }),
            yAxis: new Ext.chart.NumericAxis({
                title: 'Files'
            }),
            extraStyle: {
               xAxis: {
                    labelRotation: -90
                }
            }
        }
    });
});
