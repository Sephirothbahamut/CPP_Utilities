utils_gpu_available constexpr auto& scale_self    (this auto& self, const float                      & scaling    ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto& rotate_self   (this auto& self, const angle::concepts::angle auto& rotation   ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto& translate_self(this auto& self, const vec2f                      & translation) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());

utils_gpu_available constexpr auto  scale         (this const auto& self, const float                    & scaling    ) noexcept;
utils_gpu_available constexpr auto  rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept;
utils_gpu_available constexpr auto  translate     (this const auto& self, const vec2f                    & translation) noexcept;
utils_gpu_available constexpr auto  transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept;
utils_gpu_available constexpr auto& transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
	requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());