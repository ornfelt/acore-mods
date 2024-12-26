# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## Promotion - Module template

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-promotion-AzerothCore-/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-promotion-AzerothCore-)

[English](README.md) | [Español](README_ES.md)

### What settings does this module have?

Allows the owner to edit the armor they want without having to edit it in the source code

-------------------------------------------------------

# Promotion Module (AzerothCore)

## Description

This module allows you to do this and this.
This module allows the player to give you a promotion that consists of the following.
* 1 Level 90 Character per account
* 4 Backpacks 
* 2,500G of gold
* Level 78 armor
* Mount 1

## Requirements

My module requires:

- AzerothCore

## Installation

```
1) Simply place the module into the AzerothCore `modules` directory (repository), not the compilation.
2) Import the SQL manually to the correct database (auth, world or characters) or with the `db_assembler.sh` (if `include.sh` is provided).
3) Run Cmake again and generate the necessary compilation. (Check the guide)
```

## Edit module configuration (optional)

If you need to change the module configuration, go to your server configuration folder (where your `worldserver` or `worldserver.exe` is), copy `mod_promotion.conf.dist` and edit that new file.

## Créditos

* [Asmadeuxx](https://github.com/Asmadeuxx) (module author) Edit the link to point to your github if you wish.

AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
