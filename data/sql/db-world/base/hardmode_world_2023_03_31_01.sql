DROP TABLE IF EXISTS `hardmode_selfcraft_exclude`;
CREATE TABLE IF NOT EXISTS `hardmode_selfcraft_exclude` (
  `item_id` int(11) DEFAULT NULL,
  `comment` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;