/// <reference path="~/Scripts/_references.js" />

// :TODO: Document header here

// ----------------------------------------------------------------------------
//  Stack object which manages actions used for undo/redo functionality
// ----------------------------------------------------------------------------
function Action() {
}

Action.prototype =
{
    undo: function () {
        /// <summary>Reverses the effects of this action</summary>
        throw Error("Virtual function call");
    },

    redo: function () {
        /// <summary>Reapplies the effects of this action</summary>
        throw Error("Virtual function call");
    },

    text: ""
}

// ----------------------------------------------------------------------------
//  Stack object which manages actions used for undo/redo functionality
// ----------------------------------------------------------------------------
function ActionHistory() {
    this._undoStack = [];
    this._redoStack = [];
};

ActionHistory.prototype =
{
    push: function (action) {
        /// <summary>Appends a new action to the undo history</summary>
        /// <param name="action" type="Action">The new action to be added to the history</param>

        this._undoStack.push(action);

        if (this._undoStack.length > this._maxLength) this._undoStack.splice(0, 1);

        this._redoStack = [];

        $(this).trigger("onAction");
    },

    undo: function () {
        /// <summary>Undoes the last action and adds it to the redo stack</summary>

        if (this._undoStack.length == 0) return;

        var action = this._undoStack.pop();

        action.undo();

        this._redoStack.push(action);

        $(this).trigger("onAction");
    },

    redo: function () {
        // <summary>Redoes the last action and adds it to the undo stack</summary>

        if (this._redoStack.length == 0) return;

        var action = this._redoStack.pop();

        action.redo();

        this._undoStack.push(action);

        $(this).trigger("onAction");
    },

    nextRedo: function () {
        // <summary>Returns the text for the next action to be redone</summary>

        if (this._redoStack.length == 0) return;

        return this._redoStack[this._redoStack.length - 1].text;
    },

    nextUndo: function () {
        // <summary>Returns the text for the next action to be undone</summary>

        if (this._undoStack.length == 0) return;

        return this._undoStack[this._undoStack.length - 1].text;
    },

    // Private:

    _undoStack: [], /// <field name='_undoStack' type='Array'>The undo stack</field>
    _redoStack: [], /// <field name='_redoStack' type='Array'>The redo stack</field>
    _maxLength: 20, /// <field name='_maxLength' type='Number'>The maximum number of actions in the history</field>
};

