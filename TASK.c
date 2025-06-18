
#include "MAIN.h"
// #include "tgmath.h"

#define DEFAULT 0
#define SENSORCHECK 1
#define RUN 2
#define ERROR 3

#define OBSMAXVOL 35			// 35V报过压故障
#define OBGMAXVOL 35			// 35V报过压故障
#define VCUMAXVOL 420			//{390
#define OBSAIMCUR 1.85			// 目标是单侧2A
#define OBGAIMCUR 1.85			// 目标是单侧2A
#define OBSMAXCUR OBSAIMCUR + 1 // OBSAIMCUR + 1   //3A报过流故障
#define OBGMAXCUR OBGAIMCUR + 1 // OBGAIMCUR + 1   //3A报过流故障
#define VCU_PumpEnable IO_P8_1	// 水泵控制
#define VCU_FanEnable IO_P8_2	// 风扇控制

#define VCUMAXTEMP 100

#define ERROR_CNT 10
#define FRAMEDROPCNT 10

#define DISABLE_DCAC 0
#define ENABLE_DCAC 1

#define DISABLE_DCDC 4
#define ENABLE_DCDC 8

#define DISABLE_CHG 1 //{No charge output
#define ENABLE_CHG 0  //{Charge with current output
#define ENABLE_HEAT 2 //{Heat battery with OBC
#define FINISH_CHG 3  //{Finish charge

#define FAULT_NUM 48

ubyte EnableFlag = 0;
ubyte TaskState = 0;
ubyte FBLFlag = 0;
ubyte ErrorFlag = 0;
uword ErrorCode = 0;

float LvDcMeaFilted = 0;

sword OBG_PwmRef = 0;
sword OBS_PwmRef = 0;
sword OBG_PwmStep = 0;
sword OBS_PwmStep = 0;

float VolRefRamp = 0;
float OBG_VolRef = 0;
float OBS_VolRef = 0;
float OBG_CurRef = 0;
float OBS_CurRef = 0;

float KP_Vol = 0;
float KI_Vol = 0;
float OBG_VolKI = 0;
float OBS_VolKI = 0;

float KP_Cur = 0;
float KI_Cur = 0;
float OBS_CurKI = 0;
float OBG_CurKI = 0;

float OBS_CurOutput = 0;
float OBS_CurOutputPrev = 0;
float OBG_CurOutput = 0;
float OBC_CurMeaFilted = 0;
float DCAC_CurReq = 0;
float DCDC_CurReq = 0;

float OBS_VolInput = 0;
float OBG_VolInput = 0;
float VCU_VolOutput = 0;

float NTC_TempMea0Filted = 0;
float NTC_TempMea1Filted = 0;
float KEY_ON = 0; //[]//LYMG--22.11.15
float NTC_TempMax = 0;
float NTC_TempMin = 0;

int KEY_ON_cnt = 0;
int KEY_OFF_cnt = 0;

//{044 VCU_COMMAND
ubyte VCU_LifeTimeBMS = 0;
ubyte VCU_RunCmdBMS = 0;
ubyte VCU_HeatingEnableBMS = 0;
ubyte VCU_HVPowerEnableBMS = 0;
ubyte KEY_ON_EN = 0;
//}044

//{052
ubyte VCU_Status01DropCntBMS = 0;
ubyte VCU_CellInfoDropCntBMS = 0;
ubyte VCU_CurInfoDropCntBMS = 0;
ubyte VCU_Status02DropCntBMS = 0;
ubyte VCU_FaultInfoDropCntBMS = 0;
ubyte VCU_TempInfoDropCntBMS = 0;
ubyte VCU_NorminalInfoDropCntBMS = 0;
ubyte VCU_Status02DropCntBMS_OBC = 0;

//}052

//{045
ubyte BMS_HVPowerAllow = 0;
ubyte BMS_HVPowerLoopStatus = 0;
ubyte BMS_HeatingRequest = 0;
ubyte BMS_HeatingLoopStatus = 0;
float BMS_VolOutput = 0;
float BMS_CurOutput = 0;
float BMS3_DCBusVoltage = 0;
float BMS_CapChg2Full = 0;
ubyte BMS_SOC = 0;
ubyte BMS_DCChgStatus = 0;
ubyte BMS_fault_code = 0;

ubyte ISG_ChargeEnable = 0;
ubyte ChgPos_ConState = 0;
ubyte ISG_LiftTime = 0;
ubyte ISG_fault_code = 0;
ubyte ISG_run_state = 0;

//}045
//{046
uword BMS_CellVolMin = 3500;
ubyte BMS_CellVolMinID = 0;
ubyte BMS_CellVolMinModule = 0;
uword BMS_CellVolMax = 0;
ubyte BMS_CellVolMaxID = 0;
ubyte BMS_CellVolMaxModule = 0;
uword test_tpp01 = 0;
uword BMS_CellVolAvg = 0;
//}046
//{047
float BMS_PusleDischgCurAllowed = 0;
float BMS_PulseChgCurAllowed = 0;
float BMS_ContDischgCurAllowed = 0;
float BMS_ContChgCurAllowed = 0;
//}047
//{048
uword BMS_InsResPos = 0;
uword BMS_InsResNeg = 0;
ubyte BMS_PosRelayStatus = 0;
ubyte BMS_NegRelayStatus = 0;
ubyte BMS_PrechgRelayStaus = 0;
ubyte BMS_DCchgRelayStaus = 0;
ubyte BMS_SOCMinCanUse = 0;
ubyte BMS_SOH = 0;
//}048
//{049
ubyte BMS_FaultLevel = 0;
ubyte BMS_Code1 = 0;
ubyte BMS_Code2 = 0;
ubyte BMS_Code3 = 0;
ubyte BMS_Code4 = 0;
ubyte BMS_Code5 = 0;
//}049
//{050
ubyte BMS_TempAvg = 0;
ubyte BMS_TempMin = 0;
ubyte BMS_TempMinID = 0;
ubyte BMS_TempMinModule = 0;
ubyte BMS_TempMax = 0;
ubyte BMS_TempMaxID = 0;
ubyte BMS_TempMaxModule = 0;
//}050
//{051
uword BMS_CellVolMaxAllowed = 0;
uword BMS_CellVolMinAllowed = 0;
float BMS_CapacityNorminal = 0;
float BMS_VolNorminal = 0;
//}051
//{053
ubyte EF_Status01LostBMS = 0;
ubyte EF_CellInfoLostBMS = 0;
ubyte EF_CurInfoLostBMS = 0;
ubyte EF_Status02LostBMS = 0;
ubyte EF_FaultInfoLostBMS = 0;
ubyte EF_TempInfoLostBMS = 0;
ubyte EF_NorminalInfoLostBMS = 0;
ubyte EF_NodeLostBMS = 0;
//}053

float VCU_VolRefAllowedOBS = 0;
float VCU_CurRefAllowedOBS = 0;
float VCU_VolRefAllowedOBG = 0;
float VCU_CurRefAllowedOBG = 0;

float VCU_VolRefAllowedOBC = 0;
float VCU_CurRefAllowedOBC = 0;
ubyte VCU_LifeTimeOBC = 0;

ubyte VCU_ChgCmd = DISABLE_CHG;	   //{Need to confirm with supplier about default value
ubyte VCU_ChgCmdOBC = DISABLE_CHG; //{Need to confirm with supplier about default value
ubyte VCU_ChgCmdOBG = DISABLE_CHG; //{Need to confirm with supplier about default value
ubyte VCU_ChgCmdOBS = DISABLE_CHG; //{Need to confirm with supplier about default value

ubyte VCU_Status01DropCntOBC = 0;
ubyte VCU_Status02DropCntOBC = 0;
ubyte VCU_Status03DropCntOBC = 0;

float OBC_VolOutput = 0;
float OBC_CurOutput = 0;
float OBC_TempModule = 0;
float OBC_VolACInput = 0;
float OBC_VolPFC = 0;
float OBC_VolBat = 0;

float OBC_CurLimit = 0;
float OBG_CurLimit = 0;
float OBS_CurLimit = 0;

ubyte OBG_FaultPwm = 0;
ubyte OBS_FaultPwm = 0;

ubyte OBC_FaultCode = 0;
float OBC_CurACInput = 0;
float OBC_CurOutputPileAllowed = 0;
float OBC_TempCB = 0;
ubyte OBC_CCResStatus = 0;
float OBC_CurMax = 0;
ubyte OBC_CC_Status = 0;
ubyte OBC_CP_Status = 0;

ubyte EF_Status01LostOBC = 0;
ubyte EF_Status02LostOBC = 0;
ubyte EF_Status03LostOBC = 0;
ubyte EF_NodeLostOBC = 0;
ubyte LostCntOBC = 0;

ubyte VCU_EnableDCAC = DISABLE_DCAC; //{默认关闭

ubyte VCU_StatusDropCntDCAC = 0;

float DCAC_CurInput = 0;
float DCAC_TempModule = 0;

ubyte EF_StatusLostDCAC = 0;
ubyte EF_NodeLostDCAC = 0;

ubyte EF_StatusLostGUI = 0;
ubyte EF_NodeLostGUI = 0;
ubyte VCU_StatusDropCntGUI = 0;
ubyte EF_StatusLostISG = 0;
ubyte VCU_StatusDropCntISG = 0;

ubyte VCU_LifeTimeDCDC = 0;
ubyte VCU_EnableDCDC = DISABLE_DCDC; //{默认不打开
ubyte VCU_SBOperationDCDC = 0;
ubyte VCU_StatusDropCntDCDC = 0;

ubyte DCDC_RunStatus = 0;
ubyte DCDC_FaultStatus = 0;

float DCDC_TempModule = 0;
float DCDC_VolOutput = 0;
float DCDC_CurOutput = 0;
ubyte DCDC_FaultCode = 0;
ubyte DCDC_CANversion = 0;

ubyte DCDC_PowerOutput = 0;

ubyte EF_StatusLostDCDC = 0;
ubyte EF_NodeLostDCDC = 0;
ubyte EF_NodeLostISG = 0;

float OBC_SysStatus = 0;
ubyte OBG_SysStatus = 0;
ubyte OBS_SysStatus = 0;
ubyte DCAC_TempCapOBG = 0;
ubyte DCAC_TempCapOBS = 0;
ubyte DCAC_SysStatus = 0;
ubyte DCAC_HandSwitch = 0; //{为1时执行下电流程
ubyte DCDC_SysStatus = 0;

ubyte VCU_ChgStatusBMS = 0; // 0:高压未闭合；1:为高压闭合  2:无     3:满充状态待机状态 循环充电
ubyte VCU_EmptyFlagBMS_class01 = 0;
ubyte VCU_EmptyFlagBMS_class02 = 0;

float VCU_PowComp = 0;

// ubyte VCU_FanEnable = 1;
// ubyte VCU_PumpEnable = 1;

float VCU_TempMax = 0;
float VolRefBoost = 0;

ubyte VCU_PWMDebug = 0;
ubyte VCU_PWMEnable = 0;
ubyte VCU_HandSwitchGUI = 0;

ubyte MS10Cnt = 0;
ubyte MS20Cnt = 0;
ubyte MS100Cnt = 0;
ubyte MS200Cnt = 0;
ubyte MS500Cnt = 0;
ubyte MS1000Cnt = 0;
uword MS5000Cnt = 0;

CAN_SWObj RXObj;
ubyte TXData[8];

ubyte FaultMatrix[FAULT_NUM] = {0};

ubyte LYQ_dcdc_Data_tx[8];
ubyte LYQ_DCAC_Data_tx[8];
ubyte LYQ_obc_Data01_tx[8];
ubyte LYQ_obc_Data02_tx[8];
ubyte LYQ_obc_Data03_tx[8];

int engine_BOOST_OFF_cnt = 0;
int solar_BOOST_OFF_cnt = 0;
int engine_BOOST_ON = 0;
int solar_BOOST_ON = 0;

float NTCTB[39] = {183402, 139680, 107381, 83297, 65175, 51419, 40889, 32763, 26444, 21493, 17586, 14482, 11999, 10000, 8355.2, 7004.1, 5891.3, 4972.5, 4211.6, 3579.7, 3053.3, 2613.3, 2244.5, 1934.4, 1672.8, 1451.3, 1263.4, 1103.3, 966.49, 849.29, 748.56, 661.73, 586.65, 521.56, 464.96, 415.62, 372.48, 334.66, 301.43};
float NTCTM[39] = {-40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150};

float lim(float v, float up, float down)
{
	if (v > up)
	{
		v = up;
	}
	if (v < down)
	{
		v = down;
	}
	return v;
}

void delay(void)
{
	volatile int i, j, k;
	for (i = 0; i < 5000; i++)
	{
		for (j = 0; j < 500; j++)
		{
			k = i + j;
		}
	}
}

float adcon(float adVal)
{
	ubyte i;
	float temp;
	if (adVal > NTCTB[0])
	{
		temp = NTCTM[0] - 1;
	}
	else if (adVal < NTCTB[38])
	{
		temp = NTCTM[38] + 1;
	}
	else
	{
		for (i = 0; i < 38; ++i)
		{
			if (_CHECKRANGE(adVal, NTCTB[i + 1], NTCTB[i]))
			{
				temp = NTCTM[i] + 5 * (adVal - NTCTB[i]) / (NTCTB[i + 1] - NTCTB[i]);
				break;
			}
		}
	}
	return (temp);
}

void vars_clear_01(void)
{
	OBG_VolKI = 0;
	OBS_VolKI = 0;
	OBG_CurKI = 0;
	OBS_CurKI = 0;
	OBG_PwmRef = 0;
	OBS_PwmRef = 0;

	VCU_CurRefAllowedOBC = 0;
	VCU_CurRefAllowedOBG = 0;
	VCU_CurRefAllowedOBS = 0;
	VCU_VolRefAllowedOBC = 0;
	VCU_VolRefAllowedOBG = 0;
	VCU_VolRefAllowedOBS = 0;

	VCU_EnableDCAC = DISABLE_DCAC;
	VCU_EnableDCDC = DISABLE_DCDC;
	VCU_ChgCmdOBC = FINISH_CHG;
	VCU_ChgCmdOBG = DISABLE_CHG;
	VCU_ChgCmdOBS = DISABLE_CHG;
	VCU_ChgStatusBMS = 0;

	VCU_HVPowerEnableBMS = 2;
}

