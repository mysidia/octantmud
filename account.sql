drop table account;
drop sequence acct_id_seq;

create sequence acct_id_seq;

create table account (
	acct_id	int    DEFAULT NEXTVAL('acct_id_seq'),
	acct_name	varchar(25) UNIQUE NOT NULL,
	acct_passwd	varchar(64),
	acct_email	varchar(256) UNIQUE,
	acct_opened	timestamp DEFAULT NOW(),
	acct_activated  timestamp DEFAULT NULL,

	primary key(acct_id)
);

COPY account (acct_id, acct_name, acct_passwd, acct_email, acct_opened, acct_activated) FROM stdin;
1	Mysid	foo	mysidia-mudb@darkfire.net	2004-02-14 13:55:09.189058	2004-02-14 15:02:25.093847
\.

