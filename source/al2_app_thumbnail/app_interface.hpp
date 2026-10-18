#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t : app_base_t
	{
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }
	} *app = nullptr;
}
