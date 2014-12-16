// VeloPlotter is a helper object for plotting VELO data
// It exposes a single function, `displayPlot`, which, given the plot's type
// and its data and a container element, draws the data inside the container
var VeloPlotter = (function(window, undefined) {
  'use strict';

  // Store all HTML up here because it looks messy inline with JS and this provides
  // a single place to edit all classes, elements, etc.
  var TEMPLATES = {};
  TEMPLATES.failureMsg = 'Plot type <code>{0}</code> not supported.';

  // Pleasant colours that combine well, from colorbrewer's Set2
  // http://colorbrewer2.org/
  var COLORS = [
    '#377eb8',
    '#e41a1c',
    '#4daf4a',
    '#984ea3',
    '#ff7f00',
    '#ffff33',
    '#a65628',
    '#f781bf',
    '#999999'
  ];

  // Return s with all non alpha-numeric characters replaced with _
  // Accepts:
  //   s: String to be sanitised (default: '')
  // Returns:
  //   A copy of s that has been sanitised
  var sanitise = function(s) {
    return (s || '').replace(/\W+/g, '_')
  };

  // Make a d3.chart.AxesChart inside the container
  // Accepts:
  //   container: jQuery to draw the chart in
  // Returns:
  //   d3.chart.AxesChart instance
  var makeChart = function(container) {
    // Use the width and height provided, if given
    var width = container.width(),
        height = container.height();
    return d3.select(container.get(0)).append('svg')
      .attr('width', width)
      .attr('height', height)
      .chart('AxesChart');
  };

  // Display plot inside container
  //
  // The type of plot is deduced from the 'object_class' object property on plot,
  // and then the appropriate drawing method is called.
  // If the plotType is not supported, display an error in the container.
  // Accepts:
  //   plotType: ROOT class name
  //   plotData: Data to be consumed by d3.chart.histograms
  //   container: jQuery element to insert plot into
  //   opts: Object of options passed to appropriate d3.plotable
  //         See d3.plotable documentation for available options
  //   failure: Callback executed if the plot can't be drawn
  // Returns:
  //   d3.chart instance
  var displayPlot = function(plotType, plotData, opts, container, failure) {
    var chart = makeChart(container)
      .xAxisLabel(plotData['axis_titles'][0])
      .yAxisLabel(plotData['axis_titles'][1]);
    addPlotToChart(plotType, plotData, opts, chart);
    // Failed to add plotable to chart
    if (chart.plotables().length === 0) {
      var failMsg = TEMPLATES.failureMsg.format(plotType);
      if (failure !== undefined) {
        failure(failMsg);
      }
      return;
    }
    return chart;
  };

  // Display plots inside container
  //
  // The type of each plot is deduced from the 'object_class' object property on
  // the plot, and then the appropriate drawing method is called.
  // If the plotType is not supported, display an error in the container.
  // Accepts:
  //   plotTypes: Array of ROOT class names, one per plot
  //   plotData: Array of data to be consumed by d3.chart.histograms, one per plot
  //   container: jQuery element to insert plot into
  //   opts: List of objects of options passed to appropriate d3.plotable
  //         See d3.plotable documentation for available options
  //   failure: Callback executed if the plot can't be drawn
  // Returns:
  //   d3.chart instance
  var displayPlots = function(plotTypes, plotData, opts, container, failure) {
    // Label the axes with the axis titles of the first plot
    var chart = makeChart(container)
      .xAxisLabel(plotData[0]['axis_titles'][0])
      .yAxisLabel(plotData[0]['axis_titles'][1]);
    var numPlotables = 0;
    for (var i = 0; i < plotTypes.length; i++) {
      addPlotToChart(plotTypes[i], plotData[i], opts[i], chart, i);
      // Failed to add the plotable to the chart
      if (numPlotables === chart.plotables().length) {
        var failMsg = TEMPLATES.failureMsg.format(plotTypes[i]);
        if (failure !== undefined) {
          failure(failMsg);
        }
        return;
      }
      numPlotables++;
    }
    return chart;
  };

  // Add the appropriate, optionally indexed, plotable to to chart
  //
  // Specifying an optional index allows automatic color assignment
  // Accepts:
  //   type: Plot type used to deduce plotable type
  //   data: Plot data to be consumed by the approrpriate d3.plotable
  //   opts: Plot options passed to the d3.plotable instantiation (default: {})
  //   chart: d3.chart to add the plotable to
  //   index: Optional index, used to distinguish multiple plotables in the
  //            same chart (default: 0)
  // Returns:
  //   d3.chart instance
  var addPlotToChart = function(type, data, opts, chart, index) {
    if (opts === undefined) {
      opts = {};
    }
    if (index === undefined) {
      index = 0;
    }
    if (!('color' in opts)) {
      opts.color = COLORS[index];
    }
    var plotable = makePlotable(type, data, opts),
        stats = makeStatisticsBox(type, data, {x: 70 + (index*160), color: opts.color});
    if (plotable !== undefined) {
      chart.addPlotable(plotable);
    }
    if (stats !== undefined) {
      chart.addOrnament(stats);
    }
    // Don't animate large 2D histograms, it's too slow and CPU intensive
    if (type.slice(0, 3) === 'TH2' && data.xbinning.length*data.ybinning.length > 1000) {
      JobMonitor.log('Disabling animation for large TH2');
      chart.animate(false);
    }
    return chart;
  };

  // Accepts:
  //   plotType: ROOT class name
  //   plotData: Data to be consumed by d3.chart.histograms
  //   opts: Object of options passed to appropriate d3.plotable
  //         See d3.plotable documentation for available options (default: {})
  // Returns:
  //   A d3.plotable if plotType is supported, else undefined
  var makePlotable = function(plotType, plotData, opts) {
    if (opts === undefined) {
      opts = {};
    }
    var plotable;
    if (plotType.slice(0, 3) === 'TH1') {
      plotable = plotableTH1(plotData, opts);
    } else if (plotType.slice(0, 3) === 'TH2') {
      plotable = plotableTH2(plotData, opts);
    } else if (plotType === 'TProfile') {
      plotable = plotableTProfile(plotData, opts);
    }
    // Append a title property, useful for other things to fetch later
    plotable.title = plotData.title;
    return plotable;
  };

  // Create a statistics box for the plotable
  //
  // Accepts:
  //   plotType: ROOT class name
  //   plotData: Data to be consumed by d3.plotable.TextBox
  //   opts: Object of options passed to d3.plotable.TextBox (default: {})
  // Returns:
  //   A d3.plotable.TextBox if appropriate for the plotType, else undefined
  var makeStatisticsBox = function(plotType, plotData, opts) {
    if (opts === undefined) {
      opts = {};
    }
    var stats, textBox;
    if (plotType.slice(0, 3) === 'TH1' || plotType === 'TProfile') {
      stats = [
          ['Entries', plotData.entries],
          ['Mean', Math.round(100*plotData.mean)/100],
          ['RMS', Math.round(100*plotData.rms)/100]
      ];
    }
    if (plotType.slice(0, 3) === 'TH2') {
      stats = [
          ['Entries', plotData.entries]
      ];
      // A TH2 doesn't have a single color, so make sure one isn't specified
      delete opts.color
    }
    if (stats !== undefined) {
      textBox = d3.plotable.TextBox(sanitise(plotData.name) + 'StatsBox', stats, opts);
    }
    return textBox;
  };

  // Return d3.plotable.LineChart for the TProfile data
  // Accepts:
  //   data: Plot data as returned by the veloview API
  //   opts: Options passed to the plotable
  // Returns:
  //   d3.plotable.LineChart object
  var plotableTProfile = function(data, opts) {
    var formattedData = [],
        points = data['binning'].length,
        binning, uncertainties, x;
    for (var i = 1; i < points; i++) {
      binning = data['binning'][i];
      uncertainties = data['uncertainties'][i];
      x = (binning[0] + binning[1])/2.0;
      formattedData.push({
        x: x,
        y: data['values'][i],
        xerr: [0,0],
        yerr: [uncertainties[0], uncertainties[1]]
      });
    }
    return d3.plotable.LineChart(sanitise(data.name), formattedData, opts);
  };

  // Return d3.plotable.Histogram for the TH1 data
  // Accepts:
  //   data: Plot data as returned by the veloview API
  //   opts: Options passed to the plotable
  // Returns:
  //   d3.plotable.Histogram object
  var plotableTH1 = function(data, opts) {
    var formattedData = [],
        points = data['binning'].length,
        binning, uncertainties, x;
    for (var i = 1; i < points; i++) {
      binning = data['binning'][i];
      uncertainties = data['uncertainties'][i];
      formattedData.push({
        xlow: binning[0],
        xhigh: binning[1],
        y: data['values'][i],
        yerr: uncertainties
      });
    }
    return d3.plotable.Histogram(sanitise(data.name), formattedData, opts);
  };

  // Return d3.plotable.Histogram2D for the TH2 data
  // Accepts:
  //   data: Plot data as returned by the veloview API
  //   opts: Options passed to the plotable
  // Returns:
  //   d3.plotable.Histogram2D object
  var plotableTH2 = function(data, opts) {
    var formattedData = [],
        xBins = data['xbinning'],
        yBins = data['ybinning'];
    for (var i = 0; i < xBins.length; i++) {
      for (var j = 0; j < yBins.length; j++) {
        formattedData.push({
          xlow: xBins[i][0],
          xhigh: xBins[i][1],
          ylow: yBins[j][0],
          yhigh: yBins[j][1],
          z: data['values'][i][j]
        });
      }
    }
    return d3.plotable.Histogram2D(sanitise(data.name), formattedData, opts);
  };

  return {
    displayPlot: displayPlot,
    displayPlots: displayPlots
  };
})(window);
