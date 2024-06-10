/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "Config.h"
#include "Chat.h"
#include "TaskScheduler.h"

enum Texts
{
    SAY_INTRO   = 0,
    SAY_KILL    = 1,
    SAY_DESPAWN = 2
};

enum Spells
{
    SPELL_SHADOW_VOLLEY = 30354,
    SPELL_CLEAVE = 31779,
    SPELL_THUNDERCLAP = 36706,
    SPELL_VOID_BOLT = 21066,
    SPELL_MARK_OF_KAZZAK = 21056,
    SPELL_MARK_OF_KAZZAK_DAMAGE = 21058,
    SPELL_ENRAGE = 32964,
    SPELL_CAPTURE_SOUL = 32966,
    SPELL_TWISTED_REFLECTION = 21063,

    //
    SPELL_SUMMON_FLAMES = 19629,
    SPELL_INFERNALING_SPAWNIN = 33543
};

enum Events
{
    EVENT_SHADOW_VOLLEY = 1,
    EVENT_CLEAVE = 2,
    EVENT_THUNDERCLAP = 3,
    EVENT_VOID_BOLT = 4,
    EVENT_MARK_OF_KAZZAK = 5,
    EVENT_ENRAGE = 6,
    EVENT_TWISTED_REFLECTION = 7,
    EVENT_BERSERK = 8
};

enum Phases
{
    PHASE_NORMAL = 1,
    PHASE_OUTRO = 2
};

enum Misc
{
    GO_CONSUMING_FLAMES = 178672,
    NPC_MASSIVE_INFERNAL = 19214,
    NPC_INFERNAL_HOUND = 19207
};

class boss_highlord_kruul : public CreatureScript
{
public:
    boss_highlord_kruul() : CreatureScript("boss_highlord_kruul") { }

    struct boss_highlord_kruulAI : public ScriptedAI
    {
        boss_highlord_kruulAI(Creature* creature) : ScriptedAI(creature)
        {
            _supremeMode = false;
            _intro = false;
        }

        void Reset() override
        {
            _events.Reset();
            _supremeMode = false;

            if (!sConfigMgr->GetOption<bool>("ModBlackPortal.Enable", true))
            {
                me->DisappearAndDie();
                return;
            }

            if (!_intro)
            {
                Talk(SAY_INTRO);

                me->SummonCreature(NPC_MASSIVE_INFERNAL, me->GetNearPosition(15.0f, 1.0f));
                me->SummonCreature(NPC_MASSIVE_INFERNAL, me->GetNearPosition(15.0f, -1.0f));

                Position pos = me->GetNearPosition(15.0f, 1.0f);

                if (GameObject* fires = me->SummonGameObject(GO_CONSUMING_FLAMES, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2 * HOUR * IN_MILLISECONDS))
                {
                    _gobGuids.insert(fires->GetGUID());
                }
                pos = me->GetNearPosition(15.0f, -1.0f);

                if (GameObject* fires = me->SummonGameObject(GO_CONSUMING_FLAMES, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2 * HOUR * IN_MILLISECONDS))
                {
                    _gobGuids.insert(fires->GetGUID());
                }

                me->GetMotionMaster()->MoveRandom(10.0f);
                _intro = true;

                _scheduler.CancelAll();
                _scheduler.Schedule(2h, [this](TaskContext context)
                {
                    if (!me->IsInCombat())
                    {
                        Talk(SAY_DESPAWN);
                        DespawnMinionsAndFlames();
                    }
                    else
                    {
                        context.Repeat(5s);
                    }
                }).Schedule(10s, [this](TaskContext context)
                {
                    if (!me->IsInCombat())
                    {
                        DoCastAOE(SPELL_SUMMON_FLAMES);
                    }
                    context.Repeat();
                }).Schedule(10s, [this](TaskContext context)
                {
                    if (Creature* hound = me->SummonCreature(NPC_INFERNAL_HOUND, me->GetRandomNearPosition(15.0f), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5 * MINUTE * IN_MILLISECONDS))
                    {
                        hound->CastSpell(hound, SPELL_INFERNALING_SPAWNIN);
                    }
                    context.Repeat(1min);
                });
            }
        }

