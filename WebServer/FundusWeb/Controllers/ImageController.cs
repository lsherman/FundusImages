using FundusWeb.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Web.Http;

namespace FundusWeb.Controllers
{
    public class ImageController : ApiController
    {
        Random rnd1 = new Random();

        public FundusImage Segment(FundusImage image)
        {
            // :TODO: Call segmentation DLL

            return new FundusImage { id = image.id, data = "Server Data for id: " + image.id.ToString() };
        }
    }
}