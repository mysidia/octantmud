drop table clan_member;
drop table character;
drop table proto_char;
drop table proto_object;
drop table locations;
drop table stats_info;
drop table location_coord_fields;
drop table races;

drop sequence char_id_seq;
drop sequence proto_char_id_seq;
drop sequence proto_obj_id_seq;
drop sequence clan_id_seq;
drop sequence location_seq;
drop table clan;


-- begin work;
create sequence char_id_seq;
create sequence proto_char_id_seq;
create sequence proto_obj_id_seq;
create sequence clan_id_seq;
create sequence location_seq;

create table stats_info
(
	col_name	varchar(25),
	stat_name	varchar(25),
	is_trainable	int
);

create table races
(
	name		varchar(25),
	s_str		int DEFAULT 0,
	s_int		int DEFAULT 0,
	s_wis		int DEFAULT 0,
	s_dex		int DEFAULT 0,
	s_con		int DEFAULT 0,
	s_cha		int DEFAULT 0,

	primary key(name)
);

insert into races (name) VALUES('Human');
insert into races (name,s_str,s_int,s_wis,s_dex,s_con,s_cha)
		VALUES('Dwarf', 1, 0, 0, 0, 0, 0);

create table clan
(
        id              int NOT NULL DEFAULT NEXTVAL('clan_id_seq'),
        name            varchar(30),
        who_title       varchar(30),
        gold            int,

        primary key(id)
);

create table location_coord_fields (
	universe	geometry,
	bounds		geometry DEFAULT 'BOX3D(0 0,0 0)'::box3d
);

create table locations (
	id		int DEFAULT NEXTVAL('location_seq') NOT NULL,
	name		varchar(256) NOT NULL,
	descr           varchar(8192) NOT NULL,
	primary key(id)
) INHERITS(location_coord_fields);

create table proto_object
(
	proto_obj_id        int DEFAULT NEXTVAL('proto_obj_id_seq'),
	name                varchar(255) NOT NULL,
	keywords            varchar(255),	
	short_desc	    varchar(255),
	long_desc           varchar(255),
	level               int DEFAULT null,
	nat_hp              int,
	nat_mp              int,
	nat_ep              int,
	nat_mv              int,
	s_str               int DEFAULT 13,
	s_int               int DEFAULT 13,
	s_wis               int DEFAULT 13,
	s_dex               int DEFAULT 13,
	s_con               int DEFAULT 13,
	s_cha               int DEFAULT 13,
	s_lck               int DEFAULT 13,
	s_ac                int DEFAULT 100,
	hr                  int DEFAULT 0,
	dr                  int DEFAULT 0,
	magblock            int DEFAULT 0,
	magpower            int DEFAULT 0,
	alignment           int DEFAULT 0,
	gender              varchar(25),
	clan_id             int references clan deferrable initially deferred, 
	default_position    varchar(25),

	primary key(proto_obj_id)
);

create table proto_char
(
	proto_char_id	int DEFAULT NEXTVAL('proto_char_id_seq'),
	name		varchar(255) UNIQUE NOT NULL,
	s_ac		int DEFAULT 100,
	hr		int DEFAULT 0,
	dr		int DEFAULT 0,
	magblock	int DEFAULT 0,
	magpower	int DEFAULT 0,
	alignment	int DEFAULT 0,
	gender		varchar(25),
	clan_id		int references clan deferrable initially deferred,
	default_position	varchar(25),

	primary key(proto_char_id)
) INHERITS(proto_object, location_coord_fields);

create table character
(
	id		int DEFAULT NEXTVAL('char_id_seq'),
	name            varchar(255) UNIQUE NOT NULL,
	acct_id		int NOT NULL references account,
	proto_id	int references proto_char,
	class		varchar(25),
	race		varchar(25) references races,
	is_person	bool,
	position	varchar(25),
--	loc_quad_xz	int DEFAULT 1,
--	loc_quad_yz	int DEFAULT 1,
--	loc_region_xz	int DEFAULT 1,
--	loc_region_yz	int DEFAULT 1,
--	loc_x		int DEFAULT 0,
--	loc_y		int DEFAULT 0,
--	loc_z		int DEFAULT 0,
	exp		int DEFAULT 0,
	hp		int DEFAULT 10,
	mp		int DEFAULT 100,
	ep		int DEFAULT 0,
	mv		int DEFAULT 100,
	h_water		int DEFAULT 24,
	h_full		int DEFAULT 24,
	h_drink		int DEFAULT 24,
	h_rest		int DEFAULT 0,
	h_stress	int DEFAULT 0,
	train_points	int DEFAULT 10,
	primary key(id)
) INHERITS(proto_char);

create table clan_member
(
	clan_id	int references clan,
	char_id int references character,
	rank int,
	primary key(clan_id, char_id)
);


create index char_person ON character (is_person);
create index char_quad_idx ON character (bounds);

