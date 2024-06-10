# Call to Arms! Switch module

[English](README.md) | [Español](README_ES.md)

This module switches the Call to Arms! PvP event with another of your choice once it starts and stops them on Tuesdays.

## Requirements

This module currently requires:

AzerothCore v1.0.2+

## How to install

### 1) Simply place the module under the `modules` folder of your AzerothCore source folder.

You can do clone it via git under the azerothcore/modules directory:

```sh
cd path/to/azerothcore/modules
git clone https://github.com/azerothcore/mod-cta-switch.git
```


### 2) Re-run cmake and launch a clean build of AzerothCore

**That's it.**

### (Optional) Edit module configuration

If you need to change the module configuration, go to your server configuration folder (e.g. **etc**), copy `mod-cta-switch.conf.dist` to `mod-cta-switch.conf` and edit it as you prefer.

### Configuration Options

This module offers two options to select which items to automatically save once looted:

1) ModCTASwitch.Enable = 0

       Description: Enables the module.
       Default:     0 - Disabled

2) ModCTASwitch.SwitchEots, ModCTASwitch.SwitchSota, ModCTASwitch.SwitchIoc

        Description: When the Call to Arms! event starts, switch it to another one specified in this config.
        Example:     ModCTASwitch.SwitchEots = 19 -- Call to Arms! Eye of the Storm will be replaced with Call to Arms! Warsong Gulch.
        Default:    ModCTASwitch.SwitchEots = 19 (Warsong Gulch)
                    ModCTASwitch.SwitchSota = 20 (Arathi Basin)
                    ModCTASwitch.SwitchIoc = 18 (Alterac Valley)


## License

This module is released under the [GNU AGPL license](https://github.com/azerothcore/mod-transmog/blob/master/LICENSE)

## Authors

- [Nyeriah](https://github.com/Nyeriah)
