
// FilterKeySettingDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FilterKeySetting.h"
#include "FilterKeySettingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable: 4996)

// CFilterKeySettingDlg dialog


static LONG GetDWORDRegKey(HKEY hKey, const WCHAR* strValueName, DWORD& nValue, DWORD nDefaultValue)
{
	nValue = nDefaultValue;
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey, strValueName, 0, NULL, (LPBYTE)&nResult, &dwBufferSize);
	if (ERROR_SUCCESS == nError) {
		nValue = nResult;
	}
	return nError;
}

static LONG GetBoolRegKey(HKEY hKey, const WCHAR* strValueName, bool& bValue, bool bDefaultValue)
{
	DWORD nDefValue((bDefaultValue) ? 1 : 0);
	DWORD nResult(nDefValue);
	LONG nError = GetDWORDRegKey(hKey, strValueName, nResult, nDefValue);
	if (ERROR_SUCCESS == nError) {
		bValue = (nResult != 0) ? true : false;
	}
	return nError;
}

static LONG GetStringRegKey(HKEY hKey, const WCHAR* strValueName, WCHAR* szValue, DWORD cbValue, const WCHAR* strDefaultValue)
{
	wcsncpy(szValue, strDefaultValue, cbValue / sizeof(wchar_t));
	DWORD dwBufferSize = cbValue;
	ULONG nError = ::RegQueryValueExW(hKey, strValueName, 0, NULL, (LPBYTE)szValue, &dwBufferSize);
	if (ERROR_SUCCESS != nError) {
		wcsncpy(szValue, strDefaultValue, cbValue / sizeof(wchar_t));
	}
	return nError;
}

static LONG GetNumericStringRegKey(HKEY hKey, const WCHAR* strValueName, INT& nValue, INT nDefaultValue)
{
	nValue = nDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	LONG nError = GetStringRegKey(hKey, strValueName, szBuffer, sizeof(szBuffer), L"");
	if (ERROR_SUCCESS == nError) {
		nValue = _wtoi(szBuffer);
	}
	return nError;
}

static LONG GetNumericStringRegKey(HKEY hKey, const WCHAR* strValueName, DWORD& nValue, DWORD nDefaultValue)
{
	INT iValue;
	INT iDefaultValue = (INT)nDefaultValue;
	LONG nError = GetNumericStringRegKey(hKey, strValueName, iValue, iDefaultValue);
	nValue = iValue;
	return nError;
}


CFilterKeySettingDlg::CFilterKeySettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTERKEYSETTING_DIALOG, pParent)
	, m_bUpdateIniFile(TRUE)
	, m_bSendChange(FALSE)
	, m_nLastBtn(eCus_1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterKeySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_WAIT_EDIT, m_nWait);
	//DDV_MinMaxInt(pDX, m_nWait, 0, 20000);
	DDX_Text(pDX, IDC_DELAY_EDIT, m_nDelay);
	//DDV_MinMaxInt(pDX, m_nDelay, 0, 20000);
	DDX_Text(pDX, IDC_REPEAT_EDIT, m_nRepeat);
	//DDV_MinMaxInt(pDX, m_nRepeat, 0, 20000);

	DDX_Check(pDX, IDC_ON, m_bOn);
	DDX_Check(pDX, IDC_AVAILABLE, m_bAvailable);
	DDX_Check(pDX, IDC_HOTKEYACTIVE, m_bHotKeyActive);
	DDX_Check(pDX, IDC_CONFIRMHOTKEY, m_bConfirmHotKey);
	DDX_Check(pDX, IDC_HOTKEYSOUND, m_bHotKeySound);
	DDX_Check(pDX, IDC_INDICATOR, m_bIndicator);
	DDX_Check(pDX, IDC_CLICK, m_bClick);

	DDX_Control(pDX, IDC_WAIT_EDIT, m_editWait);
	DDX_Control(pDX, IDC_DELAY_EDIT, m_editDelay);
	DDX_Control(pDX, IDC_REPEAT_EDIT, m_editRepeat);

	DDX_Control(pDX, IDC_FLAGVAL, m_staticFlagVal);
	DDX_Control(pDX, IDC_CHARS_PER_SEC, m_staticCharsPerSec);

	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFilterKeySettingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_ON, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_AVAILABLE, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_INDICATOR, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_CLICK, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_HOTKEYACTIVE, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_CONFIRMHOTKEY, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_HOTKEYSOUND, OnBnClickedFlag)

	ON_EN_CHANGE(IDC_REPEAT_EDIT, OnEnChangeRepeatEdit)

	ON_BN_CLICKED(IDC_APPLY, &CFilterKeySettingDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_SET_BTN_CUS1, &CFilterKeySettingDlg::OnBnClickedSetBtnCustom1)
	ON_BN_CLICKED(IDC_SET_BTN_CUS2, &CFilterKeySettingDlg::OnBnClickedSetBtnCustom2)
	ON_BN_CLICKED(IDC_SET_BTN_CUS3, &CFilterKeySettingDlg::OnBnClickedSetBtnCustom3)
	ON_BN_CLICKED(IDC_SET_BTN_CUS4, &CFilterKeySettingDlg::OnBnClickedSetBtnCustom4)
	ON_BN_CLICKED(IDC_SET_BTN_CUS5, &CFilterKeySettingDlg::OnBnClickedSetBtnCustom5)
	ON_BN_CLICKED(IDC_BTN_TITLE_UPDATE, &CFilterKeySettingDlg::OnBnClickedBtnTitleUpdate)
