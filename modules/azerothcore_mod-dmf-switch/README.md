# Darkmoon faire Switch module

[English](README.md)

This module switches the Darkmoon faire event with another of your choice once it starts and stops them on Saturdays.

## Requirements

This module currently requires:

AzerothCore v1.0.2+

## How to install

### 1) Simply place the module under the `modules` folder of your AzerothCore source folder.

You can do clone it via git under the azerothcore/modules directory:

```sh
cd path/to/azerothcore/modules
git clone 
```


### 2) Re-run cmake and launch a clean build of AzerothCore

**That's it.**

### (Optional) Edit module configuration

If you need to change the module configuration, go to your server configuration folder (e.g. **etc**), copy `mod-dmf-switch.conf.dist` to `mod-dmf-switch.conf` and edit it as you prefer.

### Configuration Options

This module offers two options to select which items to automatically save once looted:

1) ModDMFSwitch.Enable = 0

       Description: Enables the module.
       Default:     0 - Disabled

2) ModCTASwitch.SwitchTerokkar

        Description: When the DMF event starts, switch it to another one specified in this config.
        Example:     ModDMFSwitch.SwitchTerokkar = 4 -- Elwynn
        Default:    ModDMFSwitch.SwitchTerokkar = 4 


## License

This module is released under the [GNU AGPL license](https://github.com/azerothcore/mod-transmog/blob/master/LICENSE)

## Authors

- [Nyeriah](https://github.com/Nyeriah) (Based on mod-cta-switch = https://github.com/azerothcore/mod-cta-switch)
- Temperrr
