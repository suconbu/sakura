﻿/*!	@file
	@brief GREP置換ダイアログボックス

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, Stonee, genta
	Copyright (C) 2002, MIK, genta, Moca, YAZAKI
	Copyright (C) 2003, Moca
	Copyright (C) 2006, ryoji
	Copyright (C) 2010, ryoji
	Copyright (C) 2014, Moca
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#include "StdAfx.h"
#include "dlg/CDlgGrepReplace.h"
#include "view/CEditView.h"
#include "func/Funccode.h"		// Stonee, 2001/03/12
#include "util/module.h"
#include "util/shell.h"
#include "util/os.h"
#include "util/window.h"
#include "env/DLLSHAREDATA.h"
#include "env/CSakuraEnvironment.h"
#include "apiwrap/StdApi.h"
#include "apiwrap/StdControl.h"
#include "CSelectLang.h"
#include "sakura_rc.h"
#include "sakura.hh"
#include "String_define.h"

const DWORD p_helpids[] = {
	IDC_BUTTON_FOLDER,				HIDC_GREP_REP_BUTTON_FOLDER,			//フォルダー
	IDC_BUTTON_CURRENTFOLDER,		HIDC_GREP_REP_BUTTON_CURRENTFOLDER,		//現フォルダー
	IDOK,							HIDOK_GREP_REP,							//置換開始
	IDCANCEL,						HIDCANCEL_GREP_REP,						//キャンセル
	IDC_BUTTON_HELP,				HIDC_GREP_REP_BUTTON_HELP,				//ヘルプ
	IDC_CHK_PASTE,					HIDC_GREP_REP_CHK_PASTE,				//クリップボードから貼り付け
	IDC_CHK_WORD,					HIDC_GREP_REP_CHK_WORD,					//単語単位
	IDC_CHK_SUBFOLDER,				HIDC_GREP_REP_CHK_SUBFOLDER,			//サブフォルダーも検索
	IDC_CHK_FROMTHISTEXT,			HIDC_GREP_REP_CHK_FROMTHISTEXT,			//編集中のテキストから検索
	IDC_CHK_LOHICASE,				HIDC_GREP_REP_CHK_LOHICASE,				//大文字小文字
	IDC_CHK_REGULAREXP,				HIDC_GREP_REP_CHK_REGULAREXP,			//正規表現
	IDC_CHK_BACKUP,					HIDC_GREP_REP_CHK_BACKUP,				//バックアップ作成
	IDC_COMBO_CHARSET,				HIDC_GREP_REP_COMBO_CHARSET,			//文字コードセット
	IDC_CHECK_CP,					HIDC_GREP_REP_CHECK_CP,					//CP
	IDC_COMBO_TEXT,					HIDC_GREP_REP_COMBO_TEXT,				//置換前
	IDC_COMBO_TEXT2,				HIDC_GREP_REP_COMBO_TEXT2,				//置換後
	IDC_COMBO_FILE,					HIDC_GREP_REP_COMBO_FILE,				//ファイル
	IDC_COMBO_FOLDER,				HIDC_GREP_REP_COMBO_FOLDER,				//フォルダー
	IDC_BUTTON_FOLDER_UP,			HIDC_GREP_REP_BUTTON_FOLDER_UP,			//上
	IDC_RADIO_OUTPUTLINE,			HIDC_GREP_REP_RADIO_OUTPUTLINE,			//結果出力：行単位
	IDC_RADIO_OUTPUTMARKED,			HIDC_GREP_REP_RADIO_OUTPUTMARKED,		//結果出力：該当部分
	IDC_RADIO_OUTPUTSTYLE1,			HIDC_GREP_REP_RADIO_OUTPUTSTYLE1,		//結果出力形式：ノーマル
	IDC_RADIO_OUTPUTSTYLE2,			HIDC_GREP_REP_RADIO_OUTPUTSTYLE2,		//結果出力形式：ファイル毎
	IDC_RADIO_OUTPUTSTYLE3,			HIDC_GREP_REP_RADIO_OUTPUTSTYLE3,		//結果出力形式：結果のみ
	IDC_STATIC_JRE32VER,			HIDC_GREP_REP_STATIC_JRE32VER,			//正規表現バージョン
	IDC_CHK_DEFAULTFOLDER,			HIDC_GREP_REP_CHK_DEFAULTFOLDER,		//フォルダーの初期値をカレントフォルダーにする
	IDC_CHECK_FILE_ONLY,			HIDC_GREP_REP_CHECK_FILE_ONLY,			//ファイル毎最初のみ検索
	IDC_CHECK_BASE_PATH,			HIDC_GREP_REP_CHECK_BASE_PATH,			//ベースフォルダー表示
	IDC_CHECK_SEP_FOLDER,			HIDC_GREP_REP_CHECK_SEP_FOLDER,			//フォルダー毎に表示
	0, 0
};

CDlgGrepReplace::CDlgGrepReplace()
{
	if( 0 < m_pShareData->m_sSearchKeywords.m_aReplaceKeys.size() ){
		m_strText2 = m_pShareData->m_sSearchKeywords.m_aReplaceKeys[0];
	}
	return;
}

/* モーダルダイアログの表示 */
int CDlgGrepReplace::DoModal( HINSTANCE hInstance, HWND hwndParent, const WCHAR* pszCurrentFilePath, LPARAM lParam )
{
	m_bSubFolder = m_pShareData->m_Common.m_sSearch.m_bGrepSubFolder;			// Grep: サブフォルダーも検索
	m_sSearchOption = m_pShareData->m_Common.m_sSearch.m_sSearchOption;		// 検索オプション
	m_nGrepCharSet = m_pShareData->m_Common.m_sSearch.m_nGrepCharSet;			// 文字コードセット
	m_nGrepOutputLineType = m_pShareData->m_Common.m_sSearch.m_nGrepOutputLineType;	// 行を出力するか該当部分だけ出力するか
	m_nGrepOutputStyle = m_pShareData->m_Common.m_sSearch.m_nGrepOutputStyle;	// Grep: 出力形式
	m_bPaste = false;
	m_bBackup = m_pShareData->m_Common.m_sSearch.m_bGrepBackup;

	if( m_szFile[0] == L'\0' && m_pShareData->m_sSearchKeywords.m_aGrepFiles.size() ){
		wcscpy( m_szFile, m_pShareData->m_sSearchKeywords.m_aGrepFiles[0] );		/* 検索ファイル */
	}
	if( m_szFolder[0] == L'\0' && m_pShareData->m_sSearchKeywords.m_aGrepFolders.size() ){
		wcscpy( m_szFolder, m_pShareData->m_sSearchKeywords.m_aGrepFolders[0] );	/* 検索フォルダー */
	}
	
	/* 除外ファイル */
	if (m_szExcludeFile[0] == L'\0') {
		if (m_pShareData->m_sSearchKeywords.m_aExcludeFiles.size()) {
			wcscpy(m_szExcludeFile, m_pShareData->m_sSearchKeywords.m_aExcludeFiles[0]);
		}
		else {
			/* ユーザーの利便性向上のために除外ファイルに対して初期値を設定する */
			wcscpy(m_szExcludeFile, DEFAULT_EXCLUDE_FILE_PATTERN);	/* 除外ファイル */

			/* 履歴に残して後で選択できるようにする */
			m_pShareData->m_sSearchKeywords.m_aExcludeFiles.push_back(DEFAULT_EXCLUDE_FILE_PATTERN);
		}
	}

	/* 除外フォルダー */
	if (m_szExcludeFolder[0] == L'\0') {
		if (m_pShareData->m_sSearchKeywords.m_aExcludeFolders.size()) {
			wcscpy(m_szExcludeFolder, m_pShareData->m_sSearchKeywords.m_aExcludeFolders[0]);
		}
		else {
			/* ユーザーの利便性向上のために除外フォルダーに対して初期値を設定する */
			wcscpy(m_szExcludeFolder, DEFAULT_EXCLUDE_FOLDER_PATTERN);	/* 除外フォルダー */
			
			/* 履歴に残して後で選択できるようにする */
			m_pShareData->m_sSearchKeywords.m_aExcludeFolders.push_back(DEFAULT_EXCLUDE_FOLDER_PATTERN);
		}
	}

	if( pszCurrentFilePath ){	// 2010.01.10 ryoji
		wcscpy(m_szCurrentFilePath, pszCurrentFilePath);
	}

	return (int)CDialog::DoModal( hInstance, hwndParent, IDD_GREP_REPLACE, lParam );
}