void vars_clear_02(void) // 关闭升压功能
{
	OBG_VolKI = 0;
	OBS_VolKI = 0;
	OBG_CurKI = 0;
	OBS_CurKI = 0;
	OBG_PwmRef = 0;
	OBS_PwmRef = 0;

	VCU_CurRefAllowedOBG = 0;
	VCU_CurRefAllowedOBS = 0;
	VCU_VolRefAllowedOBC = 0;
	VCU_VolRefAllowedOBG = 0;
	VCU_VolRefAllowedOBS = 0;

	VCU_ChgCmdOBG = DISABLE_CHG;
	VCU_ChgCmdOBS = DISABLE_CHG;
}

void bms_rx(void)
{
	//{BMS报文接收
	if (MS20Cnt >= 2)
	{
		//{BMS_Status01
		if (CAN_ubNewData(9))
		{
			CAN_vGetMsgObj(9, &RXObj);
			CAN_vReleaseObj(9);
			BMS_HVPowerAllow = RXObj.ubData[0] & 0x03;
			BMS_HVPowerLoopStatus = (RXObj.ubData[0] >> 2) & 0x01;
			BMS_HeatingRequest = (RXObj.ubData[0] >> 3) & 0x01;
			BMS_DCChgStatus = (RXObj.ubData[0] >> 5) & 0x03;
			BMS_VolOutput = (RXObj.ubData[2] * 256 + RXObj.ubData[1]) * 0.1;
			BMS_CurOutput = (RXObj.ubData[4] * 256 + RXObj.ubData[3]) * 0.1 - 600;
			BMS_CapChg2Full = (RXObj.ubData[6] * 256 + RXObj.ubData[5]) * 0.1;
			BMS_SOC = RXObj.ubData[7];
			VCU_Status01DropCntBMS = 0;
		}
		else
		{
			if (VCU_Status01DropCntBMS <= FRAMEDROPCNT)
			{
				VCU_Status01DropCntBMS++;
			}
			else
			{
				EF_Status01LostBMS = 1;
			}
		}
		//}BMS_Status01
	}

	if (MS100Cnt >= 10)
	{
		//{BMS_CellInfo
		if (CAN_ubNewData(10))
		{
			CAN_vGetMsgObj(10, &RXObj);
			CAN_vReleaseObj(10);
			BMS_CellVolAvg = (RXObj.ubData[1] & 0x0F) * 256 + RXObj.ubData[0];
			BMS_CellVolMin = (((RXObj.ubData[2] >> 4) & 0x0F) * 256) + ((RXObj.ubData[2] & 0x0F) * 16) + ((RXObj.ubData[1] >> 4) & 0x0F);
			test_tpp01 = BMS_CellVolMin;
			BMS_CellVolMinID = RXObj.ubData[3];
			BMS_CellVolMax = (RXObj.ubData[5] & 0x0F) * 256 + RXObj.ubData[4];
			BMS_CellVolMaxID = ((RXObj.ubData[6] & 0x0F) << 4) | ((RXObj.ubData[5] >> 4) & 0x0F);
			VCU_CellInfoDropCntBMS = 0;
		}
		else
		{
			if (VCU_CellInfoDropCntBMS <= FRAMEDROPCNT)
			{
				VCU_CellInfoDropCntBMS++;
			}
			else
			{
				EF_CellInfoLostBMS = 1;
			}
		}
		//}BMS_CellInfo

		//{BMS_CurInfo
		if (CAN_ubNewData(11))
		{
			CAN_vGetMsgObj(11, &RXObj);
			CAN_vReleaseObj(11);
			BMS_PusleDischgCurAllowed = (RXObj.ubData[1] * 256 + RXObj.ubData[0]) * 0.1;
			BMS_PulseChgCurAllowed = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 0.1;
			BMS_ContDischgCurAllowed = (RXObj.ubData[5] * 256 + RXObj.ubData[4]) * 0.1;
			BMS_ContChgCurAllowed = (RXObj.ubData[7] * 256 + RXObj.ubData[6]) * 0.1;
			VCU_CurInfoDropCntBMS = 0;
		}
		else
		{
			if (VCU_CurInfoDropCntBMS <= FRAMEDROPCNT)
			{
				VCU_CurInfoDropCntBMS++;
			}
			else
			{
				EF_CurInfoLostBMS = 1;
			}
		}
		//}BMS_CurInfo

		//{BMS_Status02
		if (CAN_ubNewData(12))
		{
			CAN_vGetMsgObj(12, &RXObj);
			CAN_vReleaseObj(12);
			BMS_InsResPos = (RXObj.ubData[1] * 256 + RXObj.ubData[0]) * 10;
			BMS_InsResNeg = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 10;
			BMS_PosRelayStatus = RXObj.ubData[4] & 0x01;
			BMS_NegRelayStatus = (RXObj.ubData[4] >> 1) & 0x01;
			if ((BMS_PosRelayStatus == 1) && (BMS_NegRelayStatus == 1))
			{
				BMS_HVPowerLoopStatus = 1;
			}
			else
				BMS_HVPowerLoopStatus = 0;
			BMS_PrechgRelayStaus = (RXObj.ubData[4] >> 2) & 0x01;
			BMS_DCchgRelayStaus = (RXObj.ubData[4] >> 2) & 0x01;
			BMS_HeatingLoopStatus = (RXObj.ubData[4] >> 4) & 0x01;
			BMS_SOCMinCanUse = RXObj.ubData[5];
			BMS_SOH = RXObj.ubData[6];
			VCU_Status02DropCntBMS = 0;
		}
		else
		{
			if (VCU_Status02DropCntBMS <= FRAMEDROPCNT)
			{
				VCU_Status02DropCntBMS++;
			}
			else
			{
				EF_Status02LostBMS = 1;
			}
		}
		//}BMS_Status02
	}

	if (MS200Cnt >= 20)
	{
		//{BMS_FaultInfo
		if (CAN_ubNewData(15))
		{
			CAN_vGetMsgObj(15, &RXObj);
			CAN_vReleaseObj(15);
			BMS_FaultLevel = RXObj.ubData[0] & 0x0F;
			BMS_Code1 = RXObj.ubData[1];
			BMS_Code2 = RXObj.ubData[2];
			BMS_Code3 = RXObj.ubData[3];
			BMS_Code4 = RXObj.ubData[4];
			BMS_Code5 = RXObj.ubData[5] & 0x7F;
			VCU_FaultInfoDropCntBMS = 0;
		}
		else
		{
			if (VCU_FaultInfoDropCntBMS <= FRAMEDROPCNT)
			{
				VCU_FaultInfoDropCntBMS++;
			}
			else
			{
				EF_FaultInfoLostBMS = 1;
			}
		}
		//}BMS_FaultInfo
	}

	if (MS1000Cnt >= 100)
	{
		//{BMS_TempInfo
		if (CAN_ubNewData(13))
		{
			CAN_vGetMsgObj(13, &RXObj);
			CAN_vReleaseObj(13);
			BMS_TempAvg = RXObj.ubData[0] - 40;
			BMS_TempMin = RXObj.ubData[1] - 40;
			BMS_TempMinID = RXObj.ubData[2];
			BMS_TempMinModule = RXObj.ubData[3];
			BMS_TempMax = RXObj.ubData[4] - 40;
			BMS_TempMaxID = RXObj.ubData[5];
			BMS_TempMaxModule = RXObj.ubData[6];
			VCU_TempInfoDropCntBMS = 0;
		}
		else
		{
			if (VCU_TempInfoDropCntBMS <= FRAMEDROPCNT)
			{
				VCU_TempInfoDropCntBMS++;
			}
			else
			{
				EF_TempInfoLostBMS = 1;
			}
		}
		//}BMS_TempInfo
	}

	if (MS5000Cnt >= 500)
	{
		//{BMS_NorminalInfo
		if (CAN_ubNewData(14))
		{
			CAN_vGetMsgObj(14, &RXObj);
			CAN_vReleaseObj(14);
			BMS_CellVolMaxAllowed = RXObj.ubData[1] * 256 + RXObj.ubData[0];
			BMS_CellVolMinAllowed = RXObj.ubData[3] * 256 + RXObj.ubData[2];
			BMS_CapacityNorminal = (RXObj.ubData[5] * 256 + RXObj.ubData[4]) * 0.1;
			BMS_VolNorminal = (RXObj.ubData[7] * 256 + RXObj.ubData[6]) * 0.1;
			VCU_NorminalInfoDropCntBMS = 0;
		}
		else
		{
			if (VCU_NorminalInfoDropCntBMS <= FRAMEDROPCNT)
			{
				VCU_NorminalInfoDropCntBMS++;
			}
			else
			{
				EF_NorminalInfoLostBMS = 1;
			}
		}
		//}BMS_NorminalInfo
	}
	//}BMS报文接收
}

void obc_rx(void)
{
	static ubyte ms30000Cnt = 0; //{充电枪拔枪后风扇水泵状态保持计数

	//{OBC报文接收
	if (MS500Cnt >= 50)
	{
		//{OBC_Status01--------------------接收0x18FF50E5报文
		if (CAN_ubNewData(19))
		{
			CAN_vGetMsgObj(19, &RXObj);
			CAN_vReleaseObj(19);
			OBC_VolOutput = (RXObj.ubData[1] * 256 + RXObj.ubData[0]) * 0.1;
			OBC_CurOutput = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 0.1;
			OBC_TempModule = RXObj.ubData[5] - 40;
			VCU_Status01DropCntOBC = 0;
			EF_Status01LostOBC = 0;
		}
		else
		{
			if (VCU_Status01DropCntOBC <= FRAMEDROPCNT)
			{
				VCU_Status01DropCntOBC++;
				ms30000Cnt = 0;
			}
			else
			{
				if (ms30000Cnt >= 120)
				{
					OBC_TempModule = 0; //{持续时间过后清零
				}
				else
				{
					ms30000Cnt++; //{保持拔枪后OBC最后的温度状态持续一段时间
				}
				OBC_VolOutput = 0;
				OBC_CurOutput = 0;
				EF_Status01LostOBC = 1;
			}
		}
		//}OBC_Status01

		//{OBC_Status02--------------------接收0x18FF50E6报文
		if (CAN_ubNewData(20))
		{
			CAN_vGetMsgObj(20, &RXObj);
			CAN_vReleaseObj(20);
			OBC_VolACInput = (RXObj.ubData[1] * 256 + RXObj.ubData[0]) * 0.1;
			OBC_VolPFC = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 0.1;
			OBC_CurLimit = RXObj.ubData[4] * 0.1;
			OBC_VolBat = (RXObj.ubData[6] * 256 + RXObj.ubData[5]) * 0.1;
			OBC_SysStatus = RXObj.ubData[7] & 0x0F; // 没有用
			OBC_FaultCode = (RXObj.ubData[7] >> 4) & 0x0F;
			VCU_Status02DropCntOBC = 0;
			EF_Status02LostOBC = 0;
		}
		else
		{
			OBC_CurLimit = 0;
			if (VCU_Status02DropCntOBC <= FRAMEDROPCNT)
			{
				VCU_Status02DropCntOBC++;
			}
			else
			{
				OBC_VolACInput = 0;
				OBC_VolPFC = 0;
				OBC_CurLimit = 0;
				OBC_VolBat = 0;
				OBC_SysStatus = 1; // 没有用
				OBC_FaultCode = 0;
				EF_Status02LostOBC = 1;
			}
		}
		//}OBC_Status02

		//{OBC_Status03--------------------接收0x18FF50E7报文
		if (CAN_ubNewData(21))
		{
			CAN_vGetMsgObj(21, &RXObj);
			CAN_vReleaseObj(21);
			OBC_CurACInput = ((RXObj.ubData[1] & 0x0F) * 256 + RXObj.ubData[0]) * 0.1;
			OBC_CurOutputPileAllowed = (((RXObj.ubData[2] * 256 + RXObj.ubData[1]) >> 4) & 0x0FFF) * 0.1;
			OBC_TempCB = RXObj.ubData[3] - 40;
			OBC_CCResStatus = RXObj.ubData[7] >> 5;
			if (_MAX(OBC_VolOutput, OBC_VolBat) > 0)
			{
				if (OBC_CCResStatus == 5)
				{
					OBC_CurMax = _MIN(6138.0 / _MAX(OBC_VolOutput, OBC_VolBat), OBC_CurOutputPileAllowed);
				}
				else if (OBC_CCResStatus == 4)
				{
					OBC_CurMax = _MIN(3069.0 / _MAX(OBC_VolOutput, OBC_VolBat), OBC_CurOutputPileAllowed);
				}
				else
				{
					OBC_CurMax = 0;
				}
			}
			else
			{
				OBC_CurMax = 0;
			}
			VCU_Status03DropCntOBC = 0;
			EF_Status03LostOBC = 0;
		}
		else
		{
			if (VCU_Status03DropCntOBC <= FRAMEDROPCNT)
			{
				VCU_Status03DropCntOBC++;
			}
			else
			{
				OBC_CurACInput = 0;
				OBC_CurOutputPileAllowed = 0;
				OBC_TempCB = 0;
				OBC_CCResStatus = 0;
				OBC_CurMax = 0;
				EF_Status03LostOBC = 1;
			}
		}
		//}OBC_Status03
	}
	//}OBC报文接收
}

void dcac_rx(void) //{在debug_rx()前执行
{
	if (MS100Cnt >= 10)
	{
		//{DCAC_Status--------------------接收0x04C80000报文
		if (CAN_ubNewData(23))
		{
			CAN_vGetMsgObj(23, &RXObj);
			CAN_vReleaseObj(23);
			DCAC_SysStatus = RXObj.ubData[0] & 0x0F;
			if (VCU_HandSwitchGUI == 0) // 如果接收到屏幕强制下高压的指令，不再接收DCAC传过来的开关状态信息
				DCAC_HandSwitch = (RXObj.ubData[0] >> 4) & 0x01;
			DCAC_TempModule = RXObj.ubData[1] - 50;
			DCAC_TempCapOBG = RXObj.ubData[2] - 50;
			DCAC_TempCapOBS = RXObj.ubData[3] - 50;
			VCU_StatusDropCntDCAC = 0;
			EF_StatusLostDCAC = 0;
		}
		else
		{
			if (VCU_StatusDropCntDCAC <= FRAMEDROPCNT)
			{
				VCU_StatusDropCntDCAC++;
			}
			else
			{
				EF_StatusLostDCAC = 1;
			}
		}
		//}DCAC_Status
	}
}

