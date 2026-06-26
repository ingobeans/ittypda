# cyberdeck / pda thingy
<img src="render/render.png" width=800>

hi! this is my little thing im working on.

## download symbols/footprints

to open the kicad project, youll need to download some footprints. my parts are sourced from JLCPCB's parts / LCSC, and you can use `easyeda2kicad` to download LCSC parts to kicad.

the command you need to run is `easyeda2kicad --full --lcsc_id C318884 C191023 C22435642 C13738 C81080 C49023767`

which will download footprints/symbols/3d models for the switches, diodes, the 14P FPC connector, the crystal, and the battery charging circuit.
