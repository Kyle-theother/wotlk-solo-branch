-- Keep every Darkmoon Faire location active. The building events are the short setup phase, so they stay off.
UPDATE `game_event`
SET `start_time` = '2000-01-01 00:00:00',
    `end_time` = '2035-01-01 00:00:00',
    `occurence` = 1,
    `length` = 5256000
WHERE `eventEntry` IN (3, 4, 5);

UPDATE `game_event`
SET `length` = 1
WHERE `eventEntry` IN (23, 71, 77);
