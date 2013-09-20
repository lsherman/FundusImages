/// <reference path="~/Scripts/jquery-2.0.3.js" />
/// <reference path="~/Scripts/Page.js" />

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

        var newIndex;
        if (index == undefined) newIndex = this._images.length;
        else newIndex = Math.min(this._images.length, index);
        this._images.splice(newIndex, 0, image);

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(image, newIndex));

        // :TODO: Make the image current/active
    },

    removeByIndex: function (index, suppressHistory) {
        /// <summary>Removes an image from the bar by its ID</summary>

        // :TODO: Check if the image is active/current

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(
            this._images.splice(index, 1)[0], index, true));
    },

    getState: function () {
        /// <summary>Returns the compressed state of the image bar</summary>

        return null;
    },

    reset: function () {
        /// <summary>Resets the image bar to its default state</summary>
    },

    restore: function (state) {
        /// <summary>Restores the image bar from a compressed state</summary>
    },

    // Private:

    _$element: null, /// <field name='_$element' type=''>JQuery handle to the image bars DOM element</field>
    _images: null,   /// <field name='_images' type='Array'>Images loaded into the application</field>
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