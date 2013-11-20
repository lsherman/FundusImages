/// <reference path="~/Scripts/Support/_references.js" />
/// <reference path="~/Scripts/Page.js" />
/// <reference path="~/Scripts/FundusImage.js" />

// ----------------------------------------------------------------------------
//  Manages the editing and annotation tools for the fundus images
// ----------------------------------------------------------------------------
function ToolBar() {
    this.init();
};

ToolBar.prototype =
{
    init: function () {
        /// <summary>Initializes the application's image bar</summary>

        $("#toolsPane").hide();

        // Initialize the control element
        $("#toolGrayscale").button().change(function () {
            var checked = $("#toolGrayscale").attr('checked');
            WebPage.toolBar._image.setGrayscale(checked);
        });

        // Initialize the 
        $("#toolWindowLevel").slider({
            range: true,
            min: 0,
            max: 100,
            values: [0, 100],
            slide: function (event, ui) {
                var v = ui.values;
                var window = (v[1] - v[0]) / (100);
                var level  = (v[1] + v[0]) / (200);
                WebPage.toolBar._image.setWindowLevel(window, level);
            }
        });
        
        this._$element = $("#toolBar");
        this._image = null;
    },

    populate: function (fundusImage) {
        /// <summary>Populates the toolbar controls with the specified image's data</summary>

        this._image = fundusImage;

        if (this._image == null) { return; }
    },

    disable: function () {
        /// <summary>Disables the controls on the toolbar</summary>

    },

    // Private:

    _$element: null, /// <field name='_$element' type=''>JQuery handle to the image bars DOM element</field>
    _image: null,    /// <field name='_image' type='FundusImage'>Image currently associated with the tools</field>
};