END_MESSAGE_MAP()


// CFilterKeySettingDlg message handlers

BOOL CFilterKeySettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	for (int i = 0; i < eCustom_max; i++)
	{
		UpdateButton(i);
	}

	int last = theApp.GetProfileInt(SEC_CONFIG, DEF_BTN_LAST, eCus_1);
	UpdateTitleWindow(last);

	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(last, &filter_keys);
	SetValues(filter_keys);

	SetWindowText(_T("FilterKeySetting"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFilterKeySettingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFilterKeySettingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CFilterKeySettingDlg::GetButtonTitle(int idx)
{
	CString strKey;
	strKey.Format(DEF_BTN_TITLE, idx + 1);

	return theApp.GetProfileString(SEC_CONFIG, strKey, DEF_DEFAULT_TITLE[idx]);
}

void CFilterKeySettingDlg::SetButtonTitle(int idx, CString title)
{
	CString strKey;
	strKey.Format(DEF_BTN_TITLE, idx + 1);

	theApp.WriteProfileString(SEC_CONFIG, strKey, title);
	SetDlgItemText(IDC_SET_BTN_CUS1 + idx, title);

	UpdateTitleWindow(idx);
}

void CFilterKeySettingDlg::UpdateButton(int idx)
{
	GetDlgItem(IDC_SET_BTN_CUS1 + idx)->SetWindowText(GetButtonTitle(idx));
}

void CFilterKeySettingDlg::SaveCustomValue(int idx, FILTERKEYS& filter_keys)
{
	CString strKey;

	strKey.Format(DEF_VAL_WAITMSEC, idx + 1);
	theApp.WriteProfileInt(SEC_CONFIG, strKey, filter_keys.iWaitMSec);

	strKey.Format(DEF_VAL_DELAYMSEC, idx + 1);
	theApp.WriteProfileInt(SEC_CONFIG, strKey, filter_keys.iDelayMSec);

	strKey.Format(DEF_VAL_REPEATMSEC, idx + 1);
	theApp.WriteProfileInt(SEC_CONFIG, strKey, filter_keys.iRepeatMSec);

	strKey.Format(DEF_VAL_FLAGS, idx + 1);
	theApp.WriteProfileInt(SEC_CONFIG, strKey, filter_keys.dwFlags);
}

void CFilterKeySettingDlg::LoadCustomValue(int idx, FILTERKEYS* filter_keys)
{
	CString strKey;

	strKey.Format(DEF_VAL_WAITMSEC, idx + 1);
	filter_keys->iWaitMSec		= theApp.GetProfileInt(SEC_CONFIG, strKey, DEF_DEFAULT_VALUE[idx][eValue_WaitTime]);

	strKey.Format(DEF_VAL_DELAYMSEC, idx + 1);
	filter_keys->iDelayMSec		= theApp.GetProfileInt(SEC_CONFIG, strKey, DEF_DEFAULT_VALUE[idx][eValue_DelayTime]);

	strKey.Format(DEF_VAL_REPEATMSEC, idx + 1);
	filter_keys->iRepeatMSec	= theApp.GetProfileInt(SEC_CONFIG, strKey, DEF_DEFAULT_VALUE[idx][eValue_RepeatTime]);

	strKey.Format(DEF_VAL_FLAGS, idx + 1);
	filter_keys->dwFlags		= theApp.GetProfileInt(SEC_CONFIG, strKey, DEF_DEFAULT_VALUE[idx][eValue_Flag]);

	filter_keys->iBounceMSec	= 0;
}

void CFilterKeySettingDlg::OnBnClickedFlag()
{
	UpdateControls();
}

void CFilterKeySettingDlg::OnEnChangeRepeatEdit()
{
	UpdateCharsPerSec();
}

void CFilterKeySettingDlg::OnBnClickedApply()
{
	if (!UpdateData(TRUE)) {
		return;
	}

	if (SaveSettings() == true)
	{
		AfxMessageBox(_T("Ok"));
	}
	else
	{
		AfxMessageBox(_T("Set fail"));
	}
}

void CFilterKeySettingDlg::SetValues(const FILTERKEYS& filter_keys)
{
	UpdateData(TRUE);
	m_nWait = filter_keys.iWaitMSec;
	m_nDelay = filter_keys.iDelayMSec;
	m_nRepeat = filter_keys.iRepeatMSec;
	m_nBounce = filter_keys.iBounceMSec;
	SetFlags(filter_keys.dwFlags);
	UpdateData(FALSE);
	UpdateControls();
}

void CFilterKeySettingDlg::UpdateControls()
{
	UpdateData(TRUE);
	UpdateFlagVal();
	UpdateCharsPerSec();
}

void CFilterKeySettingDlg::UpdateCharsPerSec()
{
	CString s;
	GetDlgItemText(IDC_REPEAT_EDIT, s);
	int val = atoi(s);
	if (val) {
		s.Format("(%.1f per second)", 1000.0 / val);
	}
	else {
		s = "(0 per second)";
	}
	m_staticCharsPerSec.SetWindowText(s);
}

void CFilterKeySettingDlg::UpdateTitleWindow(int idx)
{
	theApp.WriteProfileInt(SEC_CONFIG, DEF_BTN_LAST, idx);
	SetDlgItemText(IDC_STATIC_TITLE, GetButtonTitle(idx));
	m_nLastBtn = idx;
}

void CFilterKeySettingDlg::SetFlags(DWORD dwFlags)
{
	m_bOn = !!(dwFlags & FKF_FILTERKEYSON);
	m_bAvailable = !!(dwFlags & FKF_AVAILABLE);
	m_bIndicator = !!(dwFlags & FKF_INDICATOR);
	m_bClick = !!(dwFlags & FKF_CLICKON);
	m_bHotKeyActive = !!(dwFlags & FKF_HOTKEYACTIVE);
	m_bConfirmHotKey = !!(dwFlags & FKF_CONFIRMHOTKEY);
	m_bHotKeySound = !!(dwFlags & FKF_HOTKEYSOUND);
}

DWORD CFilterKeySettingDlg::GetFlagsVal()
{
	DWORD dwFlags = 0;
	if (m_bOn) dwFlags |= FKF_FILTERKEYSON;
	if (m_bAvailable) dwFlags |= FKF_AVAILABLE;
	if (m_bIndicator) dwFlags |= FKF_INDICATOR;
	if (m_bClick) dwFlags |= FKF_CLICKON;
	if (m_bHotKeyActive) dwFlags |= FKF_HOTKEYACTIVE;
	if (m_bConfirmHotKey) dwFlags |= FKF_CONFIRMHOTKEY;
	if (m_bHotKeySound) dwFlags |= FKF_HOTKEYSOUND;
	return dwFlags;
}

void CFilterKeySettingDlg::UpdateFlagVal()
{
	DWORD dwFlags = GetFlagsVal();
	CString s; s.Format("(%u)", dwFlags);
	m_staticFlagVal.SetWindowText(s);
}

bool CFilterKeySettingDlg::SaveSettings()
{
	if (m_nWait > 20000) {
		AfxMessageBox("Ignore under value is too large.\nMaximum value is 20000.");
		m_editWait.SetFocus();
		return false;
	}
	if (m_nDelay > 20000) {
		AfxMessageBox("Repeat delay value is too large.\nMaximum value is 20000.");
		m_editDelay.SetFocus();
		return false;
	}
	if (m_nRepeat > 20000) {
		AfxMessageBox("Repeat rate value is too large.\nMaximum value is 20000.");
		m_editRepeat.SetFocus();
		return false;
	}


	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	filter_keys.iWaitMSec = m_nWait;
	filter_keys.iDelayMSec = m_nDelay;
	filter_keys.iRepeatMSec = m_nRepeat;
	filter_keys.dwFlags = GetFlagsVal();

	UINT fWinIni = 0;
	if (m_bUpdateIniFile) fWinIni |= SPIF_UPDATEINIFILE;
	if (m_bSendChange) fWinIni |= SPIF_SENDCHANGE;
	bool ok = !!SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &filter_keys, fWinIni);
	if (!ok) {
		AfxMessageBox("Failed to save new settings.");
	}

	SaveCustomValue(m_nLastBtn, filter_keys);

	return ok;
}

void CFilterKeySettingDlg::OnBnClickedSetBtnCustom1()
{
	UpdateTitleWindow(eCus_1);
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(eCus_1, &filter_keys);
	SetValues(filter_keys);
}

void CFilterKeySettingDlg::OnBnClickedSetBtnCustom2()
{
	UpdateTitleWindow(eCus_2);
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(eCus_2, &filter_keys);
	SetValues(filter_keys);
}

void CFilterKeySettingDlg::OnBnClickedSetBtnCustom3()
{
	UpdateTitleWindow(eCus_3);
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(eCus_3, &filter_keys);
	SetValues(filter_keys);
}

void CFilterKeySettingDlg::OnBnClickedSetBtnCustom4()
{
	UpdateTitleWindow(eCus_4);
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(eCus_4, &filter_keys);
	SetValues(filter_keys);
}

void CFilterKeySettingDlg::OnBnClickedSetBtnCustom5()
{
	UpdateTitleWindow(eCus_5);
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	LoadCustomValue(eCus_5, &filter_keys);
	SetValues(filter_keys);
}

void CFilterKeySettingDlg::OnBnClickedBtnTitleUpdate()
{
	CString strNewTitle;
	GetDlgItemText(IDC_EDIT1, strNewTitle);

	SetButtonTitle(m_nLastBtn, strNewTitle);
}

BOOL CFilterKeySettingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
