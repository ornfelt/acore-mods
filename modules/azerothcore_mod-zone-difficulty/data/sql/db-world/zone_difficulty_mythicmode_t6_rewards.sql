SET @PRICE := 36;
SET @ILLIDANPRICE := 45;
DELETE FROM `zone_difficulty_mythicmode_rewards` WHERE `ContentType` = 11;
INSERT INTO `zone_difficulty_mythicmode_rewards` (`ContentType`, `ItemType`, `Entry`, `Price`, `Enchant`, `EnchantSlot`, `Achievement`, `Enabled`, `Comment`) VALUES
(11, 1, 32337, @PRICE, 0, 0, 697, 1, 'Shroud of Forgiveness - Gurtogg'),
(11, 1, 32524, @ILLIDANPRICE, 0, 0, 697, 1, 'Shroud of the Highborne - Illidan'),
(11, 1, 32323, @PRICE, 0, 0, 697, 1, 'Shadowmoon Destroyer Drape - Teron'),
(11, 1, 32370, @PRICE, 0, 0, 697, 1, 'Nadina Pendant of Purity - Shahraz'),
(11, 1, 32349, @PRICE, 0, 0, 697, 1, 'Translucent Spellthread Necklace - Reliquary'),
(11, 1, 32260, @PRICE, 0, 0, 697, 1, 'Choker of Endless Nightmares - Supremus'),
(11, 1, 32368, @PRICE, 0, 0, 697, 1, 'Tome of the Lightbringer - Shahraz'),
(11, 1, 32257, @PRICE, 0, 0, 697, 1, 'Idol of the White Stag - Supremus'),
(11, 1, 32330, @PRICE, 0, 0, 697, 1, 'Totem of Astral Guidance - Teron'),
(11, 1, 32335, @PRICE, 0, 0, 697, 1, 'Unstoppable Aggressor Ring - Gurtogg'),
(11, 1, 32238, @PRICE, 0, 0, 697, 1, 'Ring of Calming Waves - Najentus'),
(11, 1, 32497, @ILLIDANPRICE, 0, 0, 697, 1, 'Stormrage Signet - Illidan'),
(11, 1, 32261, @PRICE, 0, 0, 697, 1, 'Band of the Abyssal - Supremus'),

(11, 1, 32501, @PRICE, 0, 0, 697, 1, 'Shadowmoon Insignia - Gurtogg'),
(11, 1, 32496, @ILLIDANPRICE, 0, 0, 697, 1, 'Memento of Tyrande - Illidan'),
(11, 1, 32483, @ILLIDANPRICE, 0, 0, 697, 1, 'Skull of Guldan - Illidan'),
(11, 1, 32505, @PRICE, 0, 0, 697, 1, 'Madness of the Betrayer - Council'),

(11, 2, 32340, @PRICE, 0, 0, 697, 1, 'Garment of Temperance - Gurtog'),
(11, 2, 32327, @PRICE, 0, 0, 697, 1, 'Robe of the Shadow Countil - Teron'),
(11, 2, 32239, @PRICE, 0, 0, 697, 1, 'Slippers of the Seacaller - Najentus'),
(11, 2, 32353, @PRICE, 0, 0, 697, 1, 'Gloves of Unfailing Faith - Reliquary'),
(11, 2, 32525, @ILLIDANPRICE, 0, 0, 697, 1, 'Cown of the Illidari High Lord - Illidan'),
(11, 2, 32329, @PRICE, 0, 0, 697, 1, 'Cowl of Benevolence - Teron'),
(11, 2, 32367, @PRICE, 0, 0, 697, 1, 'Leggins of Devastation - Shahraz'),
(11, 2, 32338, @PRICE, 0, 0, 697, 1, 'Blood-cursed Shoulderpads - Gurtogg'),
(11, 2, 32273, @PRICE, 0, 0, 697, 1, 'Amice of Brilliant Light - Akama'),
(11, 2, 32256, @PRICE, 0, 0, 697, 1, 'Waistwrap of Infinity - Supremus'),