void dcdc_rx(void)
{
	if (MS100Cnt >= 10)
	{
		//{DCDC_Status--------------------接收0x1828272B报文
		if (CAN_ubNewData(17))
		{
			CAN_vGetMsgObj(17, &RXObj);
			CAN_vReleaseObj(17);
			DCDC_RunStatus = (RXObj.ubData[0] >> 1) & 0x03;
			DCDC_FaultStatus = (RXObj.ubData[0] >> 3) & 0x03;
			DCDC_SysStatus = (RXObj.ubData[0] >> 5) & 0x07; // 没有用
			DCDC_TempModule = RXObj.ubData[1] - 40;
			DCDC_VolOutput = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 0.05;
			DCDC_CurOutput = (RXObj.ubData[5] * 256 + RXObj.ubData[4]) * 0.05 - 1600;
			DCDC_FaultCode = RXObj.ubData[6];
			DCDC_CANversion = RXObj.ubData[7];
			VCU_StatusDropCntDCDC = 0;
			EF_StatusLostDCDC = 0;
		}
		else
		{
			if (VCU_StatusDropCntDCDC <= FRAMEDROPCNT)
			{
				VCU_StatusDropCntDCDC++;
			}
			else
			{
				EF_StatusLostDCDC = 1;
			}
		}
		//}DCDC_Status
	}
}

void debug_rx(void) //{在dcac_rx()后执行
{
	static uword tmp_last = 0;
	//--------------------接收0x04040000报文
	if (MS100Cnt >= 10)
	{
		if (CAN_ubNewData(0))
		{
			CAN_vGetMsgObj(0, &RXObj);
			CAN_vReleaseObj(0);
			EnableFlag = RXObj.ubData[0];
			// VolRefRamp = RXObj.ubData[1] * 0.01;
			VolRefBoost = (RXObj.ubData[3] * 256 + RXObj.ubData[2]) * 0.01;
			FBLFlag = RXObj.ubData[4];
			//	if ( SnapState == SNAP_WRITTEN )
			//		SnapState = RXObj.ubData[7];
		}

		// if ( CAN_ubNewData( 1 ) ) {
		//	CAN_vGetMsgObj( 1, &RXObj );
		//	CAN_vReleaseObj( 1 );
		//	KP_Vol = ( RXObj.ubData[1] * 256 + RXObj.ubData[0] ) * 0.001;	//{[0, 65.535] 0.001
		//	KI_Vol = ( RXObj.ubData[3] * 256 + RXObj.ubData[2] ) * 0.00001; //{[0, 0.65535] 0.00001
		//	KP_Cur = ( RXObj.ubData[5] * 256 + RXObj.ubData[4] ) * 0.001;	//{[0, 65.535] 0.001
		//	KI_Cur = ( RXObj.ubData[7] * 256 + RXObj.ubData[6] ) * 0.00001; //{[0, 0.65535] 0.00001
		// }

		if (CAN_ubNewData(25))
		{
			CAN_vGetMsgObj(25, &RXObj);
			CAN_vReleaseObj(25);
			if (RXObj.ubData[0])
			{
				VCU_HandSwitchGUI = 1;
				DCAC_HandSwitch = 1; //{接收到屏幕上强制下高压的指令
			}
			VCU_PWMEnable = RXObj.ubData[1] & 0x01; //{接收屏幕发过来的关DCAC输出指令<004(20201216)2.>
			VCU_StatusDropCntGUI = 0;
			EF_StatusLostGUI = 0;
		}
		else
		{
			if (VCU_StatusDropCntGUI <= FRAMEDROPCNT)
			{
				VCU_StatusDropCntGUI++;
			}
			else
			{
				EF_StatusLostGUI = 1;
			}
		}
	}
}

void isg_rx(void)
{
	if (MS100Cnt >= 10)
	{
		//{ISG_Status--------------------接收0x1601EFF4报文
		if (CAN_ubNewData(33))
		{
			CAN_vGetMsgObj(33, &RXObj);
			CAN_vReleaseObj(33);

			ISG_fault_code = RXObj.ubData[6];
			ISG_run_state = RXObj.ubData[7] & 0x03;
			EF_StatusLostISG = 0;
		}
		else
		{
			if (VCU_StatusDropCntISG <= FRAMEDROPCNT)
			{
				VCU_StatusDropCntISG++;
			}
			else
			{
				EF_StatusLostISG = 1;
			}
		}
		//}DCDC_Status
	}
}

void temp_sample(void)
{
	float tmp;

	ADC1_vStartSeq2ReqChNum(0, 0, 0, 12); // LYMG---22.11.15
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_12);
	tmp = tmp * 0.00732;			   //{30/4096
	KEY_ON = KEY_ON * 0.9 + tmp * 0.1; // 大于20V为启动发动机； 小于18V为关闭发动机
	// KEY_ON_cnt;KEY_OFF_cnt;
	if (KEY_ON > 20)
	{
		KEY_ON_cnt++;
		KEY_OFF_cnt = 0;
		if (KEY_ON_cnt > 100)
		{
			KEY_ON_EN = 1;
			KEY_ON_cnt = 101;
		}
	}
	if (KEY_ON < 18)
	{
		KEY_OFF_cnt++;
		KEY_ON_cnt = 0;
		if (KEY_OFF_cnt > 100)
		{
			KEY_ON_EN = 0;
			KEY_OFF_cnt = 101;
		}
	}

	ADC1_vStartSeq4ReqChNum(0, 0, 0, 14);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_14);
	tmp = tmp * 0.00122; //{5/4096
	tmp = adcon(tmp * 1000 / (5 - tmp));
	NTC_TempMea0Filted = NTC_TempMea0Filted * 0.9 + tmp * 0.1;

	ADC1_vStartSeq0ReqChNum(0, 0, 0, 15);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_15);
	tmp = tmp * 0.00122; //{5/4096
	tmp = adcon(tmp * 1000 / (5 - tmp));
	NTC_TempMea1Filted = NTC_TempMea1Filted * 0.9 + tmp * 0.1;

	NTC_TempMax = _MAX(NTC_TempMea0Filted, NTC_TempMea1Filted);
}

void volcur_sample(void)
{
	float tmp;

	ADC1_vStartSeq0ReqChNum(0, 0, 0, 1);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_1);
	tmp = tmp * 0.12400; //{500/4096
	VCU_VolOutput = VCU_VolOutput * 0.9 + tmp * 0.1;

	ADC0_vStartSeq0ReqChNum(0, 0, 0, 9);
	tmp = (float)ADC0_uwGetResultData(RESULT_REG_9);
	tmp = (2048 - tmp) * 0.0397; //{
	OBS_CurOutput = OBS_CurOutput * 0.9 + tmp * 0.1;

	ADC1_vStartSeq2ReqChNum(0, 0, 0, 9);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_9);
	tmp = (tmp - 2048) * 0.0397; //{
	OBG_CurOutput = OBG_CurOutput * 0.9 + tmp * 0.1;

	ADC0_vStartSeq2ReqChNum(0, 0, 0, 5);
	tmp = (float)ADC0_uwGetResultData(RESULT_REG_5);
	tmp = tmp * 0.01221; //{50/4096  P11
	OBG_VolInput = OBG_VolInput * 0.9 + tmp * 0.1;

	ADC1_vStartSeq4ReqChNum(0, 0, 0, 5);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_5);
	tmp = tmp * 0.01230; //{50/4096  P6
	OBS_VolInput = OBS_VolInput * 0.9 + tmp * 0.1;

	ADC1_vStartSeq0ReqChNum(0, 0, 0, 13);
	tmp = (float)ADC1_uwGetResultData(RESULT_REG_13);
	tmp = tmp * 0.01220; //{50/4096
	LvDcMeaFilted = LvDcMeaFilted * 0.9 + tmp * 0.1;
}

void error_check(void)
{
	static ubyte prevLostOBC = 0;
	static uword s60Cnt = 0;
	float tmpCurDCDC;
	float tmpPow45 = 0;

	//{OBS输入电压过高故障 62
	static ubyte obsmaxivCnt = 0;
	if (OBS_VolInput > OBSMAXVOL)
	{
		if (++obsmaxivCnt > ERROR_CNT)
		{
			// ErrorCode |= (uword)0x01 << 0;  //0X00000001
			// TaskState = ERROR;
		}
	}
	else
		obsmaxivCnt = 0;

	//{OBG输入电压过高故障 63
	static ubyte obgmaxivCnt = 0;
	if (OBG_VolInput > OBGMAXVOL)
	{
		if (++obgmaxivCnt > ERROR_CNT)
		{
			// ErrorCode |= (uword)0x01 << 1; //0X00000002
			// TaskState = ERROR;
		}
	}
	else
		obgmaxivCnt = 0;

	//{升压输出电压过高故障 64
	static ubyte vcumaxovCnt = 0;
	if (VCU_VolOutput > VCUMAXVOL)
	{
		if (++vcumaxovCnt > 5)
		{								   //{5个检测周期确认时间
			ErrorCode |= (uword)0x01 << 2; // 0X00000004
			TaskState = ERROR;
			IO_P6_1 = 1;
			vars_clear_02();
		}
	}
	else
		vcumaxovCnt = 0;

	//{OBS输出电流过大故障 65
	static ubyte obsmaxocCnt = 0;
	if (OBS_CurOutput > OBSMAXCUR)
	{
		if (++obsmaxocCnt > 50)
		{
			ErrorCode |= (uword)0x01 << 3; // 0X00000008
			TaskState = ERROR;
			VCU_CurRefAllowedOBS = 0;
		}
	}
	else
		obsmaxocCnt = 0;

	//{OBG输出电流过大故障 66
	static ubyte obgmaxocCnt = 0;
	if (OBG_CurOutput > OBGMAXCUR)
	{
		if (++obgmaxocCnt > 50)
		{
			ErrorCode |= (uword)0x01 << 4; // 0X00000010
			TaskState = ERROR;
			VCU_CurRefAllowedOBG = 0;
		}
	}
	else
		obgmaxocCnt = 0;

	//{OBS输出反向电流异常故障 67
	static ubyte obsnegcCnt = 0;
	if (OBS_CurOutput < -2.0)
	{
		if (++obsnegcCnt > ERROR_CNT)
		{
			// ErrorCode |= (uword)0x01 << 5;//0X00000020
			// TaskState = ERROR;
		}
	}
	else
		obsnegcCnt = 0;

	//{OBG输出反向电流异常故障 68
	static ubyte obgnegcCnt = 0;
	if (OBG_CurOutput < -2.0)
	{
		if (++obgnegcCnt > 50)
		{
			// ErrorCode |= (uword)0x01 << 6;//0X00000040
			// TaskState = ERROR;
		}
	}
	else
		obgnegcCnt = 0;

	//{NTC温度过高故障 69
	static ubyte ntcTMA = 0;
	if (VCU_TempMax > VCUMAXTEMP)
	{
		if (++ntcTMA > 5)
		{
			ErrorCode |= (uword)0x01 << 7; // 0X0000080
			TaskState = ERROR;
		}
	}
	else
		ntcTMA = 0;

	//{升压输出电压与BMS采集到的不匹配故障 70
	static ubyte vcubmsovCnt = 0;
	if (_ABS(VCU_VolOutput - BMS_VolOutput) > 30)
	{
		if (++vcumaxovCnt > 5)
		{ //{3个检测周期确认时间
		  // ErrorCode |= (uword)0x01 << 8;//0X0000100
		}
	}
	else
		vcubmsovCnt = 0;

	//{如果BMS所有CAN报文丢失,报BMS节点丢失故障,报文丢失故障清零  故障 71
	if (EF_Status01LostBMS && EF_CellInfoLostBMS && EF_CurInfoLostBMS && EF_Status02LostBMS && EF_FaultInfoLostBMS && EF_TempInfoLostBMS && EF_NorminalInfoLostBMS)
	{
		// ErrorCode |= (uword)0x01 << 9;       //0X0000200
		// EF_NodeLostBMS = 1;
	}
	else
	{
		EF_NodeLostBMS = 0;
	}

	//{OBC没有节点丢失的故障,只有CAN报文丢失的故障
	if (EF_Status01LostOBC && EF_Status02LostOBC && EF_Status03LostOBC)
	{
		EF_NodeLostOBC = 1;
	}
	else
	{
		EF_NodeLostOBC = 0;
	}

	//{E210 DCDC节点丢失的故障
	if (EF_StatusLostDCDC == 0)
	{
		//{小功率补偿
		if (DCDC_CurOutput < 6.0)
		{
			tmpPow45 = 0;
			if (VCU_ChgStatusBMS == 3)
			{
				if ((OBG_VolInput > 12.6) || (OBS_VolInput > 12.6))
				{
					if ((VCU_PWMEnable == 0) && (DCAC_SysStatus == 2))
					{
						tmpPow45 = 45;
					}
				}
			}
			//{20201211_002:只要DCAC出PWM波,就把小功率补偿中加一个45W
			if ((VCU_PWMEnable == 0) && (DCAC_SysStatus == 2))
			{
				tmpPow45 = 45;
			}
			//}20201211_002
			if (DCDC_CurOutput < 1.0)
			{
				tmpCurDCDC = _MAX((DCDC_CurOutput - 0.3), 0.0);
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.14 + tmpPow45;
			}
			else if (DCDC_CurOutput < 2.0)
			{
				tmpCurDCDC = DCDC_CurOutput - 0.1;
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.39 + tmpPow45;
			}
			else if (DCDC_CurOutput < 3.0)
			{
				tmpCurDCDC = DCDC_CurOutput - 0.1;
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.54 + tmpPow45;
			}
			else if (DCDC_CurOutput < 4.0)
			{
				tmpCurDCDC = DCDC_CurOutput - 0.1;
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.64 + tmpPow45;
			}
			else if (DCDC_CurOutput < 5.0)
			{
				tmpCurDCDC = DCDC_CurOutput - 0.1;
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.69 + tmpPow45;
			}
			else
			{
				tmpCurDCDC = DCDC_CurOutput - 0.1;
				VCU_PowComp = DCDC_VolOutput * tmpCurDCDC / 0.74 + tmpPow45;
			}
		}
		else
		{
			VCU_PowComp = 0;
		}
		//}小功率补偿

		EF_NodeLostDCDC = 0;
	}
	else
	{
		// ErrorCode |= (uword)0x01 << 10;  //故障 72 //0X0000400
		// EF_NodeLostDCDC = 1;
		// VCU_PowComp = 0;
	}

	if (EF_StatusLostDCAC)
	{
		// ErrorCode |= (uword)0x01 << 11;  //故障 73 //0X0000800
		// EF_NodeLostDCAC = 1;
	}
	else
	{
		EF_NodeLostDCAC = 0;
	}

	if (EF_StatusLostGUI)
	{
		EF_NodeLostGUI = 1;
		VCU_PWMEnable = 0;
	}
	else
	{
		EF_NodeLostGUI = 0;
	}

	if (BMS_FaultLevel >= 3)
	{
		ErrorCode |= (uword)0x01 << 11; // 0X0000800
		IO_P6_1 = 1;
		vars_clear_01();
	}

	//   	if ( ErrorCode > 0 ) {
	//		IO_P6_1 = 1;
	//		vars_clear_02();
	//	}
}

