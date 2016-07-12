#include "ChipsDB.h"

using namespace std;


//static char *opt_host_name = "SHKBOX\\SQLEXPRESS"; /* HOST */
//static char *opt_user_name = "Admin"; /* USERNAME */
//static char *opt_password = "Admin"; /* PASSWORD */
//static unsigned int opt_port_num = 3306; /* PORT */
//static char *opt_socket_name = NULL; /* SOCKET NAME, DO NOT CHANGE */
//static char *opt_db_name = "DBChocolateChips"; /* DATABASE NAME */
//static unsigned int opt_flags = 0; /* CONNECTION FLAGS, DO NOT CHANGE */
//mysqlcppconn-static.lib;
ChipsDB::ChipsDB()
{
	m_Connected = false;

	sql::mysql::MySQL_Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("SHKBOX\\SQLEXPRESS", "Admin", "Admin");

	if (con->isClosed())
	{
		cout << "Shit Fucked";
	}


	stmt = con->createStatement();
	stmt->execute("USE DBChocolateChips");
	stmt->execute("DROP TABLE IF EXISTS test");
	stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");
	stmt->execute("INSERT INTO test(id, label) VALUES (1, 'a')");

	delete stmt;
	delete con;
}


ChipsDB::~ChipsDB()
{
	//do nothing
}



vector<Broker> ChipsDB::GetTopBrokers(int tBrokerCount, std::string tWhereClause)
{
	vector<Broker> aTopList;



	return aTopList;
}
bool ChipsDB::CheckThenInsert(MarketPrice tMarketPrice)
{
	bool aRet = false;



	return aRet;
}
bool ChipsDB::CheckThenInsert(Broker tBestBroker)
{
	bool aRet = false;


	return aRet;
}
bool ChipsDB::BrokerAction(std::string tPriceTimestamp, std::string tMarketName, int tBrokerGuid, std::string tAction)
{
	bool aRet = false;



	return aRet;
}