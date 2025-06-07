#pragma once

#include <set>
#include <optional>

#include "regions.h"
#include "../optional.h"
#include "../aggregate.h"

namespace utils::containers
	{
	template <typename AGGREGATE_T, typename AGGREGATE_OF_REGIONS_T, typename ACCESSORS_HELPER, bool observer = false>
	struct aggregate_regions
		{
		using aggregate_t            = AGGREGATE_T;
		using aggregate_of_regions_t = AGGREGATE_OF_REGIONS_T;
		using accessors_helper       = ACCESSORS_HELPER;
		using regions_of_aggregate_t = regions<aggregate_t>;

		using regions_per_field_t = std::conditional_t<observer, const aggregate_of_regions_t&, aggregate_of_regions_t>;
		regions_per_field_t regions_per_field;

		inline static constexpr const size_t elements_count{[]()
			{
			size_t ret{0};
			utils::aggregate::apply<accessors_helper>([&ret]() { ret++; });
			return ret;
			}()};

		aggregate_t at(size_t index) const noexcept
			{
			aggregate_t ret;

			aggregate::apply<accessors_helper>([index](const auto& field_regions, auto& field_return)
				{
				const auto value{field_regions.at_element_index(index).value()};
				field_return = value;
				}, regions_per_field, ret);

			return ret;
			}
 
		std::set<size_t> split_indices_set() const noexcept
			{
			std::set<size_t> ret;
			utils::aggregate::apply<accessors_helper>
				(
				[&](const auto& field_regions)
					{
					const auto field_split_indices{field_regions.split_indices()};
					std::copy(field_split_indices.begin(), field_split_indices.end(), std::inserter(ret, ret.end()));
					},
				regions_per_field
				);
			return ret;
			}
		std::vector<size_t> split_indices() const noexcept
			{
			const auto tmp{split_indices_set()};
			return std::vector<size_t>{tmp.begin(), tmp.end()};
			}

		regions_of_aggregate_t combine_regions() const noexcept
			{
			regions_of_aggregate_t ret;
			
			size_t element_index{0};
			while (element_index != std::numeric_limits<size_t>::max())
				{
				const size_t region_begin{element_index};
			
				const size_t region_end{[&]()
					{
					size_t ret{std::numeric_limits<size_t>::max()};
					utils::aggregate::apply<accessors_helper>
						(
						[&](const auto& field_regions)
							{
							ret = std::min(ret, field_regions.at_element_index(element_index).region_end());
							},
						regions_per_field
						);
					return ret;
					}()};
			
				const aggregate_t aggregate_for_this_region{[&]()
					{
					aggregate_t aggregate_for_this_region;
					utils::aggregate::apply<accessors_helper>//TODO early return
						(
						[&](const auto& input_field_regions, auto& output_field_value)
							{
							const auto& value{input_field_regions.at_element_index(element_index).value()};
							output_field_value = value;
							},
						regions_per_field, aggregate_for_this_region
						);
					return aggregate_for_this_region;
					}()};
				ret.add(aggregate_for_this_region, {region_begin, region_end - region_begin});
			
				element_index = region_end;
				}

			return ret;
			}
		};
	}
