using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(ChocolateChipsWeb.Startup))]
namespace ChocolateChipsWeb
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
