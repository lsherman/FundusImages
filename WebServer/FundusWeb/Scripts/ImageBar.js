/// <reference path="~/Scripts/Support/_references.js" />
/// <reference path="~/Scripts/Page.js" />
/// <reference path="~/Scripts/FundusImage.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Manages the sortable list of uploaded images in the side panel
// ----------------------------------------------------------------------------
function ImageBar() {
    this.init();
};

ImageBar.prototype =
{
    init: function () {
        /// <summary>Initializes the application's image bar</summary>

        this._$element = $("#imageBar");
        this._images   = [];

        // Enable sorting within the image pane
        this._$element.find("#imagePane").sortable({
            containment: "parent"
        });
    },

    add: function (image, index, suppressHistory) {
        /// <summary>Adds a new fundus image to the image bar</summary>
        /// <param name="image" type="FundusImage">The image to be added</param>
        /// <param name="index" type="Number">The index of insertion</param>
        /// <param name="suppressHistory" type="Boolean">If true, not action will be added to the history</param>

        var newIndex;
        if (index == undefined) newIndex = this._images.length;
        else newIndex = Math.min(this._images.length, index);
        this._images.splice(newIndex, 0, image);

        // Create the HTML element for the image bar
        var img = document.createElement("img");
        $(img).css("width", "100%")
        // :TODO: Set loading src
        $("#imagePane").append(img);
        
        // Set the image for the thumbnail
        if (image.baseImage == null) {
            $(image).bind("onBaseLoad.ImageBar", function (event, data) {
                img.src = data.imgSrc;
                $(image).unbind("onBaseLoad.ImageBar");
            });
        }
        else img.src = image.baseImage;

        // :DEBUG: 
        $(image).bind("onSegLoad.ImageBar", function (event, data) {
            $('#webServiceTarget').text(data.imgSrc);
            $(image).unbind("onSegLoad.ImageBar");
        });
        $(image).bind("onSegError.ImageBar", function () {
            $('#webServiceTarget').text('Error: ' + err);
            $(image).unbind("onSegError.ImageBar");
        });

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(image, newIndex));

        // :TODO: Make the image current/active
    },

    removeByIndex: function (index, suppressHistory) {
        /// <summary>Removes an image from the bar by its ID</summary>
        /// <param name="index" type="Number">The index of insertion</param>
        /// <param name="suppressHistory" type="Boolean">If true, not action will be added to the history</param>
        
        // :TODO: Check if the image is active/current

        if (index > this._images.length) return;

        var image = this._images[index];

        $(image).unbind(".ImageBar");

        $("#imagePane").children().slice(index).detach();
        this._images.splice(index, 1);

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(
            this._images.splice(index, 1)[0], index, true));
    },

    getState: function () {
        /// <summary>Returns the compressed state of the image bar</summary>

        return { images: this._images };
    },

    reset: function () {
        /// <summary>Resets the image bar to its default state</summary>

        $("#imagePane").empty();
        this._images = [];
    },

    restore: function (state) {
        /// <summary>Restores the image bar from a compressed state</summary>

        for (var i = 0; i < state.images.length; i++)
            this.add(state.images[i], i, true);
    },

    // Private:

    _$element: null, /// <field name='_$element' type=''>JQuery handle to the image bars DOM element</field>
    _images:   null, /// <field name='_images' type='Array'>Images loaded into the application</field>
};

// ----------------------------------------------------------------------------
//  Action for adding an image to the application image bar
// ----------------------------------------------------------------------------
function AddRemImageAction(image, index, isRemove) {
    this._index = index;
    this._image = image;
    this.undo = isRemove ? this._add : this._remove;
    this.redo = isRemove ? this._remove : this._add;
}

AddRemImageAction.prototype =
{
    undo: null,
    redo: null,
    text: "Loading a new image",

    // Private:

    _remove: function () {
        WebPage.imageBar.removeByIndex(this._index, true);
    },

    _add: function () {
        WebPage.imageBar.add(this._image, this._index, true);
    },

    _index: null, /// <field name='_index' type='Number'>The index of insertion</field>
    _image: null, /// <field name='_image' type='FundusImage'>The image</field>
}