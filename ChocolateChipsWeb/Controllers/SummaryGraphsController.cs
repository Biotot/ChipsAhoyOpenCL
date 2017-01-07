using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Threading.Tasks;
using System.Net;
using System.Web;
using System.Web.Mvc;
using ChocolateChipsWeb;
using System.Web.Helpers;

namespace ChocolateChipsWeb.Controllers
{
    public class SummaryGraphsController : Controller
    {
        private DBBroker db = new DBBroker();

        // GET: SummaryGraphs0.
        public async Task<ActionResult> Index()
        {
            var aList = await
                (from aSummary in db.SummaryGraphs orderby aSummary.Timestamp descending select aSummary).ToListAsync();
            if (aList.Count>1000)
            {
                aList.RemoveRange(1000, aList.Count-1000);
            }
            aList = aList.OrderBy(c => DateTime.Parse(c.Timestamp)).ThenBy(c => c.Score).Reverse().ToList<SummaryGraph>();
           // aList.Sort((a,b) => ((double)b.Score).CompareTo(a.Score));
            //aList.Sort((a, b) => (DateTime.Parse(a.Timestamp).CompareTo(DateTime.Parse(b.Timestamp))));
            return View(aList);
        }

        // GET: SummaryGraphs/Details/5
        public async Task<ActionResult> Details(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            SummaryGraph summaryGraph = await db.SummaryGraphs.FindAsync(id);
            if (summaryGraph == null)
            {
                return HttpNotFound();
            }
            return View(summaryGraph);
        }

        // GET: SummaryGraphs/Create
        public ActionResult Create()
        {
            return View();
        }

        // POST: SummaryGraphs/Create
        // To protect from overposting attacks, please enable the specific properties you want to bind to, for 
        // more details see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> Create([Bind(Include = "SummaryID,MarketSymbol,Timestamp,ClosePrice,Score,BuyPercent,SellPercent,ShareCapacity,ActionCount,AverageProfitPerShare,AverageProfit,AveragePercentReturn,ProfitPerShareDeviation,PercentReturnDeviation,BrokerCount")] SummaryGraph summaryGraph)
        {
            if (ModelState.IsValid)
            {
                db.SummaryGraphs.Add(summaryGraph);
                await db.SaveChangesAsync();
                return RedirectToAction("Index");
            }
            
            return View(summaryGraph);
        }

        // GET: SummaryGraphs/Edit/5
        public async Task<ActionResult> Edit(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            SummaryGraph summaryGraph = await db.SummaryGraphs.FindAsync(id);
            if (summaryGraph == null)
            {
                return HttpNotFound();
            }
            return View(summaryGraph);
        }

        // POST: SummaryGraphs/Edit/5
        // To protect from overposting attacks, please enable the specific properties you want to bind to, for 
        // more details see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> Edit([Bind(Include = "SummaryID,MarketSymbol,Timestamp,ClosePrice,Score,BuyPercent,SellPercent,ShareCapacity,ActionCount,AverageProfitPerShare,AverageProfit,AveragePercentReturn,ProfitPerShareDeviation,PercentReturnDeviation,BrokerCount")] SummaryGraph summaryGraph)
        {
            if (ModelState.IsValid)
            {
                db.Entry(summaryGraph).State = EntityState.Modified;
                await db.SaveChangesAsync();
                return RedirectToAction("Index");
            }
            return View(summaryGraph);
        }

        // GET: SummaryGraphs/Delete/5
        public async Task<ActionResult> Delete(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            SummaryGraph summaryGraph = await db.SummaryGraphs.FindAsync(id);
            if (summaryGraph == null)
            {
                return HttpNotFound();
            }
            return View(summaryGraph);
        }

        // POST: SummaryGraphs/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> DeleteConfirmed(int id)
        {
            SummaryGraph summaryGraph = await db.SummaryGraphs.FindAsync(id);
            db.SummaryGraphs.Remove(summaryGraph);
            await db.SaveChangesAsync();
            return RedirectToAction("Index");
        }

        public async Task<ActionResult> GraphView(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            SummaryGraph aSummaryGraph = await db.SummaryGraphs.FindAsync(id);
            if (aSummaryGraph == null)
            {
                return HttpNotFound();
            }

            
            var aList = await 
                (from aSummary in db.SummaryGraphs where aSummary.MarketSymbol.Equals(aSummaryGraph.MarketSymbol) orderby aSummary.Timestamp descending select aSummary).ToListAsync();

            if (aList.Count > 1000)
            {
                aList.RemoveRange(1000, aList.Count - 1000);
            }
            aList = aList.OrderBy(c => DateTime.Parse(c.Timestamp)).ThenBy(c => c.Score).Reverse().ToList<SummaryGraph>();
            
            //Chart aChart = new Chart(1000,200);
            //aChart.AddTitle("Price Trend");
            //aChart.AddSeries(null,"Column",null,null,null,1,aList, "Timestamp", aList,"Price");
            //aChart.DataBindTable(aList, "Timestamp");
            

            return View(aList);
        }


        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
