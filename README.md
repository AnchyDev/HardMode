# HardMode
HardMode is a module heavily inspired by mod-challenge-modes. The purpose of this module is to develop extra functionality with a difference in code architecture. This makes it easier for me to develop on existing or expanding modes.

This module is still a WIP.

## Features
- **Self-Crafted Mode**
  - You can only equip and use items crafted by the player.
- **HardCore Mode**
   - Dying means you get sent to the shadow realm (permadeath).
- **LowXP Mode**
   - Your kill and quest exp is reduced to 1x rates.

Read the `mode-restrictions.txt` file for a list of restrictions.
## ⚠️ Attention ⚠️
You _must_ enable `EnablePlayerSettings` in the World config to use this module.

## Database Values
When referencing the modes and reward types you have to use the id value.


### Modes / Rewards
| Mode | Value | | Reward | Value |
| --- | --- | --- | --- | --- |
| Self-Crafted | 0 | | Item | 0 |
| HardCore | 1 | | Title | 1 |
| LowXP | 2 | | Spell | 2 |