﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace ChocolateChipsWeb.Models
{
    using System;
    using System.Data.Entity;
    using System.Data.Entity.Infrastructure;
    using System.Data.Entity.Core.Objects;
    using System.Linq;
    
    public partial class DBChocolateChipsEntities2 : DbContext
    {
        public DBChocolateChipsEntities2()
            : base("name=DBChocolateChipsEntities2")
        {
        }
    
        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            throw new UnintentionalCodeFirstException();
        }
    
    
        public virtual ObjectResult<sp_MarketView_Result> sp_MarketView()
        {
            return ((IObjectContextAdapter)this).ObjectContext.ExecuteFunction<sp_MarketView_Result>("sp_MarketView");
        }
    }
}
