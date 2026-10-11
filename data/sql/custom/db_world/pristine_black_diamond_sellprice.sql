-- Pristine Black Diamond (18335) ships with SellPrice 0, so vendors refuse it.
-- Official retail/classic sell price is 50 copper.
UPDATE `item_template`
SET `SellPrice` = 50
WHERE `entry` = 18335;
