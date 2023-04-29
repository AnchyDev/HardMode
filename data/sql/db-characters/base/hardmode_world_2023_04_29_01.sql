DROP TABLE IF EXISTS `hardmode_player_settings`;
CREATE TABLE IF NOT EXISTS `hardmode_player_settings` (
  `guid` int unsigned DEFAULT NULL,
  `modes` text COLLATE utf8mb4_general_ci,
  `tainted` tinyint DEFAULT NULL,
  `shadowban` tinyint DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

DELETE FROM `hardmode_player_settings`;