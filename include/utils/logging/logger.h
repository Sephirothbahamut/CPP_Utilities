#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <format>
#include <fstream>
#include <iostream>
#include <concepts>

#include "message.h"
#include "../containers/multithreading/self_consuming_queue.h"

#include "../oop/disable_move_copy.h"

namespace utils::logging
	{
	template <typename T>
	class logger
		{
		public:
			using value_type = T;

			logger(const std::string& file_name = "log.txt") : file {file_name} {}

			logger           (const logger&  copy)          = delete ; // don't copy threads please, thank you
			logger& operator=(const logger&  copy)          = delete ; // don't copy threads please, thank you
			logger           (      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy
			logger& operator=(      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy

#pragma region Push messages begin
		public:
			void operator<<(const value_type& message) noexcept { push(message); }
			void operator()(const value_type& message) noexcept { push(message); }

			template <typename ...Args>
			void emplace(Args&&... args) noexcept
				{
				message_queue.emplace(std::forward<Args>(args)...);
				}

			void push(const value_type& message) noexcept
				{
				message_queue.push(message);
				}

			void flush() noexcept
				{
				message_queue.flush();
				}

			constexpr void inf(const string::concepts::stringlike auto& string) noexcept requires(concepts::message<value_type>) { push(value_type::inf(string, indents_count)); }
			constexpr void log(const string::concepts::stringlike auto& string) noexcept requires(concepts::message<value_type>) { push(value_type::log(string, indents_count)); }
			constexpr void dgn(const string::concepts::stringlike auto& string) noexcept requires(concepts::message<value_type>) { push(value_type::dgn(string, indents_count)); }
			constexpr void err(const string::concepts::stringlike auto& string) noexcept requires(concepts::message<value_type>) { push(value_type::err(string, indents_count)); }
			constexpr void wrn(const string::concepts::stringlike auto& string) noexcept requires(concepts::message<value_type>) { push(value_type::wrn(string, indents_count)); }
#pragma endregion Push messages end

#pragma region Indent management begin
		private:
			class section_marker;
			friend class section;
			class section_marker : utils::oop::non_copyable, utils::oop::non_movable
				{
				private:
					friend class logger<T>;
					section_marker(logger<T>&logger, const std::string& name)
						requires(concepts::message<value_type>) : logger_ptr{&logger}, name{name}, time{std::chrono::system_clock::now()}
						{
						logger.push(value_type::section_enter(name, logger.indents_count));
						logger.indents_count++; 
						}

					logger<T>* logger_ptr{nullptr};
					std::string name;
					std::chrono::time_point<std::chrono::system_clock> time;

				public:
					~section_marker() noexcept
						requires(concepts::message<value_type>)
						{
						const auto now{std::chrono::system_clock::now()};
						const auto delta_time{now - time};

						logger_ptr->indents_count--;
						std::stringstream ss;
						ss << name << ", duration: " << std::format("{:%T}", delta_time);
						logger_ptr->push(value_type::section_leave(ss.str(), logger_ptr->indents_count));
						};

					operator bool() const noexcept { return true; }
				};
		public:
			[[nodiscard]] section_marker section(const std::string& name) noexcept
				requires(concepts::message<value_type>)
				{
				return section_marker{*this, name};
				}

#pragma endregion Indent management end
		protected:
			std::ofstream file;
			size_t indents_count{0};

			utils::containers::multithreading::self_consuming_queue<T, utils::containers::multithreading::operation_flag_bits::pre> message_queue
				{
				[this](T& element) -> void
					{
					std::cout << element << std::endl;
					file << element << std::endl;
					},
				[this](std::vector<T>& elements)
					{
					if constexpr (concepts::message<T>)
						{
						std::sort(elements.begin(), elements.end());
						}
					}
				};
		};
	}