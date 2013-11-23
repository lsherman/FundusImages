using FundusWeb.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Web.Http;
using System.IO;
using System.Text.RegularExpressions;
using System.Diagnostics;
using System.Web;
using System.Runtime.InteropServices;

namespace FundusWeb.Controllers
{
    public class ImageController : ApiController
    {
        Random rnd1 = new Random();

        public FundusImage Segment(FundusImage image)
        {
            // **
            // Parse the data URL for the information necessary for conversion
            // http://stackoverflow.com/questions/5714281/regex-to-parse-image-data-uri-c
            // **
            var regex    = new Regex(@"data:(?<mime>[\w/]+);(?<encoding>\w+),(?<data>.*)", RegexOptions.Compiled);
            var match    = regex.Match(image.data);
            var mime     = match.Groups["mime"].Value;
            var encoding = match.Groups["encoding"].Value;
            var data     = match.Groups["data"].Value;

            // **
            // Call the segmentation service to segment the image
            // :TODO: Get ride of the marshalling here and just compile for unsafe?
            // **
            var appdir = HttpContext.Current.Server.MapPath("~");
            var ibytes = Convert.FromBase64String(data);

            IntPtr outStr = (IntPtr)0;
            IntPtr outLen = (IntPtr)0;
            IntPtr length = (IntPtr)ibytes.Length;
            FundusSegment.segmentImage(ibytes, length, ref outStr, ref outLen);
            byte[] obytes = new byte[(int)outLen];
            Marshal.Copy((IntPtr)outStr, obytes, 0, (int)outLen);

            var ob64String = Convert.ToBase64String(obytes);
            var dataUrl    = "data:image/jpeg"
                + ";base64," 
                + ob64String;

            FundusSegment.freeImage(outStr);

            return new FundusImage { id = image.id, data = dataUrl };
        }
    }
}