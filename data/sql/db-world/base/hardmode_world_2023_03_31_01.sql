DROP TABLE IF EXISTS `hardmode_rewards`;
CREATE TABLE IF NOT EXISTS `hardmode_rewards` (
  `mode` int(11) DEFAULT NULL,
  `reward_type` int(11) DEFAULT NULL,
  `reward_id` int(11) DEFAULT NULL,
  `reward_count` int(11) DEFAULT NULL,
  `comment` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

DROP TABLE IF EXISTS `hardmode_selfcraft_exclude`;
CREATE TABLE IF NOT EXISTS `hardmode_selfcraft_exclude` (
  `item_id` int(11) DEFAULT NULL,
  `comment` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;