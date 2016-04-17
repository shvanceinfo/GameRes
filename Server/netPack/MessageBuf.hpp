#ifndef __MESSAGEBUF_H__
#define __MESSAGEBUF_H__

#include "commDef.hpp"

//包头
DEFINE_STRUCT_2(NetHead, int, len, std::string, body);


// NetHead header;
// header.len = sn.GetSize() + sizeof(int);
// sn >> header.body;
// 
// BinaryWriteStream send;
// header.Pack(send);


//登入请求
DEFINE_MSG_5(RequestLogin, 1, 
	std::string, _userName, 
	std::string, _userPassword,
	std::string, _userGUID,
	std::string, _deviceToken, 
	int, _reConnect);

//登入回应
DEFINE_MSG_3(RequestLoginRet, 1, 
	std::string, _userName, 
	std::string, _userGUID, 
	int, _flag);

//ping消息
DEFINE_MSG_1(RequestPing, 1, 
	std::string, vlaue);

//角色列表
DEFINE_MSG_22(GSNotifyRoleBaseInfo, 1,
	int, m_un32ObjID,
	std::string, m_n32RoleNickName,
	int, m_n32CareerID,
	int, m_bGender,
	int, m_un32CoatTypeID,
	int, m_un32LegGuardId,
	int, m_un32ShoeId,
	int, m_un32NecklaceId,
	int, m_un32RingId,
	int, m_un32WeaponId,
	int, m_un32Level,
	int, m_un32Exp,
	int, m_un32CurHP,
	int, m_un32CurMP,
	int, m_un32CurHPVessel,
	int, m_un32CurMPVessel,
	int, m_un32MaxHP,
	int, m_un32MaxMP,
	int, m_un32MaxHPVessel,
	int, m_un32MaxMPVessel,
	int, m_un32Max_packages,
	int, wingID
	);

//选择角色
DEFINE_MSG_1(GCAskSelectRole, 1, std::string, _roleName);

//错误消息提示
DEFINE_MSG_4(NetErrorMessage, 1, int, protocolId, int, strLength, int, errorCode, std::string, errorMessage);

