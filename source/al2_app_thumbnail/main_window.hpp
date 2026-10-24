#pragma once

namespace apn::app_thumbnail
{
	//
	// このクラスはメインウィンドウです。
	//
	inline struct main_window_t : my::layout::Window<config_dialog_base_t>
	{
		//
		// このクラスはタイマーのIDです。
		//
		inline static constexpr struct timer_id_t {
			inline static constexpr UINT_PTR k_sync = 1000;
		} k_timer_id;

		//
		// このクラスはプレースホルダーの状態です。
		//
		struct state_t {
			//
			// プレースホルダーの表示状態です。
			//
			BOOL flag_is_visible = FALSE;

			//
			// プレースホルダーの表示位置です。
			//
			RECT rc = {};
		} state;

		//
		// dwmサムネイルのハンドルです。
		//
		HTHUMBNAIL thumbnail = nullptr;

		//
		// サムネイルウィンドウをプレースホルダー領域全体まで広げます。
		//
		void sync()
		{
//			MY_TRACE_FUNC("");

			// サムネイルウィンドウの表示状態を切り替える必要があるかどうかのフラグです。
			auto flag_need_show = FALSE;

			// サムネイルウィンドウをリサイズする必要があるかどうかのフラグです。
			auto flag_need_resize = FALSE;

			// プレースホルダーの表示状態を取得します。
			auto flag_is_visible = ::IsWindowVisible(*this);

			// プレースホルダーの表示状態が前回と異なる場合は
			if (state.flag_is_visible != flag_is_visible)
			{
				// サムネイルウィンドウの表示状態を切り替えるようにします。
				flag_need_show = TRUE;

				// プレースホルダーの表示状態を更新します。
				// 同時にリサイズする必要があるフラグもセットします。
				flag_need_resize = state.flag_is_visible = flag_is_visible;
			}

			// プレースホルダーが表示されている場合は
			if (state.flag_is_visible)
			{
				// プレースホルダーの位置を取得します。
				auto rc = my::get_window_rect(ctrl(idc_placeholder));

				// サムネイルウィンドウをリサイズする必要がある場合、
				// またはプレースホルダーの位置が前回と異なる場合は
				if (flag_need_resize || !::EqualRect(&state.rc, &rc))
				{
					// プレースホルダーの位置を更新します。
					state.rc = rc;

					// サムネイルウィンドウのオーナーを変更します。
//					::SetWindowLongPtrW(thumbnail_window,
//						GWLP_HWNDPARENT, (LONG_PTR)(HWND)*this);

					// サムネイルウィンドウの位置も更新します。
					my::set_window_pos(thumbnail_window, nullptr,
						&rc, SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);

					if (thumbnail)
					{
						// ソースサイズを取得します。
						auto src = SIZE {};
						auto hr = ::DwmQueryThumbnailSourceSize(thumbnail, &src);

						// ターゲットのサイズを取得します。
						auto dst = my::get_client_rect(thumbnail_window);
						auto dst_w = my::get_width(dst);
						auto dst_h = my::get_height(dst);

						// アスペクト比を維持するための比率です。
						auto s = std::min(dst_w / (double)src.cx, dst_h / (double)src.cy);

						// アスペクト比を維持しながらサイズを合わせます。
						auto w = (int)(src.cx * s);
						auto h = (int)(src.cy * s);
						auto x = dst.left + (dst_w - w) / 2;
						auto y = dst.top  + (dst_h - h) / 2;

						// サムネイルプロパティを更新します。
						DWM_THUMBNAIL_PROPERTIES dwm_tp {};
						dwm_tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
							DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
						dwm_tp.rcDestination = { x, y, x + w, y + h };
						dwm_tp.fVisible = TRUE;
						dwm_tp.opacity = 255;
						dwm_tp.fSourceClientAreaOnly = TRUE;
						hr = ::DwmUpdateThumbnailProperties(thumbnail, &dwm_tp);
						MY_TRACE_STR(my::get_error_message(hr));
					}
				}

				// サムネイルウィンドウも表示します。
				if (flag_need_show) ::ShowWindowAsync(thumbnail_window, SW_SHOWNOACTIVATE);
			}
			// プレースホルダーが表示されていない場合は
			else
			{
				// サムネイルウィンドウも非表示にします。
				if (flag_need_show)
				{
					::ShowWindowAsync(thumbnail_window, SW_HIDE);

					if (thumbnail)
					{
						// サムネイルプロパティを更新します。
						DWM_THUMBNAIL_PROPERTIES dwm_tp {};
						dwm_tp.dwFlags = DWM_TNP_VISIBLE;
						dwm_tp.fVisible = FALSE;
						auto hr = ::DwmUpdateThumbnailProperties(thumbnail, &dwm_tp);
						MY_TRACE_STR(my::get_error_message(hr));
					}
				}
			}
		}

		//
		// 指定されたウィンドウがalt+tabで表示されるウィンドウの場合はTRUEを返します。
		//
		inline static BOOL is_alt_tab_window(HWND hwnd)
		{
			MY_TRACE_HWND(hwnd);

			// ウィンドウハンドルが無効の場合はFALSEを返します。
			if (!hwnd || !::IsWindow(hwnd)) return FALSE;

			// ウィンドウが非表示の場合はFALSEを返します。
			if (!::IsWindowVisible(hwnd)) return FALSE;

			// 拡張ウィンドウスタイルを取得します。
			auto ex_style = my::get_ex_style(hwnd);
			auto flag_is_app_window = !!(ex_style & WS_EX_APPWINDOW);
			auto flag_is_tool_window = !!(ex_style & WS_EX_TOOLWINDOW);
			auto flag_is_no_activate = !!(ex_style & WS_EX_NOACTIVATE);

			// アプリウィンドウではない場合は
			if (!flag_is_app_window)
			{
				// ツールウィンドウの場合はFALSEを返します。
				if (flag_is_tool_window) return FALSE;

				// アクティブ化不可の場合はFALSEを返します。
				if (flag_is_no_activate) return FALSE;
			}

			// クロークされている場合はFALSEを返します。
			auto flag_is_cloaked = FALSE;
			::DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &flag_is_cloaked, sizeof(flag_is_cloaked));
			if (flag_is_cloaked) return FALSE;

			// オーナーが存在する場合はFALSEを返します。
			if (auto owner = ::GetWindow(hwnd, GW_OWNER))
				if (owner != ::GetDesktopWindow()) return FALSE;

			// ウィンドウ名が無効の場合はFALSEを返します。
			if (::GetWindowTextLengthW(hwnd) == 0) return FALSE;

			// それ以外の場合はTRUEを返します。
			return TRUE;
		}

