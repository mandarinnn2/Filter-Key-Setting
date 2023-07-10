
// FilterKeySettingDlg.h : header file
//

#pragma once

enum {
	eCus_1,
	eCus_2,
	eCus_3,
	eCus_4,
	eCus_5,

	eCustom_max
};

enum {
	eValue_WaitTime,
	eValue_DelayTime,
	eValue_RepeatTime,
	eValue_Flag,

	eValue_max
};

static LPCTSTR DEF_DEFAULT_TITLE[eCustom_max] = {
	"SET1",
	"SET2",
	"SET3",
	"SET4",
	"SET5"
};

static int DEF_DEFAULT_VALUE[eCustom_max][eValue_max] = {
	{ 0,	200,	30,		35 },
	{ 0,	200,	25,		35 },
	{ 0,	200,	20,		35 },
	{ 0,	100,	20,		35 },
	{ 0,	100,	15,		35 },
};

#define		SEC_CONFIG			(_T("Config"))
#define DEF_BTN_LAST			(_T("LastSettingBtn"))
#define DEF_BTN_TITLE			(_T("ButtonTitle_%d"))

#define DEF_VAL_WAITMSEC		(_T("Option_%d_WaitMSec"))
#define DEF_VAL_DELAYMSEC		(_T("Option_%d_DelayMSec"))
#define DEF_VAL_REPEATMSEC		(_T("Option_%d_RepeatMSec"))
#define DEF_VAL_FLAGS			(_T("Option_%d_Flags"))

// CFilterKeySettingDlg dialog
class CFilterKeySettingDlg : public CDialogEx
{
// Construction
public:
	CFilterKeySettingDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTERKEYSETTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
	CString GetButtonTitle(int idx);
	void SetButtonTitle(int idx, CString title);
	void UpdateButton(int idx);

	void SaveCustomValue(int idx, FILTERKEYS& filter_keys);
	void LoadCustomValue(int idx, FILTERKEYS* filter_keys);

public:
	afx_msg void OnBnClickedFlag();
	afx_msg void OnEnChangeRepeatEdit();
	afx_msg void OnBnClickedApply();


public:
	int	m_nLastBtn;

	FILTERKEYS m_fkOriginal;

	int m_nWait;
	int m_nDelay;
	int m_nRepeat;
	int m_nBounce;

	BOOL m_bOn;
	BOOL m_bAvailable;
	BOOL m_bIndicator;
	BOOL m_bClick;
	BOOL m_bHotKeyActive;
	BOOL m_bConfirmHotKey;
	BOOL m_bHotKeySound;

	BOOL m_bUpdateIniFile;
	BOOL m_bSendChange;
	//

	CStatic m_staticFlagVal;
	CStatic m_staticCharsPerSec;

	CEdit m_editWait;
	CEdit m_editDelay;
	CEdit m_editRepeat;
	CEdit m_editBounce;

public:

	void SetValues(const FILTERKEYS& filter_keys);
	void SetFlags(DWORD dwFlags);
	DWORD GetFlagsVal();

	void UpdateFlagVal();
	void UpdateControls();
	void UpdateCharsPerSec();
	void UpdateTitleWindow(int idx);

	bool SaveSettings();
	afx_msg void OnBnClickedSetBtnCustom1();
	afx_msg void OnBnClickedSetBtnCustom2();
	afx_msg void OnBnClickedSetBtnCustom3();
	afx_msg void OnBnClickedSetBtnCustom4();
	afx_msg void OnBnClickedSetBtnCustom5();
	afx_msg void OnBnClickedBtnTitleUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
