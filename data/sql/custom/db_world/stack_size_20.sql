-- Raise every item that currently stacks below 20 up to 20.
-- Unique ownership caps (maxcount) are left alone.
-- Run against acore_world, then .reload item_template or restart worldserver.
UPDATE `item_template`
SET `stackable` = 20
WHERE `stackable` > 0 AND `stackable` < 20;
