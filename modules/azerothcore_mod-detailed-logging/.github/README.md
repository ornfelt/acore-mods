# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## Azerothcore Detailed Logging Module

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-detailed-logging/workflows/core-build/badge.svg)](https://github.com/azerothcore/mod-detailed-logging)

The detailed logging module will provide additional logging for certain events (kills, deaths, movement) that will help facilitate future modules and applications via activities such as:

- Data Mining
- Machine Learning
- Statistical Feedback

### Usage

- Once this module is added to AzerothCore and configured you will see three new log files
    - added to the bin folder:  kills.log, deaths.log and zonearea.log
- This module provides no additional functionality other than enhanced logging.  Any further
    - utility will need to be created with additional modules, applications or code.

### File Description

| File Name        | Description           |
| ------------- |----------------|
| kills.log      | The kills.log will record vital statistics whenever a creature is killed by a player or their pet/totem |
| deaths.log     | The Deaths.log will record vital statistics whenever a player is killed by a creature      |
| zonearea.log | The zonearea.log will record vital statistics whenever a player moves to a new area and/or zone      |
