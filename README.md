# cyberdeck / pda thingy

hi! this is my little thing im working on.

## download symbols/footprints

to open the kicad project, youll need to download some footprints. my parts are sourced from JLCPCB's parts / LCSC, and you can use `easyeda2kicad` to download LCSC parts to kicad.

the command you need to run is `easyeda2kicad --full --lcsc_id C318884 C191023 C22435642 C13738`

which will download footprints/symbols/3d models for the switches, diodes, the 14P FPC connector, and the crystal.
