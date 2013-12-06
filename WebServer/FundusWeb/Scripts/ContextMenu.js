/// <reference path="~/Scripts/Support/_references.js" />
/// <reference path="~/Scripts/Canvas.js" />

// ----------------------------------------------------------------------------
//  A context menu for the canvas containing access to common tools
//  Different from quik-tools in that these are active tools set to the mouse
//  or touch motion events on the canvas.
// ----------------------------------------------------------------------------
function ContextMenu() {

    // Create the context menu
    $("#contextMenu").menu().hide();

    // Hook the context menu to the browser event
    $("#canvas").on('contextmenu', $.proxy(function (e) {
        WebPage.contextMenu.open(e);
        return false;
    }, this));

    // Context menu paint brush selection
    $("#ctxCursor").on('mousedown', function () { WebPage.canvas.setTool(CanvasTool.cursor); });
    $("#ctxBrush").on('mousedown', function () { WebPage.canvas.setTool(CanvasTool.brush); });
    $("#ctxText").on('mousedown', function () { WebPage.canvas.setTool(CanvasTool.text); });
    $("#ctxZoom").on('mousedown', function () { WebPage.canvas.setTool(CanvasTool.zoom); });

    // :TODO:

    // :TODO: unrelated to context but make mouse button 3 drag image
}

ContextMenu.prototype =
{
    open: function (e) {
        $("#contextMenu")
            .show()
            .position({ my: "left top", of: e });
        $(document).on('mousedown.Context', function (e) {
            //    e.stopPropagation();
            //    $(document).off('.Context');
            $("#contextMenu").hide();
        });
    }

    // Private:

}