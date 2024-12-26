# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## mod-ip2config

This module holds the SQL information to populate the `ip2config` & `ip2nationcountries` tables in your auth database.

The data is gathered from http://www.ip2nation.com/ip2nation

AzerothCore or the author of the module does not take any credit for the data that is provided within the SQL files. All the credit for this data goes to http://www.ip2nation.com/

## Import into database

There are two alternatives to import this into your auth database, either manually or using the DB assambler.

### Manually

You can download the SQL files and manually import them from https://github.com/azerothcore/mod-ip2nation/releases/tag/v1.0

### DB assambler

You can also follow the normal procedure of using modules by installing it in your `azerothcore-wotlk/modules/` directory and then importing them by running the
DB assambler to using `customs` or `import-customs` options.

NOTE: You do not need to reconfigure the project with this module as it does not add any sourcefiles.
