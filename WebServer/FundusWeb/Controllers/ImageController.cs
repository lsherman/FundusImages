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

        public FundusImage GetById(int id)
        {
            // :TODO: Implement WebService endpoint for Image Segmentation

            if (id == 0)
            {
                throw new HttpResponseException(HttpStatusCode.NotFound);
            }
            return new FundusImage { Id = rnd1.Next() };
        }
    }
}