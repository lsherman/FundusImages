/// <reference path="~/Scripts/jquery-2.0.3.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Container for fundus images which includes metadata, etc for management
// ----------------------------------------------------------------------------
function FundusImage(image, file) {

    this.file = file;

    // :TODO: Launch segmentation request to webservers
}

FundusImage.prototype =
{
    _file:      null, /// <field name='_file'      type='File'>The source file for the image</field>
    _baseImage: null, /// <field name='_baseImage' type='File'>The loaded image element</field>
    _segImage:  null  /// <field name='_segImage'  type='File'>The segmented image element</field>
}