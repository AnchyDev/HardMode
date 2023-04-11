DROP TABLE IF EXISTS `hardmode_modes`;
CREATE TABLE IF NOT EXISTS `hardmode_modes` (
  `id` tinyint unsigned NOT NULL DEFAULT '0',
  `name` varchar(50) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `description` varchar(50) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `restrictions` bigint unsigned DEFAULT NULL,
  `enabled` tinyint DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
