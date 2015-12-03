// VeloMonitor defines an init method which, when invoked with the activePage,
// runs page-specific actions to do things like fetch job results and display
// plots
var VeloMonitor = (function(window, undefined) {
  'use strict';

  // Store all HTML up here because it looks messy inline with JS and this provides
  // a single place to edit all classes, elements, etc.
  var TEMPLATES = {}
  TEMPLATES.failureDiv = '<div class="alert alert-danger">{0}</div>';
  TEMPLATES.failureMsg = '<p>There was a problem retrieving plot '
    + '<code>{0}</code>. '
    + 'Please contact the administrator. Error message:</p>';
  TEMPLATES.div = '<div></div>';
  TEMPLATES.plotablesContainer = '<ul class="plotables">{0}</ul>';
  TEMPLATES.plotablesItem = '<li id="{0}" style="border-left-color: {2};">'
    + '{1}'
    + '<a class="plotable-toggle pull-right">Hide</a>'
    + '</li>';
  TEMPLATES.sortablePlaceholder = '<li>&nbsp</li>';

  // We need to check if a job result is an array or not, so polyfill the
  // isArray method on the Array object in case it isn't available
  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/isArray
  if (!Array.isArray) {
    JobMonitor.log('Applying polyfill for Array.isArray')
    Array.isArray = function(arg) {
      return Object.prototype.toString.call(arg) === '[object Array]';
    };
  }

  // Return a string of random alphanumeric characters.
  //
  // http://stackoverflow.com/a/8084248/596068
  // Accepts:
  //   length: Length of string to return (default: 7)
  // Returns:
  //   String object of length `length`
  var randomString = function(length) {
    return (1 + Math.random()).toString(36).substring(7);
  };

  // Display msg inside container, styled as a red error box
  // Accepts:
  //   msg: HTML message to display inside container
  //   container: jQuery element to insert msg into
  // Returns:
  //   undefined
  var displayFailure = function(msg, container) {
    container.html(TEMPLATES.failureDiv.format(msg));
  };

  // Add a `Spinner` object to the `element`, using `JobMonitor.settings.spinnerDefaults` as options.
  // Accepts:
  //   element: jQuery element
  // Returns:
  //   Spinner object
  var appendSpinner = function($element) {
    var spinner = new Spinner(JobMonitor.settings.spinnerDefaults).spin($element.get(0));
    // We explicitly set the positioning of the spinner, as although the default
    // position is {top: '50%', left: '50%'}, this doesn't seem to work inside
    // elements hidden with 'display: none' unless we set it explicitly.
    spinner.el.style['top'] = '50%';
    spinner.el.style['left'] = '50%';
    return spinner;
  }

  // Keep track of running jobs, disabling all forms if there are any running
  // This is the simplest way of ensuring that there aren't multiple Promises
  // waiting to replace the current plots with new ones, i.e. ensuring there's
  // only one set of Promises at a time by not allowing the creation of new
  // ones via form submission
  var JobCounter = {
    _njobs: 0,
    increment: function() {
      this._njobs += 1;
      $('form :input').prop('disabled', true);
    },
    decrement: function() {
      this._njobs -= 1;
      if (this._njobs == 0) {
        $('form :input').prop('disabled', false);
      }
    }
  };

  // Load the plot(s) returned by the job
  // 'runview.plots.get_run_plot_with_reference' in to container.
  // Accepts:
  //   args: Object of keyword arguments to pass to the job
  //   opts: Drawing options passed to appropriate d3.plotable
  //   container: jQuery element to load the resulting plot(s) in to, if any
  // Returns:
  //   Polling job task
  var loadRunViewPlotInContainer = function(args, opts, container) {
    var spinner = appendSpinner(container);
    var failure = function(msg) { displayFailure(msg, container); };

    // Use cached job result if available, else create a job
    // Promises used to unify resolution/failure logic
    var cacheKey = args.name + '_' + args.run;
    var cacheResult = JobCache.getItem(cacheKey);
    var task;
    if (cacheResult !== null) {
      JobMonitor.log('Cache hit for key ' + cacheKey + '')
      task = $.Deferred();
      // Can always resolve as only successful jobs are cached
      task.resolve(cacheResult);
    } else {
      task = JobMonitor.createTask('runview.plots.get_run_plot_with_reference', args);
      JobCounter.increment();
    }

    task.done(function(job) {
      JobCache.setItem(cacheKey, job);
      var result = job['result'];
      // Need to treat multiple plots differently from a single one
      if (Array.isArray(result) === true) {
        var plotTypes = [],
            plotData = [];
        for (var i = 0; i < result.length; i++) {
          if (result[i] === null) {
            // TODO alert user that a plot failed
            continue;
          }
          var resultData = result[i]['data'],
              keyData = resultData['data'];
          // Add the key name and title on to the data passed to the plotter
          keyData.name = resultData['name'] + randomString();
          keyData.title = resultData['title'];
          plotTypes.push(resultData['object_class']);
          plotData.push(keyData);
        }

        var chart = VeloPlotter.displayPlots(plotTypes, plotData, [opts], container, failure);
        // Set up the plotable container after the .plot element
        // As the .plot element has a fixed height, inserting them inside it will
        // cause them to overflow outside the div, so insert after
        setupPlotablesContainer(chart, $(TEMPLATES.div).insertAfter(container));
      } else {
        var resultData = result['data'],
            plotType = resultData['object_class'],
            plotData = resultData['data'];
        // Add the key name and title on to the data passed to the plotter
        plotData.name = resultData['name'];
        plotData.title = resultData['title'];

        VeloPlotter.displayPlot(plotType, plotData, opts, container, failure);
      }
    });

    task.fail(function(message) {
      var msg = TEMPLATES.failureMsg.format(args.name) + message;
      displayFailure(msg, container);
    });

    task.always(function() {
      spinner.stop();
      // Don't need to touch the counter if there was a cache hit, i.e. no new
      // job created
      if (cacheResult === null) {
        JobCounter.decrement();
      }
    });

    return task;
  };

  // Display plotable label, one per plotable in the chart, in container that
  // can be reordered and toggled to affect plotables in the chart
  // Accepts:
  //   chart: d3.chart.AxesChart instance
  //   container: jQuery element to hold the container
  // Returns:
  //   undefined
  var setupPlotablesContainer = function(chart, container) {
    var plotables = chart.plotables(),
        names = [],
        containerHTML = '';
    for (var name in plotables) {
      names.push(name);
    }
    // Display plotable labels in reverse order to match expectations
    for (var i = names.length - 1; -1 < i; i--) {
      var name = names[i];
      var title = plotables[name].title,
          // Use the plotables color for the label, if present
          color = plotables[name].color || 'inherit';
      containerHTML += TEMPLATES.plotablesItem.format(name, title, color);
    }
    // Add the container to the DOM
    container.append(TEMPLATES.plotablesContainer.format(containerHTML));
    // Activate sortable lists, adding sortable class to avoid binding drag-&-drop handlers twice
    $('.plotables').not('.sortable')
      .sortable({placeholder: TEMPLATES.sortablePlaceholder})
      .addClass('sortable')
      .bind('sortupdate', function(e, ui) {
        // Get the plotables that the swapped list items represent
        var id1 = ui.item.attr('id');
        var id2 = $(this).find('li').eq(ui.oldindex).attr('id');
        var el1 = $('svg g.' + id1).get(0);
        var el2 = $('svg g.' + id2).get(0);
        // Swap the order of the two plotable-containing <g> elements, using a
        // dummy div and insertBefore to preserve event handlers
        var temp = document.createElement("div");
        el1.parentNode.insertBefore(temp, el1);
        el2.parentNode.insertBefore(el1, el2);
        temp.parentNode.insertBefore(el2, temp);
        temp.parentNode.removeChild(temp);
      })
      .find('.plotable-toggle')
        .on('click', function() {
          // Toggle the SVG plotable visisbility
          $('svg g.{0}'.format(this.parentNode.id)).toggle();
          // Toggle plotable label opacity
          $(this).parents('li').css('opacity', function(i, val) { return val === '0.7' ? 1.0 : 0.7; });
          // Toggle anchor text
          $(this).text(function(i, text) { return text === 'Show' ? 'Hide' : 'Show'; });
        });
  };

  // Each child page in the run view has its contents set by the run view
  // configuration dictionary
  // This sets what plots to retrieve from the run file
  // Each plot has a name, which is sent to the relevant run view job, along
  // with page-specific arguments like sensor number
  var runView = {
    init: function() {
      JobMonitor.log('runView.init');
      this.persistPlotDimensions();
      this.setupTabs();
      this.setupRunSelector();
      this.setupPlots();
      this.fixNavigationOverflow();
    },
    // Load the plots in to the page
    // Accepts:
    //   args: Optional object of arguments to send to the job
    //         The name of the plot is sent automatically
    // Returns:
    //   undefined
    setupPlots: function(args) {
      var requiresSensorSelector = false;
      $('.plot').each(function(idx, el) {
        var $el = $(el),
            name = $el.data('name'),
            run = $('#run-number').data('run'),
            title = $el.data('title'),
            plotOpts = $el.data('plot-options') || {},
            sensorDependent = $el.data('sensor-dependent'),
            sensor = $el.data('sensor');
        args = args || {};
        args.name = name;
        args.run = run;
        // If a plot is sensor dependent and a number is given, format the
        // job's plot name argument with the sensor number
        if (sensorDependent === 'True' && sensor !== '') {
          requiresSensorSelector = true;
          args.name = name.format(sensor)
        }
        loadRunViewPlotInContainer(args, plotOpts, $el);
      });
      // We don't do this inside the each loop else bindings will be duplicated
      if (requiresSensorSelector === true) {
        runView.setupSensorSelector();
      }
    },
    // Explicitly set the plot container's dimensions.
    //
    // As the .run-view-pane elements are hidden with display: none for the
    // tabbing feature, their dimensions are not correctly set by the time the 
    // plots are initialised.
    // Set their width and height inline here, before the tabbing is enabled.
    // ancestor.
    // Returns:
    //   undefined
    persistPlotDimensions: function() {
      $('.plot').each(function(idx, el) {
        var $el = $(el);
        $el.css({
          width: $el.width(),
          height: $el.height()
        });
      });
    },
    // Set up the tab switching mechanism.
    //
    // The showing of the panes is handled by Bootstrap, but we set a URL hash
    // when switching and read it on page load, allowing persistent tab state.
    // Returns:
    //   undefined
    setupTabs: function() {
      var $tabs = $('.run-view-tab'),
          $panes = $('.run-view-pane');

      // Enable the pane-hiding class
      $('.to-be-tab-content').attr('class', 'tab-content');

      // Update the URL hash on tab change
      // Append a '/' to the ID value so the page doesn't jump to the pane's
      // location
      $tabs.children('a').on('shown.bs.tab', function(e) {
        e.preventDefault();
        var tabID = $(e.target).attr('href').substr(1);
        window.location.hash = '/{0}'.format(tabID);
      });

      // Make the tab specified by the window hash active, or activate the
      // first tab if no hash is present
      // We need to strip the '/' off of the URL hash to match the anchor href
      var onHashChange = function() {
        var hash = window.location.hash.substr(2);
        if (hash === '') {
          $tabs.first().children('a').click();
        } else {
          $tabs.children('a[href="#{0}"]'.format(hash)).click();
        }
      };

      window.onhashchange = onHashChange;
      onHashChange();
    },
    // Set up the run selector form.
    //
    // The run selector form, shown in a modal window, shows both a select
    // of nearby runs, and an autocompleting textfield. It's just the textfield's
    // value that's used by the server, so when a run is picked using the select,
    // the textfield needs to be updated to the chosen value.
    // We also need to link the submit button outside the form element to the
    // form itself.
    // Returns:
    //   undefined
    setupRunSelector: function() {
      var $runSelector = $('.run-selector form'),
          $submitButton = $('.run-selector button[type=submit]'),
          $runField = $runSelector.find('input[type=text]');
      $runSelector.find('select').change(function() {
        $runField.val($(this).val());
      });
      $submitButton.click(function() {
        $runSelector.submit();
      });
    },
    // Set up bindings for sensor selector buttons
    //
    // When a VELO layout sensor number button is clicked, the sensor number
    // form is filled in appropriately and clicked
    // Returns:
    //   undefined
    setupSensorSelector: function() {
      // Make sure this function is imdempotent, as we don't want to
      // duplicate these bindings
      if (this.sensorSelectorReady === true) {
        return;
      }
      this.sensorSelectorReady = true;
      var $veloLayout = $('#velo-layout'),
          // Grab any form as they all submit to the same place
          $form = $('.sensor-selector'),
          $input = $form.find('input[type=text]');

      // Submit the sensor number form when a sensor number is clicked
      // TODO active sensor is not updated when new sensor is AJAX'd in
      $veloLayout.find('button:not([disabled])').on('click', function(e) {
        var $target = $(e.target);
        // Don't reload the page if we're already on that sensor
        if ($target.hasClass('active') === true) return;
        // Otherwise, fill in the sensor selector form and submit it
        $input.val($target.text());
        $veloLayout.modal('hide');
        $form.submit();
        return false;
      });

      // If the previous/next button is clicked, decrement/increment the sensor
      // number in the input Because the button's are type=submit, the click
      // event on button will fire first, then the submit event on the form,
      // using the updated sensor number
      $form.find('button[type=submit]').on('click', function() {
        // $form is all forms, so find the form elements of the button that was clicked
        var $target = $(this);
        var $parentForm = $target.closest('.sensor-selector');
        var $siblingInput = $parentForm.find('input[type=text]');
        var diff = 0;
        var sensorNumber = parseInt($siblingInput.val(), 10);

        // Increment/decrement the sensor number as requested, unless the
        // sensor number would become invalid or the text field is focused, in
        // which case the form should be submitted with the current value
        if ($target.hasClass('previous') === true && sensorNumber !== 0) {
          diff = -1;
        } else if ($target.hasClass('next') === true && sensorNumber !== 105) {
          diff = 1;
        }
        if ($siblingInput.is(':focus') === true) {
          diff = 0;
        }

        // We want to update *all* forms with the new value,
        // as all plots will be updated
        $input.val(parseInt($siblingInput.val(), 10) + diff);
      });

      var updateSensorNumber = function(sensorNumber) {
        // We don't set attr data-sensor as it's data('sensor') that's used
        // elsewhere, and jQuery caches calls to data so won't see the updated
        // the DOM
        // TODO this leaves empty ul.plotables in the DOM
        $('.plot, .plotables').empty().data('sensor', sensorNumber);
        runView.setupPlots();
      };

      // Reload any sensor-dependent plots when the sensor selector is submitted
      // TODO corresponding r/phi sensor is not updated when new sensor is AJAX'd in
      $form.on('submit', function() {
        // Must be an integer so it can formatted in to strings in setupPlots
        var sensorNumber = parseInt($input.val(), 10);
        // Build the new URL, preserving the hash (i.e. the tab state)
        var url = $form.attr('action') + '/' + sensorNumber + window.location.hash;
        // Store the sensor number so it can be accessed on window popstate
        window.history.pushState({sensorNumber: sensorNumber}, null, url);
        updateSensorNumber(sensorNumber);
        return false;
      });

      // When the user hits the 'back' button, reload the sensor plots with
      // the 'old' sensor number
      window.onpopstate = function(e) {
        if (e.state !== null && e.state.sensorNumber !== undefined) {
          updateSensorNumber(e.state.sensorNumber);
        }
      };
    },
    // Adjust tabbed navigation to accommodate long tab labels and singular
    // plots
    //
    // The navigation doesn't display correctly with long labels, i.e. that
    // have multiple lines. This fixes that by making all tabs have the same
    // height as the tallest one, and also hides the navigation if there's only
    // tab.
    // Returns:
    //   undefined
    fixNavigationOverflow: function() {
      var navItems = $('.nav-tabs.nav-justified').find('li');
      if (navItems.length == 1) {
          navItems.hide();
      }
      // Find the tallest list item (the <li> element wraps the <a>)
      var heights = navItems.map(function() {
          return $(this).height();
      }).get();
      var maxHeight = Math.max.apply(Math, heights);
      // Force every <a> to have the same height as the tallest <li>
      navItems.find('a').innerHeight(maxHeight);
    }
  };

  // Page-specific modules
  var pages = {
    runView: runView
  };

  // Initialise the app
  var init = function(pageModule) {
    JobMonitor.init(pageModule, pages);
  };

  return {
    init: init
  };
})(window);

$(function() {
  // Poll running jobs every 500 milliseconds
  JobMonitor.settings.pollRate = 500;
  VeloMonitor.init(activePage);
});