void obg_boost(void)
{
	static uword prevPwmRefOBG = 0;
	static uword pwmEnCnt = 0;
	static uword pwmDisCnt = 0;
	float volDet, curDet;
	float volKP, curKP;
	float pwmMax, pwmMin;
	float CurRef;
	uword tmp;

	pwmMin = 0;
	pwmMax = 250; // pwmMax = 300;   //  LYMG---22.6.16

	if ((VCU_ChgCmdOBG == ENABLE_CHG) && (ISG_run_state <= 1))
	{ //{恒流模式
		//{OBG出电流故障保护  //PWM失效保护
		if ((OBG_PwmRef > (pwmMax - 5)) && (OBG_CurOutput < _MAX((OBG_CurLimit - 0.5), 0)))
		{ // 0.2
			OBG_FaultPwm = 1;
		}
		if (OBG_FaultPwm == 1)
		{ //{PWM输出但是没有期望电流出来,关掉这一路不再工作,重启消除
			OBG_CurKI = 0;
			OBG_CurRef = 0;
			OBG_PwmRef = 0;
			prevPwmRefOBG = 0;
			VCU_CurRefAllowedOBG = 0;
		}
		//************************电流指令加梯度************************************//
		if (OBG_CurRef < VCU_CurRefAllowedOBG)
			OBG_CurRef = OBG_CurRef + 0.01; // _MIN( (OBG_CurRef + 0.01), VCU_CurRefAllowedOBG );//23.4.20//
		if (OBG_CurRef > VCU_CurRefAllowedOBG)
			OBG_CurRef = OBG_CurRef - 0.1; // _MAX( (OBG_CurRef - 0.02), VCU_CurRefAllowedOBG );//23.4.20 //
		if (VCU_CurRefAllowedOBG <= 0)
		{
			VCU_CurRefAllowedOBG = 0;
		}
		if (OBG_CurRef <= 0)
		{
			OBG_CurRef = 0;
		}
		//************************电流指令加梯度************************************//

		//************************OBG侧电流环处理************************************//
		curDet = OBG_CurRef - _MAX(OBG_CurOutput, 0.1);
		if (((curDet > 0) && (prevPwmRefOBG <= pwmMax)) || ((curDet < 0) && (prevPwmRefOBG > 0)))
		{
			curKP = curDet * 1;		   // KP_Cur;  //1;  //KP_Cur;  //0.1;
			OBG_CurKI += curDet * 0.1; // KI_Cur;  //0.001;  //KI_Cur;  //0.01;
			OBG_CurKI = _MID(OBG_CurKI, pwmMin, pwmMax);
			OBG_PwmRef = _MID(_ROUNDU(curKP + OBG_CurKI), pwmMin, pwmMax);
			prevPwmRefOBG = OBG_PwmRef;
		}
		//************************OBG侧电流环处理************************************//
		if ((VCU_CurRefAllowedOBG == 0) && (OBG_CurRef == 0)) // 无电流指令  快速关闭 PWM
		{
			OBG_CurKI = 0;
			OBG_PwmRef = OBG_PwmRef - 0.75;
			prevPwmRefOBG = 0;
			if (OBG_PwmRef < 1)
			{
				OBG_PwmRef = 0;
			}
		}

		if ((DCAC_HandSwitch == 1) || (VCU_ChgStatusBMS != 1))
		{
			OBG_CurKI = 0;
			OBG_CurRef = 0;
			OBG_PwmRef = 0;
			prevPwmRefOBG = 0;
		}
	}
	else
	{ //{关闭输出
		OBG_CurKI = 0;
		OBG_CurRef = 0;
		OBG_PwmRef = 0;
		prevPwmRefOBG = 0;
	}

	//************************PWM指令增加再次增加梯度************************************//
	if (OBG_PwmStep < OBG_PwmRef)
		OBG_PwmStep = _MIN((OBG_PwmStep + 20), OBG_PwmRef); // 20
	if (OBG_PwmStep > OBG_PwmRef)
		OBG_PwmStep = _MAX((OBG_PwmStep - 20), OBG_PwmRef); // 20

	// OBG_PwmStep = lim(OBG_PwmStep,0,250); //PWM做限幅处理
	//************************PWM指令增加再次增加梯度************************************//

	tmp = GPTA0_LTCCTR10;
	if ((GPTA0_LTCCTR10_RED == 1) && (GPTA0_LTCCTR10_FED == 1) && (GPTA0_LTCCTR10_CEN == 1) && (GPTA0_LTCCTR10_OSM == 0))
	{
		GPTA0_LTCXR11 = _PWMZ(OBG_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR11_OSM = 0;
		GPTA0_LTCCTR11_RED = 1;
		GPTA0_LTCCTR11_FED = 1;
		GPTA0_LTCCTR10_OSM = 1;
		tmp = GPTA0_LTCXR10;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR10)
		{
			GPTA0_LTCCTR10_RED = 0;
			GPTA0_LTCCTR10_FED = 0;
		}
	}
	else
	{
		GPTA0_LTCXR10 = _PWMZ(OBG_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR10_OSM = 0;
		GPTA0_LTCCTR10_RED = 1;
		GPTA0_LTCCTR10_FED = 1;
		GPTA0_LTCCTR11_OSM = 1;
		tmp = GPTA0_LTCXR11;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR11)
		{
			GPTA0_LTCCTR11_RED = 0;
			GPTA0_LTCCTR11_FED = 0;
		}
	}
	tmp = GPTA0_LTCCTR14;
	if ((GPTA0_LTCCTR14_RED == 1) && (GPTA0_LTCCTR14_FED == 1) && (GPTA0_LTCCTR14_CEN == 1) && (GPTA0_LTCCTR14_OSM == 0))
	{
		GPTA0_LTCXR15 = _PWMZ(OBG_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR15_OSM = 0;
		GPTA0_LTCCTR15_RED = 1;
		GPTA0_LTCCTR15_FED = 1;
		GPTA0_LTCCTR14_OSM = 1;
		tmp = GPTA0_LTCXR14;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR14)
		{
			GPTA0_LTCCTR14_RED = 0;
			GPTA0_LTCCTR14_FED = 0;
		}
	}
	else
	{
		GPTA0_LTCXR14 = _PWMZ(OBG_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR14_OSM = 0;
		GPTA0_LTCCTR14_RED = 1;
		GPTA0_LTCCTR14_FED = 1;
		GPTA0_LTCCTR15_OSM = 1;
		tmp = GPTA0_LTCXR15;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR15)
		{
			GPTA0_LTCCTR15_RED = 0;
			GPTA0_LTCCTR15_FED = 0;
		}
	}
}

void obs_boost(void)
{
	static uword prevPwmRefOBS = 0;
	static uword pwmEnCnt = 0;
	static uword pwmDisCnt = 0;
	float volDet, curDet;
	float volKP, curKP;
	float pwmMax, pwmMin;
	float CurRef;
	uword tmp;

	pwmMin = 0;
	pwmMax = 250; // pwmMax = 300;   //  LYMG---22.6.10

	if ((VCU_ChgCmdOBS == ENABLE_CHG) && (ISG_run_state <= 1))
	{ //{恒流模式
		//{OBS出电流故障保护
		if ((OBS_PwmRef > (pwmMax - 5)) && (OBS_CurOutput < _MAX((OBS_CurLimit - 0.5), 0)))
		{ // 0.2
			OBS_FaultPwm = 1;
		}
		if (OBS_FaultPwm == 1)
		{ //{PWM输出但是没有期望电流出来,关掉这一路不再工作,重启消除
			OBS_CurKI = 0;
			OBS_CurRef = 0;
			OBS_PwmRef = 0;
			prevPwmRefOBS = 0;
			VCU_CurRefAllowedOBS = 0;
		}
		//************************电流指令加梯度************************************//
		if (OBS_CurRef < VCU_CurRefAllowedOBS)
			OBS_CurRef = OBS_CurRef + 0.01; //_MIN( (OBS_CurRef + 0.01), VCU_CurRefAllowedOBS );//23.4.20
		if (OBS_CurRef > VCU_CurRefAllowedOBS)
			OBS_CurRef = OBS_CurRef - 0.1; //_MAX( (OBS_CurRef - 0.02), VCU_CurRefAllowedOBS );  //23.4.20
		if (VCU_CurRefAllowedOBS <= 0)
		{
			VCU_CurRefAllowedOBS = 0;
		}
		if (OBS_CurRef <= 0)
		{
			OBS_CurRef = 0;
		}
		//************************电流指令加梯度************************************//

		//************************OBS侧电流环处理************************************//
		curDet = OBS_CurRef - _MAX(OBS_CurOutput, 0.1);
		if (((curDet > 0) && (prevPwmRefOBS <= pwmMax)) || ((curDet < 0) && (prevPwmRefOBS > 0)))
		{
			curKP = curDet * 1;		   // KP_Cur;  //1;  //KP_Cur;  //0.1;
			OBS_CurKI += curDet * 0.1; // KI_Cur;  //0.001;  //KI_Cur;  //0.01;
			OBS_CurKI = _MID(OBS_CurKI, pwmMin, pwmMax);
			OBS_PwmRef = _MID(_ROUNDU(curKP + OBS_CurKI), pwmMin, pwmMax);
			// GPTA0_LTCXR11 = GPTA0_LTCXR15 = _PWMZ( OBS_PwmRef );  //{100%占空比赋值0x03E8,0%占空比复制0x03E7
			prevPwmRefOBS = OBS_PwmRef;
		}
		//************************OBS侧电流环处理************************************//
		if ((VCU_CurRefAllowedOBS == 0) && (OBS_CurRef == 0)) // 无电流指令  快速关闭 PWM
		{
			OBS_CurKI = 0;
			OBS_PwmRef = OBS_PwmRef - 0.75;
			prevPwmRefOBS = 0;
			if (OBS_PwmRef < 1)
			{
				OBS_PwmRef = 0;
			}
		}
		// if ( ( OBS_CurLimit == 0 ) || ( DCAC_HandSwitch == 1 ) ) {
		if ((DCAC_HandSwitch == 1) || (VCU_ChgStatusBMS != 1))
		{
			OBS_CurKI = 0;
			OBS_CurRef = 0;
			OBS_PwmRef = 0;
			prevPwmRefOBS = 0;
			// GPTA0_LTCXR11 = GPTA0_LTCXR15 = _PWMZ( OBS_PwmRef );  //{100%占空比赋值0x03E8,0%占空比复制0x03E7
		}
	}
	else
	{ //{关闭输出
		OBS_CurKI = 0;
		OBS_CurRef = 0;
		OBS_PwmRef = 0;
		prevPwmRefOBS = 0;
	}
	//************************PWM指令增加再次增加梯度************************************//
	if (OBS_PwmStep < OBS_PwmRef)
		OBS_PwmStep = _MIN((OBS_PwmStep + 20), OBS_PwmRef); // 20
	if (OBS_PwmStep > OBS_PwmRef)
		OBS_PwmStep = _MAX((OBS_PwmStep - 20), OBS_PwmRef); // 20

	// OBS_PwmStep = lim(OBS_PwmStep,0,250); //PWM做限幅处理
	//************************PWM指令增加再次增加梯度************************************//

	// GPTA0_LTCXR03 = GPTA0_LTCXR07 = _PWMZ( OBS_PwmRef );	//{100%占空比赋值0x03E8,0%占空比复制0x03E7
	tmp = GPTA0_LTCCTR02;
	if ((GPTA0_LTCCTR02_RED == 1) && (GPTA0_LTCCTR02_FED == 1) && (GPTA0_LTCCTR02_CEN == 1) && (GPTA0_LTCCTR02_OSM == 0))
	{
		GPTA0_LTCXR03 = _PWMZ(OBS_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR03_OSM = 0;
		GPTA0_LTCCTR03_RED = 1;
		GPTA0_LTCCTR03_FED = 1;
		GPTA0_LTCCTR02_OSM = 1;
		tmp = GPTA0_LTCXR02;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR02)
		{
			GPTA0_LTCCTR02_RED = 0;
			GPTA0_LTCCTR02_FED = 0;
		}
	}
	else
	{
		GPTA0_LTCXR02 = _PWMZ(OBS_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR02_OSM = 0;
		GPTA0_LTCCTR02_RED = 1;
		GPTA0_LTCCTR02_FED = 1;
		GPTA0_LTCCTR03_OSM = 1;
		tmp = GPTA0_LTCXR03;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR03)
		{
			GPTA0_LTCCTR03_RED = 0;
			GPTA0_LTCCTR03_FED = 0;
		}
	}
	tmp = GPTA0_LTCCTR06;
	if ((GPTA0_LTCCTR06_RED == 1) && (GPTA0_LTCCTR06_FED == 1) && (GPTA0_LTCCTR06_CEN == 1) && (GPTA0_LTCCTR06_OSM == 0))
	{
		GPTA0_LTCXR07 = _PWMZ(OBS_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR07_OSM = 0;
		GPTA0_LTCCTR07_RED = 1;
		GPTA0_LTCCTR07_FED = 1;
		GPTA0_LTCCTR06_OSM = 1;
		tmp = GPTA0_LTCXR06;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR06)
		{
			GPTA0_LTCCTR06_RED = 0;
			GPTA0_LTCCTR06_FED = 0;
		}
	}
	else
	{
		GPTA0_LTCXR06 = _PWMZ(OBS_PwmStep); //{100%占空比赋值0x03E8,0%占空比赋值0x03E7
		GPTA0_LTCCTR06_OSM = 0;
		GPTA0_LTCCTR06_RED = 1;
		GPTA0_LTCCTR06_FED = 1;
		GPTA0_LTCCTR07_OSM = 1;
		tmp = GPTA0_LTCXR07;
		tmp = GPTA0_LTCXR00;
		if (GPTA0_LTCXR00 > GPTA0_LTCXR07)
		{
			GPTA0_LTCCTR07_RED = 0;
			GPTA0_LTCCTR07_FED = 0;
		}
	}
}

