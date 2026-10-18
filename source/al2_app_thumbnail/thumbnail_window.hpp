#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはサムネイルウィンドウです。
	//
	inline struct thumbnail_window_t : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// サムネイルウィンドウのクラス名です。
			constexpr auto k_class_name = L"app_thumbnail.thumbnail_window";

			// ウィンドウクラスを登録します。
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = k_class_name;
			::RegisterClass(&wc);

			// ウィンドウを作成します。
			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				nullptr,
				WS_POPUP,
				0, 0, 0, 0,
				nullptr, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} thumbnail_window;
}
