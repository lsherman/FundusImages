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
            // Store the image file locally for segmentation operations
            // **
            var appdir = HttpContext.Current.Server.MapPath("~");
            var ibytes = Convert.FromBase64String(data);
            var ifile = appdir + "filename." + mime.Replace("image/", "");
            try
            {
                File.WriteAllBytes(ifile, ibytes);
            }
            catch (DirectoryNotFoundException e)
            {
                var i = 0;
            }
            catch (UnauthorizedAccessException e)
            {
                var i = 0;
            }

            // **
            // Execute the segmentation process on the image file
            // **
            // :NOTE: For debug, set Window=true, Shell=true, WindowStyle=Normal
            //        else Window=false, Shell=false, WindowStyle=Hidden
            // :NOTE: In any case this is temp until we do a direct call to DLL
            //        from within the server thread.
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = true;
            startInfo.WorkingDirectory = appdir;
            startInfo.FileName = appdir + "Vasc_detection_rework_DRIVE_classify_DRIVE_trn.exe";
            startInfo.WindowStyle = ProcessWindowStyle.Normal;
            startInfo.Arguments = ifile;

            try
            {
                using (Process exeProcess = Process.Start(startInfo)) { exeProcess.WaitForExit(); }
            }
            catch
            {
                // Log error.

                return new FundusImage { id = image.id, data = "" }; 
            }

            // **
            // Load the output file from the segmentation 
            // **
            var ofile      = appdir + "Data\\DRIVE segmented\\01_test.tif";
            var obytes     = File.ReadAllBytes(ofile);
            var ob64String = Convert.ToBase64String(obytes);
            var dataUrl    = "data:image/tiff"
                //+ Path.GetExtension(ofile).Replace(".", "")
                + ";base64," 
                + ob64String;

            return new FundusImage { id = image.id, data = dataUrl };
        }
    }
}