void mode_switch(void)
{
	static uword checkRangeCnt = 0;
	static uword fullCnt = 0;
	static uword ms3000Cnt = 0;
	static uword ms30000Cnt = 0;
	static float curLimitOBG = 0;
	static float curLimitOBS = 0;
	float volInput;
	float curLimit;
	float tempMax;
	// static uword ms300Cnt = 300;

	VCU_TempMax = _MAX(_MAX(_MAX(OBC_TempModule, DCDC_TempModule), DCAC_TempModule), NTC_TempMax);

	//{***************************************************************
	//{风扇水泵的控制策略
	if (ms30000Cnt >= 3000)
	{
		if (VCU_TempMax > 55)
		{						//{风扇水泵同时开启
			VCU_PumpEnable = 0; //{水泵
			VCU_FanEnable = 0;	//{风扇
		}
		else if (VCU_TempMax < 45)
		{						//{风扇水泵同时关闭
			VCU_PumpEnable = 1; //{水泵
			VCU_FanEnable = 1;	//{风扇
		}
		else
		{
			if (VCU_TempMax > 48)
			{
				VCU_PumpEnable = 0; //{水泵
			}
			if (VCU_TempMax < 50)
			{
				VCU_FanEnable = 1; //{风扇
			}
		}
	}
	else
	{						//{风扇水泵工作30S
		VCU_PumpEnable = 0; //{水泵
		VCU_FanEnable = 0;	//{风扇
		ms30000Cnt++;
	}
	if ((DCAC_TempCapOBG > 48) || (DCAC_TempCapOBS > 48))
	{
		VCU_PumpEnable = 0;
		if ((OBG_PwmRef > 0) || (OBS_PwmRef > 0))
		{
			VCU_FanEnable = 0;
		}
	}
	if (DCAC_HandSwitch == 1)
	{
		VCU_PumpEnable = 1; //{水泵
		VCU_FanEnable = 1;	//{风扇
	}
	//	IO_P8_1 = VCU_PumpEnable;
	//	IO_P8_2 = VCU_FanEnable;
	//}风扇水泵的控制策略
	//}***************************************************************

	if (DCAC_HandSwitch == 0)
	{ //{手动开关闭合

		volInput = _MAX(OBG_VolInput, OBS_VolInput);

		curLimit = (_CHECKRANGE(volInput, 26.5, 30)) ? (OBGAIMCUR) : (0); //{更新OBG能输出的电流limit  //用电池升压测试
		// curLimit=0;
		tempMax = _MAX(NTC_TempMax, DCAC_TempCapOBG); //{电容和NTC的温度取最大值
		if (tempMax > 95)
		{
			OBG_CurLimit = 0;
		}
		else if (tempMax > 90)
		{
			OBG_CurLimit = curLimit * 0.4;
		}
		else if (tempMax > 85)
		{
			OBG_CurLimit = curLimit * 0.6;
		}
		else if (tempMax > 80)
		{
			OBG_CurLimit = curLimit * 0.8;
			curLimitOBG = OBG_CurLimit;
		}
		else if (tempMax < 75)
		{
			OBG_CurLimit = curLimit;
			curLimitOBG = OBG_CurLimit;
		}
		else
		{
			if (curLimit > 0)
			{
				OBG_CurLimit = curLimitOBG;
			}
			else
			{
				OBG_CurLimit = 0;
			}
		}

		curLimit = (_CHECKRANGE(volInput, 26.5, 30)) ? (OBSAIMCUR) : (0); //{更新OBS能输出的电流limit
		// curLimit=0;
		tempMax = _MAX(NTC_TempMax, DCAC_TempCapOBS); //{电容和NTC的温度取最大值
		if (tempMax > 95)
		{
			OBS_CurLimit = 0;
		}
		else if (tempMax > 90)
		{
			OBS_CurLimit = curLimit * 0.4;
		}
		else if (tempMax > 85)
		{
			OBS_CurLimit = curLimit * 0.6;
		}
		else if (tempMax > 80)
		{
			OBS_CurLimit = curLimit * 0.8;
			curLimitOBS = OBS_CurLimit;
		}
		else if (tempMax < 75)
		{
			OBS_CurLimit = curLimit;
			curLimitOBS = OBS_CurLimit;
		}
		else
		{
			if (curLimit > 0)
			{
				OBS_CurLimit = curLimitOBS;
			}
			else
			{
				OBS_CurLimit = 0;
			}
		}

		if ((BMS_HVPowerLoopStatus == 0) || (EF_NodeLostBMS == 1))
		{ //{高压回路未闭合或BMS掉线
			VCU_CurRefAllowedOBG = 0;
			VCU_VolRefAllowedOBG = 0;
			VCU_CurRefAllowedOBS = 0;
			VCU_VolRefAllowedOBS = 0;
			VCU_CurRefAllowedOBC = 0;
			VCU_VolRefAllowedOBC = 0;
			VCU_EnableDCAC = DISABLE_DCAC;
			VCU_EnableDCDC = DISABLE_DCDC;
			VCU_ChgCmdOBG = DISABLE_CHG;
			VCU_ChgCmdOBS = DISABLE_CHG;
			VCU_ChgStatusBMS = 0; //{高压回路未闭合
		}

		// if ( BMS_HVPowerAllow == 2 ) {  //{BMS主动下电请求
		//	BMS_ContChgCurAllowed = 0;
		//	BMS_ContDischgCurAllowed = 0;
		//	if ( _ABS( BMS_CurOutput ) < 2 ) {
		//		VCU_HVPowerEnableBMS = 3;  //{BMS下电请求允许
		//	}
		// }

		if (VCU_ChgStatusBMS == 1)
		{ //{高压回路已经闭合 //可以进行充放电

			// if ( OBC_CurMax > BMS_ContChgCurAllowed ) {  //{OBC允许的输出电流不小于BMS允许的输入电流
			if (OBC_CurMax >= BMS_ContChgCurAllowed)
			{ // YMG--22.3.27

				if (BMS_HeatingRequest == 1)
				{ //{电池有加热的需求
					BMS_ContChgCurAllowed = 0;
					VCU_CurRefAllowedOBG = 0;
					VCU_VolRefAllowedOBG = 0;
					VCU_CurRefAllowedOBS = 0;
					VCU_VolRefAllowedOBS = 0;
					VCU_CurRefAllowedOBC = 0;
					VCU_VolRefAllowedOBC = 0;
					VCU_EnableDCAC = DISABLE_DCAC; //{可以加热则禁止DCAC输出
					if ((OBC_CurOutput + OBG_CurOutput + OBS_CurOutput) < 2)
					{ //{OB端电流小于2A时,禁止AC负载端输出
						VCU_EnableDCDC = DISABLE_DCDC;
						VCU_EnableDCAC = DISABLE_DCAC;
						VCU_ChgCmdOBC = DISABLE_CHG;
						VCU_ChgCmdOBG = ENABLE_CHG;
						VCU_ChgCmdOBS = ENABLE_CHG;
						if (_ABS(BMS_CurOutput) < 2)
						{							  //{BMS输出电流小于2A后,下高压
							VCU_HVPowerEnableBMS = 2; //{主动下高压
							VCU_HeatingEnableBMS = 1;
						}
						else
						{
							VCU_HeatingEnableBMS = 0;
						}
					}
				}

				if ((OBC_CurOutput - BMS_ContChgCurAllowed) < (0 - 0.2))
				{
					VCU_CurRefAllowedOBC = BMS_ContChgCurAllowed;
				}
				else
				{
					if (BMS_CurOutput >= (0 - BMS_ContChgCurAllowed))
					{
						VCU_CurRefAllowedOBC = BMS_ContChgCurAllowed + (BMS_ContChgCurAllowed + BMS_CurOutput);
						if (VCU_CurRefAllowedOBC > OBC_CurMax)
						{
							VCU_CurRefAllowedOBC = OBC_CurMax;
						}
					}
					else
					{
						VCU_CurRefAllowedOBC = BMS_ContChgCurAllowed;
					}
				}

				if ((BMS_SOC >= 100) || (BMS_ContChgCurAllowed == 0))
				{
					VCU_CurRefAllowedOBC = 0;
					VCU_ChgCmdOBC = DISABLE_CHG;
				}

				//	if ( BMS_HeatingRequest == 1 ) {BMS_ContChgCurAllowed = 0;	}//{有加热请求则允许充电电流清零
			}
			else
			{ //{如果OBC允许的输出电流小于BMS允许的输入电流，让OBC以最大输出功率运行即可
				VCU_CurRefAllowedOBC = OBC_CurMax;
			}
			//{临时代码
			//		if ( (BMS_SOC < 100) && (BMS_FaultLevel == 0) && (BMS_ContChgCurAllowed <= 0) && (OBC_CurMax > 0) ) {
			//				VCU_CurRefAllowedOBC = 5;
			//	        }

			if ((BMS_SOC >= 100) || (BMS_ContChgCurAllowed == 0))
			{
				VCU_CurRefAllowedOBC = 0;
				VCU_ChgCmdOBC = DISABLE_CHG;
			}
			//}临时代码
			if ((VCU_CurRefAllowedOBC > 0) && (BMS_HeatingLoopStatus != 1))
			{
				VCU_VolRefAllowedOBC = BMS_VolOutput + 10; //{比高压电池的输出电压大10V
				VCU_ChgCmdOBC = ENABLE_CHG;
				VCU_CurRefAllowedOBG = 0;
				VCU_CurRefAllowedOBS = 0;
			}
			else
			{
				VCU_CurRefAllowedOBG = _MIN((VCU_CurRefAllowedOBG + 0.01), _MIN(BMS_ContChgCurAllowed / 2.0, OBG_CurLimit));
				VCU_CurRefAllowedOBS = _MIN((VCU_CurRefAllowedOBS + 0.01), _MIN(BMS_ContChgCurAllowed / 2.0, OBS_CurLimit));
			}

			if (BMS_CurOutput > BMS_ContDischgCurAllowed)
			{
				VCU_EnableDCAC = DISABLE_DCAC;
				if (DCAC_SysStatus != RUN)
				{
					VCU_EnableDCDC = DISABLE_DCDC;
				}
			}
		}
		else if (VCU_ChgStatusBMS == 0)
		{ //{高压回路未闭合,禁能所有部件功能并等待可上高压指令,或者是等待手动开关下低压电
			if (DCAC_HandSwitch == 0)
			{

				if ((BMS_HVPowerAllow == 1) && (BMS_HeatingLoopStatus == 0) && (VCU_HeatingEnableBMS == 0) && (OBC_VolOutput < 200))
				{
					VCU_HVPowerEnableBMS = 1; //{允许上高压且加热回路断开且无加热请求且充电机无电压输出VCU发送主动上高压指令
				}
				else
				{
					VCU_HVPowerEnableBMS = 0;
				}

				if (BMS_HeatingRequest == 1)
				{
					VCU_HVPowerEnableBMS = 2;
					if (BMS_HeatingLoopStatus == 1)
					{ // 加热回路闭合的情况下
						if (OBC_CurMax > 0)
						{ // OBC慢充加热 处理逻辑
							VCU_CurRefAllowedOBG = 0;
							VCU_VolRefAllowedOBG = 0;
							VCU_CurRefAllowedOBS = 0;
							VCU_VolRefAllowedOBS = 0;
							VCU_CurRefAllowedOBC = OBC_CurMax;
							VCU_VolRefAllowedOBC = 394;
							VCU_EnableDCDC = ENABLE_DCDC;
							VCU_EnableDCAC = ENABLE_DCAC;
							VCU_ChgCmdOBC = ENABLE_HEAT;
							VCU_ChgCmdOBG = ENABLE_CHG;
							VCU_ChgCmdOBS = ENABLE_CHG;
						}
						if (BMS_DCChgStatus > 0)
						{ // 直流加热 处理逻辑
							VCU_CurRefAllowedOBG = 0;
							VCU_VolRefAllowedOBG = 0;
							VCU_CurRefAllowedOBS = 0;
							VCU_VolRefAllowedOBS = 0;
							VCU_CurRefAllowedOBC = 0;
							VCU_VolRefAllowedOBC = 0;
							VCU_EnableDCDC = DISABLE_DCDC;
							VCU_EnableDCAC = DISABLE_DCAC;
							VCU_ChgCmdOBC = FINISH_CHG;
							VCU_ChgCmdOBG = ENABLE_CHG;
							VCU_ChgCmdOBS = ENABLE_CHG;
							VCU_HeatingEnableBMS = 1; // 22.8.4  维持加热允许
							if (BMS_HVPowerAllow == 1)
							{
								VCU_HVPowerEnableBMS = 1;
								if (BMS_HVPowerLoopStatus == 1)
								{ //{高压回路已经闭合  开启DCDC
									VCU_VolRefAllowedOBG = 0;
									VCU_VolRefAllowedOBS = 0;
									VCU_EnableDCDC = ENABLE_DCDC;
									VCU_EnableDCAC = DISABLE_DCAC;
									VCU_ChgCmdOBC = FINISH_CHG;
									VCU_ChgCmdOBG = ENABLE_CHG;
									VCU_ChgCmdOBS = ENABLE_CHG;
									VCU_HeatingEnableBMS = 1; // 22.8.4  维持加热允许
								}
							} // else  VCU_HVPowerEnableBMS = 0;
						}
						// LYMG---22.4.5   增加区分 交直流两种不同的加热方式
						/*
						//						VCU_CurRefAllowedOBG = 0;
						//						VCU_VolRefAllowedOBG = 0;
						//						VCU_CurRefAllowedOBS = 0;
						//						VCU_VolRefAllowedOBS = 0;
						//						VCU_CurRefAllowedOBC = OBC_CurMax;
						//						VCU_VolRefAllowedOBC = 394;
						//						VCU_EnableDCDC = ENABLE_DCDC;
						//						VCU_EnableDCAC = ENABLE_DCAC;
						//						VCU_ChgCmdOBC = ENABLE_HEAT;
						//						VCU_ChgCmdOBG = ENABLE_CHG;
						//						VCU_ChgCmdOBS = ENABLE_CHG;
						//					    VCU_HVPowerEnableBMS = 1;
						 */
					}
					else
					{ // 加热回路未闭合的时候
						if (VCU_RunCmdBMS == 1)
						{ // if ( OBC_CurMax > 0 ) {   //LYMG---22.4.5
							VCU_HeatingEnableBMS = 1;
						}
						else
						{
							VCU_HeatingEnableBMS = 0;
						}
					}
				}
				else
				{ // 高压回路断路---没有加热需求
					VCU_HeatingEnableBMS = 0;
					if (OBC_CurMax > 0)
					{
						VCU_CurRefAllowedOBG = 0;
						VCU_VolRefAllowedOBG = 0;
						VCU_CurRefAllowedOBS = 0;
						VCU_VolRefAllowedOBS = 0;
						VCU_CurRefAllowedOBC = 0;
						VCU_VolRefAllowedOBC = 0;
						VCU_EnableDCDC = DISABLE_DCDC;
						VCU_EnableDCAC = DISABLE_DCAC;
						VCU_ChgCmdOBC = DISABLE_CHG;
						VCU_ChgCmdOBG = ENABLE_CHG;
						VCU_ChgCmdOBS = ENABLE_CHG;
					}
					// BMS 掉线且高压回路断路 且手动开关闭合具备应急供电的功能
				}
			}
			if ((BMS_HVPowerLoopStatus == 1) && (BMS_HeatingRequest == 0) && (BMS_HeatingLoopStatus == 0))
			{ // if ( BMS_HVPowerLoopStatus == 1 ) { //22.8.4 //{高压回路已经闭合  //此处添加是否允许跳转到 模式1
				VCU_VolRefAllowedOBG = 0;
				VCU_VolRefAllowedOBS = 0;
				VCU_VolRefAllowedOBC = BMS_VolOutput + 10; //{比高压电池的输出电压大10V
				VCU_EnableDCDC = ENABLE_DCDC;
				VCU_EnableDCAC = ENABLE_DCAC;
				VCU_ChgCmdOBC = DISABLE_CHG;
				VCU_ChgCmdOBG = ENABLE_CHG;
				VCU_ChgCmdOBS = ENABLE_CHG;
				VCU_ChgStatusBMS = 1; //{状态跳转
			}
		}
		else
		{ //{电池处于满充状态  VCU_ChgStatusBMS =3的时候  //{状态跳转
			if (BMS_SOC > 90)
			{ // 90 //循环充电 设置启动阈值 90~100% SOC维持区间
				if (BMS_CurOutput > 100)
				{ //{电池输出电流大于一定值, 从充电端补一定电流, 现在不起作用
					VCU_CurRefAllowedOBC = _MIN((BMS_CurOutput - 1), OBC_CurMax);
					if (VCU_CurRefAllowedOBC > 0)
					{
						VCU_VolRefAllowedOBC = BMS_VolOutput + 10; //{比高压电池的输出电压大10V
						VCU_ChgCmdOBC = ENABLE_CHG;
						VCU_CurRefAllowedOBG = 0;
						VCU_CurRefAllowedOBS = 0;
					}
					else
					{
						VCU_CurRefAllowedOBG = _MIN((VCU_CurRefAllowedOBG + 0.01), _MIN((BMS_CurOutput - 1) / 2.0, OBG_CurLimit));
						VCU_CurRefAllowedOBS = _MIN((VCU_CurRefAllowedOBS + 0.01), _MIN((BMS_CurOutput - 1) / 2.0, OBS_CurLimit));
					}
				}
				else
				{
					VCU_ChgCmdOBC = DISABLE_CHG;
					VCU_CurRefAllowedOBC = 0;
					VCU_CurRefAllowedOBG = 0;
					VCU_CurRefAllowedOBS = 0;
				}
			}
			else
			{
				VCU_ChgStatusBMS = 1;
			}
		}
		//*************************************************满充处理机制******************************************************//
		if ((BMS_CellVolMax >= 3650) || (BMS_SOC == 100))
		{ //{电池处于满充状态 单体电压到3.65或者 SOC到100 //22.4.3
			//{如果是有OBC充电,必须要SOC到100才结束充电,若是只有OBG或OBS,单体电压条件满足即可
			if ((VCU_ChgCmdOBC == ENABLE_CHG && BMS_SOC == 100) || VCU_ChgCmdOBC != ENABLE_CHG)
			{
				VCU_CurRefAllowedOBC = 0;
				VCU_VolRefAllowedOBC = 0;
				VCU_CurRefAllowedOBG = 0;
				VCU_VolRefAllowedOBG = 0;
				VCU_CurRefAllowedOBS = 0;
				VCU_VolRefAllowedOBS = 0;
				VCU_ChgCmdOBC = DISABLE_CHG;
				VCU_ChgCmdOBG = DISABLE_CHG; // ENABLE_CHG
				VCU_ChgCmdOBS = DISABLE_CHG; // ENABLE_CHG
				VCU_ChgStatusBMS = 3;
				// VCU_RunCmdBMS = 0;  //循环充电 间歇停止充电时，不给BMS发充电模式
			}
			if (VCU_CurRefAllowedOBC >= 5)
			{
				VCU_CurRefAllowedOBC = 5;
			} // 限制5A电流充电

			if (BMS_CellVolMax >= 3700)
			{
				VCU_CurRefAllowedOBC = 0;
				VCU_VolRefAllowedOBC = 0;
				VCU_CurRefAllowedOBG = 0;
				VCU_VolRefAllowedOBG = 0;
				VCU_CurRefAllowedOBS = 0;
				VCU_VolRefAllowedOBS = 0;
				VCU_ChgCmdOBC = DISABLE_CHG;
				VCU_ChgCmdOBG = DISABLE_CHG; // ENABLE_CHG
				VCU_ChgCmdOBS = DISABLE_CHG; // ENABLE_CHG
				VCU_ChgStatusBMS = 3;
				// VCU_RunCmdBMS = 0;  //循环充电 间歇停止充电时，不给BMS发充电模式
			}
		}
		//*************************************************满充处理机制******************************************************//

		//*************************************************通过SOC控制DC&AC负载**********************************************//

		if (BMS_SOC <= 10)
		{
			VCU_EnableDCAC = DISABLE_DCAC;
		}
		if (BMS_SOC <= 5)
		{
			VCU_EnableDCDC = DISABLE_DCDC; // TODO @Evan looks like a bug, should it be VCU_EnableDCDC?
		}
		if ((BMS_SOC >= 15) && (BMS_HVPowerLoopStatus == 1))
		{
			VCU_EnableDCAC = ENABLE_DCAC;
		}
		if ((BMS_SOC > 10) && (BMS_HVPowerLoopStatus == 1))
		{
			VCU_EnableDCDC = ENABLE_DCDC;
		}
		//*************************************************通过SOC控制DC&AC负载**********************************************//

		//****************************************低电量保护切断继电器**************************************************//

		//{通过最低单体电池电压来判定负载DCAC, DCDC开启闭合的合适条件
		if ((BMS_CellVolMin <= 3150) && (BMS_HVPowerLoopStatus == 1)) // 3250{单体电压低一级故障报警阈值,停止使用220V放电
		{
			VCU_EmptyFlagBMS_class01 = 1;
		}

		if ((BMS_CellVolMin <= 3100) && (BMS_HVPowerLoopStatus == 1)) // 3200{单体电压低二级故障报警，关闭所有负载必须先给电池充电
		{
			if (ms3000Cnt >= 300)
				VCU_EmptyFlagBMS_class02 = 1;
			else
			{
				ms3000Cnt++;
			}
		}

		if (VCU_EmptyFlagBMS_class01 == 1)
		{ //{电池SOC高于20%前不再AC放电  //LYMG---22.11.15
			if ((BMS_SOC >= 15) && (BMS_CellVolMin > 3180))
			{ // 3280//OVC--15%--3228mv
				VCU_EnableDCAC = ENABLE_DCAC;
				VCU_EnableDCDC = ENABLE_DCDC;
				VCU_EmptyFlagBMS_class01 = 0;
			}
			else
			{
				VCU_EnableDCAC = DISABLE_DCAC;
				VCU_EnableDCDC = ENABLE_DCDC;
				// BMS_SOC = BMS_SOC * 0.5;
			}
		}

		if (VCU_EmptyFlagBMS_class02 == 1)
		{ //{电池SOC高于20%前不再给电池放电  //LYMG---22.11.15
			if ((BMS_SOC >= 15) && (BMS_CellVolMin > 3180))
			{ // 3280//OVC--15%--3280mv
				VCU_EnableDCAC = ENABLE_DCAC;
				VCU_EnableDCDC = ENABLE_DCDC;
				VCU_EmptyFlagBMS_class01 = 0;
				VCU_EmptyFlagBMS_class02 = 0;
				ms3000Cnt = 0;
			}
			else
			{
				VCU_EnableDCAC = DISABLE_DCAC;
				VCU_EnableDCDC = DISABLE_DCDC;
				// BMS_SOC = BMS_SOC*0.5;
			}
		}

		if (VCU_EmptyFlagBMS_class02 == 1)
		{ // 出现单体二级亏电标志后，  不插枪不会闭合继电器 ，连接交流枪或直流枪会闭合 高压继电器指令
			if ((OBC_CurMax > 0) || (BMS_DCChgStatus > 0) || ((KEY_ON_EN == 1)))
			{							  // 有插枪或启动发动机ON档后 VCU发送高压继电器闭合 指令
				VCU_HVPowerEnableBMS = 1; // 允许上高压指令
				// BMS_SOC = BMS_SOC * 0.5;
			}
			else
			{
				VCU_HVPowerEnableBMS = 2; // 请求下高压指令
				VCU_ChgCmdOBC = DISABLE_CHG;
				// VCU_ChgCmdOBG = DISABLE_CHG;
				// VCU_ChgCmdOBS = DISABLE_CHG;
				VCU_CurRefAllowedOBC = 0;
				VCU_VolRefAllowedOBC = 0;
				VCU_CurRefAllowedOBG = 0;
				VCU_VolRefAllowedOBG = 0;
				VCU_CurRefAllowedOBS = 0;
				VCU_VolRefAllowedOBS = 0;
				BMS_SOC = 0;
			}
		}
		//****************************************低电量保护切断继电器**************************************************//

		//****************************************充电模式处理**************************************************//

		if (((VCU_ChgCmdOBC == ENABLE_CHG) || (VCU_ChgCmdOBG == ENABLE_CHG) || (VCU_ChgCmdOBS == ENABLE_CHG)) && (BMS_DCChgStatus == 0))
		{
			if ((OBC_CurOutput > 0) || (OBG_PwmRef > 0) || (OBS_PwmRef > 0))
			{
				VCU_RunCmdBMS = 1; //{发给BMS告诉BMS目前电池是处于充电状态,可以做满充校准
			}
			else
			{
				VCU_RunCmdBMS = 0;
			}
		}
		else
		{
			VCU_RunCmdBMS = 0;
		}

		//	if ( ( OBC_CurOutput + OBG_CurOutput + OBS_CurOutput > 0 ) && ( BMS_CurOutput <= 0 )&& (BMS_DCChgStatus == 0) )
		//	 {
		//		if ( (  OBC_CurMax > 0) || ( OBG_PwmRef > 0 ) || ( OBS_PwmRef > 0 ) )
		//		{
		//			  VCU_RunCmdBMS = 2;	//{发给BMS告诉BMS目前电池是处于充电状态,可以做满充校准
		//		}
		//	 } else {
		//		      VCU_RunCmdBMS = 0;
		//	        }

		if (BMS_DCChgStatus > 0)
		{								   // 任何工况，只要直流枪插枪 即停止交流充电 //22.8.4
			VCU_ChgCmdOBC = FINISH_CHG;	   // 直流充电，关闭OBC电流
			VCU_CurRefAllowedOBC = 0;	   // 直流充电，关闭OBC电流
			VCU_VolRefAllowedOBC = 0;	   // 直流充电，关闭OBC电压
			VCU_RunCmdBMS = 0;			   // 直流枪连接，VCU只能发放电状态
			VCU_ChgCmdOBG = DISABLE_CHG;   // ENABLE_CHG
			VCU_ChgCmdOBS = DISABLE_CHG;   // ENABLE_CHG
			VCU_EnableDCAC = DISABLE_DCAC; // 直流充电，关闭220V负载用电
		}
		//****************************************充电模式处理**************************************************//
		if ((BMS_DCChgStatus > 0) || (OBC_CurMax > 0))
		{
			VCU_RunCmdBMS = 1;
		} // BMS终版程序时，需调整35参数  22.8.1
		// else {VCU_RunCmdBMS = 0;}
		//****************************************应急用电**************************************************//
		if ((OBC_CurMax > 0) && (EF_NodeLostBMS == 1) && (BMS_HVPowerLoopStatus == 0) && (BMS_DCChgStatus == 0) && (BMS_HVPowerAllow == 0))
		{
			VCU_CurRefAllowedOBG = 0;
			VCU_VolRefAllowedOBG = 0;
			VCU_CurRefAllowedOBS = 0;
			VCU_VolRefAllowedOBS = 0;
			VCU_CurRefAllowedOBC = OBC_CurMax;
			VCU_VolRefAllowedOBC = 360;
			VCU_EnableDCDC = ENABLE_DCDC;
			VCU_EnableDCAC = ENABLE_DCAC;
			VCU_ChgCmdOBC = ENABLE_HEAT;
			VCU_ChgCmdOBG = DISABLE_CHG; // ENABLE_CHG
			VCU_ChgCmdOBS = DISABLE_CHG; // ENABLE_CHG
		}

		//****************************************应急用电**************************************************//

		//****************************************温度超过-20℃（不允许回馈式充电）****************************//
		if (BMS_TempMin < -20)
		{
			VCU_CurRefAllowedOBC = 0;
			VCU_VolRefAllowedOBC = 0;
			VCU_CurRefAllowedOBG = 0;
			VCU_VolRefAllowedOBG = 0;
			VCU_CurRefAllowedOBS = 0;
			VCU_VolRefAllowedOBS = 0;
			VCU_ChgCmdOBC = FINISH_CHG;
			VCU_ChgCmdOBG = DISABLE_CHG;
			VCU_ChgCmdOBS = DISABLE_CHG;
		}
		//****************************************温度超过-20℃（不允许回馈式充电）****************************//

		if ((BMS_CurOutput < -2) && (BMS_HVPowerLoopStatus == 1))
			VCU_EnableDCDC = ENABLE_DCDC; //{充电时强制开启DCDC
	}
	else
	{ //{检测到手动开关断开
		VCU_CurRefAllowedOBC = 0;
		VCU_VolRefAllowedOBC = 0;
		VCU_CurRefAllowedOBG = 0;
		VCU_VolRefAllowedOBG = 0;
		VCU_CurRefAllowedOBS = 0;
		VCU_VolRefAllowedOBS = 0;
		if ((OBC_CurOutput + OBG_CurOutput + OBS_CurOutput) < 2)
		{ // OB端电流小于2A时,禁止AC负载端输出
			VCU_EnableDCDC = DISABLE_DCDC;
			VCU_EnableDCAC = DISABLE_DCAC;
			VCU_ChgCmdOBC = FINISH_CHG;
			VCU_ChgCmdOBG = DISABLE_CHG;
			VCU_ChgCmdOBS = DISABLE_CHG;
			if (_ABS(BMS_CurOutput) < 2)
			{							  //{BMS输出电流小于2A后,下高压
				VCU_HVPowerEnableBMS = 2; //{主动下高压
			}
		}
	}

	VCU_CurRefAllowedOBC = _MIN(VCU_CurRefAllowedOBC, OBC_CurMax);
	VCU_CurRefAllowedOBG = _MIN(VCU_CurRefAllowedOBG, OBG_CurLimit);
	VCU_CurRefAllowedOBS = _MIN(VCU_CurRefAllowedOBS, OBS_CurLimit);

	if ((BMS_DCChgStatus > 0) || (OBC_CurMax > 0))
	{
		ChgPos_ConState = 1;
	}
	else
	{
		ChgPos_ConState = 0;
	}

	if ((ChgPos_ConState == 0) && (BMS_HVPowerLoopStatus == 1) && (DCAC_HandSwitch == 0) && (BMS_ContChgCurAllowed > 0) && (BMS_SOC <= 85)) // SOC-小于85%开启，高于 90% 关闭取力器充电
	{
		ISG_ChargeEnable = 1;
	}
	else if ((ChgPos_ConState == 1) || (BMS_HVPowerLoopStatus == 0) || (DCAC_HandSwitch == 1) || (BMS_ContChgCurAllowed == 0) || (BMS_SOC >= 90))
	{
		ISG_ChargeEnable = 0;
	}
	else
	{
	}
}

