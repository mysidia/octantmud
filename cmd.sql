drop table player_skills;
drop table player_commands;
drop table mud_target_templates;
drop sequence mud_command_sortslot;

create sequence mud_command_sortslot;

create table mud_target_templates (
	name VARCHAR(30),
	str VARCHAR(255),
	parser VARCHAR(80),

	primary key(name)
);

create table player_commands
(
	sortpos 	int DEFAULT NEXTVAL('mud_command_sortslot'),
	name		VARCHAR(30),
	func		VARCHAR(50),
	min_position 	VARCHAR(80),
	min_level    	int DEFAULT 0,
	log	 	int DEFAULT 0,
	lag_count	int,
	mana_cost	int,
	move_cost	int,
	command_class	int,

	battle_time 	varchar(30),

	target		VARCHAR(30) references mud_target_templates
					not null,

	flags   	int,
	subcmd		int,

	primary key(name)
);

create table player_skills
(
) INHERITS(player_commands);

INSERT INTO mud_target_templates VALUES('no_target', 'tar_ignore', NULL);
INSERT INTO mud_target_templates VALUES('arbitrary', 'tar_phrase', NULL);
INSERT INTO mud_target_templates VALUES('one_char', 'tar_char_room', 'proc_onechar_local');
INSERT INTO mud_target_templates VALUES('two_chars', 'tar_char_room tar_char_room', NULL);
INSERT INTO mud_target_templates VALUES('one_other_char', 'tar_char_room|tar_not_self', NULL);
INSERT INTO mud_target_templates VALUES('give_cmd', 'tar_held tar_char', NULL);


INSERT INTO player_commands
         VALUES(1, 'quit', 'do_quit', 'Sleeping', 0, 0, 0, 0, 0, 0, 0, 'no_target', 0, 0);

INSERT INTO player_commands
	VALUES(0, 'qui', 'do_not_quit', 'Sleeping', 0, 0, 0, 0, 0, 0, 0, 'no_target', 0, 0);

INSERT INTO player_commands
	VALUES(10, 'save', 'do_save', 'Dead', 0, 0, 0, 0, 0, 0, 0, 'no_target', 0, 0);

INSERT INTO player_commands
	VALUES(10, 'look', 'do_look', 'Awake', 0, 0, 0, 0, 0, 0, 0, 'no_target',0, 0);

INSERT INTO player_commands
	VALUES(10, 'who', 'do_who', 'Dead', 0, 0, 0, 0, 0, 0, 0, 'no_target', 0, 0);

INSERT INTO player_commands
        VALUES(10, 'commands', 'do_commands', 'Dead', 0, 0, 0, 0, 0, 0, 0, 'no_target', 0, 0);


