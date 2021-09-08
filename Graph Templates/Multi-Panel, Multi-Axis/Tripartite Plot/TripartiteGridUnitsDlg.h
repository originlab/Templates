/*------------------------------------------------------------------------------*
 * File Name:				 													*
 * Creation: 																	*
 * Purpose: OriginC Source C file												*
 * Copyright (c) ABCD Corp.	2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010		*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
#include "DialogEx.h"
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