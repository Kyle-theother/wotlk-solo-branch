Vendored from https://github.com/AlsoNotMehh/mod-arac-enhanced
Commit: 1ab183d8e2d620ccd7a96ccec65786fd84cc2fb5
License: MIT (see LICENSE)

Undead paladin is enabled by default (`ARAC.Undead.Paladin = 1`).

The character screen will stay grey until the 3.3.5a client loads a patch.
Copy this file from the upstream repo into the client Data folder:

https://github.com/AlsoNotMehh/mod-arac-enhanced/raw/main/client-patch/Patch-A.MPQ

Destination: `<WoW 3.3.5a>/Data/Patch-A.MPQ`
If that name is already taken, rename it to `Patch-B.MPQ`.
Then delete the client Cache folder and reopen character creation.

Server DBC files (CharBaseInfo.dbc, CharStartOutfit.dbc, SkillRaceClassInfo.dbc)
also live in the upstream `client-patch/patch-contents/DBFilesClient/` directory.
Copy those into the server data/dbc folder if new characters spawn naked.