        void DespawnMinionsAndFlames()
        {
            for (ObjectGuid guid : _minionGuids)
            {
                if (Creature* minion = ObjectAccessor::GetCreature(*me, guid))
                {
                    minion->DespawnOrUnsummon();
                }
            }

            for (ObjectGuid guid : _gobGuids)
            {
                if (GameObject* flames = ObjectAccessor::GetGameObject(*me, guid))
                {
                    flames->DespawnOrUnsummon();
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            _minionGuids.insert(summon->GetGUID());
            summon->SetFaction(me->GetFaction());
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(6000, 10000));
            _events.ScheduleEvent(EVENT_CLEAVE, 7000);
            _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(14000, 18000));
            _events.ScheduleEvent(EVENT_VOID_BOLT, 30000);
            _events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 25000);
            _events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 33000);
            _events.ScheduleEvent(EVENT_BERSERK, 60000);
        }

        void KilledUnit(Unit* victim) override
        {
            if (victim->IsPlayer())
            {
                DoCast(me, SPELL_CAPTURE_SOUL);
                Talk(SAY_KILL, victim);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _scheduler.Update(diff);
            // Return since we have no target
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHADOW_VOLLEY:
                    DoCastVictim(SPELL_SHADOW_VOLLEY);
                    if (!_supremeMode)
                        _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(4000, 30000));
                    else
                        _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, 1000);
                    break;
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    _events.ScheduleEvent(EVENT_CLEAVE, urand(8000, 12000));
                    break;
                case EVENT_THUNDERCLAP:
                    DoCastVictim(SPELL_THUNDERCLAP);
                    _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(10000, 14000));
                    break;
                case EVENT_VOID_BOLT:
                    DoCastVictim(SPELL_VOID_BOLT);
                    _events.ScheduleEvent(EVENT_VOID_BOLT, urand(15000, 18000));
                    break;
                case EVENT_MARK_OF_KAZZAK:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 1, [&](Unit* u) { return u && !u->IsPet() && u->getPowerType() == POWER_MANA; }))
                        DoCast(target, SPELL_MARK_OF_KAZZAK);
                    _events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 20000);
                    break;
                case EVENT_ENRAGE:
                    DoCast(me, SPELL_ENRAGE);
                    _events.ScheduleEvent(EVENT_ENRAGE, 60000);
                    break;
                case EVENT_TWISTED_REFLECTION:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 1, 0.0f, true))
                        DoCast(target, SPELL_TWISTED_REFLECTION);
                    _events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 15000);
                    break;
                case EVENT_BERSERK:
                    _supremeMode = true;
                    _events.ScheduleEvent(EVENT_SHADOW_VOLLEY, 1000);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/) override
        {
            _intro = false;
            DespawnMinionsAndFlames();
            _scheduler.CancelAll();
        }

    private:
        GuidSet _minionGuids;
        GuidSet _gobGuids;
        EventMap _events;
        TaskScheduler _scheduler;
        bool _supremeMode;
        bool _intro;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_highlord_kruulAI(creature);
    }
};

enum BlackportalEvent
{
    NPC_ARGENT_PROTECTOR     = 19320,
    NPC_THUNDERBLUFF_HUNTER  = 19406,
    NPC_AZUREMYST_VINDICATOR = 19407,
    NPC_ARGENT_HUNTER        = 19366,
    NPC_ARGENT_BOWSMAN       = 19365,
    NPC_STORMWIND_MARSHAL    = 19386,

    NPC_JUSTINIUS            = 18966,
    NPC_MELGROMM             = 18969,
    NPC_RAYNOR               = 19385,

    // Demons
    NPC_INVADING_FELGUARD   = 19284,
    NPC_INVADING_FELHUNTER  = 19286,
    NPC_INVADING_VOIDWALKER = 19287,
    NPC_INVADING_ANGUISHER  = 19290,
    NPC_INVADING_INFERNAL   = 19285,
    NPC_FELGUARD_LIEUTENANT = 19391,
    NPC_DREADKNIGHT         = 19288,
    NPC_PORTAL_HOUND        = 19311,
    NPC_PORTAL_BOSS         = 192880,

