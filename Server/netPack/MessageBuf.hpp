#ifndef __MESSAGEBUF_H__
#define __MESSAGEBUF_H__

#include "commDef.hpp"

//��ͷ
DEFINE_STRUCT_2(NetHead, int, len, std::string, body);


// NetHead header;
// header.len = sn.GetSize() + sizeof(int);
// sn >> header.body;
// 
// BinaryWriteStream send;
// header.Pack(send);


//��������
DEFINE_MSG_5(RequestLogin, 1, 
	std::string, _userName, 
	std::string, _userPassword,
	std::string, _userGUID,
	std::string, _deviceToken, 
	int, _reConnect);

//�����Ӧ
DEFINE_MSG_3(RequestLoginRet, 1, 
	std::string, _userName, 
	std::string, _userGUID, 
	int, _flag);

//ping��Ϣ
DEFINE_MSG_1(RequestPing, 1, 
	std::string, vlaue);

//��ɫ�б�
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

//ѡ���ɫ
DEFINE_MSG_1(GCAskSelectRole, 1, std::string, _roleName);

//������Ϣ��ʾ
DEFINE_MSG_4(NetErrorMessage, 1, int, protocolId, int, strLength, int, errorCode, std::string, errorMessage);

//�ı䳡��
DEFINE_MSG_6(GSNotifyChangeScene, 1,
	int, m_un32MapID,
	int, m_un32SceneID,
	int, m_un32ClientNO,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//��ɫ������Ұ
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


//��ɫ�뿪��Ұ
DEFINE_MSG_2(GSObjectDisappear, 1,
	int, ObjNum,
	std::vector<int>, m_un32ObjID);

//GCAskLogout

//��������ƶ�
DEFINE_MSG_8(GSObjectMove, 1,
	int, m_un32ObjID,
	float, m_fCurPosX,
	float, m_fCurPosY,
	float, m_fCurPosZ,
	float, m_fTarPosX,
	float, m_fTarPosY,
	float, m_fTarPosZ,
	float, m_fMoveSpeed );

//�����ƶ�
DEFINE_MSG_8(GCAskMove, 1,
	int, m_un32ObjID,
	float, m_fCurPosX,
	float, m_fCurPosY,
	float, m_fCurPosZ,
	int, node_num,
	float, m_fTarPosX,
	float, m_fTarPosY,
	float, m_fTarPosZ);

//���������ۣ��ȼ��仯
DEFINE_MSG_4(GSNotifyRoleProfileChange, 1,
	int, m_un32ObjID,
	int, m_un32WeaponTypeID,
	int, m_un32CoatTypeID,
	int, m_un32Level);

//�������
DEFINE_MSG_8(GCReportPassGate, 1,
	int, m_un32MapID,
	int, m_un32SceneID,
	int, m_un32HPVessel,
	int, m_un32MPVessel,
	int, m_un32GotExpNum,
	int, m_un32GotSilverNum,
	int, m_un32UseSecond,
	int, m_bPickAllTempGoods);

//���󿪱���
DEFINE_MSG_2(GCAskTreasureBoxDropGoods, 1,
	int, m_un32BoxID,
	int, m_isShowAward);

//������볡��
DEFINE_MSG_7(GCAskEnterScene, 1,
	int, m_n32MapID,
	int, m_n32SceneID,
	int, m_bEnterByUI,
	int, m_un32HPVessel,
	int, m_un32MPVessel,
	int, m_un32GotExp,
	int, m_un32GotSilver);

//�����Ѿ�ͨ�صĵ�ͼ�б�TODO
DEFINE_MSG_2(GSNotifyRoleMapScheduleList, 1,
	int, lineNum,
	int, mapID);

//����������/�ȼ��仯TODO

//�ʲ�֪ͨTODO

//�ʲ��ȼ�TODO

//����TODO

//Ѫ��/����TODO

//����TODO

//���󸴻�
DEFINE_MSG_3(GCAskRelive,1,
	int, _reliveType,
	int, _assetType,
	int, m_un32AssetNum);

//GCReportExp
DEFINE_MSG_1(GCReportExp, 1,
	int, m_un32GotExp);

//ʹ�ü���֪ͨ
DEFINE_MSG_8(GCReportUseSkill, 1,
	int, m_un32ObjID,
	int, m_un32SkillID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);


//�������ʹ�ü���
DEFINE_MSG_8(GSNotifySkillReleased, 1,
	int, m_un32ObjectID,
	int, m_un32SkillID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//�������
DEFINE_MSG_8(GCReportOBjectAppear, 1,
	int, m_un32ObjID,
	int, m_un32TempID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//�յ��������
DEFINE_MSG_8(GSNotifyObjectAppear, 1,
	int, m_un32ObjID,
	int, m_un32TempID,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ);

//��������
DEFINE_MSG_2(GCReportOBjectDisappear, 1,
	int, m_un32ObjID,
	int, m_n32Reason);

//�յ�����
DEFINE_MSG_2(GSNotifyObjectDisappear, 1,
	int, m_un32ObjID,
	int, m_n32Reason);

//������Ϊ
DEFINE_MSG_8(GCReportObjectAction, 1,
	int, m_un32ObjID,
	int, m_n32ActionCate,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//�㲥������Ϊ
DEFINE_MSG_8(GSNotifyObjectAction, 1,
	int, m_un32ObjID,
	int, m_n32ActionCate,
	float, m_fDirX,
	float, m_fDirY,
	float, m_fDirZ,
	float, m_fPosX,
	float, m_fPosY,
	float, m_fPosZ);

//�Թ����˺�
DEFINE_MSG_5(GCReportObjectHurm, 1,
	int, m_bHost,
	int, m_un32ObjID,
	int, m_n32HPValue,
	int, m_n32CurHP,
	int, m_n32Effect);

//�㲥��������˺� ����ֻ��������Ҫ����
DEFINE_MSG_5(GSNotifyObjectHurm, 1,
	int, m_bHost,
	int, m_un32ObjID,
	int, m_n32HPValue,
	int, m_n32HP,
	int, m_n32Effect);

//��������� 45
DEFINE_MSG_2(GCAskEnterTowerInstance, 1,
	int, m_un32TowerId,
	int, m_bCurWave);

//�㱨�ɼ� 46
DEFINE_MSG_1(GCReportTowerInstanceScore, 1,
	int, m_un32TowerId);

//���� 47
DEFINE_MSG_1(GCAskTowerInstanceRank, 1,
	int, m_un16Type);

//��Ʒ
DEFINE_MSG_2(GCAskTowerInstanceAward, 1,
	int, m_u32TowerID,
	int, m_u16TowerType);

//�����ʼ��б�

//�����Ķ��ʼ�
DEFINE_MSG_1(GCAskReadEMail, 1,
	int, m_un32EMailID);

//�����ʼ���Ʒ
DEFINE_MSG_1(GCAskGetEMailPrize, 1,
	int, m_un32EMailID);

//����ɾ���ʼ�
DEFINE_MSG_1(GCAskRemoveEMail, 1,
	int, m_un32EMailID);

//֪ͨ�����б�
DEFINE_MSG_4(GCAskTowerInstanceAward1, 1,
	int, m_n8RankCate,
	int, m_n32SelfRank,
	int, m_n32SelfRankValue,
	int, m_n32RankNum);

//֪ͨ�ʼ��б�TODO
DEFINE_MSG_1(GSNotifyEMailList, 1,
	int, m_un32EMailNum);

//���ʹ�����ɫ��ϢTODO
DEFINE_MSG_4(GCSendCreateRole, 1,
	int, _u32VocationID,
	int, _byteGender,
	std::string, _nickName,
	int, _byteVerify);

//���ʹ�����ɫ��Ϣ
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

//����������Ϣ
DEFINE_MSG_2(GCReportProcuce, 1,
	int, _bookID,
	int, _bUseDiamond);

//�����½ڽ���
DEFINE_MSG_2(GCAskChapterAward, 1,
	int, _chapterNum,
	int, _hardChapter);

//����ѫ����Ϣ

//����ѫ������

enum FriendSendType
{
	UNKONW = 0,
	///<summary>
	///1:��Ӻ���
	///</summary>
	AddFriend = 1,
	///<summary>
	///2:�ܾ�����
	///</summary>
	Not = 2,
	///<summary>
	///3:ͬ���������
	///</summary>
	Ok = 3,
	///<summary>
	///4:ɾ������
	///</summary>
	DeleteFriend = 4,
	///<summary>
	///5:���ͺ�������.
	///</summary>
	SendAward = 5,
	///<summary>
	///.6:���պ�������
	///</summary>
	ReceiveAward = 6,
};

//�������ѹ���108
DEFINE_MSG_2(GCAskAddFriend, 1,
	int, _FriendOpt,
	std::string, _roleName);

//����VIP����109
DEFINE_MSG_1(GCVipFriend, 1,
	int, _FriendOpt);

//��������б�110

//�����ѯ���
DEFINE_MSG_1(GCAskSelectPlayer, 1,
	std::string, _byte);

//��������������ɱ���
DEFINE_MSG_2(GCAskGuideComplate, 1,
	int, GroupId,
	int, u8Finish);

#endif