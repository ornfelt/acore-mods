# Módulo de cambio: ¡Llamada a las armas!

[English](README.md) | [Español](README_ES.md)

Este módulo cambia el evento de ¡Llamada a las armas! con otro evento PvP de tu elección una vez que comienza y los detiene los martes.

## Requisitos

Este módulo requiere actualmente:

AzerothCore v1.0.2+

## Instalación

### 1) Simplemente coloca el módulo en la carpeta `modules` de tu carpeta de fuentes de AzerothCore.

Puedes clonarlo vía git en el directorio azerothcore/modules:

```sh
cd path/to/azerothcore/modules
git clone https://github.com/Nyeriah/mod-cta-switch.git
```

### 2) Ejecuta de nuevo cmake y lanza una compilación limpia de AzerothCore

**Eso es todo.**

### (Opcional) Editar la configuración del módulo

Si necesitas cambiar la configuración del módulo, ve a la carpeta de configuración de tu servidor (por ejemplo, **etc**), copia `mod-cta-switch.conf.dist` a `mod-cta-switch.conf` y edítalo como prefieras.

### Opciones de configuración

Este módulo ofrece dos opciones para seleccionar qué objetos guardar automáticamente una vez saqueados:

```
1) ModCTASwitch.Enable = 0

    Description:    Activa el módulo.

    Default:        0 - Disabled

2) ModCTASwitch.SwitchEots, ModCTASwitch.SwitchSota, ModCTASwitch.SwitchIoc

    Description:    Cuando se inicie el evento ¡Llamada a las armas!, cámbialo por otro especificado en esta configuración.

    Example:        ModCTASwitch.SwitchEots = 19 -- Call to Arms! Eye of the Storm will be replaced with Call to Arms! Warsong Gulch.

    Default:        ModCTASwitch.SwitchEots = 19 (Warsong Gulch)
                    ModCTASwitch.SwitchSota = 20 (Arathi Basin)
                    ModCTASwitch.SwitchIoc = 18 (Alterac Valley)
```

## Licencia

Este módulo está publicado bajo la [licencia GNU AGPL](https://github.com/azerothcore/mod-transmog/blob/master/LICENSE)

## Autores

- [Nyeriah](https://github.com/Nyeriah)