BOOL CDlgGrepReplace::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	_SetHwnd( hwndDlg );

	/* コンボボックスのユーザー インターフェースを拡張インターフェースにする */
	Combo_SetExtendedUI( GetItemHwnd( IDC_COMBO_TEXT2 ), TRUE );

	SetComboBoxDeleter(GetItemHwnd(IDC_COMBO_TEXT2), &m_cRecentReplace);

	BOOL bRet = CDlgGrep::OnInitDialog( hwndDlg, wParam, lParam );
	if( !bRet ) return bRet;

	HFONT hFontOld = (HFONT)::SendMessageAny( GetItemHwnd( IDC_COMBO_TEXT2 ), WM_GETFONT, 0, 0 );
	HFONT hFont = SetMainFont( GetItemHwnd( IDC_COMBO_TEXT2 ) );
	m_cFontText2.SetFont( hFontOld, hFont, GetItemHwnd( IDC_COMBO_TEXT2 ) );

	return bRet;
}

BOOL CDlgGrepReplace::OnCbnDropDown( HWND hwndCtl, int wID )
{
	switch( wID ){
	case IDC_COMBO_TEXT2:
		if ( ::SendMessage(hwndCtl, CB_GETCOUNT, 0L, 0L) == 0) {
			const auto& keys = m_pShareData->m_sSearchKeywords.m_aReplaceKeys;
			for( int i = 0; i < keys.size(); ++i ){
				Combo_AddString( hwndCtl, keys[i] );
			}
		}
		break;
	}
	return CDlgGrep::OnCbnDropDown( hwndCtl, wID );
}

