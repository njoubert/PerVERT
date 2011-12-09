/*!
 * pervert.js Performance Visualization and Error Remediation Toolkit
 * frontend
 *
 * Copyright 2011, Niels Joubert, Eric Schkufza
 *
 * Dependencies: 
 *   - jQuery      http://jquery.com
 *   - JSON        https://github.com/douglascrockford/JSON-js
 *
 * This uses the Revealing Module pattern.
 * Do NOT use this pattern if you create lots of objects,
 * it is for revealing a single module with a public API.
 * If you create many objects, functions should live on prototypes,
 * so that you do not duplicate function implementations every time you create an object
 */

(function(global){ 
  var pervert = function(exec, toggleBusy) {
    return Pervert(exec, toggleBusy);
  }

  /* AJAX Requests Queue - Helper Object */
  var jqXHRQueue = function() {
    self = this;
    self.__data = [];
    self.add = function(xhr,settings) { self.__data.push(xhr); };
    self.complete = function(xhr,success) { 
      var idx = self.__data.indexOf(xhr);
      if (idx!=-1) self.__data.splice(idx, 1);
    };
    self.abort = function() { $.each(self.__data, function(id,xhr) { xhr.abort(); }); self.__data = []; };
    self.ajax = function(path, settings) {
      var oldcomplete = settings["complete"];
      settings["complete"] = function() { self.complete(); oldcomplete(); }
      var t = $.ajax(path, settings);
      self.add(t);
      return t; 
    } 
  }
  
  /* 
  * Back-end Cache Helper Module 
  *   This class handles all the server data interaction and caching.
  *   It gets blown away when we "init" something, so we can start fresh.
  */
  var DB = function(pv, exec) {
    var __pv = pv;
    var __exec = exec;
    var __ajaxqueue = new jqXHRQueue(); 
    
        
    function getListOfExecs(cont) {
      __ajaxqueue.ajax('/pp/list', {
        success: function(data) { $("#pv_bullshit").html(JSON.stringify(data)); },
        complete: function() { cont(); }
       });
    }
    
    var init = function(cont) {
      __pv.log("initting DB");
      getListOfExecs(cont);

      
    }
    
    var abort = function() {
      __ajaxqueue.abort();
    }
    
    return {
      init: init,
      abort: abort,
      
    }
  }
  
  /* 
  * Carries the view state around. 
  *   This interacts with the user and the UI,
  *   and keeps everything happy and up to date.
  *   The UI itself registers event-listeners to this
  */
  var ViewState = function(pv) {
    var __pv = pv;
    var __listeners = {};
    
    //Public API
    var reset = function() {
      //reset to the original view state;
    }

    //EVENT architecture:
    var fireEvent = function(event,caller) {
      $.each(__listeners[event], function(idx,func) {
        func(event,caller);
      })
    }
    var addEvent = function(event) {
      if (!__listeners[event]) {
        __listeners[event] = [];        
      }
      return this;
    }
    //calls func(event,caller) when event fires
    var addListener = function(event,func) {
      if (!__listeners[event]) {
        addEvent(event);
      }
      __listeners[event].push(func);
      return this;
    }
    return {
      reset: reset,
      addListener: addListener,
      addEvent: addEvent,
      fireEvent: fireEvent
    }
  }


  var Pervert = function(exec) {
    var self = this;
    var __exec = exec;
    var __toggleBusy = function() {return;};
    
    var __div_controls = null;
    var __div_memmap = null;
    var __div_context = null;
    var __div_memscatter = null;
    var __div_debug = null;
    
    var __vS = ViewState(this);
    var __db = null;
    
    //private functions:

    function create_controls_view() {
      $(__div_controls).html("<div id='pv_ctx_view'></div>");
      __vS
        .addEvent("controls_click")
        .addEvent("controls_range")
      $("#pv_ctx_view")
        .css("width", 400)
        .css("height", 100)
        .css("background", "#ddd")
        .click(function() {__vS.fireEvent("controls_click", this)});
    }
    
    function create_mem_view() {
    //  __div_memmap.html("CREATE MEM VIEW");
    }
    
    function create_context_view() {
      
    }
    
    function create_scatter_view() {
      //d3.select(__div_memscatter).text("CREATE SCATTER VIEW");
    }
     
    // Public API:  
    var init = function() {
      __toggleBusy(true);
      if (__db)
        __db.abort();
      log("Initializing PerVert...");
      __db = DB(this); //create a new DB
      
      __vS.addListener("controls_click", function() { log("event Controls_Click"); })
      
      __db.init(function() {__toggleBusy(false);});
    }
    
    var clean = function() {
      __toggleBusy(true);

      __db.abort();
      __toggleBusy(false);
    }     
    
    var log = function log(msg) {
      if (__div_debug) {
        var d1 = new Date();
        var ds = d1.getHours() + ":" + d1.getMinutes() + ":" + d1.getSeconds() + "  "
        $(__div_debug).prepend("<p>" + ds + msg + "</p>");
        
      }
    }

    var bindBusy = function(busy) {
      __toggleBusy = busy;
      return this;
    }

    var bindMemView = function(div_memmap, div_memslider) {
      __div_memmap = div_memmap;
      create_mem_view();
      return this;
    }

    var bindContextView = function(div_context) {
      __div_context = div_context;
      create_context_view();
      return this;
    }
    
    var bindControlsView = function(div_controls) {
      __div_controls = div_controls;
      create_controls_view();
      return this;
    }
    
    var bindMemScatterView = function(div_memscatter) {
      __div_memscatter = div_memscatter;
      create_scatter_view();
      return this;
    }

    var bindDebugView = function(div_debug) {
      __div_debug = div_debug;
      return this;
    }
    
    // Exports the public api:
    return {
      init: init,
      clean: clean,
      log: log,
      bindBusy: bindBusy,
      bindMemView: bindMemView,
      bindContextView: bindContextView,
      bindControlsView: bindControlsView,
      bindMemScatterView: bindMemScatterView,
      bindDebugView: bindDebugView,
      
      
    }
    
  }
  
  pervert.VERSION = '0.0.1';
  
  if (global.pervert) {
    throw new Error("pervert has already been defined.")
  } else {
    global.pervert = pervert;
  }
  
})(window);