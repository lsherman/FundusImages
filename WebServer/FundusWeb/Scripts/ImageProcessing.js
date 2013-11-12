/// <reference path="~/Scripts/Support/_references.js" />

// :TODO: Document header here

var FundusWeb = FundusWeb || {};

FundusWeb.Image = {

    windowLevel: function (data, window, level) {
        /// <summary>Applies a window level filter to the image</summary>
        /// <param name="data" type="Array">The image data (RGB)</param>
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
    },

    grayscale: function (data) {
        /// <summary>Converts the image to an 8-bit grayscale format</summary>
        /// <param name="data" type="Array">The image data (RGB)</param>

        // Apply the conversion to each pixel element
        var pixels = data.length / 4;
        for (var i = 0; i < pixels; ++i) {
            var index = i * 4;
            var r = data[index+0];
            var g = data[index+1];
            var b = data[index+2];
            var gray = (r+g+b)/3;
            data[index+0] = gray;
            data[index+1] = gray;
            data[index+2] = gray;
        }

        return data;
    }
};