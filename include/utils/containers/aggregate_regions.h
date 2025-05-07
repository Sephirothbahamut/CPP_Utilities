#pragma once

#include <set>
#include <optional>

#include "regions.h"
#include "../optional.h"
#include "../aggregate.h"

namespace utils::containers
	{
	template <typename AGGREGATE_T, typename AGGREGATE_OF_OPTIONALS_T, typename AGGREGATE_OF_REGIONS_T, typename ACCESSORS_HELPER, bool observer = false>
	struct aggregate_regions
		{
		using aggregate_t                         = AGGREGATE_T;
		using aggregate_of_optionals_t            = AGGREGATE_OF_OPTIONALS_T;
		using aggregate_of_regions_t              = AGGREGATE_OF_REGIONS_T;
		using accessors_helper                    = ACCESSORS_HELPER;
		using regions_of_aggregate_of_optionals_t = regions<aggregate_of_optionals_t>;
		using regions_of_aggregate_t              = regions<aggregate_t             >;

		using regions_per_field_t = std::conditional_t<observer, const aggregate_of_regions_t&, aggregate_of_regions_t>;
		regions_per_field_t regions_per_field;

		inline static constexpr const size_t elements_count{[]()
			{
			size_t ret{0};
			utils::aggregate::apply<accessors_helper>([&ret]() { ret++; });
			return ret;
			}()};

		aggregate_of_optionals_t at(size_t index) const noexcept
			{
			aggregate_of_optionals_t ret;

			aggregate::apply<accessors_helper>([index](const auto& field_regions, auto& field_return)
				{
				const auto value{utils::observer_ptr_to_optional(field_regions.at(index))};
				field_return = value;
				}, regions_per_field, ret);

			return ret;
			}

		aggregate_t at(size_t index, const aggregate_t& default_aggregate) const noexcept
			{
			aggregate_t ret;
			const aggregate_of_optionals_t opt{at(index)};

			aggregate::apply<accessors_helper>([](const auto& field_opt, const auto& field_default, auto& field_return)
				{
				field_return = field_opt.value_or(field_default);
				}, opt, default_aggregate, ret);

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

		regions_of_aggregate_of_optionals_t combine_regions() const noexcept
			{
			regions_of_aggregate_of_optionals_t ret;

			const size_t end{[&]()
				{
				size_t ret{0};
				utils::aggregate::apply<accessors_helper>
					(
					[&](const auto& field_regions)
						{
						ret = std::max(ret, field_regions.size());
						},
					regions_per_field
					);
				return ret;
				}()};
			
			size_t element_index{0};
			while (element_index != end)
				{
				const size_t region_begin{element_index};
			
				const size_t region_end{[&]()
					{
					size_t ret{std::numeric_limits<size_t>::max()};
					utils::aggregate::apply<accessors_helper>
						(
						[&](const auto& field_regions)
							{
							ret = std::min(ret, field_regions.region_at(element_index).end());
							},
						regions_per_field
						);
					return ret;
					}()};
			
				const bool fully_nullopt{[&]()
					{
					bool fully_nullopt{true};
					utils::aggregate::apply<accessors_helper>//TODO early return
						(
						[&](const auto& field_regions)
							{
							if (field_regions.at(element_index)) { fully_nullopt = false; }
							},
						regions_per_field
						);
					return fully_nullopt;
					}()};
			
				if (!fully_nullopt)
					{
					const aggregate_of_optionals_t aggregate_of_optionals{[&]()
						{
						aggregate_of_optionals_t aggregate_of_optionals;
						utils::aggregate::apply<accessors_helper>//TODO early return
							(
							[&](const auto& field_regions, auto& optional)
								{
								const auto* tmp{field_regions.at(element_index)};
								if (tmp) { optional = *tmp; }
								else { optional = std::nullopt; }
								},
							regions_per_field, aggregate_of_optionals
							);
						return aggregate_of_optionals;
						}()};
					
					ret.add(aggregate_of_optionals, {region_begin, region_end - region_begin});
					}
			
				element_index = region_end;
				}

			return ret;
			}
		};
	}
