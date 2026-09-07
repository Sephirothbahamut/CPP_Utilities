#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <format>
#include <fstream>
#include <iostream>
#include <concepts>

#include "../memory.h"
#include "message.h"
#include "../containers/multithreading/self_consuming_queue.h"

#include "../oop/disable_move_copy.h"

namespace utils::logging
	{
	class logger
		{
		public:
			logger(const std::string& file_name = "log.txt");

			logger           (const logger&  copy)          = delete ; // don't copy threads please, thank you
			logger& operator=(const logger&  copy)          = delete ; // don't copy threads please, thank you
			logger           (      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy
			logger& operator=(      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy

#pragma region Push messages begin
		public:
			void operator<<(const message& message) noexcept;
			void operator()(const message& message) noexcept;

			template <typename ...Args>
			void emplace(Args&&... args) noexcept
				{
				message_queue.emplace(std::forward<Args>(args)...);
				}

			void push(const message& message) noexcept;

			void flush() noexcept;

			inline constexpr void raw(const string::concepts::stringlike auto& string) noexcept { push(message::raw(string, indents_count)); }
			inline constexpr void inf(const string::concepts::stringlike auto& string) noexcept { push(message::inf(string, indents_count)); }
			inline constexpr void log(const string::concepts::stringlike auto& string) noexcept { push(message::log(string, indents_count)); }
			inline constexpr void dgn(const string::concepts::stringlike auto& string) noexcept { push(message::dgn(string, indents_count)); }
			inline constexpr void err(const string::concepts::stringlike auto& string) noexcept { push(message::err(string, indents_count)); }
			inline constexpr void wrn(const string::concepts::stringlike auto& string) noexcept { push(message::wrn(string, indents_count)); }
			inline constexpr void suc(const string::concepts::stringlike auto& string) noexcept { push(message::suc(string, indents_count)); }

#pragma endregion Push messages end

#pragma region Indent management begin
		private:
			class section_marker;
			friend class section;
			class section_marker : utils::oop::non_copyable, utils::oop::non_movable
				{
				private:
					friend class logger;
					section_marker(logger& logger, const std::string& name);

					utils::observer_ptr<logger> logger_ptr{nullptr};
					std::string name;
					std::chrono::time_point<std::chrono::system_clock> time;

				public:
					~section_marker() noexcept;

					operator bool() const noexcept;
				};
		public:
			[[nodiscard]] section_marker section(const std::string& name) noexcept;

			size_t current_indents() const noexcept;

#pragma endregion Indent management end
		protected:
			std::ofstream file;
			size_t indents_count{0};

			utils::containers::multithreading::self_consuming_queue<message, utils::containers::multithreading::operation_flag_bits::pre> message_queue;
		};
	}

#ifdef utils_implementation
#include "logger.cpp"
#endif