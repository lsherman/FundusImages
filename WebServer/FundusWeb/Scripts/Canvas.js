/// <reference path="~/Scripts/Support/_references.js" />
/// <reference path="~/Scripts/FundusImage.js" />
/// <reference path="~/Scripts/ImageProcessing.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Creates a canvas element for displaying and annotating an image
// ----------------------------------------------------------------------------
function Canvas(canvasElem) {
    this._canvasElem = canvasElem;

    // Handle mouse events on the canvas
    var canvas = $(canvasElem);
    canvas.on('mousemove', $.proxy(this._onMouseMove, this));
    canvas.on('mousedown', $.proxy(function (e) {
        this._mousePos.x = e.screenX;
        this._mousePos.y = e.screenY;
        this._mousebutton[e.which-1] = true; }, this));
    canvas.on('mouseup', $.proxy(function (e) {
        this._mousebutton[e.which-1] = false;
    }, this));

    // Initialize the Hammer gesture detection library
    if (Modernizr.touch) {
        var hammertime = Hammer(canvasElem, {
            transform_always_block: true,
            transform_min_scale: 1,
            drag_block_horizontal: true,
            drag_block_vertical: true,
            drag_min_distance: 0
        });

        var last_scale, last_posx, last_posy;
        hammertime.on('touch drag transform', function (ev) {
            switch (ev.type) {
                case 'touch':
                    WebPage.canvas.last_scale = WebPage.canvas._fundusImage._zoomLevel;
                    WebPage.canvas.last_posx = WebPage.canvas._fundusImage._offset.x;
                    WebPage.canvas.last_posy = WebPage.canvas._fundusImage._offset.y;
                    break;

                case 'drag':
                    WebPage.canvas._fundusImage.setPosition(
                        WebPage.canvas.last_posx + ev.gesture.deltaX / WebPage.canvas._fundusImage._zoomLevel,
                        WebPage.canvas.last_posy + ev.gesture.deltaY / WebPage.canvas._fundusImage._zoomLevel);
                    break;

                case 'transform':
                    var scale = WebPage.canvas.last_scale * ev.gesture.scale;
                    WebPage.canvas._fundusImage.setZoom(scale);
                    break;
            }
        });
    }

    // Setup the canvas display buttons
    var dispButtons = $("#dispTools");
    dispButtons.buttonset();
    dispButtons.find("#dispBase")
        .button({ icons: { primary: "icon-show-base" }, text: false })
        .click(function () { WebPage.canvas._fundusImage.showBase(this.checked); });
    dispButtons.find("#dispSeg")
        .button({ icons: { primary: "icon-show-segment" }, text: false })
        .click(function () { WebPage.canvas._fundusImage.showSegmented(this.checked); });
    dispButtons.find("#dispAnn")
        .button({ icons: { primary: "icon-show-annotate" }, text: false })
        .click(function () { WebPage.canvas._fundusImage.showAnnotated(this.checked); });
       
    // Setup the canvas quick-tool buttons
    var quickTools = $("#quickTools");
    quickTools.buttonset();
    quickTools.find(".quickPageFit")
        .button({ icons: { primary: "icon-quick-fit" }, text: false })
        .click($.proxy(this.fitToPage, this));
    quickTools.find(".quickSave")
        .button({ icons: { primary: "icon-quick-save" }, text: false })
        .click($.proxy(this.download, this));
    quickTools.find(".quickPrint")
        .button({ icons: { primary: "icon-quick-print" }, text: false })
        .click($.proxy(this.print, this));
    quickTools.find(".quickZoomIn")
        .button({ icons: { primary: "icon-quick-zoom-in" }, text: false })
        .click($.proxy(function () { this.zoom(5); }, this));
    quickTools.find(".quickZoomOut")
        .button({ icons: { primary: "icon-quick-zoom-out" }, text: false })
        .click($.proxy(function () { this.zoom(-5); }, this));

    // Hotkeys for image annotation and tools
    $(document).keypress(this, this._keyEventHandler);

    // Create the offscreen canvas for image editing
    this._offCanvas = document.createElement("canvas");
    this._annCanvas = document.createElement("canvas");

    this._mousebutton = [false, false, false];
    this._mousePos    = { x: 0, y: 0 };
}

