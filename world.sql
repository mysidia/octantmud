delete from stats_info;
delete from locations;

INSERT INTO LOCATIONS
(universe, bounds, name, descr)
 VALUES(
   'POINT(0 0 0)'::geometry,
   'BOX3D(0 10,0 10)'::box3d,
  '<font color=cyan>Welcome!</font>',
  'Welcome to my incomplete mud.'
);


INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Strength', 's_str', 1);
	
INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Intelligence', 's_int', 1);
	
INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Wisdom', 's_wis', 1);

INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Dexterity', 's_dex', 1);

INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Constitution', 's_con', 1);

INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Charisma', 's_cha', 1);

INSERT into stats_info (stat_name, col_name, is_trainable)
	VALUES('Luck', 's_lck', 1);

