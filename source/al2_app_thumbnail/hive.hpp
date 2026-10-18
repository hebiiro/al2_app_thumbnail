#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t : hive_base_t
	{
		//
		// サムネイルウィンドウとプレースホルダーをシンクロさせる間隔(ms)です。
		//
		int32_t sync_elapse = 500;
	} hive;
}
