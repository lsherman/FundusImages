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

        // Present this image in the current view
        WebPage.canvas.setImage(this);

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
        $(this).trigger('windowLevelChanged', { image: this });
    },

    createUiElement: function () {
        /// <summary>Creates an HTML list element for the image bar</summary>

        // Create the HTML element for the image bar
        var img = $("<img draggable='false' class='image-thumb'></img><div style='height:32px;'></div>");
        img.data("Image", this);

        // Display the image when clicked
        img.click($.proxy(function () {
            // Check if this is already the current image

            WebPage.canvas.setImage(this);

            //WebPage.history.push();
        }, this));

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

        return img;
    },

    id:        0,    /// <field name='id'         type='Number'>Unique identifier</field>
    file:      null, /// <field name='file'      type='File'>The source file for the image</field>
    baseImage: null, /// <field name='baseImage' type='URI'>The original image dataURI, or null if unloaded</field>
    segImage:  null, /// <field name='segImage'  type='URI'>The segmented image dataURI, or null if unloaded</field>

    // *** Display Parameters ***
    _window: 0.5,        /// <field name='_window' type='Array'>Window size for the image</field>
    _level: 0.5,         /// <field name='_level' type='Number'>Level value for the image</field>
    _zoomLevel: 1.0,     /// <field name='_zoomLevel' type='Number'>Zoom scale for the image</field>
    _offset: { x: 0, y: 0 },
    _annotations: [],    /// <field name='_annotations' type='Array'>List of annotations on the image</field>
}