void bms_tx(void)
{
	// BMS_COMMAND
	if (++VCU_LifeTimeBMS > 15)
		VCU_LifeTimeBMS = 1;
	TXData[0] = VCU_RunCmdBMS << 3 | VCU_HeatingEnableBMS << 2 | VCU_HVPowerEnableBMS; // 高压上下电指令
	TXData[1] = VCU_PowComp * 2;
	TXData[2] = 0;
	TXData[3] = 0;
	TXData[4] = 0;
	TXData[5] = 0;
	TXData[6] = 0;
	TXData[7] = VCU_LifeTimeBMS;
	if (CAN_ubRequestMsgObj(8))
	{ //--------------------发送0x0CFF8A31报文
		CAN_vLoadData(8, TXData);
		CAN_vTransmit(8);
	}
	// BMS_COMMAND
}

void obc_tx(void)
{
	sword tmp = 0; /* should never be uword for can tx */

	if (MS500Cnt >= 50)
	{
		//{OBC_COMMAND
		tmp = VCU_VolRefAllowedOBC * 10;
		TXData[0] = tmp;
		TXData[1] = tmp >> 8;
		tmp = VCU_CurRefAllowedOBC * 10;
		TXData[2] = tmp;
		TXData[3] = tmp >> 8;
		tmp = VCU_ChgCmdOBC;
		TXData[4] = tmp;
		tmp = VCU_LifeTimeOBC++;
		TXData[5] = tmp;
		TXData[6] = 0;
		TXData[7] = 0;
		if (CAN_ubRequestMsgObj(18))
		{
			CAN_vLoadData(18, TXData);
			CAN_vTransmit(18);
		}
		//}OBC_COMMAND
	}
}