BOOL CDlgGrepReplace::OnDestroy()
{
	m_cFontText2.ReleaseOnDestroy();
	return CDlgGrep::OnDestroy();
}

BOOL CDlgGrepReplace::OnBnClicked( int wID )
{
	switch( wID ){
	case IDC_BUTTON_HELP:
		MyWinHelp( GetHwnd(), HELP_CONTEXT, ::FuncID_To_HelpContextID(F_GREP_REPLACE_DLG) );
		return TRUE;
	case IDC_CHK_PASTE:
	case IDOK:
		{
			bool bStop = false;
			CEditView* pcEditView = (CEditView*)m_lParam;
			if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_PASTE ) &&
				!pcEditView->m_pcEditDoc->m_cDocEditor.IsEnablePaste() ){
				OkMessage( GetHwnd(), LS(STR_DLGREPLC_CLIPBOARD) );
				::CheckDlgButton( GetHwnd(), IDC_CHK_PASTE, FALSE );
				bStop = true;
			}
			::DlgItem_Enable( GetHwnd(), IDC_COMBO_TEXT2, !IsDlgButtonCheckedBool( GetHwnd(), IDC_CHK_PASTE ) );
			if( wID == IDOK && bStop ){
				return TRUE;
			}
		}
	}
	/* 基底クラスメンバ */
	return CDlgGrep::OnBnClicked( wID );
}

/* ダイアログデータの設定 */
void CDlgGrepReplace::SetData( void )
{
	/* 置換後 */
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_TEXT2, m_strText2.c_str() );
	
	CheckDlgButtonBool( GetHwnd(), IDC_CHK_BACKUP, m_bBackup );

	CDlgGrep::SetData();

	/* 編集中テキストから検索チェックを無効化 */
	::EnableWindow(GetItemHwnd(IDC_CHK_FROMTHISTEXT), FALSE);
}

/*! ダイアログデータの取得
	TRUE==正常  FALSE==入力エラー
*/
int CDlgGrepReplace::GetData( void )
{
	m_bPaste = IsDlgButtonCheckedBool( GetHwnd(), IDC_CHK_PASTE );

	/* 置換後 */
	ApiWrap::DlgItem_GetText(GetHwnd(), IDC_COMBO_TEXT2, m_strText2 );

	if( 0 == ::GetWindowTextLength( GetItemHwnd(IDC_COMBO_TEXT) ) ){
		WarningMessage(	GetHwnd(), LS(STR_DLGREPLC_REPSTR) );
		return FALSE;
	}

	m_bBackup = IsDlgButtonCheckedBool( GetHwnd(), IDC_CHK_BACKUP );
	m_pShareData->m_Common.m_sSearch.m_bGrepBackup = m_bBackup;

	if( !CDlgGrep::GetData() ){
		return FALSE;
	}

	if( m_strText2.size() < _MAX_PATH ){
		CSearchKeywordManager().AddToReplaceKeyArr( m_strText2.c_str() );
	}
	m_nReplaceKeySequence = GetDllShareData().m_Common.m_sSearch.m_nReplaceKeySequence;

	return TRUE;
}

LPVOID CDlgGrepReplace::GetHelpIdTable(void)
{
	return (LPVOID)p_helpids;
}