//改变场景
DEFINE_MSG_6(GSNotifyChangeScene, 1,
	int, m_un32MapID,
	int, m_un32SceneID,
	int, m_un32ClientNO,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//角色进入视野
DEFINE_MSG_19(GSNotifyRoleAppear, 1,
	int, m_un32ObjID,
	std::string, m_n32RoleNickName,
	int, m_n32CareerID,
	int, m_bGender,
	int, m_un32WeaponTypeID,
	int, m_un32CoatTypeID,
	int, m_un32Level,
	float, m_fCurPosX,
	float, m_fCurPosY,
	float, m_fCurPosZ,
	float, m_fTarPosX,
	float, m_fTarPosY,
	float, m_fTarPosZ,
	float, m_fMoveSpeed,
	int, m_un32CurTitleID,
	int, m_un32CurHp,
	int, m_u32WingID,
	int, m_u32MedalID,
	int, m_u32PetID);


//角色离开视野
DEFINE_MSG_2(GSObjectDisappear, 1,
	int, ObjNum,
	std::vector<int>, m_un32ObjID);

//GCAskLogout

//其它玩家移动
DEFINE_MSG_8(GSObjectMove, 1,
	int, m_un32ObjID,
	float, m_fCurPosX,
	float, m_fCurPosY,
	float, m_fCurPosZ,
	float, m_fTarPosX,
	float, m_fTarPosY,
	float, m_fTarPosZ,
	float, m_fMoveSpeed );

//请求移动
DEFINE_MSG_8(GCAskMove, 1,
	int, m_un32ObjID,
	float, m_fCurPosX,
	float, m_fCurPosY,
	float, m_fCurPosZ,
	int, node_num,
	float, m_fTarPosX,
	float, m_fTarPosY,
	float, m_fTarPosZ);

//其它玩家外观，等级变化
DEFINE_MSG_4(GSNotifyRoleProfileChange, 1,
	int, m_un32ObjID,
	int, m_un32WeaponTypeID,
	int, m_un32CoatTypeID,
	int, m_un32Level);

//请求过关
DEFINE_MSG_8(GCReportPassGate, 1,
	int, m_un32MapID,
	int, m_un32SceneID,
	int, m_un32HPVessel,
	int, m_un32MPVessel,
	int, m_un32GotExpNum,
	int, m_un32GotSilverNum,
	int, m_un32UseSecond,
	int, m_bPickAllTempGoods);

//请求开宝箱
DEFINE_MSG_2(GCAskTreasureBoxDropGoods, 1,
	int, m_un32BoxID,
	int, m_isShowAward);

//请求进入场景
DEFINE_MSG_7(GCAskEnterScene, 1,
	int, m_n32MapID,
	int, m_n32SceneID,
	int, m_bEnterByUI,
	int, m_un32HPVessel,
	int, m_un32MPVessel,
	int, m_un32GotExp,
	int, m_un32GotSilver);

//解析已经通关的地图列表TODO
DEFINE_MSG_2(GSNotifyRoleMapScheduleList, 1,
	int, lineNum,
	int, mapID);

//其它玩家外观/等级变化TODO

//资产通知TODO

//资产等级TODO

//经验TODO

//血量/经验TODO

//经验TODO

//请求复活
DEFINE_MSG_3(GCAskRelive,1,
	int, _reliveType,
	int, _assetType,
	int, m_un32AssetNum);

//GCReportExp
DEFINE_MSG_1(GCReportExp, 1,
	int, m_un32GotExp);

//使用技能通知
DEFINE_MSG_8(GCReportUseSkill, 1,
	int, m_un32ObjID,
	int, m_un32SkillID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);


//其它玩家使用技能
DEFINE_MSG_8(GSNotifySkillReleased, 1,
	int, m_un32ObjectID,
	int, m_un32SkillID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//对象出现
DEFINE_MSG_8(GCReportOBjectAppear, 1,
	int, m_un32ObjID,
	int, m_un32TempID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//收到对象出现
DEFINE_MSG_8(GSNotifyObjectAppear, 1,
	int, m_un32ObjID,
	int, m_un32TempID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//对象消亡
DEFINE_MSG_2(GCReportOBjectDisappear, 1,
	int, m_un32ObjID,
	int, m_n32Reason);

//收到对象
DEFINE_MSG_2(GSNotifyObjectDisappear, 1,
	int, m_un32ObjID,
	int, m_n32Reason);

//对象行为
DEFINE_MSG_8(GCReportObjectAction, 1,
	int, m_un32ObjID,
	int, m_n32ActionCate,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//广播对象行为
DEFINE_MSG_8(GSNotifyObjectAction, 1,
	int, m_un32ObjID,
	int, m_n32ActionCate,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//对怪物伤害
DEFINE_MSG_5(GCReportObjectHurm, 1,
	int, m_bHost,
	int, m_un32ObjID,
	int, m_n32HPValue,
	int, m_n32CurHP,
	int, m_n32Effect);

//广播对象怪物伤害 但是只有主机需要处理
DEFINE_MSG_5(GSNotifyObjectHurm, 1,
	int, m_bHost,
	int, m_un32ObjID,
	int, m_n32HPValue,
	int, m_n32HP,
	int, m_n32Effect);

//请求进入塔 45
DEFINE_MSG_2(GCAskEnterTowerInstance, 1,
	int, m_un32TowerId,
	int, m_bCurWave);

//汇报成绩 46
DEFINE_MSG_1(GCReportTowerInstanceScore, 1,
	int, m_un32TowerId);

//排名 47
DEFINE_MSG_1(GCAskTowerInstanceRank, 1,
	int, m_un16Type);

//奖品
DEFINE_MSG_2(GCAskTowerInstanceAward, 1,
	int, m_u32TowerID,
	int, m_u16TowerType);

//请求邮件列表

//请求阅读邮件
DEFINE_MSG_1(GCAskReadEMail, 1,
	int, m_un32EMailID);

//请求邮件奖品
DEFINE_MSG_1(GCAskGetEMailPrize, 1,
	int, m_un32EMailID);

//请求删除邮件
DEFINE_MSG_1(GCAskRemoveEMail, 1,
	int, m_un32EMailID);

//通知排行列表
DEFINE_MSG_4(GCAskTowerInstanceAward1, 1,
	int, m_n8RankCate,
	int, m_n32SelfRank,
	int, m_n32SelfRankValue,
	int, m_n32RankNum);

//通知邮件列表TODO
DEFINE_MSG_1(GSNotifyEMailList, 1,
	int, m_un32EMailNum);

//发送创建角色消息TODO
DEFINE_MSG_4(GCSendCreateRole, 1,
	int, _u32VocationID,
	int, _byteGender,
	std::string, _nickName,
	int, _byteVerify);

//发送创建角色消息
DEFINE_MSG_1(GCAskRankList, 1,
	int, m_n8RankCate);

//GCAsChargeIOS
DEFINE_MSG_3(GCAsChargeIOS, 1,
	int, m_bIsDebug,
	int, m_strLen,
	std::string, m_szAppleReceipt);

//GCNotifyChargeIOS
DEFINE_MSG_3(GCNotifyChargeIOS, 1,
	int, m_nResult,
	int, m_strLen,
	std::string, m_szAppleReceipt);

//发送制作消息
DEFINE_MSG_2(GCReportProcuce, 1,
	int, _bookID,
	int, _bUseDiamond);

//请求章节奖励
DEFINE_MSG_2(GCAskChapterAward, 1,
	int, _chapterNum,
	int, _hardChapter);

//请求勋章信息

//请求勋章升级

enum FriendSendType
{
	UNKONW = 0,
	///<summary>
	///1:添加好友
	///</summary>
	AddFriend = 1,
	///<summary>
	///2:拒绝好友
	///</summary>
	Not = 2,
	///<summary>
	///3:同意好友申请
	///</summary>
	Ok = 3,
	///<summary>
	///4:删除好友
	///</summary>
	DeleteFriend = 4,
	///<summary>
	///5:赠送好友体力.
	///</summary>
	SendAward = 5,
	///<summary>
	///.6:接收好友体力
	///</summary>
	ReceiveAward = 6,
};

//基本好友功能108
DEFINE_MSG_2(GCAskAddFriend, 1,
	int, _FriendOpt,
	std::string, _roleName);

//好友VIP功能109
DEFINE_MSG_1(GCVipFriend, 1,
	int, _FriendOpt);

//请求好友列表110

//请求查询玩家
DEFINE_MSG_1(GCAskSelectPlayer, 1,
	std::string, _byte);

//请求新手引导完成保存
DEFINE_MSG_2(GCAskGuideComplate, 1,
	int, GroupId,
	int, u8Finish);

#endif