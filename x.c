#include <stdio.h>
#include <dbi/dbi.h>


int main() {
	dbi_conn conn;
	dbi_result result;
	int v;

	dbi_initialize(NULL);
	conn = dbi_conn_new("pgsql");

	dbi_conn_set_option(conn, "host", "localhost");
	dbi_conn_set_option(conn, "username", "mud");
	dbi_conn_set_option(conn, "dbname", "mud");

	if ( (v = dbi_conn_connect(conn)) < 0 ) {
		const char* p;

		dbi_conn_error(conn, &p);
		printf(" :: %s\n", p);
		return 0;
	}
	else printf("%d\n", v);
	result = dbi_conn_query(conn, "select * from account");
}
