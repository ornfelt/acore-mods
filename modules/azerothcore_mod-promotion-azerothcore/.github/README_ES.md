 [English](README.md) | [Español](README_ES.md)

### ¿Que Configuraciones Tiene Este Modulo?

Permite Al Owner, Editar Si Permite La Promocion, La Armadura Que Quiera Sin Necesidad De Editarlo En El Codigo Fuente

-------------------------------------------------------

# Modulo De Promoción (AzerothCore)

## Descripción

Este módulo permite hacer esto y esto.

Este Modulo Permite al jugador darle una promocion que consta de lo siguiente.

* 1 Personaje Level 90 Por Cuenta
* 4 Mochilas 
* 2,500G De Oro
* Armadura de Nivel 78
* Una Montura

## Cómo utilizar

Haz esto y aquello.

Puedes agregar una carpeta de pantalla:

[screenshot](/screenshots/my_module.png?raw=true "screenshot")

O incluso un video donde expliques su uso:

[Youtube](https://www.youtube.com/watch?v=T6UEX47mPeE)

## Requisitos

Mi módulo requiere:

- AzerothCore 

## Instalación

```
1) Simplemente coloque el módulo dentro del directorio `modules` de AzerothCore (repositorio), no la compilación.
2) Importe el SQL manualmente a la base de datos correcta (auth, mundo o caracteres) o con el `db_assembler.sh` (si se proporciona `include.sh`).
3) Vuelva a ejecutar el Cmake y genere la compilación necesaria. (Revise la guía)
```

## Editar la configuración del módulo (opcional)

Si necesita cambiar la configuración del módulo, vaya a la carpeta de configuración de su servidor (donde está su `worldserver` o `worldserver.exe`), copie `mod_promotion.conf.dist` y edite ese nuevo archivo.

## Créditos

* [Yo](https://github.com/Asmadeuxx) (autor del módulo) Edite el enlace para que apunte a su github si lo desea.

AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
