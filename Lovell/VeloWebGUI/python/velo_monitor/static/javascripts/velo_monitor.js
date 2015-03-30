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
    + '<code>{0}</code>'
    + 'Please contact the administrator. Error message:</p>';
  TEMPLATES.row = '<div class="row"></div>';
  TEMPLATES.togglesContainer = '<div class="toggles col-md-4 col-md-offset-4">{0}</div>';
  TEMPLATES.togglesToggle = '<div class="toggle">'
    + '<label for="{0}">'
    + '<input type="checkbox" id="{0}" checked> <span style="color: {2}">{1}</span>'
    + '</label>'
    + '</div>';

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

  // Load the plot(s) returned by the job 'runview.plots.get_run_plot' in to container.
  // Accepts:
  //   args: Object of keyword arguments to pass to the job
  //   opts: Drawing options passed to appropriate d3.plotable
  //   container: jQuery element to load the resulting plot(s) in to, if any
  // Returns:
  //   Polling job task
  var loadRunViewPlotInContainer = function(args, opts, container) {
    var task = JobMonitor.createTask('runview.plots.get_run_plot', args),
        spinner = appendSpinner(container),
        failure = function(msg) { displayFailure(msg, container); };

    task.done(function(job) {
      spinner.stop();

      var result = job['result'];
      // Need to treat multiple plots differently from a single one
      if (Array.isArray(result) === true) {
        var plotTypes = [],
            plotData = [];
        for (var i = 0; i < result.length; i++) {
          var resultData = result[i]['data'],
              keyData = resultData['data'];
          // Add the key name and title on to the data passed to the plotter
          keyData.name = resultData['name'] + randomString();
          keyData.title = resultData['title'];
          plotTypes.push(resultData['object_class']);
          plotData.push(keyData);
        }

        var chart = VeloPlotter.displayPlots(plotTypes, plotData, [opts], container, failure);
        // Set up the plotable toggles after the .plot element
        // As the .plot element has a fixed height, inserting them inside it will
        // cause them to overflow outside the div, so insert after
        setupPlotablesToggler(chart, $(TEMPLATES.row).insertAfter(container));
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
      spinner.stop();
      var msg = TEMPLATES.failureMsg.format(args.name) + message;
      displayFailure(msg, container);
    });
    return task;
  };

  // Display checkboxes, one per plotable in the chart, in container that toggle plotable visibility
  // Accepts:
  //   chart: d3.chart.AxesChart instance
  //   container: jQuery element to hold the toggles
  // Returns:
  //   undefined
  var setupPlotablesToggler = function(chart, container) {
    var plotables = chart.plotables(),
        toggles = '';
    for (var name in plotables) {
      var title = plotables[name].title,
          // Use the plotables color for the label, if present
          color = plotables[name].color || 'inherit';
      toggles += TEMPLATES.togglesToggle.format(name, title, color);
      // Bind to the checkbox's change event (i.e. when its state toggles)
      $(document).on('change', '#{0}'.format(name), function(e) {
        $('svg g.{0}'.format(e.target.id)).toggle();
      });
    }
    // Add the toggles to the DOM
    container.append(TEMPLATES.togglesContainer.format(toggles));
  };

  // Each child page in the run view has its contents set by the run view
  // configuration dictionary
  // This sets what plots to retrieve from the run file
  // Each plot has a name, which is sent to the relevant run view job, along
  // with page-specific arguments like sensor number
  var runView = {
    init: function() {
      JobMonitor.log('runView.init');
      // Before hiding the plot DOM elements, set their dimensions explicitly
      // Dimensions are lost when using `display: none`, so this allows us
      // to retrieve them even when the element is hidden
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
    // tabbing feature, their dimensions cannot be retrieved by default
    // Instead, set their width and height inline equal to that of a non-hidden
    // ancestor.
    // Returns:
    //   undefined
    persistPlotDimensions: function() {
      var $plots = $('.run-view-pane').children('.plot');
      $plots.each(function(idx, el) {
        var $el = $(el);
        $el.css({width: $el.closest('.tab-content').width(), height: $el.height()});
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
      // Update the URL hash on tab change
      // Append a '/' to the ID value so the page doesn't jump to the pane's
      // location
      $tabs.children('a').on('shown.bs.tab', function(e) {
        e.preventDefault();
        var id = $(e.target).attr('href').substr(1);
        window.location.hash = '/{0}'.format(id);
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

      // Reload any sensor-dependent plots when the sensor selector is submitted
      $form.on('submit', function() {
        // Must be an integer so it can formatted in to strings in setupPlots
        var sensorNumber = parseInt($input.val(), 10);
        // Build the new URL, preserving the hash (i.e. the tab state)
        var url = $form.attr('action') + '/' + sensorNumber + window.location.hash;
        // We don't set attr data-sensor as it's data('sensor') that's used
        // elsewhere, and jQuery caches calls to data so won't see the updated
        // the DOM
        $('.plot').empty().data('sensor', sensorNumber);
        window.history.pushState(null, null, url);
        runView.setupPlots();
        return false;
      });
    },
    // Adjust tabbed navigation to accommodate long tab labels and singular plots
    //
    // The navigation doesn't display correctly with long labels, i.e. that have
    // multiple lines. This fixes that by making all tabs have the same height
    // as the tallest one, and also hides the navigation if there's only tab.
    // Returns:
    //   undefined
    fixNavigationOverflow: function() {
      var navItems = $('.nav-tabs.nav-justified').find('li');
      if (navItems.length == 1) {
          console.log('hidden nav');
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
  VeloMonitor.init(activePage);
});
