/// <reference path="~/Scripts/Support/_references.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Container for fundus images which includes metadata, etc for management
// ----------------------------------------------------------------------------
function FundusImage(id, file) {

    this.file = file;
    this.id   = id;

    this._annotations = [];
    this._offset      = { x: 0, y: 0 };

    // Load the base image, then submit the segmentation request
    var reader = new FileReader();
    reader.onloadend = $.proxy(function () {

        // Decode the image and extract statistics
        var img = new Image();
        img.src = reader.result;
        img.onload = $.proxy(function () {
            this.baseImage = img;
            $(this).trigger("onBaseLoad", { img: img });
        }, this);

        // Issue the segmentation request to the server
        $.post('api/image/segment',
            { id: this.id, data: reader.result },
            $.proxy(function (data, textStatus, jqXHR) {
                // Decode the image and extract statistics
                var img = new Image();
                img.src = data.data;
                img.onload = $.proxy(function () {
                    this.segImage = img;
                    $(this).trigger("onSegLoad", { imgSrc: this.segImage });
                }, this);
            }, this), "json")
            .fail(function (jqXHR, textStatus, err) {
                $(this).trigger("onSegError", err);
            });
    }, this);
    reader.readAsDataURL(file);
}

FundusImage.prototype =
{
    showBase: function (show, suppressHistory) {
        if (this._showBase == show) return;
        this._showBase = show;
        if (!suppressHistory) WebPage.history.push(new ToggleLayerVisibility(this, 'base'));
        $(this).trigger('displayChanged');
    },

    showSegmented: function (show, suppressHistory) {
        if (this._showSegment == show) return;
        this._showSegment = show;
        if (!suppressHistory) WebPage.history.push(new ToggleLayerVisibility(this, 'segmentation'));
        $(this).trigger('displayChanged');
    },

    showAnnotated: function (show, suppressHistory) {
        if (this._showAnnotate == show) return;
        this._showAnnotate = show;
        if (!suppressHistory) WebPage.history.push(new ToggleLayerVisibility(this, 'annotation'));
        $(this).trigger('displayChanged');
    },

    setPosition: function (x, y) {
        /// <summary>Changes the position of the image</summary>
        this._offset.x = x;
        this._offset.y = y;
        $(this).trigger('positionChanged', { image: this });
    },

    move: function (xDist, yDist) {
        /// <summary>Changes the position of the image</summary>
        this._offset.x += xDist;
        this._offset.y += yDist;
        $(this).trigger('positionChanged', { image: this });
    },

    zoom: function (steps) {
        /// <summary>Changes the zoomlevel of the image</summary>
        var scale = this._zoomLevel + steps / 100.0;
        scale = Math.min(100, Math.max(0.1, scale));
        this.setZoom(scale);
    },

    setZoom: function (scale) {
        /// <summary>Changes the zoomlevel of the image</summary>
        this._zoomLevel = scale;
        $(this).trigger('zoomChanged', { image: this });
    },

    setWindowLevel: function (window, level) {
        /// <summary>Changes the window/level of the image</summary>
        this._window = Math.max(Math.min(window, 1.0), 0.0);
        this._level  = Math.max(Math.min(level, 1.0), 0.0);
        $(this).trigger('dataChanged', { image: this });
    },

    setGrayscale: function (set, suppressHistory) {
        /// <summary>Sets the grayscale option for the fundus image</summary>
        if (set == this._grayscale) return;
        this._grayscale = set;
        $(this).trigger('dataChanged', { image: this });
        if (!suppressHistory) WebPage.history.push(new ToggleGrayscaleAction(this));
    },

    createUiElement: function () {
        /// <summary>Creates an HTML list element for the image bar</summary>

        var image = this;
        var id    = "fundusRadio" + this.id;

        // Create the HTML element for the image bar
        var elem = $("<div id='fundus-" + image.id + "' style='width:100%; margin-bottom:5px;'>" +
                        "<img draggable='false' class='image-thumb'></img>" +
                        "<div style='height:32px; width:80%; left:10%; position:relative;'>" + 
                           "<div style='float:right'; class='image-rem'>X</div>" +
                        "</div>" +
                     "</div>"
                     );

        // Setup the image to trigger display in the canvas
        var img = elem.find('.image-thumb');

        // Display the image when clicked
        img.click($.proxy(function () {
            // Check if this is already the current image
            if (WebPage.canvas.getImage != this) {
                WebPage.canvas.setImage(this);
            }
        }, this));

        // Remove image button
        elem.find('.image-rem').click(function () {
            WebPage.imageBar.remove(image.id);
        });

        // Set the image for the thumbnail
        if (this.baseImage == null) {
            $(this).bind("onBaseLoad.ImageBar", function (event, data) {
                img.attr('src', this.baseImage.src);
                $(this).unbind("onBaseLoad.ImageBar");
            });
        }
        else img.attr('src', this.baseImage.src);

        // :DEBUG: 
        $(this).bind("onSegLoad.ImageBar", function (event, data) {
            // HAS IMAGE
            $(this).unbind("onSegLoad.ImageBar");
        });

        // :TODO:
        $(this).bind("onSegError.ImageBar", function () {
            // Message("");
            $(this).unbind("onSegError.ImageBar");
        });

        return elem;
    },

    id:        0,    /// <field name='id'        type='Number'>Unique identifier</field>
    file:      null, /// <field name='file'      type='File'>The source file for the image</field>
    baseImage: null, /// <field name='baseImage' type='URI'>The original image dataURI, or null if unloaded</field>
    segImage:  null, /// <field name='segImage'  type='URI'>The segmented image dataURI, or null if unloaded</field>

    // *** Display Parameters ***
    _offset: { x: 0, y: 0 },
    _window: 0.5,         /// <field name='_window' type='Array'>Window size for the image</field>
    _level: 0.5,          /// <field name='_level' type='Number'>Level value for the image</field>
    _zoomLevel: 1.0,      /// <field name='_zoomLevel' type='Number'>Zoom scale for the image</field>
    _grayscale:    false, /// <field name='_grayscale' type='Boolean'>Grayscale toggle option for image display</field>
    _showAnnotate: true,  /// <field name='_showAnnotate' type='Boolean'>Annotation toggle option for image display</field>
    _showSegment:  true,  /// <field name='_showSegment' type='Boolean'>Segmentation toggle option for image display</field>
    _showBase:     true,  /// <field name='_showBase' type='Boolean'>Base image toggle option for image display</field>
}

