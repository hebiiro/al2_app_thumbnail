#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはこのプログラムのバージョンを管理します。
	//
	inline struct version_t : version_base_t {
		//
		// コンストラクタです。
		//
		version_t() : version_base_t(L"アプリサムネイル表示", L"🐍アプリサムネイル表示🔖", L"r1") {}
	} version;
}
