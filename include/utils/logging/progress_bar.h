#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <format>
#include <fstream>
#include <iostream>
#include <concepts>

#include "message.h"
#include "../console/colour.h"
#include "../containers/multithreading/self_consuming_queue.h"

#include "../oop/disable_move_copy.h"

//TODO remove inlines and split into .cpp

namespace utils::logging
	{
	class partial_progress;

	class progress_bar : utils::oop::non_copyable, utils::oop::non_movable
		{
		public:
			progress_bar();
			progress_bar(float update_step, size_t bar_width);

			void advance(float new_state) noexcept;

			void complete() noexcept;

			partial_progress partial_progress(size_t steps_count) noexcept;

		private:
			float state{0.f};
			float last_drawn_state{0.f};
			const float update_step{.01f};
			const size_t bar_width{20};
			std::mutex mutex;

			utils::containers::multithreading::self_consuming_queue<float, utils::containers::multithreading::operation_flag_bits::none> message_queue
				{
				[this](const float& element) -> void
					{
					draw(element);
					}
				};

			void draw(const float& percent) const noexcept;
		};

	class partial_progress : utils::oop::non_copyable, utils::oop::non_movable
		{
		friend class progress_bar;
		public:

			void advance() noexcept;

			partial_progress split_partial_progress(size_t steps_count) noexcept;

		private:
			partial_progress(progress_bar& progress_bar, float from, float to, size_t steps_count) noexcept;

			std::mutex mutex;
			progress_bar& progress_bar;
			const float from{0.f};
			const float to  {1.f};
			const float step{0.f};
			size_t index{0};

			static float evaluate_step(float from, float to, size_t steps_count) noexcept;
		};
	}

#ifdef utils_implementation
#include "progress_bar.cpp"
#endif