// ----------------------------------------------------------------------------
//  Action for toggling the image grayscale mode
// ----------------------------------------------------------------------------
function ToggleGrayscaleAction(image) {
    this._image = image;
}

ToggleGrayscaleAction.prototype =
{
    text: "changing the grayscale mode",

    // Private:

    undo: function () {
        this._image.setGrayscale(!this._image._grayscale, true);
    },

    redo: function () {
        this._image.setGrayscale(!this._image._grayscale, true);
    },

    _image: null, /// <field name='_index' type='FundusImage'>The associated fundus image</field>
}

// ----------------------------------------------------------------------------
//  Action for toggling the image layer visibility
// ----------------------------------------------------------------------------
function ToggleLayerVisibility(image, layer) {
    this._image = image;
    this._layer = layer;
    this.redo   = this.undo;
    this.text   = "Changing the image's " + layer + " layer visibility";
}

ToggleLayerVisibility.prototype =
{
    text: null,

    // Private:

    undo: function () {
        switch (this._layer) {
            case 'segmentation':
                this._image.showSegmented(!this._image._showSegment, true);
                break;
            case 'annotation':
                this._image.showAnnotated(!this._image._showAnnotate, true);
                break;
            case 'base':
                this._image.showBase(!this._image._showBase, true);
                break;
        }
    },

    redo: null,

    _image: null, /// <field name='_index' type='FundusImage'>The associated fundus image</field>
}