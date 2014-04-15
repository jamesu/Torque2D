Torque2D
========

MIT Licensed Open Source version of Torque 2D from GarageGames.

You can find the main Wiki page [here](https://github.com/GarageGames/Torque2D/wiki)

This branch is an experimental branch embeds a lua interpreter alongside TorqueScript, with the ultimate intention of replacing the TorqueScript core.

Current limitations:

* Existing Console* thunk functions are reused, which may impact performance and type handling
* You currently cannot bind functions to ScriptObject classes or named objects since object namespaces are linked AFTER the object is added to the lua interpreter (refactoring is needed to resolve this)
* Lua functions currently are not called from object callbacks
* Since TorqueScript uses case insensitive field and function names, the case of equivalent names in lua may be inconsistent (this would ideally be fixed by making the StringTable case insensitive).

Have fun!