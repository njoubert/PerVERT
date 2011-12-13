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
      settings["complete"] = function() { self.complete(); if (oldcomplete) { oldcomplete(); } }
      var t = $.ajax(path, settings);
      self.add(t);
      return t; 
    } 
  }
  
  /* Scaling functions */
  

  function make_linear_scale(x, m, minclamp) {
    return function(val) {
      var r = x + m*val;
      if (r < minclamp) {
        r = minclamp;
      }
      return Math.floor(r);      
    }
  }
  
  function make_quadratic_scale(x, m, minclamp) {
    return function(val) {
      var r = x + m*(val*val);
      if (r < minclamp) {
        r = minclamp;
      }
      return Math.floor(r);      
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
    
    var __f_counts = null;
    var __f_counts_pending = false;
    var __f_counts_continuations = [];
    
    var __f_mem_status = {};

    var __f_context = {};
    var __f_memscatter = {};
    var __f_memderiv = {};
    var __f_memhisto = {};
    
        
    var init = function(cont) {
      __pv.log("initting DB");      
    }
    
    var abort = function() {
      __ajaxqueue.abort();
    }
    
    //safe to be called to your heart's delight
    var f_counts = function(cont) { 
      if (__f_counts) {
        cont(__f_counts);
      } else {
        __f_counts_continuations.push(cont);
        if (!__f_counts_pending) {
          __f_counts_pending = true;
          __ajaxqueue.ajax('/f/counts?exec='+__exec, 
          {
            success: function(data) { 
              __pv.log("f_counts returned: " + data); 
              __f_counts = data; 
              $.each(__f_counts_continuations, function(idx,el) { el(__f_counts); }); 
            },
            error: function() { __pv.log("/f/counts FAILED!"); },
            complete: function() { __f_counts_continuations = []; __f_counts_pending = false; }
           });          
        }
      }
    }
    
    var f_mem_status = function(frame, windw, cont) {
      if (__f_mem_status[frame]) {
        return cont(__f_mem_status[frame]);
      } else {
        __ajaxqueue.ajax('/f/mem_status?exec='+__exec+'&frame='+frame+"&window="+windw, {
          success: function(data) { 
            __pv.log("f_mem_status returned: " + data); 
            __f_mem_status[frame] = data;
            cont(__f_mem_status[frame]);
          },
          error: function() { __pv.log("f_mem_status FAILED!"); },
         });          
      }
    }
    
    var f_context = function(frame, cont) {
      if (__f_context[frame]) {
        return cont(__f_context[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_stack?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_context returned: " + data); 
            __f_context[frame] = data;
            cont(__f_context[frame]);
          },
          error: function() { __pv.log("f_context FAILED!"); },
         });          
      }
    }
    
    var f_memscatter = function(frame, cont) {
      if (__f_memscatter[frame]) {
        return cont(__f_memscatter[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_events?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_memscatter returned: " + data); 
            __f_memscatter[frame] = data;
            cont(__f_memscatter[frame]);
          },
          error: function() { __pv.log("f_memscatter FAILED!"); },
         });          
      }
    }
    
    var f_memderiv = function(frame, cont) {
      if (__f_memderiv[frame]) {
        return cont(__f_memderiv[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_deriv?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_memderiv returned: " + data); 
            __f_memderiv[frame] = data;
            cont(__f_memderiv[frame]);
          },
          error: function() { __pv.log("f_memderiv FAILED!"); },
         });          
      }
    }
    
    var f_memhisto = function(frame, cont) {
      if (__f_memhisto[frame]) {
        return cont(__f_memhisto[frame]);
      } else {
        __ajaxqueue.ajax('/f/context_histo?exec='+__exec+'&frame='+frame, {
          success: function(data) { 
            __pv.log("f_memhisto returned: " + data); 
            __f_memhisto[frame] = data;
            cont(__f_memhisto[frame]);
          },
          error: function() { __pv.log("f_memhisto FAILED!"); },
         });          
      }
    }
    
    return {
      init: init,
      abort: abort,
      f_counts: f_counts,
      f_context: f_context,
      f_mem_status: f_mem_status,
      f_memscatter: f_memscatter,
      f_memderiv: f_memderiv,
      f_memhisto: f_memhisto,
    }
  }
  
  /* 
  * Carries the view state around. 
  *   This interacts with the user and the UI,
  *   and keeps everything happy and up to date.
  *   The UI itself registers event-listeners to this
  */
  var ViewState = function(pv) {
    var self = this;
    var __pv = pv;
    var __listeners = {};
    
    var __currentFrame = 0;
    var __maxFrame = 0;
    
    var __playing = false;
    var __playingTimer = null;
    
    function construct() {
      addEvent("frameslider_change");
      addEvent("frameslider_play");
      addEvent("frameslider_pause");
    }
    
    //Public API
    var reset = function() {
      setPlaying(false);
      setCurrentFrame(0);
    }

    var setFrameRange = function(maxframe) {
      __pv.log("Setting max frame to " + maxframe);
      __maxFrame = maxframe;
    }
    
    var setCurrentFrame = function(f) {
      if (f > __maxFrame) {
        setPlaying(false);
      } else if (f < 0) {
        setPlaying(false);
      } else {
        __currentFrame = f;
        fireEvent("frameslider_change", f, this);         
      }
    }
    var getCurrentFrame = function() {
      return __currentFrame;
    }
    
    var setPlaying = function(v) {
      __playing = v;
      if (__playing) {
        
        fireEvent("frameslider_play", true, this);
        if (__currentFrame == __maxFrame) {
          setCurrentFrame(0);
        }
        __playingTimer = setInterval(function() {
          setCurrentFrame(__currentFrame + 1);
        },100);        
        
      } else {
        
        clearInterval(__playingTimer);
        fireEvent("frameslider_pause", true, this);
      
      }
    }
    
    var stepCurrentFrame = function(amt) {
      setCurrentFrame(__currentFrame + amt);
    }
    var togglePlaying = function() {
      setPlaying(!__playing);
    }
    //EVENT architecture:
    var fireEvent = function(eventname,event,caller) {
      if (__listeners[eventname]) {
        $.each(__listeners[eventname], function(idx,func) {
          func(eventname,event,caller);
        })        
      } else {
        __pv.log("Fired an undefined event: " + eventname);
      }
    }
    var addEvent = function(eventname) {
      if (!__listeners[eventname]) {
        __listeners[eventname] = [];
//        __listeners[eventname].push(function(eventname,event,caller) {__pv.log(eventname + " fired " + event)});
        __pv.log(eventname + " added.");
      }
      return this;
    }
    //calls func(event,caller) when event fires
    var addListener = function(eventname,func) {
      if (!__listeners[eventname]) {
        addEvent(eventname);
      }
      __listeners[eventname].push(func);
      return this;
    }

    var obj = {
      reset: reset,
      getCurrentFrame: getCurrentFrame,
      setCurrentFrame: setCurrentFrame,
      stepCurrentFrame: stepCurrentFrame,
      setFrameRange: setFrameRange,
      setPlaying: setPlaying,
      togglePlaying: togglePlaying,
      addListener: addListener,
      addEvent: addEvent,
      fireEvent: fireEvent
    }
    construct(obj)
    return obj;
  }


  var Pervert = function(exec) {
    var self = this;
    var __exec = exec;
    var __toggleBusy = function() {return;};
    
    var __good = false;
    
    var __div_controls = null;
    var __div_memmap = null;
    var __div_context = null;
    var __div_memscatter = null;
    var __div_memderiv = null;
    var __div_memhisto = null;
    var __div_debug = null;
    
    var __vS = null;
    var __db = null;
    
    var data_x_offset = 25;  //on both sides
    var data_y_offset = 25;  //on both sides
    var boxside = 2;

    var canvasWidth = 1024 + 2*data_x_offset;
    var canvasHeight = 550 + 2*data_y_offset;
    
    var dataWidth  = canvasWidth - 2*data_x_offset;
    var dataHeight = canvasHeight - 2*data_y_offset;
    var dataLastX  = canvasWidth - data_x_offset;
    var dataLastY  = canvasHeight - data_y_offset;

    var show_zoom = true;
    var zoom_scalefnc = make_quadratic_scale(20, -0.5, 1);
    //var zoom_scalefnc = make_linear_scale(25, -1, 1);
    
    function construct(obj) {
      __vS = ViewState(obj);
    }
    //private functions:

    function drawgrid(data,f_counts) {
      var canvas_grid = document.getElementById("pv_memmap_canvas_grid");
      var ctx_grid = canvas_grid.getContext("2d");
      var canvasWidth  = canvas_grid.width;
      var canvasHeight = canvas_grid.height;
      ctx_grid.clearRect ( 0 , 0 , canvasWidth , canvasHeight );

      var imageData_grid = ctx_grid.getImageData(0, 0, canvasWidth, canvasHeight);
      var dtg = imageData_grid.data;
                  
      var gridcolor = 235;
      /* Draw the Grid */
      for (var y = data_y_offset - boxside; y < dataLastY; y+=4) { //horizontal
        for (var x = data_x_offset-boxside; x < dataLastX-1; x+=1) {
          var index =  (y*canvasWidth + x)*4
          dtg[index] =   gridcolor;
          dtg[++index] = gridcolor;
          dtg[++index] = gridcolor;
          dtg[++index] = 255;
          
        }
      }
      for (var y = data_y_offset - boxside; y < dataLastY; y+=1) {
        for (var x = data_x_offset-boxside; x < dataLastX; x+=4) {
          var index =  (y*canvasWidth + x)*4
          dtg[index] =   gridcolor;
          dtg[++index] = gridcolor;
          dtg[++index] = gridcolor;
          dtg[++index] = 255;
        }
      }
      
      ctx_grid.putImageData(imageData_grid, 0, 0);

      
    }

    function drawmockup(data,f_counts) {
      
      var canvas = document.getElementById("pv_memmap_canvas");
      
      var ctx = canvas.getContext("2d");
      ctx.clearRect ( 0 , 0 , canvasWidth , canvasHeight );

      //let's calculate global offsets;
      
      
      var fixed_index_offset = (canvasWidth*data_y_offset) + data_x_offset;      
      var total_lines = (f_counts.max_addr / (canvasWidth-100));
      var offset_between_lines = 8;//Math.floor(((canvasHeight-100) / total_lines) + 1);
      
      
      var imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);
      var dt = imageData.data;

      function addr_gxgy(a) { return {
        gx: (Math.floor(a/4)%256),
        gy: Math.floor(Math.floor(a/4)/256)
      }}
      function alloc_gxgy(a) { return {
        gx: (Math.ceil(a/4)%256),
        gy: Math.floor(Math.ceil(a/4)/256)
      }}
      
      function gxgy_index_tl(p) {
        return 4*(fixed_index_offset + 
          Math.floor(p.gx*4 - boxside) + 
          Math.floor((p.gy*4 - boxside)*(dataWidth + data_y_offset*2) ));        
      }
      function gxgy_index_center(p) { 
        return 4*(fixed_index_offset + 
          (p.gx*4) +
          ((p.gy*4)*(dataWidth + data_y_offset*2) ));        
      }
      function index_shift(i,x,y) {
        return i + (x + y*(canvasWidth))*4;
      }
      function index_plus_x(i,x) {
        return i + 4*x;
      }
      function index_plus_y(i,y) {
        return i + y*(canvasWidth)*4;
      }
      
      function newFilledArray(len, val) {
          var a = [];
          while(len--){
              a.push(val);
          }
          return a;
      }


      
      /* Draw the regions */
      var malloc_color = 175;
      for (var idx = 0; idx < data.regions.length; idx++) {
        
        var r = data.regions[idx];     
        
        var index = gxgy_index_tl(addr_gxgy(r.begin));
        
        function draw_vert(x,indx) {
          for (var y = 0; y < 5; y++) {
          var i = index_shift(indx, x, y);
            dt[i] = malloc_color;
            dt[++i] = malloc_color;
            dt[++i] = malloc_color;
            dt[++i] = 255;            
          }
        }
        draw_vert(0,gxgy_index_tl(addr_gxgy(r.begin)));
        draw_vert(0,gxgy_index_tl(alloc_gxgy(r.end)));
           
        for (var s = r.begin; s < r.end; s++) {
          var gxgy = addr_gxgy(s);            
          for (var row = 0; row < 2; row++) {
            var index = gxgy_index_tl(gxgy);
            for (var x = 0; x < 5; x++) {
              var i = index_shift(index, x, 0);
              dt[i] = malloc_color;
              dt[++i] = malloc_color;
              dt[++i] = malloc_color;
              dt[++i] = 255;
            }
            gxgy.gy += 1;
          }
        }
        
      }
      
      /* Draw accesses */
      
      if (!show_zoom) {

        for (var idx = data.addr.length - 1; idx >= 0 ; idx--) {
          var ob = data.addr[idx];
          var gxgy = addr_gxgy(ob);
          var index = gxgy_index_tl(gxgy);

          for (var x = 1; x < 4; x++) {
            for (var y = 1; y < 4; y++) {
              var i = index_shift(index,x,y);            
              if (data.events[idx] == "r") {
                dt[i] = 0;
                dt[i+1] = 255;
                dt[i+2] = 0;
              } else {
                dt[i] = 255;
                dt[i+1] = 0;
                dt[i+2] = 0;
              }
              dt[i+3] = 255;
            }
          }
        }
        
      } else {

        var decay = 0.8;
        var value = 255;
        
        for (var idx = data.addr.length - 1; idx >= 0 ; idx--) {
          var ob = data.addr[idx];
          var gxgy = addr_gxgy(ob);
          var index = gxgy_index_center(gxgy);
          
          var hw = zoom_scalefnc(idx);
          
          for (var x = -hw; x <= hw; x++) {
            for (var y = -hw; y <= hw; y++) {
              var i = index_shift(index,x,y);

              if (hw > 2 && x > -2 && x < 2 && y > -2 && y < 2) {

                  dt[i] = 1;
                  dt[i+1] = 1;
                  dt[i+2] = 1;
                  
              } else if (data.events[idx] == "r") { //READS
                                  
                  dt[i] *= decay;
                  if (dt[i+1] == 0) {
                    dt[i+1] = value;
                  } else {
                    dt[i+1] = Math.max(dt[i+1]*decay, 0);                
                  }
                  dt[i+2] = 0;
                  
                
              } else {  //WRITES
                
                  if (dt[i] == 0)  {
                    dt[i] = value;
                  } else {
                    dt[i] = Math.max(dt[i]*decay, 0);
                  }
                  dt[i+1] *= decay; 
                  dt[i+2] = 0;
                
              }
              
              dt[i+3] = 255;
            }
          }
        }
        
        
        
      }

      ctx.putImageData(imageData, 0, 0);

    }


    function create_controls_view() {
      $(__div_controls)
        .append("<div id='pv_controls_playpause'>4</div>")
        .append("<div id='pv_controls_slider_container'><div id='pv_controls_slider'></div></div>");
      
      __vS.addListener("init", function(eventname, event, caller) {
                
        __db.f_counts(function(counts) {
          __vS.setFrameRange(counts.event-1);
          $("#pv_controls_slider").slider("destroy");
          $("#pv_controls_slider").slider({
            range: false,
            min: 0,
            max: counts.event-1,
            value: 0,
            create: function() { __vS.setCurrentFrame(0,this); return true;},
            slide: function() { __vS.setCurrentFrame($("#pv_controls_slider").slider("value"),this); return true;},
            stop: function() { __vS.setCurrentFrame($("#pv_controls_slider").slider("value"),this); return true;}
          });
        });
        
      });
      
      __vS.addListener("frameslider_change", function(eventname, event, caller) {
        var pr = $("#pv_controls_slider").slider("value");
        if (pr != event) { $("#pv_controls_slider").slider("value", event); }
      });
      
      __vS.addEvent("frameslider_play");
      __vS.addEvent("frameslider_pause");
      
      $("#pv_controls_playpause").click(function() { 
        if (__good) {
          var v = $("#pv_controls_playpause").html();
          if (v == "4") { __vS.setPlaying(true); } else { __vS.setPlaying(false); }          
        }
      });
      __vS.addListener("frameslider_play",  function(eventname, event, caller) { $("#pv_controls_playpause").html("5"); });
      __vS.addListener("frameslider_pause", function(eventname, event, caller) { $("#pv_controls_playpause").html("4"); });
      
      KeyboardJS.bind.key("p", function() { if (__good) { __vS.togglePlaying(); } }, function() {});
      KeyboardJS.bind.key("s", function() { if (__good) { __vS.stepCurrentFrame(1); } }, function() {});
      KeyboardJS.bind.key("a", function() { if (__good) { __vS.stepCurrentFrame(-1); } }, function() {});
      
    }
    
    function create_mem_view() {
      $(__div_memmap).css("width", canvasWidth);
      $(__div_memmap).css("height", canvasHeight);
      
      $(__div_memmap).html("<canvas id='pv_memmap_canvas_grid' width='"+canvasWidth+"' height='"+canvasHeight+"'></canvas>");
      $(__div_memmap).append("<canvas id='pv_memmap_canvas_alloc' width='"+canvasWidth+"' height='"+canvasHeight+"'></canvas>");
      $(__div_memmap).append("<canvas id='pv_memmap_canvas' width='"+canvasWidth+"' height='"+canvasHeight+"'></canvas>");

      $(__div_memmap).css("position", "relative");

      $("#pv_memmap_canvas_grid").css("position", "absolute");
      $("#pv_memmap_canvas_grid").css("top", 0);
      $("#pv_memmap_canvas_grid").css("left", 0);      
      $("#pv_memmap_canvas_grid").css("margin-left", -15);
      $("#pv_memmap_canvas_grid").css("margin-top", -15);
      $("#pv_memmap_canvas_grid").css("z-index", 1);
      
      $("#pv_memmap_canvas_alloc").css("position", "absolute");
      $("#pv_memmap_canvas_alloc").css("top", 0);
      $("#pv_memmap_canvas_alloc").css("left", 0);      
      $("#pv_memmap_canvas_alloc").css("margin-left", -15);
      $("#pv_memmap_canvas_alloc").css("margin-top", -15);
      $("#pv_memmap_canvas_alloc").css("z-index", 2);

      $("#pv_memmap_canvas").css("position", "absolute");
      $("#pv_memmap_canvas").css("top", 0);
      $("#pv_memmap_canvas").css("left", 0);      
      $("#pv_memmap_canvas").css("margin-left", -15);
      $("#pv_memmap_canvas").css("margin-top", -15);
      $("#pv_memmap_canvas").css("z-index", 3);
      
      __vS.addEvent("memmap_click");
      
      //$("#pv_memmap_canvas").click(function(eventObj) {__vS.fireEvent("memmap_click", eventObj, this);})
      
      //__vS.addListener("memmap_click", function(eventname,event,caller) { drawanim();});

      //only draw the grid on init
      __vS.addListener("init", function(eventname, event, caller) {
        __db.f_mem_status(event,150,function(data) {
          __db.f_counts(function(f_counts) {
            drawgrid(data, f_counts);
          })
        })
      });
            
      //draw the other things automatically.
      __vS.addListener("frameslider_change", function(eventname, event, caller) {
        __db.f_mem_status(event,50,function(data) {
          __db.f_counts(function(f_counts) {
            drawmockup(data, f_counts);
          })
        })
      })

    }
    
    function create_context_view() {
      $(__div_context).css("border", "solid black 1px"); 
      __vS.addListener("frameslider_change", function(eventname, event, caller) { __db.f_context(event, function(data) {
          $(__div_context).html("");
          $.each(data.stack, function(idx, val) { 
            $(__div_context).append((data.stack.length-idx) + ": " + val.file + " " + val.line + "<br/>");
          });
        })
      });
    }
    
    function create_scatter_view() {
      var scatter_vis = null;
      var scatter_x = null;
      var scatter_y = null;
      $(__div_memscatter).css("border", "solid red 1px"); 
      __vS.addListener("init", function(eventname, event, caller) {
        __db.f_counts(function(counts) {          
          var xmax = counts.event-1,
              ymax = counts.max_addr,
              w = 450,
              h = 450,
              p = 20;

          scatter_x = d3.scale.linear().domain([0,xmax]).range([0, w]);
          scatter_y = d3.scale.linear().domain([0,ymax]).range([h, 0]);
          scatter_vis = d3.select(__div_memscatter).append("svg")
               .attr("width", w + p * 2)
              .attr("height", h + p * 2)
            .append("g")
              .attr("transform", "translate(" + p + "," + p + ")");

          var xrule = scatter_vis.selectAll("g.x")
              .data(scatter_x.ticks(10))
            .enter().append("g")
              .attr("class", "x");

          xrule.append("line")
              .attr("x1", scatter_x)
              .attr("x2", scatter_x)
              .attr("y1", 0)
              .attr("y2", h);
/*
          xrule.append("text")
              .attr("x", scatter_x)
              .attr("y", h + 3)
              .attr("dy", ".71em")
              .attr("text-anchor", "middle")
              .text(scatter_x.tickFormat(10));
*/
          var yrule = scatter_vis.selectAll("g.y")
              .data(scatter_y.ticks(10))
            .enter().append("g")
              .attr("class", "y");

          yrule.append("line")
              .attr("x1", 0)
              .attr("x2", w)
              .attr("y1", scatter_y)
              .attr("y2", scatter_y);

          yrule.append("text")
              .attr("x", -3)
              .attr("y", scatter_y)
              .attr("dy", ".35em")
              .attr("text-anchor", "end")
              .text(scatter_y.tickFormat(10));

          scatter_vis.append("rect")
              .attr("width", w)
              .attr("height", h);
        });
      });

      __vS.addListener("frameslider_change", function(eventname, event, caller) { 
        __db.f_memscatter(event, function(data) {
              var points = scatter_vis.selectAll("path.dot")
                .data(data.events);
              points.enter().append("path")
                .attr("class", "dot")
                .attr("stroke", function(d, i) { return data.type == "w" ? "red" : "blue"; })
                .attr("transform", function(d) { return "translate(" + scatter_x(d.index) + "," + scatter_y(d.addr) + ")"; })
                .attr("d", d3.svg.symbol());
              points.transition().duration(0)
                .attr("class", "dot")
                .attr("stroke", function(d, i) { return data.type == "w" ? "red" : "blue"; })
                .attr("transform", function(d) { return "translate(" + scatter_x(d.index) + "," + scatter_y(d.addr) + ")"; })
                .attr("d", d3.svg.symbol());
              points.exit().remove();
        });
      });
    }

    function create_deriv_view() {
      var deriv_vis = null;
      var deriv_x = null;
      var deriv_y = null;
      $(__div_memderiv).css("border", "solid green 2px"); 
      __vS.addListener("init", function(eventname, event, caller) {
        __db.f_counts(function(counts) {          
          var xmax = counts.event-1,
              ymax = Math.round(counts.max_addr / 1024),
              w = 450,
              h = 450,
              p = 20;

          deriv_x = d3.scale.linear().domain([0,xmax]).range([0, w]);
          deriv_y = d3.scale.linear().domain([0,ymax]).range([h, 0]);
          deriv_vis = d3.select(__div_memderiv).append("svg")
              .attr("width", w + p * 2)
              .attr("height", h + p * 2)
            .append("g")
              .attr("transform", "translate(" + p + "," + p + ")");

          var xrule = deriv_vis.selectAll("g.x")
              .data(deriv_x.ticks(10))
            .enter().append("g")
              .attr("class", "x");

          xrule.append("line")
              .attr("x1", deriv_x)
              .attr("x2", deriv_x)
              .attr("y1", 0)
              .attr("y2", h);
/*
          xrule.append("text")
              .attr("x", deriv_x)
              .attr("y", h + 3)
              .attr("dy", ".71em")
              .attr("text-anchor", "middle")
              .text(deriv_x.tickFormat(10));
*/
          var yrule = deriv_vis.selectAll("g.y")
              .data(deriv_y.ticks(10))
            .enter().append("g")
              .attr("class", "y");

          yrule.append("line")
              .attr("x1", 0)
              .attr("x2", w)
              .attr("y1", deriv_y)
              .attr("y2", deriv_y);

          yrule.append("text")
              .attr("x", -3)
              .attr("y", deriv_y)
              .attr("dy", ".35em")
              .attr("text-anchor", "end")
              .text(deriv_y.tickFormat(10));

          deriv_vis.append("rect")
              .attr("width", w)
              .attr("height", h);
        });
      });

      __vS.addListener("frameslider_change", function(eventname, event, caller) { 
        __db.f_memderiv(event, function(data) {
          deriv_vis.data(data.events).append("path")
            .attr("class", "line")
            .attr("d", d3.svg.line()
              .x(function(d) { return deriv_x(d.index); })
              .y(function(d) { return deriv_y(d.delta); }));
          /*lines.transition().duration(0)
            .attr("class", "line")
            .attr("d", d3.svg.line()
            .x(function(d) { return deriv_x(d.index); })
            .y(function(d) { return deriv_y(d.delta); }));*/

          var points = deriv_vis.selectAll("circle.line")
              .data(data.events);
          points.enter().append("circle")
              .attr("class", "line")
              .attr("cx", function(d) { return deriv_x(d.index); })
              .attr("cy", function(d) { return deriv_y(d.delta); })
              .attr("r", 3.5)
          points.transition().duration(0)
              .attr("class", "line")
              .attr("cx", function(d) { return deriv_x(d.index); })
              .attr("cy", function(d) { return deriv_y(d.delta); })
              .attr("r", 3.5)
          points.exit().remove();
        });
      });
    }
     
    function create_histo_view() {

      var histo_vis = null;
      var w = 400;
      var h = 400;

      $(__div_memhisto).css("border", "solid yellow 1px"); 
      __vS.addListener("init", function(eventname, event, caller) {
        histo_vis = d3.select(__div_memhisto).append("svg")
            .attr("width", w)
            .attr("height", h)
          .append("g")
            .attr("transform", "translate(.5)");
          
        histo_vis.append("line")
            .attr("class", "histo_line")
            .attr("x1", 0)
            .attr("x2", w)
            .attr("y1", h)
            .attr("y2", h);
      });

      __vS.addListener("frameslider_change", function(eventname, event, caller) { 
        __db.f_memhisto(event, function(data) {
          var x = d3.scale.ordinal()
              .domain(d3.range(d3.max(data.histo, function(d) { return d.line; })))
              .rangeBands([0, w]);

          var y = d3.scale.linear()
              .domain([0, d3.max(data.histo, function(d) { return d.count; })])
              .range([0, h]);

          var bars = histo_vis.selectAll("rect")
              .data(data.histo);

          bars.enter().append("rect")
              .attr("class", "histo_bar")
              .attr("x", function(d) { return x(d.line) + x.rangeBand()/2; })
              .attr("height", function(d) { return y(d.count); })
              .attr("width", x.rangeBand())
              .attr("y", function(d) { return h - y(d.count); });

          bars.exit().remove();

          bars.transition()
              .duration(0)
              .attr("x", function(d) { return x(d.line) + x.rangeBand()/2; })
              .attr("height", function(d) { return y(d.count); })
              .attr("width", x.rangeBand())
              .attr("y", function(d) { return h - y(d.count); });
        });
      });
    }

    // Public API:  
    var init = function() {
      __toggleBusy(true);
      if (__db)
        __db.abort();
      log("Initializing PerVert...");
      __db = DB(this,__exec); //create a new DB
            
      __db.init(function() {__toggleBusy(false);});
      __good = true;
      __vS.addEvent("init");
      __vS.fireEvent("init", null, this);
    }
    
    var clean = function(clean) {
      __toggleBusy(true);
      __good = false;
      if (__db) {
        __db.abort();
      }
      __toggleBusy(false);
    }     
    
    var log = function log(msg) {
      if (__div_debug) {
        var d1 = new Date();
        var ds = d1.getHours() + ":" + d1.getMinutes() + ":" + d1.getSeconds() + "  ";
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

    var bindMemDerivView = function(div_memderiv) {
      __div_memderiv = div_memderiv;
      create_deriv_view();
      return this;
    }

    var bindMemHistoView = function(div_memhisto) {
      __div_memhisto = div_memhisto;
      create_histo_view();
      return this;
    }

    var bindDebugView = function(div_debug) {
      __div_debug = div_debug;
      return this;
    }
    
    var getInfo = function() {
      return {vS : __vS, db : __db};
    }
    
    // Exports the public api:
    var obj = {
      init: init,
      clean: clean,
      log: log,
      bindBusy: bindBusy,
      bindMemView: bindMemView,
      bindContextView: bindContextView,
      bindControlsView: bindControlsView,
      bindMemScatterView: bindMemScatterView,
      bindMemDerivView: bindMemDerivView,
      bindMemHistoView: bindMemHistoView,
      bindDebugView: bindDebugView,
      getInfo: getInfo,
      bindDebugView: bindDebugView
    };
    construct(obj);
    
    return obj;
  }
  
  pervert.VERSION = '0.0.1';
  
  if (global.pervert) {
    throw new Error("pervert has already been defined.")
  } else {
    global.pervert = pervert;
  }
  
})(window);
