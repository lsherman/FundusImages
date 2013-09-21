/// <reference path="~/Scripts/jquery-2.0.3.js" />

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
    id:        0,    /// <field name='id'         type='Number'>Unique identifier</field>
    file:      null, /// <field name='_file'      type='File'>The source file for the image</field>
    baseImage: null, /// <field name='_baseImage' type='File'>The original image dataURI, or null if unloaded</field>
    segImage:  null, /// <field name='_segImage'  type='File'>The segmented image dataURI, or null if unloaded</field>
}