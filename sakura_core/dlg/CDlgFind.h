/*!	@file
	@brief 検索ダイアログボックス

	@author Norio Nakatani
	@date	1998/12/02 再作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, YAZAKI
	Copyright (C) 2009, ryoji
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#ifndef SAKURA_CDLGFIND_AF260AA4_6075_4B87_9F03_2CEEDAD64094_H_
#define SAKURA_CDLGFIND_AF260AA4_6075_4B87_9F03_2CEEDAD64094_H_
#pragma once

#include "dlg/CDialog.h"
#include "recent/CRecentSearch.h"
#include "util/window.h"

/*-----------------------------------------------------------------------
クラスの宣言
-----------------------------------------------------------------------*/
class CDlgFind final : public CDialog
{
public:
	/*
	||  Constructors
	*/
	CDlgFind();
	/*
	||  Attributes & Operations
	*/
//	int DoModal( HINSTANCE, HWND, LPARAM );	/* モーダルダイアログの表示 */
	HWND DoModeless(HINSTANCE hInstance, HWND hwndParent, LPARAM lParam);	/* モードレスダイアログの表示 */

	void ChangeView(LPARAM pcEditView);

	void SetSearchText(const wchar_t* text);

	SSearchOption m_sSearchOption;	// 検索オプション
	int		m_bNOTIFYNOTFOUND;	// 検索／置換  見つからないときメッセージを表示
	std::wstring	m_strText;	// 検索文字列

	CLogicPoint	m_ptEscCaretPos_PHY;	// 検索開始時のカーソル位置退避エリア

	CRecentSearch			m_cRecentSearch;
	CFontAutoDeleter		m_cFontText;

protected:
//@@@ 2002.2.2 YAZAKI CShareDataに移動
//	void AddToSearchKeyArr( const char* );
	BOOL OnCbnDropDown( HWND hwndCtl, int wID ) override;
	int GetData( void ) override;		/* ダイアログデータの取得 */
	void GetSearchTextAndOption( void );/* 検索文字列/オプションの取得 */
	void SetCombosList( void );			/* 検索文字列/置換後文字列リストの設定 */
	void SetData( void ) override;		/* ダイアログデータの設定 */
	void UpdateMessageText();			/* メッセージ表示内容の更新 */
	BOOL OnInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam) override;
	BOOL OnDestroy() override;
	BOOL OnBnClicked(int wID) override;
	BOOL OnCbnSelChange( HWND hwndCtl, int wID ) override;
	BOOL OnCbnEditChange( HWND hwndCtl, int wID ) override;
	BOOL OnActivate( WPARAM wParam, LPARAM lParam ) override;	// 2009.11.29 ryoji

	// BOOL OnKeyDown( WPARAM wParam, LPARAM lParam ) override;
	LPVOID GetHelpIdTable(void) override;	//@@@ 2002.01.18 add
	size_t GetDocLength( const CEditDoc* pcDoc ) const;
	size_t GetMatchCount( const std::wstring& text, const SSearchOption& option, size_t nMaxCount = SIZE_MAX ) const;

	size_t m_nDocLength = 0;	// 検索対象文書の文字数(ダイアログボックス表示時に設定)
};
#endif /* SAKURA_CDLGFIND_AF260AA4_6075_4B87_9F03_2CEEDAD64094_H_ */
