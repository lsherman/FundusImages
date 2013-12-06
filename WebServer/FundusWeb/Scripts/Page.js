/// <reference path="~/Scripts/Support/_references.js" />
/// <reference path="~/Scripts/ActionHistory.js" />
/// <reference path="~/Scripts/Canvas.js" />
/// <reference path="~/Scripts/ContextMenu.js" />
/// <reference path="~/Scripts/FundusImage.js" />
/// <reference path="~/Scripts/ImageBar.js" />
/// <reference path="~/Scripts/ToolBar.js" />
/// <reference path="~/Scripts/Message.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Master object which manages the elements of the main page
// ----------------------------------------------------------------------------
function Page() {
    // Check for the necessary File API support
    if (window.File && window.FileReader && window.FileList && window.Blob) {
        // File APIs are supported.
    } else {
        alert('The File APIs are not supported in this browser.');
    }

    // Check for Print API support
    if (window.print) {
        // Print API is supported.
    } else {
        alert('The Print API is not supported in this browser.');
    }

    $(document).disableSelection();
    document.onselectstart = function () { return false; } // ie
    document.onmousedown   = function () { return false; } // others
};

Page.prototype =
{
    init: function () {
        /// <summary>Initializes the web page for the application</summary>

        // Use a page-leave message to alert potential data loss
        window.onbeforeunload = function () {
            return 'Any unsaved data will be lost';
        };

        // Hook the mouse wheel to the image zoom functionality
        window.onmousewheel = $.proxy(this._onMouseWheel, this);

        // Hook the resize event to the page
        window.onresize = $.proxy(this._onResize, this);

        // Configure the undo/redo/reset buttons and the action history
        this.history = new ActionHistory();
        $("#undoButton").click(function () { WebPage.history.undo(); });
        $("#redoButton").click(function () { WebPage.history.redo(); });
        $("#resetButton").click(function () { WebPage.reset(); });
        $(document).keyup(function (event) {
            if (!event.ctrlKey) return;
            if (event.which == 89) WebPage.history.redo();
            if (event.which == 90) WebPage.history.undo();
        });
        $(this.history).bind("onAction", this._onAction);
        this._onAction(); // Initialize state
        
        // Configure the image bar
        this.imageBar    = new ImageBar();
        this.toolBar     = new ToolBar();
        this.contextMenu = new ContextMenu();

        // Configure drag'n'drop image upload for the page
        $("#page")[0].addEventListener("dragover", function (e) { e.preventDefault(); }, false);
        $("#page")[0].addEventListener("drop", function (e) {
            e.stopPropagation();
            var files = e.dataTransfer.files;
            for (var i = 0; i < files.length; i++) {
                var id = WebPage.generateUID();
                WebPage.imageBar.add(new FundusImage(id, files[i]));
            }
        }, false);

        // Configure the upload control panel elements
        $("#uploadButton").click(function () { $("#files").click(); });
        $("#files").change(function () {
            var files = document.getElementById("files").files;
            if (files.length != 0) {
                var id = WebPage.generateUID();
                WebPage.imageBar.add(new FundusImage(id, files[0]));
            }
        });

        // Configure the canvas elements
        var canvasElem = document.getElementById("canvas");
        this.canvas = new Canvas(canvasElem);

        // Configure the image/tool bar toggles
        $("#tabTool").on("click", $.proxy(function () { this._setActivePane('tools'); }, this));
        $("#tabImage").on("click", $.proxy(function () { this._setActivePane('image'); }, this));

        // Apply the initial image view layout 
        this.applyLayout();
    },

    applyLayout: function () {
        /// <summary>Applies the currently selected layout configuration</summary>

        // Acquire the canvas handle
        var canvas = $("#canvas");
        var layout = $("#imageView");
        var width  = layout.width();
        var height = layout.height();
        canvas.css('width', width);
        canvas.css('height', height);
        canvas.attr('width', width);
        canvas.attr('height', height);

        // Redraw the image
        WebPage.canvas.draw();
    },

    getState: function () {
        /// <summary>Acquires a  plain JSON representation of the application state</summary>

        var state = {};

        state.imageBarState = this.imageBar.getState();

        return state;
    },

    reset: function (suppressHistory) {
        /// <summary>Resets the page to its default state</summary>
        /// <param name="action" type="Boolean">If true, an action will not be added to the history</param>

        if (!suppressHistory) {
            var state = this.getState();
            this.history.push(new ResetAction(state));
        }

        this.imageBar.reset();

        this.canvas.setImage(null);
    },

    restore: function (state) {
        /// <summary>Restores the page to a given state</summary>
        /// <param name="state">A plain JSON object describing the application state</param>

        this.imageBar.restore(state.imageBarState);
    },

    generateUID: function () {
        /// <summary>Generates a UID</summary>

        return this._uidGen++;
    },

    // Public:

    history: null,  /// <field name='history' type='ActionHistory'>The action history</field>
    imageBar: null, /// <field name='imageBar' type='ImageBar'>Manages load fundus images</field>
    canvas: null,   /// <field name='canvas' type='Canvas'>Canvas used for image display/interaction</field>
    toolBar: null,  /// <field name='toolBar' type='ToolBar'>Toolbar for fundus image editing</field>

    // Private:

    _setActivePane: function (pane) {
        /// <summary>Sets the active window on the side pane, or hides if null</summary>

        if (pane == this._currPane) return;

        if (pane == 'tools') {
            $("#imagePane").hide();
            $("#toolsPane").show();
        }
        else if (pane == 'image') {
            $("#imagePane").show();
            $("#toolsPane").hide();
        }
        else { // Hide
        }

        this._currPane = pane;
    },

    _onResize: function () {
        /// <summary>Updates the page on a resize event</summary>

        this.applyLayout();
    },

    _onMouseWheel: function (e) {
        /// <summary>Redirects the mouse wheel from browser default to image zoom</summary>

        e.preventDefault();
        var image = WebPage.canvas.getImage();
        if (image) {
            var delta = e.wheelDelta;
            image.zoom(delta);
        }
    },

    _onAction: function () {
        /// <summary>Updates the history buttons when an action occurs</summary>

        var undoText = WebPage.history.nextUndo();
        if (undoText === undefined) {
            $("#undoButton").attr("title", "The previous action cannot be reversed");
            $("#undoButton").attr("disabled", true);
        } else {
            $("#undoButton").attr("title", "Reverses the previous action: '" + undoText + "'");
            $("#undoButton").attr("disabled", false);
        }

        var redoText = WebPage.history.nextRedo();
        if (redoText === undefined) {
            $("#redoButton").attr("title", "There is no next action to apply");
            $("#redoButton").attr("disabled", true);
        } else {
            $("#redoButton").attr("title", "Performs the next action: '" + redoText + "'");
            $("#redoButton").attr("disabled", false);
        }
    },

    _currPane: 'image', /// <field name='_currPane' type='String'>Active tool pane</field>
    _uidGen: 0,         /// <field name='_uidGen' type='Number'>Key for generating UIDs</field>
};

// ----------------------------------------------------------------------------
//  Action for resetting/restoring the application state
// ----------------------------------------------------------------------------
function ResetAction(state) {
    this._state = state;
}

ResetAction.prototype =
{
    undo: function () {
        WebPage.restore(this._state);
        this._state = null;
    },

    redo: function () {
        this._state = WebPage.getState();
        WebPage.reset(true);
    },

    text: "Resetting the page",

    _state: null, /// <field name='_state' type='JSON'>The previous application state</field>
}