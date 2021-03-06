﻿//------------------------------------------------------------------------------
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
    using System.Data.Entity;
    using System.Data.Entity.Infrastructure;
    using System.Data.Entity.Core.Objects;
    using System.Linq;
    
    public partial class DBBroker : DbContext
    {
        public DBBroker()
            : base("name=DBBroker")
        {
        }
    
        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            throw new UnintentionalCodeFirstException();
        }
    
        public virtual DbSet<Action> Actions { get; set; }
        public virtual DbSet<Broker> Brokers { get; set; }
        public virtual DbSet<Market> Markets { get; set; }
        public virtual DbSet<Summary> Summaries { get; set; }
        public virtual DbSet<SummaryGraph> SummaryGraphs { get; set; }
    
        public virtual int sp_ClenseMarkets()
        {
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction("sp_ClenseMarkets");
        }
    
        public virtual int sp_PurgeDatabase(Nullable<bool> tPurgeBrokers, Nullable<bool> tPurgeMarkets, Nullable<bool> tPurgeActions)
        {
            var tPurgeBrokersParameter = tPurgeBrokers.HasValue ?
                new ObjectParameter("tPurgeBrokers", tPurgeBrokers) :
                new ObjectParameter("tPurgeBrokers", typeof(bool));
    
            var tPurgeMarketsParameter = tPurgeMarkets.HasValue ?
                new ObjectParameter("tPurgeMarkets", tPurgeMarkets) :
                new ObjectParameter("tPurgeMarkets", typeof(bool));
    
            var tPurgeActionsParameter = tPurgeActions.HasValue ?
                new ObjectParameter("tPurgeActions", tPurgeActions) :
                new ObjectParameter("tPurgeActions", typeof(bool));
    
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction("sp_PurgeDatabase", tPurgeBrokersParameter, tPurgeMarketsParameter, tPurgeActionsParameter);
        }
    
        public virtual int sp_MarketView()
        {
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction("sp_MarketView");
        }
    
        public virtual int UpdateSummaries(Nullable<int> high, Nullable<int> low)
        {
            var highParameter = high.HasValue ?
                new ObjectParameter("High", high) :
                new ObjectParameter("High", typeof(int));
    
            var lowParameter = low.HasValue ?
                new ObjectParameter("Low", low) :
                new ObjectParameter("Low", typeof(int));
    
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction("UpdateSummaries", highParameter, lowParameter);
        }
    
        public virtual int LoadSummaryList()
        {
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction("LoadSummaryList");
        }
    }
}
