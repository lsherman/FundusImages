/// <reference path="~/Scripts/Support/_reference.js" />
/// <reference path="~/Scripts/ActionHistory.js" />
/// <reference path="~/Scripts/ImageBar.js" />
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
};

Page.prototype =
{
    init: function () {
        /// <summary>Initializes the web page for the application</summary>

        // Configure the undo/redo/reset buttons and the action history
        this.history = new ActionHistory();
        $("#undoButton").click(function () { WebPage.history.undo(); });
        $("#redoButton").click(function () { WebPage.history.redo(); });
        $("#resetButton").click(function () { WebPage.reset(); });
        $(document).on("keypress", function (event) {
            if (!event.ctrlKey) return;
            if (event.key == "z") WebPage.history.undo();
            if (event.key == "y") WebPage.history.redo();
        });
        $(this.history).bind("onAction", this._onAction);
        this._onAction(); // Initialize state
        
        // Configure the image bar
        this.imageBar = new ImageBar();

        // Configure drag'n'drop image upload for the page
        $("#page")[0].addEventListener("dragover", function (e) { e.preventDefault(); }, false);
        $("#page")[0].addEventListener("drop", function (e) {
            e.stopPropagation();
            var files = e.dataTransfer.files;
            for (var i = 0; i < files.length; i++) {
                WebPage.loadImageFile(files[i]);
            }
        }, false);

        // Configure the upload control panel elements
        $("#beginUploadButton").click(function () {
            var files = document.getElementById("files").files;
            if (files.length != 0) {
                WebPage.loadImageFile(files[0]);
            }
            else {
                // No image specified error
            }
        });
    },

    loadImageFile: function (file) {
        /// <summary>Attempts to load a fundus image file selected by the user</summary>

        //Message("Hellow World!");

        $.getJSON('api/image/1')
            .done(function (data) {
                $('#webServiceTarget').text(data.Id.toString());
            })
            .fail(function (jqXHR, textStatus, err) {
                $('#webServiceTarget').text('Error: ' + err);
            });

        var img = document.createElement("img");
        $(img).css("width", "100%")
        var reader = new FileReader();
        reader.onloadend = function () {
            img.src = reader.result;
        }
        reader.readAsDataURL(file);

        $("#imagePane").append(img);
        
        this.imageBar.add(null);

        // :DEBUG:
        console.log("Filename: " + file.name);
        console.log("Type: " + file.type);
        console.log("Size: " + file.size + " bytes");
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
    },

    restore: function (state) {
        /// <summary>Restores the page to a given state</summary>
        /// <param name="state">A plain JSON object describing the application state</param>

        this.imageBar.restore(state);
    },

    // Public:

    history:  null, /// <field name='history' type='ActionHistory'>The action history</field>
    imageBar: null, /// <field name='imageBar' type='ImageBar'>Manages load fundus images</field>

    // Private:

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
    }
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
    },

    redo: function () {
        WebPage.reset(true);
    },

    text: "Resetting the page",

    _state: null, /// <field name='_state' type='JSON'>The previous application state</field>
}