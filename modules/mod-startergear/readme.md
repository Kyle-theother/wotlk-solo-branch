# ⚔️ mod-startergear

### A modern starter gear module for AzerothCore WotLK 3.3.5a  > ***Beta*** <

<p align="center">

<a href="https://github.com/azerothcore/azerothcore-wotlk">
<img src="https://img.shields.io/badge/AzerothCore-Module-0ea5e9?style=for-the-badge&logo=github&logoColor=white">
</a>

<a href="https://www.azerothcore.org/">
<img src="https://img.shields.io/badge/WoW-3.3.5a-facc15?style=for-the-badge">
</a>

<img src="https://img.shields.io/badge/C%2B%2B-17-f97316?style=for-the-badge&logo=cplusplus&logoColor=white">
<img src="https://img.shields.io/badge/License-MIT-22c55e?style=for-the-badge">

</p>

<p align="center">

<img src="https://img.shields.io/badge/Status-Beta-10b981?style=flat-square">
<img src="https://img.shields.io/badge/Auto_SQL-Supported-8b5cf6?style=flat-square">
<img src="https://img.shields.io/badge/Configurable-Yes-06b6d4?style=flat-square">
<img src="https://img.shields.io/badge/WotLK-Heirlooms-ec4899?style=flat-square">

</p>

---

> ✨ Give freshly created characters a real head start.  
> Class-specific heirlooms, bags, riding skills, mounts and bonus XP rewards — all delivered instantly with a single command.

---

# 📚 Table of Contents

- [Features]
- [How It Works]
- [Installation]
- [Configuration]
- [Item Reference]
- [Class Gear Overview]
- [Database Schema]
- [For Developers]
- [FAQ]
- [License]

---

# ✨ Features

| Feature | Description |
|---|---|
| 🎒 Starter Bags | Configurable bag item and amount |
| ⚔️ Class Heirlooms | Automatic class-specific heirloom sets |
| 💍 XP Ring Included | Grants the Dread Pirate Ring |
| 🐴 Riding at Level 1 | Gives riding skills instantly |
| 🗡️ Multi-Spec Support | Hybrid classes receive multiple gear paths |
| 🔒 One-Time Usage | Character DB tracking |
| 😂 Funny Responses | Random witty retry messages |
| ⚙️ Fully Configurable | Enable or disable everything |
| 🗄️ Automatic SQL | AzerothCore DB updater support |

---

# 🧠 How It Works

Players simply type:

```bash
.startergear
```

The module then:

1. Checks whether the module is enabled
2. Validates the player's class
3. Checks if the character already used the command
4. Grants bags
5. Grants riding skills
6. Grants mount item
7. Grants Dread Pirate Ring
8. Grants class-specific heirlooms
9. Stores the character GUID in the database

---

## 🐴 Riding at Level 1

The module uses:

```cpp
Player::learnSpell()
```

This bypasses level validation without modifying the core.

The included SQL patch additionally changes:

```sql
RequiredLevel = 1
```

to Celestial Steed (ID:54811) it's the default mount item.
> if you want to change the mount in .conf file, you need to patch the new mount in your database, so level 1 player can learn the mount.

---

# 📦 Installation

## Requirements

- AzerothCore master branch
- C++17 compatible compiler
- Linux or Windows

---

## Clone the Module

```bash
cd /path/to/azerothcore/modules

git clone https://github.com/hypopheria2k/mod-startergear.git
```

---

## Re-run CMake

```bash
cmake -B build .
```

---

## Compile

### Linux

```bash
cd build
make -j$(nproc)
```

### Windows

```powershell
cmake --build . --config Release
```

---

## Install Config

```bash
cp modules/mod-startergear/conf/mod-startergear.conf.dist \
configs/modules/mod-startergear.conf
```

---

## Start Worldserver

```bash
./worldserver
```

SQL files will automatically be applied by AzerothCore.

---

# ⚙️ Configuration

Config location:

```text
configs/modules/mod-startergear.conf
```

---

## Example Config

```ini
StarterGear.Enable = 1

StarterGear.BagId = 41600
StarterGear.BagCount = 4

StarterGear.GiveRiding = 1
StarterGear.MountId = 54811

StarterGear.Class.1.Enable = 1
StarterGear.Class.2.Enable = 1
StarterGear.Class.3.Enable = 1
StarterGear.Class.4.Enable = 1
StarterGear.Class.5.Enable = 1
StarterGear.Class.6.Enable = 1
StarterGear.Class.7.Enable = 1
StarterGear.Class.8.Enable = 1
StarterGear.Class.9.Enable = 1
StarterGear.Class.11.Enable = 1
```

---

# 📋 Item Reference

## Chest & Shoulder Heirlooms

| Item ID | Name | Type |
|---|---|---|
| 48691 | Tattered Dreadmist Robe | Cloth |
| 48689 | Stained Shadowcraft Tunic | Leather |
| 48687 | Preened Ironfeather Breastplate | Leather |
| 48677 | Champion's Deathdealer Breastplate | Mail |
| 48683 | Mystical Vest of Elements | Mail |
| 48685 | Polished Breastplate of Valor | Plate |

---

## Weapon Heirlooms

| Item ID | Name | Type |
|---|---|---|
| 42947 | Dignified Headmaster's Charge | Staff |
| 42943 | Bloodied Arcanite Reaper | 2H Axe |
| 42948 | Devout Aurastone Hammer | 1H Mace |
| 48716 | Venerable Mass of McGowan | 1H Mace |
| 42945 | Venerable Dal'Rend's Sacred Charge | 1H Sword |
| 42944 | Balanced Heartseeker | Dagger |
| 42946 | Charmed Ancient Bone Bow | Bow |

---

# 🧙 Class Gear Overview

| Class | Gear Focus |
|---|---|
| Mage / Warlock / Priest | Cloth + Spellpower |
| Druid | Intellect + Agility |
| Rogue | Agility |
| Hunter | Ranged |
| Shaman | Hybrid |
| Warrior / DK / Paladin | Strength |

---

# 🗄️ Database Schema

```sql
CREATE TABLE IF NOT EXISTS `custom_startergear_used` (
    `guid` INT UNSIGNED NOT NULL,
    `used_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

---

# 🛠️ For Developers

## Structure

```text
mod-startergear/
├── conf/
├── data/
├── src/
├── CMakeLists.txt
└── README.md
```

---

## Goals

- Lightweight
- No core edits
- Fully configurable
- AzerothCore compliant
- Easy maintenance

---

# ❓ FAQ

<details>
<summary>Can players use it multiple times?</summary>

No. Usage is permanently tracked in the character database.

</details>

<details>
<summary>Does this require core modifications?</summary>

No. Everything works through the module API.

</details>

<details>
<summary>Does this support existing characters?</summary>

Yes. Every unused character can use it once.

</details>

---

# 📄 License

Licensed under the MIT License.

---

# ❤️ Built for the AzerothCore Community

> "Because every hero deserves a proper start."