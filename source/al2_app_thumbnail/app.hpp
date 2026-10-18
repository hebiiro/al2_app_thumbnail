#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_base_t
	{
		//
		// プラグインの初期化処理を実行します。
		//
		virtual BOOL on_initialize_plugin(DWORD version) override
		{
			MY_TRACE_FUNC("");

			// 各種開始処理を実行します。
			debug.init();
			config_io.init();
			thumbnail_window.init();
			main_window.init(idd_main);

			// コンフィグをファイルから読み込みます。
			// ※書き込みはコンフィグダイアログの終了時に実行します。
			config_io.read();

			return TRUE;
		}

		//
		// プラグインの後始末処理を実行します。
		//
		virtual BOOL on_uninitialize_plugin() override
		{
			MY_TRACE_FUNC("");

			// 各種終了処理を実行します。
			main_window.exit();
			thumbnail_window.exit();
			config_io.exit();
			debug.exit();

			return TRUE;
		}

		//
		// プラグインを登録します。
		//
		virtual BOOL on_register_plugin(HOST_APP_TABLE* host) override
		{
			MY_TRACE_FUNC("");

			// メインウィンドウを登録します。
			host->register_window_client(version_base->get_name().c_str(), main_window);

			return TRUE;
		}
	} app_impl;
}
