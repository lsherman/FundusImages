/// <reference path="~/Scripts/Support/_references.js" />

// :TODO: Document header here

var FundusWeb = FundusWeb || {};

FundusWeb.Image = {

    windowLevel: function (data, window, level) {
        /// <summary>Applies a window level filter to the image</summary>
        /// <param name="data" type="Array">The image data (RGBA)</param>
        /// <param name="window" type="Number">The normalized window half-width</param>
        /// <param name="level" type="Number">The normalized level</param>

        var min   = Math.max(level - window, 0.0);
        var max   = Math.min(level + window, 1.0);
        var range = max - min;

        // Apply the window level to each pixel element
        for (var i = 0; i < data.length; ++i) {
            var normIn = data[i] / 255.0;
            var clip   = Math.min(Math.max(normIn, min), max);
            data[i]    = (clip - min) / range * 255;
        }

        return data;
    }

};