void gui_tx(void)
{
	static ubyte j = 0;
	sword tmp = 0; //{用于CAN发送,必须是sword类型数据
	ubyte i;
	uword DC_VOL_display = 0;
	float VCU_volInput;

	if (MS500Cnt >= 50)
	{
		//{VCU_Status01
		tmp = 0;
		if (BMS_HVPowerAllow == 2)
		{ //{bit0  电池状态
			tmp = tmp | 0x01;
		}
		else
		{
			tmp = tmp & 0xFE;
		}
		if ((BMS_CurOutput < 0) && (BMS_HVPowerLoopStatus == 1))
		{ //{bit1 充电闪电图标显示
			tmp = tmp | 0x02;
		}
		else
		{
			tmp = tmp & 0xFD;
		}

		//*******************************增加ON档信号，指示行车充电*****************************//	23.4.20
		uword engine_BOOST_OFF_cnt = 0;
		uword solar_BOOST_OFF_cnt = 0;
		uword engine_BOOST_ON = 0;
		uword solar_BOOST_ON = 0;

		VCU_volInput = _MAX(OBG_VolInput, OBS_VolInput);
		if ((KEY_ON_EN == 1) && (VCU_volInput > 26.5))
		{
			engine_BOOST_ON = 1;
			engine_BOOST_OFF_cnt = 0;
			solar_BOOST_ON = 0;
			solar_BOOST_OFF_cnt = 0;
		}
		else
		{
			if (engine_BOOST_ON == 1)
			{
				engine_BOOST_OFF_cnt++;
				if (engine_BOOST_OFF_cnt > 10)
				{
					engine_BOOST_ON = 0;
					engine_BOOST_OFF_cnt = 11;
				}
			}
		}

		if ((KEY_ON_EN == 0) && (VCU_volInput > 26.5) && (engine_BOOST_ON == 0))
		{
			solar_BOOST_ON = 1;
			solar_BOOST_OFF_cnt = 0;
		}
		else
		{
			if (solar_BOOST_ON == 1)
			{
				solar_BOOST_OFF_cnt++;
				if (solar_BOOST_OFF_cnt > 50)
				{
					solar_BOOST_ON = 0;
					solar_BOOST_OFF_cnt = 51;
				}
			}
		}

		if ((solar_BOOST_ON == 0) && (engine_BOOST_ON == 1)) //{bit2 开启发电机充电图标
		{
			tmp = tmp | 0x04;
			tmp = tmp & 0xF7;
		}
		if ((solar_BOOST_ON == 1) && (engine_BOOST_ON == 0)) //{bit3 开启太阳能充电图标
		{
			tmp = tmp | 0x08;
			tmp = tmp & 0xFB;
		}
		if ((solar_BOOST_ON == 0) && (engine_BOOST_ON == 0)) // 关闭太阳能和行车充电图标
		{
			tmp = tmp & 0xF7;
			tmp = tmp & 0xFB;
		}
		//*******************************增加ON档信号，指示行车充电*****************************//	23.4.20
		/*
				if ( ( OBG_PwmRef > 50 ) || ( OBS_PwmRef > 50 ) )) {       //{bit2 发电机开启充电//30
					tmp = tmp | 0x04;
					tmp = tmp & 0xF7;
				} else if ( ( OBG_PwmRef > 10 ) || ( OBS_PwmRef > 10 ) ) {  //{bit3 太阳能 开启充电
					tmp = tmp | 0x08;
					tmp = tmp & 0xFB;
				} else {
					tmp = tmp & 0xF7;
					tmp = tmp & 0xFB;
				}
		*/
		if ((OBC_CurOutput > 0) || (BMS_DCChgStatus > 0))
		{ //{bit4  交流桩或直流桩开启充电
			tmp = tmp | 0x10;
		}
		else
		{
			tmp = tmp & 0xEF;
		}
		//
		//		if (DCDC_VolOutput > 26 ) {  //{bit3 太阳能 开启充电
		//			tmp = tmp | 0x08;
		//		    tmp = tmp & 0xFB;}
		//		else {
		//			tmp = tmp & 0xF7;
		//		    tmp = tmp & 0xFB;}

		// 希欧德屏幕----指示灯状态修改 -----有故障亮红灯
		//		if ( DCDC_VolOutput < 12 ) {  //{bit5
		//			tmp = tmp & 0xDF;
		//		} else {
		//			if ( DCDC_VolOutput >= 12.5 ) {
		//				tmp = tmp | 0x20;
		//			}
		//		}
		//		if ( DCAC_SysStatus == 2 ) {  //{bit6
		//			tmp = tmp | 0x40;
		//		} else {
		//			tmp = tmp & 0xBF;
		//		}
		//**************************************DC模块状态显示*****************************************//

		if (DCDC_FaultCode == 0)
		{
			tmp = tmp & 0xDF;
		} // DC模块故障码 为0，   显示DC模块正常
		else
		{
			tmp = tmp | 0x20;
		} // DC模块故障码 不为0，显示DC模块异常

		//		if ( DCDC_VolOutput < 24.5 ) {    //DCDC_FaultCode
		//			tmp = tmp | 0x20;            //{bit5   DC降压模块状态异常 //24
		//		} else {
		//		         if ( DCDC_VolOutput >= 25.0 ) {
		//		    	   tmp = tmp & 0xDF;     //{bit5   DC降压模块状态正常
		//		     }
		//		  }

		//**************************************DC模块状态显示*****************************************//

		//**************************************AC模块状态显示*****************************************//
		if ((EF_StatusLostDCAC == 1) || (DCAC_SysStatus == 3))
		{ //{bit6 DCAC模块状态
			tmp = tmp | 0x40;
		}
		else
		{
			tmp = tmp & 0xBF;
		}
		//**************************************AC模块状态显示*****************************************//
		TXData[0] = (ubyte)tmp;
		TXData[1] = 0;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		if (EF_StatusLostDCDC == 0)
			DC_VOL_display = ((ubyte)DCDC_VolOutput) * 10;
		else
			DC_VOL_display = ((ubyte)LvDcMeaFilted) * 10;

		if (DC_VOL_display > 255)
			DC_VOL_display = 255;
		else
			DC_VOL_display = DC_VOL_display;

		TXData[6] = DC_VOL_display;
		TXData[7] = BMS_SOC;
		if (CAN_ubRequestMsgObj(26))
		{ //--------------------发送0x10080000报文
			CAN_vLoadData(26, TXData);
			CAN_vTransmit(26);
		}

		//{VCU_FaultInfo
		if (j == FAULT_NUM)
			j = 0;
		if (j == 0)
		{
			for (i = 0; i < FAULT_NUM; i++)
			{
				FaultMatrix[i] = 0;
			}
			if (BMS_Code1 & 0x02)
				FaultMatrix[0] = 2;
			if (BMS_Code1 & 0x20)
				FaultMatrix[1] = 6;
			if (BMS_Code1 & 0x40)
				FaultMatrix[2] = 7;
			if (BMS_Code2 & 0x02)
				FaultMatrix[3] = 10;
			if (BMS_Code2 & 0x04)
				FaultMatrix[4] = 11;
			if (BMS_Code2 & 0x08)
				FaultMatrix[5] = 12;
			if (BMS_Code2 & 0x10)
				FaultMatrix[6] = 13;
			if (BMS_Code2 & 0x20)
				FaultMatrix[7] = 14;
			if (BMS_Code2 & 0x40)
				FaultMatrix[8] = 15;
			if (BMS_Code2 & 0x80)
				FaultMatrix[9] = 16;
			if (BMS_Code3 & 0x01)
				FaultMatrix[10] = 17;
			if (BMS_Code3 & 0x04)
				FaultMatrix[11] = 19;
			if (BMS_Code3 & 0x08)
				FaultMatrix[12] = 20;
			if (BMS_Code3 & 0x10)
				FaultMatrix[13] = 21;
			if (BMS_Code3 & 0x20)
				FaultMatrix[14] = 22;
			if (BMS_Code3 & 0x40)
				FaultMatrix[15] = 23;
			if (BMS_Code3 & 0x80)
				FaultMatrix[16] = 24;
			if (BMS_Code4 & 0x01)
				FaultMatrix[17] = 25;
			if (BMS_Code4 & 0x02)
				FaultMatrix[18] = 26;
			if (BMS_Code4 & 0x04)
				FaultMatrix[19] = 27;
			if (BMS_Code4 & 0x08)
				FaultMatrix[20] = 28;
			if (BMS_Code4 & 0x10)
				FaultMatrix[21] = 29;
			if (BMS_Code4 & 0x20)
				FaultMatrix[22] = 30;
			if (BMS_Code4 & 0x40)
				FaultMatrix[23] = 31;
			if (BMS_Code4 & 0x80)
				FaultMatrix[24] = 32;
			if (BMS_Code5 & 0x01)
				FaultMatrix[25] = 33;
			if (BMS_Code5 & 0x02)
				FaultMatrix[26] = 34;
			if (BMS_Code5 & 0x04)
				FaultMatrix[27] = 35;
			if (BMS_Code5 & 0x08)
				FaultMatrix[28] = 36;
			if (BMS_Code5 & 0x10)
				FaultMatrix[29] = 37;
			if (BMS_Code5 & 0x20)
				FaultMatrix[30] = 38;
			if (BMS_Code5 & 0x40)
				FaultMatrix[31] = 39;
			if (DCDC_FaultCode)
				FaultMatrix[32] = 39 + DCDC_FaultCode;
			if (OBC_FaultCode)
				FaultMatrix[33] = 46 + OBC_FaultCode;
			if (OBG_FaultPwm > 0)
				FaultMatrix[34] = 60;
			if (OBS_FaultPwm > 0)
				FaultMatrix[35] = 61;
			if (ErrorCode & 0x00000001)
				FaultMatrix[36] = 62;
			if (ErrorCode & 0x00000002)
				FaultMatrix[37] = 63;
			if (ErrorCode & 0x00000004)
				FaultMatrix[38] = 64;
			if (ErrorCode & 0x00000008)
				FaultMatrix[39] = 65;
			if (ErrorCode & 0x00000010)
				FaultMatrix[40] = 66;
			if (ErrorCode & 0x00000020)
				FaultMatrix[41] = 67;
			if (ErrorCode & 0x00000040)
				FaultMatrix[42] = 68;
			if (ErrorCode & 0x00000080)
				FaultMatrix[43] = 69;
			if (ErrorCode & 0x00000100)
				FaultMatrix[44] = 70;
			if (ErrorCode & 0x00000200)
				FaultMatrix[45] = 71;
			if (ErrorCode & 0x00000400)
				FaultMatrix[46] = 72;
			if (ErrorCode & 0x00000800)
				FaultMatrix[47] = 73;
		}
		TXData[0] = 0;
		while (j < FAULT_NUM)
		{
			if (FaultMatrix[j++] > 0)
			{
				TXData[0] = FaultMatrix[j - 1];
				break;
			}
		}
		TXData[1] = 0;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		TXData[6] = 0;
		TXData[7] = 0;
		if (CAN_ubRequestMsgObj(27))
		{ //--------------------发送0x100C0000报文
			CAN_vLoadData(27, TXData);
			CAN_vTransmit(27);
		}
		else
		{
			// CAN_vInit();
			CAN_NCR1 = 0x00000041;
			CAN_NECNT1 = 0x00600000;
			CAN_NCR1 &= ~0x00000041;
		}
	}
}

void dcac_tx(void)
{
	if (MS100Cnt >= 10)
	{
		//{DCAC_COMMAND
		TXData[0] = VCU_EnableDCAC;
		TXData[1] = VCU_PWMEnable;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		TXData[6] = 0;
		TXData[7] = 0;
		if (CAN_ubRequestMsgObj(22))
		{ //--------------------发送0x04080000报文
			CAN_vLoadData(22, TXData);
			CAN_vTransmit(22);
		}
		//}DCAC_COMMAND
	}
}

void dcdc_tx(void)
{
	if (MS100Cnt >= 10)
	{
		if (++VCU_LifeTimeDCDC > 15)
			VCU_LifeTimeDCDC = 1;
		TXData[0] = VCU_LifeTimeDCDC;
		TXData[1] = 0;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		TXData[6] = 0;
		TXData[7] = VCU_EnableDCDC;
		if (CAN_ubRequestMsgObj(16))
		{ //--------------------发送0x10FF7AD0报文
			CAN_vLoadData(16, TXData);
			CAN_vTransmit(16);
		}
	}
}

