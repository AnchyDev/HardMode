DELETE FROM `command` WHERE `name`='hardmode set mode';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('hardmode set mode', 2, 'Syntax: .hardmode setmode <mode> <value>');
DELETE FROM `command` WHERE `name`='hardmode set taint';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('hardmode set taint', 2, 'Syntax: .hardmode settaint <value>');
DELETE FROM `command` WHERE `name`='hardmode set shadowban';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('hardmode set shadowban', 2, 'Syntax: .hardmode setshadowban <value> ');
