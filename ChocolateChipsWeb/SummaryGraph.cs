//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace ChocolateChipsWeb
{
    using System;
    using System.Collections.Generic;
    
    public partial class SummaryGraph
    {
        public int SummaryID { get; set; }
        public string MarketSymbol { get; set; }
        public string Timestamp { get; set; }
        public Nullable<double> ClosePrice { get; set; }
        public Nullable<double> Score { get; set; }
        public Nullable<decimal> BuyPercent { get; set; }
        public Nullable<decimal> SellPercent { get; set; }
        public Nullable<double> ShareCapacity { get; set; }
        public Nullable<int> ActionCount { get; set; }
        public Nullable<double> AverageProfitPerShare { get; set; }
        public Nullable<double> AverageProfit { get; set; }
        public Nullable<double> AveragePercentReturn { get; set; }
        public Nullable<double> ProfitPerShareDeviation { get; set; }
        public Nullable<double> PercentReturnDeviation { get; set; }
        public Nullable<int> BrokerCount { get; set; }
    }
}