(11, 3, 32252, @PRICE, 0, 0, 697, 1, 'Nether Shadow Tunic - Supremus'),
(11, 3, 32366, @PRICE, 0, 0, 697, 1, 'Shadowmaster Boots - Shahraz'),
(11, 3, 32347, @PRICE, 0, 0, 697, 1, 'Grips of Damnation - Reliquary'),
(11, 3, 32328, @PRICE, 0, 0, 697, 1, 'Botanist Gloves - Teron'),
(11, 3, 32240, @PRICE, 0, 0, 697, 1, 'Guise of the Tidal Lurker - Najentus'),
(11, 3, 32235, @ILLIDANPRICE, 0, 0, 697, 1, 'Cursed Vision of Sargeras - Illidan'),
(11, 3, 32271, @PRICE, 0, 0, 697, 1, 'Kilt of Immortal Nature - Akama'),
(11, 3, 32377, @PRICE, 0, 0, 697, 1, 'Mantle of Darkness - Najentus'),
(11, 3, 32518, @PRICE, 0, 0, 697, 1, 'Veil of Turning Leaves - Council'),
(11, 3, 32339, @PRICE, 0, 0, 697, 1, 'Belt of Primal Majesty - Gurtog'),
(11, 3, 32265, @PRICE, 0, 0, 697, 1, 'Shadow-walker Cord - Akama'),
(11, 3, 32324, @PRICE, 0, 0, 697, 1, 'Insidious Bands - Teron'),

(11, 4, 32334, @PRICE, 0, 0, 697, 1, 'Vest of the Mounting Assault - Gurtog'),
(11, 4, 32242, @PRICE, 0, 0, 697, 1, 'Boots of Oceanic Fury - Najentus'),
(11, 4, 32510, @PRICE, 0, 0, 697, 1, 'Softstep Boots of Tracking - Teron'),
(11, 4, 32234, @PRICE, 0, 0, 697, 1, 'Fists of Mukoa - Najentus'),
(11, 4, 32275, @PRICE, 0, 0, 697, 1, 'Spiritwalker Gauntlets - Akama'),
(11, 4, 32241, @PRICE, 0, 0, 697, 1, 'Helm of Soothing Currents - Najentus'),
(11, 4, 32517, @PRICE, 0, 0, 697, 1, 'The Wavemender Mantle - Reliquary'),
(11, 4, 32264, @PRICE, 0, 0, 697, 1, 'Shoulders of the Hidden Predator - Akama'),
(11, 4, 32276, @PRICE, 0, 0, 697, 1, 'Flashfire Girdle - Akama'),
(11, 4, 32258, @PRICE, 0, 0, 697, 1, 'Naturalist Preserving Cinch - Supremus'),
(11, 4, 32259, @PRICE, 0, 0, 697, 1, 'Bands of the Coming Storm - Supremus'),
(11, 4, 32251, @PRICE, 0, 0, 697, 1, 'Wraps of Precise Flight - Supremus'),

