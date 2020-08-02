#pragma once

#include <tuple>

namespace ghassanpl::error_handling
{
	template <typename... ARGS>
	struct generic_value_exception : std::exception
	{
		generic_value_exception(ARGS... args)
			: error_values(std::move(args)...)
		{

		}
		std::tuple<ARGS...> error_values;
	};

	template <typename... ARGS>
	struct error_handler
	{
		virtual void error(ARGS... args)
		{
			throw generic_value_exception<ARGS...>{ std::move(args)... };
		}
	};

	template <typename T, typename... ARGS>
	struct throw_on_error : error_handler<ARGS...>
	{
		virtual void error(ARGS... args) override
		{
			throw T{ std::move(args)... };
		}
	};

	template <typename... ARGS>
	struct error_values : error_handler<ARGS...>
	{
		virtual ~error_values() = default;

		std::tuple<ARGS...> values;
		bool was_thrown = false;

		virtual void error(ARGS... args) override
		{
			values = { std::move(args)... };;
			was_thrown = true;
		}
	};

	template <unsigned I, typename... ARGS>
	auto& get(error_values<ARGS...>& ev) { return std::get<I>(ev.values); }
	template <unsigned I, typename... ARGS>
	auto const& get(error_values<ARGS...> const& ev) { return std::get<I>(ev.values); }

	template <typename... ARGS>
	struct error_handler_handle
	{
		using handler_type = error_handler<ARGS...>;

		template <typename T>
		requires std::is_base_of_v<handler_type, T>
			error_handler_handle(T& handler)
			: mHandler(handler) {}

		error_handler_handle(error_handler_handle&) = default;

		template <typename T>
		error_handler_handle(T&&) = delete;

		error_handler_handle()
			: mHandler(mDefaultHandler) {}

		template <typename... ERROR_ARGS>
		void error(ERROR_ARGS&&... args)
		{
			mHandler.error(std::forward<ERROR_ARGS>(args)...);
		}

	private:

		static inline throw_on_error<generic_value_exception<ARGS...>, ARGS...> mDefaultHandler;
		handler_type& mHandler;
	};

}
