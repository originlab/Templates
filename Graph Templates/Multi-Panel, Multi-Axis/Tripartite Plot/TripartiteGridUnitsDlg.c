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
#include "TripartiteDlg.h"
////////////////////////////////////////////////////////////////////////////////////
// Include your own header files here.
#include <../Originlab/okThemeID.h>
#include <../Originlab/grobj_utils.h>
#include <..\Originlab\DialogEx.h>
#define STR_TRIPAERTITE_GRID_UNIT_BUTTON					_L("Tripartite...")
#define MAKEDWORD(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
/**********************************************************************
*                       TripartiteGridUnitsDlg                               *
**********************************************************************/
class TripartiteGridUnitsDlg : public ResizeDialog 
{
public:	
	TripartiteGridUnitsDlg();
	
	~TripartiteGridUnitsDlg();
	
	int DoModalEx(HWND hWndParent = NULL)
	{
		InitMsgMap();
		int nRet = DoModal(hWndParent);

		return nRet;
	}
	
protected :
	EVENTS_BEGIN
		ON_INIT(OnInitDialog) 
		ON_SIZE(OnDlgResize)
		ON_OK(OnOK)
		ON_CANCEL(OnClose)
		ON_DESTROY(OnDestroy);
		ON_BN_CLICKED(IDC_APPLY, OnApply)
		
		ON_BN_CLICKED(IDC_TRIPARTITE_HELP_BUTTIN, OnClickHelpBTN)
		
		ON_EN_CHANGE(IDC_TRIPARTITE_NUMBER_OF_MINOR_GRIDS, OnEditChange)
		ON_EN_CHANGE(IDC_TRIPARTITE_45_DEG_A_VALUE, OnEditChange)
		ON_EN_CHANGE(IDC_TRIPARTITE_135_DEG_B_VALUE, OnEditChange)
		ON_EN_CHANGE(IDC_TRIPARTITE_45_DEG_LABEL, OnEditChange)
		ON_EN_CHANGE(IDC_TRIPARTITE_135_DEG_LABEL, OnEditChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_A, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_B, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_C, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_D, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_E, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_F, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_G, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_H, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_I, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_J, OnRadioChange)
		ON_BN_CLICKED(IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED, OnRadioChange)
	EVENTS_END
	
	BOOL OnInitDialog();
	BOOL OnDlgResize(int nType, int cx, int cy);
	BOOL OnOK();
	BOOL OnClose();
	BOOL OnDestroy();
	BOOL OnApply(Control ctrl);
	
	BOOL OnClickHelpBTN(Control ctrl);
	BOOL OnEditChange(Control ctrl);
	BOOL OnRadioChange(Control ctrl);
protected:
	void UpdateABValueAndLable(int nRadioID = -1);
	void SaveToStorage();
	void InitFromStorage();
	void UpdateApplyButtonEnable(BOOL bEnabel);
	BOOL GetApplyButtonEnable();
private:
	Control m_editAValue;
	Control m_editBValue;
	Control m_editALabel;
	Control m_editBLabel;
	Control m_editMinorNumber;
	GraphLayer m_gl;
};

////////////////////////////////////////////////////////////////////////////////////
// Start your functions here.
#define STR_TRIPAERTITE_GRID_UNIT_STORAGE					"TripartiteGridUnits"

#define STR_DEFAULT_NUMBER_OF_MINOR_GRID "1"
#define STR_DEFAULT_A_VALUE		"0.1591549"
#define STR_DEFAULT_A_LABEL		"in."
#define STR_DEFAULT_B_VALUE		"0.016273972"
#define STR_DEFAULT_B_LABEL		"g"

#define AXIS_BENGIN_AND_AXIS_END 2


DWORD dwSTYLE =  STYLE_RELATIVE | STYLE_NO_CLEANUP | STYLE_UNDO;

static BOOL _get_numeric_from_text(string str, double& dValue)
{
	if (is_numeric(str))
	{
        dValue =  atof(str);
        return true;
	}
    else
    {
    	ASSERT(false);
        return NANUM;
    }
}

