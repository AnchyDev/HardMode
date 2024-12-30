# HardMode
HardMode is a module heavily inspired by [mod-challenge-modes](https://github.com/ZhengPeiRu21/mod-challenge-modes). The purpose of this module is to develop extra functionality with a difference in code architecture. This makes it easier for me to develop on existing or expanding modes.

This module is still a WIP.

## How to add rewards
To add rewards, run mysql and use the world database.
Run the following command and take note of the hardmode_modes id: SELECT * FROM hardmode_modes;
Then with each reward you want to give, run the following command: INSERT INTO hardmode_rewards (mode, reward_level, reward_type, reward_id, reward_count, comment) VALUES(<hardmode_mode id>, <reward_type: 0 = item, 1 = title, 2 = spell>, <reward_id>, <reward_count>, 'this comment is not used in the game, so suggest to make it the name of the item, title, spell for future reference');

example: 
![image](https://github.com/user-attachments/assets/539e6b38-db23-4d91-9cb6-adfd46b5cd87)
- INSERT INTO hardmode_rewards (mode, reward_level, reward_type, reward_id, reward_count, comment) VALUES (4, 80, 0, 46708, 1, 'Deadly Gladiators Frost Wyrm');
- INSERT INTO hardmode_rewards (mode, reward_level, reward_type, reward_id, reward_count, comment) VALUES (5, 80, 0, 46171, 1, 'Furious Gladiators Frost Wyrm');
- INSERT INTO hardmode_rewards (mode, reward_level, reward_type, reward_id, reward_count, comment) VALUES (6, 80, 0, 47840, 1, 'Relentless Gladiators Frost Wyrm');

## Features
- Build your own HardModes using the [restriction builder](https://anchydev.github.io/HardMode/#mode-editor).
- Level based rewards for each mode you have created.
- Dynamic auras for each mode.

## Database
Head over to my HardMode editor to create your first mode: https://anchydev.github.io/HardMode/#mode-editor

## Credits
Thanks to [ZhengPeiRu21](https://github.com/ZhengPeiRu21) for creating the original module which inspired this one.
