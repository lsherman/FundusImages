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

    this._mousebutton = [false, false, false];
    this._mousePos    = { x: 0, y: 0 };
}

Canvas.prototype =
{
    draw: function () {
        /// <summary>Redraws the image on the canvas element</summary>

        var ctx = this._canvasElem.getContext("2d");

        var img = new Image();
        
        var paint = $.proxy(function () {
            ctx.fillStyle = "#000000";
            ctx.rect(0, 0, this._canvasElem.width, this._canvasElem.height);
            ctx.fill();
            ctx.drawImage(img, this._fundusImage._offset.x, this._fundusImage._offset.y);
        }, this);

        if (this._fundusImage.segImage == null) {
            $(this).bind("onSegLoad.ImageBar", function (event, data) {
                img.src = this._fundusImage.baseImage;
                $(this).unbind("onSegLoad.ImageBar");
            });
            paint();
        }
        else img.src = this._fundusImage.baseImage;

        paint();

        // Acquire the canvas image data
        var img = ctx.getImageData(0, 0, this._canvasElem.width, this._canvasElem.height);
        var pix = img.data;

        // Apply the window level to the base fundus image
        FundusWeb.Image.windowLevel(pix, this._fundusImage._window, this._fundusImage._level);

        // Replace the image data
        ctx.putImageData(img, 0, 0);
    },

    setImage: function (image) {
        /// <summary>Sets the image displayed in the canvas</summary>
        /// <param name="image" type="FundusImage"></param>

        $(this._fundusImage).unbind();

        this._fundusImage = image;

        var canvas = this;
        $(this._fundusImage).bind('positionChanged', function () { canvas.draw(); });
        $(this._fundusImage).bind('windowLevelChanged', function () { canvas.draw(); });

        this.draw();
    },

    download: function () {
        /// <summary>Downloads the annotated image to the client</summary>

        var iframeId = 'hiddenDownloader';
        var iframe   = document.getElementById(hiddenIFrameID);
        if (iframe === null) {
            iframe = document.createElement('iframe');
            iframe.id = hiddenIFrameID;
            iframe.style.display = 'none';
            document.body.appendChild(iframe);
        }
        iframe.src = this._canvasElem.toDataURL();
    },

// Private:

    _onMouseMove: function (e) {
        /// <param name="e" type="JQuery.Event">event</param>

        if (!this._fundusImage) return;

        var moveX = this._mousePos.x - e.screenX;
        var moveY = this._mousePos.y - e.screenY;

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
    
}