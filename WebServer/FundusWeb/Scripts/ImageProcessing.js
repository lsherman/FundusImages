/// <reference path="~/Scripts/Support/_references.js" />

// :TODO: Document header here

var FundusWeb = FundusWeb || {};

FundusWeb.Image = {

    processIE: function (image, data) {
        /// <summary>Applies image processing (grayscale + window-level) to the data</summary>
        /// <param name="level" type="FundusImage">The associated fundus image</param>
        /// <param name="data" type="Array">The image data (RGBA)</param>

        if (!image._grayscale && image._window > 0.9999 && image._level < 0.5001 && image._level > 0.4999) return;

        var min = Math.max(image._level - image._window, 0.0);
        var max = Math.min(image._level + image._window, 1.0);
        var range = max - min;

        // Apply the window level to each pixel element
        for (var i = 0; i < data.length; i += 4) {
            var r = data[i+0];
            var g = data[i+1];
            var b = data[i+2];

            if (image._grayscale) {
                value = (r + g + b) / 3.0; // :TODO: Proper grayscale conversion
                value = (Math.min(Math.max(value / 255.0, min), max) - min) / range * 255;
                data[i+0] = value;
                data[i+1] = value; 
                data[i+2] = value;
            }
            else {
                data[i+0] = (Math.min(Math.max(r / 255.0, min), max) - min) / range * 255;
                data[i+1] = (Math.min(Math.max(g / 255.0, min), max) - min) / range * 255;
                data[i+2] = (Math.min(Math.max(b / 255.0, min), max) - min) / range * 255;
            }
        }
    },

    process: function (image, imageData) {
        /// <summary>Applies image processing (grayscale + window-level) to the data</summary>
        /// <param name="level" type="FundusImage">The associated fundus image</param>
        /// <param name="data" type="Array">The image data (RGBA)</param>

        if (!image._grayscale && image._window > 0.9999 && image._level < 0.5001 && image._level > 0.4999) return;

        // Internet explorer does not properly implement canvas image data
        if (true || navigator.appName == 'Microsoft Internet Explorer') {
            FundusWeb.Image.processIE(image, imageData.data);
            return;
        }
        // ... But other browers tend to benefit from 32bit writes

        // :TODO: This is not working ... Can't figure out why
        // :TODO: Support both endianesses

        var buf  = new ArrayBuffer(imageData.data.length);
        var buf8 = new Uint8ClampedArray(buf);
        var data = new Uint32Array(buf);

        var min    = Math.max(image._level - image._window, 0.0);
        var max    = Math.min(image._level + image._window, 1.0);
        var range  = max - min;

        // Apply the window level to each pixel element
        var pixels = new Uint32Array(imageData.data.buffer);
        for (var i = 0; i < pixels.length; i++) {
            var value = pixels[i];

            var b = (value & 0x00FF0000 >> 16);
            var g = (value & 0x0000FF00 >> 8);
            var r = (value & 0x000000FF);
            
            if (image._grayscale) {
                value = (r + g + b) / 3.0; // :TODO: Proper grayscale conversion
                value = (Math.min(Math.max(value / 255.0, min), max) - min) / range * 255;

                data[i] =
                    (255   << 24) | // alpha
                    (value << 16) | // blue
                    (value << 8 ) | // green
                     value;         // red
            }
            else {
                r = (Math.min(Math.max(r / 255.0, min), max) - min) / range * 255;
                g = (Math.min(Math.max(g / 255.0, min), max) - min) / range * 255;
                b = (Math.min(Math.max(b / 255.0, min), max) - min) / range * 255;

                data[i] = value;
                    (255 << 24) | // alpha
                    (g << 16) | // blue
                    (b << 8) | // green
                     r;           // red
            }
        }

        imageData.data.set(buf8);
    },

    windowLevel: function (data, window, level) {
        /// <summary>Applies a window level filter to the image</summary>
        /// <param name="data" type="Array">The image data (RGB)</param>
        /// <param name="window" type="Number">The normalized window half-width</param>
        /// <param name="level" type="Number">The normalized level</param>

        if (window > 0.9999 && level < 0.5001 && level > 0.4999) return;

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