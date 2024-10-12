#pragma once

namespace utils
	{
	struct alignment
		{
		enum class horizontal{ left, centre, right  };
		enum class vertical  { top , middle, bottom };
		horizontal horizontal_alignment;
		vertical   vertical_alignment  ;

		struct create
			{
			create() = delete;

			inline static alignment centre() noexcept 
				{
				return
					{
					.horizontal_alignment{horizontal::centre},
					.vertical_alignment  {vertical  ::middle}
					};
				}
			};
		};
	}