static void _update_reference_line_value(TreeNode& trOneRefLine, string strFormula, string strLabel, BOOL bMinor = false)
{
	TreeNode trFormula = tree_check_get_node( trOneRefLine, "Formula", OTID_AXIS_REFLINE_FORMULA );
	if(trFormula)
		trFormula.strVal = strFormula;
	
	TreeNode trType = tree_check_get_node( trOneRefLine, "Type", OTID_AXIS_REFLINE_TYPE );
	if(trType)
		trType.nVal = RefLineCtrl_Pos_Value;
	
	TreeNode trShow = tree_check_get_node( trOneRefLine.Line, "Show", OTID_AXIS_REFLINE_LINE_SHOW );
	if(trShow)
		trShow.nVal = 1;
	
	trOneRefLine.Labels.Label1.Font.Size.nVal = 12;
	trOneRefLine.Labels.Label1.AutoRotation.nVal = 1;
	trOneRefLine.LabelPos.nVal = RefLabelPos_Left;
	trOneRefLine.Line.Color.nVal = 18;
	
	TreeNode trLabelText = tree_check_get_node( trOneRefLine.Labels.Label1, "Text", OTID_LABEL_TEXT );
	if(trLabelText)
		trLabelText.strVal = strLabel;
	
	TreeNode trLabelShow = tree_check_get_node( trOneRefLine.Labels.Label1, "Show", OTID_LABEL_SHOW );
	if(trLabelShow)
		trLabelShow.nVal = bMinor ? 0 : 1;

}

static void _update_x_y_axis_begin_and_end_Value(GraphLayer& gl, TreeNode &trTheme, TreeNode &trXRefLines, TreeNode &trYRefLines)
{
	trTheme = gl.GetFormat(FPB_ALL, FOB_AXIS_REFLINES, TRUE, TRUE);
	trXRefLines = trTheme.Root.Axes.X.RefLines;
	trXRefLines.RefLines.RefLine1.Type.nVal = RefLineCtrl_Pos_Begin;
	trXRefLines.RefLines.RefLine2.Type.nVal = RefLineCtrl_Pos_End;
	
	trYRefLines = trTheme.Root.Axes.Y.RefLines;
	trYRefLines.RefLines.RefLine1.Type.nVal = RefLineCtrl_Pos_Begin;
	trYRefLines.RefLines.RefLine2.Type.nVal = RefLineCtrl_Pos_End;
}

TripartiteGridUnitsDlg::TripartiteGridUnitsDlg() : ResizeDialog(IDD_TRIPARTITE_GRID_UNITS_DLG, "TripartiteDlg")
{
}

TripartiteGridUnitsDlg::~TripartiteGridUnitsDlg()
{
}

BOOL TripartiteGridUnitsDlg::OnInitDialog()
{
	m_gl = Project.ActiveLayer();
	if(!m_gl)
		return false;
	
	m_editAValue = GetItem(IDC_TRIPARTITE_45_DEG_A_VALUE);
	m_editBValue = GetItem(IDC_TRIPARTITE_135_DEG_B_VALUE);
	m_editALabel = GetItem(IDC_TRIPARTITE_45_DEG_LABEL);
	m_editBLabel = GetItem(IDC_TRIPARTITE_135_DEG_LABEL);
	m_editMinorNumber = GetItem(IDC_TRIPARTITE_NUMBER_OF_MINOR_GRIDS);
	GetItem(IDC_TRIPARTITE_HELP_BUTTIN).Visible = false;
	InitFromStorage();
	return TRUE;
}

BOOL TripartiteGridUnitsDlg::OnDlgResize(int nType, int cx, int cy)
{
	return TRUE;
}

BOOL TripartiteGridUnitsDlg::OnClose()
{
	return IDCLOSE;
}

BOOL TripartiteGridUnitsDlg::OnDestroy()
{
	CreateButtonInTripartiteGraph(m_gl);
	return TRUE;
}

