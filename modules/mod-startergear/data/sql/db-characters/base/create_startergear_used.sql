-- ============================================================
--  mod-startergear
--  Base table for once-per-character usage tracking
--  Auto-loaded by AzerothCore DB updater from:
--  modules/mod-startergear/data/sql/db-characters/base/
-- ============================================================

CREATE TABLE IF NOT EXISTS `custom_startergear_used` (
    `guid`    INT UNSIGNED  NOT NULL,
    `used_at` TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='mod-startergear: tracks which characters already used .startergear';
