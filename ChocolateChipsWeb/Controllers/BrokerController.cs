
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

using System.Threading;
using System.Threading.Tasks;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Data.Entity;

using System.Xml;
using System.IO;
using System.Text;

using System.Diagnostics;
using ChocolateChipsWeb;

namespace ChocolateChipsWeb.Controllers
{


    public class BrokerController : Controller
    {

        //static DBBroker SContext;

        List<Thread> m_ThreadList;

        String m_ConnectionString = "Server=SHKBOX\\SQLEXPRESS;Database=DBChocolateChips;User Id=Admin;Password=Admin;";
        SqlConnection m_Connection;

        public static void ThreadPoolAction(String tFileName)
        {
            // int threadIndex = (int)threadContext;
            // Console.WriteLine("thread {0} started...", threadIndex);
            //fibOfN = Calculate(_n);
            ParseXML((String)tFileName);


            //if (tVar != null)
            //{
            //    tVar.Set();
            //}
            //else
            //{

            //}
            // Console.WriteLine("thread {0} result calculated...", threadIndex);
            //_doneEvent.Set();
        }

        public void LoadBrokers()
        {
            String aBaseFilePath = "E:\\Development\\ChipsAhoyOpenCL\\ChipsAhoyOpenCL\\Broker";
            String[] aFileList =  Directory.GetFiles(aBaseFilePath, "*xml");
            //ParseXML("E:\\Development\\ChipsAhoyOpenCL\\ChipsAhoyOpenCL\\Broker\\361480145__1.xml");


            //ThreadPool.SetMaxThreads(10, 10);
            ManualResetEvent[] aHandleList = new ManualResetEvent[aFileList.Count()];
            m_ThreadList = new List<Thread>();
            //ThreadPool.QueueUserWorkItem(new WaitCallback(ParseXML), aFileList);

            for (int y = 0; y < aFileList.Count() - 1; y++)
            {
                aHandleList[y] = new ManualResetEvent(false);
                Console.WriteLine(y + " of " + aFileList.Count());
                //Thread aThread = new Thread(new ThreadStart(BrokerController.ParseXML));
                Thread aThread = new Thread(x => ParseXML(aFileList[y]));
                aThread.Start();
                m_ThreadList.Add(aThread);
                //ThreadPool.QueueUserWorkItem(x =>
                //{

                //});

            }

            //WaitHandle.WaitAll(aHandleList);
            for(int x=0; x< m_ThreadList.Count(); x++)
            {
                m_ThreadList[x].Join();
            //    var aTempList = new ManualResetEvent[1];
            //    aTempList[0] = aHandleList[x];
            //    WaitHandle.WaitAll(aTempList);
                // ManualResetEvent[] = new ManualResetEvent[aFileList.Count()];


            }
            //foreach (var aHandle in aHandleList)
            //{
            //    aHandle.WaitOne();
            //    //var aTempList = new ManualResetEvent[1];
            //    //aTempList[0] = aHandle;
            //    //WaitHandle.WaitAny(aTempList);
            //}


            }