void TripartiteGridUnitsDlg::UpdateABValueAndLable(int nRadioID)
{
	if(nRadioID < 1)
		nRadioID = GetCheckedRadioButton(IDC_TRIPARTITE_SELECT_UNITS_A, IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED);
	
	BOOL bEnableEdit = nRadioID == IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED;
	m_editAValue.Enable = bEnableEdit;
	m_editBValue.Enable = bEnableEdit;
	m_editALabel.Enable = bEnableEdit;
	m_editBLabel.Enable = bEnableEdit;
	
	switch(nRadioID)
	{
	case IDC_TRIPARTITE_SELECT_UNITS_A:
		m_editAValue.Text = STR_DEFAULT_A_VALUE;
		m_editALabel.Text = STR_DEFAULT_A_LABEL;
		m_editBValue.Text = STR_DEFAULT_B_VALUE;
		m_editBLabel.Text = STR_DEFAULT_B_LABEL;
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_B:
		m_editAValue.Text = "0.1591549";
		m_editALabel.Text = "ft";
		m_editBValue.Text = "0.19528766";
		m_editBLabel.Text = "g";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_C:
		m_editAValue.Text = "0.1591549";
		m_editALabel.Text = "mm";
		m_editBValue.Text = "0.00064070659";
		m_editBLabel.Text = "g";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_D:
		m_editAValue.Text = "0.1591549";
		m_editALabel.Text = "cm";
		m_editBValue.Text = "0.0064070659";
		m_editBLabel.Text = "g";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_E:
		m_editAValue.Text = "0.1591549";
		m_editALabel.Text = "m";
		m_editBValue.Text = "0.64070659";
		m_editBLabel.Text = "g";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_F:
		m_editAValue.Text = "0.016273972";
		m_editALabel.Text = "g";
		m_editBValue.Text = "0.1591549";
		m_editBLabel.Text = "in.";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_G:
		m_editAValue.Text = "0.19528766";
		m_editALabel.Text = "g";
		m_editBValue.Text = "0.1591549";
		m_editBLabel.Text = "ft";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_H:
		m_editAValue.Text = "0.00064070659";
		m_editALabel.Text = "g";
		m_editBValue.Text = "0.1591549";
		m_editBLabel.Text = "mm";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_I:
		m_editAValue.Text = "0.0064070659";
		m_editALabel.Text = "g";
		m_editBValue.Text = "0.1591549";
		m_editBLabel.Text = "cm";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_J:
		m_editAValue.Text = "0.64070659";
		m_editALabel.Text = "g";
		m_editBValue.Text = "0.1591549";
		m_editBLabel.Text = "m";
		break;
	case IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED:
		if(m_editAValue.Text.IsEmpty())
			m_editAValue.Text = STR_DEFAULT_A_VALUE;
		if(m_editALabel.Text.IsEmpty())
			m_editALabel.Text = STR_DEFAULT_A_LABEL;
		if(m_editBValue.Text.IsEmpty())
			m_editBValue.Text = STR_DEFAULT_B_VALUE;
		if(m_editBLabel.Text.IsEmpty())
			m_editBLabel.Text = STR_DEFAULT_B_LABEL;
		break;
	default:
		ASSERT(FALSE);
	}
}

void TripartiteGridUnitsDlg::SaveToStorage()
{
	Tree trStorage;
	int nId = GetCheckedRadioButton(IDC_TRIPARTITE_SELECT_UNITS_A, IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED);
	trStorage.RadioID.nVal = nId;
	trStorage.EditAValue.strVal = m_editAValue.Text;
	trStorage.EditALabel.strVal = m_editALabel.Text;
	trStorage.EditBValue.strVal = m_editBValue.Text;
	trStorage.EditBLabel.strVal = m_editBLabel.Text;
	trStorage.EditNumberMinor.strVal = m_editMinorNumber.Text;
	m_gl.PutBinaryStorage(STR_TRIPAERTITE_GRID_UNIT_STORAGE, trStorage);
}

void TripartiteGridUnitsDlg::InitFromStorage()
{
	Tree trStorage;
	if(!m_gl.GetBinaryStorage(STR_TRIPAERTITE_GRID_UNIT_STORAGE, trStorage) )
	{
		trStorage.RadioID.nVal =IDC_TRIPARTITE_SELECT_UNITS_A;
		trStorage.EditAValue.strVal = STR_DEFAULT_A_VALUE;
		trStorage.EditALabel.strVal = STR_DEFAULT_A_LABEL;
		trStorage.EditBValue.strVal = STR_DEFAULT_B_VALUE;
		trStorage.EditBLabel.strVal = STR_DEFAULT_B_LABEL;
		trStorage.EditNumberMinor.strVal = STR_DEFAULT_NUMBER_OF_MINOR_GRID;
	}
	m_editMinorNumber.Text = trStorage.EditNumberMinor.strVal;
	CheckRadioButton(IDC_TRIPARTITE_SELECT_UNITS_A, IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED, trStorage.RadioID.nVal);
	if( trStorage.RadioID.nVal == IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED)
	{
		m_editAValue.Text = trStorage.EditAValue.strVal;
		m_editALabel.Text = trStorage.EditALabel.strVal;
		m_editBValue.Text = trStorage.EditBValue.strVal;
		m_editBLabel.Text = trStorage.EditBLabel.strVal;
	}
	UpdateABValueAndLable(trStorage.RadioID.nVal);
}

