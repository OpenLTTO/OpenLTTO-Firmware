This component should contain one or more classes that handle all LTTO Game Logic
- Hosting
- Health
- Ammo
- Damage
- Respawning
- Zones
etc...

A bit of work to come up with a meaningful class structure which abstracts these concepts out is needed
I suspect we will have some classes like:
- Game - a universal baseclass which holds overall hosted game stuff (where the hosting takes place, players join, stats are collected, etc)
- GameType classes, inheriting from the Game baseclass, and deriving any custom features/logic of the gametype
- Equipment - a base class used to support equipment functionality in game, ie: taggers, grenades, health packs, respawn points, etc
Possibly others as appropriate

This component relies on the underlying esp32_IR_LTTO lib, to talk the LTTO Protocol. And it abstracts it away, so that the higher level components don't need to think about the protocol at all.
