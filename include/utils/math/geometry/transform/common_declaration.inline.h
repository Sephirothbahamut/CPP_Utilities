utils_gpu_available constexpr auto& scale_self    (this utils::concepts::non_const auto& self, const float                      & scaling    ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto& scale_self    (this utils::concepts::non_const auto& self, const utils::math::vec2f         & scaling    ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto& rotate_self   (this utils::concepts::non_const auto& self, const angle::concepts::angle auto& rotation   ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto& translate_self(this utils::concepts::non_const auto& self, const vec2f                      & translation) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());