BOOL TripartiteGridUnitsDlg::OnEditChange(Control ctrl)
{
	BOOL bEnable = TRUE;
	if(!is_numeric(m_editAValue.Text))
		bEnable =  false;
	if(!is_numeric(m_editBValue.Text))
		bEnable = false;
	if(!is_numeric(m_editMinorNumber.Text))
		bEnable = false;
	UpdateApplyButtonEnable(bEnable);
	return TRUE;
}

BOOL TripartiteGridUnitsDlg::OnRadioChange(Control ctrl)
{
	int nId = GetCheckedRadioButton(IDC_TRIPARTITE_SELECT_UNITS_A, IDC_TRIPARTITE_SELECT_UNITS_USER_DEFINED);
	UpdateABValueAndLable(nId);
	UpdateApplyButtonEnable(TRUE);
	return TRUE;
}

BOOL TripartiteGridUnitsDlg::OnClickHelpBTN(Control ctrl)
{
	return TRUE;
}

void TripartiteGridUnitsDlg::UpdateApplyButtonEnable(BOOL bEnable)
{
	GetItem(IDC_APPLY).Enable = bEnable;
}

BOOL  TripartiteGridUnitsDlg::GetApplyButtonEnable()
{
	return GetItem(IDC_APPLY).Enable;
}

BOOL TripartiteGridUnitsDlg::OnOK()
{
	if(GetApplyButtonEnable())
	{
		Control ctrl;
		OnApply(ctrl);
	}
	return TRUE;
}

BOOL TripartiteGridUnitsDlg::OnApply(Control ctrl)
{
	BOOL bRet = UpdateReferenceLines(m_gl, m_editAValue.Text, m_editALabel.Text, m_editBValue.Text, m_editBLabel.Text, m_editMinorNumber.Text );
	UpdateApplyButtonEnable(!bRet);
	if(bRet)
		SaveToStorage();
	return bRet;;
}

BOOL UpdateXRefLinesCount(GraphLayer& gl, TreeNode &trNode, int nCount)
{
	if( trNode.RefLines)
	{
		trNode.RefLines.Remove();
	}

	Tree trTemp;
	trTemp.Root.Axes.X.RefLines.Count.nVal = nCount;
	int nRet = gl.UpdateThemeIDs(trTemp.Root);
	if( 0 != nRet ){
		ASSERT(FALSE);
		return false;
	}
	BOOL bRet = gl.ApplyFormat(dwSTYLE, trTemp);
	gl.Invalidate(FALSE);
	if( !bRet ){
		ASSERT(FALSE);
		return false;
	}
	return true;
}

BOOL UpdateYRefLinesCount(GraphLayer& gl, TreeNode &trNode, int nCount)
{
	if( trNode.RefLines)
	{
		trNode.RefLines.Remove();
	}
	Tree trTemp;
	trTemp.Root.Axes.Y.RefLines.Count.nVal = nCount;
	int nRet = gl.UpdateThemeIDs(trTemp.Root);
	if( 0 != nRet ){
		ASSERT(FALSE);
		return false;
	}
	BOOL bRet = gl.ApplyFormat(dwSTYLE, trTemp);
	gl.Invalidate(FALSE);
	if( !bRet ){
		ASSERT(FALSE);
		return false;
	}
	return true;
}