    ACTION_START_EVENT = 1,
    ACTION_RESET       = 2,
    ACTION_INFERNAL_STORM = 3,
    ACTION_START_POST_EVENT = 4,
    ACTION_SPAWN_MINIONS    = 5,

    GROUP_COMBAT       = 1,

    SAY_START_EVENT    = 0,
    SAY_RAYNOR_RANDOM  = 1,
    SAY_RAYNOR_DESPAWN = 2
};

enum BlackportalSpells
{
    SPELL_BP_CLEAVE       = 20684,
    SPELL_INFERNAL_RAIN   = 32785,
    SPELL_GREATER_MIGHT   = 33564,
};

class npc_marshal_raynor : public CreatureScript
{
public:
    npc_marshal_raynor() : CreatureScript("npc_marshal_raynor") { }

    struct npc_marshal_raynorAI : public ScriptedAI
    {
        npc_marshal_raynorAI(Creature* creature) : ScriptedAI(creature), _scheduledText(false) { }

        void Reset() override
        {
            if (!_scheduledText)
            {
                _scheduler.Schedule(5s, [this](TaskContext context)
                {
                    if (!me->IsInCombat())
                    {
                        Talk(SAY_RAYNOR_RANDOM);
                    }

                    context.Repeat(1min, 5min);
                });

                _scheduledText = true;

                if (sConfigMgr->GetOption<bool>("ModBlackPortal.EnablePostEvent", false))
                {
                    SetData(ACTION_START_POST_EVENT, 0);
                }
            }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _scheduler.CancelGroup(GROUP_COMBAT);

            _scheduler.Schedule(5s, [this](TaskContext context)
            {
                DoCastVictim(SPELL_BP_CLEAVE);
                context.SetGroup(GROUP_COMBAT);
                context.Repeat(3s, 8s);
            });
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellSchoolMask /*damageSchoolMask*/) override
        {
            if (damage >= me->GetHealth())
            {
                Talk(SAY_RAYNOR_DESPAWN);
                damage = me->GetHealth() - 1;
                me->SetVisible(false);
                me->RemoveAllAuras();
                me->CombatStop();
                me->SetFullHealth();

                _scheduler.Schedule(30s, [this](TaskContext context)
                {
                    me->SetVisible(true);
                });
            }
        }

        void SetData(uint32 data, uint32 index) override
        {
            Position const _infernalTriggerPos[4] =
            {
                { -11881.094727f, -3231.752930f, 31.772581f, 0.233761f },
                { -11889.815430f, -3181.601807f, 20.224068f, 0.085571f },
                { -11869.617188f, -3198.848877f, 24.405516f, 0.148387f },
                { -11868.207031f, -3223.622070f, 19.319908f, 0.152284f }
            };

            Position const _infernalDestPos[4] =
            {
                { -11815.575195f, -3222.292725f, -30.980101f, 0.278040f },
                { -11836.335938f, -3178.535400f, -29.808310f, 0.262335f },
                { -11836.042969f, -3197.993408f, -29.986420f, 0.187248f },
                { -11808.514648f, -3204.217285f, -29.069098f, 0.109488f }
            };

            if (data == ACTION_START_EVENT)
            {
                _summonGuids.clear();
                _deadMinionCounter = 0;
                _dreadknightCounter = 0;
                _bossSpawned = false;

                SummonFriendlyNPCs();

                Talk(SAY_START_EVENT);

                _scheduler.Schedule(5s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_INVADING_FELGUARD, 0);
                    SummonPortalDemon(NPC_INVADING_FELGUARD, 1);
                    context.Repeat();
                });

                _scheduler.Schedule(12s, [this](TaskContext context)
                {
                    if (roll_chance_i(50))
                    {
                        SummonPortalDemon(NPC_INVADING_FELHUNTER, 0);
                        SummonPortalDemon(NPC_INVADING_FELHUNTER, 1);
                    }
                    else
                    {
                        SummonPortalDemon(NPC_PORTAL_HOUND, 0);
                        SummonPortalDemon(NPC_PORTAL_HOUND, 1);
                    }

                    context.Repeat();
                });

