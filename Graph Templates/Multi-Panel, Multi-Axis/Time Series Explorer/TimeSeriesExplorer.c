/*------------------------------------------------------------------------------*
 * File Name:				 													*
 * Creation: 																	*
 * Purpose: OriginC Source C file												*
 * Copyright (c) ABCD Corp.	2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010		*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
////////////////////////////////////////////////////////////////////////////////////
// Including the system header file Origin.h should be sufficient for most Origin
// applications and is recommended. Origin.h includes many of the most common system
// header files and is automatically pre-compiled when Origin runs the first time.
// Programs including Origin.h subsequently compile much more quickly as long as
// the size and number of other included header files is minimized. All NAG header
// files are now included in Origin.h and no longer need be separately included.
//
// Right-click on the line below and select 'Open "Origin.h"' to open the Origin.h
// system header file.
#include <Origin.h>
////////////////////////////////////////////////////////////////////////////////////

//#pragma labtalk(0) // to disable OC functions for LT calling.
////////////////////////////////////////////////////////////////////////////////////
// Include your own header files here.
#include <Control.h>
#include <../Originlab/DialogEx.h>
//#include <../Originlab/grobj_utils.h>
#define STR_ALL			_L("All")
#define STR_CUSTOM		_L("Custom")
#define STR_START_FROM_BEGIN	_L("Start from Beginning of Period")

#define STR_1_DAY		_L("1 Day")
#define STR_1_MONTH		_L("1 Month")
#define STR_1_QUARTER	_L("1 Quarter")
#define STR_6_MONTHS	_L("6 Months")
#define STR_1_YEAR		_L("1 Year")

#define STR_12_HOURS	_L("12 Hours")
#define STR_6_HOURS		_L("6 Hours")
#define STR_1_HOUR		_L("1 Hour")
#define STR_15_MINUTES	_L("15 Minutes")
#define STR_1_MINUTE	_L("1 Minute")
#define STR_1_SECOND	_L("1 Second")


#define ORS_MODE_NAME	"ORSMODE"
#define ORS_START_NAME	"ORSSTART"
enum
{
	ORS_AUTO_FLIP_STATUS_STARTED	= 0x00000001,
	ORS_AUTO_FLIP_STATUS_PAUSED		= 0x00000002,
};
#define ORS_AUTO_FLIP_STATUS 	"ORSFLIPSTATUS"
#define ORS_MODE_INVALID 0
///------ Tony 01/22/2023 ORG-28342-S3 RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
static string s_strRangeFormat = "Range: %s\\f:Segoe UI Symbol(✎)";
class ResizeEventValidTempChange
{
public:
	ResizeEventValidTempChange()
	{
		m_bOldVal = m_bResizeEventValid;
		m_bResizeEventValid = false;
	}
	~ResizeEventValidTempChange()
	{
		m_bResizeEventValid = m_bOldVal;
	}
	
	static bool m_bResizeEventValid;
	
private:
	bool m_bOldVal;
};

bool ResizeEventValidTempChange::m_bResizeEventValid = true;
///------ End RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END

static int _get_time_period_count(double dVal, double dPeriod)
{
	static double s_dTimeTolerance = 1.0 / (24 * 60 * 60 * 1000 * 10);	// 1 millisecond / 10
	double dCount = dVal * dPeriod;
	int nCount = dCount;
	if(dCount >= 0)
	{
		if(fabs(dCount - nCount - 1) < s_dTimeTolerance)
			++nCount;
	}
	else
	{
		if(fabs(dCount - nCount + 1) < s_dTimeTolerance)
			--nCount;
	}
	return nCount;
}

static double _sign_width(double DX, bool bAscending)
{
	if(bAscending)
		return DX;
	
	return -DX;
}

///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
#define ORS_TEXT_RANGE_NAME "ORSTextRange"
#define ORS_TEXT_RANGE_FORMAT "%s - %s"
static bool _SetTextRange(const GraphLayer& gl, const GraphLayer& gl2, int nMode)
{
	GraphObject go = gl.GraphObjects(ORS_TEXT_RANGE_NAME);
	if(!go)
		return false;
	
	string strText = "";
	AxisObject aoXLabel = gl.XAxis.AxisObjects(AXISOBJPOS_LABEL_FIRST);
	int nLabelType;
	aoXLabel.GetTypeFormatEtc(nLabelType);
	bool bDate = nLabelType == OULABEL_DATE;

	vector<double> vd;
    vd.Add(gl2.X.From);
    vd.Add(gl2.X.To);
    
    vector<string> vs;
    string strTime;
	if(nMode == ORS_ALL || nMode == ORS_MODE_INVALID)
	{
		if(bDate)
		{
			strTime = get_date_str(vd[0], LDF_OBJ_CUSTOM, "MM/dd/yyyy");
			vs.Add(strTime);
			strTime = get_date_str(vd[1], LDF_OBJ_CUSTOM, "MM/dd/yyyy");
			vs.Add(strTime);
		}
		else
		{
			strTime = get_time_str(vd[0], LTF_OBJ_CUSTOM, "DDD HH:mm");
			vs.Add(strTime);
			strTime = get_time_str(vd[1], LTF_OBJ_CUSTOM, "DDD HH:mm");
			vs.Add(strTime);
		}
	}
	else if(nMode >= ORSD_MODE_BEGIN && nMode < ORSD_MODE_END)
	{
		switch(nMode)
		{
		case ORSD_1_DAY:
		{
			strTime = get_date_str(vd[0], LDF_OBJ_CUSTOM, "MM/dd/yyyy HH:mm");
			vs.Add(strTime);
			strTime = get_date_str(vd[1], LDF_OBJ_CUSTOM, "MM/dd/yyyy HH:mm");
			vs.Add(strTime);
		}
			break;
		case ORSD_1_MONTH:
		case ORSD_1_QUARTER:
		case ORSD_6_MONTHS:
		case ORSD_1_YEAR:
			strTime = get_date_str(vd[0], LDF_OBJ_CUSTOM, "MM/dd/yyyy");
			vs.Add(strTime);
			strTime = get_date_str(vd[1], LDF_OBJ_CUSTOM, "MM/dd/yyyy");
			vs.Add(strTime);
			break;
		default:
			ASSERT(FALSE);
			return false;
		}
	}
	else if(nMode >= ORST_MODE_BEGIN && nMode < ORST_MODE_END)
	{
		switch(nMode)
		{
		case ORST_1_DAY:
		case ORST_12_HOURS:
		case ORST_6_HOURS:
		case ORST_1_HOUR:
			strTime = get_time_str(vd[0], LTF_OBJ_CUSTOM, "DDD HH:mm");
			vs.Add(strTime);
			strTime = get_time_str(vd[1], LTF_OBJ_CUSTOM, "DDD HH:mm");
			vs.Add(strTime);
			break;
		case ORST_15_MINUTES:
		case ORST_1_MINUTE:
			strTime = get_time_str(vd[0], LTF_OBJ_CUSTOM, "HH:mm:ss");
			vs.Add(strTime);
			strTime = get_time_str(vd[1], LTF_OBJ_CUSTOM, "HH:mm:ss");
			vs.Add(strTime);
			break;
		case ORST_1_SECOND:
			strTime = get_time_str(vd[0], LTF_OBJ_CUSTOM, "HH:mm:ss.###");
			vs.Add(strTime);
			strTime = get_time_str(vd[1], LTF_OBJ_CUSTOM, "HH:mm:ss.###");
			vs.Add(strTime);
			break;
	
		default:
			ASSERT(FALSE);
			return false;
		}
	}
	
	strText.Format(ORS_TEXT_RANGE_FORMAT, vs[0], vs[1]);
	go.Text = strText;
	return true;
}
///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER

static bool _MoveSlider(const GraphLayer& gl, const GraphLayer* pgl2, const GraphObject& go, int nMode, bool bStart, int nLeftInc = 0, bool bCorrectValidLeftRight = false)
{
	ResizeEventValidTempChange revTempChange;
	if(!gl || !go)
		return false;

	double dFrom = gl.X.From;
	double dTo = gl.X.To;
	bool bAscending = dTo > dFrom;
	
	double dLeft;
	double dRight;
	double dXInc = 1;	///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
	int nDateFormat = -1;
	string strCustomDisplay = "";
	
	AxisObject aoXLabel = gl.XAxis.AxisObjects(AXISOBJPOS_LABEL_FIRST);
	int nLabelType;
	aoXLabel.GetTypeFormatEtc(nLabelType);
	bool bDate = nLabelType == OULABEL_DATE;
	if(nMode == ORS_ALL)
	{
		go.DX = dTo - dFrom;
		go.X = dFrom + _sign_width(go.DX, bAscending)/2;
		
		///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
		if(pgl2)
		{
			Axis axis = pgl2->XAxis;
			Tree tr;
			tr.Root.Scale.IncrementBy.nVal = OAI_MAJORTICKCOUNT;
			tr.Root.Scale.MajorTicksCount.nVal = 10;
			tr.Root.Labels.BottomLabels.DisplayAuto.nVal = 1;
			if(bDate)
			{
				tr.Root.Labels.BottomLabels.DateFormat.nVal = 0;
				tr.Root.Labels.BottomLabels.DateCustomDisplay.strVal = "";
			}
			else
			{
				tr.Root.Labels.BottomLabels.TimeFormat.nVal = 0;
				tr.Root.Labels.BottomLabels.TimeCustomDisplay.strVal = "";
			}
			
			if(0 == axis.UpdateThemeIDs(tr.Root))
			{
				axis.ApplyFormat(tr, true, true);
			}
		}
		///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		return false;
	}
	///------ Tony 01/22/2023 ORG-28342-S3 RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
	else if(nMode == ORS_MODE_INVALID)
	{
		double dWidth = go.DX;
		dLeft = go.X - _sign_width(dWidth, bAscending)/2;
		if(nLeftInc != 0)
		{
			dLeft = dLeft + _sign_width(nLeftInc * dWidth, bAscending);
		}
		dRight = dLeft + _sign_width(dWidth, bAscending);
	}
	///------ End RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
	else if(nMode >= ORSD_MODE_BEGIN && nMode < ORSD_MODE_END)
	{
		ASSERT(bDate);
		nDateFormat = LDF_OBJ_CUSTOM;
		dLeft = go.X - _sign_width(go.DX, bAscending)/2;
		SYSTEMTIME st;
		JulianDateToSystemTime(&dLeft, &st);
		switch(nMode)
		{
		case ORSD_1_DAY:
		{
			if(bStart)
			{
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;
			}
			if(nLeftInc != 0)
			{
				system_time_inc(&st, bAscending ? nLeftInc : -nLeftInc, STI_INC_UNIT_DAY);
			}
			SystemTimeToJulianDate(&dLeft, &st);
			system_time_inc(&st, bAscending ? 1 : -1, STI_INC_UNIT_DAY);
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = get_date_period(DATEPERIOD_HOUR, 4);
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm\\nMM/dd/yyyy";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORSD_1_MONTH:
		{
			if(bStart)
			{
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;
			}
			if(nLeftInc != 0)
			{
				system_time_inc(&st, bAscending ? nLeftInc : -nLeftInc, STI_INC_UNIT_MONTH);
			}
			SystemTimeToJulianDate(&dLeft, &st);
			system_time_inc(&st, bAscending ? 1 : -1, STI_INC_UNIT_MONTH);
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = get_date_period(DATEPERIOD_DAY, 5);
			dXInc = s_dXInc;
			strCustomDisplay = "dd\\nMMM, yyyy";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORSD_1_QUARTER:
		{
			if(bStart)
			{
				switch(st.wMonth)
				{
				case 1:
				case 2:
				case 3:
					st.wMonth = 1;
					break;
				case 4:
				case 5:
				case 6:
					st.wMonth = 4;
					break;
				case 7:
				case 8:
				case 9:
					st.wMonth = 7;
					break;
				case 10:
				case 11:
				case 12:
					st.wMonth = 10;
					break;
				}
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;
			}
			if(nLeftInc != 0)
			{
				system_time_inc(&st, bAscending ? nLeftInc * 3 : -nLeftInc * 3, STI_INC_UNIT_MONTH);
			}
			SystemTimeToJulianDate(&dLeft, &st);
			system_time_inc(&st, bAscending ? 3 : -3, STI_INC_UNIT_MONTH);
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = get_date_period(DATEPERIOD_MONTH, 1);
			dXInc = s_dXInc;
			strCustomDisplay = "MMM\\nyyyy";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORSD_6_MONTHS:
		{
			if(bStart)
			{
				switch(st.wMonth)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					st.wMonth = 1;
					break;
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					st.wMonth = 7;
					break;
				}
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;
			}
			if(nLeftInc != 0)
			{
				system_time_inc(&st, bAscending ? nLeftInc * 6 : -nLeftInc * 6, STI_INC_UNIT_MONTH);
			}
			SystemTimeToJulianDate(&dLeft, &st);
			system_time_inc(&st, bAscending ? 6 : -6, STI_INC_UNIT_MONTH);
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = get_date_period(DATEPERIOD_MONTH, 1);
			dXInc = s_dXInc;
			strCustomDisplay = "MMM\\nyyyy";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORSD_1_YEAR:
		{
			if(bStart)
			{
				st.wMonth = 1;
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;
			}
			if(nLeftInc != 0)
			{
				system_time_inc(&st, bAscending ? nLeftInc : -nLeftInc, STI_INC_UNIT_YEAR);
			}
			SystemTimeToJulianDate(&dLeft, &st);
			system_time_inc(&st, bAscending ? 1 : -1, STI_INC_UNIT_YEAR);
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = get_date_period(DATEPERIOD_MONTH, 1);
			dXInc = s_dXInc;
			strCustomDisplay = "MMM\\nyyyy";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		default:
			ASSERT(FALSE);
			return false;
		}
		
		SystemTimeToJulianDate(&dRight, &st);
	}
	else if(nMode >= ORST_MODE_BEGIN && nMode < ORST_MODE_END)
	{
		ASSERT(!bDate);
		nDateFormat = LTF_OBJ_CUSTOM;
		dLeft = go.X - _sign_width(go.DX, bAscending)/2;
		double dInc = 0.0;
		switch(nMode)
		{
		case ORST_1_DAY:
		{
			static double s_dInc = str_to_time("001:00:00:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 1);
				dLeft = nCount;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:04:00:00.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_12_HOURS:
		{
			static double s_dInc = str_to_time("000:12:00:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 2);	// div 0.5
				dLeft = nCount / 2.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:01:00:00.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_6_HOURS:
		{
			static double s_dInc = str_to_time("000:06:00:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 4);	// div 0.25
				dLeft = nCount / 4.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:01:00:00.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_1_HOUR:
		{
			static double s_dInc = str_to_time("000:01:00:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 24);	// div 1/24
				dLeft = nCount / 24.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:00:10:00.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_15_MINUTES:
		{
			static double s_dInc = str_to_time("000:00:15:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 96); // 24 * 60 / 15
				dLeft = nCount / 96.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:00:03:00.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm:ss\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_1_MINUTE:
		{
			static double s_dInc = str_to_time("000:00:01:00.0");
			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 1440); // 24 * 60
				dLeft = nCount / 1440.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:00:00:10.0");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm:ss\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
		case ORST_1_SECOND:
		{
			static double s_dInc = str_to_time("000:00:00:01.0");
 			if(bStart)
			{
				int nCount = _get_time_period_count(dLeft, 86400);	// 24 * 60 * 60
				dLeft = nCount / 86400.0;
			}
			dInc = s_dInc;
			
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			static double s_dXInc = str_to_time("000:00:00:00.1");
			dXInc = s_dXInc;
			strCustomDisplay = "HH:mm:ss.###\\nDDD";
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
			break;
	
		default:
			ASSERT(FALSE);
			return false;
		}
		
		if(nLeftInc != 0)
		{
			dLeft = dLeft + _sign_width(nLeftInc * dInc, bAscending);
		}
		
		dRight = dLeft + _sign_width(dInc, bAscending);
	}
	else
	{
		ASSERT(FALSE);
	}
	
	///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
	if(pgl2)
	{
		Axis axis = pgl2->XAxis;
		//TreeNode trTheme = pgl2->XAxis.GetFormat(FPB_ALL, FOB_ALL, TRUE, TRUE);

		Tree tr;
		tr.Root.Scale.IncrementBy.nVal = OAI_VALUE;
		tr.Root.Scale.Value.dVal = dXInc;
		tr.Root.Labels.BottomLabels.DisplayAuto.nVal = 0;
		if(bDate)
		{
			if(nDateFormat != -1)
				tr.Root.Labels.BottomLabels.DateFormat.nVal = nDateFormat;
			tr.Root.Labels.BottomLabels.DateCustomDisplay.strVal = strCustomDisplay;
		}
		else
		{
			if(nDateFormat != -1)
				tr.Root.Labels.BottomLabels.TimeFormat.nVal = nDateFormat;
			tr.Root.Labels.BottomLabels.TimeCustomDisplay.strVal = strCustomDisplay;
		}
		
		if(0 == axis.UpdateThemeIDs(tr.Root))
		{
			axis.ApplyFormat(tr, true, true);
		}
	}
	///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
	
	bool bValidLeftRight = true;
	if(bAscending)
	{
		if(dRight <= dFrom || dLeft >= dTo)
			bValidLeftRight = false;
	}
	else
	{
		if(dRight >= dFrom || dLeft <= dTo)
			bValidLeftRight = false;
	}
	double dWidth = dRight - dLeft;
	if(!bValidLeftRight)
	{
		if(bCorrectValidLeftRight)
		{
			go.DX = dWidth;
			go.X = (bAscending ? dFrom : dTo) + dWidth/2;
			///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
			//_MoveSlider(gl, go, nMode, bStart, nLeftInc, false);
			_MoveSlider(gl, NULL, go, nMode, bStart, nLeftInc, false);
			///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		}
		return false;
	}
	
	go.DX = dWidth;
	go.X = dLeft + dWidth/2;
	return true;
}


class ORSDateTimeMenu
{
public:
	enum
	{
		ORSD_MODE_BEGIN = 100,
		ORSD_1_DAY = ORSD_MODE_BEGIN,
		ORSD_1_MONTH,
		ORSD_1_QUARTER,
		ORSD_6_MONTHS,
		ORSD_1_YEAR,
		
		ORSD_MODE_END,
		
		ORST_MODE_BEGIN = 200,
		ORST_1_DAY = ORST_MODE_BEGIN,
		ORST_12_HOURS,
		ORST_6_HOURS,
		ORST_1_HOUR,
		ORST_15_MINUTES,
		ORST_1_MINUTE,
		ORST_1_SECOND,

		ORST_MODE_END,
		
		ORS_ALL = 300,
		ORS_CUSTOM,
		ORS_START_FROM_BEGIN,
	};
	
    ORSDateTimeMenu(int nType = OULABEL_DATE)
    {
    	m_nType = nType;
    }
    
    void TrackPopupMenu(const GraphLayer& gl, const GraphLayer& gl2, const GraphObject& go, const GraphObject& goRange, UINT nFlags, int x, int y, HWND hwnd)
    {
    	Menu menu;
    	Init(menu, go);
    	int nCmd = 0;
    	if(!menu.TrackPopupMenu(nFlags, x, y, hwnd, &nCmd))
    	{
    		HandleCmd(gl, gl2, go, goRange, nCmd);
    	}
    }
    
    void HandleCmd(const GraphLayer& gl, const GraphLayer& gl2, const GraphObject& go, const GraphObject& goRange, int nCmd)
    {
    	double dStart = 0.0;
    	go.GetStorageNumericVar(ORS_START_NAME, dStart);
    	if(nCmd == ORS_START_FROM_BEGIN)
		{
			dStart = dStart > 0.0 ? 0.0 : 1.0;
			go.SetStorageNumericVar(ORS_START_NAME, dStart, FALSE);
			
			double dTemp = nCmd;
			go.GetStorageNumericVar(ORS_MODE_NAME, dTemp);
			nCmd = dTemp;
			if(nCmd == ORS_ALL)
				return;
		}
		else
		{
			go.SetStorageNumericVar(ORS_MODE_NAME, nCmd, FALSE);
		}

		///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
		//_MoveSlider(gl, go, nCmd, dStart > 0.0, 0, true);
		_MoveSlider(gl, &gl2, go, nCmd, dStart > 0.0, 0, true);
		///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
		
		///------ Tony 01/22/2023 ORG-28342-S3 RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
		string strRangeText = "";
		switch(nCmd)
		{
		case ORS_ALL:
			strRangeText.Format(s_strRangeFormat, STR_ALL);
			break;

		case ORSD_1_DAY:
			strRangeText.Format(s_strRangeFormat, STR_1_DAY);
			break;
		case ORSD_1_MONTH:
			strRangeText.Format(s_strRangeFormat, STR_1_MONTH);
			break;
		case ORSD_1_QUARTER:
			strRangeText.Format(s_strRangeFormat, STR_1_QUARTER);
			break;
		case ORSD_6_MONTHS:
			strRangeText.Format(s_strRangeFormat, STR_6_MONTHS);
			break;
		case ORSD_1_YEAR:
			strRangeText.Format(s_strRangeFormat, STR_1_YEAR);
			break;
			
		case ORST_1_DAY:
			strRangeText.Format(s_strRangeFormat, STR_1_DAY);
			break;
		case ORST_12_HOURS:
			strRangeText.Format(s_strRangeFormat, STR_12_HOURS);
			break;
		case ORST_6_HOURS:
			strRangeText.Format(s_strRangeFormat, STR_6_HOURS);
			break;
		case ORST_1_HOUR:
			strRangeText.Format(s_strRangeFormat, STR_1_HOUR);
			break;
		case ORST_15_MINUTES:
			strRangeText.Format(s_strRangeFormat, STR_15_MINUTES);
			break;
		case ORST_1_MINUTE:
			strRangeText.Format(s_strRangeFormat, STR_1_MINUTE);
			break;
		case ORST_1_SECOND:
			strRangeText.Format(s_strRangeFormat, STR_1_SECOND);
			break;
	
		default:
			break;
		}
		
		if(!strRangeText.IsEmpty())
		{
			goRange.Text = strRangeText;
		}
		///------ End RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
		
		///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
		_SetTextRange(gl, gl2, nCmd);
		///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
    }
    
    void Init(Menu& menu, const GraphObject& go)
    {
    	double dMode = ORS_MODE_INVALID;
    	double dStart = 0.0;
    	go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
		go.GetStorageNumericVar(ORS_START_NAME, dStart);

    	if(m_nType == OULABEL_DATE)
    	{
			int nPos = ORSD_1_DAY;
			menu.Add(STR_1_DAY, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_MONTH, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_QUARTER, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_6_MONTHS, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_YEAR, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_ALL, ORS_ALL, MF_STRING | (ORS_ALL == dMode ? MF_CHECKED : MF_UNCHECKED));
    	}
    	else
    	{
			int nPos = ORST_1_DAY;
			menu.Add(STR_1_DAY, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_12_HOURS, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_6_HOURS, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_HOUR, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_15_MINUTES, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_MINUTE, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_1_SECOND, nPos, MF_STRING | (nPos == dMode ? MF_CHECKED : MF_UNCHECKED));	nPos++;
			menu.Add(STR_ALL, ORS_ALL, MF_STRING | (ORS_ALL == dMode ? MF_CHECKED : MF_UNCHECKED));
    	}
    	
    	menu.Add(NULL, 0, MF_SEPARATOR);
    	menu.Add(STR_START_FROM_BEGIN, ORS_START_FROM_BEGIN, MF_STRING | (dStart > 0.0 ? MF_CHECKED : MF_UNCHECKED));
    }
protected:
	int m_nType;
};


#define ORS_SLIDER_NAME "zoomrect"
#define ORS_RANGE_NAME "ORSRange"
#define ORS_AUTO_FLIP_NAME "ORSAutoFlip"
bool OriginRangeSlider()
{
	GraphLayer gl = Project.ActiveLayer();
	if(!gl)
		return false;
	
	GraphPage pg = gl.GetPage();
	if(!pg)
		return false;
	
	GraphLayer gl2 = pg.Layers(gl.GetIndex()+1);
	if(!gl2)
		return false;
	
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return false;
	
	GraphObject goRange = gl.GraphObjects(ORS_RANGE_NAME);
	if(!goRange)
		return false;
	
	go.SetStates(0, GOC2_ACCESS_COOR_USING_VARS);
	
	AxisObject aoXLabel = gl.XAxis.AxisObjects(AXISOBJPOS_LABEL_FIRST);
	int nType;				
    int iRet = aoXLabel.GetTypeFormatEtc(nType);
    if(iRet != 0 || (nType != OULABEL_TIME && nType != OULABEL_DATE))
    	return false;

    ORSDateTimeMenu menu(nType);
    POINT pt;
	GetCursorPos(&pt);
    menu.TrackPopupMenu(gl, gl2, go, goRange, 0, pt.x, pt.y, GetWindow());

	return true;
}

void OriginRangeSliderInc(int nLeftInc)
{
	GraphLayer gl = Project.ActiveLayer();
	if(!gl)
		return;
	
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;
	
	double dStart = 0.0;
	go.GetStorageNumericVar(ORS_START_NAME, dStart);
	double dMode = 0.0;
	go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
	///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
	//_MoveSlider(gl, go, dMode, dStart > 0.0, nLeftInc);
	_MoveSlider(gl, NULL, go, dMode, dStart > 0.0, nLeftInc);
	///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
	
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	GraphPage pg = gl.GetPage();
	if(pg)
	{
		GraphLayer gl2 = pg.Layers(gl.GetIndex()+1);
		if(gl2)
			_SetTextRange(gl, gl2, dMode);
	}
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}

static string s_strAutoFlipStart = " \\f:Segoe UI Symbol(▶) ";
static string s_strAutoFlipStop = " \\f:Segoe UI Symbol(⏸) ";
static GraphLayer s_gl;
static GraphObject s_go;
static GraphObject s_goAutoFlip;
void OriginRangeSliderMove()
{
	if(!s_gl || !s_go)
	{
		LT_execute("Timer -k");
		return;
	}
	
	double dStart = 0.0;
	s_go.GetStorageNumericVar(ORS_START_NAME, dStart);
	double dMode = 0.0;
	s_go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
	///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
	//if(!_MoveSlider(s_gl, s_gl2, s_go, dMode, dStart > 0.0, 1))
	if(!_MoveSlider(s_gl, NULL, s_go, dMode, dStart > 0.0, 1))
	///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
	{
		s_go.RemoveStorageNumericVars(ORS_AUTO_FLIP_STATUS, FALSE);
		if(s_goAutoFlip)
			s_goAutoFlip.Text = s_strAutoFlipStart;
		LT_execute("Timer -k");
	}
	
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	GraphPage pg = s_gl.GetPage();
	if(pg)
	{
		GraphLayer gl2 = pg.Layers(s_gl.GetIndex()+1);
		if(gl2)
			_SetTextRange(s_gl, gl2, dMode);
	}
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}

void OriginRangeSliderAutoFlipDialog()
{
	s_gl = Project.ActiveLayer();
	if(!s_gl)
		return;

	s_go = s_gl.GraphObjects(ORS_SLIDER_NAME);
	if(!s_go)
		return;
	
	s_goAutoFlip = s_gl.GraphObjects(ORS_AUTO_FLIP_NAME);
	
	static Tree s_tr;
	if(s_tr.IsEmpty())
	{
		GETN_TREE(tr)
		GETN_NUM(MovePerSeconds, _L("Move every N seconds"), 1)
		GETN_CHECK(StartFromBegining, _L("Start from begining"), true)
		GETN_CUSTOM_BUTTON("OK|Cancel")
		
		s_tr = tr;
	}
	
	double dFrom = s_gl.X.From;
	double dTo = s_gl.X.To;
	bool bAscending = dTo > dFrom;
	double dFlipStatus = 0.0;
	int nFlipStatus = dFlipStatus;
	if(s_go.GetStorageNumericVar(ORS_AUTO_FLIP_STATUS, dFlipStatus) && dFlipStatus > 0.0)
	{
		nFlipStatus = dFlipStatus;
		double dXEnd = dTo - _sign_width(s_go.DX, bAscending)/2;
		if((nFlipStatus & ORS_AUTO_FLIP_STATUS_PAUSED))
		{
			if((bAscending && (s_go.X < dXEnd)) || (!bAscending && (s_go.X > dXEnd)))
			{
				s_tr.StartFromBegining.nVal = 0;
				nFlipStatus &= ~ORS_AUTO_FLIP_STATUS_PAUSED;
			}
			else
			{
				s_tr.StartFromBegining.nVal = 1;
				nFlipStatus = 0;
			}
		}
		else
		{
			s_go.SetStorageNumericVar(ORS_AUTO_FLIP_STATUS, nFlipStatus | ORS_AUTO_FLIP_STATUS_PAUSED, FALSE);
			if(s_goAutoFlip)
				s_goAutoFlip.Text = s_strAutoFlipStart;
			LT_execute("Timer -k");
			return;
		}
	}
	else
	{
		s_tr.StartFromBegining.nVal = 1;
	}
	
	
	if ( GetNBox(s_tr, _L("Automatically Flip Through")) )
	{
		LT_execute("Timer -k");
		
		if(s_tr.StartFromBegining.nVal)
		{
			s_go.X = dFrom + _sign_width(s_go.DX, bAscending)/2;
		}
		
		s_go.SetStorageNumericVar(ORS_AUTO_FLIP_STATUS, nFlipStatus | ORS_AUTO_FLIP_STATUS_STARTED, FALSE);
		if(s_goAutoFlip)
			s_goAutoFlip.Text = s_strAutoFlipStop;
		
		string strCmd;
		strCmd.Format("def TimerProc { run -oc {OriginRangeSliderMove();}; };Timer %f;", s_tr.MovePerSeconds.dVal);
		LT_execute(strCmd);
	}
}

void OriginRangeSliderInit()
{
	GraphLayer gl = Project.ActiveLayer();
	if(!gl)
		return;
	
	GraphPage pg = gl.GetPage();
	if(!pg)
		return;
	
	GraphLayer gl2 = pg.Layers(gl.GetIndex()+1);
	if(!gl2)
		return;
	
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;
	
	AxisObject aoXLabel = gl.XAxis.AxisObjects(AXISOBJPOS_LABEL_FIRST);
	int nType;				
    int iRet = aoXLabel.GetTypeFormatEtc(nType);
    if(iRet != 0 || (nType != OULABEL_TIME && nType != OULABEL_DATE))
    	return;
    
    ResizeEventValidTempChange revTempChange;
    go.RemoveStorageNumericVars(ORS_AUTO_FLIP_STATUS, FALSE);
    double dMode = 0.0;
    if(nType == OULABEL_DATE)
    {
    	dMode = ORS_ALL;
    }
    else if(nType == OULABEL_TIME)
    {
    	dMode = ORS_ALL;
    }
    go.SetStorageNumericVar(ORS_MODE_NAME, dMode, FALSE);
	double dStart = 1.0;
	go.SetStorageNumericVar(ORS_START_NAME, dStart, FALSE);
	///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
	//_MoveSlider(gl, go, dMode, dStart > 0.0, 0, true);
	_MoveSlider(gl, &gl2, go, dMode, dStart > 0.0, 0, true);
	///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
	double dFrom = gl.X.From;
	double dTo = gl.X.To;
	bool bAscending = dTo > dFrom;
	go.X = dFrom + _sign_width(go.DX, bAscending)/2;

	go.DY = gl.Y.To - gl.Y.From;
	go.Y = gl.Y.From + go.DY/2;
	
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	_SetTextRange(gl, gl2, dMode);
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}
///------ Tony 01/22/2023 ORG-28342-S3 RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END
void OriginRangeSliderMoveToBegin()
{
	GraphLayer gl = Project.ActiveLayer();
	if(!gl)
		return;
	
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;
	
	double dFrom = gl.X.From;
	double dTo = gl.X.To;
	bool bAscending = dTo > dFrom;
	go.X = dFrom + _sign_width(go.DX, bAscending)/2;
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	GraphPage pg = gl.GetPage();
	if(pg)
	{
		GraphLayer gl2 = pg.Layers(gl.GetIndex()+1);
		if(gl2)
		{
			double dMode = 0.0;
			go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
			_SetTextRange(gl, gl2, dMode);
		}
	}
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}

void OriginRangeSliderMoveToEnd()
{
	GraphLayer gl = Project.ActiveLayer();
	if(!gl)
		return;
	
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;
	
	double dFrom = gl.X.From;
	double dTo = gl.X.To;
	bool bAscending = dTo > dFrom;
	go.X = dTo - _sign_width(go.DX, bAscending)/2;
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	GraphPage pg = gl.GetPage();
	if(pg)
	{
		GraphLayer gl2 = pg.Layers(gl.GetIndex()+1);
		if(gl2)
		{
			double dMode = 0.0;
			go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
			_SetTextRange(gl, gl2, dMode);
		}
	}
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}

#define MAJOR_TICK_COUNT_SIZE 10
void OriginRangeSliderAfterResize()
{
	if(!ResizeEventValidTempChange::m_bResizeEventValid)
		return;
	
	GraphLayer glActive = Project.ActiveLayer();
	if(!glActive)
		return;
	
	GraphPage pg = glActive.GetPage();
	if(!pg)
		return;
	
	GraphLayer gl = pg.Layers(0);
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;

	GraphObject goRange = gl.GraphObjects(ORS_RANGE_NAME);
	if(!goRange)
		return;
	
	go.SetStorageNumericVar(ORS_MODE_NAME, ORS_MODE_INVALID, FALSE);
	string strRangeText;
	strRangeText.Format(s_strRangeFormat, STR_CUSTOM);
	goRange.Text = strRangeText;
	
	///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
	GraphLayer gl2 = pg.Layers(1);
	if(gl2)
	{
		_SetTextRange(gl, gl2, ORS_MODE_INVALID);
		
		///------ Tony 01/17/2023 ORG-28342-S2 AXIS_INC_DEPENDS_ON_RANGE_MODE
		Axis axis = gl2.XAxis;
		TreeNode trTheme = gl2.XAxis.GetFormat(FPB_ALL, FOB_ALL, TRUE, TRUE);
		if(trTheme.Root.Scale.IncrementBy.nVal == OAI_VALUE)
		{
			if( (fabs(trTheme.Root.Scale.To.dVal - trTheme.Root.Scale.From.dVal) / trTheme.Root.Scale.Value.dVal) > MAJOR_TICK_COUNT_SIZE)
			{
				Tree tr;
				tr.Root.Scale.IncrementBy.nVal = OAI_MAJORTICKCOUNT;
				tr.Root.Scale.MajorTicksCount.nVal = MAJOR_TICK_COUNT_SIZE;

				if(0 == axis.UpdateThemeIDs(tr.Root))
				{
					axis.ApplyFormat(tr, true, true);
				}
			}
		}
		///------ End AXIS_INC_DEPENDS_ON_RANGE_MODE
	}
	///------ End TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
}

///------ Tony 04/07/2024 ORG-28342-S4 TEXT_OBJECT_TO_SHOW_RANGE_OF_TOP_LAYER
void OriginRangeSliderAfterMove()
{
	GraphLayer glActive = Project.ActiveLayer();
	if(!glActive)
		return;
	
	GraphPage pg = glActive.GetPage();
	if(!pg)
		return;
	
	GraphLayer gl = pg.Layers(0);
	GraphObject go = gl.GraphObjects(ORS_SLIDER_NAME);
	if(!go)
		return;

	GraphLayer gl2 = pg.Layers(1);
	if(!gl2)
		return;

	double dMode = 0.0;
	go.GetStorageNumericVar(ORS_MODE_NAME, dMode);
	_SetTextRange(gl, gl2, dMode);
}
///------ End RANGE_BUTTON_SHOW_SELECT_AND_MOVE_TO_BEGIN_END