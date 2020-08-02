#pragma once

#include "error_handler.h"

namespace ghassanpl::error_handling
{
	template<typename HANDLER, typename... ARGS >
	concept can_error_with = requires (HANDLER & handler)
	{
		handler.error(std::declval<ARGS>()...);
	};

	template <typename FIRST_HANDLER, typename... OTHER_HANDLERS>
	struct handler_finder
	{
		template <typename... ARGS>
		static constexpr size_t find_handler(size_t i = 0)
		{
			if constexpr (can_error_with<FIRST_HANDLER, ARGS...>)
				return i;
			else
				return handler_finder<OTHER_HANDLERS...>::template find_handler<ARGS...>(i + 1);
		}
	};

	template <typename... HANDLERS>
	struct multi_error_handler_handle
	{
		multi_error_handler_handle(HANDLERS&... handlers)
			: mHandlers(handlers...) {}
		multi_error_handler_handle()
			: mHandlers(mDefaultHandlers) {}

		template <typename... ARGS>
		requires (can_error_with<HANDLERS, ARGS...> || ...)
		void error(ARGS&&... args)
		{
			using finder = handler_finder<HANDLERS...>;
			static constexpr size_t handler_index = finder::template find_handler<ARGS...>();
			std::get<handler_index>(mHandlers).get().error(std::forward<ARGS>(args)...);
		}

	private:

		static inline std::tuple<HANDLERS...> mDefaultHandlers;
		std::tuple<std::reference_wrapper<HANDLERS>...> mHandlers;
	};

}
