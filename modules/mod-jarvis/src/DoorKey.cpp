#include "GameObject.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Item.h"
#include "ItemScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "CellImpl.h"

class item_dungeon_key : public ItemScript
{
public:
    item_dungeon_key() : ItemScript("item_dungeon_key") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        std::list<GameObject*> doors;
        Acore::GameObjectInRangeCheck check(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 12.0f);
        Acore::GameObjectListSearcher<Acore::GameObjectInRangeCheck> searcher(player, doors, check);
        Cell::VisitObjects(player, searcher, 12.0f);

        uint32 opened = 0;
        for (GameObject* door : doors)
        {
            if (!door || (door->GetGoType() != GAMEOBJECT_TYPE_DOOR && door->GetGoType() != GAMEOBJECT_TYPE_BUTTON && door->GetGoType() != GAMEOBJECT_TYPE_CHEST))
                continue;
            if (!door->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED) && door->GetGoState() == GO_STATE_ACTIVE)
                continue;
            door->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            door->SetGoState(GO_STATE_ACTIVE);
            ++opened;
        }
        if (opened)
            ChatHandler(player->GetSession()).PSendSysMessage("The key opens {} locked door(s).", opened);
        else
            ChatHandler(player->GetSession()).SendSysMessage("No locked door is close enough.");
        return false;
    }
};

void AddJarvisKeyScripts()
{
    new item_dungeon_key();
}