        public static void ParseXML(String tFile)
        {
            StringBuilder aRet = new StringBuilder();
            DBBroker aDBContext = new DBBroker();
            bool aSuccess;
            using (XmlReader aReader = XmlReader.Create(tFile))
            {
                List<Market> aMarketList = new List<Market>();

                aReader.MoveToContent();
                String aGhettoEnum = "";
                
                Broker aCurrentBroker = new Broker();
                Market aCurrentMarket;
                Action aCurrentAction;


                while (aReader.Read())
                {
                    if (aReader.NodeType == XmlNodeType.Element)
                    {

                        switch (aReader.Name)
                        {
                            case "AlgorithmID":
                                aReader.Read();
                                aCurrentBroker.Broker_Algorithm_Version = int.Parse(aReader.Value);
                                break;
                            case "BrokerGUID":
                                aReader.Read();
                                aCurrentBroker.Broker_GUID = aReader.Value;
                                break;

                            case "TotalProfit":
                                aReader.Read();
                                aCurrentBroker.Broker_NetWorth = decimal.Parse(aReader.Value);
                                break;
                            case "ShareCount":
                                aReader.Read();
                                aCurrentBroker.Broker_TotalShareCount = int.Parse(aReader.Value);
                                break;
                            case "ProfitPerShare":
                                aReader.Read();
                                aCurrentBroker.Broker_ProfitPerShare = decimal.Parse(aReader.Value);
                                break;

                            #region SETTINGS
                            case "Setting0":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_00 = double.Parse(aReader.Value);
                                break;
                            case "Setting1":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_01 = double.Parse(aReader.Value);
                                break;
                            case "Setting2":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_02 = double.Parse(aReader.Value);
                                break;
                            case "Setting3":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_03 = double.Parse(aReader.Value);
                                break;
                            case "Setting4":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_04 = double.Parse(aReader.Value);
                                break;
                            case "Setting5":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_05 = double.Parse(aReader.Value);
                                break;
                            case "Setting6":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_06 = double.Parse(aReader.Value);
                                break;
                            case "Setting7":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_07 = double.Parse(aReader.Value);
                                break;
                            case "Setting8":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_08 = double.Parse(aReader.Value);
                                break;
                            case "Setting9":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_09 = double.Parse(aReader.Value);
                                break;
                            case "Setting10":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_10 = double.Parse(aReader.Value);
                                break;
                            case "Setting11":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_11 = double.Parse(aReader.Value);
                                break;
                            case "Setting12":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_12 = double.Parse(aReader.Value);
                                break;
                            case "Setting13":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_13 = double.Parse(aReader.Value);
                                break;
                            case "Setting14":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_14 = double.Parse(aReader.Value);
                                break;
                            case "Setting15":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_15 = double.Parse(aReader.Value);
                                break;
                            case "Setting16":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_16 = double.Parse(aReader.Value);
                                break;
                            case "Setting17":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_17 = double.Parse(aReader.Value);
                                break;
                            case "Setting18":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_18 = double.Parse(aReader.Value);
                                break;
                            case "Setting19":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_19 = double.Parse(aReader.Value);
                                break;
                            case "Setting20":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_20 = double.Parse(aReader.Value);
                                break;
                            case "Setting21":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_21 = double.Parse(aReader.Value);
                                break;
                            case "Setting22":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_22 = double.Parse(aReader.Value);
                                break;
                            case "Setting23":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_23 = double.Parse(aReader.Value);
                                break;
                            case "Setting24":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_24 = double.Parse(aReader.Value);
                                break;
                            case "Setting25":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_25 = double.Parse(aReader.Value);
                                break;
                            case "Setting26":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_26 = double.Parse(aReader.Value);
                                break;
                            case "Setting27":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_27 = double.Parse(aReader.Value);
                                break;
                            case "Setting28":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_28 = double.Parse(aReader.Value);
                                break;
                            case "Setting29":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_29 = double.Parse(aReader.Value);
                                break;
                            case "Setting30":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_30 = double.Parse(aReader.Value);
                                break;
                            case "Setting31":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_31 = double.Parse(aReader.Value);
                                break;
                            case "Setting32":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_32 = double.Parse(aReader.Value);
                                break;
                            case "Setting33":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_33 = double.Parse(aReader.Value);
                                break;
                            case "Setting34":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_34 = double.Parse(aReader.Value);
                                break;
                            case "Setting35":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_35 = double.Parse(aReader.Value);
                                break;
                            case "Setting36":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_36 = double.Parse(aReader.Value);
                                break;
                            case "Setting37":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_37 = double.Parse(aReader.Value);
                                break;
                            case "Setting38":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_38 = double.Parse(aReader.Value);
                                break;
                            case "Setting39":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_39 = double.Parse(aReader.Value);
                                break;
                            case "Setting40":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_40 = double.Parse(aReader.Value);
                                break;
                            case "Setting41":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_41 = double.Parse(aReader.Value);
                                break;
                            case "Setting42":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_42 = double.Parse(aReader.Value);
                                break;
                            case "Setting43":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_43 = double.Parse(aReader.Value);
                                break;
                            case "Setting44":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_44 = double.Parse(aReader.Value);
                                break;
                            case "Setting45":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_45 = double.Parse(aReader.Value);
                                break;
                            case "Setting46":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_46 = double.Parse(aReader.Value);
                                break;
                            case "Setting47":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_47 = double.Parse(aReader.Value);
                                break;
                            case "Setting48":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_48 = double.Parse(aReader.Value);
                                break;
                            case "Setting49":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_49 = double.Parse(aReader.Value);
                                break;
                            case "Setting50":
                                aReader.Read();
                                aCurrentBroker.Broker_Setting_50 = double.Parse(aReader.Value);
                                break;
                            #endregion
                                
                            case "MarketActions":
                                if (aReader.NodeType == XmlNodeType.Element)
                                {
                                    aCurrentMarket = new Market();
                                    List<Action> aActionList = new List<Action>();
                                    string aMarketName = aReader.GetAttribute("Name");
                                    aReader.Read();
                                    aCurrentMarket.BrokerGUID = (aCurrentBroker.Broker_GUID).ToString();
                                    aCurrentMarket.MarketSymbol = aMarketName;

                                    bool aLoadingMarket = true;
                                    while (aReader.Read() && aLoadingMarket)
                                    {
                                        if (aReader.Name == "Action")
                                        {
                                            aCurrentAction = new Action();
                                            aCurrentAction.BrokerGUID = (aCurrentBroker.Broker_GUID).ToString();
                                            aCurrentAction.MarketSymbol = aMarketName;

                                            String aTimeStamp = aReader.GetAttribute("TimeStamp");
                                            aTimeStamp = aTimeStamp.Replace("|", "");
                                            aCurrentAction.TimeStamp = aTimeStamp;
                                            aCurrentAction.Action1 = aReader.GetAttribute("Action");
                                            aCurrentAction.ShareCount = int.Parse(aReader.GetAttribute("ShareCount"));
                                            aCurrentAction.Price = double.Parse(aReader.GetAttribute("Price"));
                                            aCurrentAction.Investment = double.Parse(aReader.GetAttribute("Investment"));
                                            aCurrentAction.Budget = double.Parse(aReader.GetAttribute("Budget"));
                                            aCurrentAction.Value = aCurrentAction.ShareCount * aCurrentAction.Price;


                                            // int aActionInsert = 
                                            aActionList.Add(aCurrentAction);
                                            //aActionInsert.
                                            // aActionInsert.Start();
                                        }


                                        if (aReader.Name == "Details" && aReader.NodeType == XmlNodeType.Element)
                                        {
                                            while (aReader.Read() && aLoadingMarket)
                                            {
                                                if (aReader.NodeType == XmlNodeType.Element)
                                                {
                                                    if (aReader.Name == "Profit")
                                                    {
                                                        aReader.Read();
                                                         aCurrentMarket.MarketProfit = double.Parse(aReader.Value);
                                                    }
                                                    else if (aReader.Name == "ShareCount")
                                                    {
                                                        aReader.Read();
                                                        aCurrentMarket.MarketShareCount = int.Parse(aReader.Value);

                                                        aMarketList.Add(aCurrentMarket);
                                                        
                                                    }
                                                }
                                                else
                                                {
                                                    if (aReader.NodeType == XmlNodeType.EndElement)
                                                    {

                                                        if (aReader.Name == "Details")
                                                        {
                                                            //Task aMarketInsert = CheckMarket(aCurrentMarket, aContext);
                                                            //aMarketInsert.Wait();
                                                            aLoadingMarket = false;
                                                        }
                                                    }
                                                }
                                            }
                                            
                                                
                                            
                                        }
                                    }

                                    for (int x = aActionList.Count - 1; x >= 0; x--)
                                    {

                                        if (CheckAction(aActionList[x], aDBContext))
                                        {
                                            //aDBContext.Actions.Add(aActionList[x]);
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }

                                }
                                break;

                            default:
                                break;
                        }
                    }

                }

                for (int x = aMarketList.Count - 1; x >= 0; x--)
                {

                    if (CheckMarket(aMarketList[x], aDBContext))
                    {
                        //aDBContext.Markets.Add(aMarketList[x]);
                    }
                    else
                    {
                        break;
                    }
                }
                
                CheckBroker(aCurrentBroker, aDBContext);
                //aDBContext.Brokers.Add(aCurrentBroker);

                IEnumerable<System.Data.Entity.Validation.DbEntityValidationResult> aList = aDBContext.GetValidationErrors();
                if (aList.Count() > 0)
                {

                }


                aDBContext.SaveChangesAsync();
            }

        }




        //reader.ReadToFollowing("book");
        //        reader.MoveToFirstAttribute();
        //        reader.MoveToAttribute("Action");
        //        string genre = reader.Value;
        //output.AppendLine("The genre value: " + genre);

        //        reader.ReadToFollowing("title");
        //        output.AppendLine("Content of the title element: " + reader.ReadElementContentAsString());







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

        public static bool CheckAction(Action tAction, DBBroker aDBContext)
        {
            bool aRet = false;
            List<Action> aActionList = new List<Action>();

            //aActionList = await (from aAction in SContext.Actions
            //   where   aAction.TimeStamp.Equals(tAction.TimeStamp)
            //   &&       aAction.Action1.Equals(tAction.Action1)
            //   &&       aAction.BrokerGUID.Equals(tAction.BrokerGUID)
            //   &&       aAction.MarketSymbol.Equals(tAction.MarketSymbol)
            //            select aAction).ToListAsync();
            bool aRetQuery = false;
            while (!aRetQuery)
            {
                try
                {
                    aActionList = (from aAction in aDBContext.Actions
                                   where aAction.TimeStamp.Equals(tAction.TimeStamp)
                                   && aAction.Action1.Equals(tAction.Action1)
                                   && aAction.BrokerGUID.Equals(tAction.BrokerGUID)
                                   && aAction.MarketSymbol.Equals(tAction.MarketSymbol)
                                   select aAction).ToList();
                    aRetQuery = true;
                }
                catch(Exception e)
                {

                }
            }

            if (aActionList.Count == 0)
            {
                aRet = true;
                aDBContext.Actions.Add(tAction);
                //SContext.SaveChanges();
            }
            return aRet;
        }
        public static bool CheckMarket(Market tMarket, DBBroker aDBContext)
        {
            bool aRet = false;
            List<Market> aMarketList = new List<Market>();
            //SContext.ChangeTracker.
            aMarketList = (from aMarket in aDBContext.Markets where 
                                  aMarket.BrokerGUID.Equals(tMarket.BrokerGUID) && aMarket.MarketSymbol.Equals(tMarket.MarketSymbol)
                                  select aMarket).ToList();

            if (aMarketList.Count == 0)
            {
                aRet = true;
                aDBContext.Markets.Add(tMarket);
                //SContext.SaveChanges();
            }
            return aRet;
        }
        public static bool CheckBroker(Broker tBroker, DBBroker aDBContext)
        {
            bool aRet = false;
            List<Broker> aBrokerList = new List<Broker>();
            
            aBrokerList = (from aBroker in aDBContext.Brokers where 
                           aBroker.Broker_GUID.Equals(tBroker.Broker_GUID) select aBroker).ToList();

            if (aBrokerList.Count == 0)
            {
                aRet = true;
                aDBContext.Brokers.Add(tBroker);
                //SContext.SaveChanges();
                //await SContext.
            }
            return aRet;
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