(11, 5, 32365, @PRICE, 0, 0, 697, 1, 'Heartshatter Breastplate - Shahraz'),
(11, 5, 32243, @PRICE, 0, 0, 697, 1, 'Pearl Inlaid Boots - Najentus'),
(11, 5, 32245, @PRICE, 0, 0, 697, 1, 'Tide-stomper Greaves - Najentus'),
(11, 5, 32345, @PRICE, 0, 0, 697, 1, 'Dreadbots of the Legion - Reliquary'),
(11, 5, 32268, @PRICE, 0, 0, 697, 1, 'Myrmidon Treads - Akama'),
(11, 5, 32278, @PRICE, 0, 0, 697, 1, 'Grips of Silent Justice - Akama'),
(11, 5, 32280, @PRICE, 0, 0, 697, 1, 'Gauntlets of Enforcement - Teron'),
(11, 5, 32521, @ILLIDANPRICE, 0, 0, 697, 1, 'Faceplate of the Impenetrable - Illidan'),
(11, 5, 32354, @PRICE, 0, 0, 697, 1, 'Crowd of the Empowered Fate - Reliquary'),
(11, 5, 32341, @PRICE, 0, 0, 697, 1, 'Leggins of the Divine Retribution - Gurtog'),
(11, 5, 32263, @PRICE, 0, 0, 697, 1, 'Praetorian Legguards - Akama'),
(11, 5, 32250, @PRICE, 0, 0, 697, 1, 'Pauldrons of the Abyssal Fury - Supremus'),
(11, 5, 32342, @PRICE, 0, 0, 697, 1, 'Girdle of the Mighty Resolve - Gurtog'),
(11, 5, 32333, @PRICE, 0, 0, 697, 1, 'Girdle of Stability - Gurtog'),
(11, 5, 32512, @PRICE, 0, 0, 697, 1, 'Girdle of Lordaeron Fall - Teron'),
(11, 5, 32232, @PRICE, 0, 0, 697, 1, 'Eternium Shell Bracers - Najentus'),
(11, 5, 32279, @PRICE, 0, 0, 697, 1, 'The Seeker Wristguards - Akama'),

(11, 6, 32236, @PRICE, 0, 0, 697, 1, 'Rising Tide - Najentus'),
(11, 6, 32254, @PRICE, 0, 0, 697, 1, 'The Brutalizer - Supremus'),
(11, 6, 32269, @PRICE, 0, 0, 697, 1, 'Messenger of Fate - Gurtogg'),
(11, 6, 32237, @PRICE, 0, 0, 697, 1, 'The Maelstrom Fury - Najentus'),
(11, 6, 32471, @ILLIDANPRICE, 0, 0, 697, 1, 'Shard of Azzinoth - Illidan'),
(11, 6, 32500, @ILLIDANPRICE, 0, 0, 697, 1, 'Crystal Spire of karabor - Illidan'),
(11, 6, 32262, @PRICE, 0, 0, 697, 1, 'Syphon of the Nathrezim - Supremus'),
(11, 6, 32369, @PRICE, 0, 0, 697, 1, 'Blade of Savagery - Shahraz'),
(11, 6, 32348, @PRICE, 0, 0, 697, 1, 'Soul Cleaver - Teron'),
(11, 6, 32332, @PRICE, 0, 0, 697, 1, 'Torch of the Damned - Reliquary'),
(11, 6, 32248, @PRICE, 0, 0, 697, 1, 'Halberd of Desolation - Najentus'),
(11, 6, 32344, @PRICE, 0, 0, 697, 1, 'Staff of Immaculate Recovery - Gurtogg'),
(11, 6, 32374, @ILLIDANPRICE, 0, 0, 697, 1, 'Zhardoom Greatstaff of the Devourer - Illidan'),
(11, 6, 32253, @PRICE, 0, 0, 697, 1, 'Legionkiller - Supremus'),
(11, 6, 32325, @PRICE, 0, 0, 697, 1, 'Rifle of the Stoic Guardian - Teron'),
(11, 6, 32336, @ILLIDANPRICE, 0, 0, 697, 1, 'Black bow of the Betrayer - Illidan'),
(11, 6, 32326, @PRICE, 0, 0, 697, 1, 'Twisted Blades of Zarak - Teron'),
(11, 6, 32343, @PRICE, 0, 0, 697, 1, 'Wand of Prismatic Focus - Gurtogg'),
(11, 6, 32363, @PRICE, 0, 0, 697, 1, 'Naaru-Blessed Life Rod - Reliquary'),
(11, 6, 32350, @PRICE, 0, 0, 697, 1, 'Touch of Inspiration - Reliquary'),
(11, 6, 32361, @PRICE, 0, 0, 697, 1, 'Blind-seers Icon - Akama'),
(11, 6, 32375, @ILLIDANPRICE, 0, 0, 697, 1, 'Bulwark of Azzinoth - Illidan'),
(11, 6, 32255, @PRICE, 0, 0, 697, 1, 'Felstone Bulwark - Supremus');