Canvas.prototype =
{
    zoom: function (steps) {
        /// <summary>Changes the zoom level of the fundus image</summary>
        if (this._fundusImage == null) return;
        this._fundusImage.zoom(steps);
    },

    fitToPage: function () {
        /// <summary>Fits the current fundus image, if present, to the page</summary>

        if (!this._fundusImage || !this._fundusImage.baseImage) return;

        var iw = this._fundusImage.baseImage.width;
        var ih = this._fundusImage.baseImage.height;
        var cw = this._canvasElem.width;
        var ch = this._canvasElem.height;

        var s = Math.min(cw/iw,ch/ih);
        
        this._blockRedraws = true;
        this._fundusImage.setPosition(0, 0);
        this._fundusImage.setZoom(s);
        this._blockRedraws = false;

        this.draw();
    },

    drawCached: function () {
        /// <summary>Updates the cached image data on the offscreen canvas</summary>

        // Acquire a handle to the canvas context
        var ctx = this._offCanvas.getContext("2d");

        ctx.drawImage(this._fundusImage.baseImage, 0, 0);

        // Perform the image processing / annotation / effects
        var img = ctx.getImageData(0, 0, this._offCanvas.width, this._offCanvas.height);
        FundusWeb.Image.process(this._fundusImage, img);
        ctx.putImageData(img, 0, 0);
    },

    draw: function () {
        /// <summary>Redraws the image on the visible canvas element</summary>

        if (this._blockRedraws) return;

        // If there is no image set, clear the drawing canvas
        if (!this._fundusImage) {
            var ctx = this._canvasElem.getContext("2d");
            ctx.fillStyle = "#000000";
            ctx.rect(0, 0, this._canvasElem.width, this._canvasElem.height);
            ctx.fill();
            return;
        }

        // If there is an unloaded image, display load graphic
        if (!this._fundusImage.baseImage) return;

        // Acquire a handle to the canvas context
        var ctx = this._canvasElem.getContext("2d");

        // Compute the image position offset
        var s = this._fundusImage._zoomLevel;
        var w = this._canvasElem.width  / s / 2;
        var h = this._canvasElem.height / s / 2;
        var x = w + this._fundusImage._offset.x;
        var y = h + this._fundusImage._offset.y;
        x -= this._fundusImage.baseImage.width / 2;
        y -= this._fundusImage.baseImage.height / 2;

        // Draw the fundus images
        ctx.save();
        ctx.fillStyle = "#000000";
        ctx.rect(0, 0, this._canvasElem.width, this._canvasElem.height);
        ctx.fill();
        ctx.scale(s, s);
        if (this._fundusImage._showBase) ctx.drawImage(this._offCanvas, x, y);
        if (this._fundusImage._showSegment && this._fundusImage.segImage) {
            var ssx = this._fundusImage.baseImage.width / this._fundusImage.segImage.width;
            var ssy = this._fundusImage.baseImage.height / this._fundusImage.segImage.height;
            var sx = this._canvasElem.width  / ssx / s / 2 + this._fundusImage._offset.x / ssx - this._fundusImage.segImage.width / 2;
            var sy = this._canvasElem.height / ssy / s / 2 + this._fundusImage._offset.y / ssy - this._fundusImage.segImage.height / 2;
            ctx.scale(ssx, ssy);
            ctx.drawImage(this._fundusImage.segImage, sx, sy);
        }
        ctx.restore();
    },

    setImage: function (image, blockHistory) {
        /// <summary>Sets the image displayed in the canvas</summary>
        /// <param name="image" type="FundusImage"></param>

        WebPage.toolBar.populate(image);

        $(this._fundusImage).unbind('.Canvas');

        this._fundusImage = image;

        // Detect a null image set so we can clear the view
        if (this._fundusImage == null) { this.draw(); return; }

        var canvas = this;

        // Update the status of the layer display buttons
        $("#dispBase").prop('checked', this._fundusImage._showBase).button("refresh");
        $("#dispAnn").prop('checked', this._fundusImage._showAnnotate).button("refresh");
        $("#dispSeg").prop('checked', this._fundusImage._showSegment).button("refresh");

        // Canvas interaction which does not modify the base image data (viewing)
        $(this._fundusImage).bind('onSegLoad.Canvas', function () { canvas.draw(); });
        $(this._fundusImage).bind('positionChanged.Canvas', function () { canvas.draw(); });
        $(this._fundusImage).bind('zoomChanged.Canvas', function () { canvas.draw(); });
        $(this._fundusImage).bind('displayChanged.Canvas', function () { canvas.draw(); });

        // Canvas interaction which modifies the base image data (editing)
        $(this._fundusImage).bind('dataChanged.Canvas', function () { canvas.drawCached(); canvas.draw(); });

        // Load the initial image dimensions into the canvas when first available
        var initialDraw = function () {
            canvas._offCanvas.width  = canvas._fundusImage.baseImage.width;  // Modified image
            canvas._offCanvas.height = canvas._fundusImage.baseImage.height;
            canvas._annCanvas.width  = canvas._fundusImage.baseImage.width;  // Annotation overlay
            canvas._annCanvas.height = canvas._fundusImage.baseImage.height;
            canvas.drawCached();
            canvas.draw();
        };
        if (this._fundusImage.baseImage == null) {
            $(this._fundusImage).bind('onBaseLoad.Canvas', initialDraw);
            canvas.draw(); // :TODO: Draw loading screen/animation/etc...
        } else {
            initialDraw();
        }
    },

    download: function () {
        /// <summary>Downloads the annotated image to the client</summary>

        // Setup the anchor element for image download
        var canvas = this._offCanvas;
        var a = $("<a id='link' href='#'>Download</a>");
        a.on("click", function () {
            $(this)
                .attr("href", canvas.toDataURL("image/png"))
                .attr("download", "image.png");
        });

        // DOM 2 Events for initiating the anchor link
        var dispatchMouseEvent = function (target, var_args) {
            var e = document.createEvent("MouseEvents");
            e.initEvent.apply(e, Array.prototype.slice.call(arguments, 1));
            target.dispatchEvent(e);
        };
        dispatchMouseEvent(a[0], 'mouseover', true, true);
        dispatchMouseEvent(a[0], 'mousedown', true, true);
        dispatchMouseEvent(a[0], 'click',     true, true);
        dispatchMouseEvent(a[0], 'mouseup',   true, true);
    },

    print: function () {
        /// <summary>Prints the canvas image</summary>

        popup = window.open();
        popup.document.write('<img src="' + this._offCanvas.toDataURL("image/png") + '";></img>');
        popup.print();
    },

    getImage: function () {
        return this._fundusImage;
    },

// Private:

    _onMouseMove: function (e) {
        /// <param name="e" type="JQuery.Event">event</param>

        e.preventDefault()

        if (!this._fundusImage) return;

        var scale = this._fundusImage._zoomLevel;
        var moveX = this._mousePos.x - e.screenX;
        var moveY = this._mousePos.y - e.screenY;
        moveX /= scale;
        moveY /= scale;

        // Apply active tool on left button
        if (this._mousebutton[0]) {
            var window = moveX / 500.0 + this._fundusImage._window;
            var level  = moveY / 500.0 + this._fundusImage._level;
            this._fundusImage.setWindowLevel(window, level);
        }

        // Drag image on right button
        if (this._mousebutton[2]) this._fundusImage.move(-moveX, -moveY);

        this._mousePos.x = e.screenX;
        this._mousePos.y = e.screenY;
    },

    _keyEventHandler: function (e) {
        /// <param name="e" type="JQuery.Event">event</param>
        var canvas = e.data;
        var fundus = canvas._fundusImage;
        
        if (e.charCode == 103) { // G
            fundus.setGrayscale(!fundus._grayscale);
        }
    },

    // Private:
    _mousebutton: [],           /// <field name='_mousebutton' type='Array'>Tracking array for mouse button status</field>
    _mousePos: { x: 0, y: 0 },  /// <field name='_mousePos'>The mouse position for the most recent event</field>
    _touchPos: { x: 0, y: 0 },  /// <field name='_mousePos'>The touch position for the most recent event</field>
    _fundusImage: null,         /// <field name='_fundusImage' type='FundusImage'>The image currently in this canvas</field>
    _canvasElem: null,          /// <field name='_canvasElem' type=''>The HTML canvas elemented associated with this object</field>
    _offCanvas: null,           /// <field name='_offCanvas' type=''>An offscreen canvas for image editing</field>
    _annCanvas: null,           /// <field name='_annCanvas' type=''>An offscreen canvas for image annotations</field>
    _blockRedraws: false,       /// <field name='_blockRedraws' type='Boolean'>Blocks the draw function from being executed</field>
}

// ----------------------------------------------------------------------------
//  Action for switching the currently displayed image
// ----------------------------------------------------------------------------
function SetImageAction(oldImage, newImage) {
    this._old = oldImage;
    this._new = newImage;
}

SetImageAction.prototype =
{
    text: "changing the active image",

    // Private:

    undo: function () {
        WebPage.canvas.setImage(this._old, true);
    },

    redo: function () {
        WebPage.canvas.setImage(this._new, true);
    },

    _old: null, /// <field name='_index' type='FundusImage'>The previous fundus image</field>
    _new: null, /// <field name='_index' type='FundusImage'>The newer fundus image</field>
}