		//
		// alt+tabで表示されるウィンドウのコレクションを返します。
		//
		inline static std::vector<HWND> enum_alt_tab_windows()
		{
			std::vector<HWND> windows;

			::EnumWindows([](HWND hwnd, LPARAM l_param)
			{
				auto* windows = (std::vector<HWND>*)l_param;

				if (is_alt_tab_window(hwnd)) windows->push_back(hwnd);

				return TRUE;
			}, (LPARAM)&windows);

			return windows;
		}

		//
		// アプリリストを更新します。
		//
		void refresh_app_list()
		{
			// コンボボックスを取得します。
			auto app_list = ctrl(idc_app_list);

			// 選択中のHWNDを取得しておきます。
			auto cur_sel = (int)::SendMessage(app_list, CB_GETCURSEL, 0, 0);
			auto cur_hwnd = (HWND)::SendMessage(app_list, CB_GETITEMDATA, cur_sel, 0);

			// 再描画を一時停止します。
			::SendMessage(app_list, WM_SETREDRAW, FALSE, 0);

			// コンボボックスをリセットします。
			::SendMessage(app_list, CB_RESETCONTENT, 0, 0);

			// 選択項目です。
			auto new_cur_sel = CB_ERR;

			// サムネイル候補のウィンドウを列挙します。
			for (auto hwnd : enum_alt_tab_windows())
			{
				// ウィンドウ名を取得します。
				auto title = my::get_window_text(hwnd);

				// コンボボックスに項目を追加します。
				auto index = (int)::SendMessageW(app_list, CB_ADDSTRING, 0, (LPARAM)title.c_str());

				// 追加された項目にウィンドウハンドルを関連付けます。
				::SendMessage(app_list, CB_SETITEMDATA, index, (LPARAM)hwnd);

				// 選択中のHWNDの場合はインデックスを取得しておきます。
				if (hwnd == cur_hwnd) new_cur_sel = index;
			}

			// 選択項目を復元します。
			if (new_cur_sel != CB_ERR)
				::SendMessage(app_list, CB_SETCURSEL, new_cur_sel, 0);

			// 再描画を再開します。
			::SendMessage(app_list, WM_SETREDRAW, TRUE, 0);
		}

