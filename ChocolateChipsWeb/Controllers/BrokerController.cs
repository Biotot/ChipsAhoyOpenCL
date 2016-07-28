using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

using System.Threading.Tasks;
using System.Data.Sql;
using System.Data.SqlClient;

namespace ChocolateChipsWeb.Controllers
{
    public class BrokerController : Controller
    {

        String m_ConnectionString = "Server=SHKBOX\\SQLEXPRESS;Database=DBOreos;User Id=Admin;Password=Admin;";
        SqlConnection m_Connection;
        public void LoadBrokers()
        {

            String aRet = "";
            String aSqlString = m_ConnectionString;
            using (SqlConnection aConnection = new SqlConnection(m_ConnectionString))
            {
                SqlCommand aCommand = new SqlCommand(aSqlString, aConnection);
                aCommand.Connection.Open();
                SqlDataReader aReader = aCommand.ExecuteReader();
            }
            //return View();
        }


        // GET: Broker
        public ActionResult Index()
        {
            return View();
        }

        // GET: Broker/Details/5
        public ActionResult Details(int id)
        {
            return View();
        }

        // GET: Broker/Create
        public ActionResult Create()
        {
            return View();
        }

        // POST: Broker/Create
        [HttpPost]
        public ActionResult Create(FormCollection collection)
        {
            try
            {
                // TODO: Add insert logic here

                return RedirectToAction("Index");
            }
            catch
            {
                return View();
            }
        }

        // GET: Broker/Edit/5
        public ActionResult Edit(int id)
        {
            return View();
        }

        // POST: Broker/Edit/5
        [HttpPost]
        public ActionResult Edit(int id, FormCollection collection)
        {
            try
            {
                // TODO: Add update logic here

                return RedirectToAction("Index");
            }
            catch
            {
                return View();
            }
        }

        // GET: Broker/Delete/5
        public ActionResult Delete(int id)
        {
            return View();
        }

        // POST: Broker/Delete/5
        [HttpPost]
        public ActionResult Delete(int id, FormCollection collection)
        {
            try
            {
                // TODO: Add delete logic here

                return RedirectToAction("Index");
            }
            catch
            {
                return View();
            }
        }


        private void ParseBrokers()
        {

        }

        private void CheckBrokers()
        {

        }

        private void InsertBrokers()
        {

        }

        private void UpdateBrokers()
        {

        }
    }
}
