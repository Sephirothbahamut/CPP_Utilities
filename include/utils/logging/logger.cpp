#include "logger.h"

#include "message.h"

namespace utils::logging
	{
	logger::logger(const std::filesystem::path& file_path) : 
		file {file_path},
		message_queue
			{
			[this](message& element) -> void
				{
				element.output<true>(std::cout);
				std::cout << std::flush;
				element.output<false>(file);
				file << std::flush;
				},
			[this](std::vector<message>& elements)
				{
				std::sort(elements.begin(), elements.end());
				}
			}
		{}

	//        logger::logger   (      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy
	//logger& logger::operator=(      logger&& move) noexcept = default; //could make those (running is already a flag for the instance being alive or not) but I'm lazy

#pragma region Push messages begin
	void logger::operator<<(const message& message) noexcept { push(message); }
	void logger::operator()(const message& message) noexcept { push(message); }

	void logger::push(const message& message) noexcept
		{
		message_queue.push(message);
		}

	void logger::flush() noexcept
		{
		message_queue.flush();
		}

#pragma endregion Push messages end

#pragma region Indent management begin
	logger::section_marker::section_marker(logger&logger, const std::string& name) : logger_ptr{&logger}, name{name}, time{std::chrono::system_clock::now()}
		{
		logger.push(message::section_enter(name, logger.indents_count));
		logger.indents_count++; 
		}

	logger::section_marker::~section_marker() noexcept
		{
		const auto now{std::chrono::system_clock::now()};
		const auto delta_time{now - time};

		logger_ptr->indents_count--;
		std::stringstream ss;
		ss << name << ", duration: " << std::format("{:%T}", delta_time);
		logger_ptr->push(message::section_leave(ss.str(), logger_ptr->indents_count));
		};

	logger::section_marker::operator bool() const noexcept { return true; }

	[[nodiscard]] logger::section_marker logger::section(const std::string& name) noexcept
		{
		return section_marker{*this, name};
		}

	size_t logger::current_indents() const noexcept { return indents_count; }

#pragma endregion Indent management end
	}