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

    // Handle the canvas quick-tool buttons
    var quickTools = $("#quickTools");
    quickTools.buttonset();
    quickTools.find(".quickPageFit").click($.proxy(this.fitToPage, this));
    quickTools.find(".quickSave").click($.proxy(this.download, this));
    quickTools.find(".quickPrint").click($.proxy(this.print, this));
    quickTools.find(".quickZoomIn").click($.proxy(function () { this.zoom(1); }, this));
    quickTools.find(".quickZoomOut").click($.proxy(function () { this.zoom(-1); }, this));

    // Create the offscreen canvas for image editing
    this._offCanvas = document.createElement("canvas");

    this._mousebutton = [false, false, false];
    this._mousePos    = { x: 0, y: 0 };
}

Canvas.prototype =
{
    zoom: function (steps) {
        /// <summary>Changes the zoom level of the fundus image</summary>
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
        
        this._fundusImage.setPosition(0, 0);
        this._fundusImage.setZoom(s);

        this.draw();
    },

    drawCached: function () {
        /// <summary>Updates the cached image data on the offscreen canvas</summary>

        // Acquire a handle to the canvas context
        var ctx = this._offCanvas.getContext("2d");

        ctx.drawImage(this._fundusImage.baseImage, 0, 0);

        // Perform the image processing / annotation / effects
        var img = ctx.getImageData(0, 0, this._offCanvas.width, this._offCanvas.height);
        var pix = img.data;
        FundusWeb.Image.grayscale(pix);
        FundusWeb.Image.windowLevel(pix, this._fundusImage._window, this._fundusImage._level);
        ctx.putImageData(img, 0, 0);
    },

    draw: function () {
        /// <summary>Redraws the image on the visible canvas element</summary>

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
        ctx.drawImage(this._offCanvas, x, y);
        ctx.restore();
    },

    setImage: function (image) {
        /// <summary>Sets the image displayed in the canvas</summary>
        /// <param name="image" type="FundusImage"></param>

        $(this._fundusImage).unbind('.Canvas');

        this._fundusImage = image;

        var canvas = this;

        // Canvas interaction which does not modify the base image data (viewing)
        $(this._fundusImage).bind('onSegLoad.Canvas', function () { canvas.draw(); });
        $(this._fundusImage).bind('positionChanged.Canvas', function () { canvas.draw(); });
        $(this._fundusImage).bind('zoomChanged.Canvas', function () { canvas.draw(); });

        // Canvas interaction which modifies the base image data (editing)
        $(this._fundusImage).bind('windowLevelChanged.Canvas', function () { canvas.drawCached(); canvas.draw(); });

        // Load the initial image dimensions into the canvas when first available
        var initialDraw = function () {
            canvas._offCanvas.width = canvas._fundusImage.baseImage.width;
            canvas._offCanvas.height = canvas._fundusImage.baseImage.height;
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
        popup.document.write(this._offCanvas.toDataURL("image/png"));
        popup.print();
    },

    getImage: function () {
        return this._fundusImage;
    },

// Private:

    _onMouseMove: function (e) {
        /// <param name="e" type="JQuery.Event">event</param>

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

    _drawLine: function () {

    },

    // Private:
    _mousebutton: [],           /// <field name='_mousebutton' type='Array'>Tracking array for mouse button status</field>
    _mousePos: { x: 0, y: 0 },  /// <field name='_mousePos'>The mouse position for the mose recent event</field>
    _fundusImage: null,         /// <field name='_fundusImage' type='FundusImage'>The image currently in this canvas</field>
    _canvasElem: null,          /// <field name='_canvasElem' type=''>The HTML canvas elemented associated with this object</field>
    _offCanvas: null,           /// <field name='_offCanvas' type=''>An offscreen canvas for image editing</field>
}