BOOL UpdateReferenceLines(GraphLayer& gl, string strAValue, string strALabel, string strBValue, string strBLabel, string strNumberMinorValue)
{
	if(!gl)
		return false;
	if(strAValue.IsEmpty() || strBValue.IsEmpty()  || strALabel.IsEmpty() || strBLabel.IsEmpty()  || strNumberMinorValue.IsEmpty())
	{
		Tree trStorage;
		if(!gl.GetBinaryStorage(STR_TRIPAERTITE_GRID_UNIT_STORAGE, trStorage) )
		{
			return false;
		}
		strAValue = trStorage.EditAValue.strVal;
		strALabel = trStorage.EditALabel.strVal;
		strBValue = trStorage.EditBValue.strVal;
		strBLabel = trStorage.EditBLabel.strVal;
		strNumberMinorValue = trStorage.EditNumberMinor.strVal;
	}
	double dAValue, dBValue, dMinor;
	if(!_get_numeric_from_text(strAValue, dAValue)|| 
		!_get_numeric_from_text(strBValue, dBValue))
		return false;
	if(!_get_numeric_from_text(strNumberMinorValue, dMinor))
		return false;
	
	TreeNode trTheme = gl.GetFormat(FPB_ALL, FOB_AXIS_REFLINES, TRUE, TRUE);
	TreeNode trXRefLines = trTheme.Root.Axes.X.RefLines;
	TreeNode trYRefLines = trTheme.Root.Axes.Y.RefLines;
	if(!trXRefLines.IsValid() || !trYRefLines.IsValid())
		return false;
		
	double dXFrom = gl.X.From;
	double dXTo	= gl.X.To;
	double dYFrom = gl.Y.From;
	double dYTo	= gl.Y.To;
	
	double dXMaxDeg = (dAValue * dYTo) / dXFrom;
	double d45DegTo = floor(log10(dXMaxDeg));
	double dXMinDeg = (dAValue * dYFrom) / dXTo;
	double d45DegFrom = (int)ceil(log10(dXMinDeg));
	if(d45DegTo < d45DegFrom)
		swap(d45DegTo, d45DegFrom);
	int nXNumberShow = (int)(d45DegTo - d45DegFrom + 1);

	double dYMaxDeg = dBValue * dXTo * dYTo;
	double d135DegTo =  floor(log10(dYMaxDeg));
	double dYMinDeg = dBValue * dXFrom * dYFrom;
	double d135DegFrom = ceil(log10(dYMinDeg));
	if(d135DegTo < d135DegFrom)
		swap(d135DegTo, d135DegFrom);
	int nYNumberShow = (int)(d135DegTo - d135DegFrom + 1);
	
	int nXMinorNumber = (nXNumberShow + 1) * (int)dMinor;
	int nYMinorNumber = (nYNumberShow + 1) * (int)dMinor;
	
	int nXRefLinesCount = nXMinorNumber + nXNumberShow + AXIS_BENGIN_AND_AXIS_END;
	int nYRefLinesCount = nYMinorNumber + nYNumberShow + AXIS_BENGIN_AND_AXIS_END;
	BOOL bXSameCount = nXRefLinesCount == trXRefLines.Count.nVal;
	BOOL bYSameCount = nYRefLinesCount == trYRefLines.Count.nVal;
	if( !(bXSameCount || UpdateXRefLinesCount(gl, trXRefLines, nXRefLinesCount)) || 
		!(bYSameCount || UpdateYRefLinesCount(gl, trYRefLines, nYRefLinesCount))	)
		return false;
	if(!bYSameCount || !bYSameCount)
		_update_x_y_axis_begin_and_end_Value(gl, trTheme, trXRefLines, trYRefLines );
	
	//create X reference lines
	TreeNodeCollection trXRefLineColl(trXRefLines.RefLines, E_STR_REFLINE_PREFIX);
	int nTemp = (int)d45DegTo;
	int nAddMinorGrid = 0;
	double dStep = 0;
	foreach(TreeNode trXNode in trXRefLineColl)
	{
		if(trXNode.Type.nVal == RefLineCtrl_Pos_Begin || trXNode.Type.nVal==RefLineCtrl_Pos_End)
		{
			continue;
		}
		//add show reference line
		if(d45DegFrom <= nTemp)
		{
			double dTickValue = pow(10, nTemp);
			string strFormula = "%s*Y/%g";
			strFormula.Format(strFormula, strAValue, dTickValue);
			string strLabel = "%g %s";
			strLabel.Format(strLabel, dTickValue, strALabel);
			_update_reference_line_value(trXNode, strFormula, strLabel);
		}
		//--
		else
		{
			if(nAddMinorGrid == 0)
			{
				nAddMinorGrid = (int)dMinor;
				if((int)dMinor == 1)
					dStep = (pow(10, d45DegTo + 1) -  2 * pow(10, d45DegTo)) / 2.0;
				else
					dStep = (pow(10, d45DegTo + 1) -  pow(10, d45DegTo)) / (dMinor + 1.0);
				d45DegTo -= 1;
			}
			double dMinorTickValue = pow(10, d45DegTo + 1) + nAddMinorGrid * dStep;
			string strFormula = "%s*Y/%g";
			strFormula.Format(strFormula, strAValue, dMinorTickValue);
			nAddMinorGrid -= 1;
			_update_reference_line_value(trXNode, strFormula, "", true);
		}
		nTemp -= 1;
	}
	
	//create Y reference lines
	TreeNodeCollection trYRefLineColl(trYRefLines.RefLines, E_STR_REFLINE_PREFIX);
	nTemp = (int)d135DegTo;
	nAddMinorGrid = 0;
	dStep = 0;
	foreach( TreeNode trYNode in trYRefLineColl)
	{
		if(trYNode.Type.nVal == RefLineCtrl_Pos_Begin || trYNode.Type.nVal==RefLineCtrl_Pos_End)
		{
			continue;
		}
		if(d135DegFrom <= nTemp)
		{
			double dTickValue = pow(10, (int)nTemp);
			string strFormula = "%g/(%s * x)";
			strFormula.Format(strFormula,dTickValue, strBValue);
			string strLabel = "%g %s";
			strLabel.Format(strLabel, dTickValue, strBLabel);
			_update_reference_line_value(trYNode, strFormula, strLabel);
		}
		else
		{
			if(nAddMinorGrid == 0)
			{
				nAddMinorGrid = (int)dMinor;
				if((int)dMinor == 1)
					dStep = (pow(10, d135DegTo + 1) -  2 * pow(10, d135DegTo)) / 2.0;
				else
					dStep = (pow(10, d135DegTo + 1) -  pow(10, d135DegTo)) / (dMinor + 1.0);
				d135DegTo -= 1;
			}
			double dMinorTickValue = pow(10, d135DegTo + 1) + nAddMinorGrid * dStep;
			string strFormula = "%g/(%s * x)";
			strFormula.Format(strFormula, dMinorTickValue, strBValue);
			nAddMinorGrid -= 1;
			_update_reference_line_value(trYNode, strFormula, "", true);
		}
		nTemp -= 1;
	}
	BOOL bRet = gl.ApplyFormat(dwSTYLE, trTheme);
	gl.Invalidate(FALSE);	
	return bRet;
}