                _scheduler.Schedule(18s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_INVADING_ANGUISHER, 0);
                    SummonPortalDemon(NPC_INVADING_ANGUISHER, 1);
                    context.Repeat();
                });

                _scheduler.Schedule(30s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_FELGUARD_LIEUTENANT, 2);
                    context.Repeat();
                });

                _scheduler.Schedule(1min, [this](TaskContext context)
                {
                    if (Creature* justinius = me->FindNearestCreature(NPC_JUSTINIUS, 500.0f))
                    {
                        justinius->CastSpell(justinius, SPELL_GREATER_MIGHT);
                    }
                    context.Repeat();
                });

                _scheduler.Schedule(35s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_INVADING_VOIDWALKER, 2);
                    context.Repeat();
                });

                _scheduler.Schedule(10s, [this, _infernalTriggerPos, _infernalDestPos](TaskContext context)
                {
                    for (uint8 i = 0; i < 4; i++)
                    {
                        SummonInfernalRain(_infernalTriggerPos[i], _infernalDestPos[i]);
                    }
                });

                _scheduler.Schedule(20s, [this, _infernalTriggerPos, _infernalDestPos](TaskContext context)
                {
                    for (uint8 i = urand(0, 3); i < 4; i++)
                    {
                        SummonInfernalRain(_infernalTriggerPos[i], _infernalDestPos[i]);
                    }

                    context.Repeat(30s, 60s);
                });
            }
            else if (data == ACTION_RESET)
            {
                for (ObjectGuid guid : _summonGuids)
                {
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, guid))
                    {
                        creature->DespawnOrUnsummon();
                    }
                }

                _scheduler.CancelAll();
            }
            else if (data == ACTION_INFERNAL_STORM)
            {
                for (uint8 i = 0; i < 4; i++)
                {
                    SummonInfernalRain(_infernalTriggerPos[i], _infernalDestPos[i]);
                }
            }
            else if (data == ACTION_START_POST_EVENT)
            {
                SetData(ACTION_RESET, 0);
                SummonFriendlyNPCs();

                _scheduler.Schedule(30s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_INVADING_FELGUARD, 0);
                    SummonPortalDemon(NPC_INVADING_FELGUARD, 1);
                    _deadMinionCounter = 0;
                    context.Repeat();
                });

                _scheduler.Schedule(38s, [this](TaskContext context)
                {
                    if (roll_chance_i(50))
                    {
                        SummonPortalDemon(NPC_INVADING_FELHUNTER, 0);
                        SummonPortalDemon(NPC_INVADING_FELHUNTER, 1);
                    }
                    else
                    {
                        SummonPortalDemon(NPC_PORTAL_HOUND, 0);
                        SummonPortalDemon(NPC_PORTAL_HOUND, 1);
                    }

                    context.Repeat();
                });

                _scheduler.Schedule(45s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_INVADING_ANGUISHER, 0);
                    SummonPortalDemon(NPC_INVADING_ANGUISHER, 1);
                    context.Repeat();
                });

                _scheduler.Schedule(60s, [this](TaskContext context)
                {
                    SummonPortalDemon(NPC_FELGUARD_LIEUTENANT, 2);
                    context.Repeat();
                });

                _scheduler.Schedule(20s, [this, _infernalTriggerPos, _infernalDestPos](TaskContext context)
                {
                    for (uint8 i = urand(0, 3); i < 4; i++)
                    {
                        SummonInfernalRain(_infernalTriggerPos[i], _infernalDestPos[i]);
                    }

                    context.Repeat(30s, 60s);
                });
            }
            else if (data == ACTION_SPAWN_MINIONS)
            {
                std::vector minions = { NPC_INVADING_FELGUARD, NPC_INVADING_ANGUISHER, NPC_INVADING_FELHUNTER, NPC_INVADING_VOIDWALKER, NPC_FELGUARD_LIEUTENANT };

                if (index > 4)
                {
                    index = 0;
                }

                SummonPortalDemon(minions[index], 0);
                SummonPortalDemon(minions[index], 1);
            }
        }

        void SummonInfernalRain(Position pos, Position dest)
        {
            if (Creature* trigger = me->SummonCreature(22515, pos))
            {
                trigger->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                trigger->SetDisableGravity(true);
                trigger->CastSpell(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), SPELL_INFERNAL_RAIN, false);
                trigger->DespawnOrUnsummon(1000);

                me->m_Events.AddEventAtOffset([this, dest]()
                {
                    me->SummonCreature(NPC_INVADING_INFERNAL, dest);
                }, 5s);
            }
        }

        void SummonFriendlyNPCs()
        {
            Position const _argentProtectorPositions[9] =
            {
                { -11808.666992f, -3206.407227f, -29.388878f, 3.431497f },
                { -11809.618164f, -3203.834473f, -29.247328f, 3.348245f },
                { -11810.923828f, -3201.016846f, -29.976776f, 3.538312f },
                { -11797.455078f, -3203.081055f, -28.437138f, 3.457416f },
                { -11798.879883f, -3199.793701f, -28.107952f, 3.543025f },
                { -11800.028320f, -3196.452637f, -28.509819f, 3.538312f },
                { -11805.366211f, -3198.688721f, -29.207794f, 3.538312f },
                { -11803.918945f, -3201.932617f, -28.675598f, 3.543025f },
                { -11802.583984f, -3204.756836f, -28.891459f, 3.457416f },
            };

            Position const _stormwindMarshals[6] =
            {
                { -11832.259766f, -3196.820801f, -30.171431f, 3.247656f },
                { -11832.734375f, -3204.129883f, -30.501616f, 3.132202f },
                { -11832.288086f, -3199.888916f, -30.269533f, 3.198176f },
                { -11835.124023f, -3199.776855f, -30.081173f, 3.213420f },
                { -11834.855469f, -3196.740723f, -30.022821f, 3.156872f },
                { -11834.820312f, -3203.186523f, -30.302736f, 3.144305f },
            };

            Position const _thunderbluffHunters[6] =
            {
                { -11843.174805f, -3176.412598f, -28.853539f, 5.226863f },
                { -11850.358398f, -3176.458496f, -28.090700f, 4.995171f },
                { -11855.361328f, -3176.151611f, -27.311512f, 5.097272f },
                { -11840.394531f, -3170.019287f, -29.219015f, 4.629200f },
                { -11848.649414f, -3169.063477f, -28.248203f, 4.783135f },
                { -11855.245117f, -3166.247803f, -27.773533f, 4.892200f }
            };

            Position const _azuremystVindicatorPos[6] =
            {
                { -11852.507812f, -3230.814941f, -25.037802f, 1.870232f },
                { -11848.397461f, -3229.869629f, -25.696739f, 1.501095f },
                { -11843.356445f, -3229.766113f, -26.406046f, 1.660531f },
                { -11842.769531f, -3236.298584f, -25.826141f, 1.660531f },
                { -11848.006836f, -3235.759766f, -25.197475f, 1.596914f },
                { -11854.534180f, -3234.660645f, -24.871737f, 1.314171f }
            };

            Position const _argentHunter[6] =
            {
                { -11823.658203f, -3186.115479f, -30.234430f, 3.629969f },
                { -11825.404297f, -3183.600830f, -29.367455f, 3.649604f },
                { -11827.452148f, -3181.226807f, -29.720917f, 3.698299f },
                { -11824.925781f, -3168.900391f, -30.088284f, 4.053295f },
                { -11827.259766f, -3166.896240f, -30.524405f, 4.057221f },
                { -11829.310547f, -3165.329834f, -30.430052f, 4.057221f }
            };

            Position const _argentBowsman[6] =
            {
                { -11822.590820f, -3202.611816f, -31.032156f, 3.263978f },
                { -11822.884766f, -3199.638184f, -30.985357f, 3.256124f },
                { -11823.301758f, -3196.711182f, -30.834106f, 3.256124f },
                { -11819.934570f, -3220.567383f, -31.033857f, 2.770744f },
                { -11818.669922f, -3218.126953f, -31.196154f, 2.832006f },
                { -11818.377930f, -3215.409668f, -31.301699f, 2.847713f }
            };

            for (Position const pos : _argentProtectorPositions)
            {
                me->SummonCreature(NPC_ARGENT_PROTECTOR, pos);
            }

            for (Position const pos : _stormwindMarshals)
            {
                me->SummonCreature(NPC_STORMWIND_MARSHAL, pos);
            }

            for (Position const pos : _thunderbluffHunters)
            {
                me->SummonCreature(NPC_THUNDERBLUFF_HUNTER, pos);
            }

            for (Position const pos : _azuremystVindicatorPos)
            {
                me->SummonCreature(NPC_AZUREMYST_VINDICATOR, pos);
            }

            for (Position const pos : _argentHunter)
            {
                me->SummonCreature(NPC_ARGENT_HUNTER, pos);
            }

            for (Position const pos : _argentBowsman)
            {
                me->SummonCreature(NPC_ARGENT_BOWSMAN, pos);
            }
        }

        void SummonPortalDemon(uint32 npc, uint8 pos)
        {
            Position const _demonSpawnPos[3] =
            {
                { -11891.429688f, -3214.041260f, -14.628236f, 0.164177f },
                { -11892.669922f, -3199.839111f, -14.652026f, 0.117053f },
                { -11894.831055f, -3207.194092f, -14.628990f, 0.245866f }
            };

            Position const _demonDestPos[3] =
            {
                { -11840.213867f, -3211.273926f, -29.908098f, 0.146900f },
                { -11843.071289f, -3192.713135f, -29.261871f, 0.289843f },
                { -11840.541016f, -3200.893555f, -29.749325f, 0.102138f }
            };

            if (Creature* summon = me->SummonCreature(npc, _demonSpawnPos[pos]))
            {
                summon->GetMotionMaster()->MovePoint(1, _demonDestPos[pos]);
                summon->SetHomePosition(_demonDestPos[pos]);

                if (summon->GetEntry() == NPC_PORTAL_BOSS)
                {
                    summon->AI()->Talk(SAY_INTRO);
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            _summonGuids.insert(summon->GetGUID());
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
                case NPC_INVADING_FELGUARD:
                case NPC_INVADING_FELHUNTER:
                case NPC_INVADING_ANGUISHER:
                case NPC_INVADING_INFERNAL:
                case NPC_INVADING_VOIDWALKER:
                case NPC_FELGUARD_LIEUTENANT:
                case NPC_PORTAL_HOUND:
                    summon->DespawnOrUnsummon(5000);
                    _deadMinionCounter++;
                    break;
                case NPC_ARGENT_PROTECTOR:
                case NPC_THUNDERBLUFF_HUNTER:
                case NPC_AZUREMYST_VINDICATOR:
                case NPC_ARGENT_BOWSMAN:
                case NPC_ARGENT_HUNTER:
                case NPC_STORMWIND_MARSHAL:
                {
                    ObjectGuid guid = summon->GetGUID();
                    _scheduler.Schedule(5s, [this, guid](TaskContext /*context*/)
                    {
                        if (Creature* defender = ObjectAccessor::GetCreature(*me, guid))
                        {
                            defender->Respawn();
                        }
                    });
                    break;
                }
                case NPC_PORTAL_BOSS:
                    _scheduler.CancelAll();

                    if (Creature* justinius = me->FindNearestCreature(NPC_JUSTINIUS, 500.0f))
                    {
                        justinius->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                        justinius->CombatStop();
                        justinius->GetMotionMaster()->MovePoint(0, Position(-11891.429688f, -3214.041260f, -14.628236f, 0.164177f));
                        justinius->DespawnOrUnsummon(60000);
                    }

                    if (Creature* melgromm = me->FindNearestCreature(NPC_MELGROMM, 500.0f))
                    {
                        melgromm->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                        melgromm->CombatStop();
                        melgromm->GetMotionMaster()->MovePoint(0, Position(-11892.669922f, -3199.839111f, -14.652026f, 0.117053f));
                        melgromm->DespawnOrUnsummon(60000);
                    }
                    return;
            }

            if (!sConfigMgr->GetOption<bool>("ModBlackPortal.EnablePostEvent", false))
            {
                if (_deadMinionCounter > 50 && !_bossSpawned)
                {
                    if (_dreadknightCounter > 2)
                    {
                        SummonPortalDemon(NPC_PORTAL_BOSS, 2);
                        _bossSpawned = true;
                        _deadMinionCounter = 0;
                        _scheduler.DelayAll(1min);
                        return;
                    }

                    _scheduler.DelayAll(30s);
                    SummonPortalDemon(NPC_DREADKNIGHT, 2);
                    _deadMinionCounter = 0;
                    _dreadknightCounter++;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _scheduler.Update(diff);

            if (!UpdateVictim())
            {
                return;
            }

            DoMeleeAttackIfReady();
        }

    private:
        TaskScheduler _scheduler;
        uint32 _deadMinionCounter;
        uint8 _dreadknightCounter;
        bool _bossSpawned{ false };
        bool _scheduledText{ false };
        GuidSet _summonGuids;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_marshal_raynorAI(creature);
    }
};

enum BossSpells
{
    SPELL_CARRION_SWARM = 34240,
    SPELL_SLEEP         = 8399,
    SPELL_RAIN_OF_FIRE  = 19717,
    SPELL_KNOCKBACK     = 26027,
    SPELL_SHADOWFORM    = 69144,
    SPELL_VOLLEY        = 21341
};

enum BossText
{
    SAY_SPAWN  = 0,
    SAY_AGGRO  = 1,
    EMOTE_STORM  = 2,
    SAY_KILLS  = 3,
    SAY_OUTRO  = 4,
    SAY_DEATH  = 5
};

class npc_dark_portal_boss : public CreatureScript
{
public:
    npc_dark_portal_boss() : CreatureScript("npc_dark_portal_boss") { }

    struct npc_dark_portal_bossAI : public ScriptedAI
    {
        npc_dark_portal_bossAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
            _scheduler.CancelAll();
            _scheduler.Schedule(15s, [this](TaskContext context)
            {
                DoCastVictim(SPELL_CARRION_SWARM);
                context.Repeat(15s, 30s);
            }).Schedule(10s, [this](TaskContext context)
            {
                DoCastRandomTarget(SPELL_SLEEP);
                context.Repeat(30s, 60s);
            });
        }

        void KilledUnit(Unit* victim) override
        {
            if (victim->IsPlayer())
            {
                Talk(SAY_KILLS);
            }
            else if (roll_chance_i(20))
            {
                Talk(SAY_KILLS);
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellSchoolMask /*damageSchoolMask*/) override
        {
            if (me->HealthBelowPctDamaged(30, damage) && !_upset)
            {
                _upset = true;
                Talk(SAY_OUTRO);

                DoCastAOE(SPELL_KNOCKBACK, true);
                DoCastSelf(SPELL_SHADOWFORM, true);

                _scheduler.Schedule(5s, [this](TaskContext context)
                {
                    DoCastRandomTarget(SPELL_RAIN_OF_FIRE, 0, 0.0f, false);
                    context.Repeat(15s, 30s);
                });

                _scheduler.Schedule(10s, [this](TaskContext context)
                {
                    DoCastAOE(SPELL_SHADOW_VOLLEY);
                    context.Repeat(30s, 45s);
                });

                Talk(EMOTE_STORM);

                _scheduler.Schedule(1s, [this](TaskContext context)
                {
                    if (Creature* raynor = me->FindNearestCreature(NPC_RAYNOR, 500.0f))
                    {
                        raynor->AI()->SetData(ACTION_INFERNAL_STORM, 0);
                    }

                    if (context.GetRepeatCounter() < 3)
                    {
                        context.Repeat(3s);
                    }
                    else
                    {
                        context.Repeat(20s, 30s);
                    }
                });
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
        }

        void UpdateAI(uint32 diff) override
        {
            _scheduler.Update(diff);

            if (!UpdateVictim())
            {
                return;
            }

            DoMeleeAttackIfReady();
        }

    private:
        TaskScheduler _scheduler;
        bool _upset {false};
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_dark_portal_bossAI(creature);
    }
};

// Add all scripts in one
void AddModBlackPortalScripts()
{
    new boss_highlord_kruul();
    new npc_marshal_raynor();
    new npc_dark_portal_boss();
}
