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
    },

    add: function (image, index, active, suppressHistory) {
        /// <summary>Adds a new fundus image to the image bar</summary>
        /// <param name="image" type="FundusImage">The image to be added</param>
        /// <param name="index" type="Number">The index of insertion</param>
        /// <param name="active" type="Boolean">If true, the image is displayed in the view</param>
        /// <param name="suppressHistory" type="Boolean">If true, action will not be added to the history</param>

        var newIndex;
        if (index == undefined) newIndex = this._images.length;
        else newIndex = Math.min(this._images.length, index);

        // Create the HTML element for the image bar
        var imageElem = image.createUiElement();
        if (newIndex >= this._images.length) $("#imagePane").append(imageElem);
        else $("#imagePane").children(":eq(" + (index-1) + ")").after(imageElem);

        this._images.splice(newIndex, 0, image);

        // Present this image in the current view
        var isActive = (active || active == undefined);
        if (isActive) WebPage.canvas.setImage(image);

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(image, newIndex, false, isActive));
    },

    remove: function (id) {
        /// <summary>Removes an image from the bar by its id</summary>
        /// <param name="image" type="String">The id of the image to remove</param>

        var index = null;
        for (var i = 0; i < this._images.length; i++) {
            if (this._images[i].id == id) { index = i; break; }
        }
        if (index == null) return;

        this.removeByIndex(index);
    },

    removeByIndex: function (index, suppressHistory) {
        /// <summary>Removes an image from the bar by its index</summary>
        /// <param name="index" type="Number">The index of insertion</param>
        /// <param name="suppressHistory" type="Boolean">If true, not action will be added to the history</param>

        if (index > this._images.length) return;

        var image = this._images[index];

        var active = false;
        if (WebPage.canvas.getImage() == image) {
            WebPage.canvas.setImage(null);
            active = true;
        }

        $(image).unbind(".ImageBar");

        $("#fundus-" + image.id).detach();
        this._images.splice(index, 1);

        if (!suppressHistory) WebPage.history.push(new AddRemImageAction(
            image, index, true, active));
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
function AddRemImageAction(image, index, isRemove, active) {
    this._index  = index;
    this._image  = image;
    this._active = active;
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
        WebPage.imageBar.add(this._image, this._index, this._active, true);
    },

    _active: false, /// <field name='_index' type='Boolean'>True if the image is in the view</field>
    _index: 0,      /// <field name='_index' type='Number'>The index of insertion</field>
    _image: null,   /// <field name='_image' type='FundusImage'>The image</field>
}