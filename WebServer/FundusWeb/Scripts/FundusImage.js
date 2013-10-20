/// <reference path="~/Scripts/Support/_references.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Container for fundus images which includes metadata, etc for management
// ----------------------------------------------------------------------------
function FundusImage(id, file) {

    this.file = file;
    this.id   = id;

    // Load the base image, then submit the segmentation request
    var reader = new FileReader();
    reader.onloadend = $.proxy(function () {
        this.baseImage = reader.result;
        $(this).trigger("onBaseLoad", { imgSrc: this.baseImage });

        // Issue the segmentation request to the server
        $.post('api/image/segment',
            { id: this.id, data: this.baseImage },
            $.proxy(function (data, textStatus, jqXHR) {
                this.segImage = data.data;
                WebPage.canvas.setImage(this);
                $(this).trigger("onSegLoad", { imgSrc: this.segImage });
            }, this), "json")
            .fail(function (jqXHR, textStatus, err) {
                $(this).trigger("onSegError", err);
            });
    }, this);
    reader.readAsDataURL(file);
}

FundusImage.prototype =
{
    createUiElement: function () {
        /// <summary>Creates an HTML list element for the image bar</summary>

        // Create the HTML element for the image bar
        var img = $("<img draggable='true'></img>");
        $(img).css("width", "100%")
        $(img).data("Image", this);

        // Display the image when clicked
        $(img).click($.proxy(function () {
            // Check if this is already the current image

            WebPage.canvas.setImage(this);

            //WebPage.history.push();
        }, this));

        // Set the image for the thumbnail
        if (this.baseImage == null) {
            $(this).bind("onBaseLoad.ImageBar", function (event, data) {
                img.attr('src', data.imgSrc);
                $(this).unbind("onBaseLoad.ImageBar");
            });
        }
        else img.attr('src', this.baseImage);

        // :DEBUG: 
        $(this).bind("onSegLoad.ImageBar", function (event, data) {
            $('#webServiceTarget').text(data.imgSrc);
            $(this).unbind("onSegLoad.ImageBar");
        });

        // :TODO:
        $(this).bind("onSegError.ImageBar", function () {
            $('#webServiceTarget').text('Error: ' + err);
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
    _zoomLevel: 1.0,
    _offset: { x: 0, y: 0 },
    _annotations: [],    /// <field name='_annotations' type='Array'>List of annotations on the image</field>
}