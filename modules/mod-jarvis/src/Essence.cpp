#include "ItemScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"

class item_essence_of_immortals : public ItemScript
{
public:
    item_essence_of_immortals() : ItemScript("item_essence_of_immortals") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        uint32 const buffs[] = { 48470, 48162, 48074, 43002, 48934, 48938, 25898, 47436 };
        for (uint32 spell : buffs)
            player->AddAura(spell, player);
        return true;
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
}
