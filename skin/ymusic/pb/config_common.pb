
��
config_common.protoPB google/protobuf/descriptor.protogoogle/protobuf/any.protoconstant.proto"6

WeightTask
tid (Rtid
weight (Rweight"8
WeightTaskList&
wts (2.PB.WeightTaskB��Rwts"w
RandomTaskObject&
check_recv_cond (RcheckRecvCond;
weight_tasks (2.PB.WeightTaskListB��RweightTasks"y
TransferPos
world (Rworld
scene (Rscene
x (Rx
y (Ry
z (Rz
angle (Rangle"@
RewardMoneyParam
unbind (Runbind
count (Rcount"Q
RewardItemParam
unbind (Runbind
tid (Rtid
count (Rcount"?
RewardExpParam
exp (Rexp
	ratio_tid (RratioTid"�
CreateNPCParam
npc_tid (RnpcTid
count (Rcount
script_path (	R
scriptPath 
params (B����Rparams
pos (B����Rpos
distance (Rdistance

exclusived (R
exclusived*
exhaust_with_task (RexhaustWithTask
	live_time	 (RliveTime
name_prefix
 (	R
namePrefix"B
RemoveNPCParam
npc_tid (RnpcTid
task_id (RtaskId"n
TaskFinishRange
use_sum (RuseSum
tids (B����Rtids
low (Rlow
high (Rhigh"�
CreateConvoyNPCParam
npc_tid (RnpcTid
count (Rcount
	spline_id (RsplineId

create_dis (R	createDis
	limit_dis (RlimitDis
scene_id (RsceneId"s
ConvoyRoundAreaParam
x (Rx
y (Ry
z (Rz
npc_id (RnpcId
distance (Rdistance"i
RectAreaParam
pos (B����Rpos
size (B����Rsize
angle (B����Rangle"�
TaskExpression
comments (	Rcomments
show (Rshow&
cond (2.PB.TASK_CONDITIONRcond
tid (Rtid
op (2.PB.TASK_OPRop#

int_params (B��R	intParams'
float_params (B��RfloatParams3

any_params (2.google.protobuf.AnyR	anyParams
	count_min
 (RcountMin
sn (Rsn"�
TaskCondition5
	end_event (2.PB.TaskExpressionB��RendEvent.
fixed (2.PB.TaskExpressionB��Rfixed2
option1 (2.PB.TaskExpressionB��Roption12
option2 (2.PB.TaskExpressionB��Roption22
option3 (2.PB.TaskExpressionB��Roption3"�

TalkChoice
repuId (RrepuId&
repuValueDelta (RrepuValueDelta
npcWords (	RnpcWords
	hostWords (	R	hostWords 
hostOptions (	RhostOptions/
repuOp (2.PB.TASK_CHOICE_REPU_OPRrepuOp"�
Talk
npcId (RnpcId
voiceId (RvoiceId
npcWords (	RnpcWords
	hostWords (	R	hostWords.
choices (2.PB.TalkChoiceB��Rchoices"J
Dialog
waitTime (RwaitTime$
talks (2.PB.TalkB��Rtalks"�
TaskProperty
	title_pre (	RtitlePre!
chapter_name (	RchapterName
declare (	B��Rdeclare
	recommend (	R	recommend
time (B��
��Rtime%
activity_clear	 (RactivityClear.
activity_open_clear
 (RactivityOpenClear
can_drop (RcanDrop)
no_reward_on_drop (RnoRewardOnDrop
icon (Ricon
order (Rorder*
	task_type (2.PB.TASK_TYPERtaskType7
achieve_type (2.PB.ACHIEVEMENT_TYPERachieveType*
	show_type (2.PB.SHOW_TYPERshowType
system_task (R
systemTask"
auto_get_task (RautoGetTask*
unlock_by_premise (RunlockByPremise.
auto_deliver_reward (RautoDeliverReward*
quality (2.PB.TASK_QUALITYRquality
can_find (RcanFind
auto_do (RautoDo 
auto_hand_in (R
autoHandIn)
hide_in_task_list (RhideInTaskList
	recv_span (RrecvSpan$
record_on_recv (RrecordOnRecv$
max_recv_count (RmaxRecvCount:
reset_recv_count_when_fail  (RresetRecvCountWhenFail0
clear_task_time! (B��	��RclearTaskTime'
disposable_task# (RdisposableTask'
target_descript$ (	RtargetDescript!
end_descript% (	RendDescript0

recvDialog& (2
.PB.DialogB��R
recvDialog4
finishDialog' (2
.PB.DialogB��RfinishDialog3
recv_promp_when_finsih( (RrecvPrompWhenFinsih&
show_task_token) (RshowTaskToken
auto_search* (R
autoSearch&
show_recv_panel+ (RshowRecvPanel*
show_finish_panel, (RshowFinishPanel
	task_name- (	RtaskName2
reward_ratio_activity. (RrewardRatioActivity!
reward_ratio/ (RrewardRatio3
can_away_from_keyboard0 (RcanAwayFromKeyboard
	wait_time1 (RwaitTime,
remaining_each_min2 (RremainingEachMin)
can_finish_by_pay3 (RcanFinishByPay
	cost_item4 (RcostItem&
cost_item_count5 (RcostItemCount2
ratio_of_finish_by_pay6 (RratioOfFinishByPay$
show_reward_of7 (RshowRewardOf9
show_special_reward_words8 (RshowSpecialRewardWords0
special_reward_words9 (	RspecialRewardWordsU
recive_stask_npc: (B+����#NPC.NPC_ESSENCE.id_task_out_serviceRreciveStaskNpcR
finish_task_npc; (B*����"NPC.NPC_ESSENCE.id_task_in_serviceRfinishTaskNpc"
weight_in_lib< (RweightInLib$
is_branch_task= (RisBranchTask3
preposition_task> (B����RprepositionTask1
singular_preposition? (RsingularPreposition
recv_manual@ (R
recvManualJJ"�
TaskRewardUnit
comments (	Rcomments#
type (2.PB.TASK_REWARDRtype
tid (Rtid
count (Rcount#

params_int (B��R	paramsInt'
params_float (B��RparamsFloat3

params_any (2.google.protobuf.AnyR	paramsAny"-
TaskRewardStringParam
param (	Rparam"{
TaskConditionReward0
exps (2.PB.TaskExpressionB����Rexps2
rewards (2.PB.TaskRewardUnitB��Rrewards"m

TaskReward.
fixed (2.PB.TaskRewardUnitB��Rfixed/
opt (2.PB.TaskConditionRewardB��Ropt"�

TaskConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments,
property (2.PB.TaskPropertyRproperty.
	recv_cond (2.PB.TaskConditionRrecvCond/
recv_reward (2.PB.TaskRewardR
recvReward.
	succ_cond (2.PB.TaskConditionRsuccCond/
succ_reward	 (2.PB.TaskRewardR
succReward.
	fail_cond
 (2.PB.TaskConditionRfailCond/
fail_reward (2.PB.TaskRewardR
failReward,
static_finished_sn (RstaticFinishedSn<
system_fail_cond (2.PB.TaskExpressionRsystemFailCond:���JJJJJJJ"=
TaskAnyTIDOne
TID (B��RTID
value (Rvalue"L

TaskAnyTID
TID (RTID
value (Rvalue
value2 (Rvalue2"6

Coordinate
x (Rx
y (Ry
z (Rz"B
ItemInfo
item_id (RitemId

item_count (R	itemCount"�
DistributeServiceZone$
world_war_zone (RworldWarZone+
big_world_man_zone (RbigWorldManZone(
roam_league_zone (RroamLeagueZone"�
SKILL_PERFORM_STATES
state_id (RstateId0
state_param_count (B��RstateParamCount%
state_param (	B��R
stateParam

state_time (	R	stateTime+
state_probability (	RstateProbability"�
Open7DaysAchievement;
open7days_general_cost_tid (Ropen7daysGeneralCostTid?
open7days_general_reward_tid (Ropen7daysGeneralRewardTid;
open7days_common_limit_tid (Ropen7daysCommonLimitTid"�
SuspendParam
length (Rlength
hight (Rhight3
accelerate_horizontal (RaccelerateHorizontal/
accelerate_vertical (RaccelerateVertical!
fly_duration (RflyDuration
roll_length (R
rollLength"�
VIPLevelLimitShow/
vip_level_limit_mall (RvipLevelLimitMall/
vip_level_limit_wing (RvipLevelLimitWing-
vip_level_limit_pet (RvipLevelLimitPet3
vip_level_limit_weapon (RvipLevelLimitWeapon"�

CorpsLimit?
corps_kickout_limit_count (B��
RcorpsKickoutLimitCount8
corps_leave_cooldown (B���RcorpsLeaveCooldown"�
HuntNovelty3
hunt_reward_repu_limit (RhuntRewardRepuLimit5
hunt_general_reward_tid (RhuntGeneralRewardTid1
hunt_common_limit_tid (RhuntCommonLimitTid"
hunt_speak_id (RhuntSpeakId"�
	LuckPlate8
plates (2.PB.LuckPlate.PlateElementB��Rplatesu
PlateElement!
activity_tid (RactivityTid
	plate_tid (RplateTid%
activity_index (RactivityIndex"T
Material!
material_tid (RmaterialTid%
material_count (RmaterialCount"b

Reputation)
reputation_index (RreputationIndex)
reputation_value (RreputationValue"�
player_model_info!
sex (2.PB.GENDER_ENUMRsex2

occupation (2.PB.PROF_TYPE_ENUMR
occupation-
main_hand_weapon_id (RmainHandWeaponId
armor_id (RarmorId"�
ItemCommonProp"
id_major_type (RidMajorType
id_sub_type (R	idSubType
	file_icon (RfileIcon
quality (Rquality'
min_level_to_use (RminLevelToUse=
sell2shop_mode (2.PB.EXP_SELL2SHOP_MODERsell2shopMode
price (Rprice:
shopsell_mode (2.PB.EXP_SHOPSELL_MODERshopsellMode

shop_price	 (R	shopPrice$
shop_repu_type
 (RshopRepuType&
shop_repu_value (RshopRepuValue 
shop_item_id (R
shopItemId"
shop_item_num (RshopItemNum.
sell_to_shop_item_id (RsellToShopItemId0
sell_to_shop_item_num (RsellToShopItemNum 
pile_num_max (R
pileNumMax!
forbid_depot (RforbidDepot4
	proc_type (2.PB.ITEM_PROC_TYPE_MASKRprocType$
drop_desc_text (RdropDescText@
drop_model_type (2.PB.ITEM_DROP_MODEL_TYPERdropModelType&
hide_for_search (RhideForSearch*
compose_output_id (RcomposeOutputId5
compose_cost_item_count (RcomposeCostItemCount,
compose_cost_money (RcomposeCostMoneyB
auction_class1 (2.PB.EXP_AUCTION_CLASS1_ENUMRauctionClass1B
auction_class2 (2.PB.EXP_AUCTION_CLASS2_ENUMRauctionClass2B
auction_class3 (2.PB.EXP_AUCTION_CLASS3_ENUMRauctionClass3"
can_batch_use (RcanBatchUse2
prof_filter (2.PB.PROFTYPE_MASKR
profFilter3
	goto_shop (2.PB.EXP_GOTO_SHOP_TYPERgotoShop
	is_paimai (RisPaimai(
paimai_price_min  (RpaimaiPriceMin(
paimai_price_max! (RpaimaiPriceMax6
dropitem_type" (2.PB.DROPITEM_TYPERdropitemType"�G
TID
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
corps_talen (R
corpsTalen!
	prof_cfgs (B�� RprofCfgs
clan_reward (R
clanReward0
battle_assemble_cost	 (RbattleAssembleCost0
battle_transfer_cost
 (RbattleTransferCost(
nation_name_cost (RnationNameCost5
distzone (2.PB.DistributeServiceZoneRdistzone0
alliance_money_price (RallianceMoneyPrice3
corps_inst_count_limit (RcorpsInstCountLimitN
open_7days_achievement (2.PB.Open7DaysAchievementRopen7daysAchievement=
vip_level_limit (2.PB.VIPLevelLimitShowRvipLevelLimit/
corps_limit (2.PB.CorpsLimitR
corpsLimit<
stamina_increase_interval (H RstaminaIncreaseInterval6
stamina_increase_count (HRstaminaIncreaseCount6
multipvp_instance_group (RmultipvpInstanceGroup-
cash_buy_reward_tid (RcashBuyRewardTid+
all_server_msg_tid (RallServerMsgTid2
hunt_novelty (2.PB.HuntNoveltyRhuntNovelty0
instance_loop_tid (B��
RinstanceLoopTid3
instance_loop_fake_tid (RinstanceLoopFakeTid9
knight_honour_campaign_id (RknightHonourCampaignId&
border_scene_id (RborderSceneIdF
 knight_honour_reward_campaign_id (RknightHonourRewardCampaignId+
lucky_plate_cfg_id (RluckyPlateCfgId9
lucky_plate_diamond_count (RluckyPlateDiamondCount(
lucky_plate_time  (RluckyPlateTimeB
new_lucky_plate_activity_id" (B��RnewLuckyPlateActivityId3
lottery_lucky_plate_id# (RlotteryLuckyPlateId:
unlock_assistant_level% (B��RunlockAssistantLevel'
skill_id_can_run& (RskillIdCanRun
	wuzhen_id' (RwuzhenId&
climb_tower_tid( (RclimbTowerTid"
hero_fight_id) (RheroFightId&
faction_def_tid* (RfactionDefTid(
real_time_arena1+ (RrealTimeArena1(
real_time_arena3, (RrealTimeArena3(
real_time_arena5- (RrealTimeArena5*
real_time_arena10. (RrealTimeArena102
threshold_joborder_id/ (RthresholdJoborderId(
friend_help_time0 (RfriendHelpTime!
chat_private1 (	RchatPrivate*
private_mail_icon2 (RprivateMailIcon(
system_mail_icon3 (RsystemMailIcon/
double_exp_min_level4 (RdoubleExpMinLevel,
pvp_punish_time5 (B��<RpvpPunishTime>
team_pass_level_activity_tid6 (RteamPassLevelActivityTid&
booth_tax_ratio7 (RboothTaxRatio"
booth_tax_min8 (RboothTaxMin$
booth_time_max9 (RboothTimeMax%
shangpu_shopid: (RshangpuShopid3
partner_hire_mail_icon; (RpartnerHireMailIcon"
guaji_task_id< (RguajiTaskId$
guaji_task_id2= (RguajiTaskId2!
back_context> (	RbackContext!
send_context? (	RsendContextI
"min_level_enable_offline_auto_play@ (RminLevelEnableOfflineAutoPlay-
home_spirit_repu_idA (RhomeSpiritRepuId;
home_daily_uselimit_configB (RhomeDailyUselimitConfig;
home_clean_uselimit_configC (RhomeCleanUselimitConfig9
home_rest_uselimit_configD (RhomeRestUselimitConfig(
makeup_unlock_lvF (RmakeupUnlockLv.
customface_cost_tidG (RcustomfaceCostTid2
customface_cost_countH (RcustomfaceCostCount-
xyxw_ride_min_levelI (RxyxwRideMinLevel-
face_color_cost_tidJ (RfaceColorCostTid1
face_color_cost_countK (RfaceColorCostCount-
team_min_role_levelL (RteamMinRoleLevel0
qinggong_recharge_cdM (RqinggongRechargeCd7
ride_chongci_recharge_cdN (RrideChongciRechargeCd'
teacher_contextO (	RteacherContext'
student_contextP (	RstudentContext8
team_pass_rank_mini_awardQ (RteamPassRankMiniAward3
team_pass_ranklist_tidR (RteamPassRanklistTid3
team_pass_rank_topnS (B��RteamPassRankTopn#
flower_price1T (RflowerPrice1#
flower_price2U (RflowerPrice2
	add_amityV (RaddAmity*
flower_unit_priceW (RflowerUnitPrice2
escape_stuck_cooldownX (RescapeStuckCooldown,
qinggong_min_levelY (RqinggongMinLevel$
rush_min_levelZ (RrushMinLevel5
team_follow_activity_id[ (RteamFollowActivityId 
wine_task_id\ (R
wineTaskId/
wedding_snap_item_id] (RweddingSnapItemId1
normal_parade_item_id^ (RnormalParadeItemId1
luxury_parade_item_id_ (RluxuryParadeItemId8
is_team_pass_compare_repu` (RisTeamPassCompareRepu*
limit_free_rewarda (RlimitFreeReward.
lucky_plateb (2.PB.LuckPlateR
luckyPlate1
id_takealong_shop_npcc (RidTakealongShopNpc-
id_faction_shop_npcd (RidFactionShopNpc4
id_wing_init_fixview_ide (RidWingInitFixviewId1
id_equip_refresh_itemh (RidEquipRefreshItemI
id_equip_refresh_lock_prop_itemj (B��RidEquipRefreshLockPropItemY
'equip_refresh_activate_addon_cost_moneyl (B��R"equipRefreshActivateAddonCostMoney5
offline_exp_free_factorm (RofflineExpFreeFactor@
offline_exp_bind_money_factorn (RofflineExpBindMoneyFactor>
offline_exp_bind_money_priceo (RofflineExpBindMoneyPriceB
offline_exp_trade_money_factorp (RofflineExpTradeMoneyFactor@
offline_exp_trade_money_priceq (RofflineExpTradeMoneyPrice*
id_marriage_titler (RidMarriageTitle2
hero_upgrade_levelt (B��RheroUpgradeLevel$
hero_max_levelu (RheroMaxLevel8
hero_upgrade_cost_item_idv (RheroUpgradeCostItemId2
max_duke_gain_each_dayw (RmaxDukeGainEachDay1
id_drop_money_itemz (B��RidDropMoneyItem9
player_wuhun_unlock_level{ (RplayerWuhunUnlockLevel&
item_speaker_id| (RitemSpeakerId?
call_for_help_common_limit_id} (RcallForHelpCommonLimitId6
rune_save_slot_price� (B��RruneSaveSlotPrice1
buy_stamina_price� (B��RbuyStaminaPrice8
buy_stamina_gain_stamina� (RbuyStaminaGainStamina 
max_stamina� (R
maxStaminaB
give_stamina_daily_at_0_oclock� (RgiveStaminaDailyAt0OclockD
give_stamina_daily_at_12_oclock� (RgiveStaminaDailyAt12OclockD
give_stamina_daily_at_18_oclock� (RgiveStaminaDailyAt18OclockA
give_stamina_to_friend_amount� (RgiveStaminaToFriendAmountQ
&give_stamina_to_friend_cmn_uselimit_id� (R giveStaminaToFriendCmnUselimitId[
+receive_stamina_from_friend_cmn_uselimit_id� (R%receiveStaminaFromFriendCmnUselimitId0
wing_soul_item_id� (B��RwingSoulItemId,
id_get_title_speak� (RidGetTitleSpeak8
id_get_achievement_speak� (RidGetAchievementSpeak9
celebrity_attr_factor� (B��RcelebrityAttrFactorH
 unsummoned_celebrity_attr_factor� (RunsummonedCelebrityAttrFactor'
id_money_matter� (RidMoneyMatter/
id_reset_profession� (RidResetProfession0
item_strict_bind_tid� (RitemStrictBindTid5
item_un_strict_bind_tid� (RitemUnStrictBindTid'
prof1_level_req� (Rprof1LevelReq0
prof_reset_level_req� (RprofResetLevelReq1
id_skill_get_old_name� (RidSkillGetOldName1
id_skill_get_old_body� (RidSkillGetOldBody4
safe_time_after_unlock� (RsafeTimeAfterUnlock6
default_player_scene_id� (RdefaultPlayerSceneId)
default_player_x� (RdefaultPlayerX)
default_player_y� (RdefaultPlayerY)
default_player_z� (RdefaultPlayerZ=
friend_summon_count_per_day� (RfriendSummonCountPerDayU
%friend_summon_count_per_day_by_levels� (B��RfriendSummonCountPerDayByLevels6
modify_name_activity_id� (RmodifyNameActivityId0
modify_name_item_tid� (RmodifyNameItemTid1
modify_name_cooldown� (RmodifyNameCooldownE
modify_faction_name_activity_id� (RmodifyFactionNameActivityId?
modify_faction_name_item_tid� (RmodifyFactionNameItemTid@
modify_faction_name_cooldown� (RmodifyFactionNameCooldown:
modify_gender_activity_id� (RmodifyGenderActivityId4
modify_gender_item_tid� (RmodifyGenderItemTid5
modify_gender_cooldown� (RmodifyGenderCooldown@
player_levelup_general_param� (RplayerLevelupGeneralParam<
hero_levelup_general_param� (RheroLevelupGeneralParam<
prof_levelup_general_param� (RprofLevelupGeneralParam<
hero_challenge_instance_id� (RheroChallengeInstanceIdP
%hero_challenge_random_level_config_id� (R heroChallengeRandomLevelConfigId[
+hero_challenge_refresh_cost_money_vipseq_id� (R%heroChallengeRefreshCostMoneyVipseqIdG
 time_instance_monster_score_rate� (RtimeInstanceMonsterScoreRate[
friend_blessing_configs� (2.PB.TID.FriendBlessingConfigB��RfriendBlessingConfigs1
cyxb_instance_ids� (B��RcyxbInstanceIds1
id_skill_qinggong� (B��RidSkillQinggong%
daily_exp_time� (RdailyExpTime<
achievement_id_by_badge� (B��RachievementIdByBadge0
self_service_npc_tid� (RselfServiceNpcTid)
id_wedding_scene� (RidWeddingScene6
corps_base_instance_tid� (RcorpsBaseInstanceTid/
id_get_card_speak_id� (RidGetCardSpeakId2
ctrl_record_last_time� (RctrlRecordLastTime1
system_mail_lifetime� (RsystemMailLifetime1
deny_attach_lifetime� (RdenyAttachLifetime-
read_mail_lifetime� (RreadMailLifetimel
FriendBlessingConfig,
lottery_service_id (RlotteryServiceId&
grant_reward_id (RgrantRewardId:��g����Config��0B
stamina_increase_interval_newB
stamina_increase_count_new"�
Forge
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
category (Rcategory
quality (Rquality
product_tid (R
productTid#
product_count	 (RproductCount(
product_show_tid
 (RproductShowTid

cost_money (R	costMoney0
	materials (2.PB.MaterialB��R	materials:��f����Abandon"�
CorpsTalentShow
name (	Rname
icon (Ricon
describe (	Rdescribe*
talent_config_tid (RtalentConfigTid"�
CorpsSkillRoom
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments9

talentshow (2.PB.CorpsTalentShowB��R
talentshow:��h����Temp��公会��0"�
CorpsTalentLevel1
need_skill_room_level (RneedSkillRoomLevel'
need_contribute (RneedContribute
addon_id (RaddonId,
need_host_level (B�� RneedHostLevel&
cost_bind_money (RcostBindMoney.
need_history_contri (RneedHistoryContri"�
CorpsTalent
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments<
levelConfig (2.PB.CorpsTalentLevelB��dRlevelConfig:��i����Temp��公会��0"�	
CorpsCofferDividend
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsR
dividend_coefs (2%.PB.CorpsCofferDividend.dividend_coefB��RdividendCoefs]
money_upper_limits (2).PB.CorpsCofferDividend.money_upper_limitB��RmoneyUpperLimits(
per_dividend_tid	 (RperDividendTid,
his_contri_coef
 (B��
RhisContriCoef.
week_contri_coef (B��ZRweekContriCoef$

added_coef (B���R	addedCoef,
master_pos_coef (B��dRmasterPosCoef5
vice_master_pos_coef (B��_RviceMasterPosCoef0
zhanglao_pos_coef (B��ZRzhanglaoPosCoef,
normal_pos_coef (B��URnormalPosCoef*
base_lvl_coef (B��ІRbaseLvlCoef-
buildings_coef (B����RbuildingsCoef!
	added_fee (B�� RaddedFee0
low_base_lvl_coef (B���]RlowBaseLvlCoef3
low_buildings_coef (B���'RlowBuildingsCoef)
low_added_fee (B���NRlowAddedFee.
tangzhu_pos_coef (B��ZRtangzhuPosCoef0
jingying_pos_coef (B��ZRjingyingPosCoefQ
dividend_coef

coffer_lvl (R	cofferLvl!
percent_coef (RpercentCoefS
money_upper_limit

coffer_lvl (R	cofferLvl
money_upper (R
moneyUpper: �������Temp��公会��0"�
CapsuleForUE4 
pos (2.PB.CoordinateRpos,
	direction (2.PB.CoordinateR	direction
radius (Rradius
half_length (R
halfLength"�

CarryGoods
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
base_lvl_limit (RbaseLvlLimit!
activity_tid (RactivityTid.
choose_total_num (B��RchooseTotalNum,
choose_easy_num (B��RchooseEasyNum0
goods_lists_count	 (B��RgoodsListsCount@
goods_lists
 (2.PB.CarryGoods.goods_listB��R
goodsLists4
ask_help_need_fill_n (B��RaskHelpNeedFillN*
ask_help_upper (B��RaskHelpUpper0
help_others_upper (B��RhelpOthersUpper1
finish_need_fill_n (B��RfinishNeedFillN&
unlock_role_lvl (RunlockRoleLvl!
activity_idx (RactivityIdx/
common_use_limit (B���RcommonUseLimit�

goods_list
base_lvl (RbaseLvl/
easy_goods_table_idx (ReasyGoodsTableIdx/
hard_goods_table_idx (RhardGoodsTableIdx(
finish_award_tid (RfinishAwardTid*
fulfill_award_tid (RfulfillAwardTid.
client_finish_award (RclientFinishAward0
client_fulfill_award (RclientFulfillAward: �������Temp��公会��0"�

MergeCorps
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsF
	vitalitys (2".PB.MergeCorps.vitality_level_infoB��R	vitalitys0
merge_faction_reward (RmergeFactionReward0
fired_faction_reward (RfiredFactionReward2
merge_reward_ndays	 (B��RmergeRewardNdaysC
personal_vitality_add_corps
 (B��2RpersonalVitalityAddCorps'
protect_days (B��RprotectDays5
auto_merge_activity_tid (RautoMergeActivityTidA
protect_vitality_add_corps (B��RprotectVitalityAddCorpsD
manual_merge_vitality_upper (B���RmanualMergeVitalityUpper?
auto_merge_vitality_upper (B��RautoMergeVitalityUpper=
ndays_under_manual_upper (B��RndaysUnderManualUpper9
ndays_under_auto_upper (B��RndaysUnderAutoUpperm
vitality_level_info
name (	Rname%
vitality_level (RvitalityLevel
	vitality_ (Rvitality: �������Temp��公会��0"�
CarryGoodsEquivalent
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments/
this_table_max_index (RthisTableMaxIndex
item_tid (RitemTid-
equivalent_item (B��@RequivalentItem: �������Temp��公会��0"�
CarryGoodsItemTable
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsI
goods_items (2".PB.CarryGoodsItemTable.goods_itemB��
R
goodsItems�

goods_item
item_tid (RitemTid
need_num (RneedNum$
self_award_tid (RselfAwardTid(
helper_award_tid (RhelperAwardTid
item_idx (RitemIdx: �������Temp��公会��0"�
CorpsCharmGrade
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsP
charm_grades (2'.PB.CorpsCharmGrade.appoint_charm_gradeB��
RcharmGrades?
forbid_grade_change_activity (RforbidGradeChangeActivity�
appoint_charm_grade"
job_order_idx (RjobOrderIdx1

grade_type (2.PB.CharmGradeTypeR	gradeType8
pre_grade_type (2.PB.CharmGradeTypeRpreGradeType>
depose_grade_type (2.PB.CharmGradeTypeRdeposeGradeType2
dividend_addn_percent (RdividendAddnPercent: �������Temp��公会��0"�
ArenaScoreGrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments=
grades (2.PB.ArenaScoreGrade.grade_info1B��
Rgrades"

fx_path_id (B�� RfxPathId�
grade_info1
	grade_idx (RgradeIdx
	award_tid (RawardTid
lower_bound (R
lowerBound
	title_tid (RtitleTid

grade_name (	R	gradeName

grade_icon (	R	gradeIcon

grade_desc (	R	gradeDesc:%�������Config��	竞技场��0"�

ArenaRules
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
premission_lvl (B��RpremissionLvl$

init_score (B���R	initScore.
battle_duration (B���RbattleDuration,
ticket_gift_cd (B����RticketGiftCd'
ticket_upper	 (B��RticketUpper0
reset_ticket_cost
 (B��RresetTicketCost,
arena_instance_tid (RarenaInstanceTid9
arena_monster_id_count (B��
RarenaMonsterIdCount.
arena_monster_id (B��
RarenaMonsterId-
cron_day_of_week (B��RcronDayOfWeek!
	cron_hour (B�� RcronHour!
	cron_minu (B��RcronMinu:
pre_join_rank_delta_lvl (B��RpreJoinRankDeltaLvlE
update_highest_power_percent (B��RupdateHighestPowerPercent
activity_id (R
activityId:%�������Config��	竞技场��0"�
ArenaMatchRules
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsQ
group_opponents (2".PB.ArenaMatchRules.group_opponentB��RgroupOpponents4
fluctuation_ratio (B��
ף;RfluctuationRatio�
group_opponent&

easy_ratio (B��33s?R	easyRatio#

easy_score (B��R	easyScore*
middle_ratio (B���G�?RmiddleRatio'
middle_score (B��	RmiddleScore&

hard_ratio (B��ff�?R	hardRatio#

hard_score (B��R	hardScore
	win_award (RwinAward

lose_award (R	loseAward:%�������Config��	竞技场��0"�
ArenaWinStreak
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsJ
streak_awards (2.PB.ArenaWinStreak.streak_awardB��RstreakAwards`
streak_award

streak_num (R	streakNum
score (Rscore
	award_tid (RawardTid:%�������Config��	竞技场��0"�
DiscoverConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

zone_index (R	zoneIndex
	scene_tag (RsceneTag
trans_pos_x (R	transPosX
trans_pos_z (R	transPosZ
	trans_btn	 (	RtransBtn:�������Abandon"�
FashionRandomColor
color_index (R
colorIndex!
color_weight (RcolorWeight
pre_cnt (RpreCnt&
addon_group_tid (RaddonGroupTid)
recommand_tag (B��RrecommandTag
id_tag (RidTag"�
WorldWar
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	open_days (RopenDays
level_limit (R
levelLimit#
offical_limit (RofficalLimit!
	safe_time	 (B��RsafeTime
inst_tid (B��RinstTid>
wwar_levels (2.PB.WorldWar.wwar_levelB��
R
wwarLevels
first_award (R
firstAward(
init_morale_repu (RinitMoraleRepu-
morale_repu_per_day (RmoraleRepuPerDay4
declare_activity_index (RdeclareActivityIndex0
declare_activity_tid (RdeclareActivityTid6
alliance_activity_index (RallianceActivityIndex2
alliance_activity_tid (RallianceActivityTid.
wwar_activity_index (RwwarActivityIndex*
wwar_activity_tid (RwwarActivityTid(
max_declare_cost (RmaxDeclareCost 
pve_inst_tid (R
pveInstTid)
no_owner_inst_tid (RnoOwnerInstTid@
season (2".PB.WorldWar.WorldWarseason_configB��
Rseason0
alliance_money_lower (RallianceMoneyLower2
alliance_money_normal (RallianceMoneyNormal0
alliance_money_upper (RallianceMoneyUpper.
kaifu_pve_territory  (RkaifuPveTerritory.
kaifu_pvp_territory! (RkaifuPvpTerritory(
min_declare_cost" (RminDeclareCost/
declare_cost_add_min# (RdeclareCostAddMin,
chat_offical_limit$ (RchatOfficalLimit`

wwar_level
icon (Ricon
repu (Rrepu
reward (Rreward
name (	Rname�
WorldWarseason_config
name (	Rname
start_month (R
startMonth
	start_day (RstartDay
	life_time (RlifeTime+
backup_toplist (B��RbackupToplist

start_year (R	startYear:'��j����Config��世界大战��0"�
WWarTerritory
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname

brief_desc (	R	briefDesc+
position_typea (B��RpositionTypea%
attack_type (B��R
attackType#

level_type	 (B��R	levelType4
special_type_weight
 (B��
RspecialTypeWeight6
near_territory_count (B��RnearTerritoryCount+
near_territory (B��RnearTerritory!
reward_title (RrewardTitle0
declare_nation_money (RdeclareNationMoney0
declare_nation_power (RdeclareNationPower6
expedition_nation_power (RexpeditionNationPower(
wward_reward_tid (RwwardRewardTid:'��k����Config��世界大战��0"�

WWarReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
day_reward_id (RdayRewardId"
win_reward_id (RwinRewardId$
lose_reward_id (RloseRewardId,
alleywin_reward_id (RalleywinRewardId.
alleylose_reward_id	 (RalleyloseRewardId,
emptywin_reward_id
 (RemptywinRewardId.
emptylose_reward_id (RemptyloseRewardIdE
rank_reward (2.PB.WWarReward.npc_rank_rewardB��R
rankReward&
win_nation_repu (RwinNationRepu(
lose_nation_repu (RloseNationRepu0
alleywin_nation_repu (RalleywinNationRepu2
alleylose_nation_repu (RalleyloseNationRepu0
emptywin_nation_repu (RemptywinNationRepu2
emptylose_nation_repu (RemptyloseNationRepu^
npc_rank_reward
rank (Rrank
reward (Rreward
nation_repu (R
nationRepu:'��t����Config��世界大战��0"�
TerritoryGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$

terriories (B��R
terriories&
special_num_min (RspecialNumMin&
special_num_max (RspecialNumMaxG
specail_cfgs
 (2.PB.TerritoryGroup.special_cfgB��RspecailCfgsc
special_cfg
type (Rtype
icon (Ricon
param (Rparam
weight (Rweight:'��s����Config��世界大战��0"�

SuiteAddon
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	icon_path (	RiconPathI
suite_addon_type (2.PB.SuiteAddon.SUITE_ADDON_TYPERsuiteAddonType?
prop_limits	 (2.PB.SuiteAddon.PropLimitB��R
propLimitsY
	PropLimit
param (Rparam
addon_id (RaddonId
	texiao_id (RtexiaoId"i
SUITE_ADDON_TYPE
UNKNOWN_SUITE_ADDON_TYPE 
SUITE_ADDON_TYPE_GEM
SUITE_ADDON_TYPE_LIANXING:��m��	��Temp��装备��1"�
AchievementReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
config_name (	R
configName
level_limit (R
levelLimit

reward_tid (R	rewardTidE
agroup	 (2'.PB.AchievementReward.achievement_groupB��RagroupB
achievement_group-
achievement_tid (B��RachievementTid:��n����Abandon"�

NationInfo
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
logo (Rlogo"�

ProfConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
seq (Rseq
name (	Rname
	prof_icon (RprofIcon,
male (2.PB.ProfConfig.show_infoRmale0
female	 (2.PB.ProfConfig.show_infoRfemale
skillID (B��
RskillID
mask (Rmask 
cost_item_id (R
costItemId&
cost_item_count (RcostItemCount
lv_limit (RlvLimit%
fighting_limit (RfightingLimit 
fx_effect_id (R
fxEffectId
property_id (R
propertyId
	prof_desc (	RprofDesc
attack_type (R
attackType'
skill_seq_prof_0 (RskillSeqProf0'
skill_seq_prof_1 (RskillSeqProf1'
skill_seq_prof_2 (RskillSeqProf2'
skill_seq_prof_3 (RskillSeqProf3'
skill_seq_prof_4 (RskillSeqProf4$
skill_seq_ride (RskillSeqRide*
skill_seq_peerage (RskillSeqPeerage&
skill_seq_glide (RskillSeqGlide�
	show_info
	head_icon (RheadIcon
model_id (RmodelId
model_fx_id (R	modelFxId 
team_icon_id (R
teamIconId:��p����Abandon"�
DukePassiveSkillGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	icon_path (RiconPath 
description (	RdescriptionC
upgrades	 (2!.PB.DukePassiveSkillGroup.UpgradeB��Rupgrades�
Upgrade

cost_money (R	costMoney 
cost_repu_id (R
costRepuId"
cost_repu_val (RcostRepuVal 
cost_item_id (R
costItemId&
cost_item_count (RcostItemCount
addon_id (RaddonId:&��v����DukePassiveSkillGroup��2"�
Statue
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
toplist_tid (R
toplistTid%
toplist_nation (RtoplistNation!
toplist_rank (RtoplistRank%
nation_officer (RnationOfficer5
id_other_statue_service	 (RidOtherStatueService3
id_self_statue_service
 (RidSelfStatueService
name (	Rname
title (	Rtitle
	hello_msg (	RhelloMsg)
cron (2.PB.Statue.SimpleCronRcron
scale (Rscale
	title_pos (RtitlePos�

SimpleCron
min (B�� Rmin
hour (B�� Rhour
day_of_week (R	dayOfWeek 
day_of_month (R
dayOfMonth:��w����Config��0"�
CommonCostLimit
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments7
costs (2.PB.CommonCostLimit.OneCostB��Rcosts�
OneCost
item_tid (RitemTid

item_count (R	itemCount
money (Rmoney
repu_id (RrepuId
repu_val (RrepuVal
diamond_mix (R
diamondMix%
diamond_unbind (RdiamondUnbind:'��x����Config��通用消耗��1"�
NPCStatueService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments*
who (2.PB.NPCStatueService.WHORwho 
use_limit_id (R
useLimitId
name (	Rname5
kinds	 (2.PB.NPCStatueService.KindB��Rkinds�
Kind
	reward_id (RrewardId1
statue_reward_repu_id (RstatueRewardRepuId3
statue_reward_repu_val (RstatueRewardRepuVal"
cost_limit_id (RcostLimitId"3
WHO
UNKNOWN_WHO 
WHO_SELF
	WHO_OTHER:��y����Npc��服务��0"�
GeneralCost
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsA
	cost_type (2$.PB.GeneralCost.GeneralCostCOST_TYPERcostType0
	materials (2.PB.MaterialB��R	materials6
reputations	 (2.PB.ReputationB��Rreputations
money
 (Rmoney!
diamond_bind (RdiamondBind%
diamond_unbind (RdiamondUnbind"�
GeneralCostCOST_TYPE
UNKNOWN_COST_TYPE 
	COST_NONE
COST_REFORGE
COST_CLAN_GIFT
COST_BATTLE
COST_OPEN_7DAYS:'��z����Config��通用消耗��1"�

GmtCommand
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments*
	gmt_index (2.PB.GMT_INDEXRgmtIndex
active (B��Ractive:��{����Config��0"�

NewTopList
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
type (Rtype%
activity_index (RactivityIndex!
activity_tid (RactivityTid8
	time_type	 (2.PB.NewTopList.TOPLIST_TYPERtimeType

day_offset
 (R	dayOffset
hour_offset (R
hourOffset

min_offset (R	minOffset
weak_offset (R
weakOffset
frozen_time (R
frozenTime
max_item (RmaxItem1
total_time_can_reward (RtotalTimeCanReward*
clear_when_backup (RclearWhenBackup!
	rank_info (	B��
RrankInfoA
reward_info (2.PB.NewTopList.reward_confB��R
rewardInfo=
	data_type (2 .PB.NewTopList.TOPLIST_DATA_TYPERdataType
param1 (Rparam1
param2 (Rparam2
param3 (Rparam3*
is_auto_reward (B�� RisAutoReward�
reward_conf
rank (Rrank
reward (Rreward#

item_count (B��R	itemCount(
is_title_item (B�� RisTitleItem"p
TOPLIST_TYPE

TT_FOREVER 
TT_KAIFU
TT_PER_WEEK
TT_PER_MONTH
TT_CAMPAIGN

TT_PER_DAY"�
TOPLIST_DATA_TYPE
TDT_ROLE_LEVEL 
TDT_NATION_POWER
TDT_WORLD_WAR
TDT_ROLE_REPU
TDT_GS_PROPERTY
TDT_CHARMVALUE
TDT_JJC_SCORE
TDT_POPULARITY_VALUE
TDT_TEAM_PASS_LEVEL
TDT_COUPLE_SINCERITY	
TDT_POPULARITY

TDT_POPULARITY_HISTORY
TDT_POPULARITY_GIVE
TDT_POPULARITY_GIVE_HISTORY
TDT_LEVEL_SPRINT
TDT_SERVER_CORPS
TDT_FIGHTVALUE
TDT_ABROAD_SCORE:��|����Config��0"�

ClanReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
level_limit (R
levelLimitA
level_rewards (2.PB.ClanReward.Reward1B��
RlevelRewards4
companion_limit_perday	 (RcompanionLimitPerdayI
companion_rewards (2.PB.ClanReward.Reward1B��RcompanionRewards9
	gift_bags (2.PB.ClanReward.GiftBagB��RgiftBagsA
trade_rewards (2.PB.ClanReward.Reward1B��
RtradeRewards,
encourage_buff_tid (RencourageBuffTid+
distance_buffs (B��RdistanceBuffsB
call_together_common_limit_tid (RcallTogetherCommonLimitTid;
together_common_cost_index (RtogetherCommonCostIndex9
transfer_common_limit_tid (RtransferCommonLimitTid;
transfer_common_cost_index (RtransferCommonCostIndex0
transfer_level_limit (RtransferLevelLimitw
Reward1
value (Rvalue,
general_reward_tid (RgeneralRewardTid(
common_limit_tid (RcommonLimitTid|
GiftBag(
common_limit_tid (RcommonLimitTid
item_tid (RitemTid,
general_cost_index (RgeneralCostIndex:��}����Abandon"�
RoamBigWorld
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments.
player_number_limit (RplayerNumberLimit!
opened_count (RopenedCount!
instance_tid (RinstanceTid*
instance_name	 (	B���RinstanceName:!��~����Config��跨服��0"�

PandoraBox
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
activity_tid (RactivityTid(
consume_item_tid (RconsumeItemTid8
pandoras (2.PB.PandoraBox.PandoraB��
Rpandoras
mix_diamond	 (R
mixDiamond|
Pandora
item_tid (RitemTid

item_count (R	itemCount
weight (Rweight
lottery_tid (R
lotteryTid:������Config��0"�
Tarot
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity (Ractivity+
cards (2.PB.Tarot.Card1B��	Rcards#

free_times (B��R	freeTimes"
cost_limit_id	 (RcostLimitId3
rewards (2.PB.Tarot.BigRewardB��Rrewards6
reward_effect_pathid (B��RrewardEffectPathid
	max_times (RmaxTimes\
Card1
id (Rid
name (	Rname
	reward_id (RrewardId
path (	Rpath#
	BigReward
rid (B��Rrid:�������Config��0"�
PrivilegeCumulate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
vitality_add (RvitalityAddI
task_add (2(.PB.PrivilegeCumulate.task_award_add_perB��RtaskAddC
repu_add	 (2".PB.PrivilegeCumulate.repu_add_perB��RrepuAddF
task_award_add_per
task_id (RtaskId
add_per (RaddPer@
repu_add_per
repu_id (RrepuId
add_per (RaddPer:#�����
��Temp��	特权卡��0"�	
PrivilegeCard=
	card_type (2 .PB.PrivilegeCard.PRIVILEGE_CARDRcardType
	hook_hour (RhookHour8
offline_extra_experience (RofflineExtraExperience*
retrive_time_free (RretriveTimeFree0
online_double_reward (RonlineDoubleReward0
active_double_reward (RactiveDoubleReward6
nationwar_double_reward (RnationwarDoubleReward(
amend_mutil_time (RamendMutilTime,
general_reward_tid	 (RgeneralRewardTid&
vip_limit_level
 (RvipLimitLevel0
signin_double_reward (RsigninDoubleReward!
service_code (	RserviceCode
vip_seqs (B��RvipSeqs&
auction_add_sit (RauctionAddSit,
auction_charge_per (RauctionChargePer
makeup_free (R
makeupFree#
disguise_free (RdisguiseFree+
fashioncolor_free (RfashioncolorFree#

shop_goods (B��R	shopGoodsL
use_limit_add (2".PB.PrivilegeCard.common_use_limitB��RuseLimitAdd

card_level (R	cardLevel%
cumulate_index (RcumulateIndex!
vitality_add (RvitalityAdd*
corps_deposit_add (RcorpsDepositAdd`
common_use_limit/
common_use_limit_tid (RcommonUseLimitTid
	add_limit (RaddLimit"P
PRIVILEGE_CARD
UNKNOWN_PRIVILEGE_CARD 

COPPER

SILVER

GOLDEN"�
VipPrivilege
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments7

privileges (2.PB.PrivilegeCardB��R
privileges(
common_limit_tid (RcommonLimitTid:#�����
��Temp��	特权卡��0"�

RoamLeague
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
open_condition (RopenCondition
const_a (RconstA1
league_rank_limit (B���RleagueRankLimit!
refresh_cost (RrefreshCost:"�������Config��跨服��0"�
AudioRoomConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

max_follow (R	maxFollow 
cost_item_id (R
costItemId"
cost_item_cnt (RcostItemCnt
	vip_level (RvipLevel!
player_level	 (RplayerLevel!
max_category
 (RmaxCategory,
top_rooms_count (B��dRtopRoomsCount
forbid_time (R
forbidTime:�������Abandon"�
AudioRoomGift
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
sender_repu_add (RsenderRepuAdd&
recver_repu_add (RrecverRepuAdd 
cost_repu_id (R
costRepuId"
cost_repu_cnt (RcostRepuCnt,
need_broadcast_all	 (RneedBroadcastAll
	gift_name
 (	RgiftName$
reward_prop_id (RrewardPropId&
reward_prop_cnt (RrewardPropCnt#
general_bonus (RgeneralBonus#
general_total (RgeneralTotal 
cost_item_id (R
costItemId:�������Abandon"�

MondayStay
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
cost_limit_id (RcostLimitId0
max_refresh_times (B��RmaxRefreshTimes2
free_refresh_times (B��RfreeRefreshTimes
gift_id (RgiftId
weight
 (B��
Rweight:�������Config��0"�	
Recharge
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
platname (	Rplatname

product_id (	R	productId
icon	 (	Ricon 
description
 (	Rdescription
price (Rprice

show_price (	R	showPrice!
gain_diamond (RgainDiamond"
gain_vip_fame (RgainVipFame@
extra_diamond_on_first_charge (RextraDiamondOnFirstCharge1
extra_diamond_is_bind (RextraDiamondIsBindA
extra_vip_fame_on_first_charge (RextraVipFameOnFirstCharge*
limit_activity_id (RlimitActivityId"
is_month_card (RisMonthCard&
month_card_days (RmonthCardDays5
month_card_service_code (	RmonthCardServiceCode"
vip_card_type (RvipCardType%
show_recommend (RshowRecommend

show_index (R	showIndex
show_region (	R
showRegion#
show_platform (	RshowPlatform!
show_channel (	RshowChannel?
recharge_type (2.PB.Recharge.RECHARGE_TYPERrechargeType'
privilege_limit (RprivilegeLimit5
privilege_limitcode (	B��RprivilegeLimitcode"�
RECHARGE_TYPE
UNKNOWN_RECHARGE_TYPE 
	RT_MARKET
RT_MONTH

RT_VIPCARD
RT_GROUPPURCHASE
RT_ACTIVITY
RT_OTHER:�������Abandon"�
CommonConst
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments-
max_vip_limit_level (RmaxVipLimitLevel5
magic_weapon_open_level (RmagicWeaponOpenLevel9
magic_weapon_refine_level (RmagicWeaponRefineLevel:�������Config��0"�
InstanceChatRoom
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments1
ids (2.PB.InstanceChatRoom.tidsB��Rids,
tids
tid (Rtid
name (	Rname:�������Abandon"�
ChatChannelLimit
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments5
map_channel_open_lvl (B��RmapChannelOpenLvl;
nation_channel_open_lvl (B��RnationChannelOpenLvl<
nation_chat_cost_repu_id (B��~RnationChatCostRepuIdW
nation_chat_costs	 (2%.PB.ChatChannelLimit.nation_chat_costB��RnationChatCosts`
nation_chat_cost(
player_lvl_upper (RplayerLvlUpper"
cost_repu_num (RcostRepuNum:�������Abandon"k
AutoAddedInstanceInfo
inst_tid (RinstTid
	max_count (RmaxCount
interval (Rinterval"�
AutoAddedInstance
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
info (2.PB.AutoAddedInstanceInfoB��
Rinfo:�������Abandon"�
GeneralParams
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
describe (	Rdescribe
param (B���Rparam:�������Config��0"�
GeneralFloatParams
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
describe (	Rdescribe
param (B���Rparam:�������Config��0"�
LotteryCheat
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
lottery_tid (R
lotteryTid#
lottery_count (RlotteryCount
item_tid (RitemTid:�������Config��1"F
NPCConvoyTask
task_id (RtaskId
npc_id (RnpcId:���"�
	TaskShoes
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
task_id (RtaskId
item_tid (RitemTid

item_count (R	itemCount
nation (Rnation
sid	 (Rsid
des_x
 (RdesX
des_z (RdesZ
state (Rstate:�������Config��1"�
TestCommand
index (Rindex%
version (2.PB.VersionRversion

table_name (	R	tableName
	page_name (	RpageName
comments (	Rcomments
cmd (	Rcmd
param	 (	B��
Rparam:�������Abandon"�
DailyRecharge
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsK
daily_rewards (2 .PB.DailyRecharge.daily_reward_tB��RdailyRewards
activity_id (R
activityId)
activity_version (RactivityVersion<
daily_reward_t
days (Rdays
reward (Rreward:�������Config��0"�
DailyRechargeSuiXin
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId)
activity_version (RactivityVersion!
	reward_id (B��	RrewardId:�������Config��0"�
Title
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

title_name (	R	titleName
path_id (RpathId
title_class (R
titleClass
quality (Rquality

sort_index	 (R	sortIndex
expire_time
 (R
expireTime!
addon_active (RaddonActive#
addon_passive (RaddonPassive

need_speak (R	needSpeak:
speak_channel (2.PB.CHAT_CHANNEL_ENUMRspeakChannel/
mask (2.PB.EXP_ACHIEVESECTION_MASKRmask
	desc_text (	RdescText#
rgb_gradient1 (RrgbGradient1#
rgb_gradient2 (RrgbGradient2
is_count (RisCount&
title_show_type (RtitleShowType: �������Temp��称号��2"w
BuyHireTime&
type (2.PB.ActivationTypeRtype!
cost_diamond (RcostDiamond

cost_money (R	costMoney"�
AssistantNPC
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId%
achievement_id (RachievementId
item_id (RitemId9
buy_hire_cost	 (2.PB.BuyHireTimeB��RbuyHireCost;
activation_type
 (2.PB.ActivationTypeRactivationType
useBT (RuseBT
script_path (	R
scriptPath&
npc_type (2.PB.NPCTypeRnpcType
image_id (RimageId
charm_value (R
charmValue
path_id (RpathId
action_name (	R
actionName"
level_icon_id (RlevelIconId
is_hide (RisHide!
hide_context (	RhideContext:�������Abandon"�
MagicLevelInfo
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
level (Rlevel
addon_id (B��RaddonId
levelup_exp (R
levelupExp&
leader_addon_id	 (RleaderAddonId: �����	��Temp��阵法��1"�
PartnerMagic
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
unlock_item (R
unlockItem
	max_level (RmaxLevel(
level_info_id (B��
RlevelInfoId
icon_id	 (RiconId: �����	��Temp��阵法��1"�
PartnerSquad
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
unlock_level (RunlockLevel: �����	��Temp��阵法��1"�
PartnerTeam
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
shape_id (RshapeId
magic_id (RmagicId: �����	��Temp��阵法��1"�
ItemPartnerTeamExp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp:��
��	��Item��伙伴��1"�
FashionInfo

fashion_id (R	fashionId
src_item_id (R	srcItemId
famale_icon (R
famaleIcon
	male_icon (RmaleIcon

profession (R
profession"O
FashionInfoNew

fashion_id (R	fashionId

profession (R
profession"G
QualificationNumber
qua_min (RquaMin
qua_max (RquaMax"�
PetQualification
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsC
qualification (2.PB.QualificationNumberB��RqualificationF
v_qualification (2.PB.QualificationNumberB��RvQualification:�������Abandon"�
MaterialCombine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments-
src_item (2.PB.ItemInfoB��RsrcItem(
cost_money_count (RcostMoneyCount 
cost_repu_id (R
costRepuId&
cost_repu_count	 (RcostRepuCount,
cost_diamond_count
 (RcostDiamondCount,
is_combine_visible (RisCombineVisible*
can_quick_combine (RcanQuickCombine
describe (	Rdescribe5
cost_bind_diamond_count (RcostBindDiamondCount:�������Abandon"E
	SkillTime
skill_id (RskillId

skill_time (R	skillTime"�
SkillLianZhao
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2

skill_info (2.PB.SkillTimeB��R	skillInfo:�������SkillLianZhao��2"�
AppearanceTaskConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
addon_group_id (RaddonGroupId:�������Abandon"�
AppearanceFashionConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
addon_group_id (RaddonGroupId:�������Abandon"�
AppearanceTitleConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
addon_group_id (RaddonGroupId:�������Abandon"�
AppearanceAchievementConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
addon_group_id (RaddonGroupId:�������Abandon"�
JobOrderInfo(
need_charm_value (RneedCharmValue
describe (	Rdescribe
	max_count (RmaxCount9

apply_type (2.PB.JobOrderInfo.ApplyTypeR	applyType
apply_id (RapplyId$
addon_group_id (RaddonGroupId
skill_id (B��RskillId*
next_job_order
 (B��RnextJobOrderK
contribution (2!.PB.JobOrderInfo.ContributionInfoB��Rcontribution
path_id (RpathId

reward_tid (R	rewardTid&
level_up_reward (RlevelUpRewardM
ContributionInfo#
type (2.PB.WelfareTypeRtype
count (Rcount"�
	ApplyType
UNKNOWN_ApplyType 
AT_ITEM

AT_DIAMOND
AT_BIND_DIAMOND
AT_CHARM_VALUE
AT_MONEY
AT_TASK"�
JobOrderConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
job_info (2.PB.JobOrderInfoRjobInfo7
showtype (2.PB.JobOrderConfig.ShowTypeRshowtype"#
ShowType

NoneUI 
CharmUI:�������Abandon"5
	LevelRank
level (Rlevel
rank (Rrank"�
ClimbTopRank
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2

level_rank (2.PB.LevelRankB��R	levelRank:�������Abandon"�
RewardForSect
level (Rlevel.
teacher_show_reward (RteacherShowReward.
student_show_reward (RstudentShowReward,
teacher_gift_star1 (RteacherGiftStar1,
teacher_gift_star2 (RteacherGiftStar2,
teacher_gift_star3 (RteacherGiftStar3!
student_gift (RstudentGift
speak_id (RspeakId%
achievement_id	 (RachievementId0
event_complete_count
 (ReventCompleteCount&
max_event_count (RmaxEventCount"�
TeacherReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments8
student_level_difference (RstudentLevelDifference#
student_level (RstudentLevel:
level_reward (2.PB.RewardForSectB��RlevelReward: �������Temp��师徒��0"�
NotMustEvent
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments,

event_type (2.PB.EventTypeR	eventType

need_count (R	needCount
task_id (RtaskId
instance_id (R
instanceId
	reward_id	 (RrewardId%
achievement_id
 (RachievementId:�������Abandon"�
PetSkillInfo
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
skill_id (RskillId
skill_level (R
skillLevel

skill_rate (R	skillRate=
skll_effect (2.PB.PetSkillInfo.SkillEffectR
skllEffect
icon	 (Ricon
desc
 (	Rdesc
addon_id (RaddonId
score (Rscore"�
SkillEffect
UNKNOWN_SkillEffect 
	SE_ACTIVE
SE_TEST
SE_SCALEPHYATK
SE_SCALEMAGATK
SE_SCALEPHYDEF
SE_SCALEMAGDEF

SE_SCALEHP
SE_PHYDAMADD
SE_MAGDAMADD	
SE_PHYDAMRES

SE_MAGDAMRES
	SE_PHYCRI
	SE_MAGCRI
SE_PHYCRIRES
SE_MAGCRIRES
SE_PHYCRIRATIO
SE_MAGCRIRATIO
SE_PHYCRIRATIORES
SE_MAGCRIRATIORES

SE_WINDDEF

SE_FIREDEF
SE_WATERDEF
SE_THUNDERDEF

SE_WINDDAM

SE_FIREDAM
SE_WATERDAM
SE_THUNDERDAM: �����	��Temp��宠物��1"i
InnerDanInfo
level (Rlevel

item_count (R	itemCount$
addon_group_id (RaddonGroupId"�
PetInnerDanUpgrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments<
inner_dan_cost (2.PB.InnerDanInfoB��dRinnerDanCost: �����	��Temp��宠物��0"W
PetInnerDan 
inner_dan_id (R
innerDanId&
inner_dan_level (RinnerDanLevel"�
PetSkillGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments5
inner_dan_info (2.PB.PetInnerDanRinnerDanInfo
	pet_skill (RpetSkill$
addon_group_id (RaddonGroupId: �����	��Temp��宠物��1"�
SkillAddonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
skill_id (RskillId
skill_level (R
skillLevel$
addon_group_id (RaddonGroupId:"�������SkillAddonConfig��2"x
	AmityInfo
	min_amity (RminAmity

level_name (	R	levelName
rate (Rrate
	max_amity (RmaxAmity"�
AmityLevelInfo
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2

amity_info (2.PB.AmityInfoB��R	amityInfo: �������Temp��好友��0"�
PhotoConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments9

photo_type (2.PB.PhotoConfig.PHOTO_TYPER	photoType
photo_id (RphotoId
addon_id (RaddonId
path_id (RpathId%
default_active
 (RdefaultActive 
description (	Rdescription*
shader_texture_id (RshaderTextureId$
female_path_id (RfemalePathId
activity_id (R
activityId"@

PHOTO_TYPE
UNKNOWN_PHOTO_TYPE 
PT_PHOTO
PT_FRAME:�������Abandon"�
TeacherTask
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_count (RmaxCount
show_reward (R
showReward
send_reward (R
sendReward: �������Temp��师徒��0"�
TeacherInstance
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_count (RmaxCount
show_reward (R
showReward
send_reward (R
sendReward: �������Temp��师徒��0"�
FriendOrdinaryGift
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

cost_score (R	costScore
	min_amity (RminAmity
speak_id (RspeakId
gender (Rgender
limit_count	 (R
limitCount: �������Temp��好友��0"�
CorpsTournament
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
campaign_id (R
campaignId.
required_role_level (RrequiredRoleLevel0
required_corps_level (RrequiredCorpsLevel(
inst_campaign_id (RinstCampaignId/
max_corps_per_nation	 (RmaxCorpsPerNationA
seasons (2!.PB.CorpsTournament.season_configB��Rseasons*
zige_instance_tid (RzigeInstanceTid.
jinjie_instance_tid (RjinjieInstanceTid1
corps_zige_toplist_id (RcorpsZigeToplistId&
max_history_num (RmaxHistoryNum,
jinjie_battle_time (RjinjieBattleTime/
role_zige_toplist_id (RroleZigeToplistId�
season_config
name (	Rname

start_date (	R	startDate#
zige_duration (RzigeDuration'
jinjie_duration (RjinjieDuration: �������Temp��公会��0"�
CorpsTournamentGround
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
title (	Rtitle
groups (B��Rgroups: �������Temp��公会��0"�
CorpsTournamentGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
title (	Rtitle
rewards (B��Rrewards: �������Temp��公会��0"�
InstanceLoop
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
loop_fake_tid (RloopFakeTid!
	loop_tids (B��
RloopTids:�������Abandon"�
RideskillLevelConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_level (RmaxLevel
	train_exp (RtrainExp
level (Rlevel,
require_item_count (RrequireItemCountM
require_item	 (2$.PB.RideskillLevelConfig.item_configB��
RrequireItem$
addon_group_id
 (RaddonGroupId 
level_up_exp (R
levelUpExp-
level_up_crit_ratio (RlevelUpCritRatioE
item_config
item_id (RitemId

item_count (R	itemCount: �����	��Temp��坐骑��0"�
RideskillQualityConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
quality (Rquality,
require_ride_level (RrequireRideLevel,
require_role_level (RrequireRoleLevel$
addon_group_id (RaddonGroupId
	reward_id	 (RrewardId: �����	��Temp��坐骑��0"�
CorpsInstance
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
instance_tid (RinstanceTid
activity_id (R
activityId.
faction_level_limit (RfactionLevelLimit5
faction_join_time_limit (RfactionJoinTimeLimit: �������Temp��公会��0"�
PartnerAmityConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	min_amity (RminAmity
	max_amity (RmaxAmity$
addon_group_id (RaddonGroupId: �����	��Temp��伙伴��0"�
partner_common_config
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
percent (Rpercent!
hiring_count (RhiringCount

poor_level (R	poorLevel: �����	��Temp��伙伴��0"�
partner_level_config
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
money (Rmoney?
items (2#.PB.partner_level_config.MailRewardB��RitemsD

MailReward
item_id (RitemId

item_count (R	itemCount: �����	��Temp��伙伴��0"�
SignedInAwardConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
month (Rmonth!
	day_award (B��RdayAward:�������Config��0"�
FriendCommonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	lead_time (RleadTime!
recommend_cd (RrecommendCd.
sys_recommend_count (RsysRecommendCount#
interval_time (RintervalTime
mass_cd	 (RmassCd"
day_max_amity
 (RdayMaxAmity*
max_receive_point (RmaxReceivePoint&
min_amity_limit (RminAmityLimit&
min_level_limit (RminLevelLimit"
common_use_id (RcommonUseIdE

bless_info (2 .PB.FriendCommonConfig.BlessInfoB��R	blessInfo

like_level (R	likeLevel$
like_add_amity (RlikeAddAmity&
like_common_use (RlikeCommonUse
like_reward (R
likeReward+
be_like_common_use (RbeLikeCommonUse$
be_like_reward (RbeLikeRewardt
	BlessInfo
content (	Rcontent#
anonymous_spk (RanonymousSpk(
un_anonymous_spk (RunAnonymousSpk: �������Temp��好友��0"�
PVPBattleField
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

sort_index (R	sortIndex
	show_tips (	RshowTips
path_id (RpathId!
activity_tid (RactivityTid!
instance_tid	 (RinstanceTid
rank_id
 (RrankId#
reputation_id (RreputationIdL

conditions (2&.PB.PVPBattleField.AccomplishConditionB��R
conditions*
win_reputation_id (RwinReputationId2
winprob_reputation_id (RwinprobReputationId5
open_mvp_mark_system (B�� RopenMvpMarkSystem*
tank_die_limit (B��RtankDieLimit!
	kill_coef (B��dRkillCoef
die_coef (B��dRdieCoef%
assist_coef (B��R
assistCoef5
first_win_reputation_id (RfirstWinReputationId7
series_win_reputation_id (RseriesWinReputationId5
pvp_score_reputation_id (RpvpScoreReputationId-
pvp_score_grade_idx (RpvpScoreGradeIdx3
pvp_shop_reputation_id (RpvpShopReputationId&
pvp_rank_tid (B�� R
pvpRankTid*
all_reputation_id (RallReputationId*
guide_instance_id (RguideInstanceId"
guide_task_id (RguideTaskId�
AccomplishCondition\
condition_type (25.PB.PVPBattleField.AccomplishCondition.CONDITION_TYPERconditionType,
general_reward_tid (RgeneralRewardTid"�
CONDITION_TYPE
UNKNOWN 
CONDITION_TYPE_FIRST_WIN
CONDITION_TYPE_SERIES_WIN
CONDITION_TYPE_TEAM_WIN"
CONDITION_TYPE_TEAM_ACCOMPLISH:�������Abandon"�
PlaceSystemConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
exp (Rexp$
a_chest_weight (RaChestWeight$
b_chest_weight (RbChestWeight$
c_chest_weight (RcChestWeight(
air_chest_weight	 (RairChestWeight0
recommendFightValue
 (RrecommendFightValue!
	reward_id (B��RrewardId:(�������Config��放置玩法��0"�
	WingTrain
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
prof_id (RprofId'
property_tid (B��RpropertyTid: �����	��Temp��翅膀��0"�
PlaceCommonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
max_exp (RmaxExp(
max_reward_count (RmaxRewardCount#

reward_id1 (B��
R	rewardId1*
place_instance_id	 (RplaceInstanceId#

reward_id2 (B��R	rewardId2#

reward_id3 (B��R	rewardId3:(�������Config��放置玩法��0"u
	SkillInfo
comments (	Rcomments
name (	Rname
icon (Ricon$
addon_group_id (RaddonGroupId"�
PartnerSkillConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments>
initiative_skill (2.PB.SkillInfoB��RinitiativeSkill8
passive_skill (2.PB.SkillInfoB��RpassiveSkill: �����	��Temp��伙伴��1"�
ItemProperty
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsB
property (2 .PB.ItemProperty.PropertyElementB��Rproperty�
PropertyElement!
property_tid (RpropertyTid%
property_value (RpropertyValue!
increase_low (RincreaseLow%
increase_hight (RincreaseHight:�������Abandon"�
ItemStrenthen
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsF
	strenthen (2".PB.ItemStrenthen.StrenthenElementB��0R	strenthen�
StrenthenElement%
property_index (RpropertyIndex
lucky_value (R
luckyValue,
general_cost_index (RgeneralCostIndex
weight (Rweight: �����	��Temp��装备��1"�

ItemRefine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments:
refine (2.PB.ItemRefine.RefineElementB��0Rrefine'
strenthen_level (RstrenthenLeveld
RefineElement%
property_index (RpropertyIndex,
general_cost_index (RgeneralCostIndex: �����	��Temp��装备��1"�
ItemQuality
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2
general_cost_index (B��RgeneralCostIndex: �����	��Temp��装备��1"�
ItemEquipment
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp1

equip_type (2.PB.EQUIPTYPE_ENUMR	equipType1

equip_mask (2.PB.EQUIPSLOT_MASKR	equipMask1
file_model_male_lhand	 (RfileModelMaleLhand&
file_model_male
 (RfileModelMale5
file_model_female_lhand (RfileModelFemaleLhand*
file_model_female (RfileModelFemaleI
fashion_equip_type (2.PB.FASHION_EQUIP_TYPE_ENUMRfashionEquipType
level (Rlevel?
character_combo_id (2.PB.PROFTYPE_MASKRcharacterComboId.

req_gender (2.PB.GENDER_MASKR	reqGender
	req_level (RreqLevel#
attack_manner (RattackManner,
ak_type (2.PB.EXP_ATTACK_TYPERakType-
df_type (2.PB.EXP_DEFENCE_TYPERdfType 
id_base_prop (R
idBasePropA
qualitys (2.PB.ItemEquipment.QualityConfigB��Rqualitys8
equip_reinforce_types (B��RequipReinforceTypes7
equip_reinforce_attr_map (RequipReinforceAttrMap*
id_upgrade_config (RidUpgradeConfig,
max_lianxing_level (RmaxLianxingLevel,
id_lianxing_config (RidLianxingConfig?
exterior_decompose_config_id (RexteriorDecomposeConfigId8
inside_equip_base_prop_id (RinsideEquipBasePropIdB
inside_equip_gradeup_config_id  (RinsideEquipGradeupConfigIdJ
inside_equip_stone_shapes_count! (B��RinsideEquipStoneShapesCountt
inside_equip_stone_shapes" (23.PB.ItemEquipment.inside_equip_stone_shapes_messageB��RinsideEquipStoneShapesR
#inside_equip_hole_activate_qualitys$ (B��RinsideEquipHoleActivateQualitys&
max_addon_count% (RmaxAddonCount/
init_addon_min_count& (RinitAddonMinCount/
init_addon_max_count' (RinitAddonMaxCount*
refine_cost_money( (RrefineCostMoney:
mingwens* (2.PB.ItemEquipment.MINWENB��
Rmingwens&
soul_config_tid+ (RsoulConfigTid,
soul_convert_value, (RsoulConvertValue<
colors. (2.PB.ItemEquipment.COLOR_CONFIGB��Rcolors!
recasting_id/ (RrecastingId
holes_limit0 (R
holesLimit"
	qinggongs2 (B��R	qinggongs'
recommend_id4 (B��RrecommendId 
tag_score_id5 (R
tagScoreId&
equip_skill_typ6 (RequipSkillTyp
addon_level7 (R
addonLevel2
BaseEquipMaterail8 (B��
RBaseEquipMaterailX
QualityConfig
addon (Raddon
name (	Rname

file_icons (R	fileIcons�
!inside_equip_stone_shapes_messagea
inside_equip_stone_shapes_enum (2.PB.EXP_EQUIPSTONE_SHAPE_MASKRinsideEquipStoneShapesEnumr
MINWEN6
mingwen_color (2.PB.MINGWEN_COLORRmingwenColor0
mingwen_unlock_level (RmingwenUnlockLevel�
COLOR_CONFIG
pos_name (	RposName
pos_id (RposId
color_id (RcolorId&
is_double_color (RisDoubleColor:����	��Item��装备��3"�
FriendsLoveConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsD
relation (2".PB.FriendsLoveConfig.LoveRelationB��Rrelationy
LoveRelation
	min_amity (RminAmity
	max_amity (RmaxAmity
	love_name (	RloveName
rate (Rrate: �������Temp��好友��0"�
CommonScoreGrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments=
grades (2.PB.CommonScoreGrade.grade_infoB��
Rgrades"

fx_path_id (B�� RfxPathId�

grade_info
	grade_idx (RgradeIdx
	award_tid (RawardTid
lower_bound (R
lowerBound
	title_tid (RtitleTid

grade_name (	R	gradeName

grade_icon (	R	gradeIcon:�������Abandon"�
BeautyPageantConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments)
recommand_tag (B��RrecommandTag!
	charm_add (B��
RcharmAdd
tag_add
 (B��
RtagAdd
point_add_A (R	pointAddA
point_add_B (R	pointAddB

point_base (R	pointBase:"�������Config��选美��0"�
BeautyPageantAllConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

days_cycle (R	daysCycle
limit_level (R
limitLevel,
weight_server_like (RweightServerLike4
weight_server_autolike (RweightServerAutolike(
base_server_like	 (RbaseServerLike*
limit_friend_like
 (RlimitFriendLike&
limit_left_like (RlimitLeftLike*
time_like_recover (RtimeLikeRecover#
likenum_award (RlikenumAward.
award_first_likenum (RawardFirstLikenumS
award_likenum (2(.PB.BeautyPageantAllConfig.likenum_gradeB��RawardLikenum$
count_day_like (RcountDayLike$
award_day_like (RawardDayLikeM
award_grade (2&.PB.BeautyPageantAllConfig.limit_gradeB��
R
awardGrade�
likenum_grade

count_left (R	countLeft
count_right (R
countRight

rate_award (R	rateAward
award_id (RawardId�
limit_grade

limit_left (R	limitLeft
limit_right (R
limitRight
award_id (RawardId
title_id (RtitleId

mail_title (	R	mailTitle!
mail_content (	RmailContent:"�������Config��选美��0"�
BeautyPageantAdditionConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
type (Rtype!
	tag_point (B��
RtagPoint:"�������Config��选美��0"�

	ClashClan
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments0
prepare_instance_tid (RprepareInstanceTid.
battle_instance_tid (RbattleInstanceTid0
prepare_activity_tid (RprepareActivityTid.
battle_activity_tid	 (RbattleActivityTid&
result_win_icon
 (	RresultWinIcon(
result_fail_icon (	RresultFailIcon
	open_days (RopenDays.
activation_down (B���RactivationDown*
activation_up (B���RactivationUp
level_limit (R
levelLimit$
join_day_limit (RjoinDayLimit,
common_grade_index (RcommonGradeIndex8
charms (2.PB.ClashClan.CharmElementB��Rcharms6
clash_kick_out_award_tid (RclashKickOutAwardTid-
clash_win_award_tid (RclashWinAwardTid/
clash_lose_award_tid (RclashLoseAwardTid:
clash_battle_win_award_tid (RclashBattleWinAwardTid<
clash_battle_lose_award_tid (RclashBattleLoseAwardTid;
create_pklist_activity_tid (RcreatePklistActivityTid4
prepare_activity_index (RprepareActivityIndex2
battle_activity_index (RbattleActivityIndex0
corps_level_limit (B��RcorpsLevelLimit�
CharmElement1

grade_type (2.PB.CharmGradeTypeR	gradeType
npc_tid (RnpcTid&
call_npc_repuid (RcallNpcRepuid: �������Temp��公会��0"�
CorpsWarConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
instance_tid (RinstanceTid2
generate_activity_idx (RgenerateActivityIdx2
generate_activity_tid (RgenerateActivityTid*
prep_activity_idx (RprepActivityIdx*
prep_activity_tid	 (RprepActivityTid.
battle_activity_idx
 (RbattleActivityIdx.
battle_activity_tid (RbattleActivityTid+
participant_limit (RparticipantLimit*
corps_level_limit (RcorpsLevelLimit,
player_level_limit (RplayerLevelLimit&
join_time_limit (RjoinTimeLimit&
reward_task_tid (RrewardTaskTid:"�������Config��跨服��0"�
RepuAutoReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
repu_id (RrepuId?
targets (2.PB.RepuAutoReward.target_valueB��
Rtargets.
control_activity_id (RcontrolActivityId'
current_version	 (RcurrentVersion)
activity_version
 (RactivityVersionD
clear_repu_when_version_changed (RclearRepuWhenVersionChanged<
target_value
value (Rvalue
reward (Rreward:�������Config��1"�
GuaJiPosConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
scene_id (RsceneId7
pos_list (2.PB.GuaJiPosConfig.PosB��RposList
	level_max (RlevelMax 
description	 (	Rdescription
icon_id
 (RiconId
	level_min (RlevelMin/
Pos
pos_x (RposX
pos_z (RposZ:�������Abandon"�
CorpsAnswerConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments#
level_context (	RlevelContext#
charm_context (	RcharmContext#
fight_context (	RfightContext#
achie_context (	RachieContext#
contr_context	 (	RcontrContextH
sex_context (2!.PB.CorpsAnswerConfig.TypeContextB��R
sexContextJ
prof_context (2!.PB.CorpsAnswerConfig.TypeContextB��RprofContextR
position_context (2!.PB.CorpsAnswerConfig.TypeContextB��RpositionContext7
TypeContext
id (Rid
context (	Rcontext: �������Temp��公会��0"�
PetBaseAttribute
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments-
constitution_point (RconstitutionPoint%
strength_point (RstrengthPoint%
internal_point (RinternalPoint
savvy_point (R
savvyPoint$
god_read_point	 (RgodReadPoint: �����	��Temp��宠物��0"�
PetLevelAttribute
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
level (B���Rlevel: �����	��Temp��宠物��0"�
PartnerAmityTrain
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
general_item_id (RgeneralItemId*
add_partner_amity (RaddPartnerAmity.
quality1_need_amity (Rquality1NeedAmity.
quality2_need_amity (Rquality2NeedAmity.
quality3_need_amity	 (Rquality3NeedAmity.
quality4_need_amity
 (Rquality4NeedAmity.
quality5_need_amity (Rquality5NeedAmity: �������Temp��好友��0"�
PartnerAmityUpgrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments?
levels (2 .PB.PartnerAmityUpgrade.LevelExpB���Rlevels6
LevelExp
exp (Rexp
context (	Rcontext: �������Temp��好友��0"�
PartnerQuality
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsD
initi_skill (2.PB.PartnerQuality.SkillInfo2B��R
initiSkill5
passi_skill1_need_level (RpassiSkill1NeedLevel5
passi_skill2_need_level (RpassiSkill2NeedLevel5
passi_skill3_need_level	 (RpassiSkill3NeedLevel5
passi_skill4_need_level
 (RpassiSkill4NeedLevel>
quality_unlock_specialty (B��RqualityUnlockSpecialty>
upgrade_cost_item (2.PB.ItemInfoB��RupgradeCostItem
	debris_id (RdebrisId$
debris_add_exp (RdebrisAddExp'
level_up_prop_id (RlevelUpPropIdL

SkillInfo2
skillseq_id (R
skillseqId

need_level (R	needLevel: �����	��Temp��伙伴��0"�
PartnerSpecialtyGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
weight (B��Rweight:�������Abandon"�
EscortRandomEvent
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments0
interval_lower_bound (RintervalLowerBound0
interval_upper_bound (RintervalUpperBound
cooldown (RcooldownG

event_list	 (2".PB.EscortRandomEvent.EventElementB��
R	eventList�
EventElement?

event_type (2 .PB.EscortRandomEvent.EVENT_TYPER	eventType
param_id (RparamId

event_tips (	R	eventTips
icon_id (RiconId!
event_weight (ReventWeight
event_limit (R
eventLimit"o

EVENT_TYPE
UNKNOWN_EVENT_TYPE 
EVENT_TYPE_BUFF
EVENT_TYPE_CREATE_MONSTER
EVENT_TYPE_REWARD:�������Abandon"�
FestivalSignedInAward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
activityopen (	Ractivityopen
month (Rmonth,
day_award_start (B��RdayAwardStart!
	day_award	 (B��RdayAward:�������Abandon"�
PartnerCommissionedTask
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
use_time (RuseTime
icon_id (RiconId
graded (Rgraded2
recommend_amity_level	 (RrecommendAmityLevel*
bottom_reward_tid
 (RbottomRewardTid,
success_reward_tid (RsuccessRewardTid&
rand_reward_tid (RrandRewardTid
rand_num (RrandNum-
essential_skill (B��RessentialSkill"
is_time_limit (RisTimeLimit 
pick_up_time (R
pickUpTime
	cost_type (RcostType

cost_count (R	costCount!
skill_weight (RskillWeight!
level_weight (RlevelWeight#
partner_count (RpartnerCount)
difficulty_level (RdifficultyLevel: �����	��Temp��门客��1"�
PartnerCommissionedTaskList
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
daily_task_limit (RdailyTaskLimit+
not_rec_limit_task (RnotRecLimitTask(
simul_task_limit (RsimulTaskLimit'
refresh_time	 (B��RrefreshTime!
cost_diamond
 (RcostDiamondL
grade (20.PB.PartnerCommissionedTaskList.difficulty_gradeB��
RgradeY
difficulty_grade
level_floor (R
levelFloor$

difficulty (B��R
difficulty:�������Abandon"�
PartnerHomeSkill
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments: �����	��Temp��门客��1"�
PartnerFetters
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
classification (	Rclassification
name (	Rname>
partner (2.PB.PartnerFetters.PartnerInfoB��Rpartner
addon_id	 (RaddonId
story_level
 (R
storyLevelF
PartnerInfo

partner_id (R	partnerId
quality (Rquality: �����	��Temp��门客��0"�
PartnerAddon2Role
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments 
addon_id (B���RaddonId: �����	��Temp��门客��0"�
QingGongConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
stage_speed (B��
R
stageSpeed:�������Abandon"�
RideRushConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
stage_speed (B��
R
stageSpeed: �����	��Temp��坐骑��0"�
ExpDayConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
	exp_limit (B���RexpLimit:�������Abandon"�
ItemDescConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
itemID (RitemID9
first_desc_color_index (B��RfirstDescColorIndex

first_desc (	R	firstDesc;
second_desc_color_index (B��RsecondDescColorIndex
second_desc	 (	R
secondDesc: �������ItemDescConfig��1"�
FragmentConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
fragment_id (R
fragmentId 
icon_path_id (R
iconPathId:�������Abandon"�
FriendEmotionConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments.
max_special_friends (RmaxSpecialFriends*
min_require_level (RminRequireLevel*
min_require_amity (RminRequireAmity*
same_team_seconds (RsameTeamSeconds
emotion_inc	 (R
emotionInc
max_emotion
 (R
maxEmotion*
max_daily_emotion (RmaxDailyEmotion
title (B��Rtitle&
force_remove_id (RforceRemoveId@
rewards (2 .PB.FriendEmotionConfig.reward_tB��Rrewards
activity_id (R
activityIdQ
reward_t
value (Rvalue
name (	Rname
	reward_id (RrewardId: �������Temp��好友��0"�
ShanYuanInsConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	min_level (RminLevel)
can_get_min_level (RcanGetMinLevel

level_diff (R	levelDiff!
shanyuan_num (RshanyuanNum
	limit_num	 (RlimitNum:�������Abandon"�
ShanYuanTaskConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	min_level (RminLevel)
can_get_min_level (RcanGetMinLevel

level_diff (R	levelDiff!
shanyuan_num (RshanyuanNum
	limit_num	 (RlimitNum:�������Abandon"�
TeamFightCapacityConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
fighting_capacity (RfightingCapacity: �����	��Temp��阵法��0"�
TeacherCommonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
server_level (RserverLevel%
activity_value (RactivityValueG
activity (2%.PB.TeacherCommonConfig.activity_infoB��Ractivity4
student_interval_level	 (RstudentIntervalLevel4
teacher_interval_level
 (RteacherIntervalLevel#
student_title (RstudentTitle#
teacher_title (RteacherTitle'
student_context (	RstudentContext'
teacher_context (	RteacherContext�
activity_info!
min_activity (RminActivity!
max_activity (RmaxActivity
	reward_id (RrewardId%
teacher_reward (RteacherReward: �������Temp��师徒��0"�
	ChuanGong
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
acvite_min_level (RacviteMinLevel*
passive_min_level (RpassiveMinLevel$
diff_min_level (RdiffMinLevel(
active_limit_tid (RactiveLimitTid*
passive_limit_tid	 (RpassiveLimitTid
action_time
 (R
actionTime*
acvite_reward_tid (RacviteRewardTid5
acvite_reward_limit_tid (RacviteRewardLimitTid,
passive_reward_tid (RpassiveRewardTid7
passive_reward_limit_tid (RpassiveRewardLimitTid,
acvite_action_anim (	RacviteActionAnim.
passite_action_anim (	RpassiteActionAnim:�������Abandon"�
ChuanSongIcon
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
icon_id (RiconId:�������Abandon"�
CorpsPveConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
instance_tid (RinstanceTid!
activity_tid (RactivityTid#

rank_score (B��R	rankScore
	win_award	 (RwinAward

lose_award
 (R	loseAward#

rank_award (B��R	rankAward

join_award (R	joinAward%
begin_comments (RbeginComments!
win_comments (RwinComments#
lose_comments (RloseComments

del_layers (R	delLayers.
activity_tid_reward (RactivityTidReward%
activity_index (RactivityIndex2
activity_reward_index (RactivityRewardIndex
limit_level (R
limitLevel: �������Temp��公会��0"�
PetCommonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments#

fight_rate (B��R	fightRate*
compose_cost_item (RcomposeCostItem.
cost_item_counts	 (B��RcostItemCounts&
dragon_bone_tid
 (RdragonBoneTid$
bone_add_zizhi (RboneAddZizhi$
spirit_gem_tid (RspiritGemTid
level_delta (R
levelDelta,
battle_level_delta (RbattleLevelDelta(
revival_use_time (RrevivalUseTime*
free_xidian_times (RfreeXidianTimes9
pet_levelup_general_param (RpetLevelupGeneralParam: �����	��Temp��宠物��0"�
TeamAddonExp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
time_cool_down (B���RtimeCoolDown,
mem_count_limit (B��RmemCountLimit
	exp_ratio (RexpRatio:
list	 (2 .PB.TeamAddonExp.ActivityElementB��2Rlist}
ActivityElement!
activity_tid (RactivityTid,
general_reward_tid (RgeneralRewardTid
task_tid (RtaskTid:�������Abandon"�
CoupleCampaign
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2
daily_sincerity_limit (RdailySincerityLimit2
sincerity_each_flower (RsincerityEachFlower.
sincerity_each_kill (RsincerityEachKillV
marriage_quality	 (2%.PB.CoupleCampaign.marriage_quality_tB��RmarriageQuality2
sincerity_of_instance
 (RsincerityOfInstance?
intimate (2.PB.CoupleCampaign.intimate_tB��Rintimate,
daily_max_intimate (RdailyMaxIntimate�
marriage_quality_t
level (Rlevel
	sincerity (R	sincerity
	reward_id (RrewardId
name (	Rname
icon (Ricon!
title_reward (RtitleReward�

intimate_tG
	cost_type (2*.PB.CoupleCampaign.CoupleCampaignCOST_TYPERcostType

cost_count (R	costCount#
sincerity_add (RsincerityAdd
	reward_id (RrewardId"T
CoupleCampaignCOST_TYPE
UNKNOWN_COST_TYPE 	
YINBI

LINGYU
YUANBAO: �������Temp��好友��0"�
GmtMailActivityItem
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId
item_id (RitemId:�������Abandon"�
NightPacksAward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

begin_time (	R	beginTime
end_time (	RendTimeE
award_grade (2.PB.NightPacksAward.pack_gradeB��R
awardGrade
activity_id	 (R
activityIdg

pack_grade

limit_left (R	limitLeft
limit_right (R
limitRight
award_id (RawardId:�������Config��0"�
InstanceExtend
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsP
"forbid_friend_partner_instance_tid (B��
RforbidFriendPartnerInstanceTid:�������Abandon"�

FortuneCat
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
count (Rcount
duration (RdurationH
extend_time_cost_type (2.PB.EXP_SHOPSELL_MODERextendTimeCostType3
extend_time_cost_value (RextendTimeCostValue>

cat_config
 (2.PB.FortuneCat.CostConfigB��R	catConfig

activit_id (R	activitId�

CostConfig4

money_type (2.PB.EXP_SHOPSELL_MODER	moneyType(
cost_money_value (RcostMoneyValue

gen_money1 (R	genMoney1

gen_money2 (R	genMoney2

gen_money3 (R	genMoney3:�������Abandon"�
DailyLimitRecharge
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId)
activity_version (RactivityVersion#
diamond_count (RdiamondCount

reward_tid (R	rewardTid:�������Config��0"�
BeastUpgradeQuality
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsL
upgrade_info (2#.PB.BeastUpgradeQuality.QualityInfoB��RupgradeInfo�
QualityInfo*
qualification (B��Rqualification
talent (Rtalent 
cost_item_id (R
costItemId

need_level (R	needLevel&
cost_item_count (RcostItemCount: �����	��Temp��宠物��0"�
TaskPreAchievement
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
task_id (RtaskId%
achievement_id (RachievementId:�������Abandon"�
AutoBuyService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
item_tid (B��
RitemTid:�������Npc��服务��0"�
AllServerRepuConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
repu_id (RrepuId/
broadcast_threshold (RbroadcastThreshold/
auto_inc_activity_id (RautoIncActivityId&
auto_inc_period (RautoIncPeriod$
auto_inc_value	 (RautoIncValue:�������Config��1"�
AIRecruitConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
tid (Rtid
	min_level (RminLevel
	max_level (RmaxLevel
	prof_mask (RprofMask!
auto_recruit	 (RautoRecruit!
recruit_type
 (RrecruitType 
once_add_rep (R
onceAddRep!
novice_count (RnoviceCount:�������Abandon"�
HometownCommonConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	time_type (RtimeType
time (Rtime,
home_task_activity (RhomeTaskActivity,
home_farm_activity (RhomeFarmActivity: �������Temp��家园��0"�
TreasureMapConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
x (Rx
y (Ry
z (Rz
radius (Rradius
	min_level	 (RminLevel
	max_level
 (RmaxLevel
scene_id (RsceneId:�������Config��0"�
RaffleRewardConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments@
rewards (2 .PB.RaffleRewardConfig.reward_tpB��Rrewards
base_reward (R
baseReward
activity_id (R
activityId!
cost_diamond	 (RcostDiamond&
cost_diamond_10
 (RcostDiamond10
is_bind (RisBind!
record_limit (RrecordLimit"
cost_item_tid (RcostItemTid"
cost_item_cnt (RcostItemCnt'
cost_item_cnt_10 (RcostItemCnt10)
cost_item_cnt_100 (RcostItemCnt100(
cost_diamond_100 (RcostDiamond100�
	reward_tp
rid (Rrid
weight (Rweight
record (Rrecord
total (Rtotal
least_count (R
leastCount%
greatest_count (RgreatestCount:�������Config��0"�
CorpsAuctionItemConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
begin_price (R
beginPrice(
no_bargain_price (RnoBargainPrice
bonus_money (R
bonusMoney
ratio (Rratio#
faction_ratio	 (RfactionRatio: �������Temp��公会��0"�
TouXian
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments5
elements (2.PB.TouXian.ElementB��
Relements�
Element
fight_value (R
fightValue
	item_tid1 (RitemTid1
item_count1 (R
itemCount1
	item_tid2 (RitemTid2
item_count2 (R
itemCount2&
group_addon_tid (RgroupAddonTid!
touxian_name (	RtouxianName:�������Abandon"�
AnglingConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId

open_level (R	openLevel!
reward_times (RrewardTimes
	reward_id (RrewardId
bait_id
 (B��RbaitId
team_weight (R
teamWeight
bait_weight (R
baitWeight!
reward_count (RrewardCount:"�������Config��钓鱼��0"�
AnglingRegionConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

activit_id (R	activitId
region_name (	R
regionName%
team_comments1 (	RteamComments1%
team_comments2 (	RteamComments2%
limit_comments	 (	RlimitComments'
weight_count
 (B��
RweightCountB
weight (2$.PB.AnglingRegionConfig.WeightConfigB��
Rweight
	filter_id (RfilterId
scene_id (RsceneId�
WeightConfig
	reward_id (RrewardId
base_weight (R
baseWeight!
team_weight1 (RteamWeight1!
team_weight2 (RteamWeight2%
bait_weight (B��R
baitWeight:"�������Config��钓鱼��0"�
DepositoryConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsB
	cost_list (2.PB.DepositoryConfig.UnlockCostB��RcostList�

UnlockCostI

money_type (2*.PB.DepositoryConfig.UnlockCost.MONEY_TYPER	moneyType
count (Rcount"O

MONEY_TYPE
UNKNOWN_MONEY_TYPE 

SILVER
DIAMOND
BIND_DIAMOND:�������Abandon"�
ReputationLimit
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments#
reputation_id (RreputationId,
pre_reputation_id1 (RpreReputationId1,
pre_reputation_id2 (RpreReputationId22

clear_type (2.PB.REPU_CLEAR_TYPER	clearType
total_limit	 (R
totalLimitE

repu_limit (2 .PB.ReputationLimit.LimitElementB��R	repuLimit

alert_name (	R	alertNameF
LimitElement

reward_tid (R	rewardTid
task_id (RtaskId:�������Config��1"�
GoodsDepositoryConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsH

goods_list (2#.PB.GoodsDepositoryConfig.GoodsInfoB��R	goodsList�
	GoodsInfo
	goods_idx (RgoodsIdx
weight (Rweight!
min_discount (RminDiscount!
max_discount (RmaxDiscount:�������Abandon"�
SpecialSellConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
	slot_list (B��RslotList6
refresh_weekday_list (B��RrefreshWeekdayListV
consume_level_list
 (2".PB.SpecialSellConfig.ConsumeLevelB��RconsumeLevelList!
refresh_cost (RrefreshCost*
max_refresh_count (RmaxRefreshCount!
activity_tid (RactivityTidP
ConsumeLevel#
consume_count (RconsumeCount
	reward_id (RrewardId:�������Abandon"�
GymAddonGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments8
options (2.PB.GymAddonGroup.OptionB��Roptions�
Option
icon (Ricon
name (	Rname
desc (	Rdesc,
addon_group_ids (B��RaddonGroupIds1
cost_contribution (B��RcostContribution: �������Temp��公会��0"�)
	CorpsProp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
be_regular_lvl (RbeRegularLvl2
be_regular_contribute (RbeRegularContribute?
be_regular_join_duration (B����
RbeRegularJoinDuration@
kick_apprentice_duration (B����RkickApprenticeDuration*
be_apprentice_lvl	 (RbeApprenticeLvl8
statistic_recent_days
 (B��RstatisticRecentDays0
impeach_duration (B���RimpeachDuration9
impeach_need_join_days (B��RimpeachNeedJoinDays6
vote_impeach_percent (B��RvoteImpeachPercent1
campaign_fire_tid (B���RcampaignFireTid=
	cost_type (2 .PB.CorpsProp.CorpsPropCOST_TYPERcostType
cost_num (B��
RcostNum 
cost_item_id (R
costItemId&
require_vip_lvl (RrequireVipLvl2
require_player_lvl (B��RrequirePlayerLvl:
refresh_apply_corps_cd (B���RrefreshApplyCorpsCd7
refresh_apply_corps_n (B��dRrefreshApplyCorpsN0
kick_cost_repu_id (B���RkickCostRepuId1
kick_cost_repu_num (B�� RkickCostRepuNum6
group_msg_cost_money (B���RgroupMsgCostMoney6
group_msg_week_free_n (B��RgroupMsgWeekFreeN-
join_be_regular_lvl (RjoinBeRegularLvl<
pos_nums (2.PB.CorpsProp.pos_num_upperB��RposNumsC
nhours_offline_auto_impeach (B��0RnhoursOfflineAutoImpeach*
base_unlock_level (RbaseUnlockLevel*
shop_unlock_level (RshopUnlockLevel(
gym_unlock_level  (RgymUnlockLevel.
coffer_unlock_level! (RcofferUnlockLevel7
levelups# (2.PB.CorpsProp.LevelUpB��Rlevelups1
levels% (2.PB.CorpsProp.LevelB��Rlevels>
daily_reward_cmn_uselimit_id& (RdailyRewardCmnUselimitId(
alchemy_repu_max' (RalchemyRepuMax;

gym_levels) (2.PB.CorpsProp.GymLevelB��R	gymLevels>
shop_levels+ (2.PB.CorpsProp.ShopLevelB��R
shopLevelsD
coffer_levels- (2.PB.CorpsProp.CofferLevelB��RcofferLevels1
donation_cmn_limit_id. (RdonationCmnLimitId:
	donations0 (2.PB.CorpsProp.DonationB��R	donations&
holyboss_npc_id2 (RholybossNpcId=
holyboss_summon_need_growth3 (RholybossSummonNeedGrowthF
 holyboos_feeding_common_limit_id4 (RholyboosFeedingCommonLimitIdr
$holyboss_feeding_rank_award_id_count5 (2#.PB.EXP_HOLYBOSS_FEEDING_RANK_INDEXRholybossFeedingRankAwardIdCountH
holyboss_feeding_rank_award_id6 (B��RholybossFeedingRankAwardId:
	campfires8 (2.PB.CorpsProp.CampfireB��R	campfiresG
!defence_battle_max_count_per_week9 (RdefenceBattleMaxCountPerWeek9
defence_battle_time_limit: (RdefenceBattleTimeLimit,
guild_tree_diamond; (RguildTreeDiamond
speak_id< (RspeakId.
kickout_count_limit= (RkickoutCountLimit(
re_join_cooldown> (RreJoinCooldownG
 contribution_bonus_factor_master? (RcontributionBonusFactorMasterO
$contribution_bonus_factor_vicemaster@ (R!contributionBonusFactorVicemasterK
"contribution_bonus_factor_zhanglaoA (RcontributionBonusFactorZhanglaoE
contribution_bonus_factor_eliteB (RcontributionBonusFactorEliteJ
"chariot_camp_require_faction_levelC (RchariotCampRequireFactionLevel[
chariot_camp_level_upsE (2 .PB.CorpsProp.ChariotCampLevelUpB��RchariotCampLevelUpsD
chariot_campsG (2.PB.CorpsProp.ChariotCampB��RchariotCamps%
chariot_idsI (B��R
chariotIdsT
pos_num_upper
pos (Rpos
upper (Rupper
	corps_lvl (RcorpsLvl�
LevelUp"
contribution (Rcontribution#
faction_funds (RfactionFunds*
buildings_lvl_sum (RbuildingsLvlSum%
funds_required (RfundsRequired�
Level'
max_apprentices (RmaxApprentices!
max_regulars (RmaxRegulars
max_members (R
maxMembers
funds_level (R
fundsLevel&
daily_reward_id (RdailyRewardId*
alchemy_reward_id (RalchemyRewardId�
GymLevel+
cost_contribution (RcostContribution

cost_money (R	costMoney*
base_building_lvl (RbaseBuildingLvl%
funds_required (RfundsRequired&
gym_addon_group (RgymAddonGroup�
	ShopLevel+
cost_contribution (RcostContribution

cost_money (R	costMoney*
base_building_lvl (RbaseBuildingLvl%
funds_required (RfundsRequired#
shop_discount (RshopDiscount�
CofferLevel+
cost_contribution (RcostContribution

cost_money (R	costMoney*
base_building_lvl (RbaseBuildingLvl%
funds_required (RfundsRequired�
Donation4

money_type (2.PB.EXP_SHOPSELL_MODER	moneyType!
money_amount (RmoneyAmount"
award_repu_id (RawardRepuId*
award_repu_amount (RawardRepuAmount.
award_faction_funds (RawardFactionFundso
Campfire7
open_campfire_cost_money (RopenCampfireCostMoney*
exp_rate_permille (RexpRatePermille`
ChariotCampLevelUp+
cost_construction (RcostConstruction

cost_funds (R	costFunds�
ChariotCamp*
max_chariot_count (RmaxChariotCountS
init_chariots (2(.PB.CorpsProp.ChariotCamp.InitialChariotB��RinitChariotsT
InitialChariot

chariot_id (R	chariotId#
chariot_level (RchariotLevel"�
CorpsPropCOST_TYPE
UNKNOWN_COST_TYPE 
COST_BIND_MONEY
COST_TRADE_MONEY
COST_MIX_MONEY
COST_BIND_CASH
COST_TRADE_CASH
COST_MIX_CASH: �������Temp��公会��0"�
Hero
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
title (	Rtitle9
celebrity_type (2.PB.CELEBRITY_TYPERcelebrityType2

profession (2.PB.PROF_TYPE_ENUMR
profession1

mount_type (2.PB.EXP_MOUNT_TYPER	mountType

max_riders	 (R	maxRiders2
qualitys (2.PB.Hero.QualityB��Rqualitys
	sfx_mount (RsfxMount
sfx_unmount (R
sfxUnmount1

equip_mask (2.PB.EQUIPSLOT_MASKR	equipMask&
base_move_speed (RbaseMoveSpeed
inhesion_id (R
inhesionId
desc (	Rdesc0
break_section_ids (B��RbreakSectionIds:
property_pill_use_group_id (RpropertyPillUseGroupId!
transform_id (RtransformId8

movePhases (2.PB.Hero.MovePhaseB��R
movePhases�
Quality
name (	Rname
	icon_path (RiconPath

model_path (R	modelPath&
gradeup_item_id (RgradeupItemId�
	MovePhase
maxSpeed (RmaxSpeed"
acceleration (Racceleration"
maxTurnAngle (RmaxTurnAngle*
turnAcceleration (RturnAcceleration: �����	��Temp��坐骑��1"�
HeroBreakSection
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
icon_id (RiconId
	max_level (RmaxLevel<
levels (2.PB.HeroBreakSection.HeroLevelB��2Rlevels�
	HeroLevel"
cost_item_tid (RcostItemTid&
cost_item_count (RcostItemCount!
upgrade_rate (RupgradeRate&
max_bless_value (RmaxBlessValue
addon_group (R
addonGroup: �����	��Temp��坐骑��0"�
ExchangeActivity
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId%
activity_index (RactivityIndex
repu_id (RrepuId$
nation_repu_id (RnationRepuId$
server_repu_id	 (RserverRepuIdD
	exchanges (2 .PB.ExchangeActivity.OneExchangeB��R	exchanges,
reward_task_ids (B��RrewardTaskIdsQ
nation_rewards (2$.PB.ExchangeActivity.OneNationRewardB��RnationRewards[
all_nations_rewards (2%.PB.ExchangeActivity.AllNationsRewardB��RallNationsRewardsQ
server_rewards (2$.PB.ExchangeActivity.OneServerRewardB��RserverRewards,
assist_activity_id (RassistActivityIdC
assist_cost_type (2.PB.EXP_COMMON_ITEM_TYPE2RassistCostType*
assist_cost_count (RassistCostCount(
assist_add_value (RassistAddValue*
assist_reward_tid (RassistRewardTid,
extra_reward_times (RextraRewardTimes(
extra_reward_tid (RextraRewardTid�
OneExchange+
exchange_uselimit (RexchangeUselimit6
	cost_type (2.PB.EXP_COMMON_ITEM_TYPE1RcostType&
reward_item_tid (RrewardItemTid&
add_nation_repu (RaddNationRepu&
add_server_repu (RaddServerRepuE
costs (2).PB.ExchangeActivity.OneExchange.OneCost1B��RcostsM
OneCost1"
cost_item_tid (RcostItemTid

cost_count (R	costCountk
OneNationReward9
required_nation_rep_value (RrequiredNationRepValue

reward_tid (R	rewardTidN
AllNationsReward
	rep_value (RrepValue

reward_tid (R	rewardTidk
OneServerReward9
required_server_rep_value (RrequiredServerRepValue

reward_tid (R	rewardTid:�������Config��1"�
CommonPropertyItem
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
id (Rid
	low_value (RlowValue

high_value (R	highValue
weight (Rweight7
type	 (2#.PB.CommonPropertyItem.PropertyTypeRtype.
quality
 (2.PB.EXP_QUALITY_ENUMRquality
group_id (RgroupId
	min_level (RminLevel
	max_level (RmaxLevel
skill_id (RskillId"0
PropertyType
NONE 
PRIMARY	
MINOR:-�������Addon��通用附加属性��2"�
CommonPropertyGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsH

group_type (2).PB.CommonPropertyGroup.PropertyGroupTypeR	groupTypeC
random_type (2".PB.CommonPropertyGroup.RandomTypeR
randomType-
base_properties (B��RbaseProperties+
nproperty_rate
 (B��RnpropertyRate.
rand_properties (B���RrandProperties"x
PropertyGroupType
PGT_EQUIP_ESSENCE 
PGT_EQUIP_GROW
	PGT_STONE
PGT_SUIT
	PGT_TITLE
PGT_CARD"a

RandomType
RT_NONE 
RT_ALL_INDEPENDENT
	RT_WEIGHT
RT_COUNT
RT_EQUIP_CONF:2�������
AddonGroup��通用附加属性��2"�
InsideEquipBaseProp
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsA
props (2$.PB.InsideEquipBaseProp.OneLevelPropB���Rprops�
OneLevelProp
hp (Rhp#
physic_attack (RphysicAttack!
magic_attack (RmagicAttack%
physic_defence (RphysicDefence#
magic_defence (RmagicDefence: �����	��Temp��装备��1"�
GenerateItemConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
gen_type (RgenType$
time_limit_sec (RtimeLimitSec

bind_state (R	bindState#
equip_quality	 (RequipQuality2
equip_reinforce_level
 (RequipReinforceLevel

item_count (R	itemCount3
sell_require_repu_type (RsellRequireRepuType5
sell_require_repu_value (RsellRequireRepuValue:
shopsell_mode (2.PB.EXP_SHOPSELL_MODERshopsellMode

shop_price (R	shopPrice$
shop_repu_type (RshopRepuType&
shop_repu_value (RshopRepuValue 
shop_item_id (R
shopItemId"
shop_item_num (RshopItemNum:�������Abandon"�
Practice
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments9
prof_limit_mask (2.PB.PROFTYPE_MASKRprofLimitMask.

req_gender (2.PB.GENDER_MASKR	reqGender!
name_service (	RnameService

id_service (R	idService
	file_icon	 (RfileIcon
	min_level
 (RminLevel
	max_level (RmaxLevel3
id_pre_activated_wuxue (RidPreActivatedWuxue5
id_pre_learned_skillseq (RidPreLearnedSkillseq/
id_pre_learned_skill (RidPreLearnedSkill;
minlevel_pre_learned_skill (RminlevelPreLearnedSkill;
maxlevel_pre_learned_skill (RmaxlevelPreLearnedSkill
id_pre_task (R	idPreTask1
finish_count_pre_task (RfinishCountPreTask&
id_pre_lifeprof (RidPreLifeprof2
minlevel_pre_lifeprof (RminlevelPreLifeprof2
maxlevel_pre_lifeprof (RmaxlevelPreLifeprof9
	pre_items (2.PB.Practice.preitem_tB��RpreItems
id_pre_buff (R	idPreBuff0
can_repeately_finish (RcanRepeatelyFinish"
cool_time_sec (RcoolTimeSec!
duration_sec (RdurationSec#
segment_count (RsegmentCountE
consumeforce_value_pre_skill (B��RconsumeforceValuePreSkill;
odds_success_pre_skills  (B��RoddsSuccessPreSkills;
check_result_every_segment! (RcheckResultEverySegment&
break_when_succ" (RbreakWhenSucc+
consumeforce_type# (RconsumeforceType6
result_succ$ (2.PB.Practice.result_tR
resultSucc6
result_fail% (2.PB.Practice.result_tR
resultFailL
	preitem_t
id (Rid
num (Rnum

is_consume (R	isConsume�
result_t

reset_prop (R	resetProp
id_skill (RidSkill
level_skill (R
levelSkill-
add_pre_skill_level (RaddPreSkillLevel
id_item (RidItem
num_item (RnumItem
	exp_ratio (RexpRatio
force (Rforce
id_task	 (RidTask-
id_exp_level_config
 (RidExpLevelConfigE
learn_skill (2.PB.Practice.result_t.pskill_tB��R
learnSkillK
learn_skillseq (2.PB.Practice.result_t.pskill_tB��RlearnSkillseq.
pskill_t
id (Rid
odds (Rodds:�������Abandon"�"
InstanceConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
id_scene (RidScene0
pos_trans_in (2.PB.CoordinateR
posTransIn/
min_level_enter_inst (RminLevelEnterInst/
max_level_enter_inst (RmaxLevelEnterInstA
max_team_level_diff_enter_inst	 (RmaxTeamLevelDiffEnterInstB
prof_mask_enter_inst
 (2.PB.PROFTYPE_MASKRprofMaskEnterInst:
can_enter_in_other_country (RcanEnterInOtherCountry;
ignore_pre_inst_difficulty (RignorePreInstDifficulty$
accept_task_id (RacceptTaskId>
timelimit_type (2.PB.INST_TIMELIMIT_TYPERtimelimitType
	limit_num (RlimitNum/
can_finish_only_once (RcanFinishOnlyOnce'
can_enter_via_ui (RcanEnterViaUi)
id_item_open_inst (RidItemOpenInst5
min_difficulty_use_item (RminDifficultyUseItem)
be_open_item_lost (RbeOpenItemLost(
money_enter_inst (RmoneyEnterInst+
repu_id_enter_inst (RrepuIdEnterInst1
repu_value_enter_inst (RrepuValueEnterInst?
can_rejoin_after_quit_by_self (RcanRejoinAfterQuitBySelf.
require_activity_id (RrequireActivityIdP
quit_reset_repu (2".PB.InstanceConfig.RESET_REPU_INFOB��RquitResetRepu!
prepare_time (RprepareTime(
inst_revive_time (RinstReviveTime$
min_player_num (RminPlayerNum&
min_player_time (RminPlayerTime$
max_player_num  (RmaxPlayerNum6
instance_type! (2.PB.INSTANCE_TYPERinstanceType

time_limit" (R	timeLimit4
clear_numtimes_weekday# (RclearNumtimesWeekday.
clear_numtimes_hour$ (RclearNumtimesHour2
clear_numtimes_minute% (RclearNumtimesMinute%
instance_phase& (RinstancePhase2
requisite_instance_id' (RrequisiteInstanceIdB
requisite_instance_difficulty( (RrequisiteInstanceDifficulty
	can_sweep) (RcanSweep/
sweep_need_vip_level* (RsweepNeedVipLevel+
sweep_cost_item_id+ (RsweepCostItemId1
sweep_cost_item_count, (RsweepCostItemCount&
sweep_reward_id- (RsweepRewardId1
sweep_exp_modifier_id. (RsweepExpModifierId5
sweep_money_modifier_id/ (RsweepMoneyModifierId7
can_enter_when_reconnect0 (RcanEnterWhenReconnect>
limit_num_modifier_vipseq_id1 (RlimitNumModifierVipseqId>
limit_num_purchase_vipseq_id2 (RlimitNumPurchaseVipseqId=
purchase_price_vipseq_id5 (B��
RpurchasePriceVipseqId0

proc_death6 (2.PB.REVIVE_POLICYR	procDeath/
can_match_other_team7 (RcanMatchOtherTeam,
match_team_members8 (RmatchTeamMembers"
match_repu_id9 (RmatchRepuId-
match_repu_aver_min: (RmatchRepuAverMin.
match_faction_count; (RmatchFactionCountC
match_start_immediately_timout< (RmatchStartImmediatelyTimout*
match_team_strict= (RmatchTeamStrict.
match_result_strict> (RmatchResultStrict?
match_rank_type? (2.PB.AUTOMATCH_RANK_TYPERmatchRankTypeO
match_condition_mask@ (2.PB.AUTOMATCH_EXTRA_COND_MASKRmatchConditionMask;
match_auto_prepare_timeoutA (RmatchAutoPrepareTimeoutV
match_member_merge_ruleB (2.PB.AUTOMATCH_MEMBER_MERGE_RULERmatchMemberMergeRuleM
match_team_join_ruleC (2.PB.AUTOMATCH_TEAM_JOIN_RULERmatchTeamJoinRule>
multi_match_modeD (2.PB.MULTI_MATCH_RULERmultiMatchMode$
run_match_timeE (RrunMatchTime'
reorganize_teamF (RreorganizeTeam
nameG (	Rname4
require_activity_indexH (RrequireActivityIndex 
dir_trans_inI (R
dirTransIn*
special_mode_maskJ (RspecialModeMaskM
normalinst_enter_typeK (2.PB.NORMALINST_ENTER_TYPERnormalinstEnterType6
show_panel_when_has_taskL (RshowPanelWhenHasTask4
sweep_need_achievementM (RsweepNeedAchievement&
enter_fx_pathidN (RenterFxPathid$
exit_fx_pathidO (RexitFxPathidD
mode_configP (2.PB.InstanceConfig.ModeConfigB��R
modeConfigI
RESET_REPU_INFO
repu_id (RrepuId

repu_value (R	repuValueN
LevelTarget 
description (	Rdescription

reward_tid (R	rewardTid�

ModeConfigI
level_targets (2.PB.InstanceConfig.LevelTargetB��RlevelTargets7
first_time_finish_reward (RfirstTimeFinishReward&
min_level_limit (RminLevelLimit&
max_level_limit (RmaxLevelLimit2
requisite_instance_id (RrequisiteInstanceIdB
requisite_instance_difficulty (RrequisiteInstanceDifficulty:"�������Config��副本��1"�
TalentBreak
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments'
type (2.PB.EXP_TALENT_TYPERtype
icons (B��Ricons
desc (	Rdesc
	max_level	 (RmaxLevel9
configs (2.PB.TalentBreak.OneConfigB��Rconfigs�
	OneConfig$
addon_group_id (RaddonGroupId
	cost_item (RcostItem!
required_exp (RrequiredExp

need_level (R	needLevel$
need_talent_id (RneedTalentId*
need_talent_level (RneedTalentLevel7
need_quality (2.PB.EXP_QUALITY_ENUMRneedQuality: �����	��Temp��天赋��0"�
Talent
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments'
type (2.PB.EXP_TALENT_TYPERtype
icons (B��Ricons
desc (	Rdesc
	max_level	 (RmaxLevel5
configs (2.PB.Talent.OneConfig1B��Rconfigs�

OneConfig1$
addon_group_id (RaddonGroupId

cost_money (R	costMoney

cost_force (R	costForce

need_level (R	needLevel$
need_talent_id (RneedTalentId*
need_talent_level (RneedTalentLevel7
need_quality (2.PB.EXP_QUALITY_ENUMRneedQuality: �����	��Temp��天赋��0"�
TalentGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments9
prof_limit_mask (2.PB.PROFTYPE_MASKRprofLimitMask3
stages (2.PB.TalentGroup.StageB��$Rstages�
Stage
name (	Rname
talents (B��Rtalents9
quality_limit (2.PB.EXP_QUALITY_ENUMRqualityLimit6
need_max_level_talent_id (RneedMaxLevelTalentId: �����	��Temp��天赋��0"�
Chariot
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
property_id (R
propertyId5
props (2.PB.Chariot.OneLevelProp1B��
Rprops*
can_attack_player	 (RcanAttackPlayer�
OneLevelProp1
name (	Rname.
quality (2.PB.EXP_QUALITY_ENUMRquality!
transform_id (RtransformId.
need_build_progress (RneedBuildProgress*
need_chariot_camp (RneedChariotCamp(
repair_cost_time (RrepairCostTimeW
)cost_faction_contribution_base_to_receive (R$costFactionContributionBaseToReceive:
cost_money_base_to_receive (RcostMoneyBaseToReceive"
twins_npc_tid	 (RtwinsNpcTid-
aim_at_transform_id
 (RaimAtTransformId
	min_range (RminRange
	max_range (RmaxRange:�������Npc��战车��1"�
ChariotBuild
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsJ
build_options (2.PB.ChariotBuild.OneBuildOptionB��RbuildOptionsP
progress_speaks	 (2!.PB.ChariotBuild.OneProgressSpeakB��RprogressSpeaks�
OneBuildOption
uselimit_id (R
uselimitIdQ
faction_position_limit (2.PB.EXP_CORPS_POSITION_MASKRfactionPositionLimit(
add_progress_min (RaddProgressMin(
add_progress_max (RaddProgressMax<
	cost_type (2.PB.EXP_CHARIOT_BUILD_COST_TYPERcostType
cost_id (RcostId(
cost_amount_base (RcostAmountBase&
cost_amount_max (RcostAmountMax8
add_faction_contribution	 (RaddFactionContribution=
speak_add_progress_theshold
 (RspeakAddProgressTheshold
speak_id (RspeakIdo
OneProgressSpeak/
progress_percentage (RprogressPercentage*
progress_speak_id (RprogressSpeakId:�������Npc��战车��1"�
ChariotReceive
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments.
receive_uselimit_id (RreceiveUselimitId)
receive_duration (RreceiveDuration:�������Npc��战车��1"�
MagicWeapon
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
path_id (RpathId 
name_path_id (R
namePathId(
activate_path_id (RactivatePathId&
magic_weapon_id	 (RmagicWeaponId(
upgrade_item_tid
 (RupgradeItemTid,
upgrade_item_count (RupgradeItemCount
	use_limit (RuseLimit
use_time (RuseTime
power_up_id (R	powerUpIdB

properties (2.PB.MagicWeapon.OneProperty4B��R
properties+
property_rates (B��RpropertyRates
silver_rate (R
silverRate6
property_reward_base (B��RpropertyRewardBase,
silver_reward_base (RsilverRewardBaseL
total_level_infos (2.PB.MagicWeapon.TotalLevelB��RtotalLevelInfos,
max_multiply_limit (RmaxMultiplyLimitI
multiply_infos (2.PB.MagicWeapon.MultiplyInfoB��RmultiplyInfos
	max_level (RmaxLevel4
levels (2.PB.MagicWeapon.Level1B��Rlevels@
reset_infos  (2.PB.MagicWeapon.ResetInfoB��R
resetInfos:
property_pill_use_group_id! (RpropertyPillUseGroupIdT
OneProperty4
property_id (R
propertyId#
property_icon (RpropertyIconN

TotalLevel
total_level (R
totalLevel
addon_group (R
addonGroupe
MultiplyInfo%
multiply_value (RmultiplyValue.
multiply_count_down (RmultiplyCountDown�
Level1J

properties (2$.PB.MagicWeapon.Level1.PropertyLevelB��R
propertiesh
PropertyLevel%
property_value (RpropertyValue0
upgrade_required_exp (RupgradeRequiredExpG
	ResetInfo
	cost_item (RcostItem

item_count (R	itemCount: �����	��Temp��法器��0"�
MagicWeaponPowerUp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments>
colors (2 .PB.MagicWeaponPowerUp.ColorInfoB��Rcolors�
	ColorInfo
icon_id (RiconId
name (	Rname
color (	Rcolor
ui_pos (RuiPos
blessing_id (R
blessingId
property_id (R
propertyId-
property_values (B��RpropertyValues: �����	��Temp��法器��0"�
BlessingValue
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_level (RmaxLevel6
levels (2.PB.BlessingValue.Level2B��Rlevels�
Level2&
upgrade_item_id (RupgradeItemId,
upgrade_item_count (RupgradeItemCount!
upgrade_rate (RupgradeRate
	add_value (RaddValue
limit_value (R
limitValue: �������Temp��好友��0"�

ObjectCluster
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
id_ctrl (RidCtrl$
max_live_count (RmaxLiveCount"
max_gen_count (RmaxGenCount$
delay_time_gen (RdelayTimeGen8
min_count_when_not_enough	 (RminCountWhenNotEnough$
delay_time_add
 (RdelayTimeAdd"
per_add_count (RperAddCount)
dead_refreshtime (RdeadRefreshtime&
is_gen_counting (RisGenCounting$
gen_count_mark (RgenCountMark?
id_ctrl_exceed_gen_count_mark (RidCtrlExceedGenCountMark(
is_dead_counting (RisDeadCounting&
dead_count_mark (RdeadCountMarkA
id_ctrl_exceed_dead_count_mark (RidCtrlExceedDeadCountMarkN
%is_trigger_event_exceed_max_gen_count (RisTriggerEventExceedMaxGenCountE
 id_openctrl_exceed_max_gen_count (RidOpenctrlExceedMaxGenCountG
!id_closectrl_exceed_max_gen_count (RidClosectrlExceedMaxGenCounte
2is_trigger_event_exceed_max_gen_count_and_all_dead (R)isTriggerEventExceedMaxGenCountAndAllDead\
-id_openctrl_exceed_max_gen_count_and_all_dead (R%idOpenctrlExceedMaxGenCountAndAllDead^
.id_closectrl_exceed_max_gen_count_and_all_dead (R&idClosectrlExceedMaxGenCountAndAllDeadc
1close_self_ctrl_exceed_max_gen_count_and_all_dead (R(closeSelfCtrlExceedMaxGenCountAndAllDead:�������Npc��1"�
CommonUseLimit
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments-
type (2.PB.EXP_CMN_USELIMIT_TYPERtype
	max_count (RmaxCount>
max_count_modifier_vipseq_id (RmaxCountModifierVipseqId!
error_prompt (	RerrorPrompt:�������Config��1"�	
	Transform
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname=
transform_type (2.PB.EXP_TRANSFORM_TYPERtransformType=
transform_cooldown_time_sec (RtransformCooldownTimeSec&
has_scene_limit (RhasSceneLimit/
limit_scene_id_array	 (RlimitSceneIdArray

hang_point
 (	R	hangPoint&
hang_point_anim (	RhangPointAnim
	icon_path (RiconPath

model_path (R	modelPath,
forbid_self_skills (RforbidSelfSkills4
faction_mask (2.PB.EXP_CAMP_MASKRfactionMaskK
behaviour_limitmask1 (2.PB.BEHAVLIMIT_MAIN_MASKRbehaviourLimitmask1J
behaviour_limitmask2 (2.PB.BEHAVLIMIT_ALL_MASKRbehaviourLimitmask27
relieve_mask (2.PB.EXP_RELIEVE_MASKRrelieveMask
addon_id (RaddonId&
special_prop_id (RspecialPropId#
disable_equip (RdisableEquip,
disable_state_pack (RdisableStatePack7
disable_other_addon_prop (RdisableOtherAddonProp,
id_skill_manual (B��RidSkillManual.
id_skill_passive (B��RidSkillPassive

is_play_fx (RisPlayFx4
use_default_skillpanel (RuseDefaultSkillpanel:�������Config��1"�
DropTableEssence
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
id_type (2.PB.LOOT_TYPE_ENUMRidType;
drops (2.PB.DropTableEssence.item_tab2B���Rdrops�
	item_tab2
id_item (RidItem
odds (Rodds$
mf_affect_init (RmfAffectInit$
mf_affect_rate (RmfAffectRate:�������Abandon"�=
MonsterEssence
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
title (	Rtitle!
default_anim (	RdefaultAnim+
faction (2.PB.EXP_CAMP_MASKRfaction'
monster_faction	 (RmonsterFaction8
monster_type
 (2.PB.MONSTER_PROP_TYPERmonsterType&
prior_hurt_anim (RpriorHurtAnim<
monster_dis_prof (2.PB.PROF_TYPE_ENUMRmonsterDisProf5
monster_prof (2.PB.PROF_TYPE_ENUMRmonsterProf/
id_monster_base_prop (RidMonsterBaseProp/
trigger_qte_on_death (RtriggerQteOnDeath)
speak_id_on_birth (RspeakIdOnBirth)
speak_id_on_death (RspeakIdOnDeath0
speak_id_on_team_kill (RspeakIdOnTeamKill-
random_speak_cd_min (RrandomSpeakCdMin-
random_speak_cd_max (RrandomSpeakCdMax2
random_speak_count (B��RrandomSpeakCountJ
random_speak (2!.PB.MonsterEssence.random_speak_tB��RrandomSpeak
	death_sfx (	RdeathSfx"
hit_effect_id (RhitEffectId!
hit_hookname (	RhitHookname

file_model (R	fileModel#
texture_model (RtextureModel#
weapon_model1 (RweaponModel1#
weapon_model2 (RweaponModel2#
weapon_model3 (RweaponModel3'
weapon_texture1 (RweaponTexture1'
weapon_texture2  (RweaponTexture2'
weapon_texture3! (RweaponTexture3

cape_model" (R	capeModel#
file_portrait# (RfilePortrait
size$ (Rsize<
id_strategy% (2.PB.MONSTER_ATTACK_STRATEGYR
idStrategy&
id_monster_prop& (RidMonsterProp*
odds_monster_prop' (RoddsMonsterProp
level( (Rlevel

show_level) (R	showLevel

star_level* (R	starLevel.
quality+ (2.PB.EXP_QUALITY_ENUMRquality;
monster_usage, (2.PB.MONSTER_USAGE_TYPERmonsterUsage#
cannot_attack- (RcannotAttack2
is_lifeness. (2.PB.LIFELESS_TYPER
isLifeness#
bodysize_type/ (RbodysizeType#
property_mask0 (RpropertyMask
sight_range1 (R
sightRange
sight_angle2 (R
sightAngleC
aggressive_mode3 (2.PB.MONSTER_AGGRESIVE_MODERaggressiveModeJ
monster_faction_ask_help4 (2.PB.EXP_CAMP_MASKRmonsterFactionAskHelpJ
monster_faction_can_help5 (2.PB.EXP_CAMP_MASKRmonsterFactionCanHelp5

aggro_type6 (2.PB.MONSTER_AGGRO_TYPER	aggroType
die_task_id7 (R	dieTaskId!
patroll_mode8 (RpatrollMode5

stand_mode9 (2.PB.EXP_OBJ_STAND_MODER	standMode
hp: (Rhp&
mp_count; (2.PB.MP_TYPERmpCount
mp< (B��Rmp(
external_dmg_min= (RexternalDmgMin(
external_dmg_max> (RexternalDmgMax)
external_defense? (RexternalDefense,
external_crit_rate@ (RexternalCritRate9
external_crit_addon_ratioA (RexternalCritAddonRatio(
internal_dmg_minB (RinternalDmgMin(
internal_dmg_maxC (RinternalDmgMax)
internal_defenseD (RinternalDefense,
internal_crit_rateE (RinternalCritRate9
internal_crit_addon_ratioF (RinternalCritAddonRatio

attrib_dmgG (R	attribDmg&
attrib_dmg_antiH (RattribDmgAnti*
attrib_state_antiI (RattribStateAnti

walk_speedJ (R	walkSpeed
	run_speedK (RrunSpeed!
wander_speedL (RwanderSpeed'
common_strategyM (RcommonStrategy
score_pointN (R
scorePoint
expO (Rexp
loot_expP (RlootExp$
loot_min_moneyQ (RlootMinMoney$
loot_max_moneyR (RlootMaxMoney&
loot_animal_expS (RlootAnimalExp!
attack_speedT (RattackSpeed"
id_drop_tableU (RidDropTable(
drop_table_timesV (RdropTableTimes 
not_npc_lootW (R
notNpcLoot"
id_pass_skillX (RidPassSkill(
pass_skill_levelY (RpassSkillLevelN
not_attack_playerZ (2".PB.MONSTER_ATTACK_PLAYER_STRATEGYRnotAttackPlayer#
refresh_speed[ (RrefreshSpeed(
disapear_time_ms\ (RdisapearTimeMs(
random_walk_dist] (RrandomWalkDist0
random_walk_interval^ (RrandomWalkInterval

hp_bar_num_ (RhpBarNum%
positive_value` (RpositiveValue$
wander_time_msa (RwanderTimeMs(
wander_range_minb (RwanderRangeMin(
wander_range_maxc (RwanderRangeMaxG
type_for_activitiond (2.PB.MON_ACTIVITION_TYPERtypeForActivition7
id_acti_expfactor_confige (RidActiExpfactorConfig)
id_acti_exp_ratiof (RidActiExpRatio0
specdmg_reduce_paramg (RspecdmgReduceParam6
odds_die_playact_not_phyh (RoddsDiePlayactNotPhyY
monsterskillseqj (2).PB.MonsterEssence.monster_skill_sequenceB��Rmonsterskillseq*
born_protect_timek (RbornProtectTime"
can_be_pushedl (RcanBePushed0
reward_repu_countm (B��RrewardRepuCountH
reward_repun (2!.PB.MonsterEssence.monster_repu_tB��R
rewardRepu,
attack_nearest_oneo (RattackNearestOne(
aggro_inc_persecp (RaggroIncPersec1
min_aggro_inc_defaultq (RminAggroIncDefault1
max_aggro_inc_defaultr (RmaxAggroIncDefault.
fight_wander_chances (RfightWanderChance.
fight_wander_periodt (RfightWanderPeriodE
fight_wander_period_fluctuationu (RfightWanderPeriodFluctuation0
follow_attack_chancev (RfollowAttackChance,
chase_max_distancew (RchaseMaxDistance,
attacked_affect_hpx (RattackedAffectHp
	never_diey (RneverDie
count_deathz (R
countDeath

model_size{ (R	modelSize:
can_be_attacked_by_chariot| (RcanBeAttackedByChariot9
can_be_seen_at_further_pos} (RcanBeSeenAtFurtherPos,
related_skill_item~ (RrelatedSkillItem6
trigger_buff_id_on_death (RtriggerBuffIdOnDeath)
use_player_model� (RusePlayerModel5

model_info� (2.PB.player_model_infoR	modelInfo)
show_appear_anim� (RshowAppearAnim-
ak_type� (2.PB.EXP_ATTACK_TYPERakType.
df_type� (2.PB.EXP_DEFENCE_TYPERdfTypeL
specdmg_reduce_type� (2.PB.MON_SPECDMG_REDUCE_TYPERspecdmgReduceType<
monster_param� (2.PB.MONSTER_PARAM_MASKRmonsterParam8
repus� (2.PB.MonsterEssence.RepuTypeB��RrepusN
temp_unreachable_type� (2.PB.TEMP_UNREACHABLE_TYPERtempUnreachableType5
temp_unreachable_range� (RtempUnreachableRangeH
battle_cruise_type� (2.PB.AI_BATTLE_CRUISE_TYPERbattleCruiseType+
battle_cruise_dis� (RbattleCruiseDis4
battle_cruise_min_time� (RbattleCruiseMinTime4
battle_cruise_max_time� (RbattleCruiseMaxTime.
fight_last_min_time� (RfightLastMinTime.
fight_last_max_time� (RfightLastMaxTimeH
hurt_regions� (2.PB.MonsterEssence.region_infoB��RhurtRegions 
carrier_tid� (R
carrierTid1
is_carrier_component� (RisCarrierComponent!
pos� (2.PB.CoordinateRpos-
	direction� (2.PB.CoordinateR	direction
hp_ratio� (RhpRatio"
attack_ratio� (RattackRatio"
defend_ratio� (RdefendRatio"
damage_ratio� (RdamageRatio

turn_speed� (R	turnSpeed,
capsule� (2.PB.CapsuleForUE4Rcapsule
npc_drop_id� (R	npcDropId?
death_simu_physics_bone_name� (	RdeathSimuPhysicsBoneName

mass_scale� (R	massScale 
die_ragdoll� (R
dieRagdollC
blood_typ_info� (2.PB.MonsterEssence.BLOOD_TYPRbloodTypInfo"
blood_length� (RbloodLength+
extra_blood_ratio� (RextraBloodRatio-
extra_damage_ratio� (RextraDamageRatio

break_flag� (R	breakFlag-
file_portrait_dead� (RfilePortraitDead'
sound_on_attack� (	RsoundOnAttackS
random_speak_t
id (Rid
weight (Rweight
is_fight (RisFight�
monster_skill_sequence
skillseq_id (R
skillseqId&
is_common_style (RisCommonStyle
is_auto (RisAuto
weight (RweightL
monster_repu_t
id (Rid
value (Rvalue
param (Rparam�
RepuType
id (Rid
value (Rvalue=
effect_type (2.PB.EXP_MON_REPU_EFFECT_TYPER
effectType
param (RparamM
region_info
	region_id (RregionId!
damage_ratio (RdamageRatio"K
	BLOOD_TYP
blood_typ_first 
blood_typ_second
blood_typ_third:������Npc��2"�

LuckyPlate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	use_limit (RuseLimit.
	cost_type (2.PB.EXP_COST_MASKRcostType 
cost_item_id (R
costItemId 
cost_repu_id (R
costRepuId*
cost_diamond_once	 (RcostDiamondOnce*
fund_diamond_once
 (RfundDiamondOnce(
cost_diamond_ten (RcostDiamondTen(
fund_diamond_ten (RfundDiamondTen0
cost_diamond_hundred (RcostDiamondHundred0
fund_diamond_hundred (RfundDiamondHundred

reward_tid (R	rewardTid
reward2_tid (R
reward2Tid9
rewards (2.PB.LuckyPlate.OneReward4B��Rrewards�

OneReward4
icon_id (RiconId3
type (2.PB.EXP_LUCKY_PLATE_REWARD_TYPERtype
param (Rparam
weights (Rweights
call_tid (RcallTid
	use_limit (RuseLimit#
should_record (RshouldRecord:�������Config��0"�
NewPlate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments'
random_duration (RrandomDuration
	use_limit (RuseLimit 
cost_item_id (R
costItemId&
cost_item_count (RcostItemCount#
cost_diamonds	 (RcostDiamonds

cost_money
 (R	costMoney,
is_repeated_random (RisRepeatedRandomB
prior_rewards (2.PB.NewPlate.OneReward1B��RpriorRewardsI
not_prior_rewards (2.PB.NewPlate.OneReward2B��RnotPriorRewardsw

OneReward1
item_id (RitemId
weights (Rweights
speak_id (RspeakId
	use_limit (RuseLimitw

OneReward2
item_id (RitemId
weights (Rweights
speak_id (RspeakId
	use_limit (RuseLimit:�������Config��0"�
NewPlateActivity
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsD

activities (2.PB.NewPlateActivity.Activity2B��R
activitiesO
	Activity2
activity_id (R
activityId!
	plate_ids (B��RplateIds:�������Config��0"�
LimitTimePurchase
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity (Ractivity2
default_open_duration (RdefaultOpenDuration

reward_tid (R	rewardTid1

money_type (2.PB.EXP_MONEY_TYPER	moneyType
money_count	 (R
moneyCount*
below_ride_levels
 (RbelowRideLevels.
below_talent_levels (RbelowTalentLevels0
recharge_below_count (RrechargeBelowCount.
above_player_levels (RabovePlayerLevels3
above_server_open_time (RaboveServerOpenTime

trigger_id (R	triggerId
	mutual_id (RmutualId:�������Config��0"�
ItemTest
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments 
description (	Rdescription
value (Rvalue%
prof (2.PB.PROFTYPE_MASKRprof
ints	 (B��Rints
floats (B��Rfloats
strings (	B��Rstrings1
messages (2.PB.ItemTest.ABB��Rmessages(
AB
aaa (Raaa
bbb (Rbbb:��d����Abandon"�
Triple
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
two (2.PB.Triple.TwoB��Rtwo�
Two*
one (2.PB.Triple.Two.OneB��Rone�
One
	one_first (RoneFirst

one_second (R	oneSecond1
zero (2.PB.Triple.Two.One.ZeroB��RzeroF
Zero

zero_first (R	zeroFirst
zero_second (R
zeroSecond:���N����Abandon"�
CardGame
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
turns_use_limit (RturnsUseLimit&
reset_use_limit (RresetUseLimit(
reward_use_limit (RrewardUseLimit6
	cost_type (2.PB.EXP_COMMON_ITEM_TYPE1RcostType
cost_id	 (RcostId

cost_count
 (R	costCount&
times_use_limit (RtimesUseLimit&
grid_content_id (RgridContentId"
grid_fill1_id (RgridFill1Id"
grid_fill2_id (RgridFill2Id%
reward_tids (B��R
rewardTids
activity_id (R
activityId%
required_level (RrequiredLevel9
	exchanges (2.PB.CardGame.ExchangeB��R	exchangesa
Exchange
repu_id (RrepuId

repu_count (R	repuCount

reward_tid (R	rewardTid:�������Config��0"�
AllServerWelfare
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId#
activity_type (RactivityType,
exp_addup_percents (RexpAddupPercents!
level_limits (RlevelLimits:�������Config��0"�
BattleMethodPage
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId6
random_count_weights (B��RrandomCountWeights!
	method_id	 (B��RmethodId
	cost_repu
 (RcostRepu

cost_count (R	costCount
base_exp (RbaseExp+
critical1_weights (Rcritical1Weights+
critical2_weights (Rcritical2Weights+
critical3_weights (Rcritical3Weights+
critical5_weights (Rcritical5Weights-
critical10_weights (Rcritical10Weights;
rewards (2.PB.BattleMethodPage.RewardB��
Rrewards=
Reward
level (Rlevel

reward_tid (R	rewardTid:�������Abandon"�
BattleMethod
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
icon_id (RiconId
desc (	Rdesc
	max_level (RmaxLevel!
property_id1 (RpropertyId1'
property_value1	 (RpropertyValue1!
property_id2
 (RpropertyId2'
property_value2 (RpropertyValue25
levels (2.PB.BattleMethod.Level3B��ZRlevelsE
Level3
require_exp (R
requireExp
percents (Rpercents:�������Abandon"�
MonthCheckin
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	use_limit (RuseLimit#
cost_diamonds (RcostDiamonds
	max_level (RmaxLevel
activity_id (R
activityId5
levels (2.PB.MonthCheckin.Level4B��Rlevels;
Level4
days (Rdays

reward_tid (R	rewardTid:�������Config��0"�

WingUpgrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
wing_max_level (RwingMaxLevel
gain_exp (RgainExp3
crit_x2_odd_percentage (RcritX2OddPercentage3
crit_x3_odd_percentage (RcritX3OddPercentage3
crit_x5_odd_percentage	 (RcritX5OddPercentage5
crit_x10_odd_percentage
 (RcritX10OddPercentage:
property_pill_use_group_id (RpropertyPillUseGroupId4
grades (2.PB.WingUpgrade.GradeB���Rgrades�
Grade.
quality (2.PB.EXP_QUALITY_ENUMRquality
name (	Rname(
upgrade_cost_exp (RupgradeCostExp+
train_cost_item_id (RtrainCostItemId-
train_cost_item_num (RtrainCostItemNum-
hp_max_training_num (RhpMaxTrainingNumB
physic_attack_max_training_num (RphysicAttackMaxTrainingNum@
magic_attack_max_training_num (RmagicAttackMaxTrainingNumD
physic_defence_max_training_num	 (RphysicDefenceMaxTrainingNumB
magic_defence_max_training_num
 (RmagicDefenceMaxTrainingNum5
award_fixview_config_id (RawardFixviewConfigId

reward_tid (R	rewardTid
inc_hp (RincHp*
inc_physic_attack (RincPhysicAttack(
inc_magic_attack (RincMagicAttack,
inc_physic_defence (RincPhysicDefence*
inc_magic_defence (RincMagicDefence(
ride_sprint_segs (RrideSprintSegs: �����	��Temp��翅膀��0"�
PropertyPillUseGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsC
use_cfgs	 (2".PB.PropertyPillUseGroup.UseConfigB��
RuseCfgsY
	UseConfig
level (Rlevel
pill_tid (RpillTid
	max_count (RmaxCount:�������Config��1"�
Pet
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
icon_id (RiconId
model_id (RmodelId
quality (Rquality*
quality_ratio_min	 (RqualityRatioMin*
quality_ratio_max
 (RqualityRatioMax#
base_property (RbaseProperty)
special_property (RspecialProperty*
total_eaten_limit (RtotalEatenLimit4
skill_count_weights (B��RskillCountWeights"
skill_page_id (RskillPageId
feed_id (RfeedId
xisui_id (RxisuiId!
decompose_id (RdecomposeId
demonize_id (R
demonizeId

compose_id (R	composeId 
description (	Rdescription:
property_pill_use_group_id (RpropertyPillUseGroupIdL
special_quality (2#.PB.EXP_ANIMAL_SPECIAL_QUALITY_ENUMRspecialQuality4
mutation_groupup_value (RmutationGroupupValue*
mutation_xisui_id (RmutationXisuiId,
default_plus_value (RdefaultPlusValue
monster_tid (R
monsterTid*
mutation_model_id (RmutationModelId,
mutation_moster_id (RmutationMosterId
skill_id (RskillId
attack_type  (R
attackType,
mutation_fake_rate! (RmutationFakeRate&
high_slot_count" (RhighSlotCount$
low_slot_count# (RlowSlotCount,
medicine_max_level$ (RmedicineMaxLevel7
multi_skill_xilian_times% (RmultiSkillXilianTimes*
multi_skill_count& (RmultiSkillCount'
base_prop_cfg_id' (RbasePropCfgId

init_level( (R	initLevel:��D��	��Temp��宠物��1"�
	LootTable
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
noneuse (Rnoneuse+
id_type (2.PB.LOOT_TYPE_ENUMRidType&
loot_table_type (RlootTableType
name (	Rname#

file_model
 (B��R	fileModel$
time_from_loot (RtimeFromLoot$
time_from_drop (RtimeFromDrop
	auto_pick (RautoPick'
special_odds (B��RspecialOdds%
normal_odds (B��R
normalOdds;

solo_items (2.PB.LootTable.item_tabB��R	soloItemsA
special_items (2.PB.LootTable.item_tabB��RspecialItems@
normal_items (2.PB.LootTable.item_tabB���RnormalItems�
item_tab
id_item (RidItem"
id_gen_config (RidGenConfig
odds (Rodds
	min_count (RminCount
	max_count (RmaxCount:�������Npc��1"�
NPCTalkService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	id_dialog (RidDialog:��c����Npc��服务��0"�
NPCSellService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname7
npcsell_mode (2.PB.EXP_NPCSELL_MODERnpcsellMode
	id_dialog (RidDialog3
pages	 (2.PB.NPCSellService.PAGEB��Rpages
id_discount
 (R
idDiscount�
GOOD
id_item (RidItem
	use_limit (RuseLimit(
require_title_id (RrequireTitleId$
id_genitem_cfg (RidGenitemCfg�
PAGE

page_title (	R	pageTitle6
	cost_mode (2.PB.EXP_NPCSELL_COST_TYPERcostMode3
goods (2.PB.NPCSellService.GOODB��0Rgoods:��b����Npc��服务��0"�
NPCBuyService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	id_dialog (RidDialog4
quality_second_confirm (RqualitySecondConfirm5
items	 (2.PB.NPCBuyService.bitem_tB��Ritems
	icon_head
 (RiconHead`
prize_t/
type (2.PB.EXP_NPCBUYSEV_PRIZETYPERtype
id (Rid
count (Rcount�
bitem_t
id (Rid
count (Rcount
pop_h (RpopH
	count_max (RcountMax7
prizes (2.PB.NPCBuyService.prize_tB��Rprizes:��a����Npc��服务��0"�
ReputationSellService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
repu_id (RrepuId
repu_num (RrepuNum 
cost_repu_id (R
costRepuId"
cost_repu_num	 (RcostRepuNum

cost_money
 (R	costMoney7
npcsell_mode (2.PB.EXP_NPCSELL_MODERnpcsellMode:��`����Npc��服务��0"�
NPCTaskOutService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
id_task_set (R	idTaskSet
	id_matter (RidMatter*
id_refresh_matter (RidRefreshMatter 
id_tasks	 (B���RidTasks:��^����Npc��服务��0"�
NPCTaskInService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname 
id_tasks	 (B���RidTasks:��_����Npc��服务��0"�
NPCTaskMatterService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname;
tasks (2.PB.NPCTaskMatterService.task_tB��RtasksF
matter_t
	id_matter (RidMatter

num_matter (R	numMatterm
task_t
id_task (RidTaskJ
taks_matters (2!.PB.NPCTaskMatterService.matter_tB��RtaksMatters:��]����Npc��服务��0"�
NPCTaskEventService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments<
tasks (2.PB.NPCTaskEventService.task_t1B���RtasksR
task_t1
id (Rid
count (Rcount!
cooltime_sec (RcooltimeSec:��\����Npc��服务��0"�
PlayerDIYActivityService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname0
id_playerdiy_cfgs (B��RidPlayerdiyCfgs:��V����Npc��服务��0"�
NPCTransmitService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	id_dialog (RidDialog?
targets (2.PB.NPCTransmitService.target_tB��Rtargets�
target_t
gs_id (RgsIdD
trans_condition (2.PB.TRANSMIT_CONDITION_ENUMRtransCondition
id_instance (R
idInstance
id_scene (RidScene
name (	Rname
x (Rx
y (Ry
z (Rz
fee	 (Rfee%
required_level
 (RrequiredLevel
radius (Rradius:��[����Npc��服务��0"�
NPCEquipBindService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname$
id_object_need (RidObjectNeed
price (Rprice:��Z����Npc��服务��0"�
NPCSpecailSellService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname=
goods (2!.PB.NPCSpecailSellService.goods_tB�� Rgoods�
goods_t
id (Rid
num (Rnum
	repu_type (RrepuType
	repu_cost (RrepuCost
bind (Rbind
expire_time (R
expireTime:��Y����Npc��服务��0"�
NPCInstanceService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname=
config (2.PB.NPCInstanceService.config_tB��Rconfig�
config_t
inst_id (RinstId,
show_when_unactive (RshowWhenUnactive.
show_when_unsatisfy (RshowWhenUnsatisfy"
npc_use_count (RnpcUseCount!
activity_tid (RactivityTid:��X����Npc��服务��0"�
NPCSceneParamsService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname#

open_ctrls (B��R	openCtrls%
close_ctrls	 (B��R
closeCtrls
id_item_
 (RidItem:��W����Npc��服务��0"�
OneTask
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments*
gentask_count_min (RgentaskCountMin*
gentask_count_max (RgentaskCountMax1
max_take_count_perday (RmaxTakeCountPerday3
max_take_count_perweek (RmaxTakeCountPerweek$
max_take_count	 (RmaxTakeCount:
auto_reset_need_task_count
 (RautoResetNeedTaskCount4
max_auto_resets_per_day (RmaxAutoResetsPerDay:
refresh_free_count_per_day (RrefreshFreeCountPerDay:
refresh_money_cost_per_day (RrefreshMoneyCostPerDay>
refresh_yuanbao_cost_per_day (RrefreshYuanbaoCostPerDay1
refresh_needed_min_lv (RrefreshNeededMinLv1
refresh_needed_max_lv (RrefreshNeededMaxLv:�������Abandon"�
TaskLib
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	RnameE
clear_everyday_taketimes_minute (RclearEverydayTaketimesMinuteC
clear_everyweek_taketimes_week (RclearEveryweekTaketimesWeekC
clear_everyweek_taketimes_hour (RclearEveryweekTaketimesHourG
 clear_everyweek_taketimes_minute	 (RclearEveryweekTaketimesMinute2
refresh_everyday_hour
 (RrefreshEverydayHour6
refresh_everyday_minute (RrefreshEverydayMinuteA
clear_everyday_taketimes_hour (RclearEverydayTaketimesHour
each_lib (B��@ReachLib:�������Config��1"�
ParamAdjust
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments7
num_killmonster_half_exp (RnumKillmonsterHalfExp7
num_killmonster_zero_exp (RnumKillmonsterZeroExpQ
level_diff_adjust (2.PB.ParamAdjust.LevelDiffAdjustB��RlevelDiffAdjust[
pet_level_diff_adjust
 (2".PB.ParamAdjust.PetLevelDiffAdjustB��RpetLevelDiffAdjustD
team_adjust (2.PB.ParamAdjust.GeneralAdjustB��R
teamAdjustY
team_profession_adjust (2.PB.ParamAdjust.GeneralAdjustB��	RteamProfessionAdjustD
prof2_adjust (2.PB.ParamAdjust.Prof2AdjustB�� Rprof2Adjust.
pet_level_adjust (B��RpetLevelAdjustK
level_diff_task (2.PB.ParamAdjust.LevelDiffTaskB��RlevelDiffTask4
energy (2.PB.ParamAdjust.EnergyB��RenergyX
duke_adjust_by_level (2!.PB.ParamAdjust.DukeAdjustByLevelB��RdukeAdjustByLevelE
 duke_adjust_assist_in_nation_war (RdukeAdjustAssistInNationWar,
duke_adjust_assist (RdukeAdjustAssist0
double_duke_activity (RdoubleDukeActivity,
double_duke_modify (RdoubleDukeModify�
LevelDiffAdjust

level_diff (R	levelDiff

adjust_exp (R	adjustExp!
adjust_money (RadjustMoney#
adjust_matter (RadjustMatter#
adjust_attack (RadjustAttackR
PetLevelDiffAdjust

level_diff (R	levelDiff

adjust_exp (R	adjustExp.
GeneralAdjust

adjust_exp (R	adjustExp,
Prof2Adjust

adjust_exp (R	adjustExpM
LevelDiffTask

level_diff (R	levelDiff

adjust_exp (R	adjustExpp
Energy=
diff_player_and_inst_min_lvl (RdiffPlayerAndInstMinLvl'
cost_correction (RcostCorrectionJ
DukeAdjustByLevel

level_diff (R	levelDiff
adjust (Radjust:�������Config��0"�
FactionRelation
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
type (Rtype-
relations_count (B�� RrelationsCountI
	relations (2%.PB.FactionRelation.relations_messageB�� R	relationsM
relations_message8
relations_enum (2.PB.EXP_CAMP_MASKRrelationsEnum:�������Config��0"�	
CommonReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
remarks (	Rremarks
icon (RiconA
grant_reward_type (2.PB.GRANT_REWARD_TYPERgrantRewardType
	basic_exp	 (RbasicExp"
exp_factor_id
 (RexpFactorId$
basic_prof_exp (RbasicProfExp*
basic_bound_money (RbasicBoundMoney*
basic_trade_money (RbasicTradeMoney.
basic_bound_yuanbao (RbasicBoundYuanbao

animal_exp (R	animalExp/
animal_exp_factor_id (RanimalExpFactorId(
animal_exp_ratio (RanimalExpRatio
corps_money (R
corpsMoney%
corps_dividend (RcorpsDividend6
achievement_event_count (RachievementEventCount"
is_count_bind (RisCountBind

auto_award (R	autoAward,
tlog_activity_type (RtlogActivityType%
achievement_id (RachievementId>
basic_reput (2.PB.CommonReward.repu_tB��R
basicReput4
items (2.PB.CommonReward.ritem_tB��Ritems5
basic_exp_adjust_config (RbasicExpAdjustConfig@
repu_t
repu_id (RrepuId

repu_value (R	repuValueQ
ritem_t
id (Rid
num (Rnum$
genitem_cfg_id (RgenitemCfgId:�������Config��2"�
	Subobject
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname7
moving_type (2.PB.SUBOBJ_MOVING_TYPER
movingType;
target_type (2.PB.SUBOBJ_MOVETARGET_TYPER
targetType

id_perform (R	idPerformO
judge_effectmode_type	 (2.PB.SUBOBJ_JUDGEEFFECT_TYPERjudgeEffectmodeTypeJ
judge_collide_mask
 (2.PB.SUBOBJ_JUDGECOLLIDE_MASKRjudgeCollideMask&
judge_init_time (RjudgeInitTime&
judge_loop_time (RjudgeLoopTime*
judge_times_limit (RjudgeTimesLimit
movable (Rmovable4

limit_type (2.PB.SUBOBJ_LIMIT_TYPER	limitType
limit_group (R
limitGroup
limit_count (R
limitCount
living_time (R
livingTime
gfx_path (RgfxPath,
gfx_path_disappear (RgfxPathDisappear

model_path (R	modelPath
size (Rsize%
imitate_weapon (RimitateWeapon!
is_exclusive (RisExclusive!
imitate_type (RimitateType
rebound_max (R
reboundMax

init_speed (R	initSpeed0
stop_pursue_distance (RstopPursueDistance
	boomerang (R	boomerang
height (Rheight-
boomerang_distance (RboomerangDistance-
disappear_distance  (RdisappearDistance1
gravity_acceleration! (RgravityAcceleration

init_angle" (R	initAngle%
horizon_offset# (RhorizonOffset,
entire_scene_sight$ (RentireSceneSight,
invisible_to_owner% (RinvisibleToOwner/
skill_on_life_ending& (RskillOnLifeEnding+
execute_frequency' (RexecuteFrequency: �������SkillSubobject��2"�
BuffBox
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

model_path (R	modelPath
cursor_type (R
cursorType+
faction (2.PB.EXP_CAMP_MASKRfaction"
max_life_time	 (RmaxLifeTime 
max_use_dist
 (R
maxUseDist"
max_use_count (RmaxUseCount&
min_limit_level (RminLimitLevel&
max_limit_level (RmaxLimitLevel9
prof_limit_mask (2.PB.PROFTYPE_MASKRprofLimitMask'
can_use_in_fight (RcanUseInFight

skill_type (R	skillType1
skills (2.PB.BuffBox.skill_tB��Rskills
name (	RnameC
skill_t
id (Rid
level (Rlevel
odds (Rodds:������Npc��1"�
TransmitBox
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname!
display_name (	RdisplayNameD
trans_condition (2.PB.TRANSMIT_CONDITION_ENUMRtransCondition(
shape (2.PB.EXP_SHAPE_ENUMRshape
radius	 (Rradius!
target_scene
 (RtargetScene
id_instance (R
idInstance
target_name (	R
targetName<
need_confirm (2.PB.TRANSMIT_CONFIRM_TYPERneedConfirm 
target_pos_x (R
targetPosX 
target_pos_y (R
targetPosY 
target_pos_z (R
targetPosZ+
faction (2.PB.EXP_CAMP_MASKRfaction
	level_req (RlevelReq

model_path (R	modelPath:������Npc��1"�
Rune
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname/
unlock_need_skill_id (RunlockNeedSkillId>
unlock_need_base_skill_level (RunlockNeedBaseSkillLevel3
unlock_need_role_level (RunlockNeedRoleLevelE
upgrade_config
 (2.PB.Rune.UPGRADE_CONFIGB���RupgradeConfig�
UPGRADE_CONFIG

cost_money (R	costMoney 
cost_rune_po (R
costRunePo/
award_addon_group_id (RawardAddonGroupId:�������Abandon"�
SkillSeq
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	file_icon (RfileIcon%
type (2.PB.SKILLSEQ_TYPERtype9
prof_limit_mask (2.PB.PROFTYPE_MASKRprofLimitMask'
proflevel_limit	 (RproflevelLimit
	source_id
 (RsourceId
need_finish (R
needFinish!
	id_skills (B��RidSkills.
skill_cast_level (B��RskillCastLevel-
runes (2.PB.SkillSeq.RUNEB��Rrunes>
RUNE
skill_id (RskillId
	config_id (RconfigId:�������SkillSeq��2"�
Mail
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname%
type (2.PB.EXP_MAIL_TYPERtype
sender_name (	R
senderName
object (	Robject

id_content	 (R	idContent+
attach_trademoney
 (RattachTrademoney=
attach_items (2.PB.Mail.ATTACH_ITEMB��RattachItemsC
ATTACH_ITEM
id_item (RidItem
	id_gencfg (RidGencfg:�������Config��1"�
Mine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
id_type (RidType
title (	Rtitle

show_title (R	showTitle!
default_anim	 (	RdefaultAnim0
interact_finish_hide
 (RinteractFinishHide(
is_static_matter (RisStaticMatter
level (Rlevel
	req_level (RreqLevel
	req_skill (RreqSkill&
req_skill_level (RreqSkillLevel
req_tool_id (R	reqToolId%
eliminate_tool (ReliminateTool
time_min (RtimeMin
time_max (RtimeMax
	use_limit (RuseLimit%
achievement_id (RachievementId'
inst_difficulty (RinstDifficulty&
consume_repu_id (RconsumeRepuId,
consume_repu_value (RconsumeRepuValue9
gather_type (2.PB.EXP_MINE_GATHER_TYPER
gatherType3
only_leader_can_gather (RonlyLeaderCanGather(
max_gather_count (RmaxGatherCount#
is_continuous (RisContinuous
is_pot (RisPot,
consume_point_type (RconsumePointType7
consume_point_per_gather (RconsumePointPerGather
exp  (Rexp

skillpoint! (R
skillpoint,
lifeprof_extra_exp" (RlifeprofExtraExp

file_model# (R	fileModel 
tool_in_hand$ (R
toolInHand.
after_interact_anim% (	RafterInteractAnim.
after_interact_talk& (	RafterInteractTalk*
after_interact_cg' (RafterInteractCgG
after_interact_mask( (2.PB.AFTER_INTERACT_MASKRafterInteractMask*
file_minimap_icon) (RfileMinimapIcon(
file_occupy_icon* (RfileOccupyIcon+
file_can_gather_fx+ (RfileCanGatherFx/
has_quick_gather_btn, (RhasQuickGatherBtn&
gather_btn_icon- (RgatherBtnIcon"
id_loot_table. (RidLootTable+
is_team_share_loot/ (RisTeamShareLoot(
uninterruptable0 (Runinterruptable
task_in1 (	RtaskIn
	task_succ3 (RtaskSucc
	permenent4 (R	permenent+
faction5 (2.PB.EXP_CAMP_MASKRfaction
id_instance6 (R
idInstance-
is_show_process_bar7 (RisShowProcessBar&
process_bar_msg8 (	RprocessBarMsg
dutiao_type9 (R
dutiaoType$
dutiao_icon_id: (RdutiaoIconId,
player_gather_anim; (	RplayerGatherAnim+
is_show_npc_dialog< (RisShowNpcDialog
	hello_msg= (	RhelloMsg
open_msg> (	RopenMsg 
not_open_msg? (	R
notOpenMsg(
file_is_fighting@ (RfileIsFighting
aggro_rangeA (R
aggroRange
aggro_pointB (R
aggroPoint

monster_idC (R	monsterId
monster_numD (R
monsterNum%
monster_radiusE (RmonsterRadius!
monster_timeG (RmonsterTime#
monster_aggroH (RmonsterAggro#

open_ctrlsJ (B��R	openCtrls%
close_ctrlsL (B��R
closeCtrls$
id_begin_skillM (RidBeginSkill 
id_end_skillN (R
idEndSkill
cursor_typeO (R
cursorType;
mine_corpse_disappear_timeP (RmineCorpseDisappearTime@
after_miningR (2.PB.Mine.after_mining_tB��RafterMining$
buff_id_gatherS (RbuffIdGather?
character_combo_idT (2.PB.PROFTYPE_MASKRcharacterComboId4

model_infoU (2.PB.player_model_infoR	modelInfo4
mine_rltW (2.PB.Mine.mine_rlt_tB��RmineRlt9
interact_finish_hide_timeX (RinteractFinishHideTime�

mine_rlt_t+
faction (2.PB.EXP_CAMP_MASKRfaction#

open_ctrls (B��R	openCtrls%
close_ctrls (B��R
closeCtrlsp
after_mining_t
scene_param (R
sceneParam+
op (2.PB.AFFECT_SCENE_PARAM_TYPERop
val (Rval:������Npc��2"�(
NPC_ESSENCE
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
title (	Rtitle!
default_anim (	RdefaultAnim*
use_behavior_tree (RuseBehaviorTree
id_type	 (RidType4
npc_dis_prof
 (2.PB.PROF_TYPE_ENUMR
npcDisProf!
refresh_time (RrefreshTime
attack_rule (R
attackRule.
player_replace_unit (RplayerReplaceUnit

file_model (R	fileModel#
texture_model (RtextureModel%
lefthand_model (RlefthandModel'
righthand_model (RrighthandModel#
file_portrait (RfilePortrait
	file_icon (RfileIcon*
file_minimap_icon (RfileMinimapIcon(
can_quick_select (RcanQuickSelect$
id_src_monster (RidSrcMonster
	hello_msg (	RhelloMsg$
id_to_discover (RidToDiscover%
domain_related (RdomainRelated
	guard_npc (RguardNpc
carrier_npc (R
carrierNpc
id_instance (R
idInstance+
is_show_npc_dialog (RisShowNpcDialog
open_msg (	RopenMsg 
not_open_msg (	R
notOpenMsg$
can_multi_open  (RcanMultiOpen 
max_open_num! (R
maxOpenNum5
disapear_reach_max_open" (RdisapearReachMaxOpen(
file_is_fighting# (RfileIsFighting/
id_pre_required_task$ (RidPreRequiredTask&
id_talk_service% (RidTalkService,
id_sell_service' (B��RidSellService$
id_buy_service( (RidBuyService,
id_gshop_majortype) (RidGshopMajortype-
id_task_out_service* (	RidTaskOutService+
id_task_in_service+ (	RidTaskInService3
id_task_matter_service, (RidTaskMatterService.
id_transmit_service- (RidTransmitService(
id_proxy_service. (RidProxyService(
id_skill_service/ (RidSkillService.
id_skillseq_service0 (RidSkillseqService9
id_war_towerbuild_service1 (RidWarTowerbuildService0
id_resetprop_service2 (RidResetpropService*
id_recipe_service3 (RidRecipeService7
id_especial_sell_service4 (RidEspecialSellService.
id_instance_service5 (RidInstanceService1
id_task_event_service6 (RidTaskEventService7
id_join_lifeprof_service7 (RidJoinLifeprofServiceA
id_alliancechief_sell_service8 (RidAlliancechiefSellServiceA
id_playerdiy_activity_service9 (RidPlayerdiyActivityService7
id_dart_delivery_service: (RidDartDeliveryService7
id_store_hostage_service; (RidStoreHostageService2
id_gather_information< (RidGatherInformation(
id_title_service= (RidTitleService9
id_advanture_talk_service> (RidAdvantureTalkService

id_npc_woo? (RidNpcWoo/
id_npc_party_service@ (RidNpcPartyServiceS
$id_npc_produce_skill_upgrade_serviceB (B��RidNpcProduceSkillUpgradeService.
id_graffiti_serviceC (RidGraffitiService&
id_graffiti_boxD (RidGraffitiBoxH
!id_interserver_time_exchg_serviceE (RidInterserverTimeExchgService.
id_parading_serviceF (RidParadingService*
id_statue_serviceG (RidStatueServiceB
id_gatherfood_task_out_serviceH (RidGatherfoodTaskOutService;
id_reputation_sell_serviceI (RidReputationSellService9
id_start_minigame_serviceJ (RidStartMinigameService.
has_pkvalue_serviceL (RhasPkvalueService&
fee_per_pkvalueM (RfeePerPkvalue
id_mineN (RidMine'
id_cg_after_mineO (RidCgAfterMine
cursor_typeP (R
cursorType
dialog_typeQ (R
dialogType
	hide_nameR (RhideName
	show_typeS (RshowTypeA
show_type_minimapT (2.PB.EXP_NPC_SHOW_TYPERshowTypeMinimap
	auto_turnU (RautoTurn)
disable_interactV (RdisableInteract!
icon_minimapW (RiconMinimap,
icon_seled_minimapX (RiconSeledMinimap'
carrier_minsZ (B��RcarrierMins'
carrier_maxs\ (B��RcarrierMaxs*
mirror_npc_ids^ (B��RmirrorNpcIds5

stand_mode_ (2.PB.EXP_OBJ_STAND_MODER	standMode,
forbid_talk_camera` (RforbidTalkCamera&
id_default_rolea (RidDefaultRole1
show_famous_npc_panelb (RshowFamousNpcPanel(
id_portrait_pathc (RidPortraitPath3
begin_service_voice_idd (RbeginServiceVoiceId/
end_service_voice_ide (RendServiceVoiceId?
service_limitf (2.PB.NPC_SERVICE_LIMIT_MASKRserviceLimit
	statue_idg (RstatueId(
use_player_modelh (RusePlayerModel4

model_infoi (2.PB.player_model_infoR	modelInfo(
show_appear_animj (RshowAppearAnim

click_animk (	R	clickAnim9
id_npc_lottery_servicem (B�� RidNpcLotteryServiceA
combined_servicesn (2.PB.COMBINESEV1_MASKRcombinedServicesC
combined_services2o (2.PB.COMBINESEV2_MASKRcombinedServices2C
combined_services3p (2.PB.COMBINESEV3_MASKRcombinedServices3C
combined_services4q (2.PB.COMBINESEV4_MASKRcombinedServices4G
factionspec_typer (2.PB.EXP_NPC_FACTIONSPEC_TYPERfactionspecTypeY
type_faction_dyn_services (2 .PB.EXP_FACTION_DYN_SERVICE_TYPERtypeFactionDynService,
id_thrower_servicet (RidThrowerService,
entire_scene_sightu (RentireSceneSight3
id_arrow_tower_servicev (RidArrowTowerService
	id_matterw (RidMatter*
id_refresh_matterx (RidRefreshMatter
task_lib_idy (R	taskLibId
carrier_tidz (R
carrierTid:������Npc��2"�
Nation
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsE
officer_config (2.PB.Nation.OfficerConfigB��
RofficerConfig7
donation_cmn_uselimit_id (RdonationCmnUselimitIdJ
donation_options	 (2.PB.Nation.DonationConfigB��RdonationOptions8
gain_duke_when_kill_enemy
 (RgainDukeWhenKillEnemy:
cost_fund_to_form_alliance (RcostFundToFormAlliance,
weak_nation_reward (RweakNationReward4
powerful_nation_reward (RpowerfulNationRewardu
OfficerConfig<
daily_salary_grant_award_id (RdailySalaryGrantAwardId&
reward_title_id (RrewardTitleId�
DonationConfig4

money_type (2.PB.EXP_SHOPSELL_MODER	moneyType!
money_amount (RmoneyAmount 
gain_repu_id (R
gainRepuId&
gain_repu_count (RgainRepuCount5
gain_nation_fund_amount (RgainNationFundAmount:��o����Temp��国家��0"�
Card
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
	card_name (	RcardName%
camp (2.PB.EXP_CARD_CAMPRcamp'
gender	 (2.PB.GENDER_MASKRgender

brief_desc
 (	R	briefDesc
desc (	Rdesc
illust (Rillust
portrait (Rportrait"
how_to_obtain (	RhowToObtain1
decompose_get_repu_id (RdecomposeGetRepuId7
decompose_get_item_count (RdecomposeGetItemCount

sort_index (R	sortIndex
can_compose (R
canCompose(
compose_cost_rep (RcomposeCostRep3
compose_cost_rep_count (RcomposeCostRepCount:��"��	��Item��卡牌��1"�
	CardCombo
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname.
quality (2.PB.EXP_QUALITY_ENUMRquality
desc (	Rdesc
cards	 (B��Rcards/
bonus_addon_group_id
 (RbonusAddonGroupId*
bonus_repu_amount (RbonusRepuAmount: �����	��Temp��卡牌��1"�
CardComboGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
icon (Ricon

sort_index (R	sortIndex7
unlock_need_player_level (RunlockNeedPlayerLevel
combo_id
 (B��RcomboId
model_id (RmodelId

faction_id (R	factionId-
unlock_need_card_id (RunlockNeedCardId$
addon_group_id (RaddonGroupIdS
show_property_type (2%.PB.CardComboGroup.SHOW_PROPERTY_TYPERshowPropertyType
illust (Rillust"d
SHOW_PROPERTY_TYPE
UNKNOWN_SHOW_PROPERTY_TYPE 

ATTACK
PHY_DEF
	MAGIC_DEF
HP: �����	��Temp��卡牌��1"�
CardFaction
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname(
falg_sprite_name (	RfalgSpriteName: �����	��Temp��卡牌��0"�
EquipLianXing
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsA
configs (2!.PB.EquipLianXing.OneLevelConfig3B��0Rconfigs�
OneLevelConfig38
success_rate_denominator (RsuccessRateDenominator=
basic_prop_addition_percent (RbasicPropAdditionPercent"
cost_item_tid (RcostItemTid&
cost_item_count (RcostItemCount

cost_money (R	costMoney5
cost_guarantee_item_tid (RcostGuaranteeItemTid9
cost_guarantee_item_count (RcostGuaranteeItemCount1
new_level_when_failed (RnewLevelWhenFailed3
rate_delta_when_failed	 (RrateDeltaWhenFailed: �����	��Temp��装备��0"�
JueWei
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	icon_path (RiconPath 
description (	Rdescription

yinxi_name (	R	yinxiName*
yinxi_illust_path	 (RyinxiIllustPath.
quality
 (2.PB.EXP_QUALITY_ENUMRquality2
need_exp_to_next_level (RneedExpToNextLevel3
salary_base_bind_money (RsalaryBaseBindMoney&
salary_award_id (RsalaryAwardId.
skills (2.PB.JueWei.SkillB��Rskills/
award_addon_group_id (RawardAddonGroupId3
passive_skill_group_id (RpassiveSkillGroupId-
Skill
id (Rid
level (Rlevel:�����	��Temp��0"�
MiniGame
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
show_manual (R
showManual'
min_level_to_use (RminLevelToUse

game_index (R	gameIndex,
grant_reward_id
 (B��RgrantRewardId*
award_uselimit_id (RawardUselimitId:�������Config��0"�
Home
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments4
	home_info (2.PB.Home.HomeInfoB��RhomeInfo
clean_times (R
cleanTimes,
neat_point_consume	 (RneatPointConsume3
consume_ratio_table_id
 (RconsumeRatioTableId-
home_spirit_repu_id (RhomeSpiritRepuId;
home_daily_uselimit_config (RhomeDailyUselimitConfig;
home_clean_uselimit_config (RhomeCleanUselimitConfig9
home_rest_uselimit_config (RhomeRestUselimitConfig�
HomeInfo
img_icon (RimgIcon

max_spirit (R	maxSpirit
desc (	Rdesc!
reward_times (RrewardTimes$
reward_task_id (RrewardTaskId

rest_times (R	restTimes 
rest_task_id (R
restTaskId: �������Temp��家园��0"�
ProduceSkill
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

skill_name (	R	skillName

skill_desc (	R	skillDesc*
type (2.PB.PRODUCE_SKILL_TYPERtypeD
prof_mask_learn_skill (2.PB.PROFTYPE_MASKRprofMaskLearnSkill
	icon_path	 (RiconPath#
default_level
 (RdefaultLevelK
skill_level_ups (2.PB.ProduceSkill.SkillLevelUpB��xRskillLevelUps�
SkillLevelUp
level_limit (R
levelLimit

cost_money (R	costMoney 
cost_repu_id (R
costRepuId(
cost_repu_amount (RcostRepuAmount
addon_group (R
addonGroup:�������Abandon"�
Recipe
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
recipe_name (	R
recipeName
recipe_desc (	R
recipeDesc
	recipe_no (RrecipeNo
	icon_path (RiconPathD
prof_mask_learn_skill	 (2.PB.PROFTYPE_MASKRprofMaskLearnSkill5
learn_skill_level_limit
 (RlearnSkillLevelLimit(
produce_skill_id (RproduceSkillId9
produce_skill_level_limit (RproduceSkillLevelLimitD
material_items (2.PB.Recipe.MaterialItemB��RmaterialItems"
lucky_item_id (RluckyItemId$
lucky_item_num (RluckyItemNumB
normal_output (2.PB.Recipe.NormalOutputB��RnormalOutput?
lucky_output (2.PB.Recipe.LuckyOutputB��RluckyOutput

cost_money (R	costMoney 
cost_repu_id (R
costRepuId(
cost_repu_amount (RcostRepuAmount4
MaterialItem
id (Rid
count (RcountL
NormalOutput
id (Rid
count (Rcount
weight (RweightK
LuckyOutput
id (Rid
count (Rcount
weight (Rweight:�������Abandon"�
PetPlus
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments0
phy_strength_weights (RphyStrengthWeights#
power_weights (RpowerWeights#
magic_weights (RmagicWeights%
spirit_weights (RspiritWeights!
will_weights	 (RwillWeights: �����	��Temp��宠物��0"�
EquipmentBaseProp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments@
props (2$.PB.EquipmentBaseProp.OneQualityPropB��Rprops6
OneQualityProp$
addon_group_id (RaddonGroupId: �����	��Temp��装备��1"�
Suite
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

max_equips (R	maxEquips:

equipments (2.PB.Suite.Suit_EquipB��R
equipments2
addons	 (2.PB.Suite.Suit_EquipB��Raddons(
effect_equip_num
 (ReffectEquipNum
file_gfx (	RfileGfx
id_face (RidFace#
file_physique (	RfilePhysique*
hh_type (2.PB.EQUIP_HH_ENUMRhhType
	prop_show (RpropShow

Suit_Equip
id (Rid

Suit_Addon
id (Rid: �����	��Temp��装备��1"�
NPCStartMINIGameService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
game_ids (B��
RgameIds.
update_day_duration (RupdateDayDuration"�
ItemPropertyPill
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp*
type (2.PB.PROPERTY_PILL_TYPERtype$
addon_group_id (RaddonGroupId3
id_cmn_uselimit_config	 (RidCmnUselimitConfig:������Item��2"�
ItemLottery
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp3
id_cmn_uselimit_config (RidCmnUselimitConfig4
require_prof (2.PB.PROFTYPE_MASKRrequireProf(
require_prof_lvl	 (RrequireProfLvl
	req_level
 (RreqLevel9
min_empty_slot_normalpack (RminEmptySlotNormalpack=
min_empty_slot_materialpack (RminEmptySlotMaterialpack9
min_empty_slot_fasionpack (RminEmptySlotFasionpack

dice_count (R	diceCount1
	show_type (2.PB.EXP_LOTTERY_TYPERshowType*
bind_after_opened (RbindAfterOpened*
name_after_opened (	RnameAfterOpened3
file_icon_after_opened (RfileIconAfterOpened
	auto_draw (RautoDrawB

candidates (2.PB.ItemLottery.candidates_tB�� R
candidates^
candidates_t
desc (	Rdesc
icon (Ricon
tag (Rtag
count (Rcount:������Item��3"�
	ItemTitle
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
title_id (RtitleId:������Item��称号��2"�
ItemMedicine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp3
id_cmn_uselimit_config (RidCmnUselimitConfig4
require_prof (2.PB.PROFTYPE_MASKRrequireProf(
require_prof_lvl	 (RrequireProfLvl#
require_level
 (RrequireLevel
	cool_time (RcoolTime.
	cool_type (2.PB.EXP_COOL_TYPERcoolType
only_in_war (R	onlyInWar*
type (2.PB.MEDICINE_TYPE_ENUMRtype)
can_use_when_full (RcanUseWhenFull
values (B��
Rvalues:������Item��3"�
ItemTaskDice
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp:
taskdice_type (2.PB.EXP_TASKDICE_TYPERtaskdiceType3
id_cmn_uselimit_config (RidCmnUselimitConfig(
player_max_level	 (RplayerMaxLevel 
hide_in_pack
 (R
hideInPack

max_amount (R	maxAmount&
only_first_task (RonlyFirstTask!
prepare_time (RprepareTime
not_one_off (R	notOneOff 
file_icon_ex (R
fileIconEx=

task_lists (2.PB.ItemTaskDice.task_t2B��R	taskLists;
task_t2
id (Rid 
probability (Rprobability:������Item��3"�
ItemVehicle
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp

file_model (R	fileModel%
level_required (RlevelRequired2
id_prodskill_required	 (RidProdskillRequired8
level_prodskill_required
 (RlevelProdskillRequired9
is_stopbind_after_produce (RisStopbindAfterProduce5
cooltime_got_after_bind (RcooltimeGotAfterBind7
vehicle_type (2.PB.VEHICLETYPE_ENUMRvehicleType,
max_contain_people (RmaxContainPeople,
open_min_numpeople (RopenMinNumpeople,
keep_min_numpeople (RkeepMinNumpeopleF
relation_to_main (2.PB.VEHECLE_RELATIONREQ_TYPERrelationToMain8
only_send_confirm_to_main (RonlySendConfirmToMain#
confirm_words (	RconfirmWords5
enter_confirm_bind_auto (RenterConfirmBindAuto5
enter_confirm_bind_full (RenterConfirmBindFull<
enter_confirm_team_all_bind (RenterConfirmTeamAllBind8
keepbind_after_transport (RkeepbindAfterTransportR
behaviour_limitmask_main (2.PB.BEHAVLIMIT_MAIN_MASKRbehaviourLimitmaskMainO
behaviour_limitmask_all (2.PB.BEHAVLIMIT_ALL_MASKRbehaviourLimitmaskAll 
req_skill_id (R
reqSkillId"
req_skill_lvl (RreqSkillLvl!
prepare_time (RprepareTime%
interrupt_odds (RinterruptOdds

walk_speed (R	walkSpeed
	run_speed (RrunSpeed

turn_speed  (R	turnSpeed
tid_taskout! (R
tidTaskout
	add_amity" (RaddAmity$
id_skill_bound# (RidSkillBound*
level_skill_bound$ (RlevelSkillBound/
only_main_cast_skill% (RonlyMainCastSkill&
unbind_skill_id& (RunbindSkillId,
unbind_skill_level' (RunbindSkillLevel$
ride_acc_speed( (RrideAccSpeedN
limits_eachpos* (2!.PB.ItemVehicle.BINDLIMIT_EACHPOSB�� RlimitsEachpos

max_member+ (R	maxMember&
min_open_member, (RminOpenMember&
min_keep_member- (RminKeepMember<
confirm_type. (2.PB.EXP_BIND_CONFIRM_TYPERconfirmType�
BINDLIMIT_EACHPOS.

req_gender (2.PB.GENDER_MASKR	reqGender
forbid_xyxw (R
forbidXyxw
duration (Rduration$
trans_scene_id (RtransSceneId
trans_pos_x (R	transPosX
trans_pos_y (R	transPosY
trans_pos_z (R	transPosZ
req_item_id (R	reqItemId1
removeitem_afterbind	 (RremoveitemAfterbind
req_buff_id
 (R	reqBuffId
	req_level (RreqLevel:������Item"�

ItemNormal
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp@
normalitem_type (2.PB.EXP_NORMALITEM_TYPERnormalitemType&
is_show_tooltip (RisShowTooltip
can_compose	 (R
canCompose!
decompose_id
 (RdecomposeId:������Item��3"�
ItemMaterial
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp&
is_show_tooltip (RisShowTooltip
can_compose	 (R
canCompose!
decompose_id
 (RdecomposeId:�������Item��3"�
ItemTask
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp 
hide_in_pack (R
hideInPack

max_amount (R	maxAmount:������Item��2"�
SkillMatter
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp4
require_prof (2.PB.PROFTYPE_MASKRrequireProf(
require_prof_lvl (RrequireProfLvl
level_	 (Rlevel
id_skill
 (RidSkill
level_skill (R
levelSkill
only_in_war (R	onlyInWar

cast_skill (R	castSkill
	cool_time (RcoolTime.
	cool_type (2.PB.EXP_COOL_TYPERcoolTypeX
mask_faction_type_restr (2!.PB.FACTION_TYPE_RESTRICTION_MASKRmaskFactionTypeRestr$
is_revive_coin (RisReviveCoin
	use_limit (RuseLimit9
seltarget_type (2.PB.SELTARGET_TYPERseltargetType.
restrict_target_tid (RrestrictTargetTid'
target_distance (RtargetDistance%
target_tids (B��R
targetTids:������Item��2"�
TaskItemGenerator
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp3
id_cmn_uselimit_config (RidCmnUselimitConfig 
hide_in_pack (R
hideInPack

max_amount	 (R	maxAmount
id_pre_task
 (R	idPreTask 
id_post_task (R
idPostTask@
nation_limit (2.PB.EXP_USE_ITEM_NATION_LIMITRnationLimit
id_scene (RidScene
id_instance (R
idInstance3
target_type (2.PB.SELTARGET_TYPER
targetType
tid_monster (R
tidMonster
tid_npc (RtidNpc(
target_is_corpse (RtargetIsCorpse*
mindist_to_target (RmindistToTarget*
maxdist_to_target (RmaxdistToTarget1
target_min_hp_percent (RtargetMinHpPercent1
target_max_hp_percent (RtargetMaxHpPercent2
make_target_dissapear (RmakeTargetDissapear
action_name (	R
actionName!
enchant_time (RenchantTime!
enchant_text (	RenchantText0
canuse_in_fightstate (RcanuseInFightstate6
interrupt_when_attacked (RinterruptWhenAttacked*
consume_after_use (RconsumeAfterUse!
success_rate (RsuccessRate$
id_client_chat (RidClientChat'
disappear_delay  (RdisappearDelay&
need_count_down! (RneedCountDown
auto_use" (RautoUse%
cube_center$ (B��R
cubeCenter!
	cube_size% (B��RcubeSize#

cube_angle& (B��R	cubeAngle:������Item��3"�
ChangeFaceCard
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp!
display_name (	RdisplayName.
	prof_mask (2.PB.PROFTYPE_MASKRprofMask
prof_lvl	 (RprofLvl
level
 (Rlevel&
skill_id_switch (RskillIdSwitch
custom_name (R
customName
	spec_name (	RspecName
duration (Rduration:������Item"�
MonsterCtrl
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp$
limit_scene_id (RlimitSceneId

id_monster (R	idMonster!
summon_count	 (RsummonCount

is_private
 (R	isPrivate&
use_count_limit (RuseCountLimit.
limit_scene_mins (B��RlimitSceneMins.
limit_scene_maxs (B��RlimitSceneMaxs:������Item"�
Estone
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp:
stone_shape (2.PB.EXP_EQUIP_STONE_SHAPER
stoneShape
stone_level (R
stoneLevel1

equip_mask	 (2.PB.EQUIPSLOT_MASKR	equipMask&
min_equip_level
 (RminEquipLevel&
max_equip_level (RmaxEquipLevel
addon_group (R
addonGroup2

stone_type (2.PB.EXP_ESTONE_TYPER	stoneType:������Item��2"�
	SkillTome
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp4
require_prof (2.PB.PROFTYPE_MASKRrequireProf(
require_prof_lvl (RrequireProfLvl
level_	 (Rlevel#
consume_force
 (RconsumeForce!
prepare_time (RprepareTime%
learn_skillseq (RlearnSkillseq'
learn_skills (B��RlearnSkills:������Item��2"�
Sstone
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp&
min_equip_level (RminEquipLevel&
max_equip_level (RmaxEquipLevel

equip_mask	 (R	equipMask
addon_group
 (R
addonGroup
	effect_id (ReffectId:������Item"�

LootObject
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp

file_model (R	fileModelR
loot_object_reward_type (2.PB.LOOT_OBJECT_REWARD_TYPERlootObjectRewardType1
reward_bind_money_min	 (RrewardBindMoneyMin1
reward_bind_money_max
 (RrewardBindMoneyMax/
reward_buff_skill_id (RrewardBuffSkillId5
reward_buff_skill_level (RrewardBuffSkillLevel#
reward_points (RrewardPoints(
reward_box_level (RrewardBoxLevel0
reward_reputation_id (RrewardReputationId6
reward_reputation_value (RrewardReputationValue!
model_effect (RmodelEffect#
pickup_effect (RpickupEffect@
pickup_hiddentreasure_effect (RpickupHiddentreasureEffect:������Item"�
HeroCardCoupon
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp#
require_level (RrequireLevel2
require_pro (2.PB.PROFTYPE_MASKR
requirePro
tid	 (Rtid:����	��Item��坐骑��1"�

HeroFlixir
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp,
prof_exp_increment (RprofExpIncrement#
exp_increment (RexpIncrement,
required_player_lv	 (RrequiredPlayerLvY
exp_random_increment (2!.PB.HeroFlixir.ExpRandomIncrementB��RexpRandomIncrementM
ExpRandomIncrement
amount (Ramount
rand_weight (R
randWeight:�� ��	��Item��坐骑��1"�

ZhaoJiLing
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonPropF
user_user_limit (2.PB.ZHAOJILING_USER_LIMIT_MASKRuserUserLimit;
target_type (2.PB.ZHAOJILING_TARGET_TYPER
targetType(
target_min_level	 (RtargetMinLevel(
target_max_level
 (RtargetMaxLevel:
can_use_in_foreign_country (RcanUseInForeignCountry'
random_distance (RrandomDistance:��!����Item��1"�	
ScrollOfTownPortal
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp&
target_scene_id (RtargetSceneId 
target_pos_x (R
targetPosX 
target_pos_y	 (R
targetPosY 
target_pos_z
 (R
targetPosZ3
target_pos_rand_radius (RtargetPosRandRadius+
use_limit_scene_id (RuseLimitSceneId1
use_limit_instance_id (RuseLimitInstanceId,
use_limit_pos_min_x (RuseLimitPosMinX,
use_limit_pos_min_y (RuseLimitPosMinY,
use_limit_pos_min_z (RuseLimitPosMinZ,
use_limit_pos_max_x (RuseLimitPosMaxX,
use_limit_pos_max_y (RuseLimitPosMaxY,
use_limit_pos_max_z (RuseLimitPosMaxZ
action_name (	R
actionName!
enchant_time (RenchantTime!
enchant_text (	RenchantText
	cool_time (RcoolTime.
	cool_type (2.PB.EXP_COOL_TYPERcoolType:
can_use_in_foreign_country (RcanUseInForeignCountry-
can_use_in_instance (RcanUseInInstance0
interupt_on_attacked (RinteruptOnAttacked*
consume_after_use (RconsumeAfterUse:��#����Item��2"�
ItemDiamond
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonPropK
give_diamond_bind_type (2.PB.EXP_SELL2SHOP_MODERgiveDiamondBindType.
give_diamond_amount (RgiveDiamondAmount
	cool_time	 (RcoolTime:��$����Item��2"�

DiamondBag
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp>
is_diamond_bind (2.PB.EXP_SELL2SHOP_MODERisDiamondBind:��%����Item��2"�

PKImmunity
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
	cool_time (RcoolTime!
duration_sec (RdurationSec
uselimit_id	 (R
uselimitId6
has_effect_in_nation_war
 (RhasEffectInNationWar:��&����Item��1"�
MingWen
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp!
decompose_id (RdecomposeId'
color (2.PB.MINGWEN_COLORRcolor#
mingwen_level	 (RmingwenLevel)
mingwen_category
 (RmingwenCategory 
cost_item_id (R
costItemId&
cost_item_count (RcostItemCount1

equip_mask (2.PB.EQUIPSLOT_MASKR	equipMask&
min_equip_level (RminEquipLevel&
max_equip_level (RmaxEquipLevel
addon_group (R
addonGroup:��'��	��Item��装备��1"�
PetEgg
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp!
legal_switch (RlegalSwitch
level (Rlevel4
animals
 (2.PB.PetEgg.OneAnimalB��RanimalsO
	OneAnimal
	animal_id (RanimalId%
animal_weights (RanimalWeights:��(��	��Item��宠物��1"�
ItemGrantReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp>
rewards (2.PB.ItemGrantReward.OneReward3B��RrewardsR

OneReward3!
grant_reward (RgrantReward!
reward_count (RrewardCount:��)����Item��2"�
ItemTalentBreak
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp:
groups (2.PB.ItemTalentBreak.OneGroupB��Rgroups:
OneGroup
value (Rvalue
weights (Rweights:��*��	��Item��天赋��1"�
ServerReputationItem
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
	use_limit (RuseLimitP
server_repu	 (2).PB.ServerReputationItem.ServerReputationB��R
serverRepu.
personal_repu_count
 (RpersonalRepuCountT
personal_repu (2).PB.ServerReputationItem.ReputationRewardB��RpersonalRepuJ
ServerReputation
repu_id (RrepuId

reward_tid (R	rewardTidJ
ReputationReward
repu_id (RrepuId

reward_tid (R	rewardTid:��+����Item��2"�
ItemGeneralStar
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp&
general_soul_id (RgeneralSoulId
lottery_pos (R
lotteryPos&
got_lucky_value	 (RgotLuckyValue$
max_star_level
 (RmaxStarLevel(
upgrade_star_cfg (RupgradeStarCfgF

properties (2 .PB.ItemGeneralStar.OneProperty2B��R
propertiesV
OneProperty2
property_id (R
propertyId%
property_value (RpropertyValue:����	��Item��将魂��1"�
ItemPetSkill
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
skill_id (RskillId(
general_skill_id (RgeneralSkillId$
range_skill_id	 (RrangeSkillId"
is_high_level
 (RisHighLevel.
low_level_skilltome (RlowLevelSkilltome
	stay_rate (RstayRate:����	��Item��宠物��1"�
ItemGift
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp)
present_goodwill (RpresentGoodwill#
present_charm (RpresentCharm#
present_words	 (	RpresentWords/
id_skill_to_receiver
 (RidSkillToReceiver5
level_skill_to_receiver (RlevelSkillToReceiver;
item_disappear_after_skill (RitemDisappearAfterSkill
gfx_destroy (R
gfxDestroy
repu_id (RrepuId

repu_count (R	repuCount
money_count (R
moneyCount#
bind_diamonds (RbindDiamonds%
friendly_count (RfriendlyCount

give_limit (R	giveLimit
	use_limit (RuseLimit

use_object (R	useObject
use_gfx (RuseGfx
speak_id (RspeakId:������Item"�
ItemFashionActivator
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
name (	Rname
tid (B��Rtid
expire_time (R
expireTime: �����	��Item��时装��1"�
FashionTempl
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
icon_id (RiconId"
left_model_id (RleftModelId$
right_model_id (RrightModelId$
left_effect_id	 (RleftEffectId&
right_effect_id
 (RrightEffectId*
left_effect_point (	RleftEffectPoint,
right_effect_point (	RrightEffectPoint$
type (2.PB.TYPE_FASHIONRtype*
activate_addon_id (RactivateAddonId$
equip_addon_id (RequipAddonIdL
dyeing_part_list (2.PB.FashionTempl.dyeing_partB��RdyeingPartList'
gender (2.PB.GENDER_ENUMRgenderh
dyeing_part
	dyeing_id (RdyeingId
icon_id (RiconId#
default_color (RdefaultColor:!��,��	��Config��时装��1"�
DyeingConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
unlock_item_id (RunlockItemId*
unlock_item_count (RunlockItemCount$
dyeing_item_id (RdyeingItemId*
dyeing_item_count (RdyeingItemCountO
color_config_list	 (2.PB.DyeingConfig.color_configB�� RcolorConfigList�
color_config
color_id (RcolorId
weight (Rweight)
unlock_condition (RunlockCondition
addon_id (RaddonId:"�����	��Config��时装��1"�
Activity
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments 
id_ctrls (B���RidCtrls 
id_tasks (B���RidTasks%
id_services
 (B�� R
idServices 
id_items (B���RidItems:�������Config��1"�
ActivityDroptable
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
id_activity (R
idActivity
id_loot (RidLoot%
id_monsters	 (B��dR
idMonsters:�������Npc��1"�
EquipLevelUp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	level_max (RlevelMax7
configs (2.PB.EquipLevelUp.ConfigB��Rconfigsa
Config
req_exp (RreqExp
addon_group (R
addonGroup

money_cost (R	moneyCost: �����	��Temp��装备��0"�
ItemEquivalence
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	show_name (	RshowName
item_ids (B��@RitemIds:�������Abandon"�
Achievement
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname"
id_major_type (RidMajorType
id_sub_type (R	idSubType,
type (2.PB.EXP_ACHIEVEMENT_TYPERtype7
lib_type	 (2.PB.EXP_ACHIEVEMENT_LIB_TYPERlibType
	file_icon
 (RfileIcon
	broadcast (R	broadcastB
broadcast_channel (2.PB.CHAT_CHANNEL_ENUMRbroadcastChannel#
achieve_class (RachieveClass

sort_index (R	sortIndexD
achieve_section (2.PB.EXP_ACHIEVESECTION_MASKRachieveSection]
unlock_state_condition (2!.PB.Achievement.UNLOCK_STATE_CONDB��RunlockStateCondition`
achieve_state_condition (2".PB.Achievement.ACHIEVE_STATE_CONDB��RachieveStateCondition`
achieve_event_condition (2".PB.Achievement.ACHIEVE_EVENT_CONDB��RachieveEventCondition#
achieve_score (RachieveScore$
id_grant_award (RidGrantAward 
id_title_out (R
idTitleOut
	desc_text (	RdescText
dropinfo_id (R
dropinfoId
	show_flag (RshowFlagn
UNLOCK_STATE_COND=
	condition (2.PB.EXP_ACHIEVE_STATE_CONDITIONR	condition
param (B��Rparamo
ACHIEVE_STATE_COND=
	condition (2.PB.EXP_ACHIEVE_STATE_CONDITIONR	condition
param (B��Rparamo
ACHIEVE_EVENT_COND=
	condition (2.PB.EXP_ACHIEVE_EVENT_CONDITIONR	condition
param (B��Rparam:�������Temp��2"�
PetDecompose
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsL
decompose_infos (2.PB.PetDecompose.OneDecomposeB��dRdecomposeInfosk
OneDecompose
start_level (R
startLevel
	end_level (RendLevel

reward_tid (R	rewardTid: �����	��Temp��宠物��1"�
PetDemonize
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
critical_times (RcriticalTimes#
critical_rate (RcriticalRate"
cost_item_tid (RcostItemTid3
stars	 (2.PB.PetDemonize.OneStarB��$Rstars�
OneStar&
cost_item_count (RcostItemCount(
cost_money_count (RcostMoneyCount"
add_value_min (RaddValueMin"
add_value_max (RaddValueMax
value_limit (R
valueLimit#
value_upgrade (RvalueUpgrade%
reduction_rate (RreductionRate(
reset_reward_tid (RresetRewardTid: �����	��Temp��宠物��1"�
PetXiSui
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

animal_tid (R	animalTid"
cost_item_tid (RcostItemTid&
cost_item_count (RcostItemCount(
quality_rate_min (RqualityRateMin(
quality_rate_max	 (RqualityRateMax: �����	��Temp��宠物��0"�
NPCLotteryService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname=
cost_money_type (2.PB.EXP_SHOPSELL_MODERcostMoneyType

cost_money (R	costMoney,
cost_reputation_id (RcostReputationId'
cost_reputation	 (RcostReputation5
free_common_uselimit_id
 (RfreeCommonUselimitId-
free_cool_down_time (RfreeCoolDownTime)
free_cool_down_id (RfreeCoolDownId.
need_inventory_slot (RneedInventorySlot3
id_cmn_uselimit_config (RidCmnUselimitConfigD
	cost_item (2!.PB.NPCLotteryService.cost_item_tB��RcostItem
icon (RiconE
cost_item_t
item_id (RitemId

item_count (R	itemCount:��U����Npc��服务��0"�
InstanceGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname)
instance_tids (B��RinstanceTidsH
mode_config (2!.PB.InstanceGroup.LevelModeConfigB��R
modeConfig�
LevelModeConfig

level_mode (R	levelModeS
star_reward (2,.PB.InstanceGroup.LevelModeConfig.StarRewardB��
R
starRewardH

StarReward

star_count (R	starCount
	reward_id (RrewardId:"�������Config��副本��1"�
RecoverReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments<
configs (2.PB.RecoverReward.OneConfig2B��@Rconfigs�

OneConfig2
activtiy_id (R
activtiyId
	min_level (RminLevel@

limit_type (2!.PB.EXP_RECOVER_REWARD_LIMIT_TYPER	limitType
limit_param (R
limitParamJ
reward_show_type (2 .PB.EXP_RECOVER_REWARD_SHOW_TYPERrewardShowType!
cost_stamina (RcostStamina/
cost_money_reward_id (RcostMoneyRewardId

cost_money (R	costMoney3
cost_diamond_reward_id	 (RcostDiamondRewardId!
cost_diamond
 (RcostDiamond:�������Config��0"�
WingFixView
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
resource_path_id (RresourcePathId
icon_id (RiconId$
addon_group_id (RaddonGroupId

time_limit (R	timeLimit%
how_to_get_desc	 (	RhowToGetDesc5
how_to_get_open_ui_index
 (RhowToGetOpenUiIndex

sort_index (R	sortIndex

auto_equip (R	autoEquipR
officer_restrict_to_equip (2.PB.NATION_OFFICER_MASKRofficerRestrictToEquip
	open_soul (RopenSoul&
max_soul_phases (RmaxSoulPhases3
phases (2.PB.WingFixView.PhaseB��Rphases�
Phase
name (	Rname
soul_ids (B��RsoulIds&
cost_item_count (RcostItemCount$
soul_count_min (RsoulCountMin$
soul_count_max (RsoulCountMax;
items (2.PB.WingFixView.Phase.ItemInfo2B��Ritems�
	ItemInfo2J
results (2*.PB.WingFixView.Phase.ItemInfo2.ResultInfoB��Rresults�

ResultInfo
result_rate (R
resultRate!
total_shares (RtotalShares&
range_delta_min (RrangeDeltaMin&
range_delta_max (RrangeDeltaMax: �����	��Temp��翅膀��0"�
	NationWar
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
player_min_level (RplayerMinLevel(
capital_scene_id (RcapitalSceneId,
cocapital_scene_id (RcocapitalSceneId&
border_scene_id (RborderSceneId

need_money	 (R	needMoney#
cooldown_time
 (RcooldownTime5
result_determine_npc_id (RresultDetermineNpcId
npc_id (B��RnpcId0
free_transport_count (RfreeTransportCount;
quick_transport_cost_money (RquickTransportCostMoneyR
attack_init_revive_point (2.PB.NationWar.RevivePointRattackInitRevivePointc
attack_occupy_revive_point (2 .PB.NationWar.CoccupyRevivePointB��RattackOccupyRevivePointT
defence_init_revive_point (2.PB.NationWar.RevivePointRdefenceInitRevivePoint3
reward_cmn_uselimit_id (RrewardCmnUselimitId*
winner_exp_reward (RwinnerExpReward(
loser_exp_reward (RloserExpReward\
winner_officer_exp_bonus (2.PB.NationWar.OfficerExpBonusB��RwinnerOfficerExpBonus#
winner_reward (RwinnerReward!
loser_reward (RloserReward4
winner_alliance_reward (RwinnerAllianceReward9
kill_enemy_ranking_reward (RkillEnemyRankingRewardD
kill_enemy_ranking_lucky_reward (RkillEnemyRankingLuckyReward>
attack_kill_enemy_repu_value (RattackKillEnemyRepuValue@
defence_kill_enemy_repu_value (RdefenceKillEnemyRepuValueB
alliance_kill_enemy_repu_value  (RallianceKillEnemyRepuValue?
funds_usage" (2.PB.NationWar.FundsUsageB��R
fundsUsage~
RevivePoint
name (	Rname
scene_id (RsceneId
x (Rx
y (Ry
z (Rz
radius (Rradius�
CoccupyRevivePoint<
revive_point (2.PB.NationWar.RevivePointRrevivePoint

can_retake (R	canRetake
priority (Rpriority,
relative_npc_id (B��RrelativeNpcIdm
OfficerExpBonus:

officer_id (2.PB.EXP_NATION_OFFICER_ENUMR	officerId

percentage (R
percentage�

FundsUsage
name (	Rname
icon (RiconO
need_officer_permission (2.PB.NATION_OFFICER_MASKRneedOfficerPermission
	use_times (RuseTimes&
cost_fund_money (RcostFundMoney
param (Rparam: �������Temp��国家��0"�
MixSuite
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname.
quality (2.PB.EXP_QUALITY_ENUMRquality*
type (2.PB.EXP_MIX_SUITE_TYPERtype$
addon_group_id (RaddonGroupId"
next_suite_id	 (RnextSuiteId
desc
 (	Rdesc: �����	��Temp��装备��1"�
HeJiu
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments/
need_approach_npc_id (RneedApproachNpcId!
max_distance (RmaxDistance"
exp_each_time (RexpEachTime!
enchant_time (RenchantTime
total_count	 (R
totalCount%
lvexpfactor_id
 (RlvexpfactorId"
wine_item_tid (RwineItemTid:�������Config��0"�
EquipChaiJie
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments=
gain_material_count_odds (B��RgainMaterialCountOddsJ
gain_materials (2.PB.EquipChaiJie.GainMaterialB��RgainMaterialsJ
GainMaterial&
grant_reward_id (RgrantRewardId
odds (Rodds: �����	��Temp��装备��1"�
GeneralSoul
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_holes (RmaxHoles0
hole_unlock_level (B��RholeUnlockLevel(
common_summon_id (RcommonSummonId&
lucky_summon_id	 (RluckySummonId.
lottery_addon_group
 (RlotteryAddonGroup
	max_level (RmaxLevel!
package_size (RpackageSize6
levels (2.PB.GeneralSoul.OneLevelB��Rlevelsf
OneLevel0
upgrade_required_exp (RupgradeRequiredExp(
power_up_percent (RpowerUpPercent: �����	��Temp��将魂��0"�
SummonGeneralCost
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
unlock_level (RunlockLevel
	use_limit (RuseLimit*
lucky_value_limit (RluckyValueLimit:
costs	 (2.PB.SummonGeneralCost.OneCost2B��Rcostsa
OneCost26
	cost_type (2.PB.EXP_COMMON_ITEM_TYPE3RcostType

cost_value (R	costValue: �����	��Temp��将魂��0"�
GeneralUpgrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments:
levels (2.PB.GeneralUpgrade.OneLevel1B��2Rlevelsy
	OneLevel1!
upgrade_rate (RupgradeRate%
extra_percents (RextraPercents"
eaten_got_exp (ReatenGotExp: �����	��Temp��将魂��0"�
WingSoul
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	file_icon (RfileIcon
property_id (R
propertyId

base_value (R	baseValue
limit_value (R
limitValue-
inc_value_one_share	 (RincValueOneShare: �����	��Temp��翅膀��0"�	
HomeBuilding
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

brief_desc (	R	briefDesc;
build_style (2.PB.EXP_HOMEBUILDING_STYLER
buildStyle4

build_type (2.PB.HOMEBUILDING_TYPER	buildType&
owned_max_count (RownedMaxCount
	icon_path	 (RiconPath*
setting_icon_path
 (RsettingIconPath&
ground_size_len (RgroundSizeLen&
ground_size_wid (RgroundSizeWid*
npc_controller_id (RnpcControllerId
model_id (RmodelId 
summon_pos_x (R
summonPosX 
summon_pos_y (R
summonPosY(
summon_pos_angle (RsummonPosAngle
quality (Rquality0
special_sprite_value (RspecialSpriteValue=
sell2shop_mode (2.PB.EXP_SELL2SHOP_MODERsell2shopMode
price (Rprice:
shopsell_mode (2.PB.EXP_SHOPSELL_MODERshopsellMode

shop_price (R	shopPrice3
can_be_bought_directly (RcanBeBoughtDirectly<
	set_scene (2.PB.HOMEBUILDING_SET_SCENE_TYPERsetScene/
can_be_bought_spouse (RcanBeBoughtSpouse
get_way (RgetWay

sort_index (R	sortIndex"
jihuo_item_id (RjihuoItemId: �������Temp��家园��0"�
	EquipSoul
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments@

properties (2.PB.EquipSoul.OneProperty3B��2R
propertiesP
OneProperty3
property_id (R
propertyId
upgrade_cfg (R
upgradeCfg: �����	��Temp��装备��0"�
EquipSoulDropUpgrade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

cost_money (R	costMoney
	max_level (RmaxLevel@
levels (2".PB.EquipSoulDropUpgrade.OneLevel2B��Rlevelsb
	OneLevel2#
require_value (RrequireValue0
property_value_delta (RpropertyValueDelta: �����	��Temp��装备��0"�
WineGame
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
unlock_level (RunlockLevel$
game_use_limit (RgameUseLimit*
refresh_use_limit (RrefreshUseLimit0
items	 (2.PB.WineGame.OneItemB��Ritems
up_percents
 (R
upPercents'
bottom_percents (RbottomPercents
	wine_rate (RwineRate!
status_count (RstatusCount>
statusinfos (2.PB.WineGame.OneStatusB��Rstatusinfos*
success_award_tid (RsuccessAwardTid$
fail_award_tid (RfailAwardTidT
OneItem
item_tid (RitemTid.
wine_capacity_delta (RwineCapacityDeltaU
	OneStatus'
status_percents (RstatusPercents
status_text (	R
statusText:�������Abandon"�
CorpsRegionWar
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	open_time (RopenTime$
task_use_limit (RtaskUseLimit!
instance_tid (RinstanceTid4
required_faction_level (RrequiredFactionLevel2
required_player_level	 (RrequiredPlayerLevel,
compensate_task_id
 (RcompensateTaskId*
encourage_buff_id (RencourageBuffId.
encourage_use_limit (RencourageUseLimitH
encourageinfo (2.PB.CorpsRegionWar.OneLevel3B��Rencourageinfo2
encourage_times_limit (RencourageTimesLimit]
	OneLevel3!
cost_diamond (RcostDiamond-
accumulative_times (RaccumulativeTimes: �������Temp��公会��0"�
CorpsRegionWarSections
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

section_id (R	sectionId
is_legal (RisLegal
	nation_id (RnationId
scene_id (RsceneId"
can_be_robbed	 (RcanBeRobbed3
robbed_by_other_nation
 (RrobbedByOtherNationE
 robbed_by_other_nation_when_weak (RrobbedByOtherNationWhenWeak:
required_leaderboard_rank (RrequiredLeaderboardRank,
base_faction_money (RbaseFactionMoney7
faction_money_delta_once (RfactionMoneyDeltaOnce,
diamond_delta_once (RdiamondDeltaOnce<
adjusts_bid_by_other_nation (RadjustsBidByOtherNationA
return_money_percents_success (RreturnMoneyPercentsSuccess;
return_money_percents_fail (RreturnMoneyPercentsFail5
winner_master_bonus_tid (RwinnerMasterBonusTid3
loser_master_bonus_tid (RloserMasterBonusTid/
reward_tid_every_day (RrewardTidEveryDay<
master_reward_tid_every_day (RmasterRewardTidEveryDay: �������Temp��公会��0"�
EightInstCrack
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
unlock_vip_level (RunlockVipLevelB
	instances (2.PB.EightInstCrack.OneInstanceB��R	instances�
OneInstance!
instance_tid (RinstanceTid*
common_reward_tid (RcommonRewardTid

crack_rate (R	crackRate`
achievement_infos (2-.PB.EightInstCrack.OneInstance.OneAchievementB��RachievementInfos2
center_achievement_id (RcenterAchievementId*
center_reward_tid (RcenterRewardTidT
OneAchievement#
achivement_id (RachivementId

reward_tid (R	rewardTid:�������Config��0"�
	BadgeProp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	icon_path (RiconPath:
badge_level (2.PB.BadgeProp.BadgeB��R
badgeLevel6
Badge
pic_path (RpicPath
prop (Rprop:�������Abandon"�
InsideEquipGradeUp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments2
upgrade_need_level (B��RupgradeNeedLevelG
configs (2&.PB.InsideEquipGradeUp.OneLevelConfig1B���Rconfigs�
OneLevelConfig1U

cost_items (20.PB.InsideEquipGradeUp.OneLevelConfig1.CostItem2B��R	costItems

cost_money (R	costMoney'
sub_level_cfg_id (RsubLevelCfgId3
	CostItem2
tid (Rtid
count (Rcount: �����	��Temp��装备��1"�
	EquipSuit
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments.
	need_prof (2.PB.PROFTYPE_MASKRneedProf
	weapon_id (RweaponId
	armour_id (RarmourId$
addon_group_id (RaddonGroupId: �����	��Temp��装备��1"�
InsideEquipSubGradeUp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	max_level (RmaxLevelI
configs
 (2).PB.InsideEquipSubGradeUp.OneLevelConfig2B��Rconfigs�
OneLevelConfig2X

cost_items (23.PB.InsideEquipSubGradeUp.OneLevelConfig2.CostItem1B��R	costItems

cost_money (R	costMoney
hp (Rhp

phy_attack (R	phyAttack
phy_def (RphyDef

mag_attack (R	magAttack
mag_def (RmagDef3
	CostItem1
tid (Rtid
count (Rcount: �����	��Temp��装备��0"�
	ItemBadge
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
badge_id (RbadgeId
badge_level (R
badgeLevel:��-����Item"�
ItemBlessingTicket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp#
reputation_id (RreputationId+
reputation_amount (RreputationAmount:��.����Item��1"�
	ItemDanMu
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
	use_limit (RuseLimit
	cool_down (RcoolDown:��/����Item��1"�
ItemSceneLevelRewardBox
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp:��0����Item"�
ItemRetainedGift
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp+
days_limit_to_open (RdaysLimitToOpen&
grant_reward_id (RgrantRewardId:��1����Item��1"�
StoneCompose
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp&
max_stone_level (RmaxStoneLevel:��2����Item"�
Fragment
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
fragment_id (R
fragmentId%
fragment_count (RfragmentCount:��G����Item"�
GeneralSoulTicket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp&
general_soul_id (RgeneralSoulId:��3��	��Item��将魂��1"�
HolyBossMaterial
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp

add_growth (R	addGrowth#
feeding_award (RfeedingAward"
cmn_use_limit (RcmnUseLimit:��4����Item��2"�
ItemFurnitureActivate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp!
furniture_id (RfurnitureId:��5����Item��家园��1"�
LianXingStone
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp4
success_rate_numerator (RsuccessRateNumerator:��6��	��Item��装备��1"�
MagicWeaponTicket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp&
magic_weapon_id (RmagicWeaponId
re_use (RreUse:��7��	��Item��法器��1"�
FusionMaterial
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp:��8����Item"�
ChemistryMedicine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp:��9����Item"�
FormationLearning
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp!
formation_id (RformationId!
acquire_exp1 (RacquireExp1!
acquire_exp2 (RacquireExp2:��:��	��Item��阵法��1"�
BuddyTicket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
buddy_id (RbuddyIdD
available_duration (2.PB.EXP_DURATION_ENUMRavailableDuration:��;��	��Item��伙伴��1"�

ItemPetExp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
add_exp (RaddExp:��<��	��Item��宠物��1"�
InnerMedicine
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp$
upgrade_cfg_id (RupgradeCfgId
type (Rtype:��=����Item"�

PetCulture
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp4
type (2 .PB.EXP_ANIMAL_CULTURE_TYPE_ENUMRtype

add_points (R	addPoints
add_groupup (R
addGroupup&
add_quality_min	 (RaddQualityMin&
add_quality_max
 (RaddQualityMax
	use_limit (RuseLimit:��>��	��Item��宠物��1"�
ItemPhotoExchange
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
is_frame (RisFrame
photo_id (RphotoId:��?����Item"�
ItemRedPacket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp?
red_packet_type (2.PB.EXP_RED_PACKET_TYPERredPacketType&
cmn_uselimit_id (RcmnUselimitId<
officer_limit (2.PB.NATION_OFFICER_MASKRofficerLimitQ
faction_position_limit	 (2.PB.EXP_CORPS_POSITION_MASKRfactionPositionLimit:
can_use_in_foreign_country
 (RcanUseInForeignCountry:
gain_reward_min_role_level (RgainRewardMinRoleLevel:
gain_reward_max_role_level (RgainRewardMaxRoleLevelF
gain_reward_prof_limit (2.PB.PROFTYPE_MASKRgainRewardProfLimitI
gain_reward_gender_limit (2.PB.GENDER_LIMITRgainRewardGenderLimit<
gain_reward_cmn_uselimit_id (RgainRewardCmnUselimitId
	reward_id (RrewardId,
total_reward_count (RtotalRewardCount3
red_packet_piece_count (RredPacketPieceCountA
min_reward_count_in_each_piece (RminRewardCountInEachPieceA
max_reward_count_in_each_piece (RmaxRewardCountInEachPiece?
extra_reward_id_in_each_piece (RextraRewardIdInEachPiece(
auto_gain_reward (RautoGainReward,
affect_time_length (RaffectTimeLength#
affect_radius (RaffectRadius4
use_red_packet_speak_id (RuseRedPacketSpeakId/
gain_reward_speak_id (RgainRewardSpeakIdG
!min_reward_count_to_trigger_speak (RminRewardCountToTriggerSpeak
limit_scene (R
limitScene
desc (	Rdesc

token_ring (	R	tokenRing:��@����Item��2"�
ItemSeed
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
	ripe_time (RripeTime3
diamond_needed_per_min (RdiamondNeededPerMin
award_id (RawardId"
cmn_use_limit
 (RcmnUseLimit:��A����Item"�
	CangBaoTu
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
anim (	Ranim"
auto_use_time (RautoUseTime
task_id (RtaskId:��B����Item��1"�
ItemVipPrivilege
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp#
vip_privilege (RvipPrivilege:"��F��
��Item��	特权卡��0"�
WingFixViewTicket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp
	config_id (RconfigId
re_use (RreUse:��C��	��Item��翅膀��1"�
Compensation
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId
	reward_id (RrewardId$
min_role_level (RminRoleLevel5
counter_account_repu_id (RcounterAccountRepuId:�������Config��1"�
InstanceRandomLevel
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments?
randoms (2.PB.InstanceRandomLevel.RandomB���RrandomsY
Random0
level_random_weights (RlevelRandomWeights

reward_tid (R	rewardTid:"�������Config��副本��1"�
RankListReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
activity_id (R
activityId,
server_activity_id (RserverActivityId
ranklist_id (R
ranklistId'
reward_duration (RrewardDuration3
check_server_open_days	 (RcheckServerOpenDays/
min_server_open_days
 (RminServerOpenDays/
max_server_open_days (RmaxServerOpenDays=
configs (2.PB.RankListReward.OneConfig3B��Rconfigs
type (RtypeH

OneConfig3
rank (Rrank&
grant_reward_id (RgrantRewardId:�������Config��1"�
FriendBlessing
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsJ
greeting_type (2%.PB.EXP_FRIEND_BLESSING_GREETING_TYPERgreetingType%
greeting_value (RgreetingValueD
invest_type (2#.PB.EXP_FRIEND_BLESSING_INVEST_TYPER
investType!
invest_value (RinvestValue*
invest_valid_time	 (RinvestValidTime,
investor_reward_id
 (RinvestorRewardId,
investee_reward_id (RinvesteeRewardId: �������Temp��好友��0"�

PaTaConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
required_level (RrequiredLevel/
cmn_uselimit_free_id (RcmnUselimitFreeId/
cmn_uselimit_cost_id (RcmnUselimitCostId/
limit_time_per_floor (RlimitTimePerFloor&
start_wait_time	 (RstartWaitTime5
hero_summon_limit_count
 (RheroSummonLimitCount*
pata_instance_tid (RpataInstanceTid:�������Config��0"�
BlackMarket
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname,
refresh_cost_money (RrefreshCostMoney<
auto_refresh_hour_of_day (B��RautoRefreshHourOfDay#

library_id
 (B��R	libraryId3
id_cmn_uselimit_config (RidCmnUselimitConfig:"�������Config��黑市��1"�
BlackMarketLibrary
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname>

money_type (2.PB.EXP_BLACK_MARKET_MONEY_TYPER	moneyType(
money_type_param (RmoneyTypeParam
is_bind (RisBind6
black_market_item_id
 (B���RblackMarketItemId:"�������Config��黑市��1"�
BlackMarketItem
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname 
ivtr_item_id (R
ivtrItemId&
ivtr_item_count (RivtrItemCount
rand_weight (R
randWeight
price	 (Rprice:"�������Config��黑市��1"�

ClimbTower
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments=
reset_progress_cmn_uselimit (RresetProgressCmnUselimit
instance_id (R
instanceId
	max_level (RmaxLevel&
need_role_level (RneedRoleLevel
award_id	 (RawardId1
auto_finish_cost_time
 (RautoFinishCostTimeI
"turbo_auto_finish_cost_trade_money (RturboAutoFinishCostTradeMoney/
black_market_shop_id (RblackMarketShopIdU
(black_market_shop_appear_rate_percentage (R#blackMarketShopAppearRatePercentage5
recommended_fightvalue (RrecommendedFightvalue

monster_id (R	monsterId:�������Config��0"�
AnswerRankAward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments>
configs (2.PB.AnswerRankAward.OneConfig4B��Rconfigs9

OneConfig4
rank (Rrank
id_task (RidTask:�������Config��0"�
ItemPetResetProp
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonPropB

reset_type (2#.PB.EXP_ANIMAL_RESET_PROP_TYPE_ENUMR	resetType:��E��	��Item��宠物��1"�
ItemPetXiSui
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments3
common_prop (2.PB.ItemCommonPropR
commonProp: �����	��Item��宠物��1"�
RoleTemplate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments#
delay_seconds (RdelaySeconds
area_id (RareaId
prof (Rprof
gender (Rgender
skills (B��Rskills
tasks (B��Rtasks1
items (2.PB.RoleTemplate.ItemB��
Ritems 
Item
tids (B��Rtids:�������Config��0"�	
Thrower
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
npc_tid (RnpcTid0
levels (2.PB.Thrower.Level5B��Rlevels�
Level5
	min_range (RminRange
	max_range (RmaxRange-
aim_at_transform_id (RaimAtTransformId7
launch_ball_transform_id (RlaunchBallTransformId"
boom_skill_id (RboomSkillId(
boom_skill_level (RboomSkillLevel6
npc_launch_ball_skill_id (RnpcLaunchBallSkillId<
npc_launch_ball_skill_level (RnpcLaunchBallSkillLevel&
ball_subobj_tid	 (RballSubobjTid1
ball_subobj_skill_tid
 (RballSubobjSkillTid*
player_subobj_tid (RplayerSubobjTid5
player_subobj_skill_tid (RplayerSubobjSkillTid7
early_warning_subobj_tid (RearlyWarningSubobjTidB
early_warning_subobj_skill_tid (RearlyWarningSubobjSkillTid&
aimat_run_speed (RaimatRunSpeed#
thrower_speed (RthrowerSpeed:
npc_launch_player_skill_id (RnpcLaunchPlayerSkillId@
npc_launch_player_skill_level (RnpcLaunchPlayerSkillLevel/
drop_player_skill_id (RdropPlayerSkillId5
drop_player_skill_level (RdropPlayerSkillLevel:�������Npc��1"�
SkillClassConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments"
type (2.PB.SKILL_TYPERtype*
use_way (2.PB.SKILL_USEMODERuseWay0
	time_type (2.PB.SKILL_TIME_TYPERtimeTypeB
time_display_type (2.PB.SKILL_BARSHOW_TYPERtimeDisplayType:"�������SkillClassConfig��2"�
SkillSourceConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments(
source (2.PB.SKILL_ORIGINRsource&
learn_level_max (RlearnLevelMax 
learn_pre_id (R
learnPreId&
learn_pre_level (RlearnPreLevel+
learn_pre_level_sq	 (	RlearnPreLevelSq)
learn_pre_item_sq
 (	RlearnPreItemSq4
learn_pre_item_count_sq (	RlearnPreItemCountSq+
learn_exp_force_sq (	RlearnExpForceSq+
learn_exp_money_sq (	RlearnExpMoneySq&
pre_inhesion_sq (	RpreInhesionSq3
pre_inhesion_exp_total (RpreInhesionExpTotal1
pre_inhesion_limit_sq (	RpreInhesionLimitSq>
pre_inhesion_limit_exp_total (RpreInhesionLimitExpTotal1
level_up_exp_inhesion (RlevelUpExpInhesion!
inhesion_max (RinhesionMax!
pre_lifeprof (RpreLifeprof:#�������SkillSourceConfig��2"�
SKILL_PERFORM_SUBOBJ
	subobj_id (RsubobjId;
subobj_gen_type (2.PB.SUBOBJ_GEN_TYPERsubobjGenType(
subobj_gen_angle (	RsubobjGenAngle.
subobj_gen_distance (	RsubobjGenDistance!
create_delay (	RcreateDelay9
subobj_replace_perform_id (RsubobjReplacePerformId?
subobj_replace_perform_delay (RsubobjReplacePerformDelay"�
SkillPerformConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
type (2.PB.PERFORM_CLASSRtype

display_id (R	displayId
duration (	Rduration&
max_charge_time (RmaxChargeTime8
damage_type (2.PB.PERFORM_DAMAGE_TYPER
damageType2
damage_skill_external (	RdamageSkillExternal0
scale_skill_external (	RscaleSkillExternal2
damage_skill_internal (	RdamageSkillInternal0
scale_skill_internal (	RscaleSkillInternal!
main_element (RmainElement0
element_damage_scale (	RelementDamageScale
damage_rate (	R
damageRate(
skill_crit_level (	RskillCritLevel&
skill_crit_rate (	RskillCritRate@
target_affect_obj (2.PB.SKILL_RANGE_TYPERtargetAffectObj0
target_affect_radius (	RtargetAffectRadius0
target_affect_lenght (	RtargetAffectLenght.
target_affect_angle (	RtargetAffectAngle(
max_target_count (RmaxTargetCount>
target_affect_dt (2.PB.SKILL_DELAY_TYPERtargetAffectDt(
target_affect_dl (RtargetAffectDl(
target_affect_fs (RtargetAffectFsD
caster_auto_turn! (2.PB.SKILL_CASTER_AUTO_TURNRcasterAutoTurnC
need_target_dir" (2.PB.PERFORM_TARGET_HURT_DIRRneedTargetDir;
control_type# (2.PB.PERFORM_CONTROL_TYPERcontrolType,
is_zhuatou_control$ (RisZhuatouControl(
is_force_control% (RisForceControl6
can_add_in_immue_control& (RcanAddInImmueControl!
control_rate' (	RcontrolRate!
control_time( (RcontrolTime(
control_dir_type) (RcontrolDirType*
control_move_type* (RcontrolMoveType5
suspend_param+ (2.PB.SuspendParamRsuspendParam/
	move_type, (2.PB.SKILLMOVE_TYPERmoveType$
move_dis_scale- (RmoveDisScale$
move_dir_angle. (RmoveDirAngle 
move_dis_min/ (	R
moveDisMin 
move_dis_max0 (	R
moveDisMax
min_gap1 (	RminGap*
move_ignore_hight2 (RmoveIgnoreHight.
move_ignore_unreach3 (RmoveIgnoreUnreachC
target_states5 (2.PB.SKILL_PERFORM_STATESB��RtargetStates?
self_states7 (2.PB.SKILL_PERFORM_STATESB��R
selfStates6
subobj9 (2.PB.SKILL_PERFORM_SUBOBJB��Rsubobj4
client_skillperform_id: (RclientSkillperformId*
pre_ref_params; (	B��RpreRefParams
bp_can_move< (R	bpCanMove"
bp_can_rotate= (RbpCanRotate
bp_can_jump> (R	bpCanJump)
bp_can_next_skill? (RbpCanNextSkill 
bp_can_dodge@ (R
bpCanDodgeF
bp_can_beinterruptA (2.PB.CAN_BEINTERRUPT_MASKRbpCanBeinterrupt,
bp_pending_executeB (RbpPendingExecute%
bp_branche_id_1C (RbpBrancheId12
bp_branche_action_id_1D (RbpBrancheActionId1%
bp_branche_id_2E (RbpBrancheId22
bp_branche_action_id_2F (RbpBrancheActionId2%
bp_branche_id_3G (RbpBrancheId32
bp_branche_action_id_3H (RbpBrancheActionId37
target_affect_start_timeI (RtargetAffectStartTime3
target_affect_end_timeJ (RtargetAffectEndTime3

sweep_typeK (2.PB.SKILL_SWEEP_TYPER	sweepType"
can_free_moveL (RcanFreeMove(
offsets_x_lengthM (	RoffsetsXLength(
offsets_y_lengthN (	RoffsetsYLength(
offsets_z_lengthO (	RoffsetsZLength'
broadwise_angleP (	RbroadwiseAngle#
endwise_angleQ (	RendwiseAngle'
force_intensityR (RforceIntensity%
ignore_gravityS (RignoreGravity$
damage_bk_rateT (	RdamageBkRate%
recharge_skillU (RrechargeSkill#

fire_limitV (	B��R	fireLimit
xp_addW (	RxpAdd(
move_damage_typeX (RmoveDamageType?
collision_typeY (2.PB.SKILL_COLLISION_TYPERcollisionType/
is_show_weapon_trailZ (RisShowWeaponTrail$
is_dodge_state[ (RisDodgeStateJ
ragdoll_force_type\ (2.PB.SKILL_RAGDOLL_FORCE_TYPERragdollForceType:$�������SkillPerformConfig��2"�
SkillSubobjPerform
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
type (2.PB.PERFORM_CLASSRtype

display_id (R	displayId%
quit_condition (	RquitCondition%
skip_condition (	RskipCondition8
damage_type	 (2.PB.PERFORM_DAMAGE_TYPER
damageType2
damage_skill_external
 (	RdamageSkillExternal0
scale_skill_external (	RscaleSkillExternal2
damage_skill_internal (	RdamageSkillInternal0
scale_skill_internal (	RscaleSkillInternal!
main_element (RmainElement0
element_damage_scale (	RelementDamageScale
damage_rate (	R
damageRate(
skill_crit_level (	RskillCritLevel&
skill_crit_rate (	RskillCritRateG
target_affect_obj (2.PB.SUBOBJ_SKILL_RANGE_TYPERtargetAffectObj0
target_affect_radius (	RtargetAffectRadius0
target_affect_lenght (	RtargetAffectLenght.
target_affect_angle (	RtargetAffectAngle(
max_target_count (RmaxTargetCount>
target_affect_dt (2.PB.SKILL_DELAY_TYPERtargetAffectDt(
target_affect_dl (RtargetAffectDl(
target_affect_fs (RtargetAffectFs-
hit_mask_judge_type (RhitMaskJudgeType
be_hit_mask (R	beHitMask
hit_mask (RhitMask&
need_target_dir (RneedTargetDir;
control_type (2.PB.PERFORM_CONTROL_TYPERcontrolType(
is_force_control  (RisForceControl6
can_add_in_immue_control! (RcanAddInImmueControl!
control_rate" (	RcontrolRate!
control_time# (RcontrolTime(
control_dir_type$ (RcontrolDirType*
control_move_type% (RcontrolMoveType5
suspend_param& (2.PB.SuspendParamRsuspendParam/
	move_type' (2.PB.SKILLMOVE_TYPERmoveType$
move_dis_scale( (RmoveDisScale 
move_dis_max) (R
moveDisMax
move_dir_ex* (R	moveDirEx
move_dis_ex+ (R	moveDisEx*
move_ignore_hight, (RmoveIgnoreHight.
move_ignore_unreach- (RmoveIgnoreUnreachC
target_states/ (2.PB.SKILL_PERFORM_STATESB��RtargetStates*
pre_ref_params1 (	B��RpreRefParams(
offsets_x_length2 (	RoffsetsXLength(
offsets_y_length3 (	RoffsetsYLength(
offsets_z_length4 (	RoffsetsZLength'
broadwise_angle5 (	RbroadwiseAngle#
endwise_angle6 (	RendwiseAngle$
damage_bk_rate7 (	RdamageBkRate
xp_add8 (	RxpAdd:$�������SkillSubobjPerform��2"�
SkillLimitConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
weapon_limit (RweaponLimit8
target_limit (2.PB.SKILL_TARGET_TYPERtargetLimit*
fight_state_limit (RfightStateLimitJ
change_shape_conditon (2.PB.SKILL_USECONDITIONRchangeShapeConditon*
ex_state_conditon	 (RexStateConditon$
pre_fire_state
 (RpreFireState(
limit_fire_state (RlimitFireState%
limit_lifeprof (RlimitLifeprof:"�������SkillLimitConfig��2"�
SkillEffectConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
execute_way (	R
executeWay
mask (Rmask:#�������SkillEffectConfig��2"�
SKILL_STATE_EX_CONDITIONL
condition_ex (2).PB.SKILL_STATE_EX_CONDITION.CONDITION_EXRconditionEx,
condition_ex_param (RconditionExParam(
condition_ex_max (RconditionExMax"�
CONDITION_EX
EFFECT_BEGIN 
EFFECT_DELAY

EFFECT_HIT
EFFECT_DODGE
EFFECT_BEHIT
EFFECT_BEINJURE
EFFECT_KILL
EFFECT_MISS
EFFECT_INJURE
EFFECT_STATE	
EFFECT_BEFORE_DEATH

EFFECT_LE_HP
EFFECT_GE_HP
EFFECT_LE_MP1
EFFECT_GE_MP1
EFFECT_LE_MP2
EFFECT_GE_MP2
EFFECT_LE_MP3
EFFECT_GE_MP3
EFFECT_LE_MP4
EFFECT_GE_MP4
EFFECT_KILL_NPC"a
SKILL_STATE_EFFECT(
state_effect_tid (RstateEffectTid!
effect_param (ReffectParam"q
SKILL_STATE_INURE_EX_CONDITION!
condition_ex (RconditionEx,
condition_ex_param (RconditionExParam"�
SkillStateConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
icon (Ricon
priority (Rpriority
glow (Rglow!
shape_change (RshapeChange!
color_change	 (RcolorChange-
type
 (2.PB.STATEFILTER_TYPE_MASKRtype*
level_lower_limit (RlevelLowerLimit*
level_upper_limit (RlevelUpperLimit'
character_limit (RcharacterLimit6
	time_type (2.PB.SKILL_FILTERTIME_TYPERtimeType)
probability_type (RprobabilityType*
attrib_state_anti (RattribStateAnti0
	condition (2.PB.SKILL_ADD_TYPER	condition
initial (RinitialE
ex_condition (2.PB.SKILL_STATE_EX_CONDITIONB��RexCondition4
effect (2.PB.SKILL_STATE_EFFECTB��ReffectV
inure_ex_condition (2".PB.SKILL_STATE_INURE_EX_CONDITIONB��RinureExCondition+
pile_up (2.PB.BUFF_POLY_RULERpileUp 
pile_num_max (R
pileNumMax
props (Rprops
name (	Rname%
desc_condition (	RdescCondition
desc_effect (	R
descEffect
gfx (Rgfx
material_id  (R
materialId:"�������SkillStateConfig��2"�	
SkillConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
mask_character (RmaskCharacter
	level_max (RlevelMax
class_id (RclassId
	source_id (RsourceId
	sync_type	 (RsyncType
	cancharge
 (R	cancharge%
must_broadcast (RmustBroadcast!
target_limit (RtargetLimit.
cancast_in_qinggong (RcancastInQinggong%
ignore_silence (RignoreSilence
cooldown_id (R
cooldownId0
cooldown_single_time (	RcooldownSingleTime
	min_range (RminRange
	max_range (RmaxRange
priority (Rpriority6
select_type (2.PB.SKILL_SELTAR_TYPER
selectType$
need_auto_move (RneedAutoMove#

process_id (B��R	processId
	action_id (RactionId&
client_skill_id (RclientSkillId!
qianyao_time (RqianyaoTime%
ignore_yingzhi (RignoreYingzhi$
is_dodge_skill (RisDodgeSkill%
dodge_distance (RdodgeDistance*
face_angel_offset (RfaceAngelOffset

skill_type (R	skillType0
intonate_limit_mp  (	B��RintonateLimitMp.
RM_consider_hitback! (RRMConsiderHitback;
is_motion_ignore_collision" (RisMotionIgnoreCollision:�������SkillConfig��2"�
ThrowerService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments:�������Npc��服务��0"�

GenerateData
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	scene_tag (RsceneTag'
gen_type (2.PB.GEN_TYPERgenType
	sub_index (RsubIndex
tid (Rtid
count	 (Rcount*
pos_type
 (2.PB.REGION_TYPERposType 
pos (2.PB.CoordinateRpos 
dir (2.PB.CoordinateRdir
radius (Rradius
strategy (Rstrategy
reborn (Rreborn!
reborn_times (RrebornTimes'
reborn_interval (RrebornInterval#
controller_id (RcontrollerId$
ai_script_path (	RaiScriptPath
board_id (RboardId
wave (Rwave#
init_strategy (RinitStrategy#
init_behavior (RinitBehavior
init_action (R
initAction!
init_locator (RinitLocator*
use_behavior_tree (RuseBehaviorTree
life_mstime (R
lifeMstime
name (	Rname
camp_id (RcampId
anim_1 (	Ranim1
anim_2 (	Ranim2

switchtime  (R
switchtime
speak_id! (RspeakId
	bezier_id" (RbezierId4
patrol_type# (2.PB.NPC_PATROL_TYPER
patrolType#
isfast_patrol$ (RisfastPatrol(
group_by% (	B��	scene_tagRgroupBy<
	ai_module& (2.PB.GenerateData.AIModuleB��
RaiModuleG
AIModule)
type (2.PB.AI_COMPONENT_TYPERtype
tid (Rtid:�������Scene��1"�

Controller
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
default_active (RdefaultActive
	wait_time (RwaitTime
	stop_time (RstopTime*
active_time_range (RactiveTimeRange#
repeat_active	 (RrepeatActive
	is_global
 (RisGlobal(
only_main_mirror (RonlyMainMirror-
start (2.PB.Controller.TimeNodeRstart+
stop (2.PB.Controller.TimeNodeRstop�
TimeNode
valid (Rvalid
min (Rmin
hour (Rhour 
day_of_month (R
dayOfMonth
month (Rmonth
year (Ryear
day_of_week (R	dayOfWeek:�������Config��1"�
AnimationCrossFade
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	anim_name (	RanimName&
type (2.PB.CROSSFADE_TYPERtype!
default_fade (RdefaultFadeJ

animations	 (2$.PB.AnimationCrossFade.FadeAnimationB��R
animations@
FadeAnimation
	anim_name (	RanimName
fade (Rfade:�������Abandon"�
GoAbroad
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments%
activity_index (RactivityIndex!
activity_tid (RactivityTid
level_limit (R
levelLimit+
reputation_ticket (RreputationTicket%
consume_ticket	 (RconsumeTicket/
common_use_limit_tid
 (RcommonUseLimitTid
	scene_tag (RsceneTag
x (Rx
z (Rz
radius (Rradius#
target_zoneid (RtargetZoneid#
exchange_rate (RexchangeRate$
area_id (2.PB.VersionRareaId
icon (Ricon$
max_player_num (RmaxPlayerNum
	area_name (	RareaName.
button_activity_tid (RbuttonActivityTid6
invitation_activity_tid (RinvitationActivityTid9
	instances (2.PB.GoAbroad.instanceB��R	instances)
aid_pill_item_tid (RaidPillItemTidV
instance
tid (Rtid
repu_id (RrepuId
daily_count (R
dailyCount:"�������Config��跨服��0"�
CreateRoleConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
prof (B��Rprof
skill (B��Rskill
task
 (B��Rtask6
equip (2.PB.CreateRoleConfig.EquipB��Requip#
delay_seconds (RdelaySeconds
	min_level (RminLevel
	max_level (RmaxLevel
fixed_level (R
fixedLevel

fixed_prof (R	fixedProf#

addon_grps (B��R	addonGrps
area_id (RareaId/
base_addon_grp_id (B��RbaseAddonGrpId!
Equip
tids (B��
Rtids:�������Config��0"�

AbroadArea
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments!
activity_tid (RactivityTid3
area_score_toplist_tid (RareaScoreToplistTid5
player_kill_toplist_tid (RplayerKillToplistTid/
area_chat_repu_count (RareaChatRepuCount1
world_chat_repu_count	 (RworldChatRepuCount0
kickout_activity_tid
 (RkickoutActivityTid0
max_player_huanchong (RmaxPlayerHuanchong:"�������Config��跨服��0"�
DiamondExchangeRepu
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments&
diamond_is_bind (RdiamondIsBind#
reputation_id (RreputationId#
exchange_rate (RexchangeRate!
activity_tid (RactivityTid:�������Abandon"�
TestLocalization
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsA
	languages (2.PB.TestLocalization.LanguageB��
R	languages4
Language
name (	Rname
value (	Rvalue:�������Abandon"�
SkillGfx
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
path_id (RpathId
time (Rtime
ref_bone (	RrefBone$
is_auto_rotate (RisAutoRotate(
is_follow_action	 (RisFollowAction(
is_follow_player
 (RisFollowPlayer"
is_target_pos (RisTargetPos"
is_anim_scope (RisAnimScope
priority (Rpriority&
offset (2.PB.CoordinateRoffset*
rotation (2.PB.CoordinateRrotation 
type (2.PB.GFX_TYPERtype
	fly_speed (RflySpeed
fly_time (RflyTime$
explode_gfx_id (RexplodeGfxId$
explode_sfx_id (RexplodeSfxId
scale_x (RscaleX
scale_y (RscaleY
scale_z (RscaleZ$
interruptable (Rinterruptable<
play_time_type (2.PB.GFX_PLAY_TIME_TYPERplayTimeType:�������SkillGfx��2"�
ClientSkillPerformConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments

die_effect (R	dieEffect
hitgfx (Rhitgfx
hitsfx (	Rhitsfx
gfx_ids (B��RgfxIds
sfx_ids	 (B��RsfxIds?
client_move_type
 (2.PB.CLIENT_SKILL_MOVERclientMoveType&
move_begin_time (RmoveBeginTime"
move_end_time (RmoveEndTime+
move_up_time_ratio (RmoveUpTimeRatio$
move_up_height (RmoveUpHeight%
animation_name (	RanimationName3
hitAnimType (2.PB.HIT_ANIM_TYPERhitAnimType*
slow_motion_start (RslowMotionStart0
slow_motion_interval (RslowMotionInterval(
slow_motion_rate (RslowMotionRate%
forewarn_delay (RforewarnDelay+
forewarn_duration (RforewarnDuration%
forewarn_times (RforewarnTimes.
forewarn_process_id (RforewarnProcessId;
main_weapon_animation_name (	RmainWeaponAnimationName9
main_weapon_delay_seconds (RmainWeaponDelaySeconds?
second_weapon_animation_name  (	RsecondWeaponAnimationName=
second_weapon_delay_seconds! (RsecondWeaponDelaySeconds\
,main_weapon_animation_end_section_delay_time" (R&mainWeaponAnimationEndSectionDelayTime`
.second_weapon_animation_end_section_delay_time# (R(secondWeaponAnimationEndSectionDelayTime1
main_weapon_open_type$ (RmainWeaponOpenType5
second_weapon_open_type% (RsecondWeaponOpenType 
scar_path_id( (R
scarPathId#
scar_duration) (RscarDuration

scar_scale* (R	scarScale6
hit_camera_shake_path_id+ (RhitCameraShakePathIdC
main_weapon_glow_fade_in_enable2 (RmainWeaponGlowFadeInEnable?
main_weapon_glow_fade_in_time3 (RmainWeaponGlowFadeInTimeG
!main_weapon_glow_fade_in_duration4 (RmainWeaponGlowFadeInDurationI
"main_weapon_glow_fade_in_intensity5 (RmainWeaponGlowFadeInIntensityE
 main_weapon_glow_fade_out_enable6 (RmainWeaponGlowFadeOutEnableA
main_weapon_glow_fade_out_time7 (RmainWeaponGlowFadeOutTimeI
"main_weapon_glow_fade_out_duration8 (RmainWeaponGlowFadeOutDurationK
#main_weapon_glow_fade_out_intensity9 (RmainWeaponGlowFadeOutIntensityG
!second_weapon_glow_fade_in_enable< (RsecondWeaponGlowFadeInEnableC
second_weapon_glow_fade_in_time= (RsecondWeaponGlowFadeInTimeK
#second_weapon_glow_fade_in_duration> (RsecondWeaponGlowFadeInDurationM
$second_weapon_glow_fade_in_intensity? (RsecondWeaponGlowFadeInIntensityI
"second_weapon_glow_fade_out_enable@ (RsecondWeaponGlowFadeOutEnableE
 second_weapon_glow_fade_out_timeA (RsecondWeaponGlowFadeOutTimeM
$second_weapon_glow_fade_out_durationB (RsecondWeaponGlowFadeOutDurationO
%second_weapon_glow_fade_out_intensityC (R secondWeaponGlowFadeOutIntensity:*�������ClientSkillPerformConfig��2"�
ClientSkillConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
icon (Ricon

short_desc (	R	shortDesc

brief_desc (	R	briefDesc
detail_desc	 (	R
detailDesc
cg_id
 (RcgId:#�������ClientSkillConfig��2"�
TestHotUpdate
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	str_value (	RstrValue
	int_value (RintValue1

enum_value (2.PB.TEST_ENUM_TYPER	enumValue#

arr_values	 (B��R	arrValuesB

msg_values (2.PB.TestHotUpdate.TestMessageB��R	msgValuesz
TestMessage
	str_value (	RstrValue
	int_value (RintValue1

enum_value (2.PB.TEST_ENUM_TYPER	enumValue:�������Abandon"�
PlayerConstructionConf
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
mine_tid (RmineTid:�������Abandon"�
SkillStateEffect
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
gfx (Rgfx
material_id (R
materialId:"�������SkillStateEffect��2"�

ArrowTower
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
npc_tid (RnpcTid3
levels (2.PB.ArrowTower.Level6B��Rlevels�
SaoShe
interval (Rinterval
duration (Rduration
skill_id (RskillId
cooldown_id (R
cooldownId#
cooldown_time (RcooldownTime\
Level6#
transform_tid (RtransformTid-
saoshe (2.PB.ArrowTower.SaoSheRsaoshe:�������Npc��1"�
ArrowTowerService
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments:�������Npc��服务��0"�
	TestGroup
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	scene_tag (RsceneTag
npc_tid (RnpcTid(
group_by (	B��	scene_tagRgroupBy:�������Abandon"�
TaskExtraShuZhi
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname
	min_value (RminValue
	max_value (RmaxValue:�������Abandon"�
PhalanxConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments+
leader_substitute (RleaderSubstituteK
offset_position (2.PB.PhalanxConfig.POS_CONIFGB��	RoffsetPositionF

POS_CONIFG
x (Rx
z (Rz
	animation (	R	animation: �����	��Temp��阵法��0"�
SceneObject
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname

model_size (R	modelSize

file_model (R	fileModel
file_fx (RfileFx:�������Npc��1"�
AIModuleFleeConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
hp_below (RhpBelow"
enemy_in_sign (RenemyInSign
	flee_type (RfleeType)
flee_pos (2.PB.CoordinateRfleePos

flee_speed	 (R	fleeSpeed
buff_id
 (RbuffId
vanish (Rvanish0
vanish_effect_pathid (RvanishEffectPathid,
begin_flee_speakid (RbeginFleeSpeakid(
end_flee_speakid (RendFleeSpeakid(
begin_flee_voice (RbeginFleeVoice$
end_flee_voice (RendFleeVoice#
flee_distance (RfleeDistance:�������AiModule��1"�
AIModuleCastSkillConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
hp_skill_id (R	hpSkillId
hp_below (RhpBelow(
hp_skill_speakid (RhpSkillSpeakid-
hp_skill_delay_time (RhpSkillDelayTime$
birth_skill_id	 (RbirthSkillId.
birth_skill_speakid
 (RbirthSkillSpeakid*
birth_skill_delay (RbirthSkillDelay$
death_skill_id (RdeathSkillId.
death_skill_speakid (RdeathSkillSpeakid:�������AiModule��1"�
AIModuleAskHelpConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
hp_below (RhpBelow
enemy_count (R
enemyCount
distance (Rdistance
speakid (Rspeakid
voice	 (Rvoice
	action_id
 (RactionId:�������AiModule��1"�
CarrierConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments5

bind_limit (2.PB.CARRIER_BIND_LIMITR	bindLimit4
body_component_npcs (B��RbodyComponentNpcs
	scene_tag (RsceneTag6
	body_type (2.PB.TEMP_UNREACHABLE_TYPERbodyType
	body_size	 (RbodySize
ai_path
 (	RaiPath$
bind_body_size (RbindBodySize$
create_on_born (RcreateOnBornL
component_npcs (2.PB.CarrierConfig.component_npcB��
RcomponentNpcs3
capsules (2.PB.CapsuleForUE4B��
Rcapsules�
component_npc
	scene_tag (RsceneTag
stage_id (RstageId
wave_id (RwaveId0
use_essence_property (RuseEssenceProperty
hp_ratio (RhpRatio!
attack_ratio (RattackRatio!
defend_ratio (RdefendRatio$
addon_group_id (RaddonGroupId'
not_add_to_group	 (RnotAddToGroup:�������Config��1"�
WeaponAnimConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments$
weapon_type_id (RweaponTypeId
stand (	Rstand
walk (	Rwalk
aim_walk (	RaimWalk
	aim_stand	 (	RaimStand
swim
 (	Rswim
	dash_stop (	RdashStop

jump_start (	R	jumpStart$
run_jump_start (	RrunJumpStart
	jump_loop (	RjumpLoop
jump_end (	RjumpEnd 
run_jump_end (	R
runJumpEnd#
takeup_weapon (	RtakeupWeapon'
takedown_weapon (	RtakedownWeapon
idle1 (	Ridle1
idle2 (	Ridle2
idle3 (	Ridle3:�������Config��0"�
WeaponSkillsConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsE
normal_state (2".PB.WeaponSkillsConfig.skill_groupRnormalStateE
afloat_state (2".PB.WeaponSkillsConfig.skill_groupRafloatState?
	run_state (2".PB.WeaponSkillsConfig.skill_groupRrunState$
weapon_anim_id (RweaponAnimId
xp	 (Rxp�
skill_group#
normal_attack (RnormalAttack!
heavy_attack (RheavyAttack

left_dodge (R	leftDodge
right_dodge (R
rightDodge!
normal_dodge (RnormalDodge

mult_dodge (R	multDodge0
normal_charge_attack (RnormalChargeAttack.
heavy_charge_attack (RheavyChargeAttack:�������SkillWeapon��0"�
OnlineRewardConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
	ac_second (RacSecond
	reward_id (RrewardId:�������Config��0"�
DropCondition
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments 
dc_max_level (R
dcMaxLevel 
dc_min_level (R
dcMinLevel
activity_id (R
activityId
player_prof (R
playerProf:�������DropCondition��2"�
DropClassConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
level (RlevelD
normal_drop (2.PB.DropClassConfig.DropInfoB���R
normalDropM
independent_drop (2.PB.DropClassConfig.DropInfoB��RindependentDrop�
DropInfo9
	drop_type (2.PB.DropClassConfig.DropTypeRdropType
drop_id (RdropId
drop_weight (R
dropWeight
check_count (R
checkCount
filter (Rfilter""
DropType
DT_ITEM 	
DT_DC:�������
DropConfig�� "�
GameCoinBag
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
name (	Rname3
common_prop (2.PB.ItemCommonPropR
commonProp
	min_value (RminValue
	max_value (RmaxValue
bind	 (Rbind&
cmn_uselimit_id
 (RcmnUselimitId:�������Item��2"�
NpcDropConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments<
dc_info (2.PB.NpcDropConfig.NpcDropInfoB��RdcInfo
rate (B��
Rrate
level (RlevelC
NpcDropInfo
dc_id (RdcId
check_count (R
checkCount:�������NpcDropConfig��2"�
OpenServerReward
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments=
reward (2.PB.OpenServerReward.OpenRewardB��RrewardT

OpenReward'
interval_second (RintervalSecond

reward_tid (R	rewardTid:�������Config��0"�
DayActivityReward
index (Rindex%
version (2.PB.VersionRversion
comments (	RcommentsF
reward_info (2.PB.DayActivityReward.DayRewardB��R
rewardInfoK
	DayReward!
day_activity (RdayActivity
	reward_id (RrewardId:�������Config��0"�
LoginDayConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments6
info (2.PB.LoginDayConfig.LoginInfoB��RinfoE
	LoginInfo
	day_count (RdayCount
	reward_id (RrewardId:�������Config��0"�
VipLevelConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
seq_num (RseqNum*
data_correct_type (RdataCorrectType#

level_info (B��R	levelInfo:�������Config��0"�
RechargeRewardConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
r_type (RrType%
recharge_count (RrechargeCount
	reward_id (RrewardId
activity_id (R
activityId:�������Config��0"�
ConsumeRewardConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
r_type (RrType%
recharge_count (RrechargeCount
	reward_id (RrewardId
activity_id (R
activityId:�������Config��0"�
DayFundConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
r_index (RrIndex3
days (2.PB.DayFundConfig.DayFundB��Rdays!
activity_tid (RactivityTid
	cash_need (RcashNeed!
first_reward	 (RfirstReward5
DayFund
days (Rdays
reward (Rreward:�������Config��0"�
AccumulateRechargeConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments
type (RtypeE
info (2+.PB.AccumulateRechargeConfig.AccumuRechargeB��Rinfo�
AccumuRecharge
recharge (Rrecharge
	day_count (RdayCount
	reward_id (RrewardId
activity_id (R
activityId#
activity_days (RactivityDays:�������Config��0"�
SpeakConfig
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments9
contents (2.PB.SpeakConfig.ContentB��Rcontents
channel (Rchannel
period (Rperiod

repeattime (R
repeattime
priority	 (Rpriority
soundid
 (Rsoundid-

speak_type (2.PB.SPEAK_TYPER	speakType
fxuiid (Rfxuiid$

appendChns (B��R
appendChns+
music (2.PB.SpeakConfig.MusicRmusicQ
Content
text (	Rtext
title (	Rtitle
	showimage (	R	showimage
Music
id (Rid:�������Config��0"�
GameWorldGPS
index (Rindex%
version (2.PB.VersionRversion
comments (	Rcomments4
target_type (2.PB.GPS_TARGET_TYPER
targetType

target_tid (R	targetTid%
specified_icon (RspecifiedIcon
	pos_count (RposCount&
pos	 (2.PB.CoordinateB��Rpos:�������GPS��GPS*[
TASK_SELECT_BRANCH
TSB_INVALID 
TSK_SELECT_1
TSK_SELECT_2
TSK_SELECT_3*F
TASK_SELECT_STATE
TSS_INVLAID 
TSS_IN_STATE

TSS_NOT_IN*�
TASK_CONDITION
INVALID 
GAME_MASTER����
NULL_TARGET����#
GENDER
������GENDER_MASK$
PROF������PROF_TYPE_ENUM*
MARRIED������TASK_SELECT_STATE
	VIP_LEVEL��	��
ACCOUNT_LEVEL��	��

PROF_LEVEL��	��
PLAYER_LEVEL��	��%
FACTION������FACTION_ENUM
ACTIVITY����0
IS_IN_FACTION������TASK_SELECT_STATE
	PLAY_TIME2
NOT_HAVE_TASKc��
����
	HAVE_TASKd������:
TASK_FINISH_COUNTe#��	����TaskFinishRange�� ��
	DROP_TASKf������>
GET_COMMON_REWARDg'������config.CommonReward����:

ITEM_COUNTh*��	����
TaskAnyTID��IDSpace.Item��

HAVE_MONEYj����
SCENEk����@
MULTI_TASK_FINISH_COUNTl#��	����TaskFinishRange�� ��#
CUR_TASK_FINISH_COUNTm��	��.
CUR_REPUTATIONr��	����
TaskAnyTID��
DEAD�������5
KILL_NPC�&������NPC.MonsterEssence����#
KILL_LEVEL_RANGE���	����(
KILL_SELF_LEVEL_RANGE���	����2
IS_TEAM_FOLLOW�������TASK_SELECT_STATE(
T_CLAN_REPU���	����
TaskAnyTID,
ALL_SERVER_REPU���	����
TaskAnyTID
	TEAM_SIZE�����.

IS_CAPTAIN�������TASK_SELECT_STATE
PK_LEVEL�����(
SKILL_LEVEL���	����
TaskAnyTID
	FAMILY_ID�����1
CUR_SERVICE_NPC�������NPC.NPC_ESSENCE

HAVE_TITLE�����
FACTION_LVL�����
FACTION_ROLE�����
FACTION_TYPE�����+

BUFF_LEVEL���	����
TaskAnyTID��
NATION�����8
INSTANCE_TID�%������config.InstanceConfig��3
IN_INSTANCE�!������config.InstanceConfig

EMPTY_PACK�����;
FINISH_INSTANCE�%������config.InstanceConfig��3
TRANSFORM_STATE�������TASK_SELECT_STATE0
HAVE_CHARIOT�������TASK_SELECT_STATE9
USE_ITEM�*������
TaskAnyTID��IDSpace.Item�� 
	USE_SKILL���������3
DIG_MINE�$������NPC.Mine.task_in����
ESCORT_QUALITY���	��
TASK_LOGOUT�����

TASK_LOGIN�����?
TASK_LEAVE_INSTANCE�%������config.InstanceConfig�� 
TASK_CHANGE_SCENE�����0
COMBAT_STATE�������TASK_SELECT_STATE'

BUFF_PARAM���	����
TaskAnyTID

ROUND_AREA�����)
	RECT_AREA�������RectAreaParam
	TIME_COST���	��
TIME_COST_ONLINE���	��"
CONVOY_FINISHED�������
CUR_TASK_ID���

RANDOM�����
TASK_USED_TIME���	��2
SELECT_BRANCH�������TASK_SELECT_BRANCH*�
TASK_REWARD
NONE 
EXP����RewardExpParam#
MONEY����RewardMoneyParam!
ITEM����RewardItemParam
TR_REPUTATION��
MAIL��
TASK��-
CLEAR_TASK_CNT����TaskAnyTIDOne�� 
CHAT��
CALL	��
SPEAK��
TR_BUFF��!
TRANSFER����TransferPos+
ADD_TASK_CNT����TaskAnyTIDOne�� )
RANDOM_TASK����RandomTaskObject'
	DEC_MONEY����RewardMoneyParam%
DEC_ITEM����RewardItemParam$
TASK_LIB����config.TaskLib&

CREATE_NPC����CreateNPCParam&

REMOVE_NPC����RemoveNPCParam.
COMMON_REWARD����config.CommonReward
CG��3
CREATE_CONVOY_NPC����CreateConvoyNPCParam*D
	TASK_TYPE
NORMAL_TASK 
ACHIEVEMENT	
SCORE

GAMBLE*B
ACHIEVEMENT_TYPE
NOT_ACHIEVEMENT 	
DAILY

WHOLE_LIFE*
	SHOW_TYPE
SHOW 
HIDE*U
TASK_QUALITY
TQ_WHITE 
TQ_GREEN
TQ_BLUE
	TQ_PURPLE
	TQ_ORANGE*1
TASK_CHOICE_REPU_OP
TCRO_ADD 
TCRO_DEC*�
EXP_GOTO_SHOP_TYPE
EXP_GOTO_SHOP_NONE 
EXP_GOTO_SHOP_BAITAN
EXP_GOTO_SHOP_SHANGHANG
EXP_GOTO_SHOP_PAIMAI
EXP_GOTO_SHOP_REPU*�
CharmGradeType
CGT_NONE 
CGT_AMBASSADOR
CGT_QI_ZHI_MENGZHU
CGT_HUO_LI_MENGZHU
CGT_CHUN_QING_MENGZHU
CGT_END*X
ActivationType
UNKNOWN_ActivationType 
AT_SEVEN
AT_MONTH

AT_FOREVER*b
NPCType
UNKNOWN_NPCType 
NT_TYPE_REMOTE
NT_TYPE_SHORT_RANGE
NT_TYPE_TREATMENT*d
WelfareType
UNKNOWN_WelfareType 
WT_CORPS_CONTRIBUTION
WT_MONEY
WT_BIND_DIAMOND*}
	EventType
UNKNOWN_EventType 
ET_GREETING_DAY
ET_FRIEND_AMITY
ET_ACTIVE_DAY
ET_TASK
ET_INSTANCE*_
REPU_CLEAR_TYPE
REPU_CLEAR_TYPE_NEVER 
REPU_CLEAR_TYPE_DAY
REPU_CLEAR_TYPE_WEEK*]
GRANT_GENERAL_REWARD_TYPE%
!UNKNOWN_GRANT_GENERAL_REWARD_TYPE 
GENERAL_REWARD_CLANS�*H
CARRIER_BIND_LIMIT
CBL_INVALID 

CBL_PLAYER
CBL_NPC��:R

ConfigType.google.protobuf.MessageOptions�� (2.PB.CONFIG_TYPER
ConfigType:;
IDSpace.google.protobuf.MessageOptions�� (	RIDSpace:S
	Directory.google.protobuf.MessageOptions�� (2.PB.DIRECTORY_TYPER	Directory:9
Origin.google.protobuf.MessageOptions�� (	ROrigin:=
DirGroup.google.protobuf.MessageOptions�� (	RDirGroup:I
CreateDirLevel.google.protobuf.MessageOptions�� (	RCreateDirLevel:A

TempOrigin.google.protobuf.MessageOptions�� (	R
TempOrigin:>

PrefixCode.google.protobuf.EnumOptions�� (	R
PrefixCode:X
EnumDirectory.google.protobuf.EnumOptions�� (2.PB.DIRECTORY_TYPEREnumDirectoryB
task.pbHbproto3