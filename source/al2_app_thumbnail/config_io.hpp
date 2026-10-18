#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : config_io_base_t
	{
		//
		// 更新処理を実行します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return __super::update();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// その他の設定を読み込みます。
			read_bool(root, "sync_elapse", hive.sync_elapse);

			return __super::read_node(root);
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// その他の設定を書き込みます。
			write_bool(root, "sync_elapse", hive.sync_elapse);

			return __super::write_node(root);
		}
	} config_io;
}