void CreateButtonInTripartiteGraph(GraphLayer& gl)
{
	GraphObject goTextButton = gl.GraphObjects(STR_TRIPAERTITE_GRID_UNIT_BUTTON);
	if(goTextButton)
	{
		goTextButton.Text = STR_TRIPAERTITE_GRID_UNIT_BUTTON;
	}
	else
	{
		add_text(gl, goTextButton, 90, 5, STR_TRIPAERTITE_GRID_UNIT_BUTTON, 0, true, ATTACH_TO_PAGE);
		goTextButton.SetName(STR_TRIPAERTITE_GRID_UNIT_BUTTON);
		string strLT = "string path=%@Y;\
						path.Delete(path.GetLength());\
						int slash=path.ReverseFind('\');\
						path$=path.Left(slash)$;\
						page.active = 1;\
						run.LoadOC(%(path$)Templates\TripartiteGridUnitsDlg.c, 16);\
						run -oc {OpenTripartiteGridUnitDLG();}";
		set_LT_script(goTextButton, strLT, GRCT_MOUSEUP);
		set_textbutton_pos(gl, goTextButton, TRUE);
	}
}


BOOL OpenTripartiteGridUnitDLG()
{
	TripartiteGridUnitsDlg TGUDlg();
	
	HWND hWnd = GetWindow(OGW_MAIN);
	if(IDOK == TGUDlg.DoModalEx(hWnd))
	{
		return true;//click ok;
	}
	return false;
}