		//
		// dwmサムネイルを登録します。
		//
		BOOL register_thumbnail()
		{
			// 既存のサムネイルが存在する場合は
			if (thumbnail)
			{
				// 登録を解除します。
				::DwmUnregisterThumbnail(thumbnail), thumbnail = nullptr;
			}

			// コンボボックスを取得します。
			auto app_list = ctrl(idc_app_list);

			// 選択中のHWNDを取得します。
			auto cur_sel = (int)::SendMessage(app_list, CB_GETCURSEL, 0, 0);
			auto cur_hwnd = (HWND)::SendMessage(app_list, CB_GETITEMDATA, cur_sel, 0);

			// 選択中のHWNDが無効の場合はFALSEを返します。
			if (!cur_hwnd || !::IsWindow(cur_hwnd)) return FALSE;

			// 選択中のHWNDのサムネイルを登録します。
			auto hr = ::DwmRegisterThumbnail(thumbnail_window, cur_hwnd, &thumbnail);
			if (!thumbnail)
			{
				hive.message_box(L"::DwmRegisterThumbnail()が失敗しました");

				return FALSE;
			}

			// サムネイルウィンドウの位置をリセットします。
			state.rc = {};

			return TRUE;
		}

		//
		// コンフィグでコントロールを更新します。
		//
		virtual BOOL on_to_ui() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コントロールでコンフィグを更新します。
		//
		virtual BOOL on_from_ui() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// WM_INITDIALOGを処理します。
		//
		virtual void on_init_dialog() override
		{
			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto fixed_size = base_size * 3 + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);
			auto s_control = std::make_shared<RelativePos>(fixed_size * 1);
			auto m_control = std::make_shared<RelativePos>(fixed_size * 2);
			auto l_control = std::make_shared<RelativePos>(fixed_size * 3);

			// アプリリストのレイアウトです。
			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(idc_app_list));
			}

			// プレースホルダーのレイアウトです。
			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(idc_placeholder));
			}
		}

		//
		// WM_COMMANDを処理します。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			switch (control_id)
			{
			case idc_app_list:
				{
					switch (code)
					{
					// ドロップダウンが開始された場合は
					case CBN_DROPDOWN:
						{
							refresh_app_list(); // アプリリストを更新します。

							break;
						}
					// 項目が選択された場合は
					case CBN_SELCHANGE:
						{
							register_thumbnail(); // サムネイルを登録します。

							break;
						}
					}

					break;
				}
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// 定期的にsync()を実行します。
					::SetTimer(hwnd, k_timer_id.k_sync, hive.sync_elapse, nullptr);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// タイマーを停止します。
					::KillTimer(hwnd, k_timer_id.k_sync);

					break;
				}
			case WM_TIMER:
				{
					switch (w_param)
					{
					case k_timer_id.k_sync:
						{
							// サムネイルウィンドウの位置をプレースホルダーとシンクロさせます。
							sync();

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} main_window;
}