void debug_tx(void)
{
	static ubyte prevFault = 0;
	static ubyte s1Cnt = 0;
	static ubyte j = 0;
	sword tmp = 0; //{用于CAN发送,必须是sword类型数据
	sword Tpp01 = 0;
	sword Tpp02 = 0;
	ubyte i;

	if (MS100Cnt >= 10)
	{

		Tpp01 = BMS_CellVolMin; // VCU_CurRefAllowedOBG*100;//OBS_CurLimit;//engine_BOOST_OFF_cnt;
		Tpp02 = BMS_CellVolMax; // DCAC_HandSwitch;//OBG_CurRef*100;//OBG_CurLimit*10;//solar_BOOST_OFF_cnt;

		TXData[0] = VCU_PumpEnable;
		TXData[1] = VCU_FanEnable;
		tmp = VCU_EmptyFlagBMS_class01; // engine_BOOST_ON;// KEY_ON_EN;//BMS_TempAvg;
		TXData[2] = tmp;
		tmp = VCU_EmptyFlagBMS_class02; // engine_BOOST_ON;//BMS_SOC;//BMS_SOH ;//BMS_NegRelayStatus;
		TXData[3] = tmp;

		TXData[4] = Tpp01;
		TXData[5] = Tpp01 >> 8;

		TXData[6] = Tpp02;
		TXData[7] = Tpp02 >> 8;
		if (CAN_ubRequestMsgObj(2))
		{ //--------------------发送0x04840000报文
			CAN_vLoadData(2, TXData);
			CAN_vTransmit(2);
		}

		TXData[0] = 0;
		TXData[1] = 0;
		TXData[2] = VCU_ChgCmd << 6 | VCU_ChgCmdOBS << 5 | VCU_ChgCmdOBG << 4 | EF_NodeLostDCAC << 3 | EF_NodeLostDCDC << 2 | EF_NodeLostOBC << 1 | EF_NodeLostBMS;
		TXData[3] = VCU_EmptyFlagBMS_class02 << 2 | VCU_ChgStatusBMS;
		tmp = (ubyte)VCU_SBOperationDCDC;
		TXData[4] = tmp;
		TXData[5] = 215; // 215版本 调整单体保护阈值//214增加//213增加亏电保护以及取力器发电//200测试版本 修改水泵不启动问题；减小升压电流
		TXData[6] = KEY_ON_EN;
		TXData[7] = prevFault;
		if (++s1Cnt >= 10)
		{
			if (j == FAULT_NUM)
				j = 0;
			if (j == 0)
			{
				prevFault = 0;
				for (i = 0; i < FAULT_NUM; i++)
				{
					FaultMatrix[i] = 0;
				}
				if (BMS_Code1 & 0x02)
					FaultMatrix[0] = 2;
				if (BMS_Code1 & 0x20)
					FaultMatrix[1] = 6;
				if (BMS_Code1 & 0x40)
					FaultMatrix[2] = 7;
				if (BMS_Code2 & 0x02)
					FaultMatrix[3] = 10;
				if (BMS_Code2 & 0x04)
					FaultMatrix[4] = 11;
				if (BMS_Code2 & 0x08)
					FaultMatrix[5] = 12;
				if (BMS_Code2 & 0x10)
					FaultMatrix[6] = 13;
				if (BMS_Code2 & 0x20)
					FaultMatrix[7] = 14;
				if (BMS_Code2 & 0x40)
					FaultMatrix[8] = 15;
				if (BMS_Code2 & 0x80)
					FaultMatrix[9] = 16;
				if (BMS_Code3 & 0x01)
					FaultMatrix[10] = 17;
				if (BMS_Code3 & 0x04)
					FaultMatrix[11] = 19;
				if (BMS_Code3 & 0x08)
					FaultMatrix[12] = 20;
				if (BMS_Code3 & 0x10)
					FaultMatrix[13] = 21;
				if (BMS_Code3 & 0x20)
					FaultMatrix[14] = 22;
				if (BMS_Code3 & 0x40)
					FaultMatrix[15] = 23;
				if (BMS_Code3 & 0x80)
					FaultMatrix[16] = 24;
				if (BMS_Code4 & 0x01)
					FaultMatrix[17] = 25;
				if (BMS_Code4 & 0x02)
					FaultMatrix[18] = 26;
				if (BMS_Code4 & 0x04)
					FaultMatrix[19] = 27;
				if (BMS_Code4 & 0x08)
					FaultMatrix[20] = 28;
				if (BMS_Code4 & 0x10)
					FaultMatrix[21] = 29;
				if (BMS_Code4 & 0x20)
					FaultMatrix[22] = 30;
				if (BMS_Code4 & 0x40)
					FaultMatrix[23] = 31;
				if (BMS_Code4 & 0x80)
					FaultMatrix[24] = 32;
				if (BMS_Code5 & 0x01)
					FaultMatrix[25] = 33;
				if (BMS_Code5 & 0x02)
					FaultMatrix[26] = 34;
				if (BMS_Code5 & 0x04)
					FaultMatrix[27] = 35;
				if (BMS_Code5 & 0x08)
					FaultMatrix[28] = 36;
				if (BMS_Code5 & 0x10)
					FaultMatrix[29] = 37;
				if (BMS_Code5 & 0x20)
					FaultMatrix[30] = 38;
				if (BMS_Code5 & 0x40)
					FaultMatrix[31] = 39;
				if (DCDC_FaultCode)
					FaultMatrix[32] = 39 + DCDC_FaultCode;
				if (OBC_FaultCode)
					FaultMatrix[33] = 46 + OBC_FaultCode;
				if (OBG_FaultPwm > 0)
					FaultMatrix[34] = 60;
				if (OBS_FaultPwm > 0)
					FaultMatrix[35] = 61;
				if (ErrorCode & 0x00000001)
					FaultMatrix[36] = 62;
				if (ErrorCode & 0x00000002)
					FaultMatrix[37] = 63;
				if (ErrorCode & 0x00000004)
					FaultMatrix[38] = 64;
				if (ErrorCode & 0x00000008)
					FaultMatrix[39] = 65;
				if (ErrorCode & 0x00000010)
					FaultMatrix[40] = 66;
				if (ErrorCode & 0x00000020)
					FaultMatrix[41] = 67;
				if (ErrorCode & 0x00000040)
					FaultMatrix[42] = 68;
				if (ErrorCode & 0x00000080)
					FaultMatrix[43] = 69;
				if (ErrorCode & 0x00000100)
					FaultMatrix[44] = 70;
				if (ErrorCode & 0x00000200)
					FaultMatrix[45] = 71;
				if (ErrorCode & 0x00000400)
					FaultMatrix[46] = 72;
				if (ErrorCode & 0x00000800)
					FaultMatrix[47] = 73;
			}
			while (j < FAULT_NUM)
			{
				if (FaultMatrix[j++] > 0)
				{
					prevFault = FaultMatrix[j - 1];
					break;
				}
			}
			s1Cnt = 0;
		}
		if (CAN_ubRequestMsgObj(3))
		{
			CAN_vLoadData(3, TXData);
			CAN_vTransmit(3);
		}

		// sword *tst = (sword*)(&VCU_CurRefAllowedOBG);
		// TXData[0] = (ubyte) *tst;
		// TXData[1] = (ubyte) (*tst >> 8);
		// TXData[2] = (ubyte) (*tst >> 16);
		// TXData[3] = (ubyte) (*tst >> 24);
		// tst = (sword*)(&VCU_CurRefAllowedOBS);
		// TXData[4] = (ubyte) *tst;
		// TXData[5] = (ubyte) (*tst >> 8);
		// TXData[6] = (ubyte) (*tst >> 16);
		// TXData[7] = (ubyte) (*tst >> 24);
		// if ( CAN_ubRequestMsgObj( 4 ) ) {
		//	CAN_vLoadData( 4, TXData );
		//	CAN_vTransmit( 4 );
		// }

		tmp = OBS_VolInput * 1000;
		TXData[0] = tmp;
		TXData[1] = tmp >> 8;
		tmp = OBG_VolInput * 1000;
		TXData[2] = tmp;
		TXData[3] = tmp >> 8;
		tmp = VCU_VolOutput * 100;
		TXData[4] = tmp;
		TXData[5] = tmp >> 8;
		tmp = NTC_TempMax;
		TXData[6] = tmp;
		tmp = VCU_TempMax;
		TXData[7] = tmp;
		if (CAN_ubRequestMsgObj(5))
		{
			CAN_vLoadData(5, TXData);
			CAN_vTransmit(5);
		}

		tmp = (OBS_CurOutput - 30) * 1000;
		TXData[0] = tmp;
		TXData[1] = tmp >> 8;
		tmp = (OBG_CurOutput - 30) * 1000;
		TXData[2] = tmp;
		TXData[3] = tmp >> 8;
		tmp = OBS_PwmRef;
		TXData[4] = tmp;
		TXData[5] = tmp >> 8;
		tmp = OBG_PwmRef;
		TXData[6] = tmp;
		TXData[7] = tmp >> 8;
		if (CAN_ubRequestMsgObj(6))
		{
			CAN_vLoadData(6, TXData);
			CAN_vTransmit(6);
		}
		else
		{
			// CAN_vInit();
			CAN_NCR0 = 0x00000041;
			CAN_NECNT0 = 0x00600000;
			CAN_NCR0 &= ~0x00000041;
		}

		if (OBS_PwmRef > 10 || OBG_PwmRef > 10)
			IO_P8_0 = 0;
		else
			IO_P8_0 = 1; //    PWM升压启动 即打开风扇

		// tst = (sword*)(&DCDC_CurOutput);
		// TXData[0] = (ubyte) *tst;
		// TXData[1] = (ubyte) (*tst >> 8);
		// TXData[2] = (ubyte) (*tst >> 16);
		// TXData[3] = (ubyte) (*tst >> 24);
		// tst = (sword*)(&DCDC_VolOutput);
		// TXData[4] = (ubyte) *tst;
		// TXData[5] = (ubyte) (*tst >> 8);
		// TXData[6] = (ubyte) (*tst >> 16);
		// TXData[7] = (ubyte) (*tst >> 24);
		// if ( CAN_ubRequestMsgObj( 7 ) ) {  //--------------------发送0x05C40000报文
		//	CAN_vLoadData( 7, TXData );
		//	CAN_vTransmit( 7 );
		// }

		ISG_LiftTime++;
		if (ISG_LiftTime > 255)
		{
			ISG_LiftTime = 1;
		}

		TXData[0] = ISG_ChargeEnable & 0x1;
		TXData[1] = ChgPos_ConState & 0x1;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		TXData[6] = 0;
		TXData[7] = ISG_LiftTime;

		if (CAN_ubRequestMsgObj(32))
		{
			CAN_vLoadData(32, TXData);
			CAN_vTransmit(32);
		}
	}
}

void isg_tx(void)
{

	if (MS100Cnt >= 10)
	{
		ISG_LiftTime++;
		if (ISG_LiftTime > 255)
		{
			ISG_LiftTime = 1;
		}

		TXData[0] = ISG_ChargeEnable & 0x1;
		TXData[1] = ChgPos_ConState & 0x1;
		TXData[2] = 0;
		TXData[3] = 0;
		TXData[4] = 0;
		TXData[5] = 0;
		TXData[6] = 0;
		TXData[7] = ISG_LiftTime;

		if (CAN_ubRequestMsgObj(32))
		{
			CAN_vLoadData(32, TXData);
			CAN_vTransmit(32);
		}
	}
}

void MainTask(void)
{
	MS20Cnt++;
	MS100Cnt++;
	MS200Cnt++;
	MS500Cnt++;
	MS1000Cnt++;
	MS5000Cnt++;

	bms_rx();
	obc_rx();
	dcac_rx();
	dcdc_rx();
	debug_rx();
	isg_rx();

	temp_sample();
	volcur_sample();
	error_check();
	mode_switch();
	obg_boost();
	obs_boost();

	bms_tx();
	obc_tx();
	gui_tx();
	dcac_tx();
	dcdc_tx();
	debug_tx();
	// isg_tx();

	if (SnapState != SNAP_WRITTEN)
		snap();

	if (MS20Cnt >= 2)
		MS20Cnt = 0;
	if (MS100Cnt >= 10)
		MS100Cnt = 0;
	if (MS200Cnt >= 20)
		MS200Cnt = 0;
	if (MS500Cnt >= 50)
		MS500Cnt = 0;
	if (MS1000Cnt >= 100)
		MS1000Cnt = 0;
	if (MS5000Cnt >= 500)
		MS5000Cnt = 0;
}

void task_100us(void)
{
	static uword delayCnt = 0;

	if (delayCnt >= 10000)
	{
		if (++MS10Cnt >= 100)
		{ //{10ms任务周期
			MS10Cnt = 0;
			MainTask();
		}
	}
	else
	{
		delayCnt++;
		vars_clear_01();
		// sensor_check();
		if (SnapState == SNAP_CHECK)
			snap();
	}
}

//{3123 lookup table test
// float inputX[5] = { 1, 2, 3, 4, 5 };
// float inputY[5] = { 4, 6, 12, 24, 36 };
// float output[5][5] = {{ 0.1, 0.2, 0.3, 0.4, 0.5 },
//					  { 0.2, 0.4, 0.6, 0.8, 1.0 },
//					  { 0.3, 0.6, 0.9, 1.2, 1.5 },
//					  { 0.4, 0.8, 1.2, 1.6, 2.0 },
//					  { 0.5, 1.0, 1.5, 2.0, 2.5 }};
// AxisSearchF32 testDataX;
// AxisSearchF32 testDataY;
//
// float testsearch = RXObj.ubData[5] * 256 + RXObj.ubData[4];
// testsearch = testsearch * 0.01;
// SearchAxisVar_f32( testsearch, inputX, 4, 0, &testDataX );
//
// testsearch = RXObj.ubData[7] * 256 + RXObj.ubData[6];
// testsearch = testsearch * 0.01;
// SearchAxisVar_f32( testsearch, inputY, 4, 0, &testDataY );
// float t3 = GetResultTbl2dip_f32( &testDataX, &testDataY, 5, output );
//
// testsearch = VolRefBoost * 0.001;
// SearchAxisVar_f32( testsearch, NtcTable, 119, 60, &testDataX );
// t3 = GetResultTblip_f32( &testDataX, TempTable );
// float t1 = testDataX.DeltaInput;
// float t2 = testDataX.DeltaAxis;
// uword u1 = testDataX.Index1;
//}3123
